#ifndef BLIT_H
#define BLIT_H
/*
 * rtl/blit/blit_engine.v 的 RGB565 二维绘图加速器驱动。
 * APB 基地址为 0xF8101000（io_apbSlave_0，paddr[12] = 1）；
 * AXI4 主接口连接 io_ddrMasters_0，读通道与 HDMI 扫描输出共享。
 *
 * 下方 BLIT_R_* 是相对基地址的字节偏移，使用 32 位 MMIO 访问：
 * RO = 只读，写入忽略；RW = 可读写，保存最近写入的有效位；
 * WO = 写入触发动作，读出为 0，不保存最后一次写入值。
 *
 * 地址以字节为单位，指向绘制区域左上角；行间隔也以字节为单位。
 * RGB565 源/目标地址需按 2 字节对齐，A8 每像素透明度数据占 1 字节。
 * 目标通常是当前帧缓冲，源通常是 Sprite 图片，二者行间隔分别设置。
 * 参数寄存器在命令执行后保持原值；blit_st 保存软件影子，只写变化项。
 * 写 CMD 时，硬件将当前参数和 CMD 标志复制进命令 FIFO；后续参数写入
 * 不会修改已入队的命令，提交命令也不等于等待绘图完成。
 *
 * 透明混合需要读取目标像素，会自动等此前的写入响应返回后再开始。
 * 源、目标矩形不能相互重叠；如果复制源由此前的命令写入，应设置
 * BLIT_SYNC 等待先前写入完成。这里的 SYNC 与 HDMI 垂直同步无关。
 *
 * CPU 数据缓存为写穿模式：CPU 写好的素材经 blit_fence() 后供硬件读取。
 * CPU 读取硬件绘制结果前，应先 blit_wait()，再 blit_dcache_invalidate()，
 * 避免仍从数据缓存中读到旧像素。
 *
 * 硬件命令列表（CAPS[24] = 1）：CPU 把命令编码成 32 位字写进 DDR 数组，
 * 写 LIST_END 后由引擎 DMA 读取、解码并压入同一个命令 FIFO。一批命令只需
 * 一次 APB 写；每个子弹（已定义的精灵槽）只需 1 个 DDR 字。
 * 驱动有三种提交方式（blit_set_mode）：
 *   BLIT_MODE_APB  ：原方式，每条命令写参数寄存器 + CMD。
 *   BLIT_MODE_LIST ：全部命令写入 DDR 列表（blit_fill/blit_copy 自动转为记录），
 *                    攒够 BLIT_LIST_KICK 个字或 blit_wait()/gfx_end() 时写 LIST_END。
 *   BLIT_MODE_WORD ：普通命令仍走 APB 寄存器；批量精灵（hw_copy_list）每个
 *                    只写一次 LIST_WORD（APB），用于和 DDR 列表对比两种写入的代价。
 */
#include <stdint.h>

#ifndef BLIT_BASE
#define BLIT_BASE 0xF8101000u
#endif

#define BLIT_ID_VALUE    0x424C5431u  /* ID 固定值，字符标识 "BLT1"。 */

/* ID（RO）：[31:0] = 0x424C5431，由 RTL 固定返回，用于检测引擎。 */
#define BLIT_R_ID        0x00u

/* STATUS（RO）：硬件实时状态，不由软件直接写入。
 * [0]     BUSY：队列、内部处理、命令列表或 DDR 事务尚未全部结束时为 1。
 * [1]     FULL：命令 FIFO 的 RAM 部分已满时为 1。
 * [2]     ERR：AXI 读/写响应错误后保持为 1，由 CTRL[1] 或复位清除。
 * [3]     LIST：命令列表仍在执行（LIST_ADDR ≠ LIST_END、取数未完成或解码器
 *         停在某条记录中间）时为 1；此时写 LIST_ADDR 或 CMD 会被 PREADY 挂起。
 * [4]     LERR：列表中出现保留操作码的字（已跳过）后保持为 1，由 CTRL[1] 清除。
 * [15:5]  保留，读出为 0。
 * [31:16] 命令 FIFO 的空位数，零扩展；当前 CQ_AW=4（寄存器实现），范围 0~16。
 * 空位数不包含 FIFO 输出预取寄存器，也不表示整个流水线的剩余命令数。
 */
#define BLIT_R_STATUS    0x04u

/* CTRL（WO）：写入时触发清除动作，读出为 0。
 * [0]    写 1 清除 PERF_BUSY/CMDS/RB/WB，不清除 CMD_PUSHED。
 * [1]    写 1 清除 STATUS.ERR 和 STATUS.LERR；同周期发生新错误时，新错误优先。
 * [31:2] 忽略。写 0 不触发对应动作，写 3 同时清性能计数和错误。
 * 此寄存器不复位引擎、不清空命令 FIFO，也不是绘图使能开关。
 */
#define BLIT_R_CTRL      0x08u

/* CAPS（RO）：由 RTL 参数决定，软件不能修改。
 * [7:0]   CQ_AW：命令 FIFO 的 RAM 深度为 2^CQ_AW 条命令。
 * [15:8]  WF_AW：写数据 FIFO 的 RAM 深度为 2^WF_AW 个 32 位字。
 * [23:16] RING_AW：每路行数据环形缓冲深度为 2^RING_AW 个 32 位字。
 * [24]    LIST：1 = 支持硬件命令列表（LIST_ADDR/LIST_END/LIST_WORD）。
 * [31:25] 保留，读出为 0。当前值为 0x010A0908（列表、10、9、8）。
 */
#define BLIT_R_CAPS      0x0Cu

/* DST（RW）：[31:0] 目标区域左上角像素的字节地址，硬件内部为 r_dst。
 * 硬件执行时递增内部工作地址，不会自动修改本寄存器；复位值为 0。
 */
#define BLIT_R_DST       0x10u

/* DSTR（RW）：[15:0] 目标相邻两行起点的字节距离，内部为 r_dstr。
 * [31:16] 写入忽略，读出为 0。它由目标内存布局决定，不是矩形宽度。
 * 640 像素宽的 RGB565 帧缓冲为 1280 字节，与透明度和矩形大小无关。
 * 硬件复位值为 1280；blit_init() 写成 0，绘图时再写实际行间隔。
 */
#define BLIT_R_DSTR      0x14u

/* SRC（RW）：[31:0] 源区域左上角像素的字节地址，内部为 r_src。
 * COPY 从该地址读取 RGB565 像素；通常指向 Sprite 素材，复位值为 0。
 */
#define BLIT_R_SRC       0x18u

/* SSTR（RW）：[15:0] 源相邻两行起点的字节距离，内部为 r_sstr。
 * [31:16] 写入忽略，读出为 0；复位值为 0。
 * 紧密排列的 RGB565 Sprite 通常为图片完整宽度乘 2，裁剪不改变行间隔。
 */
#define BLIT_R_SSTR      0x1Cu

/* AADDR（RW）：[31:0] 源区域左上角对应的 A8 透明度字节地址，内部为 r_aad。
 * 每像素占 1 字节；仅 COPY 且 CMD[2]=1 时使用，复位值为 0。
 */
#define BLIT_R_AADDR     0x20u

/* ASTR（RW）：[15:0] A8 数据相邻两行起点的字节距离，内部为 r_astr。
 * [31:16] 写入忽略，读出为 0；复位值为 0。
 * 紧密排列的 A8 数据通常为源图片完整宽度乘 1。
 */
