PROJ_NAME=game
STANDALONE = ..
# The Efinity RISC-V IDE sets BSP in its environment; default it so a plain
# command-line "make" (e.g. build_game.bat) also finds the BSP headers.
BSP ?= efinix/EfxSapphireSoc

# Performance build by default (-O3). soc.mk defaults to DEBUG=yes (-Og), which
# would make the FPS numbers meaningless. Override with: make DEBUG=yes
DEBUG ?= no
BENCH ?= yes

# 1 = 32-bit stores in CPU fill_rect, 0 = original per-pixel 16-bit loop
GFX_CPU_WORD_FILL ?= 1
# 1 = start in autoplay (bot drives the game), 0 = wait for UART keys
AUTOPLAY ?= 1

# Stress mode (l): 0 = enemy seeds vanish at the arena edge like normal mode (board default),
#                  1 = they wrap to the opposite edge (original SproutRun behaviour)
STRESS_WRAP ?= 0

# 1 = wait for vsync (max 60 FPS), 0 = uncapped render loop (toggle with 'v' at run time)
VSYNC ?= 1
# 1 = 开 vsync 时分带帧也流水（引擎画第 N 帧时 CPU 录第 N+1 帧，画面晚一帧；'%' 键切换），0 = 旧做法
VPIPE ?= 1

# 1 = start with the BitBlt engine backend (if the bitstream has it), 0 = start on CPU ('g' toggles)
HW ?= 1

# 1 = text glyphs are colour-keyed block copies from a pre-drawn font atlas (one
#     BitBlt command per character), 0 = merged fills per glyph (no atlas memory)
FONT_ATLAS ?= 1

# 0 = 发行版（默认）：HUD 只有 FPS/MS，串口每秒一行，没有启动测速和 A/B 调试键
# 1 = 调试 / 测量版：详细串口报告、统计面板 'f'、启动测速、k o i u j z q y c 0 键
#     用法：make STATS=1   或   build_game.bat STATS=1（改了要先 make clean）
STATS ?= 1

# 1 = 开机打开引擎源缓存（位流 CAPS[27] 有缓存时），0 = 关（同一位流做 A/B 对比用）
CACHE ?= 1

# 1 = 位流有分带合成（CAPS[31]）时开机就用（BANDS..ENDB + 流水帧），0 = 不用（逐条直接画，同旧位流）
#     STATS=1 版里还可以用 '5' 键随时开关
BAND ?= 1

# 0（默认，2026-10-20）= 不把声音库链接进 elf：JTAG 800 kHz 下 2 MB 的库要下载好几分钟，每次改代码都等不起。
#     声音库上电后用 python tools/load_sound.py 单独下载一次（程序运行时也行，游戏一秒内自动认出来），
#     之后只要不断电，重新下载程序都不用再下声音库。
# 1 = 把 sound/sound_bank.bin（tools/wav2bank.py 生成）链接进 elf，放在 DDR 0x02000000（game.ld 的 .sound），
#     下载 elf 时一起写进 DDR（elf 大、下载慢；适合最后交付 / 演示用的版本）。
#     没有 sound/sound_bank.S 时自动不链接。
SOUND ?= 1

# 排查引擎卡死用（2026-10-20）：KPATH=0 分带模式里不用 K 通路；LIMITW = 引擎在途读字数上限（默认 64，1024 = 基本不限）
KPATH ?= 1
# 弹幕场景 720p（2026-10-21）：HD=1 开机就是 1280x720 原生（布局 1.5 倍），HD=0 开机 640x480 居中；运行中 '#' 键切换
HD ?= 1
LIMITW ?= 64

# 15 MB program region below the framebuffers (see game.ld).
# Must be set before bsp.mk, which uses LDSCRIPT ?= default.ld
LDSCRIPT = game.ld

SRCS = 	$(wildcard src/*.c) \
		$(wildcard src/*.cpp) \
		$(wildcard src/*.S) \
		${STANDALONE}/common/start.S
ifeq ($(SOUND),1)
SRCS += $(wildcard sound/sound_bank.S)
endif

# 必须写在 include standalone.mk 之前：standalone.mk 用 $(call LIST_RULE) 生成编译规则时
# 就把当时的 $(CFLAGS) 展开写死了，之后再 CFLAGS += 对 .c 的编译不起作用
# （以前这行在后面，所以 STATS / HW / VSYNC 等 make 变量其实一直没生效，用的是源码里的默认值）。
CFLAGS += -DGFX_CPU_WORD_FILL=$(GFX_CPU_WORD_FILL) -DAUTOPLAY_DEFAULT=$(AUTOPLAY) -DVSYNC_DEFAULT=$(VSYNC) -DVPIPE_DEFAULT=$(VPIPE) -DSTRESS_WRAP=$(STRESS_WRAP) -DHW_DEFAULT=$(HW) -DGFX_FONT_ATLAS=$(FONT_ATLAS) -DSPROUT_STATS=$(STATS) -DBLIT_CACHE_DEFAULT=$(CACHE) -DBAND_DEFAULT=$(BAND) -DKPATH_DEFAULT=$(KPATH) -DTH_HD_DEFAULT=$(HD) -DGAME_LIMIT_WORDS=$(LIMITW)u

# Track header dependencies so changed Game/FX layouts rebuild every user.
CFLAGS += -MMD -MP

include ${STANDALONE}/common/bsp.mk
include ${STANDALONE}/common/riscv64-unknown-elf.mk
include ${STANDALONE}/common/standalone.mk

# 声音库换了要重新汇编 sound_bank.S（.incbin 的依赖 make 自己看不出来）
ifneq ($(wildcard sound/sound_bank.S),)
$(OBJDIR)/obj_files/sound_bank.o: sound/sound_bank.bin
endif

# Matrix action and help definitions are shared; rebuild consumers on changes.
$(OBJDIR)/obj_files/input_devices.o: src/input_devices.h src/matrix_keys.def
$(OBJDIR)/obj_files/main.o: src/input_devices.h src/input_link.h src/panel_link.h src/matrix_keys.def

-include $(wildcard $(OBJDIR)/obj_files/*.d)