#define BLIT_R_ASTR      0x24u

/* SIZE（RW）：设置本次绘制区域尺寸，单位为像素。
 * [10:0]  宽度，内部为 r_w；[26:16] 高度，内部为 r_h。
 * [15:11]、[31:27] 写入忽略，读出为 0；宽、高复位值均为 0。
 * 软件打包为 (h << 16) | w；宽或高为 0 的命令不绘制像素。
 */
#define BLIT_R_SIZE      0x28u

/* COLOR（RW）：纯色填充的 RGB565 颜色，内部为 r_color。
 * [15:11] 红色 R5；[10:5] 绿色 G6；[4:0] 蓝色 B5。
 * [31:16] 写入忽略，读出为 0；复位值为 0（黑色）。
 */
#define BLIT_R_COLOR     0x2Cu

/* KEY（RW）：颜色键，内部为 r_key；COPY 且 CMD[1]=1 时使用。
 * [15:11] 红色 R5；[10:5] 绿色 G6；[4:0] 蓝色 B5。
 * [31:16] 写入忽略，读出为 0；复位值为 0xF81F（品红色）。
 * 源 RGB565 像素等于 KEY 时，跳过该像素的目标写入。
 */
#define BLIT_R_KEY       0x30u

/* CMD（WO）：每次被接受的写入都提交一条参数快照，读出为 0。
 * [0]     OP：0=纯色填充 FILL，1=图片复制 COPY。
 * [1]     KEY_EN：1=启用颜色键，仅 COPY 有效。
 * [2]     APLANE：1=读取每像素 A8 透明度，仅 COPY 有效。
 * [3]     SYNC：等此前写入全部获得响应后再开始，与 HDMI 垂直同步无关。
 * [4]     CACHE：源图经引擎内 16 KB 源缓存读取（CAPS[27]，仅 COPY 有效）。
 * [5]     INV：开始本命令前清空源缓存（尺寸为 0 的命令也生效，可当作单独的 INV）。
 * [7:6]   忽略。
 * [15:8]  全局透明度，0=透明，255=不透明；启用 A8 时两者共同参与混合。
 * [31:16] 忽略。
 * 提交时一并保存 DST/DSTR/SRC/SSTR/AADDR/ASTR/SIZE/COLOR/KEY。
 * 命令 RAM 满时，硬件拉低 APB PREADY，等待空位后才接受写入。
 */
#define BLIT_R_CMD       0x34u

/* PERF_BUSY（RO）：[31:0] BUSY=1 的硬件时钟周期累计数。
 * 单位是引擎时钟周期，不是微秒或 CPU 占用率；CTRL[0] 或复位清零。
 */
#define BLIT_R_PERF_BUSY 0x40u

/* PERF_CMDS（RO）：[31:0] w_cmd_done 事件累计数，CTRL[0] 或复位清零。
 * 事件发生在安排命令最后一行的最后一笔写地址事务时；DDR 写响应此时
 * 可能尚未返回，不能据此判断整批绘图结束，应检查 STATUS.BUSY。
 */
#define BLIT_R_PERF_CMDS 0x44u

/* PERF_RB（RO）：[31:0] 接收的 AXI 32 位读数据拍累计数。
 * 按拍计数，不是像素数；每拍数据宽度为 4 字节，CTRL[0] 或复位清零。
 */
#define BLIT_R_PERF_RB   0x48u

/* PERF_WB（RO）：[31:0] AXI 写数据握手成功的 32 位数据拍累计数。
 * 每拍数据宽度为 4 字节，有效字节由 WSTRB 决定；CTRL[0] 或复位清零。
 */
#define BLIT_R_PERF_WB   0x4Cu

/* PUSHED（RO）：[31:0] 从硬件复位起成功入队的命令累计数。
 * 每次 cq_push 加 1；仅硬件复位清零，CTRL[0] 和 blit_init() 不清除此值。
 * 上述五个 32 位计数器溢出时按无符号 32 位自然回绕。
 */
#define BLIT_R_PUSHED    0x50u

/* LIST_ADDR（RW）：写入 = 从该字节地址（4 字节对齐）开始一个新列表，
 * 同时令 LIST_END = 该地址（列表为空）。上一个列表未执行完时，写入被
 * PREADY 挂起直到 STATUS.LIST = 0。读出 = 下一个要取的字地址。
 * 列表上下文（DST…BASE）与精灵槽不因写 LIST_ADDR 而改变。
 */
#define BLIT_R_LADDR     0x38u

/* LIST_END（RW）：[31:2] 列表中最后一个有效字之后的字节地址，可在引擎执行
 * 期间随时向后推进（流式提交）。只能推进到“记录边界”，否则解码器会停在
 * 半条记录中，并挂起之后的 APB CMD。读出 = 当前结束地址。
 */
#define BLIT_R_LEND      0x3Cu

/* LIST_WORD（WO）：经 APB 直接送入一个列表字，与 DDR 列表使用同一个解码器；
 * 解码器或命令 FIFO 忙时 PREADY 挂起。DDR 列表执行期间不要使用。
 */
#define BLIT_R_LWORD     0x58u

/* 源缓存（CAPS[27]）：16 KB 直接映射，512 行 × 32 B，tag = 地址[31:14]。
 * 带 CMD[4] 的 COPY 按行查表：命中直接从片上 RAM 取数（不发 AXI 读），缺失读整行（8 拍 burst）。
 * 缓存不感知任何写（CPU 或引擎自己的），所以只能给"画之前就写好、之后不再变"的源图用，
 * CPU 改写源图后必须先 INV（CMD[5]）。驱动在每帧开头和每次 publish（新字模图集）时自动 INV。
 * PERF_CHIT / PERF_CMISS：命中 / 缺失的行请求数（一行源像素跨两条缓存行时算两次），CTRL[0] 清零。 */
#define BLIT_R_PERF_CHIT  0x5Cu
#define BLIT_R_PERF_CMISS 0x60u

/* 停顿计数器（CAPS[29]），都按引擎时钟计数，CTRL[0] 清零（RO，0x64~0x80 连续 8 个）。
 * 引擎 BUSY 的每个时钟恰好落在下面一类（RTL 仿真里逐拍校验过）：
 *   PERF_BUSY = PERF_WB（写出一个字）+ PXWAIT + PXFULL + PXBUB + PXIDLE
 *              （CAPS[30] 位流再加 PERF_WCLK：宽通路占用的时钟，见 BLIT_R_WIDE）
 *   （2026-10-07 起 PIXEL 提前一行准备行参数、命令间不再排空，装命令不再单独占拍，
 *    PERF_CMDS 只是命令数；更早的位流是 PERF_BUSY = PERF_WB + PERF_CMDS + 这 4 项。）
 *   PXWAIT 行数据还没从 DDR 读回来；PXFULL 写侧满（W FIFO / 写头 FIFO）；
 *   PXBUB  固定开销：取命令头 / 准备行 / 起行而当拍没出字；PXIDLE 没有就绪的命令头。
 * 另外 4 个看 AXI 端口（可以与上面重叠，单独看占 BUSY 的比例）：
 *   WSTALL W 数据有效但 DDR 不收；AWWAIT 数据已在 W FIFO 但 AW 还没发；
 *   AWSTALL AW 有效但 DDR 不收；ARSTALL AR 有效但读口不收（HDMI 在读 / DDR 忙）。 */
#define BLIT_R_PERF_WSTALL  0x64u
#define BLIT_R_PERF_AWWAIT  0x68u
#define BLIT_R_PERF_AWSTALL 0x6Cu
#define BLIT_R_PERF_ARSTALL 0x70u
#define BLIT_R_PERF_PXWAIT  0x74u
#define BLIT_R_PERF_PXFULL  0x78u
#define BLIT_R_PERF_PXBUB   0x7Cu
#define BLIT_R_PERF_PXIDLE  0x80u
enum { BLIT_ST_WSTALL, BLIT_ST_AWWAIT, BLIT_ST_AWSTALL, BLIT_ST_ARSTALL,
       BLIT_ST_PXWAIT, BLIT_ST_PXFULL, BLIT_ST_PXBUB, BLIT_ST_PXIDLE, BLIT_ST_N };

/* 128 位宽通路（C2，CAPS[30]）：不透明 COPY（无色键、全局透明度 FF、
 * dst/src 与 dstr/sstr 模 16 字节同相位）和不透明 FILL，宽度 >= MINW 时
 * 硬件自动走 128 位口，每拍 8 像素（窄通路每拍 1 个 32 位字 = 2 像素）。
 * 软件不用改命令，只是满足条件的命令自己变快。这样的命令要等窄流水线排空才开始，
 * 借用行环形缓冲 + 源缓存数据 RAM 当 1024 × 128 位缓冲（会作废源缓存第 0..127 行）。
 * 0x84 WIDE（RW）：[0] OFF：1 = 不用宽通路（全部走窄通路，同旧位流）
 *                 [7:4] WTH：COPY 攒够 WTH × 64 拍再成批写（0 = 有数据就写）；
 *                       读写成批分开，DDR 读写换向少，但每批开头要等读回
 *                 [15:8] MINW/8：宽度 >= MINW 像素才走宽通路（复位 16 = 128 像素）
 *                 [26:16] 宽通路在途读字数上限（32 位字计，1..1024，复位 256），
 *                        与 LIMIT[26:16] 分开：它决定宽 COPY 在 CPU 前面排多深
 * 计数器（RO，CTRL[0] 清零）：PERF_WCLK 宽通路占用的时钟（此时 PERF_WB/PX* 不计，
 * 所以 PERF_BUSY = PERF_WB + PXWAIT + PXFULL + PXBUB + PXIDLE + PERF_WCLK），
 * PERF_XWBEAT / PERF_XRBEAT 宽通路的 128 位写 / 读拍数（1 拍 = 8 像素）。 */
#define BLIT_R_WIDE         0x84u
#define BLIT_R_PERF_WCLK    0x88u
#define BLIT_R_PERF_XWBEAT  0x8Cu
#define BLIT_R_PERF_XRBEAT  0x90u
#define BLIT_WIDE_OFF       1u
#define BLIT_WIDE_RESET     ((256u << 16) | (16u << 8))
#define BLIT_WIDE(minw, wth, words) ((((uint32_t)(words) & 0x7FFu) << 16) | \
                                     ((((uint32_t)(minw) >> 3) & 0xFFu) << 8) | \
                                     (((uint32_t)(wth) & 15u) << 4))

/* 分带合成（D，CAPS[31]）：一帧的命令夹在列表记录 BANDS 和 ENDB 之间，引擎不再逐条直接写 DDR，
 * 而是把这一帧按 16 行一带，在片上"带缓冲"（16 行 × 1280 字节，24 块 BRAM）里合成好再整带写回：
 *   第 1 阶段（分桶）：BANDS..ENDB 之间的每条命令（REC / DRAW / TAB 的每一项）由引擎算出它落在
 *                     哪几带，把解析好的命令（8 个字）追加到 DDR 里每带一个的"桶"（BAND_BIN）；
 *                     目标不在这一帧里的命令（画别的缓冲）照旧立即执行。
 *   第 2 阶段（逐带）：读回这一带的桶，命令裁剪到这 16 行后画进带缓冲——
 *                     走源缓存的 COPY（不透明 / 色键）和不透明 FILL 每拍 8 像素（K 通路）；
 *                     同相位的大块不透明 COPY 读回的 128 位拍直接写进带缓冲；
 *                     透明混合从带缓冲读目标像素（不再在 DDR 上读-改-写）；
 *                     这一带画完用 16 拍 burst 整带写回 DDR，同时把带缓冲清零。
 * 所以：帧里没画到的像素是黑色（不是帧缓冲里的旧内容）；帧内命令的行距必须等于帧行距；
 * 命令之间的先后次序与直接画完全相同。一帧结束（ENDB）后引擎才开始第 2 阶段，
 * 画完 FRAMES 加 1：CPU 可以一边录下一帧一边等上一帧（见 blit_band_done）。
 * 0x94 BAND_GEO（RW）：[15:0] 帧行距（字节，16 的倍数，<= 1280），[21:16] 带数（1..32，帧高 = 带数 × 16 行），
 *                     [31] KOFF：1 = 不用 K 通路（全部走 32 位窄通路，A/B 对比用）
 * 0x98 BAND_RECIP（RW）：floor(2^24 / (行距 / 16)) + 1（引擎用它把地址换成行号，行距 256..1280 时精确）
 * 0x9C BAND_BIN（RW）：[31:12] 桶区基地址，[4:0] log2(每带桶的字节数)（12..20）；
 *                     桶区占 带数 << 该值 字节，每条命令 32 字节，溢出时丢命令并置 STATUS[6]
 * 0xA0 FRAMES（RO）：复位以来画完（最后一带的写响应都回来）的分带帧数
 * 0xA4.. 计数器（RO，CTRL[0] 清零）：PERF_P1 第 1 阶段时钟数，PERF_FLUSH 整带写回 + 清零的时钟数，
 *        PERF_KBEAT K 通路写进带缓冲的拍数（1 拍 8 像素），PERF_RECS 追加到桶里的命令数，
 *        PERF_BAND 第 2 阶段时钟数（含写回），PERF_KCLK K 通路忙的时钟数。
 *        分带帧期间 0x74~0x88 的 PIXEL / 宽通路时钟分类不计数。
 * STATUS [5] 分带帧进行中  [6] 某带的桶满了（丢了命令）  [7] 帧内命令行距不等于帧行距（已丢弃）；
 *        [6][7] 由 CTRL[1] 清除。 */
#define BLIT_R_BAND_GEO     0x94u
#define BLIT_R_BAND_RECIP   0x98u
#define BLIT_R_BAND_BIN     0x9Cu
#define BLIT_R_FRAMES       0xA0u
#define BLIT_R_PERF_P1      0xA4u
#define BLIT_R_PERF_FLUSH   0xA8u
#define BLIT_R_PERF_KBEAT   0xACu
#define BLIT_R_PERF_RECS    0xB0u
#define BLIT_R_PERF_BAND    0xB4u
#define BLIT_R_PERF_KCLK    0xB8u
#define BLIT_BAND_ROWS      16u         /* 每带行数（硬件固定） */
#define BLIT_BAND_MAX_PITCH 1280u       /* 带缓冲一行最多这么多字节（旧位流；CAPS2[2] 时 2560） */
#define BLIT_BAND_MAX_BANDS 32u         /* 旧位流；新位流看 CAPS2[15:8]（45 = 720 行） */
#define BLIT_BAND_KOFF      (1u << 31)
/* 桶区：默认放在 0x03000000（程序 / 帧缓冲 / 图片之上），每带 256 KB = 8192 条命令 */
#ifndef BLIT_BAND_BIN_BASE
#define BLIT_BAND_BIN_BASE  0x03000000u
#endif
#ifndef BLIT_BAND_BIN_SHIFT
#define BLIT_BAND_BIN_SHIFT 18u
#endif
/* 开一帧分带之前列表缓冲里至少要剩这么多字，不够就先回到开头（否则帧中途回绕会把这一帧拆开） */
/* 2026-10-11（200 MHz 实测）：原来是缓冲的一半，可用空间只有一半，流水时每秒回绕 10 次左右，
 * 每次要等引擎把整个列表画完（qstall 约 2.5 ms/次）。一帧最多约 6000 字，留 8192 就够。 */
#ifndef BLIT_BAND_ROOM
#define BLIT_BAND_ROOM      (BLIT_LIST_WORDS / 2u < 8192u ? BLIT_LIST_WORDS / 2u : 8192u)
#endif

/* 引擎计数器快照（blit_perf_snap）：两次快照相减（blit_perf_diff）就是这段时间的量。
 * 没有 CAPS[29] 时 st[] 恒为 0；没有 CAPS[27] 时 chit/cmiss 恒为 0；
 * 没有 CAPS[30] 时 wclk/xwbeat/xrbeat 恒为 0；没有 CAPS[31] 时 band[] 恒为 0。 */
enum { BLIT_BP_P1, BLIT_BP_FLUSH, BLIT_BP_KBEAT, BLIT_BP_RECS, BLIT_BP_BAND, BLIT_BP_KCLK, BLIT_BP_N };
typedef struct {
    uint32_t busy, cmds, rbeat, wbeat, chit, cmiss;
    uint32_t st[BLIT_ST_N];
    uint32_t wclk, xwbeat, xrbeat;
    uint32_t band[BLIT_BP_N];    /* 0xA4..0xB8：P1 FLUSH KBEAT RECS BAND KCLK */
} BlitPerf;

/* 列表字格式。列表有自己的一套上下文寄存器（与 APB 的 0x10~0x30 互相独立）：
 *   位号 0 DST  1 DSTR  2 SRC  3 SSTR  4 AADDR  5 ASTR  6 SIZE  7 COLOR  8 KEY  9 BASE
 * REC  ：[31:28]=0，[25] INV [24] CACHE（同 CMD[5:4]），[23:16] 全局透明度，[15:12] 标志（同 CMD[3:0]），[10] 只设置
 *        不提交，[9:0] 字段掩码；其后按位号从小到大每个置位字段跟 1 个字，
 *        然后（[10]=0 时）用整套上下文提交一条命令。
 * SLOT ：[31:28]=1，[24] CACHE，[23:16] 透明度，[15:12] 标志，[5:0] 槽号；
 *        槽 <- {上下文 SRC、SSTR、SIZE，标志，透明度}。
 * DRAW ：[31]=1，[30:25] 槽号，[24:0] 字节偏移；把槽中的精灵画到 BASE + 偏移，
 *        DSTR/KEY/COLOR/AADDR/ASTR 取自上下文，且不改变上下文。
 * TAB  ：[31:28]=2，[15:0] 项数 n（CAPS[28]）；其后 1 个字 = 精灵表的字节地址（4 字节对齐）。
 *        引擎自己去 DDR 读这张表，每项 8 B，正好是 GfxItem 的内存布局：
 *          字0 = {y[31:16], x[15:0]}（引擎只用 x[11:0]、y[27:16]，无符号）
 *          字1 = 精灵号，[5:0] 当槽号
 *        每项等于一个 DRAW(槽, y*DSTR + 2x)。表读完后回到主列表继续。
 *        引擎读完之前（blit_wait 之前）CPU 不能改这张表。n = 0 时只跳过地址字。
 * BANDS：[31:28]=3（CAPS[31]）；其后 1 个字 = 这一帧的帧缓冲基地址（256 字节对齐）。之后直到 ENDB 的
 *        记录属于这一帧，由引擎分带合成（见 BLIT_R_BAND_GEO 处的说明）。不能嵌套。
 * ENDB ：[31:28]=4；这一帧录完，引擎开始逐带画并写回 DDR，画完 FRAMES 加 1，再接着执行后面的列表。
 *        BANDS 发出后必须有 ENDB，否则引擎一直等（STATUS.BUSY 不会清）。
 * 其他操作码：跳过并置 STATUS.LERR。
 */
#define BLIT_LF_DST      (1u << 0)
#define BLIT_LF_DSTR     (1u << 1)
#define BLIT_LF_SRC      (1u << 2)
#define BLIT_LF_SSTR     (1u << 3)
#define BLIT_LF_A        (1u << 4)
#define BLIT_LF_ASTR     (1u << 5)
#define BLIT_LF_SIZE     (1u << 6)
#define BLIT_LF_COLOR    (1u << 7)
#define BLIT_LF_KEY      (1u << 8)
#define BLIT_LF_BASE     (1u << 9)
#define BLIT_L_NOPUSH    (1u << 10)
/* cmd = CMD 寄存器格式（标志 [3:0]，CACHE/INV [5:4]，透明度 [15:8]） -> 列表字中的 {INV, CACHE, 透明度, 标志} */
#define BLIT_L_CMD(cmd)        ((((uint32_t)(cmd) & 0x70u) << 20) | (((uint32_t)(cmd) & 0xFF00u) << 8) | \
                                (((uint32_t)(cmd) & 0xFu) << 12))
#define BLIT_L_REC(mask, cmd)  (BLIT_L_CMD(cmd) | (uint32_t)(mask))
#define BLIT_L_SLOT(slot, cmd) (0x10000000u | BLIT_L_CMD(cmd) | ((uint32_t)(slot) & 63u))
#define BLIT_L_DRAW(slot, off) (0x80000000u | (((uint32_t)(slot) & 63u) << 25) | ((uint32_t)(off) & 0x1FFFFFFu))
#define BLIT_L_TAB(n)          (0x20000000u | ((uint32_t)(n) & 0xFFFFu))   /* 后面跟表地址字 */
#define BLIT_L_BANDS           0x30000000u   /* 后面跟帧缓冲基地址字 */
#define BLIT_L_ENDB            0x40000000u
#define BLIT_L_TAB_MAX   0xFFFFu      /* 一条 TAB 最多的项数 */
#define BLIT_L_TAB_MAX_XY 4095        /* 表项 x、y 只用 12 位 */
#define BLIT_L_SLOTS     64u
#define BLIT_L_MAX_OFF   0x1FFFFFFu   /* DRAW 偏移上限（32 MB） */

#define BLIT_CAPS_LIST   (1u << 24)  /* CAPS[24]：支持命令列表。 */
#define BLIT_CAPS_LIMIT  (1u << 25)  /* CAPS[25]：有 LIMIT 寄存器。 */
#define BLIT_CAPS_LIMIT_WORDS (1u << 26)  /* CAPS[26]：LIMIT 有 [26:16] 在途读字数上限。 */
#define BLIT_CAPS_CACHE  (1u << 27)  /* CAPS[27]：有 16 KB 源缓存（CMD[4] CACHE / CMD[5] INV）。 */
#define BLIT_CAPS_TABLE  (1u << 28)  /* CAPS[28]：列表支持 TAB 记录（引擎自己读精灵表）。 */
#define BLIT_CAPS_STALL  (1u << 29)  /* CAPS[29]：有 0x64~0x80 停顿计数器。 */
#define BLIT_CAPS_WIDE   (1u << 30)  /* CAPS[30]：有 128 位宽通路（0x84 WIDE，0x88~0x90 计数器）。 */
#define BLIT_CAPS_BAND   (1u << 31)  /* CAPS[31]：有分带合成（BANDS / ENDB 记录，0x94~0xB8）。 */
/* 0x54 LIMIT：[4:0] 引擎最多同时挂起的读 burst 数（1..16，复位 16），
 *            [15:8] 最多同时挂起的写 burst 数（1..128，复位 128），
 *            [26:16] 在途读字数上限（1..1024，复位 1024）：已发出但数据还没回来的读字数
 *                    少于该值时才发新的读 burst。它限制的是字节而不是 burst 个数：
 *                    整屏拷贝（64 字 burst）只能在 CPU 前面排几百字节，
 *                    小精灵（每行 2..6 字）仍可同时挂 16 个 burst，速度不受影响。
 * 引擎已经排进 DDR 控制器的 burst 会先于之后到达的 CPU 访存（D$ 缺失重填、store）被服务，
 * 排得越深引擎越快，但 CPU 每次访问 DDR 等得越久；调小 = 引擎让路给 CPU。 */
#define BLIT_R_LIMIT     0x54u
#define BLIT_LIMIT_RD_MAX 16u
#define BLIT_LIMIT_WR_MAX 128u
#define BLIT_LIMIT_RW_MAX 1024u

#define BLIT_ST_BUSY     (1u << 0)  /* STATUS[0]：引擎及相关 DDR 事务仍忙。 */
#define BLIT_ST_FULL     (1u << 1)  /* STATUS[1]：命令 FIFO 的 RAM 已满。 */
#define BLIT_ST_ERR      (1u << 2)  /* STATUS[2]：保持的 AXI 响应错误标志。 */
#define BLIT_ST_LIST     (1u << 3)  /* STATUS[3]：命令列表仍在执行。 */
#define BLIT_ST_LERR     (1u << 4)  /* STATUS[4]：列表中出现过无效字。 */
#define BLIT_ST_BAND     (1u << 5)  /* STATUS[5]：分带帧进行中（BANDS 到这一帧写回完成）。 */
#define BLIT_ST_BOVF     (1u << 6)  /* STATUS[6]：某带的桶满了，丢了命令（CTRL[1] 清除）。 */
#define BLIT_ST_BERR     (1u << 7)  /* STATUS[7]：帧内命令的行距不等于帧行距，已丢弃（CTRL[1] 清除）。 */

/* 提交方式，见文件开头说明。 */
#define BLIT_MODE_APB    0
#define BLIT_MODE_LIST   1
#define BLIT_MODE_WORD   2

/* DDR 列表缓冲的字数（.bss，256 字节对齐）；一帧最高压力约 6000 字。
 * 分带 + 流水帧时引擎一直在跑上一帧，只有它空闲的帧（有逻辑 tick 的帧）才回到缓冲开头，
 * 所以留够几十帧的量（2026-10-10：16384 -> 65536 字；2026-10-11：131072 字 = 512 KB）。 */
#ifndef BLIT_LIST_WORDS
#define BLIT_LIST_WORDS  131072u
#endif
/* 未发布的字攒到这么多就写一次 LIST_END（在记录边界检查）。 */
#ifndef BLIT_LIST_KICK
#define BLIT_LIST_KICK   32u
#endif

#define BLIT_FILL        0u        /* CMD[0]=0：纯色填充。 */
#define BLIT_COPY        (1u << 0)  /* CMD[0]=1：图片复制。 */
#define BLIT_KEY         (1u << 1)  /* CMD[1]=1：COPY 使用颜色键。 */
#define BLIT_APLANE      (1u << 2)  /* CMD[2]=1：COPY 使用每像素 A8 透明度。 */
#define BLIT_SYNC        (1u << 3)  /* CMD[3]=1：等待先前写入响应后再开始。 */
#define BLIT_CACHE       (1u << 4)  /* CMD[4]=1：源图走引擎源缓存（仅 COPY）。 */
#define BLIT_INV         (1u << 5)  /* CMD[5]=1：先清空源缓存。 */
/* CMD[6]=1：加法混合（CAPS2[0]，2026-10-11）：每通道 out = min(dst + (src * a + 128) >> 8, 最大值)，
 * a = 全局透明度（× A8 平面），255 = 源全部加上去；色键照样跳过。发光弹、火花、闪光用。
 * 列表里是 REC[26] / SLOT[26]（BLIT_L_CMD 自动转换）。分带模式下"纯加法"（透明度 255、无 A8）的
 * 带缓存 COPY / FILL 走 K 通路（每拍 8 像素，在带缓冲里原地读-加-写），和不透明一样快；
 * 其余加法命令走窄通路（每拍 2 像素）。直接画（非分带）时要读 DDR 里的目标，慢很多。 */
#define BLIT_ADD         (1u << 6)
#define BLIT_R_CAPS2     0xBCu      /* CAPS2（RO）：[0] 有加法混合。旧位流读出 0。 */
#define BLIT_CAPS2_ADD   (1u << 0)
#define BLIT_CAPS2_DBG   (1u << 1)      /* DBG0..3（0xC0..0xCC） */
#define BLIT_CAPS2_WIN2560 (1u << 2)    /* 分带窗口每行 2560 字节（1280 像素，720p，2026-10-21） */
#define BLIT_CAPS2_MAXB(c) (((c) >> 8) & 0xFFu)   /* 一帧最多几带（0 = 旧位流，按 32） */
/* 只有宽度不超过这么多像素的拷贝才走源缓存：精灵、字模、敌人、宝石都在内，
 * 整屏 / 背景这类大图不进缓存（会把缓存冲掉，且按 8 字的行读比 64 字 burst 慢）。 */
/* 开机时源缓存开 / 关（makefile CACHE ?= 1；调试版也可用 'c' 键切换）。 */
#ifndef BLIT_CACHE_DEFAULT
#define BLIT_CACHE_DEFAULT 1
#endif
#ifndef BLIT_CACHE_MAX_W
#define BLIT_CACHE_MAX_W 64
#endif
#define BLIT_GA(a)       (((uint32_t)(a) & 0xFFu) << 8) /* 全局透明度放入 CMD[15:8]。 */
#define BLIT_OPAQUE      255u       /* 不透明的全局透明度值。 */

#define BLIT_MAX_W       2045
#define BLIT_MAX_H       2047

#ifdef BLIT_HOST_MODEL                   /* host test: C model instead of MMIO */
uint32_t blit_model_rd(uint32_t off);
void     blit_model_wr(uint32_t off, uint32_t v);
static inline uint32_t blit_rd(uint32_t off)             { return blit_model_rd(off); }
static inline void     blit_wr(uint32_t off, uint32_t v) { blit_model_wr(off, v); }
#else
static inline uint32_t blit_rd(uint32_t off)
{
    return *(volatile uint32_t *)(BLIT_BASE + off);
}
static inline void blit_wr(uint32_t off, uint32_t v)
{
    *(volatile uint32_t *)(BLIT_BASE + off) = v;
}
#endif

/* Pushes allowed per STATUS read. Smaller than the queue so the occupancy is
 * sampled (qpeak) at least every BLIT_CREDIT_MAX commands; one APB read per
 * 64 commands costs nothing measurable. */
#ifndef BLIT_CREDIT_MAX
#define BLIT_CREDIT_MAX 64u
#endif

/* CPU 内存中的驱动状态，不是 FPGA 命令 FIFO。
 * 参数成员保存最近写入的寄存器值，BLIT_SET 据此跳过重复 MMIO 写入。
 * blit_init() 初始化全部成员；之后累计统计通常由调用方取差值，
 * qpeak 可由调用方在新的测量窗口开始时清零。CTRL 不会修改这些软件变量。
 */
typedef struct {
    uint32_t dst;         /* @dst：DST 的软件影子，目标左上角像素的字节地址。 */
    uint32_t dstr;        /* @dstr：DSTR 的软件影子，目标相邻两行起点的字节距离。 */
    uint32_t src;         /* @src：SRC 的软件影子，源区域左上角 RGB565 像素的字节地址。 */
    uint32_t sstr;        /* @sstr：SSTR 的软件影子，源相邻两行起点的字节距离。 */
    uint32_t a;           /* @a：AADDR 的软件影子，每像素 A8 透明度数据的起始字节地址。 */
    uint32_t astr;        /* @astr：ASTR 的软件影子，A8 数据相邻两行起点的字节距离。 */
    uint32_t size;        /* @size：SIZE 的软件影子，低 11 位为宽度，[26:16] 为高度，单位为像素。 */
    uint32_t color;       /* @color：COLOR 的软件影子，低 16 位保存填充用 RGB565 颜色。 */
    uint32_t key;         /* @key：KEY 的软件影子，低 16 位保存需要跳过的源 RGB565 颜色。 */
    uint32_t credits;     /* @credits：已确认可提交的剩余额度，提交时减 1，最多 BLIT_CREDIT_MAX 条。 */
    uint32_t cmds;        /* @cmds：驱动从最近一次 blit_init() 起提交的命令累计数，不是完成数。 */
    uint32_t depth;       /* @depth：命令 FIFO 的 RAM 深度，由 CAPS.CQ_AW 计算，当前为 16。 */
    uint32_t stall_ticks; /* @stall_ticks：命令 RAM 满时等待空位的累计 CLINT tick 数，不含 blit_wait()。 */
    uint32_t stalls;      /* @stalls：blit_refill() 因命令 RAM 满而进入等待的累计次数。 */
    uint32_t refills;     /* @refills：blit_refill() 的累计调用次数，不计内部轮询或其他 STATUS 读取。 */
    uint32_t qpeak;       /* @qpeak：采样到的命令 RAM 最大占用量，不含 FIFO 输出预取和执行流水线。 */
    /* ---- 硬件命令列表 ---- */
    int       mode;       /* @mode：当前提交方式 BLIT_MODE_*，由 blit_set_mode() 设置。 */
    int       has_list;   /* @has_list：CAPS[24]，硬件支持命令列表时为 1。 */
    int       has_limit;  /* @has_limit：CAPS[25]，硬件有 LIMIT 寄存器时为 1。 */
    uint32_t  rd_max;     /* @rd_max：当前 LIMIT 读 burst 上限（无 LIMIT 时为 16）。 */
    uint32_t  wr_max;     /* @wr_max：当前 LIMIT 写 burst 上限（无 LIMIT 时为 128）。 */
    int       has_limit_words; /* @has_limit_words：CAPS[26]，LIMIT 有在途读字数字段时为 1。 */
    uint32_t  rw_max;     /* @rw_max：当前在途读字数上限（无该字段时为 1024）。 */
    int       has_cache;  /* @has_cache：CAPS[27]，引擎有源缓存时为 1。 */
    uint32_t  cflag;      /* @cflag：小图拷贝要 OR 的命令位，源缓存开启时 = BLIT_CACHE，否则 0。 */
    uint32_t  cinvs;      /* @cinvs：发出的 INV 次数（统计）。 */
    int       has_table;  /* @has_table：CAPS[28]，列表支持 TAB 记录时为 1。 */
    uint32_t  ltabs;      /* @ltabs：发出的 TAB 记录数（统计）。 */
    int       has_stall;  /* @has_stall：CAPS[29]，有停顿计数器时为 1。 */
    int       has_wide;   /* @has_wide：CAPS[30]，有 128 位宽通路时为 1。 */
    uint32_t  wide;       /* @wide：当前 WIDE 寄存器值（无宽通路时为 BLIT_WIDE_OFF）。 */
    int       lhold;      /* @lhold：1 = 列表字只攒不发布（测量用，见 blit_list_release）。 */
    uint32_t  lctx[10];   /* @lctx：列表上下文的软件影子，下标 = 字段位号（DST…BASE）。 */
    uint32_t *lwp;        /* @lwp：DDR 列表中下一个要写的字。 */
    uint32_t *lkick;      /* @lkick：已发布到 LIST_END 的位置，lwp - lkick = 未发布的字数。 */
    uint32_t *llim;       /* @llim：列表缓冲末尾（不含）。 */
    uint32_t  lwords;     /* @lwords：写入列表的字累计数（DDR 与 LIST_WORD 两种都计）。 */
    uint32_t  lkicks;     /* @lkicks：写 LIST_END 的累计次数。 */
    uint32_t  lwraps;     /* @lwraps：缓冲写满、等引擎读完后回到开头的累计次数（计入 stall）。 */
    /* ---- 分带合成（D） ---- */
    int       has_band;   /* @has_band：CAPS[31]，引擎有分带合成时为 1。 */
    int       has_add;    /* @has_add：CAPS2[0]，引擎有加法混合（CMD[6]）时为 1。 */
    uint32_t  band_max_pitch;  /* @band_max_pitch：分带帧最大行距（字节）：1280 或 2560（CAPS2[2]）。 */
    uint32_t  band_max_bands;  /* @band_max_bands：分带帧最多几带：32 或 CAPS2[15:8]（45）。 */
    int       band;       /* @band：1 = gfx_hw 的每一帧用 BANDS..ENDB 包起来（blit_set_band）。 */
    int       band_open;  /* @band_open：BANDS 已写、ENDB 还没写（这一帧正在录）。 */
    uint32_t  band_geo;   /* @band_geo：当前 BAND_GEO 寄存器值（行距 | 带数 << 16 | KOFF）。 */
    uint32_t  band_koff;  /* @band_koff：BLIT_BAND_KOFF 或 0（K 通路开关，A/B 用）。 */
    uint32_t  bseq;       /* @bseq：已经发出 ENDB 的分带帧序号 = 该帧画完时 FRAMES 的值（按 32 位回绕）。 */
    uint32_t  bsplits;    /* @bsplits：帧中途被提前结束（列表回绕 / blit_wait）的次数，之后的命令直接画。 */
} BlitState;
extern BlitState blit_st;

/* 统计计数（cmds stall_ticks stalls refills qpeak cinvs lwords lkicks lwraps）只在
 * 调试版（make STATS=1 → -DSPROUT_STATS=1）和主机模型里累加；发行版这些成员恒为 0，
 * 省掉每条命令的计数 store（D$ 写直通，每次 store 都上总线）和排队等待时的 CLINT 读取。
 * 驱动逻辑本身不依赖这些计数。 */
#if SPROUT_STATS || defined(BLIT_HOST_MODEL)
#define BLIT_STATS 1
#define BLIT_STAT(x) do { x; } while (0)
#else
#define BLIT_STATS 0
#define BLIT_STAT(x) do { } while (0)
#endif

/* 精灵槽的软件标签（gfx_hw.c 批量精灵用）：槽中已定义的 SRC 与 SIZE|key 位。
 * blit_init() / blit_set_mode() 置为无效值。 */
extern uint32_t blit_slot_src[BLIT_L_SLOTS];
extern uint32_t blit_slot_size[BLIT_L_SLOTS];

/* 检查 ID；找到引擎后等待空闲、清性能计数和错误，并同步软件影子与参数寄存器。
 * 参数写为 0，KEY 写为 0xF81F；读取 CAPS 设置 depth，清额度及软件统计。
 * 成功返回 1，ID 不符返回 0；不触发绘图、不复位 FIFO、不清硬件 CMD_PUSHED。
 */
int  blit_init(void);
int  blit_present(void);

/* Slow path of blit_push (blit.c): read STATUS for new credits; if the queue
 * is completely full, spin until a slot frees and account that time. */
void blit_refill(void);

/* 采样命令 RAM 占用量 depth-free，更新 qpeak 和 credits；不含预取及执行中的命令。 */
uint32_t blit_queue_used(void);

/* 切换提交方式：先发布并等待引擎空闲；硬件不支持列表时只接受 BLIT_MODE_APB。
 * 返回实际生效的方式。 */
int  blit_set_mode(int mode);

/* 源缓存（blit.c）：开 / 关（无 CAPS[27] 时恒关，打开时先 INV），返回实际状态。 */
int  blit_set_cache(int on);
/* CPU 改写了引擎可能已缓存的源图之后调用：在命令流里插一条尺寸为 0 的 INV 命令。
 * 源缓存关闭时什么也不做。gfx_hw.c 在每帧开头和 publish 时调用。 */
void blit_cache_inv(void);
/* 读全部引擎计数器（只读，不清零，十几次 APB 读，测量时用）；d = b - a（按 32 位回绕）。 */
void blit_perf_snap(BlitPerf *p);
void blit_perf_diff(BlitPerf *d, const BlitPerf *a, const BlitPerf *b);

/* 宽 w 的拷贝应带的缓存位。 */
static inline uint32_t blit_cache_flag(int w) { return w <= BLIT_CACHE_MAX_W ? blit_st.cflag : 0u; }

/* 引擎卡死诊断（两个版本都有，正常路径只多一个计数器）：blit_wait / 队列满 / 列表缓冲满
 * 这几处忙等每读 BLIT_STUCK_SPINS 次 STATUS（约 1 s）还没等到，就调 blit_stuck()，
 * 从串口直接打一份寄存器快照（最多 4 份，两份之间计数器不动 = 引擎真卡死）。 */
#ifndef BLIT_STUCK_SPINS
#define BLIT_STUCK_SPINS (1u << 22)
#endif
void blit_stuck(const char *where);

/* ---- 列表（blit.c） ---- */
void      blit_list_kick(void);            /* 发布未发布的字：fence + 写 LIST_END */
void      blit_list_frame(void);           /* 帧开始：列表空闲则回到缓冲开头 */
/* 测量用（'8' 引擎剖析）：blit_st.lhold = 1 后所有自动发布（每 BLIT_LIST_KICK 字、TAB、gfx_end）
 * 都只攒在缓冲里，引擎不动；blit_list_release() 清 lhold 并一次发布，之后 CPU 只等不碰 DDR，
 * 量到的就是引擎单独跑的时间。blit_wait / blit_busy / 缓冲写满（wrap）会自动清 lhold。 */
void      blit_list_release(void);
uint32_t *blit_list_wrap(uint32_t need);   /* 缓冲不够：等引擎读完，回到开头 */
void      blit_list_fill(uint32_t dst, uint32_t dstr, uint32_t size, uint32_t color, uint32_t cmd);
void      blit_list_copy(uint32_t dst, uint32_t dstr, uint32_t src, uint32_t sstr,
                         uint32_t size, uint32_t cmd, uint32_t a, uint32_t astr);
/* 列表上下文的 BASE / DSTR（以及 key 时的 KEY）不同则补一条只设置的记录。 */
void      blit_list_draw_ctx(uint32_t base, uint32_t dstr, int key);
/* 定义精灵槽：只设置记录（SRC/SSTR/SIZE）+ SLOT 字。 */
void      blit_list_slot(uint32_t slot, uint32_t src, uint32_t sstr, uint32_t size, uint32_t cmd);

/* ---- 分带合成（blit.c，CAPS[31]） ----
 * blit_set_band(1) 之后，gfx_hw 的每一帧自动用 BANDS..ENDB 包起来（gfx_begin_hw / gfx_end），
 * 画法见 BLIT_R_BAND_GEO 处的说明；应用代码不用改。需要 LIST 模式；切换时先等引擎空闲。
 * 返回实际状态（硬件没有分带、或不是 LIST 模式时为 0）。 */
int  blit_set_band(int on);
/* K 通路（每拍 8 像素）开 / 关，A/B 对比用；先等引擎空闲。 */
void blit_band_set_koff(int off);
/* 一帧开始（gfx_hw 调用）：fb = 帧缓冲地址（256 字节对齐），pitch = 行距（字节，16 的倍数，
 * 256..1280），rows = 行数（16 的倍数，<= 512）。条件满足时写 BANDS，返回 1；
 * 否则返回 0，这一帧照旧直接画。 */
int  blit_band_begin(uint32_t fb, uint32_t pitch, uint32_t rows);
/* 一帧结束：写 ENDB 并发布（blit_st.bseq 加 1 = 这一帧的序号）。没有打开的帧时什么也不做。
 * blit_wait() / blit_busy() / 列表缓冲回绕会自动提前结束打开的帧（之后的命令直接画，画面不变）。 */
void blit_band_end(void);
/* 序号为 seq 的分带帧是否已经画完并全部写回 DDR（可以翻页）。只读一个寄存器，不等待：
 * 主循环用它实现"引擎画第 N 帧时 CPU 已经在录第 N+1 帧"。 */
static inline int blit_band_done(uint32_t seq) { return (int32_t)(blit_rd(BLIT_R_FRAMES) - seq) >= 0; }

/* 在 DDR 列表中预留 n 个字，返回写指针（写完后由调用者更新 blit_st.lwp）。 */
static inline uint32_t *blit_list_reserve(uint32_t n)
{
    uint32_t *p = blit_st.lwp;
    if ((uint32_t)(blit_st.llim - p) < n) p = blit_list_wrap(n);
    return p;
}

/* 记录写完：未发布的字够多就发布。 */
static inline void blit_list_commit(uint32_t *p)
{
    blit_st.lwp = p;
    if ((uint32_t)(p - blit_st.lkick) >= BLIT_LIST_KICK) blit_list_kick();
}

#define BLIT_SET(field, reg, v) do { uint32_t v_ = (v); \
    if (blit_st.field != v_) { blit_st.field = v_; blit_wr(reg, v_); } } while (0)

static inline void blit_push(uint32_t cmd)
{
    /* STATUS[31:16] = free slots; the engine only ever frees more, so that
     * many pushes are safe without asking again (no APB stall on CMD) */
    if (!blit_st.credits) blit_refill();
    --blit_st.credits;
    BLIT_STAT(++blit_st.cmds);
    blit_wr(BLIT_R_CMD, cmd);
}

static inline uint32_t blit_size(int w, int h)
{
    return ((uint32_t)h << 16) | (uint32_t)w;
}

/* Solid (alpha = 255) or translucent rectangle fill. */
static inline void blit_fill(uint32_t dst, uint32_t dstr, int w, int h,
                             uint32_t color, uint32_t alpha)
{
    if (blit_st.mode == BLIT_MODE_LIST) {
        blit_list_fill(dst, dstr, blit_size(w, h), color & 0xFFFFu, BLIT_FILL | BLIT_GA(alpha));
        return;
    }
    BLIT_SET(dst,   BLIT_R_DST,   dst);
    BLIT_SET(dstr,  BLIT_R_DSTR,  dstr);
    BLIT_SET(size,  BLIT_R_SIZE,  blit_size(w, h));
    BLIT_SET(color, BLIT_R_COLOR, color & 0xFFFFu);
    blit_push(BLIT_FILL | BLIT_GA(alpha));
}

/* 加法填充（BLIT_ADD，CAPS2[0]）：每像素 dst += color * alpha / 256（每通道饱和）。 */
static inline void blit_fill_add(uint32_t dst, uint32_t dstr, int w, int h,
                                 uint32_t color, uint32_t alpha)
{
    if (blit_st.mode == BLIT_MODE_LIST) {
        blit_list_fill(dst, dstr, blit_size(w, h), color & 0xFFFFu, BLIT_FILL | BLIT_ADD | BLIT_GA(alpha));
        return;
    }
    BLIT_SET(dst,   BLIT_R_DST,   dst);
    BLIT_SET(dstr,  BLIT_R_DSTR,  dstr);
    BLIT_SET(size,  BLIT_R_SIZE,  blit_size(w, h));
    BLIT_SET(color, BLIT_R_COLOR, color & 0xFFFFu);
    blit_push(BLIT_FILL | BLIT_ADD | BLIT_GA(alpha));
}

/* Block copy. flags: BLIT_KEY (skip pixels == KEY), BLIT_SYNC, BLIT_ADD (additive).
 * alpha < 255 blends src over dst. a_addr != 0 adds a per-pixel A8 plane
 * (effective alpha = plane * alpha / 255). */
static inline void blit_copy(uint32_t dst, uint32_t dstr, uint32_t src, uint32_t sstr,
                             int w, int h, uint32_t flags, uint32_t alpha,
                             uint32_t a_addr, uint32_t astr)
{
    if (blit_st.mode == BLIT_MODE_LIST) {
        if (a_addr) flags |= BLIT_APLANE;
        blit_list_copy(dst, dstr, src, sstr, blit_size(w, h), BLIT_COPY | flags | BLIT_GA(alpha),
                       a_addr, astr);
        return;
    }
    BLIT_SET(dst,  BLIT_R_DST,  dst);
    BLIT_SET(dstr, BLIT_R_DSTR, dstr);
    BLIT_SET(src,  BLIT_R_SRC,  src);
    BLIT_SET(sstr, BLIT_R_SSTR, sstr);
    BLIT_SET(size, BLIT_R_SIZE, blit_size(w, h));
    if (a_addr) {
        BLIT_SET(a,    BLIT_R_AADDR, a_addr);
        BLIT_SET(astr, BLIT_R_ASTR,  astr);
        flags |= BLIT_APLANE;
    }
    blit_push(BLIT_COPY | flags | BLIT_GA(alpha));
}

/* blit_st.key 也是列表模式下期望的 KEY：列表记录在 KEY 与列表影子不同时自动补写。 */
static inline void blit_set_key(uint32_t key) { BLIT_SET(key, BLIT_R_KEY, key & 0xFFFFu); }

static inline uint32_t blit_status(void) { return blit_rd(BLIT_R_STATUS); }
static inline int      blit_busy(void)
{
    if (blit_st.mode == BLIT_MODE_LIST) {       /* 未发布的字永远不会执行；没有 ENDB 的分带帧永远画不完 */
        blit_st.lhold = 0;
        if (blit_st.band_open) blit_band_end(); else blit_list_kick();
    }
    return (blit_rd(BLIT_R_STATUS) & BLIT_ST_BUSY) != 0;
}

/* 设置引擎挂起 burst 上限（见 BLIT_R_LIMIT）；随时可改，只影响速度，不影响结果。 */
static inline void blit_set_limit(uint32_t rd, uint32_t wr)
{
    if (!blit_st.has_limit) return;
    rd = rd == 0 ? 1u : rd > BLIT_LIMIT_RD_MAX ? BLIT_LIMIT_RD_MAX : rd;
    wr = wr == 0 ? 1u : wr > BLIT_LIMIT_WR_MAX ? BLIT_LIMIT_WR_MAX : wr;
    blit_st.rd_max = rd; blit_st.wr_max = wr;
    blit_wr(BLIT_R_LIMIT, (blit_st.rw_max << 16) | (wr << 8) | rd);
}

/* 设置在途读字数上限（LIMIT[26:16]），读/写 burst 上限保持不变。 */
static inline void blit_set_rdwords(uint32_t words)
{
    if (!blit_st.has_limit_words) return;
    blit_st.rw_max = words == 0 ? 1u : words > BLIT_LIMIT_RW_MAX ? BLIT_LIMIT_RW_MAX : words;
    blit_set_limit(blit_st.rd_max, blit_st.wr_max);
}

/* 设置 WIDE 寄存器（见 BLIT_R_WIDE，用 BLIT_WIDE(minw, wth, words) 或 BLIT_WIDE_OFF 组值）；
 * 随时可改，只影响速度，不影响结果（下一条命令开始时生效）。 */
static inline void blit_set_wide(uint32_t v)
{
    if (!blit_st.has_wide) return;
    if (((v >> 16) & 0x7FFu) == 0u) v |= 1u << 16;   /* 硬件把 0 字当 1 字 */
    blit_st.wide = v;
    blit_wr(BLIT_R_WIDE, v);
}

/* Wait until every queued command has finished and all its writes are
 * acknowledged by the DDR port (safe to flip / to read back). */
static inline void blit_wait(void)
{
    uint32_t n = 0;
    if (blit_st.mode == BLIT_MODE_LIST) {
        blit_st.lhold = 0;
        if (blit_st.band_open) blit_band_end(); else blit_list_kick();   /* 打开的分带帧先结束 */
    }
    while (blit_rd(BLIT_R_STATUS) & BLIT_ST_BUSY)
        if ((++n & (BLIT_STUCK_SPINS - 1u)) == 0) blit_stuck("blit_wait");
    blit_st.credits = 0;
}

/* Make CPU stores visible to the engine (write-through D$: drain the store path). */
static inline void blit_fence(void)
{
#ifdef __riscv
    __asm__ volatile ("fence" ::: "memory");
#else
    __sync_synchronize();
#endif
}

/* Drop the whole D$ (VexRiscv custom instruction, bsp driver/vexriscv.h) so
 * the CPU re-reads memory the blitter wrote. */
static inline void blit_dcache_invalidate(void)
{
#ifdef __riscv
    __asm__ volatile (".word(0x500F)" ::: "memory");
#endif
}

#endif
