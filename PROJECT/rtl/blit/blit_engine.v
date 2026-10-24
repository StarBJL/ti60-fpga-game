// =============================================================================
// blit_engine.v  -  2D BitBlt accelerator for the Sapphire SoC (赛题二)
//
//   * APB3 slave  : command registers (SoC io_apbSlave_0, CPU base 0xF810_1000)
//   * AXI4 master : 32-bit INCR bursts on SoC io_ddrMasters_0 (shared with the
//                   HDMI scan-out through blit_axi_rd_arb), ID = AXI_ID
//   * Operations  : solid fill, block copy, colour key (strobe-masked, no read
//                   of the destination), alpha fill / alpha copy with a global
//                   8-bit alpha and/or a per-pixel A8 plane
//                   out = (src*a + dst*(256-a) + 128) >> 8 per RGB565 channel
//
// Pixel format: RGB565, little endian, 2 px per 32-bit word (even x = [15:0]).
// All addresses are byte addresses of the top-left pixel; strides are bytes.
// Any 2-byte alignment of source / destination is allowed (odd x on both).
//
// Pipeline (all in clk):
//
//   APB --+
//   DDR list -> decoder --+-> command queue (16) , then:
//
//   APB -> command queue (16)  -> FETCH ---AR---> DDR ---R---> line rings
//                                  |  (hand-off 1)                  |
//                                  +--> PIXEL (ring read, key, alpha blend)
//                                           |  (hand-off 2)      | W FIFO
//                                           +--> WRITE ---AW/W---> DDR
//
//   FETCH  issues per row the reads it needs (dst if blending, A8 plane,
//          source), split into bursts of <= 64 beats that stay inside one
//          256-byte block, only when the ring has room (so R is never stalled).
//   PIXEL  waits until a whole row is in the rings, then produces one 32-bit
//          output word (2 px) per clock with byte strobes (clip + colour key).
//          Row geometry is prepared one row ahead, so rows and commands follow
//          each other back to back (no setup clocks, no pipe drain).
//   WRITE  issues an AW only when the whole burst is already in the W FIFO,
//          so the SoC write arbiter is never held waiting for data.
//
// Ordering: a command that reads the destination (any alpha op) or has the
// SYNC flag waits until every earlier write has been acknowledged (B), so
// overlapping draws are always composed in command order.
//
// Register map (byte offsets inside the 4 KB window):
//   0x00 ID         RO  0x424C5431 "BLT1"
//   0x04 STATUS     RO  [0] busy  [1] queue full  [2] error (sticky)
//                       [3] list active  [4] bad list word (sticky)
//                       [5] band frame active  [6] a band bin overflowed (sticky)
//                       [7] record not allowed in a band frame (sticky)
//                       [31:16] free command slots
//   0x08 CTRL       WO  [0] clear perf counters  [1] clear error
//   0x0C CAPS       RO  {BAND, WIDE, STALL, TAB, CACHE, LIMIT_WORDS, LIMIT, LIST, RING_AW, WF_AW, CQ_AW}
//   0x10 DST_ADDR       0x14 DST_STRIDE [15:0]
//   0x18 SRC_ADDR       0x1C SRC_STRIDE [15:0]
//   0x20 A_ADDR         0x24 A_STRIDE   [15:0]   (A8 alpha plane)
//   0x28 SIZE           [10:0] width  [26:16] height  (pixels)
//   0x2C COLOR          [15:0] fill colour
//   0x30 KEY            [15:0] colour key (reset 0xF81F magenta)
//   0x34 CMD        WO  push command (registers above are sampled):
//                       [0] op 0 = FILL, 1 = COPY
//                       [1] colour key enable (COPY)
//                       [2] per-pixel A8 plane (COPY)
//                       [3] SYNC: wait for all earlier writes first
//                       [4] cache the source (COPY)  [5] INV the cache first
//                       [6] ADD (CAPS2[0]): additive blend, per channel
//                           out = min(dst + (src * a + 128) >> 8, max), a = global
//                           alpha (x A8 plane), 255 = full source; colour key
//                           still skips pixels. Band mode: pure add (alpha 255,
//                           no A8) of cached COPY / FILL runs on the K path
//                           (8 px/clk, window read in place), else narrow path.
//                           List: REC[26] / SLOT[26].
//                       [15:8] global alpha (255 = opaque)
//                       Writing CMD while the queue is full stalls the APB
//                       access (PREADY = 0) until a slot frees up.
//   0x40 PERF_BUSY  RO  clocks with busy = 1
//   0x44 PERF_CMDS  RO  commands completed
//   0x48 PERF_RBEAT RO  32-bit read beats
//   0x4C PERF_WBEAT RO  32-bit write beats
//   0x50 CMD_PUSHED RO  commands accepted since reset (APB + list)
//   0x54 LIMIT      RW  (CAPS[25]) [4:0] max outstanding read bursts (1..16, reset 16)
//                       [15:8] max outstanding write bursts (1..128, reset 128).
//                       [26:16] max read words in flight (1..1024, reset 1024):
//                       a new read burst is issued only while fewer words than
//                       this are requested but not yet returned. Unlike [4:0]
//                       it limits bytes, not bursts: big copies (64-word bursts)
//                       keep only a few hundred bytes queued ahead of the CPU,
//                       small sprites (2..6-word bursts) still get 16 bursts.
//                       Everything the engine has queued in the DDR controller is
//                       served before a later CPU access (cache refill, store), so
//                       a deep queue is fast for the engine but makes every CPU
//                       DDR access wait; a small limit trades engine speed for CPU.
//
// Command list (CAPS[24] = 1): the CPU stores 32-bit list words in DDR and
// only moves LIST_END; the engine fetches them with read bursts (64-word
// FIFO) and decodes them into the same command queue. The list has its own
// parameter registers ("list context": DST DSTR SRC SSTR A ASTR SIZE COLOR
// KEY BASE), independent of the APB registers 0x10..0x30, and 64 sprite slots.
//   0x38 LIST_ADDR  W   start a new list at this byte address (word aligned);
//                       stalls (PREADY = 0) while a list is still active.
//                   R   address of the next word to fetch
//   0x3C LIST_END   RW  byte address after the last valid word. May be moved
//                       forward at any time, but only to a record boundary.
//   0x58 LIST_WORD  WO  feed one list word through APB instead of DDR (same
//                       decoder; stalls while it cannot be accepted)
//   STATUS [3] list active (words pending or being decoded)  [4] bad list word
//   CMD written while a list is active stalls until the list is done, so APB
//   commands and list commands execute in the order the CPU issued them.
//
// List words:
//   [31] = 1  DRAW   [30:25] slot  [24:0] byte offset
//                    copy the sprite of that slot to BASE + offset
//   [31:28]=0 REC    [25] INV  [24] cache (= CMD[5:4])
//                    [23:16] global alpha  [15:12] flags (= CMD[3:0])
//                    [10] no push  [9:0] mask: one following word per set bit,
//                    lowest bit first: 0 DST 1 DSTR 2 SRC 3 SSTR 4 A 5 ASTR
//                    6 SIZE 7 COLOR 8 KEY 9 BASE; then the command is pushed
//                    with the list context (unless [10])
//   [31:28]=1 SLOT   [24] cache  [23:16] global alpha  [15:12] flags  [5:0] slot
//                    slot <- {context SRC, SSTR, SIZE, flags, alpha, cache}
//   A DRAW takes SRC/SSTR/SIZE/flags/alpha from the slot and DSTR, KEY, COLOR,
//   A, ASTR from the list context; it does not change the context.
//   [31:28]=2 TAB    [15:0] n, then one word: byte address of a sprite table
//                    (CAPS[28]). The engine reads the n items itself, 8 bytes
//                    each: {int16 x, int16 y} {sprite}, and draws every item
//                    like DRAW(slot = sprite[5:0], y * DSTR + x * 2) - x, y as
//                    unsigned 12 bit, the item must lie inside the target.
//                    The list fetch jumps to the table and back: list words
//                    already prefetched are dropped and fetched again after
//                    the table. The table must stay unchanged until the
//                    engine is idle (or a later SYNC/fence point); define the
//                    slots before the TAB record. LIST_END may still be moved
//                    meanwhile; LIST_ADDR/LIST_END read back the main list.
//                    LIST_WORD writes stall until the table is done.
//
// Source cache (CAPS[27] = 1): 16 KB, direct mapped, 512 lines x 32 bytes, in
// block RAM. Only the source stream of COPY commands that carry the cache flag
// uses it (sprites, font atlas: small images read again and again); their
// source reads are split at 32-byte line boundaries. A hit needs no AXI read
// at all; a miss reads the whole aligned line (8 words) and keeps it.
// The CPU must invalidate after it changed cached source data in DDR
// (the engine never snoops its own or the CPU's writes):
//   CMD [4] cache the source of this COPY   [5] INV: clear the whole cache
//           before this command (also allowed on a size-0 nop command)
//   REC [24] cache  [25] INV        SLOT [24] cache (DRAW uses the slot's bit)
//   0x5C PERF_CHIT  RO  cached source reads served from the cache (bursts)
//   0x60 PERF_CMISS RO  cache misses (line fills)
// INV takes effect in command order: everything queued before it still sees
// the old data (fills issued earlier complete before any later read).
//
// Stall counters (CAPS[29] = 1), cleared with CTRL[0] like the others. Each
// counts clocks; together with PERF_WBEAT they show where a busy engine waits.
// Write side (AXI W/AW):
//   0x64 PERF_WSTALL  RO  W data valid, DDR not ready (m_wvalid & ~m_wready)
//   0x68 PERF_AWWAIT  RO  W data in the FIFO but its AW not issued yet
//   0x6C PERF_AWSTALL RO  AW valid, DDR not ready
//   0x70 PERF_ARSTALL RO  AR valid, read port not ready (HDMI / DDR busy)
// PIXEL stage (counted only while busy = 1):
//   0x74 PERF_PXWAIT  RO  next row waits for read data (rings not filled)
//   0x78 PERF_PXFULL  RO  write side full (W FIFO credit or header FIFO h2)
//   0x7C PERF_PXBUB   RO  fixed overhead: header take / row prepare / row start
//                         clocks while no row is being issued
//   0x80 PERF_PXIDLE  RO  no command header ready (fetch / list side behind)
//                         (not counted in wide mode, see PERF_WCLK)
// Wide path (C2, CAPS[30]): opaque COPY (no key, galpha FF, dst/src and
// dstr/sstr equal mod 16 bytes) and opaque FILL with w >= MINW run on the
// 128-bit x_* port, 8 px per clock. Such a command waits until the narrow
// pipeline is drained; it uses the line rings + cache data RAM as a
// 1024 x 128-bit buffer (cache lines 0..127 are invalidated).
//   0x84 WIDE        RW  [0] OFF (1 = never use the wide path)
//                        [7:4] WTH: COPY writes wait for WTH*64 buffered beats
//                              before a write batch (0 = write when ready)
//                        [15:8] MINW/8 (reset 16 = 128 px)
//                        [26:16] wide read words in flight (1..1024, reset 256)
//   0x88 PERF_WCLK   RO  clocks in wide mode (busy = wbeat + PX* + WCLK)
//   0x8C PERF_XWBEAT RO  W beats on x_* (128-bit; wide path, band bins, band flush)
//   0x90 PERF_XRBEAT RO  wide R beats (128-bit)
//
// Band mode (D, CAPS[31]): a frame between the list records BANDS and ENDB is not
// drawn command by command in DDR, but composed 16 rows at a time in an on-chip
// window (16 rows x WIN_BYTES bytes: 8 pixel lanes x 16 bit x WIN_BYTES beats;
// WIN_BYTES 2560 = 1280 px rows (720p), 40 RAM blocks; 1280 = 640 px, 24 blocks)
// and written back with 16-beat bursts:
//   phase 1 (bins)  every command of the frame (REC, DRAW, every TAB item) is
//            resolved as usual and appended as an 8-word record to the bin of
//            every band it touches (BAND_BIN + band << shift, x_* 2-beat bursts).
//            Commands whose first row is outside the frame are queued and drawn
//            normally (other targets); a size-0 command only passes its INV.
//   phase 2 (bands) after ENDB, per band: the bin is fetched (list DMA), each
//            command is clipped to the 16 rows and drawn into the window -
//              K path     COPY through the source cache (opaque / keyed) and
//                         opaque FILL: 8 px per clock, no DDR write per sprite
//              wide path  aligned opaque COPY, w >= MINW, not cached: the R
//                         beats go straight into the window
//              narrow     the rest; blends read the destination from the window
//            then the window is written to the frame (x_* 16-beat bursts) and
//            cleared. Commands keep their order (a path starts only when the
//            others have written everything). When all bands are written and
//            acknowledged, FRAMES counts and the main list continues.
// A band frame starts black: pixels no command draws are 0, not the old frame.
// Commands in the frame must use the frame's row stride (single-row commands:
// any), else they are dropped (STATUS[7]). LIST_END may move at any time (the
// CPU records the next frame while this one is drawn); CMD / LIST_ADDR /
// LIST_WORD writes stall until the frame is done. BANDS without ENDB keeps the
// engine busy forever.
//   BANDS  [31:28]=3, then one word: frame base address (256-byte aligned)
//   ENDB   [31:28]=4
//   (RES   [31:28]=5: the 8-word bin record, only ever written by the engine)
//   0x94 BAND_GEO    RW  [15:0] frame row stride in bytes (x16, 256..WIN_BYTES)
//                        [21:16] bands (1..45; frame = bands x 16 rows, <= 720)
//                        [31] K path off (everything on the narrow path)
//   0x98 BAND_RECIP  RW  [20:0] floor(2^24 / (stride / 16)) + 1 (row of an address)
//   0x9C BAND_BIN    RW  [31:12] bin area  [4:0] log2(bytes per band bin), 12..20;
//                        a full bin drops further commands (STATUS[6])
//   0xA0 FRAMES      RO  band frames finished since reset
//   0xA4 PERF_P1     RO  clocks in phase 1      0xA8 PERF_FLUSH RO  clocks writing back / clearing
//   0xAC PERF_KBEAT  RO  K path beats (8 px)    0xB0 PERF_RECS  RO  records appended to bins
//   0xB4 PERF_BAND   RO  clocks in phase 2      0xB8 PERF_KCLK  RO  clocks with the K path busy
//   0xBC CAPS2       RO  [0] additive blend (CMD[6], REC[26], SLOT[26])
//                        [1] debug registers DBG0..3 (0xC0..0xCC)
//                        [2] band window rows of 2560 bytes (1280 px, 720p)
//                        [15:8] max bands (45)
//   (during a band frame the PIXEL stall classes 0x74..0x80 and PERF_WCLK stop)
// Change BAND_GEO / BAND_RECIP / BAND_BIN only while no band frame is active.
// =============================================================================
`timescale 1ns / 1ps

module blit_engine #(
    parameter       CQ_AW   = 4,     // command queue: 16 entries (flip-flops)
    parameter       RING_AW = 10,    // line rings: 1024 words per stream
    parameter       WF_AW   = 9,     // write data FIFO: 512 words
    parameter [3:0] AXI_ID  = 4'd1,
    parameter       WIN_BYTES = 2560 // band window row: 2560 (1280 px, 720p) or 1280 (640 px)
)(
    input  wire        clk,
    input  wire        rst,

    // APB3 (psel already decoded for this 4 KB window)
    input  wire [11:0] apb_paddr,
    input  wire        apb_psel,
    input  wire        apb_penable,
    input  wire        apb_pwrite,
    input  wire [31:0] apb_pwdata,
    output reg  [31:0] apb_prdata,
    output wire        apb_pready,
    output wire        apb_pslverror,

    // AXI4 master, read
    output reg         m_arvalid,
    input  wire        m_arready,
    output reg  [31:0] m_araddr,
    output reg  [7:0]  m_arlen,
    output wire [2:0]  m_arsize,
    output wire [1:0]  m_arburst,
    output wire [3:0]  m_arid,
    input  wire        m_rvalid,
    output wire        m_rready,
    input  wire [31:0] m_rdata,
    input  wire        m_rlast,
    input  wire [1:0]  m_rresp,

    // AXI4 master, write
    output reg         m_awvalid,
    input  wire        m_awready,
    output reg  [31:0] m_awaddr,
    output reg  [7:0]  m_awlen,
    output wire [2:0]  m_awsize,
    output wire [1:0]  m_awburst,
    output wire [3:0]  m_awid,
    output wire        m_wvalid,
    input  wire        m_wready,
    output wire [31:0] m_wdata,
    output wire [3:0]  m_wstrb,
    output wire        m_wlast,
    input  wire        m_bvalid,
    output wire        m_bready,
    input  wire [1:0]  m_bresp,

    // 128-bit wide path (C2) -> blit_axi_wide x_*; R is always accepted
    output reg         x_arvalid,
    input  wire        x_arready,
    output reg  [31:0] x_araddr,
    output reg  [3:0]  x_arlen,
    input  wire        x_rvalid,
    input  wire [127:0] x_rdata,
    input  wire        x_rlast,
    input  wire [1:0]  x_rresp,
    output wire        x_awvalid,
    input  wire        x_awready,
    output wire [31:0] x_awaddr,
    output wire [3:0]  x_awlen,
    output wire        x_wvalid,
    input  wire        x_wready,
    output wire [127:0] x_wdata,
    output wire [15:0] x_wstrb,
    output wire        x_wlast,

    output wire        busy
);

    localparam RW = RING_AW;                        // ring word address bits
    localparam [RING_AW+1:0] RING_WORDS = 1 << RING_AW;
    localparam [WF_AW+1:0]   WF_WORDS   = 1 << WF_AW;
    localparam               MAXB       = 45;        // band frame: <= 45 bands (720 rows)
    localparam [31:0]        CAPS2_VAL  = 32'h0000000B | ((WIN_BYTES == 2560) ? 32'h4 : 32'h0) | (MAXB << 8);
    localparam [31:0]        CAPS_VAL   = 32'h80000000 | (1 << 30) | (1 << 29) | (1 << 28) | (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24) | (RING_AW << 16) | (WF_AW << 8) | CQ_AW;

    assign m_arsize  = 3'b010;
    assign m_arburst = 2'b01;
    assign m_arid    = AXI_ID;
    assign m_awsize  = 3'b010;
    assign m_awburst = 2'b01;
    assign m_awid    = AXI_ID;
    assign m_rready  = 1'b1;          // ring space is reserved before each AR
    assign m_bready  = 1'b1;

    // =========================================================================
    // APB registers + command queue
    // =========================================================================
    reg  [31:0] r_dst, r_src, r_aad;
    reg  [15:0] r_dstr, r_sstr, r_astr, r_color, r_key;
    reg  [10:0] r_w, r_h;
    reg  [31:0] perf_busy, perf_cmds, perf_rbeat, perf_wbeat, cmd_pushed;
    reg  [31:0] perf_chit, perf_cmiss;  // source cache hits / line fills
    reg  [31:0] perf_wstall, perf_awwait, perf_awstall, perf_arstall;   // CAPS[29]
    reg  [31:0] perf_pxwait, perf_pxfull, perf_pxbub, perf_pxidle;
    reg  [31:0] perf_wclk, perf_xwbeat, perf_xrbeat;                    // CAPS[30]
    reg         x_off;                  // WIDE[0]: wide path disabled
    reg  [3:0]  x_wth;                  // WIDE[7:4]: COPY write batch, 64-beat units
    reg  [7:0]  x_minw8;                // WIDE[15:8]: min width / 8
    reg  [10:0] x_rdwmax;               // WIDE[26:16]: wide read words in flight
    reg         err;
    // band mode (D, CAPS[31])
    reg  [15:0] bg_s;                   // BAND_GEO[15:0]: frame row stride (bytes, x16, <= WIN_BYTES)
    reg  [5:0]  bg_nb;                  // BAND_GEO[21:16]: number of 16-row bands (1..MAXB)
    reg         bg_koff;                // BAND_GEO[31]: K path off (narrow path for everything)
    reg  [20:0] bg_recip;               // BAND_RECIP: floor(2^24 / (stride / 16)) + 1
    reg  [31:0] bg_bin;                 // BAND_BIN[31:12]: bin area
    reg  [4:0]  bg_sh;                  // BAND_BIN[4:0]: log2(bytes per band bin), 12..20
    reg  [31:0] bg_binsz;               // 1 << bg_sh
    reg  [15:0] bg_capm;                // record count bits that mean: bin full
    reg  [31:0] bc_frames;              // FRAMES: band frames finished
    reg  [31:0] perf_p1, perf_flush, perf_kbeat, perf_recs, perf_band, perf_kclk;
    reg         bovf, berr;             // sticky: a bin overflowed / record not allowed in a band frame
    wire [31:0] dbg0, dbg1, dbg2, dbg3; // DBG0..3 (0xC0..0xCC, RO): live FETCH / R router / K path state

    wire [5:0]  apb_reg = apb_paddr[7:2];
    wire        apb_acc = apb_psel & apb_penable;
    wire        apb_wr  = apb_acc & apb_pwrite;
    wire        cmd_wr   = apb_wr & (apb_reg == 6'h0D);
    wire        laddr_wr = apb_wr & (apb_reg == 6'h0E);
    wire        lend_wr  = apb_wr & (apb_reg == 6'h0F);
    wire        lword_wr = apb_wr & (apb_reg == 6'h16);

    // command list (declared here, logic in the "Command list" section)
    localparam CQ_W = 213;
    reg  [29:0] l_ptr, l_end;           // word addresses: next fetch / end
    reg  [6:0]  l_len;                  // length of the next list burst
    reg         l_len_v;
    reg  [7:0]  lf_inflight;            // list words requested, not yet decoded
    reg  [29:0] m_end, l_ret;           // TAB: main list end / next main word
    wire        tb_call, tb_ret, tb_in;    // TAB: jump to the table / back / inside
    wire [29:0] tb_tab, tb_tend;          // TAB: table start / end (word addresses)
    reg         lerr;
    reg  [4:0]  r_rdmax;                // LIMIT: max outstanding read bursts (1..16)
    reg  [7:0]  r_wrmax;                // LIMIT: max outstanding write bursts (1..128)
    reg  [10:0] r_rdwmax;               // LIMIT: max read words in flight (1..1024)
    reg  [10:0] rd_words;               // read words requested, not yet returned
    reg  [6:0]  rdi_m;                  // rd_words increments, registered (timing, see below)
    reg  [4:0]  rdi_x;
    wire        list_active;            // list words pending or being decoded
    wire        lw_apb_take;            // LIST_WORD accepted by the decoder
    wire        l_push;                 // decoder pushes a command
    wire [CQ_W-1:0] cq_ldin;

    // band mode (D): declared here, logic in the "Band mode" section
    localparam BC_OFF = 3'd0, BC_P1 = 3'd1, BC_P1E = 3'd2, BC_CLR = 3'd3,
               BC_BAND = 3'd4, BC_FLUSH = 3'd5, BC_FIN = 3'd6;
    reg  [2:0]  bc_state;
    reg  [5:0]  bc_b;                   // band being drawn
    reg  [31:0] bc_fb, bc_wbase;        // frame base / first byte of the current band
    // Timing (2026-10-21, 720p build core_clk -0.043 ns: bc_state -> decode -> x_* owner
    // mux -> wide W data): the decodes used on wide / far-reaching nets are registered
    // copies, written from the next state bc_nst, so they always equal the decode of
    // bc_state (cycle-identical behaviour).
    reg         bc_p1_r, bm_r, bc_fl_r;
    wire        bc_act  = (bc_state != BC_OFF);
    wire        bc_p1   = bc_p1_r;                    // = (bc_state == BC_P1) | (bc_state == BC_P1E)
    wire        bm      = bm_r;                       // = (bc_state == BC_BAND): commands draw into the band window
    wire        bc_call = bc_act & (bc_state != BC_P1);   // main list parked in l_ret / m_end
    wire        y_in_ok;                // Y pipeline takes a record this clock
    wire        y_cq_push;              // Y pipeline pushes a command into the queue
    wire [CQ_W-1:0] y_out;

    wire        cq_full;
    wire        cq_push = (cmd_wr & ~cq_full & ~list_active) | (l_push & ~bc_act) | y_cq_push;
    assign apb_pready    = ~((cmd_wr & (cq_full | list_active)) |
                             (laddr_wr & list_active) |
                             (lword_wr & ~lw_apb_take));
    assign apb_pslverror = 1'b0;

    // command record, LSB first
    //   [31:0] dst  [47:32] dstr  [79:48] src  [95:80] sstr  [127:96] aad
    //   [143:128] astr  [154:144] w  [165:155] h  [181:166] color
    //   [197:182] key  [201:198] {sync, aplane, key_en, op}  [209:202] galpha
    //   [211:210] {inv, cache}  [212] add (D2: additive blend)
    wire [CQ_W-1:0] cq_adin = {apb_pwdata[6], apb_pwdata[5:4], apb_pwdata[15:8], apb_pwdata[3:0], r_key, r_color,
                               r_h, r_w, r_astr, r_aad, r_sstr, r_src, r_dstr, r_dst};
    wire [CQ_W-1:0] cq_din  = y_cq_push ? y_out : (l_push ? cq_ldin : cq_adin);
    wire [CQ_W-1:0] cq_dout;
    wire            cq_valid;
    wire            cq_pop;
    wire [CQ_AW+1:0] cq_count;
    wire [CQ_AW:0]   cq_space;

    blit_rfifo #(.DW(CQ_W), .AW(CQ_AW)) u_cq (           // flip-flops (C2: was 11 RAM blocks)
        .clk(clk), .rst(rst),
        .push(cq_push), .din(cq_din), .full(cq_full), .space(cq_space),
        .pop(cq_pop), .dout(cq_dout), .valid(cq_valid), .count(cq_count)
    );

    wire [4:0] bw_sh = (apb_pwdata[4:0] < 5'd12) ? 5'd12 :
                       (apb_pwdata[4:0] > 5'd20) ? 5'd20 : apb_pwdata[4:0];   // BAND_BIN shift
    wire perf_clr = apb_wr & (apb_reg == 6'h02) & apb_pwdata[0];
    wire err_clr  = apb_wr & (apb_reg == 6'h02) & apb_pwdata[1];

    always @(posedge clk) begin
        if (rst) begin
            r_dst <= 32'd0;  r_src <= 32'd0;  r_aad <= 32'd0;
            r_dstr <= 16'd1280; r_sstr <= 16'd0; r_astr <= 16'd0;
            r_color <= 16'd0; r_key <= 16'hF81F;
            r_w <= 11'd0; r_h <= 11'd0;
            r_rdmax <= 5'd16; r_wrmax <= 8'd128; r_rdwmax <= 11'd1024;
            x_off <= 1'b0; x_wth <= 4'd0; x_minw8 <= 8'd16; x_rdwmax <= 11'd256;
            bg_s <= 16'd1280; bg_nb <= 6'd30; bg_koff <= 1'b0; bg_recip <= 21'd209716;
            bg_bin <= 32'h03000000; bg_sh <= 5'd18;
            bg_binsz <= 32'h00040000; bg_capm <= 16'hE000;
        end else if (apb_wr) begin
            case (apb_reg)
                6'h25: begin
                    bg_s    <= ({4'd0, apb_pwdata[15:4]} > (WIN_BYTES / 16)) ? WIN_BYTES :
                               {apb_pwdata[15:4], 4'd0};
                    bg_nb   <= (apb_pwdata[21:16] == 6'd0) ? 6'd1 :
                               (apb_pwdata[21:16] > MAXB) ? MAXB : apb_pwdata[21:16];
                    bg_koff <= apb_pwdata[31];
                end
                6'h26: bg_recip <= apb_pwdata[20:0];
                6'h27: begin
                    bg_bin   <= {apb_pwdata[31:12], 12'd0};
                    bg_sh    <= bw_sh;
                    bg_binsz <= 32'd1 << bw_sh;
                    bg_capm  <= 16'hFFFF << (bw_sh - 5'd5);   // 2^(sh - 5) records of 32 bytes
                end
                6'h21: begin
                    x_off    <= apb_pwdata[0];
                    x_wth    <= apb_pwdata[7:4];
                    x_minw8  <= apb_pwdata[15:8];
                    x_rdwmax <= (apb_pwdata[26:16] == 11'd0) ? 11'd1 :
                                (apb_pwdata[26:16] > 11'd1024) ? 11'd1024 : apb_pwdata[26:16];
                end
                6'h15: begin
                    r_rdmax <= (apb_pwdata[4:0] == 5'd0) ? 5'd1 :
                               (apb_pwdata[4:0] > 5'd16) ? 5'd16 : apb_pwdata[4:0];
                    r_wrmax <= (apb_pwdata[15:8] == 8'd0) ? 8'd1 :
                               (apb_pwdata[15:8] > 8'd128) ? 8'd128 : apb_pwdata[15:8];
                    r_rdwmax <= (apb_pwdata[26:16] == 11'd0) ? 11'd1 :
                                (apb_pwdata[26:16] > 11'd1024) ? 11'd1024 : apb_pwdata[26:16];
                end
                6'h04: r_dst   <= apb_pwdata;
                6'h05: r_dstr  <= apb_pwdata[15:0];
                6'h06: r_src   <= apb_pwdata;
                6'h07: r_sstr  <= apb_pwdata[15:0];
                6'h08: r_aad   <= apb_pwdata;
                6'h09: r_astr  <= apb_pwdata[15:0];
                6'h0A: begin r_w <= apb_pwdata[10:0]; r_h <= apb_pwdata[26:16]; end
                6'h0B: r_color <= apb_pwdata[15:0];
                6'h0C: r_key   <= apb_pwdata[15:0];
                default: ;
            endcase
        end
    end

    always @(*) begin
        case (apb_reg)
            6'h00:   apb_prdata = 32'h424C5431;
            6'h01:   apb_prdata = {{(15-CQ_AW){1'b0}}, cq_space, 8'd0, berr, bovf, bc_act, lerr,
                                   list_active, err, cq_full, busy};
            6'h03:   apb_prdata = CAPS_VAL;
            6'h04:   apb_prdata = r_dst;
            6'h05:   apb_prdata = {16'd0, r_dstr};
            6'h06:   apb_prdata = r_src;
            6'h07:   apb_prdata = {16'd0, r_sstr};
            6'h08:   apb_prdata = r_aad;
            6'h09:   apb_prdata = {16'd0, r_astr};
            6'h0A:   apb_prdata = {5'd0, r_h, 5'd0, r_w};
            6'h0B:   apb_prdata = {16'd0, r_color};
            6'h0C:   apb_prdata = {16'd0, r_key};
            6'h0E:   apb_prdata = {(tb_in | bc_call) ? l_ret : l_ptr, 2'b00};
            6'h0F:   apb_prdata = {(tb_in | bc_call) ? m_end : l_end, 2'b00};
            6'h10:   apb_prdata = perf_busy;
            6'h11:   apb_prdata = perf_cmds;
            6'h12:   apb_prdata = perf_rbeat;
            6'h13:   apb_prdata = perf_wbeat;
            6'h14:   apb_prdata = cmd_pushed;
            6'h15:   apb_prdata = {5'd0, r_rdwmax, r_wrmax, 3'd0, r_rdmax};
            6'h17:   apb_prdata = perf_chit;
            6'h18:   apb_prdata = perf_cmiss;
            6'h19:   apb_prdata = perf_wstall;
            6'h1A:   apb_prdata = perf_awwait;
            6'h1B:   apb_prdata = perf_awstall;
            6'h1C:   apb_prdata = perf_arstall;
            6'h1D:   apb_prdata = perf_pxwait;
            6'h1E:   apb_prdata = perf_pxfull;
            6'h1F:   apb_prdata = perf_pxbub;
            6'h20:   apb_prdata = perf_pxidle;
            6'h21:   apb_prdata = {5'd0, x_rdwmax, x_minw8, x_wth, 3'd0, x_off};
            6'h22:   apb_prdata = perf_wclk;
            6'h23:   apb_prdata = perf_xwbeat;
            6'h24:   apb_prdata = perf_xrbeat;
            6'h25:   apb_prdata = {bg_koff, 9'd0, bg_nb, bg_s};
            6'h26:   apb_prdata = {11'd0, bg_recip};
            6'h27:   apb_prdata = {bg_bin[31:12], 7'd0, bg_sh};
            6'h28:   apb_prdata = bc_frames;
            6'h29:   apb_prdata = perf_p1;
            6'h2A:   apb_prdata = perf_flush;
            6'h2B:   apb_prdata = perf_kbeat;
            6'h2C:   apb_prdata = perf_recs;
            6'h2D:   apb_prdata = perf_band;
            6'h2E:   apb_prdata = perf_kclk;
            6'h2F:   apb_prdata = CAPS2_VAL;         // CAPS2: [0] add [1] DBG0..3 [2] 2560 [15:8] bands
            6'h30:   apb_prdata = dbg0;
            6'h31:   apb_prdata = dbg1;
            6'h32:   apb_prdata = dbg2;
            6'h33:   apb_prdata = dbg3;
            default: apb_prdata = 32'd0;
        endcase
    end

    // queue head fields
    wire [31:0] q_dst   = cq_dout[31:0];
    wire [15:0] q_dstr  = cq_dout[47:32];
    wire [31:0] q_src   = cq_dout[79:48];
    wire [15:0] q_sstr  = cq_dout[95:80];
    wire [31:0] q_aad   = cq_dout[127:96];
    wire [15:0] q_astr  = cq_dout[143:128];
    wire [10:0] q_w     = cq_dout[154:144];
    wire [10:0] q_h     = cq_dout[165:155];
    wire [15:0] q_color = cq_dout[181:166];
    wire [15:0] q_key   = cq_dout[197:182];
    wire        q_op    = cq_dout[198];
    wire        q_keyen = cq_dout[199];
    wire        q_apl   = cq_dout[200] & cq_dout[198];
    wire        q_sync  = cq_dout[201];
    wire [7:0]  q_ga    = cq_dout[209:202];
    wire        q_cache = cq_dout[210] & cq_dout[198];   // only COPY reads a source
    wire        q_inv   = cq_dout[211];
    wire        q_add   = cq_dout[212];                  // additive: dst + src * alpha, saturated
    wire        q_blnd0 = (q_ga != 8'hFF) | q_apl;          // needs a multiply on the source
    wire        q_blend = q_blnd0 | q_add;                  // reads the destination
    wire        q_nop   = (q_w == 11'd0) | (q_h == 11'd0);
    // D: K path (band mode only) = opaque or keyed COPY through the source cache, or
    // opaque FILL: 8 px per clock straight into the band window
    wire        q_k     = bm & ~bg_koff & ~q_nop & ~q_blnd0 & (~q_op | q_cache);   // opaque / keyed / pure add
    // C2: opaque, phase-aligned COPY or opaque FILL, wide enough -> wide path
    wire        q_wide0 = ~x_off & ~q_nop & ~q_blend & ~q_keyen & (q_w >= {x_minw8, 3'd0}) &
                          (~q_op | ((q_dst[3:0] == q_src[3:0]) & (q_dstr[3:0] == q_sstr[3:0])));
    // band mode: COPY only (R beats go straight into the window); phase 1: never
    // (x_* carries the bin writes then)
    wire        q_wide  = q_wide0 & ~q_k & (bm ? q_op : ~bc_p1);

    // =========================================================================
    // Hand-off FIFOs
    // =========================================================================
    // h1 : fetch -> pixel  {galpha, blend, apl, keyen, op, key, color,
    //                       astr[1:0], aad[1:0], sstr[1:0], src[1:0],
    //                       h, w, dstr, dst}
    localparam H1_W = 1 + 8 + 4 + 16 + 16 + 8 + 11 + 11 + 16 + 32;   // 123 (+ add)
    wire [H1_W-1:0] h1_din = {q_add, q_ga, q_blend, q_apl, q_keyen, q_op, q_key, q_color,
                              q_astr[1:0], q_aad[1:0], q_sstr[1:0], q_src[1:0],
                              q_h, q_w, q_dstr, q_dst};
    wire [H1_W-1:0] h1_dout;
    wire            h1_valid, h1_full, h1_push, h1_pop;
    wire [5:0]      h1_count;

    blit_rfifo #(.DW(H1_W), .AW(4)) u_h1 (
        .clk(clk), .rst(rst),
        .push(h1_push), .din(h1_din), .full(h1_full), .space(),
        .pop(h1_pop), .dout(h1_dout), .valid(h1_valid), .count(h1_count)
    );

    // h2 : pixel -> write  {h, w, dstr, dst}
    localparam H2_W = 11 + 11 + 16 + 32;                         // 70
    wire [H2_W-1:0] h2_din = h1_dout[H2_W-1:0];
    wire [H2_W-1:0] h2_dout;
    wire            h2_valid, h2_full, h2_push, h2_pop;
    wire [5:0]      h2_count;

    blit_rfifo #(.DW(H2_W), .AW(4)) u_h2 (
        .clk(clk), .rst(rst),
        .push(h2_push), .din(h2_din), .full(h2_full), .space(),
        .pop(h2_pop), .dout(h2_dout), .valid(h2_valid), .count(h2_count)
    );

    // =========================================================================
    // FETCH : row / stream / burst sequencer on AR
    // =========================================================================
    localparam F_IDLE = 2'd0, F_SETUP = 2'd1, F_ISSUE = 2'd2, F_WIDE = 2'd3;
    localparam S_D = 2'd0, S_A = 2'd1, S_S = 2'd2, S_L = 2'd3;   // S_L: command list words

    reg  [1:0]  f_state;
    reg  [1:0]  f_stream;
    reg  [31:0] f_drow, f_srow, f_arow;
    reg  [15:0] f_dstr, f_sstr, f_astr;
    reg  [10:0] f_w, f_rows;
    reg         f_need_d, f_need_a, f_need_s;
    reg  [29:0] f_addr;                 // word address of the next burst
    reg  [11:0] f_left;                 // words left in this stream-row
    reg  [RW:0] f_alloc_d, f_alloc_a, f_alloc_s;

    // pixel-stage ring read pointers (credit return), declared here
    reg  [RW:0] c_rd_d, c_rd_a, c_rd_s;

    // read descriptor FIFO: one entry per AXI read burst or cache hit, in issue
    // order; the R router works through it in the same order.
    //   {line[8:0], len[6:0], off[2:0], fill, hit, stream[1:0]}
    //   len  = words that go to the ring (list: burst length)
    //   fill = AXI reads the whole 8-word cache line 'line'; all 8 words are
    //          written into the cache, words [off, off+len) also go to the ring
    //   hit  = no AXI read: words [off, off+len) of cache line 'line'
    localparam BD_W = 23;
    wire [BD_W-1:0] bd_din, bd_q;
    wire            bd_push, bd_pop, bd_full, bd_v;
    wire [5:0]      bd_count_small;
    wire [7:0]      bd_count = {2'd0, bd_count_small};

    // R5: descriptors are shallow control state, not pixel storage. Keeping
    // 16 entries in FFs replaces two underfilled Ti60 RAM10 blocks. All
    // producers already observe bd_full, so reduced depth only adds stalls.
    blit_rfifo #(.DW(BD_W), .AW(4)) u_bd (
        .clk(clk), .rst(rst),
        .push(bd_push), .din(bd_din), .full(bd_full), .space(),
        .pop(bd_pop), .dout(bd_q), .valid(bd_v), .count(bd_count_small)
    );

    reg  [4:0]  ax_out;                 // AXI read bursts issued, last beat not back yet
    reg  [9:0]  rf_res;                 // R FIFO words reserved (issued, not yet routed)
    // The issue increment is already registered in rdi_m for rd_words.
    // Include that pending increment in credit checks to preserve exactly
    // the original reservation semantics while shortening the issue path.
    wire [9:0]  rf_eff = rf_res + {3'd0, rdi_m};
    wire        rf_ok = (rf_eff <= 10'd448);   // room for one more 64-word burst
    wire        rf_pop;                 // router takes a word from the R FIFO

    // source cache (16 KB): tags in block RAM, valid bits in flops
    reg  [511:0] c_valid;
    reg          f_cache;               // this command reads its source through the cache
    reg  [8:0]   c_qidx;                // line index the tag RAM output belongs to
    reg          c_vq, c_qok;
    wire [17:0]  c_tq;

    wire        drained;

    // row geometry
    wire        f_dpar = f_drow[1];
    wire [11:0] f_nout = ({1'b0, f_w} + {11'd0, f_dpar} + 12'd1) >> 1;
    wire [31:0] f_va   = f_arow - {31'd0, f_dpar};
    wire [12:0] f_na_t = {11'd0, f_va[1:0]} + {f_nout, 1'b0} - 13'd1;
    wire [11:0] f_na   = {1'b0, f_na_t[12:2]} + 12'd1;
    wire [31:0] f_vs   = f_srow - {30'd0, f_dpar, 1'b0};
    wire [11:0] f_ns   = f_nout + {11'd0, f_vs[1]};

    // burst length: min(words left, words to the next 256-byte boundary);
    // cached source rows stop at the next 32-byte cache line instead
    wire        f_cs     = f_cache & (f_stream == S_S);
    wire [6:0]  f_to_bnd = f_cs ? (7'd8 - {4'd0, f_addr[2:0]}) : (7'd64 - {1'b0, f_addr[5:0]});
    wire [6:0]  f_len    = (f_left < {5'd0, f_to_bnd}) ? f_left[6:0] : f_to_bnd;

    // D: K path signals used by FETCH (logic in the "K path" section)
    wire          kb_fill_raw, kb_fill; // K walker: line miss -> fill (AR of one cache line)
    wire [26:0]   kb_lw;                // that line (byte address / 32)
    wire          k_act;                // K walker owns the tag RAM read port
    wire [8:0]    k_ra;                 // its tag RAM read index

    // cache lookup: the tag RAM is read one clock ahead (in F_SETUP for the
    // row start, then at f_addr while waiting in F_ISSUE). A lookup in the same
    // clock as a tag write is thrown away (c_qok) and simply repeated.
    wire [8:0]  c_ra   = k_act ? k_ra : ((f_state == F_SETUP) ? f_vs[13:5] : f_addr[11:3]);
    wire        c_look = c_qok & (c_qidx == f_addr[11:3]);
    wire        c_hit  = c_look & c_vq & (c_tq == f_addr[29:12]);

    reg  [RW:0] f_alloc_sel, f_rd_sel;
    always @(*) begin
        case (f_stream)
            S_D:     begin f_alloc_sel = f_alloc_d; f_rd_sel = c_rd_d; end
            S_A:     begin f_alloc_sel = f_alloc_a; f_rd_sel = c_rd_a; end
            default: begin f_alloc_sel = f_alloc_s; f_rd_sel = c_rd_s; end
        endcase
    end
    wire [RW:0]   f_used   = f_alloc_sel - f_rd_sel;
    wire [RW+1:0] f_need   = {1'b0, f_used} + {{(RW-5){1'b0}}, f_len};
    wire          f_credit = (f_need <= RING_WORDS);

    // C2 wide path (logic in the "Wide path" section)
    reg  [6:0]    fl_cnt;               // cache line fills issued, not yet written
    reg           x_mode;               // = (f_state == F_WIDE)
    wire          x_start, x_done;
    wire          xr_iss;               // wide AR issued
    wire [4:0]    xr_len;               // its length in beats
    reg           xr_v;                 // wide R beat (registered) -> buffer
    reg  [127:0]  xr_d;
    reg  [RW:0]   xb_wp, xb_rp;         // buffer pointers (beats)
    wire [RW:0]   xb_rpn;
    reg  [7:0]    bcount;               // AWs issued, B not yet received (WRITE)
    wire          bn_awi, fu_awi;       // D: AW of the binner / the band flush (bcount)

    // D: path order inside a band. A command starts on the K, wide or narrow path
    // only when the other paths have written everything into the window, so
    // overlapping draws keep the command order; blends read the window.
    wire n_idle;                        // narrow pipeline empty (PIXEL .. WRITE)
    wire k_idle;                        // K path empty (walker, row queue, compose)
    wire kw_free;                       // K walker takes a new command
    // Timing (2026-10-11): the start decision uses only registers.
    //  * the "pipeline empty" flags are registered one clock late and cleared in
    //    the clock anything enters (conservative: a path is never seen idle while
    //    it is not; it may be seen busy one clock longer);
    //  * the command class (K / wide / no-op / sync-or-blend) is worked out one
    //    clock after a new queue head appears (cl_*), and used only while the
    //    head and the mode it depends on are unchanged (cl_v).
    reg  n_idle_q, k_idle_q, drained_q, flz_q;
    reg  cl_k, cl_wide, cl_nop, cl_sb, cl_ok;
    reg  [2:0] cl_bc;                   // bc_state the class was worked out for
    wire cl_v      = cl_ok & (cl_bc == bc_state);
    wire b_drained = n_idle_q & k_idle_q;
    wire f_gate    = ~bc_act | bc_p1 | bm;   // nothing starts in clear / flush / finish
    wire f_can_start = cq_valid & cl_v & f_gate &
                       (cl_k    ? (kw_free & n_idle_q & (~q_inv | k_idle_q)) :
                        cl_wide ? (bm ? b_drained : (~h1_full & drained_q & flz_q)) :
                        bm      ? (~h1_full & k_idle_q & (~cl_sb | b_drained)) :
                                  (~h1_full & (cl_nop | ~cl_sb | drained_q)));
    wire f_start     = (f_state == F_IDLE) & f_can_start;
    assign cq_pop    = f_start;
    assign h1_push   = f_start & ~cl_nop & ~cl_wide & ~cl_k;
    assign x_start   = f_start & cl_wide;
    wire   k_start   = f_start & cl_k;

    // The list fetcher shares AR. It goes first when its FIFO has run dry,
    // otherwise it takes the cycles the row fetcher does not use.
    // read words in flight = rd_words + the increment of the last clock that is still
    // in rdi_* (registered for timing, see the rd_words update). AR issuers are mutually
    // exclusive (never two in one clock), so OR = sum: rd_eff equals the old rd_words
    // exactly, cycle for cycle.
    wire [6:0]  rd_pend  = rdi_m | {rdi_x, 2'b00};
    wire [10:0] rd_eff   = rd_words + {4'd0, rd_pend};
    wire rd_room     = (ax_out < r_rdmax) &     // LIMIT: outstanding read bursts
                       (rd_eff < r_rdwmax);     //        and read words in flight
    wire f_axi_ok    = ~m_arvalid & rd_room & rf_ok;
    wire f_issue_raw = (f_state == F_ISSUE) & f_credit & ~bd_full &
                       (f_cs ? (c_look & (c_hit | f_axi_ok)) : f_axi_ok);
    wire l_urgent    = (lf_inflight == 8'd0);
    wire l_issue     = l_len_v & ~m_arvalid & ~bd_full & rd_room & rf_ok &
                       (l_urgent | ~(f_issue_raw | kb_fill_raw));
    wire f_issue     = f_issue_raw & ~l_issue;
    wire f_chit      = f_issue & f_cs & c_hit;  // served by the cache, no AR
    wire f_fill      = f_issue & f_cs & ~c_hit; // miss: read the whole line
    wire f_ar        = f_issue & ~f_chit;
    wire [6:0] f_arw = f_cs ? 7'd8 : f_len;     // words read on AXI
    wire f_last_bst  = ({5'd0, f_len} == f_left);

    assign bd_push = f_issue | l_issue | kb_fill;   // never two in one clock
    // K fill: the whole line goes into the cache only (ring length 0)
    assign bd_din  = l_issue ? {9'd0, l_len, 3'd0, 1'b0, 1'b0, S_L} :
                     kb_fill ? {kb_lw[8:0], 7'd0, 3'd0, 1'b1, 1'b0, S_S}
                             : {f_addr[11:3], f_len, f_addr[2:0], f_fill, f_chit, f_stream};

    // next stream after the current one in this row (3 = row finished)
    reg  [1:0] f_next;
    always @(*) begin
        case (f_stream)
            S_D:     f_next = f_need_a ? S_A : (f_need_s ? S_S : 2'd3);
            S_A:     f_next = f_need_s ? S_S : 2'd3;
            default: f_next = 2'd3;
        endcase
    end
    wire [1:0] q_first = (q_blend & ~bm) ? S_D : (q_apl ? S_A : S_S);
    wire [1:0] f_first = f_need_d ? S_D : (f_need_a ? S_A : S_S);

    always @(posedge clk) begin
        if (rst) begin
            f_state   <= F_IDLE;
            f_stream  <= S_D;
            m_arvalid <= 1'b0;
            m_araddr  <= 32'd0;
            m_arlen   <= 8'd0;
            f_alloc_d <= {(RW+1){1'b0}};
            f_alloc_a <= {(RW+1){1'b0}};
            f_alloc_s <= {(RW+1){1'b0}};
            f_cache   <= 1'b0;
            f_drow <= 32'd0; f_srow <= 32'd0; f_arow <= 32'd0;
            f_dstr <= 16'd0; f_sstr <= 16'd0; f_astr <= 16'd0;
            f_w <= 11'd0; f_rows <= 11'd0;
            f_need_d <= 1'b0; f_need_a <= 1'b0; f_need_s <= 1'b0;
            f_addr <= 30'd0; f_left <= 12'd0;
        end else begin
            if (m_arvalid & m_arready)
                m_arvalid <= 1'b0;

            case (f_state)
                F_IDLE: begin
                    if (f_start & ~cl_nop & ~cl_k) begin   // K commands: K walker
                        f_drow   <= q_dst;   f_dstr <= q_dstr;
                        f_srow   <= q_src;   f_sstr <= q_sstr;
                        f_arow   <= q_aad;   f_astr <= q_astr;
                        f_w      <= q_w;     f_rows <= q_h;
                        f_need_d <= q_blend & ~bm;   // band mode: destination from the window
                        f_need_a <= q_apl;
                        f_need_s <= q_op;
                        f_cache  <= q_cache;
                        f_stream <= q_first;
                        if (cl_wide)
                            f_state <= F_WIDE;       // C2: x_* path until x_done
                        else if (q_op | (q_blend & ~bm))
                            f_state <= F_SETUP;      // plain fills read nothing
                    end
                end

                F_SETUP: begin
                    case (f_stream)
                        S_D:     begin f_addr <= f_drow[31:2]; f_left <= f_nout; end
                        S_A:     begin f_addr <= f_va[31:2];   f_left <= f_na;   end
                        default: begin f_addr <= f_vs[31:2];   f_left <= f_ns;   end
                    endcase
                    f_state <= F_ISSUE;
                end

                F_ISSUE: begin
                    if (f_issue) begin
                        if (f_ar) begin
                            m_arvalid <= 1'b1;
                            m_araddr  <= f_cs ? {f_addr[29:3], 5'd0} : {f_addr, 2'b00};
                            m_arlen   <= {1'b0, f_arw} - 8'd1;
                        end
                        case (f_stream)
                            S_D:     f_alloc_d <= f_alloc_d + {{(RW-6){1'b0}}, f_len};
                            S_A:     f_alloc_a <= f_alloc_a + {{(RW-6){1'b0}}, f_len};
                            default: f_alloc_s <= f_alloc_s + {{(RW-6){1'b0}}, f_len};
                        endcase
                        f_addr <= f_addr + {23'd0, f_len};
                        f_left <= f_left - {5'd0, f_len};
                        if (f_last_bst) begin
                            if (f_next != 2'd3) begin
                                f_stream <= f_next;
                                f_state  <= F_SETUP;
                            end else if (f_rows == 11'd1) begin
                                f_state  <= F_IDLE;
                            end else begin
                                f_rows   <= f_rows - 11'd1;
                                f_drow   <= f_drow + {16'd0, f_dstr};
                                f_srow   <= f_srow + {16'd0, f_sstr};
                                f_arow   <= f_arow + {16'd0, f_astr};
                                f_stream <= f_first;
                                f_state  <= F_SETUP;
                            end
                        end
                    end
                end

                default: begin                       // F_WIDE
                    if (x_done)
                        f_state <= F_IDLE;
                end
            endcase

            if (l_issue) begin                       // never together with f_issue
                m_arvalid <= 1'b1;
                m_araddr  <= {l_ptr, 2'b00};
                m_arlen   <= {1'b0, l_len} - 8'd1;
            end
            if (kb_fill) begin                       // K walker line fill (FETCH is idle)
                m_arvalid <= 1'b1;
                m_araddr  <= {kb_lw, 5'd0};
                m_arlen   <= 8'd7;
            end
        end
    end

    // read words in flight (LIMIT[26:16]), AXI bursts in flight (LIMIT[4:0]),
    // R FIFO reservation; m_rready is always 1
    always @(posedge clk) begin
        if (rst) begin
            rd_words <= 11'd0;
            rdi_m <= 7'd0; rdi_x <= 5'd0;
            ax_out   <= 5'd0;
            rf_res   <= 10'd0;
        end else begin
            // timing (2026-10-19, core_clk -0.129 ns after the audio was added): the
            // increments come at the end of the long tag RAM -> cache hit -> f_ar / l_issue
            // chain; they are registered first (rdi_*) and added one clock later, so that
            // chain ends in a single gate instead of six adders. rd_room / xr_iss look at
            // rd_eff = rd_words + rdi_*, which is exactly the old rd_words.
            // f_ar / l_issue / kb_fill never in one clock (bd_push), so the m port
            // increment is one AND-OR into one register (rdi_m).
            // 2026-10-20 BOARD BUG: with the K fill kept as its own 1-bit register and
            // added as {7'd0, rdi_k, 3'd0}, Efinity synthesis (2026.1) built the sum
            // at bit 2 and fed rdi_k into that adder's carry-in, i.e. +4 instead of
            // +8 per K fill (netlist: u_blit/add_564/i1__AUX_ADD_CI). rd_words went
            // below 0 after a few K fills -> rd_room = 0 forever (band K path hang,
            // DBG0 showed rd_words 1988 = -60 = 15 fills x 4). RTL sim is correct;
            // only the netlist is wrong. Do not add 1-bit terms shifted into an
            // adder whose other terms start at a lower bit.
            rdi_m <= ({7{f_ar}} & f_arw) | ({7{l_issue}} & l_len) | {3'd0, kb_fill, 3'd0};
            rdi_x <= xr_iss ? xr_len : 5'd0;
            rd_words <= rd_words + {4'd0, rdi_m}
                                 + {4'd0, rdi_x, 2'b00}                         // C2: 4 words/beat
                                 - {10'd0, m_rvalid} - {8'd0, x_rvalid, 2'b00};
            ax_out   <= ax_out + {4'd0, f_ar | l_issue | kb_fill} + {4'd0, xr_iss}
                               - {4'd0, m_rvalid & m_rlast} - {4'd0, x_rvalid & x_rlast};
            // Same mutually-exclusive issue increment as rd_words. Do not
            // reintroduce shifted single-bit additions (see Efinity note).
            rf_res   <= rf_res + {3'd0, rdi_m} - {9'd0, rf_pop};
        end
    end

    // cache tags / valid bits; INV takes effect when its command leaves the queue
    wire        c_we  = f_fill | kb_fill;       // never both (FETCH idle while K walks)
    wire [8:0]  c_wa  = kb_fill ? kb_lw[8:0]  : f_addr[11:3];
    wire [17:0] c_wd  = kb_fill ? kb_lw[26:9] : f_addr[29:12];
    // Timing (2026-10-11): the tag write and the valid bit of a line fill land one
    // clock after the fill is issued (c_we_q). A lookup whose tag/valid read falls
    // into the fill clock or the clock after is thrown away and repeated (c_qok),
    // so no lookup ever sees a line half updated.
    reg         c_we_q;
    reg  [8:0]  c_wa_q;
    reg  [17:0] c_wd_q;
    blit_ram #(.DW(18), .AW(9)) u_ctag (.clk(clk), .we(c_we_q), .wa(c_wa_q), .wd(c_wd_q),
                                        .ra(c_ra), .rd(c_tq));

    always @(posedge clk) begin
        if (rst) begin
            c_valid <= {512{1'b0}};
            c_qidx  <= 9'd0;
            c_vq    <= 1'b0;
            c_qok   <= 1'b0;
            c_we_q  <= 1'b0;
            c_wa_q  <= 9'd0;
            c_wd_q  <= 18'd0;
        end else begin
            c_we_q <= c_we;
            c_wa_q <= c_wa;
            c_wd_q <= c_wd;
            // an INV / wide start clears after the fills issued before it (same
            // order as when the valid bit was set in the issue clock)
            if (f_start & q_inv)
                c_valid <= {512{1'b0}};
            else if (x_start & ~bm)              // C2 buffer overwrites lines 0..127
                c_valid <= c_valid & ({512{1'b1}} << (RING_WORDS >> 3));
            else if (c_we_q)                     // valid from (issue + 1) on: the router
                c_valid <= c_valid | ({{511{1'b0}}, 1'b1} << c_wa_q);  // fills in order
            c_qidx <= c_ra;
            c_vq   <= c_valid[c_ra];
            c_qok  <= ~c_we & ~c_we_q;
        end
    end

    // =========================================================================
    // R router : R FIFO / source cache -> line rings, strictly in issue order
    // =========================================================================
    wire [31:0] rf_q;
    wire        rf_v;

    blit_fifo #(.DW(32), .AW(9)) u_rf (            // every AXI read beat lands here
        .clk(clk), .rst(rst),
        .push(m_rvalid), .din(m_rdata), .full(), .space(),
        .pop(rf_pop), .dout(rf_q), .valid(rf_v), .count()
    );

    wire [1:0]  t_strm = bd_q[1:0];
    wire        t_hit  = bd_q[2];
    wire        t_fill = bd_q[3];
    wire [2:0]  t_off  = bd_q[6:4];
    wire [6:0]  t_len  = bd_q[13:7];
    wire [8:0]  t_line = bd_q[22:14];
    reg  [6:0]  t_cnt;                  // words of the head descriptor done so far
    wire [6:0]  t_n    = t_fill ? 7'd8 : t_len;
    wire        t_step = bd_v & (t_hit | rf_v);
    wire [3:0]  t_end  = {1'b0, t_off} + t_len[3:0];          // hit / fill: <= 8
    wire        t_del  = ~t_fill | (({1'b0, t_cnt[2:0]} >= {1'b0, t_off}) &
                                    ({1'b0, t_cnt[2:0]} < t_end));
    wire [2:0]  t_word = t_off + t_cnt[2:0];
    assign      bd_pop = t_step & (t_cnt == t_n - 7'd1);
    assign      rf_pop = t_step & ~t_hit;

    // Source cache data: 8 RAMs of 512 x 32 (D). Word w of line L sits in RAM w at
    // address L, so RAMs 0..3 hold the even 16-byte beat of every line and RAMs
    // 4..7 the odd one: the K path reads two neighbouring beats per clock.
    // C2 (direct mode): lane 3 [127:96] of the wide beat buffer (word index = beat).
    wire        xbuf  = x_mode & ~bm;           // band mode: the wide path needs no buffer
    wire [11:0] cd_wa = xbuf ? {{(12-RW){1'b0}}, xb_wp[RW-1:0]}  : {t_line, t_cnt[2:0]};
    wire [11:0] cd_ra = xbuf ? {{(12-RW){1'b0}}, xb_rp[RW-1:0]} : {t_line, t_word};
    wire        cd_we = xbuf ? xr_v : (t_step & t_fill);
    wire [31:0] cd_wd = xbuf ? xr_d[127:96] : rf_q;
    wire        k_rd;                           // K compose reads a beat pair (K section)
    wire [8:0]  k_ea, k_oa;                     // K: line of the even / odd beat
    wire [8:0]  cd_rae = k_rd ? k_ea : cd_ra[11:3];
    wire [8:0]  cd_rao = k_rd ? k_oa : cd_ra[11:3];
    wire [31:0] cd0_q, cd1_q, cd2_q, cd3_q, cd4_q, cd5_q, cd6_q, cd7_q;
    blit_ram #(.DW(32), .AW(9)) u_cd0 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd0)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rae), .rd(cd0_q));
    blit_ram #(.DW(32), .AW(9)) u_cd1 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd1)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rae), .rd(cd1_q));
    blit_ram #(.DW(32), .AW(9)) u_cd2 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd2)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rae), .rd(cd2_q));
    blit_ram #(.DW(32), .AW(9)) u_cd3 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd3)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rae), .rd(cd3_q));
    blit_ram #(.DW(32), .AW(9)) u_cd4 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd4)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rao), .rd(cd4_q));
    blit_ram #(.DW(32), .AW(9)) u_cd5 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd5)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rao), .rd(cd5_q));
    blit_ram #(.DW(32), .AW(9)) u_cd6 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd6)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rao), .rd(cd6_q));
    blit_ram #(.DW(32), .AW(9)) u_cd7 (.clk(clk), .we(cd_we & (cd_wa[2:0] == 3'd7)), .wa(cd_wa[11:3]),
                                       .wd(cd_wd), .ra(cd_rao), .rd(cd7_q));
    reg  [2:0]  cd_rsel;                        // word of the 32-bit read (output one clock late)
    always @(posedge clk) cd_rsel <= cd_ra[2:0];
    wire [31:0] cd_q = (cd_rsel == 3'd0) ? cd0_q : (cd_rsel == 3'd1) ? cd1_q :
                       (cd_rsel == 3'd2) ? cd2_q : (cd_rsel == 3'd3) ? cd3_q :
                       (cd_rsel == 3'd4) ? cd4_q : (cd_rsel == 3'd5) ? cd5_q :
                       (cd_rsel == 3'd6) ? cd6_q : cd7_q;

    reg         r_v, r_hc;
    reg  [1:0]  r_s;
    reg  [31:0] r_dx;
    reg  [RW:0] rx_d, rx_a, rx_s;
    wire [31:0] r_d = r_hc ? cd_q : r_dx;           // cache RAM output is one clock late

    always @(posedge clk) begin
        if (rst) begin
            r_v   <= 1'b0;
            r_hc  <= 1'b0;
            r_s   <= 2'd0;
            r_dx  <= 32'd0;
            t_cnt <= 7'd0;
            rx_d  <= {(RW+1){1'b0}};
            rx_a  <= {(RW+1){1'b0}};
            rx_s  <= {(RW+1){1'b0}};
        end else begin
            r_v  <= t_step & t_del;
            r_hc <= t_hit;
            r_s  <= t_strm;
            r_dx <= rf_q;
            if (t_step)
                t_cnt <= bd_pop ? 7'd0 : t_cnt + 7'd1;
            if (r_v) begin
                case (r_s)
                    S_D:     rx_d <= rx_d + {{RW{1'b0}}, 1'b1};
                    S_A:     rx_a <= rx_a + {{RW{1'b0}}, 1'b1};
                    S_S:     rx_s <= rx_s + {{RW{1'b0}}, 1'b1};
                    default: ;                       // S_L: list FIFO
                endcase
            end
        end
    end

    wire we_d = r_v & (r_s == S_D);
    wire we_a = r_v & (r_s == S_A);
    wire we_s = r_v & (r_s == S_S);
    wire we_l = r_v & (r_s == S_L);

    // ring read addresses (driven by the pixel stage)
    wire [RW-1:0] ra_d, ra_s0, ra_s1, ra_a0, ra_a1, ra_a2, ra_a3;
    wire [31:0]   q_dw;
    wire [15:0]   q_s0, q_s1;
    wire [7:0]    q_a0, q_a1, q_a2, q_a3;

    // =========================================================================
    // Wide path (C2): one 128-bit beat (8 px) per clock on x_*.
    // Three row sequencers walk the same rows: xr = source AR (COPY only),
    // xa = destination AW, xw = W beats. All rows of a command have the same
    // number of beats per row for source and destination (equal phase), and
    // bursts stop at 256-byte blocks on both sides, so the R beats in arrival
    // order are exactly the W beats in order: the buffer is a plain FIFO.
    // Buffer: 2^RW x 128 bits in the line rings + cache data RAM (lane 0 ring_d,
    // lane 1 s1:s0, lane 2 a3..a0, lane 3 cdata). Space is reserved per AR
    // (xres), an AW is only issued once its beats are buffered (xb_unc), so R
    // is never held and W never waits for R.
    // =========================================================================
    reg  [10:0]  x_w;
    reg  [15:0]  x_color, x_dstr, x_sstr;
    reg          x_op, x_wph;
    reg  [27:0]  xr_a, xa_a;            // next burst, 16-byte (beat) address
    reg  [3:0]   xw_a;                  // W beat position in its 256-byte block
    reg  [8:0]   xr_left, xa_left, xw_left;   // beats left in the current row
    reg  [31:0]  xr_nrow, xa_nrow, xw_nrow;   // byte address of the next row
    reg  [10:0]  xr_rows, xa_rows, xw_rows;   // rows still to start
    reg          xw_first;              // W: first beat of a row
    reg  [2:0]   xw_lo, xw_hi;          // W: first / last pixel lane of the row
    reg  [9:0]   xr_pend;               // beats requested, not yet returned
    reg  [RW:0]  xres;                  // buffer beats reserved (AR issued, not popped)
    reg  [RW:0]  xb_unc;                // buffered beats not yet covered by an AW
    reg          xa_awv;                // AW of the wide path (x_* mux in "Band mode")
    reg  [31:0]  xa_awaddr;
    reg  [3:0]   xa_awlen;
    reg  [11:0]  xw_wb;                 // band mode: window beat of the next R beat

    // beats of a row starting at byte address a: (a[3:1] + w + 7) / 8;
    // last pixel lane of the row: (a[3:1] + w - 1) mod 8
    wire [11:0] xq_nbt = ({9'd0, q_dst[3:1]}   + {1'b0, q_w} + 12'd7) >> 3;
    wire [11:0] xq_hit =  {9'd0, q_dst[3:1]}   + {1'b0, q_w} - 12'd1;
    wire [11:0] xr_nbt = ({9'd0, xr_nrow[3:1]} + {1'b0, x_w} + 12'd7) >> 3;
    wire [11:0] xa_nbt = ({9'd0, xa_nrow[3:1]} + {1'b0, x_w} + 12'd7) >> 3;
    wire [11:0] xw_nbt = ({9'd0, xw_nrow[3:1]} + {1'b0, x_w} + 12'd7) >> 3;
    wire [11:0] xw_hit =  {9'd0, xw_nrow[3:1]} + {1'b0, x_w} - 12'd1;

    // ---- AR (COPY source)
    wire [4:0]  xr_tb   = 5'd16 - {1'b0, xr_a[3:0]};
    assign      xr_len  = (xr_left < {4'd0, xr_tb}) ? xr_left[4:0] : xr_tb;
    wire [4:0]  xr_lm1  = xr_len - 5'd1;
    wire        xr_rend = ({4'd0, xr_len} == xr_left);
    assign      xr_iss  = x_mode & x_op & (xr_left != 9'd0) & ~x_arvalid & ~l_issue &
                          (ax_out < r_rdmax) & (rd_eff < x_rdwmax) &
                          (bm | ({1'b0, xres} + {{(RW-3){1'b0}}, xr_len} <= RING_WORDS));
    wire        xr_fin  = (xr_left == 9'd0) & (xr_pend == 10'd0);

    // ---- AW (destination)
    wire [4:0]  xa_tb   = 5'd16 - {1'b0, xa_a[3:0]};
    wire [4:0]  xa_len  = (xa_left < {4'd0, xa_tb}) ? xa_left[4:0] : xa_tb;
    wire [4:0]  xa_lm1  = xa_len - 5'd1;
    wire        xa_rend = ({4'd0, xa_len} == xa_left);
    wire        xa_dav  = (xb_unc >= {{(RW-4){1'b0}}, xa_len});
    // batch threshold, or the buffer cannot take another full burst (no deadlock
    // when WTH x 64 is larger than the buffer)
    wire        xa_dok  = xa_dav & (x_wph | xr_fin |
                                    (xb_unc >= {{(RW-9){1'b0}}, x_wth, 6'd0}) |
                                    ({1'b0, xres} + 12'd16 > RING_WORDS));
    wire        xa_iss  = x_mode & (xa_left != 9'd0) & ~xa_awv & (bcount < r_wrmax) &
                          (~x_op | xa_dok);

    // ---- W (direct mode). Band mode (D): no AW / W at all, every R beat is
    // written straight into the window (xw walks the rows on the R beats).
    // Timing (2026-10-11): the buffer is read ahead into a small skid FIFO (u_xs),
    // so W ready (from the SoC, far away on the chip) no longer drives the buffer
    // RAM read address. A read is started while the skid has room for it:
    // count + read in flight <= 3 (the skid holds 4 + its output register, and
    // a push needs 2 clocks to reach the output): one beat per clock sustained.
    wire        xs_v;                   // skid head valid (= W data)
    wire [127:0] xs_q;
    wire [2:0]  xs_cnt;
    reg         xs_pend;                // buffer read started last clock: push now
    wire        xs_rd    = xbuf & x_op & (xb_wp != xb_rp) & ({1'b0, xs_cnt} + {3'd0, xs_pend} <= 4'd3);
    wire        xw_vi    = x_mode & ~bm & (xw_left != 9'd0) & (~x_op | xs_v);
    wire        xw_hs    = xw_vi & x_wready;
    wire        xw_adv   = bm ? (x_mode & xr_v) : xw_hs;
    wire        xb_pop   = xw_hs & x_op;
    assign      xb_rpn   = xb_rp + {{RW{1'b0}}, xs_rd};
    wire        xw_last  = (xw_left == 9'd1) | (xw_a == 4'hF);
    assign      x_done   = xw_adv & (xw_left == 9'd1) & (xw_rows == 11'd0);
    wire [7:0]  xw_pml   = xw_first ? (8'hFF << xw_lo) : 8'hFF;
    wire [7:0]  xw_pmh   = (xw_left == 9'd1) ? (8'hFF >> (3'd7 - xw_hi)) : 8'hFF;
    wire [7:0]  xw_pm    = xw_pml & xw_pmh;        // pixel lanes written
    wire [15:0] xw_strb  = {xw_pm[7], xw_pm[7], xw_pm[6], xw_pm[6], xw_pm[5], xw_pm[5],
                            xw_pm[4], xw_pm[4], xw_pm[3], xw_pm[3], xw_pm[2], xw_pm[2],
                            xw_pm[1], xw_pm[1], xw_pm[0], xw_pm[0]};
    wire [127:0] xb_q    = {cd_q, q_a3, q_a2, q_a1, q_a0, q_s1, q_s0, q_dw};
    wire [127:0] xw_data = x_op ? xs_q : {8{x_color}};

    blit_rfifo #(.DW(128), .AW(2)) u_xs (
        .clk(clk), .rst(rst),
        .push(xs_pend), .din(xb_q), .full(), .space(),
        .pop(xb_pop), .dout(xs_q), .valid(xs_v), .count(xs_cnt)
    );
    wire [31:0]  xq_wx   = q_dst - bc_wbase;       // band mode: window offset of the first row
    wire [31:0]  xw_wx   = xw_nrow - bc_wbase;

    always @(posedge clk) begin
        if (rst) begin
            fl_cnt  <= 7'd0;
            x_mode  <= 1'b0;
            x_w <= 11'd0; x_color <= 16'd0; x_dstr <= 16'd0; x_sstr <= 16'd0;
            x_op <= 1'b0; x_wph <= 1'b0;
            x_arvalid <= 1'b0; x_araddr <= 32'd0; x_arlen <= 4'd0;
            xa_awv <= 1'b0; xa_awaddr <= 32'd0; xa_awlen <= 4'd0; xw_wb <= 12'd0;
            xr_a <= 28'd0; xa_a <= 28'd0; xw_a <= 4'd0;
            xr_left <= 9'd0; xa_left <= 9'd0; xw_left <= 9'd0;
            xr_nrow <= 32'd0; xa_nrow <= 32'd0; xw_nrow <= 32'd0;
            xr_rows <= 11'd0; xa_rows <= 11'd0; xw_rows <= 11'd0;
            xw_first <= 1'b0; xw_lo <= 3'd0; xw_hi <= 3'd0;
            xr_pend <= 10'd0;
            xres    <= {(RW+1){1'b0}};
            xb_unc  <= {(RW+1){1'b0}};
            xb_wp   <= {(RW+1){1'b0}};
            xb_rp   <= {(RW+1){1'b0}};
            xs_pend <= 1'b0;
            xr_v    <= 1'b0;
            xr_d    <= 128'd0;
        end else begin
            fl_cnt <= fl_cnt + {6'd0, c_we} - {6'd0, bd_pop & t_fill};
            xs_pend <= xs_rd;
            xr_v   <= x_rvalid;
            xr_d   <= x_rdata;
            if (x_arvalid & x_arready)
                x_arvalid <= 1'b0;
            if (xa_awv & x_awready)                  // x_* owner = wide path whenever xa_awv
                xa_awv <= 1'b0;

            if (x_start) begin
                x_mode   <= 1'b1;
                x_w      <= q_w;     x_color <= q_color;
                x_dstr   <= q_dstr;  x_sstr  <= q_sstr;
                x_op     <= q_op;    x_wph   <= 1'b0;
                xr_a     <= q_src[31:4];
                xr_left  <= q_op ? xq_nbt[8:0] : 9'd0;
                xr_nrow  <= q_src + {16'd0, q_sstr};
                xr_rows  <= q_h - 11'd1;
                xa_a     <= q_dst[31:4];
                xa_left  <= bm ? 9'd0 : xq_nbt[8:0];    // band mode: no AW
                xa_nrow  <= q_dst + {16'd0, q_dstr};
                xa_rows  <= q_h - 11'd1;
                xw_a     <= q_dst[7:4];
                xw_left  <= xq_nbt[8:0];
                xw_nrow  <= q_dst + {16'd0, q_dstr};
                xw_rows  <= q_h - 11'd1;
                xw_first <= 1'b1;
                xw_lo    <= q_dst[3:1];
                xw_hi    <= xq_hit[2:0];
                xw_wb    <= xq_wx[15:4];
                xr_pend  <= 10'd0;
                xres     <= {(RW+1){1'b0}};
                xb_unc   <= {(RW+1){1'b0}};
                xb_wp    <= {(RW+1){1'b0}};
                xb_rp    <= {(RW+1){1'b0}};
            end else begin
                if (x_done)
                    x_mode <= 1'b0;

                if (xr_iss) begin
                    x_arvalid <= 1'b1;
                    x_araddr  <= {xr_a, 4'd0};
                    x_arlen   <= xr_lm1[3:0];
                    if (xr_rend) begin
                        if (xr_rows != 11'd0) begin
                            xr_a    <= xr_nrow[31:4];
                            xr_left <= xr_nbt[8:0];
                            xr_nrow <= xr_nrow + {16'd0, x_sstr};
                            xr_rows <= xr_rows - 11'd1;
                        end else begin
                            xr_left <= 9'd0;
                        end
                    end else begin
                        xr_a    <= xr_a + {23'd0, xr_len};
                        xr_left <= xr_left - {4'd0, xr_len};
                    end
                end

                if (xa_iss) begin
                    xa_awv    <= 1'b1;
                    xa_awaddr <= {xa_a, 4'd0};
                    xa_awlen  <= xa_lm1[3:0];
                    if (xa_rend) begin
                        if (xa_rows != 11'd0) begin
                            xa_a    <= xa_nrow[31:4];
                            xa_left <= xa_nbt[8:0];
                            xa_nrow <= xa_nrow + {16'd0, x_dstr};
                            xa_rows <= xa_rows - 11'd1;
                        end else begin
                            xa_left <= 9'd0;
                        end
                    end else begin
                        xa_a    <= xa_a + {23'd0, xa_len};
                        xa_left <= xa_left - {4'd0, xa_len};
                    end
                end

                if (xw_adv) begin
                    if (xw_left == 9'd1) begin
                        if (xw_rows != 11'd0) begin
                            xw_a     <= xw_nrow[7:4];
                            xw_left  <= xw_nbt[8:0];
                            xw_nrow  <= xw_nrow + {16'd0, x_dstr};
                            xw_rows  <= xw_rows - 11'd1;
                            xw_first <= 1'b1;
                            xw_lo    <= xw_nrow[3:1];
                            xw_hi    <= xw_hit[2:0];
                            xw_wb    <= xw_wx[15:4];
                        end else begin
                            xw_left  <= 9'd0;
                        end
                    end else begin
                        xw_a     <= xw_a + 4'd1;
                        xw_left  <= xw_left - 9'd1;
                        xw_first <= 1'b0;
                        xw_wb    <= xw_wb + 12'd1;
                    end
                end

                // COPY write batch: once started, write while data is there
                x_wph   <= xa_iss | (x_wph & xa_dav);
                xr_pend <= xr_pend + (xr_iss ? {5'd0, xr_len} : 10'd0) - {9'd0, x_rvalid};
                xres    <= xres + (xr_iss ? {{(RW-4){1'b0}}, xr_len} : {(RW+1){1'b0}})
                                - {{RW{1'b0}}, xs_rd};      // a buffer slot is free once read
                xb_unc  <= xb_unc + {{RW{1'b0}}, xr_v}
                                  - ((xa_iss & x_op) ? {{(RW-4){1'b0}}, xa_len} : {(RW+1){1'b0}});
                if (xr_v)
                    xb_wp <= xb_wp + {{RW{1'b0}}, 1'b1};
                xb_rp   <= xb_rpn;
            end
        end
    end

    // line rings, shared with the wide beat buffer (x_mode, direct mode only)
    wire          rg_we_d = xbuf ? xr_v : we_d;
    wire          rg_we_s = xbuf ? xr_v : we_s;
    wire          rg_we_a = xbuf ? xr_v : we_a;
    wire [RW-1:0] rg_wa_d = xbuf ? xb_wp[RW-1:0] : rx_d[RW-1:0];
    wire [RW-1:0] rg_wa_s = xbuf ? xb_wp[RW-1:0] : rx_s[RW-1:0];
    wire [RW-1:0] rg_wa_a = xbuf ? xb_wp[RW-1:0] : rx_a[RW-1:0];
    wire [31:0]   rg_wd_d = xbuf ? xr_d[31:0]  : r_d;
    wire [31:0]   rg_wd_s = xbuf ? xr_d[63:32] : r_d;
    wire [31:0]   rg_wd_a = xbuf ? xr_d[95:64] : r_d;
    wire [RW-1:0] rg_ra_d  = xbuf ? xb_rp[RW-1:0]  : ra_d;
    wire [RW-1:0] rg_ra_s0 = xbuf ? xb_rp[RW-1:0]  : ra_s0;
    wire [RW-1:0] rg_ra_s1 = xbuf ? xb_rp[RW-1:0]  : ra_s1;
    wire [RW-1:0] rg_ra_a0 = xbuf ? xb_rp[RW-1:0]  : ra_a0;
    wire [RW-1:0] rg_ra_a1 = xbuf ? xb_rp[RW-1:0]  : ra_a1;
    wire [RW-1:0] rg_ra_a2 = xbuf ? xb_rp[RW-1:0]  : ra_a2;
    wire [RW-1:0] rg_ra_a3 = xbuf ? xb_rp[RW-1:0]  : ra_a3;

    blit_ram #(.DW(32), .AW(RW)) u_ring_d  (.clk(clk), .we(rg_we_d), .wa(rg_wa_d), .wd(rg_wd_d),
                                            .ra(rg_ra_d),  .rd(q_dw));
    blit_ram #(.DW(16), .AW(RW)) u_ring_s0 (.clk(clk), .we(rg_we_s), .wa(rg_wa_s), .wd(rg_wd_s[15:0]),
                                            .ra(rg_ra_s0), .rd(q_s0));
    blit_ram #(.DW(16), .AW(RW)) u_ring_s1 (.clk(clk), .we(rg_we_s), .wa(rg_wa_s), .wd(rg_wd_s[31:16]),
                                            .ra(rg_ra_s1), .rd(q_s1));
    blit_ram #(.DW(8),  .AW(RW)) u_ring_a0 (.clk(clk), .we(rg_we_a), .wa(rg_wa_a), .wd(rg_wd_a[7:0]),
                                            .ra(rg_ra_a0), .rd(q_a0));
    blit_ram #(.DW(8),  .AW(RW)) u_ring_a1 (.clk(clk), .we(rg_we_a), .wa(rg_wa_a), .wd(rg_wd_a[15:8]),
                                            .ra(rg_ra_a1), .rd(q_a1));
    blit_ram #(.DW(8),  .AW(RW)) u_ring_a2 (.clk(clk), .we(rg_we_a), .wa(rg_wa_a), .wd(rg_wd_a[23:16]),
                                            .ra(rg_ra_a2), .rd(q_a2));
    blit_ram #(.DW(8),  .AW(RW)) u_ring_a3 (.clk(clk), .we(rg_we_a), .wa(rg_wa_a), .wd(rg_wd_a[31:24]),
                                            .ra(rg_ra_a3), .rd(q_a3));

    // =========================================================================
    // Command list : DMA fetch -> 32-word register FIFO -> decoder -> command queue
    // =========================================================================
    // ---- fetch -------------------------------------------------------------
    wire [29:0] l_rem   = l_end - l_ptr;                     // words still in DDR
    wire [6:0]  l_tobnd = 7'd64 - {1'b0, l_ptr[5:0]};        // to the 256-byte boundary
    wire [7:0]  l_free  = 8'd32 - lf_inflight;               // FIFO words not reserved
    wire [6:0]  l_m1    = (l_rem < {23'd0, l_tobnd}) ? l_rem[6:0] : l_tobnd;
    wire [6:0]  l_m2    = ({1'b0, l_m1} < l_free) ? l_m1 : l_free[6:0];
    wire        l_more  = (l_ptr != l_end);
    wire        dma_active = l_more | (lf_inflight != 8'd0);

    wire [31:0] lf_dout;
    wire        lf_valid, lf_pop;

    // R5: 32 words of register storage replace two underfilled RAM10s.
    // Reserve at most 32 entries, leaving the FWFT output slot as margin.
    blit_rfifo #(.DW(32), .AW(5)) u_lf (
        .clk(clk), .rst(rst),
        .push(we_l), .din(r_d), .full(), .space(),
        .pop(lf_pop), .dout(lf_dout), .valid(lf_valid), .count()
    );

    // band mode (D) jumps of the fetch pointer (logic in the decoder / "Band mode")
    wire        endb_call;              // ENDB taken: park the main list, stop fetching
    wire        bc_bcall;               // phase 2: fetch the bin of the next band
    wire        bc_ret;                 // frame done: back to the main list
    wire [29:0] bc_bin_a, bc_bin_e;     // that bin (word addresses)

    always @(posedge clk) begin
        if (rst) begin
            l_ptr       <= 30'd0;
            l_end       <= 30'd0;
            l_len       <= 7'd0;
            l_len_v     <= 1'b0;
            lf_inflight <= 8'd0;
            m_end       <= 30'd0;
            l_ret       <= 30'd0;
        end else begin
            if (laddr_wr & ~list_active) begin
                l_ptr <= apb_pwdata[31:2];
                l_end <= apb_pwdata[31:2];
            end else if (tb_call) begin
                // TAB: fetch the table now. Main list words requested but not
                // decoded yet are dropped by the decoder (D_SKIP) and fetched
                // again from l_ret after the table.
                l_ptr <= tb_tab;
                l_end <= tb_tend;
                m_end <= lend_wr ? apb_pwdata[31:2] : l_end;
                l_ret <= l_ptr - {22'd0, lf_inflight} + {29'd0, lf_pop};
            end else if (tb_ret) begin
                l_ptr <= l_ret;
                l_end <= lend_wr ? apb_pwdata[31:2] : m_end;
            end else if (endb_call) begin
                // ENDB: the main list waits in l_ret / m_end until the frame is
                // drawn (prefetched words are dropped, D_SKIP, like TAB)
                l_ptr <= bg_bin[31:2];
                l_end <= bg_bin[31:2];
                m_end <= lend_wr ? apb_pwdata[31:2] : l_end;
                l_ret <= l_ptr - {22'd0, lf_inflight} + {29'd0, lf_pop};
            end else if (bc_bcall) begin
                l_ptr <= bc_bin_a;
                l_end <= bc_bin_e;
                if (lend_wr) m_end <= apb_pwdata[31:2];
            end else if (bc_ret) begin
                l_ptr <= l_ret;
                l_end <= lend_wr ? apb_pwdata[31:2] : m_end;
            end else begin
                if (lend_wr) begin
                    if (tb_in | bc_call) m_end <= apb_pwdata[31:2];
                    else                 l_end <= apb_pwdata[31:2];
                end
                if (l_issue) l_ptr <= l_ptr + {23'd0, l_len};
            end
            // burst length is computed one clock ahead; LIST_END and the FIFO
            // space only grow meanwhile, so the registered value stays valid
            // (a TAB jump changes the pointer: compute it again)
            if (l_issue | tb_call | tb_ret | endb_call | bc_bcall | bc_ret) begin
                l_len_v <= 1'b0;
            end else if (~l_len_v) begin
                l_len   <= l_m2;
                l_len_v <= l_more & (l_free >= 8'd16);
            end
            lf_inflight <= lf_inflight + (l_issue ? {1'b0, l_len} : 8'd0) - {7'd0, lf_pop};
        end
    end

    // ---- decoder -----------------------------------------------------------
    localparam D_HDR = 4'd0, D_ARGS = 4'd1, D_DRAW = 4'd2;
    // TAB: table address word, drop prefetched list words, then per item:
    // x/y word, sprite word (slot read), offset, push
    localparam D_TADR = 4'd3, D_SKIP = 4'd4, D_TXY = 4'd5, D_TSP = 4'd6,
               D_TOFF = 4'd7, D_TDRW = 4'd8;
    // band mode (D): BANDS frame-address word, RES record words 1..7
    localparam D_BFB = 4'd9, D_RES = 4'd10;
    reg  [3:0]  d_state;
    reg  [31:0] d_word;                 // header of the record being decoded
    reg  [9:0]  d_mask;                 // parameter words still to come
    reg         d_endb;                 // D_SKIP belongs to an ENDB (then back to D_HDR)
    reg  [2:0]  d_rc;                   // RES: words 1..7 taken so far
    reg  [223:0] d_res;                 // RES: words shifted in from the top
    reg  [15:0] tb_cnt;                  // TAB: items still to draw
    reg  [7:0]  tb_skip;                 // TAB: prefetched list words still to drop
    reg  [11:0] tb_x, tb_y;
    reg  [5:0]  tb_slot;
    reg sq_table, sq_odd, sq_alpha;
    reg [7:0] sq_size;
    reg [15:0] sq_color;
    reg  [27:0] tb_prod;                 // y * DSTR
    reg  [24:0] tb_off;                  // y * DSTR + x * 2
    // list context
    reg  [31:0] l_dst, l_src, l_aad, l_base;
    reg  [15:0] l_dstr, l_sstr, l_astr, l_color, l_key;
    reg  [10:0] l_w, l_h;

    // next list word: from the FIFO, or straight from APB (LIST_WORD; not while a
    // band frame is being drawn)
    wire        lw_apb   = lword_wr & ~dma_active & ~bc_call;
    wire        lw_valid = lf_valid | lw_apb;
    wire [31:0] lw       = lf_valid ? lf_dout : apb_pwdata;

    // where pushed records go: the command queue, or (band frame) the Y pipeline
    wire        d_full   = bc_act ? ~y_in_ok : cq_full;
    wire        d_args_done = (d_state == D_ARGS) & (d_mask == 10'd0);
    wire        d_res_last  = (d_state == D_RES) & (d_rc == 3'd6);
    wire        lw_take  = lw_valid & ((d_state == D_HDR) | (d_state == D_TADR) | (d_state == D_BFB) |
                                       ((d_state == D_ARGS) & (d_mask != 10'd0)) |
                                       ((d_state == D_RES) & (~d_res_last | y_in_ok)));
    // table items and dropped words only ever come from the fetch FIFO
    wire        tb_take   = lf_valid & (((d_state == D_SKIP) & (tb_skip != 8'd0)) |
                                       ((d_state == D_TXY) & (tb_cnt != 16'd0)) |
                                       (d_state == D_TSP));
    assign      lf_pop      = (lw_take & lf_valid) | tb_take;
    assign      lw_apb_take = lw_take & ~lf_valid;
    assign      list_active = dma_active | (d_state != D_HDR) | bc_act;
    assign      tb_in     = (d_state == D_TXY) | (d_state == D_TSP) |
                            (d_state == D_TOFF) | (d_state == D_TDRW) |
                            ((d_state == D_SKIP) & ~d_endb);
    assign      tb_call   = lw_take & (d_state == D_TADR) & (d_word[15:0] != 16'd0);
    assign      tb_ret    = (d_state == D_TXY) & (tb_cnt == 16'd0);
    assign      tb_tab    = lw[31:2];
    assign      tb_tend   = lw[31:2] + {13'd0, d_word[15:0], 1'b0};

    wire        hdr_take = lw_take & (d_state == D_HDR);
    wire        hdr_draw = lw[31];
    wire        hdr_rec  = (lw[31:28] == 4'd0);
    wire        hdr_slot = (lw[31:28] == 4'd1);
    wire        hdr_sq   = (lw[31:28] == 4'd6) & (lw[27:17] == 11'd0) & (lw[15:0] <= 16'd30000);
    wire        hdr_tab  = (lw[31:28] == 4'd2) | hdr_sq;
    wire        hdr_bnds = (lw[31:28] == 4'd3);              // D: BANDS
    wire        hdr_endb = (lw[31:28] == 4'd4);              // D: ENDB
    wire        hdr_res  = (lw[31:28] == 4'd5);              // D: RES (only in bins)
    wire        hdr_bok  = hdr_bnds & ~bc_act;               // BANDS outside a band frame
    wire        hdr_eok  = hdr_endb & (bc_state == BC_P1);   // ENDB inside one
    assign      endb_call = hdr_take & ~bm & hdr_eok;
    wire        bands_go  = lw_take & (d_state == D_BFB);
    wire [9:0]  d_low    = d_mask & (~d_mask + 10'd1);       // lowest pending bit

    // sprite slots {cache, galpha, flags, h, w, sstr, src}
    localparam SL_W = 1 + 1 + 8 + 4 + 11 + 11 + 16 + 32;     // 84 {add, cache, ...}
    wire [SL_W-1:0] sl_q;
    wire [5:0]      sl_ra = (d_state == D_HDR) ? lw[30:25] :
                            tb_in ? tb_slot : d_word[30:25];
    wire            sl_we = hdr_take & hdr_slot & ~bm;
    blit_ram #(.DW(SL_W), .AW(6)) u_slot (
        .clk(clk), .we(sl_we), .wa(lw[5:0]),
        .wd({lw[26], lw[24], lw[23:16], lw[15:12], l_h, l_w, l_sstr, l_src}),
        .ra(sl_ra), .rd(sl_q)
    );

    wire        l_push_rec  = d_args_done & ~d_word[10] & ~d_full;
    wire        d_drawing   = (d_state == D_DRAW) | (d_state == D_TDRW);
    wire        l_push_draw = d_drawing & ~d_full;
    assign      l_push      = l_push_rec | l_push_draw;
    // RES (phase 2): the 7th word completes the command record
    wire        res_push    = d_res_last & lf_valid & y_in_ok;
    wire [223:0] res_rec    = {lf_dout, d_res[223:32]};

    wire [24:0] d_off  = (d_state == D_TDRW) ? tb_off : d_word[24:0];
    wire [31:0] sl_dst = l_base + {7'd0, d_off};
    // Palette exactly matches demo.c; no texture RAM or new pixel buffer.
    wire [CQ_W-1:0] cq_lsquare = {1'b0,2'b00,
        ((sq_alpha & sq_odd) ? 8'd150 : 8'd255),4'b0000,l_key,sq_color,
        {3'd0,sq_size},{3'd0,sq_size},16'd0,32'd0,16'd0,32'd0,l_dstr,sl_dst};
    wire [CQ_W-1:0] cq_lrec  = {d_word[26], d_word[25:24], d_word[23:16], d_word[15:12], l_key, l_color,
                                l_h, l_w, l_astr, l_aad, l_sstr, l_src, l_dstr, l_dst};
    wire [CQ_W-1:0] cq_ldraw = {sl_q[83], 1'b0, sl_q[82], sl_q[81:74], sl_q[73:70], l_key, l_color,
                                sl_q[69:59], sl_q[58:48], l_astr, l_aad,
                                sl_q[47:32], sl_q[31:0], l_dstr, sl_dst};
    assign      cq_ldin = d_drawing ? ((sq_table & (d_state == D_TDRW)) ? cq_lsquare : cq_ldraw) : cq_lrec;

    always @(posedge clk) begin
        if (rst) begin
            d_state <= D_HDR;
            d_word  <= 32'd0;
            d_mask  <= 10'd0;
            l_dst <= 32'd0; l_src <= 32'd0; l_aad <= 32'd0; l_base <= 32'd0;
            l_dstr <= 16'd1280; l_sstr <= 16'd0; l_astr <= 16'd0;
            l_color <= 16'd0; l_key <= 16'hF81F;
            l_w <= 11'd0; l_h <= 11'd0;
            tb_cnt <= 16'd0; tb_skip <= 8'd0;
            sq_table<=0;sq_odd<=0;sq_alpha<=0;sq_size<=0;sq_color<=0;
            tb_x <= 12'd0; tb_y <= 12'd0; tb_slot <= 6'd0;
            tb_prod <= 28'd0; tb_off <= 25'd0;
            d_endb <= 1'b0; d_rc <= 3'd0; d_res <= 224'd0;
        end else begin
            case (d_state)
                D_HDR: begin
                    if (hdr_take) begin
                        d_word <= lw;
                        d_mask <= lw[9:0];
                        tb_cnt  <= lw[15:0];
                        sq_table <= hdr_sq;sq_odd<=0;sq_alpha<=lw[16];
                        d_rc    <= 3'd0;
                        if (bm)            d_state <= hdr_res ? D_RES : D_HDR;  // bins: RES only
                        else if (hdr_draw) d_state <= D_DRAW;
                        else if (hdr_rec)  d_state <= D_ARGS;
                        else if (hdr_tab)  d_state <= D_TADR;
                        else if (hdr_bok)  d_state <= D_BFB;
                        else if (hdr_eok) begin
                            // ENDB: drop the prefetched main list words (refetched
                            // from l_ret after the frame), then wait for the bins
                            tb_skip <= lf_inflight + (l_issue ? {1'b0, l_len} : 8'd0)
                                                   - {7'd0, lf_pop};
                            d_endb  <= 1'b1;
                            d_state <= D_SKIP;
                        end
                    end
                end

                // ---- band mode (D) ----------------------------------------
                D_BFB: begin                         // BANDS: frame base address
                    if (lw_take)
                        d_state <= D_HDR;
                end

                D_RES: begin                         // RES words 1..7 -> command record
                    if (lw_take) begin
                        d_res <= {lw, d_res[223:32]};
                        d_rc  <= d_rc + 3'd1;
                        if (d_res_last)
                            d_state <= D_HDR;
                    end
                end

                // ---- TAB --------------------------------------------------
                D_TADR: begin                        // table address word
                    if (lw_take) begin
                        // every list word requested and not decoded after
                        // this clock belongs to the main list: drop them
                        tb_skip  <= lf_inflight + (l_issue ? {1'b0, l_len} : 8'd0)
                                               - {7'd0, lf_pop};
                        d_state <= tb_call ? D_SKIP : D_HDR;
                    end
                end

                D_SKIP: begin
                    if (tb_skip == 8'd0) begin
                        d_state <= d_endb ? D_HDR : D_TXY;
                        d_endb  <= 1'b0;
                    end else if (lf_valid)
                        tb_skip <= tb_skip - 8'd1;
                end

                D_TXY: begin                         // tb_cnt == 0: tb_ret, back
                    if (tb_cnt == 16'd0) begin
                        d_state <= D_HDR;
                    end else if (lf_valid) begin
                        tb_x     <= sq_table ? lf_dout[15:4] : lf_dout[11:0];
                        tb_y     <= sq_table ? lf_dout[31:20] : lf_dout[27:16];
                        d_state <= D_TSP;
                    end
                end

                D_TSP: begin                         // sprite word: read the slot
                    if (lf_valid) begin
                        tb_slot  <= lf_dout[5:0];
                        sq_size <= lf_dout[23:16];
                        case(lf_dout[26:24])
                            0:sq_color<=16'hf32a;1:sq_color<=16'h3dbb;
                            2:sq_color<=16'hb70d;3:sq_color<=16'hf5eb;
                            4:sq_color<=16'h9bdc;5:sq_color<=16'heb50;
                            6:sq_color<=16'h7698;7:sq_color<=16'hf77a;
                        endcase
                        tb_prod  <= tb_y * l_dstr;
                        d_state <= D_TOFF;
                    end
                end

                D_TOFF: begin
                    tb_off   <= tb_prod[24:0] + {12'd0, tb_x, 1'b0};
                    d_state <= D_TDRW;
                end

                D_TDRW: begin
                    if (~d_full) begin
                        tb_cnt   <= tb_cnt - 16'd1;
                        sq_odd <= ~sq_odd;
                        d_state <= D_TXY;
                    end
                end

                D_ARGS: begin
                    if (d_mask == 10'd0) begin
                        if (d_word[10] | ~d_full)
                            d_state <= D_HDR;
                    end else if (lw_valid) begin
                        d_mask <= d_mask & ~d_low;
                        if (d_low[0]) l_dst   <= lw;
                        if (d_low[1]) l_dstr  <= lw[15:0];
                        if (d_low[2]) l_src   <= lw;
                        if (d_low[3]) l_sstr  <= lw[15:0];
                        if (d_low[4]) l_aad   <= lw;
                        if (d_low[5]) l_astr  <= lw[15:0];
                        if (d_low[6]) begin l_w <= lw[10:0]; l_h <= lw[26:16]; end
                        if (d_low[7]) l_color <= lw[15:0];
                        if (d_low[8]) l_key   <= lw[15:0];
                        if (d_low[9]) l_base  <= lw;
                    end
                end

                D_DRAW: begin
                    if (~d_full)
                        d_state <= D_HDR;
                end

                default: d_state <= D_HDR;
            endcase
        end
    end

    wire lerr_set = hdr_take & (bm ? ~hdr_res
                                   : ~(hdr_draw | hdr_rec | hdr_slot | hdr_tab | hdr_bok | hdr_eok));

    // =========================================================================
    // PIXEL : ring read, colour key, alpha blend -> W FIFO
    // =========================================================================
    // Two parts, so that rows and commands follow each other without dead clocks:
    //   ROW GEN  takes the command headers from h1 (and hands them on to WRITE
    //            through h2) and works out the geometry of each row one row
    //            ahead into a one-entry row slot (up to one row per clock).
    //   ISSUE    starts the row in the slot as soon as its words are in the
    //            rings - in the clock right after the previous row's last word
    //            when they already are - and issues one output word per clock.
    //            The command parameters travel with every word through P1..P4,
    //            so a new command can start while the pipe still holds the last
    //            words of the previous one (no drain between commands).
    // Ring words are released (c_rd_*, credit to FETCH) when a row is finished;
    // c_ps_* run ahead of them and mark where the next row to start begins.
    localparam G_IDLE = 1'b0, G_ROW = 1'b1;
    localparam C_IDLE = 1'b0, C_RUN = 1'b1;

    // row generator: current command and the next row's low address bits
    reg         g_state;
    reg         g_op, g_keyen, g_apl, g_blend, g_add;
    reg  [15:0] g_key, g_color;
    reg  [8:0]  g_ga9;
    reg  [10:0] g_w, g_rows;
    reg  [1:0]  g_dlo, g_dstr_lo, g_slo, g_sstr_lo, g_alo, g_astr_lo;
    reg  [31:0] g_drow;                 // D: destination row address (window word index)
    reg  [15:0] g_dstr;
    reg  [13:0] s_wq, c_wq;             // D: window word of the slot row / the next word
    // row slot: one row ready to start
    reg         s_v;
    reg         s_op, s_keyen, s_apl, s_blend, s_add;
    reg  [15:0] s_key, s_color;
    reg  [8:0]  s_ga9;
    reg         s_dpar, s_lastodd, s_odd;
    reg  [1:0]  s_boff;
    reg  [11:0] s_nout, s_ns, s_na;
    // issue: the row being issued
    reg         c_state;
    reg         c_op, c_keyen, c_apl, c_blend, c_add;
    reg  [15:0] c_key, c_color;
    reg  [8:0]  c_ga9;
    reg         c_dpar, c_lastodd, c_first;
    reg  [11:0] c_nout, c_ns, c_na;
    reg  [11:0] c_left;                 // words of this row not issued yet
    reg  [RW:0]   c_i;                  // source pixel index in ring
    reg  [RW+1:0] c_j;                  // alpha byte index in ring
    reg  [RW-1:0] c_dq;                 // destination word index in ring
    reg  [RW:0]   c_ps_d, c_ps_a, c_ps_s;   // ring start of the next row

    // pipeline valid bits
    reg p1_v, p2_v, p3_v, p4_v, p5_v, p6_v;
    wire pipe_busy = p1_v | p2_v | p3_v | p4_v | p5_v | p6_v;

    // W FIFO credit (words issued into the pipe and not yet sent on W)
    reg  [WF_AW+1:0] wf_inflight;
    wire             wf_credit = (wf_inflight < WF_WORDS);

    // h1 fields
    wire [31:0] h1_dst   = h1_dout[31:0];
    wire [10:0] h1_w     = h1_dout[58:48];
    wire [10:0] h1_h     = h1_dout[69:59];
    wire [1:0]  h1_slo   = h1_dout[71:70];
    wire [1:0]  h1_sstr  = h1_dout[73:72];
    wire [1:0]  h1_alo   = h1_dout[75:74];
    wire [1:0]  h1_astr  = h1_dout[77:76];
    wire [15:0] h1_color = h1_dout[93:78];
    wire [15:0] h1_key   = h1_dout[109:94];
    wire        h1_op    = h1_dout[110];
    wire        h1_keyen = h1_dout[111];
    wire        h1_apl   = h1_dout[112];
    wire        h1_blend = h1_dout[113];
    wire [7:0]  h1_ga    = h1_dout[121:114];
    wire        h1_add   = h1_dout[122];
    wire [1:0]  h1_dstr  = h1_dout[33:32];

    // band mode (D): no WRITE stage (P6 writes the window), so nothing goes to h2
    wire g_load = (g_state == G_IDLE) & h1_valid & (bm | ~h2_full);
    assign h1_pop  = g_load;
    assign h2_push = g_load & ~bm;
    wire [31:0] g_wx = g_drow - bc_wbase;           // D: row offset inside the window

    // geometry of the generator's next row
    wire        r_dpar    = g_dlo[1];
    wire [11:0] r_nout    = ({1'b0, g_w} + {11'd0, r_dpar} + 12'd1) >> 1;
    wire        r_odd     = g_slo[1] ^ r_dpar;
    wire [1:0]  r_boff    = g_alo - {1'b0, r_dpar};
    wire [12:0] r_na_t    = {11'd0, r_boff} + {r_nout, 1'b0} - 13'd1;
    wire [11:0] r_na      = {1'b0, r_na_t[12:2]} + 12'd1;
    wire [11:0] r_ns      = r_nout + {11'd0, r_odd};

    // slot row data present? (ring words beyond the rows already started)
    wire [RW:0] av_d = rx_d - c_ps_d;
    wire [RW:0] av_a = rx_a - c_ps_a;
    wire [RW:0] av_s = rx_s - c_ps_s;
    wire s_ok = s_v & (~s_blend | bm | ({1'b0, av_d} >= {1'b0, s_nout})) &
                      (~s_apl   | ({1'b0, av_a} >= {1'b0, s_na}))   &
                      (~s_op    | ({1'b0, av_s} >= {1'b0, s_ns}));

    wire c_issue  = (c_state == C_RUN) & wf_credit;
    wire c_lastw  = (c_left == 12'd1);
    wire c_rowend = c_issue & c_lastw;
    wire c_start  = s_ok & ((c_state == C_IDLE) | c_rowend);
    wire g_push   = (g_state == G_ROW) & (~s_v | c_start);

    // ---- ROW GEN ------------------------------------------------------------
    always @(posedge clk) begin
        if (rst) begin
            g_state <= G_IDLE;
            g_op <= 1'b0; g_keyen <= 1'b0; g_apl <= 1'b0; g_blend <= 1'b0; g_add <= 1'b0;
            g_key <= 16'd0; g_color <= 16'd0; g_ga9 <= 9'd0;
            g_w <= 11'd0; g_rows <= 11'd0;
            g_dlo <= 2'd0; g_dstr_lo <= 2'd0; g_slo <= 2'd0; g_sstr_lo <= 2'd0;
            g_alo <= 2'd0; g_astr_lo <= 2'd0;
            s_v <= 1'b0;
            s_op <= 1'b0; s_keyen <= 1'b0; s_apl <= 1'b0; s_blend <= 1'b0; s_add <= 1'b0;
            s_key <= 16'd0; s_color <= 16'd0; s_ga9 <= 9'd0;
            s_dpar <= 1'b0; s_lastodd <= 1'b0; s_odd <= 1'b0; s_boff <= 2'd0;
            s_nout <= 12'd0; s_ns <= 12'd0; s_na <= 12'd0;
            g_drow <= 32'd0; g_dstr <= 16'd0; s_wq <= 14'd0;
        end else begin
            if (g_load) begin
                g_drow    <= h1_dst;
                g_dstr    <= h1_dout[47:32];
                g_op      <= h1_op;
                g_keyen   <= h1_keyen;
                g_apl     <= h1_apl;
                g_blend   <= h1_blend;
                g_add     <= h1_add;
                g_key     <= h1_key;
                g_color   <= h1_color;
                g_ga9     <= {1'b0, h1_ga} + {8'd0, h1_ga[7]};
                g_w       <= h1_w;
                g_rows    <= h1_h;
                g_dlo     <= h1_dst[1:0];
                g_dstr_lo <= h1_dstr;
                g_slo     <= h1_slo;
                g_sstr_lo <= h1_sstr;
                g_alo     <= h1_alo;
                g_astr_lo <= h1_astr;
                g_state   <= G_ROW;
            end
            if (g_push) begin
                s_op      <= g_op;
                s_keyen   <= g_keyen;
                s_apl     <= g_apl;
                s_blend   <= g_blend;
                s_add     <= g_add;
                s_key     <= g_key;
                s_color   <= g_color;
                s_ga9     <= g_ga9;
                s_dpar    <= r_dpar;
                s_lastodd <= r_dpar ^ g_w[0];
                s_odd     <= r_odd;
                s_boff    <= r_boff;
                s_nout    <= r_nout;
                s_ns      <= r_ns;
                s_na      <= r_na;
                s_wq      <= g_wx[15:2];
                g_drow    <= g_drow + {16'd0, g_dstr};
                g_dlo     <= g_dlo + g_dstr_lo;
                g_slo     <= g_slo + g_sstr_lo;
                g_alo     <= g_alo + g_astr_lo;
                if (g_rows == 11'd1)
                    g_state <= G_IDLE;
                else
                    g_rows  <= g_rows - 11'd1;
            end
            if (g_push)
                s_v <= 1'b1;
            else if (c_start)
                s_v <= 1'b0;
        end
    end

    // ---- ISSUE ---------------------------------------------------------------
    always @(posedge clk) begin
        if (rst) begin
            c_state <= C_IDLE;
            c_rd_d <= {(RW+1){1'b0}};
            c_rd_a <= {(RW+1){1'b0}};
            c_rd_s <= {(RW+1){1'b0}};
            c_ps_d <= {(RW+1){1'b0}};
            c_ps_a <= {(RW+1){1'b0}};
            c_ps_s <= {(RW+1){1'b0}};
            c_op <= 1'b0; c_keyen <= 1'b0; c_apl <= 1'b0; c_blend <= 1'b0; c_add <= 1'b0;
            c_key <= 16'd0; c_color <= 16'd0; c_ga9 <= 9'd0;
            c_dpar <= 1'b0; c_lastodd <= 1'b0; c_first <= 1'b0;
            c_nout <= 12'd0; c_ns <= 12'd0; c_na <= 12'd0; c_left <= 12'd0;
            c_i <= {(RW+1){1'b0}}; c_j <= {(RW+2){1'b0}}; c_dq <= {RW{1'b0}};
            c_wq <= 14'd0;
        end else begin
            if (c_start) begin
                c_op      <= s_op;
                c_keyen   <= s_keyen;
                c_apl     <= s_apl;
                c_blend   <= s_blend;
                c_add     <= s_add;
                c_key     <= s_key;
                c_color   <= s_color;
                c_ga9     <= s_ga9;
                c_dpar    <= s_dpar;
                c_lastodd <= s_lastodd;
                c_nout    <= s_nout;
                c_ns      <= s_ns;
                c_na      <= s_na;
                c_left    <= s_nout;
                c_first   <= 1'b1;
                c_i       <= {c_ps_s[RW-1:0], s_odd};
                c_j       <= {c_ps_a[RW-1:0], s_boff};
                c_dq      <= c_ps_d[RW-1:0];
                c_wq      <= s_wq;
                if (s_blend & ~bm) c_ps_d <= c_ps_d + s_nout[RW:0];   // band mode: dst from window
                if (s_apl)   c_ps_a <= c_ps_a + s_na[RW:0];
                if (s_op)    c_ps_s <= c_ps_s + s_ns[RW:0];
                c_state   <= C_RUN;
            end else if (c_issue) begin
                c_left  <= c_left - 12'd1;
                c_first <= 1'b0;
                c_i     <= c_i + {{(RW-1){1'b0}}, 2'd2};
                c_j     <= c_j + {{RW{1'b0}}, 2'd2};
                c_dq    <= c_dq + {{(RW-1){1'b0}}, 1'b1};
                c_wq    <= c_wq + 14'd1;
                if (c_lastw)
                    c_state <= C_IDLE;
            end
            if (c_rowend) begin                     // release the finished row
                if (c_blend & ~bm) c_rd_d <= c_rd_d + c_nout[RW:0];
                if (c_apl)   c_rd_a <= c_rd_a + c_na[RW:0];
                if (c_op)    c_rd_s <= c_rd_s + c_ns[RW:0];
            end
        end
    end

    // ring read addresses (combinational from the word counters)
    wire [RW-1:0] c_ibase = c_i[RW:1];
    wire [RW-1:0] c_jbase = c_j[RW+1:2];
    wire [RW-1:0] c_jnext = c_jbase + {{(RW-1){1'b0}}, 1'b1};
    assign ra_s1 = c_ibase;
    assign ra_s0 = c_ibase + {{(RW-1){1'b0}}, c_i[0]};
    assign ra_a0 = (c_j[1:0] > 2'd0) ? c_jnext : c_jbase;
    assign ra_a1 = (c_j[1:0] > 2'd1) ? c_jnext : c_jbase;
    assign ra_a2 = (c_j[1:0] > 2'd2) ? c_jnext : c_jbase;
    assign ra_a3 = c_jbase;
    assign ra_d  = c_dq;

    // ---- P1 : ring data available ------------------------------------------
    reg        p1_lo_ok, p1_hi_ok, p1_isel;
    reg [1:0]  p1_jsel;
    // command parameters travel with the word (rows/commands overlap in P1..P4)
    reg        p1_op, p1_keyen, p1_apl, p1_blend, p1_add;
    reg [15:0] p1_key, p1_color;
    reg [8:0]  p1_ga9;
    reg [13:0] p1_wq, p2_wq, p3_wq, p4_wq, p5_wq, p6_wq;   // D: window word of the pipe word
    always @(posedge clk) begin
        if (rst) begin
            p1_v <= 1'b0; p1_lo_ok <= 1'b0; p1_hi_ok <= 1'b0;
            p1_isel <= 1'b0; p1_jsel <= 2'd0;
        end else begin
            p1_v     <= c_issue;
            p1_lo_ok <= ~(c_first & c_dpar);
            p1_hi_ok <= ~(c_lastw & c_lastodd);
            p1_isel  <= c_i[0];
            p1_jsel  <= c_j[1:0];
        end
        p1_op    <= c_op;
        p1_keyen <= c_keyen;
        p1_apl   <= c_apl;
        p1_blend <= c_blend;
        p1_add   <= c_add;
        p1_key   <= c_key;
        p1_color <= c_color;
        p1_ga9   <= c_ga9;
        p1_wq    <= c_wq;
    end

    // D: band mode reads the destination word from the window (read at c_issue
    // with address c_wq, data here), word p1_wq[1:0] of the 128-bit beat
    wire [127:0] wn_q;                  // window read data (window section)
    wire [31:0] p1_dw = (p1_wq[1:0] == 2'd0) ? wn_q[31:0]  : (p1_wq[1:0] == 2'd1) ? wn_q[63:32] :
                        (p1_wq[1:0] == 2'd2) ? wn_q[95:64] : wn_q[127:96];

    wire [15:0] p1_s_lo = ~p1_op ? p1_color : (p1_isel ? q_s1 : q_s0);
    wire [15:0] p1_s_hi = ~p1_op ? p1_color : (p1_isel ? q_s0 : q_s1);
    reg  [7:0]  p1_pa_lo, p1_pa_hi;
    always @(*) begin
        case (p1_jsel)
            2'd0:    begin p1_pa_lo = q_a0; p1_pa_hi = q_a1; end
            2'd1:    begin p1_pa_lo = q_a1; p1_pa_hi = q_a2; end
            2'd2:    begin p1_pa_lo = q_a2; p1_pa_hi = q_a3; end
            default: begin p1_pa_lo = q_a3; p1_pa_hi = q_a0; end
        endcase
    end
    wire p1_key_lo = p1_op & p1_keyen & (p1_s_lo == p1_key);
    wire p1_key_hi = p1_op & p1_keyen & (p1_s_hi == p1_key);

    // ---- P2 : selected pixels ----------------------------------------------
    reg [15:0] p2_s_lo, p2_s_hi;
    reg [31:0] p2_d;
    reg [7:0]  p2_pa_lo, p2_pa_hi;
    reg        p2_m_lo, p2_m_hi;
    reg        p2_apl, p2_blend, p2_add;
    reg [8:0]  p2_ga9;
    always @(posedge clk) begin
        if (rst) p2_v <= 1'b0;
        else     p2_v <= p1_v;
        p2_s_lo  <= p1_s_lo;
        p2_s_hi  <= p1_s_hi;
        p2_d     <= bm ? p1_dw : q_dw;
        p2_wq    <= p1_wq;
        p2_pa_lo <= p1_pa_lo;
        p2_pa_hi <= p1_pa_hi;
        p2_m_lo  <= p1_lo_ok & ~p1_key_lo;
        p2_m_hi  <= p1_hi_ok & ~p1_key_hi;
        p2_apl   <= p1_apl;
        p2_blend <= p1_blend;
        p2_add   <= p1_add;
        p2_ga9   <= p1_ga9;
    end

    // ---- P3 : per-pixel alpha x global alpha --------------------------------
    reg [15:0] p3_s_lo, p3_s_hi;
    reg [31:0] p3_d;
    reg [16:0] p3_pr_lo, p3_pr_hi;
    reg        p3_m_lo, p3_m_hi;
    reg        p3_apl, p3_blend, p3_add;
    reg [8:0]  p3_ga9;
    always @(posedge clk) begin
        if (rst) p3_v <= 1'b0;
        else     p3_v <= p2_v;
        p3_s_lo  <= p2_s_lo;
        p3_s_hi  <= p2_s_hi;
        p3_d     <= p2_d;
        p3_wq    <= p2_wq;
        p3_pr_lo <= {9'd0, p2_pa_lo} * {8'd0, p2_ga9};
        p3_pr_hi <= {9'd0, p2_pa_hi} * {8'd0, p2_ga9};
        p3_m_lo  <= p2_m_lo;
        p3_m_hi  <= p2_m_hi;
        p3_apl   <= p2_apl;
        p3_blend <= p2_blend;
        p3_add   <= p2_add;
        p3_ga9   <= p2_ga9;
    end

    // ---- P4 : blend factor a in 0..256 --------------------------------------
    wire [7:0] p3_e_lo = p3_pr_lo[15:8];
    wire [7:0] p3_e_hi = p3_pr_hi[15:8];
    reg [15:0] p4_s_lo, p4_s_hi;
    reg [31:0] p4_d;
    reg [8:0]  p4_a_lo, p4_a_hi;
    reg        p4_add;
    reg        p4_m_lo, p4_m_hi;
    always @(posedge clk) begin
        if (rst) p4_v <= 1'b0;
        else     p4_v <= p3_v;
        p4_s_lo <= p3_s_lo;
        p4_s_hi <= p3_s_hi;
        p4_d    <= p3_d;
        p4_wq   <= p3_wq;
        p4_a_lo <= ~p3_blend ? 9'd256 : (p3_apl ? ({1'b0, p3_e_lo} + {8'd0, p3_e_lo[7]}) : p3_ga9);
        p4_a_hi <= ~p3_blend ? 9'd256 : (p3_apl ? ({1'b0, p3_e_hi} + {8'd0, p3_e_hi[7]}) : p3_ga9);
        p4_m_lo <= p3_m_lo;
        p4_add  <= p3_add;
        p4_m_hi <= p3_m_hi;
    end

    // ---- P5 : channel products ----------------------------------------------
    // additive: the destination keeps its full weight (dst*256 + src*a + 128) >> 8
    wire [8:0] p4_ia_lo = p4_add ? 9'd256 : 9'd256 - p4_a_lo;
    wire [8:0] p4_ia_hi = p4_add ? 9'd256 : 9'd256 - p4_a_hi;
    reg [13:0] p5_rs_lo, p5_rd_lo, p5_bs_lo, p5_bd_lo, p5_rs_hi, p5_rd_hi, p5_bs_hi, p5_bd_hi;
    reg [14:0] p5_gs_lo, p5_gd_lo, p5_gs_hi, p5_gd_hi;
    reg        p5_m_lo, p5_m_hi;
    always @(posedge clk) begin
        if (rst) p5_v <= 1'b0;
        else     p5_v <= p4_v;
        p5_rs_lo <= {9'd0, p4_s_lo[15:11]} * {5'd0, p4_a_lo};
        p5_gs_lo <= {9'd0, p4_s_lo[10:5]}  * {6'd0, p4_a_lo};
        p5_bs_lo <= {9'd0, p4_s_lo[4:0]}   * {5'd0, p4_a_lo};
        p5_rd_lo <= {9'd0, p4_d[15:11]}    * {5'd0, p4_ia_lo};
        p5_gd_lo <= {9'd0, p4_d[10:5]}     * {6'd0, p4_ia_lo};
        p5_bd_lo <= {9'd0, p4_d[4:0]}      * {5'd0, p4_ia_lo};
        p5_rs_hi <= {9'd0, p4_s_hi[15:11]} * {5'd0, p4_a_hi};
        p5_gs_hi <= {9'd0, p4_s_hi[10:5]}  * {6'd0, p4_a_hi};
        p5_bs_hi <= {9'd0, p4_s_hi[4:0]}   * {5'd0, p4_a_hi};
        p5_rd_hi <= {9'd0, p4_d[31:27]}    * {5'd0, p4_ia_hi};
        p5_gd_hi <= {9'd0, p4_d[26:21]}    * {6'd0, p4_ia_hi};
        p5_bd_hi <= {9'd0, p4_d[20:16]}    * {5'd0, p4_ia_hi};
        p5_m_lo  <= p4_m_lo;
        p5_m_hi  <= p4_m_hi;
        p5_wq    <= p4_wq;
    end

    // ---- P6 : round, pack -> W FIFO ------------------------------------------
    reg [14:0] p6_r_lo, p6_g_lo, p6_b_lo, p6_r_hi, p6_g_hi, p6_b_hi;
    reg        p6_m_lo, p6_m_hi;
    always @(posedge clk) begin
        if (rst) p6_v <= 1'b0;
        else     p6_v <= p5_v;
        p6_r_lo <= {1'b0, p5_rs_lo} + {1'b0, p5_rd_lo} + 15'd128;
        p6_g_lo <= p5_gs_lo + p5_gd_lo + 15'd128;
        p6_b_lo <= {1'b0, p5_bs_lo} + {1'b0, p5_bd_lo} + 15'd128;
        p6_r_hi <= {1'b0, p5_rs_hi} + {1'b0, p5_rd_hi} + 15'd128;
        p6_g_hi <= p5_gs_hi + p5_gd_hi + 15'd128;
        p6_b_hi <= {1'b0, p5_bs_hi} + {1'b0, p5_bd_hi} + 15'd128;
        p6_m_lo <= p5_m_lo;
        p6_m_hi <= p5_m_hi;
        p6_wq   <= p5_wq;
    end

    // pack; additive results above the channel maximum saturate (a normal blend
    // never gets there: its sum stays below 32 / 64 x 256)
    wire [4:0]  p6_rlo = p6_r_lo[13] ? 5'd31 : p6_r_lo[12:8];
    wire [5:0]  p6_glo = p6_g_lo[14] ? 6'd63 : p6_g_lo[13:8];
    wire [4:0]  p6_blo = p6_b_lo[13] ? 5'd31 : p6_b_lo[12:8];
    wire [4:0]  p6_rhi = p6_r_hi[13] ? 5'd31 : p6_r_hi[12:8];
    wire [5:0]  p6_ghi = p6_g_hi[14] ? 6'd63 : p6_g_hi[13:8];
    wire [4:0]  p6_bhi = p6_b_hi[13] ? 5'd31 : p6_b_hi[12:8];
    wire [31:0] p6_data = {p6_rhi, p6_ghi, p6_bhi, p6_rlo, p6_glo, p6_blo};
    wire [3:0]  p6_strb = {p6_m_hi, p6_m_hi, p6_m_lo, p6_m_lo};
    // D: band mode -> window write (pixel lanes 2*wq[1:0] and +1 of beat wq[13:2])
    wire        p6_win  = bm & p6_v;
    wire [7:0]  p6_wen  = {6'd0, p6_m_hi, p6_m_lo} << {p6_wq[1:0], 1'b0};

    // =========================================================================
    // W FIFO + WRITE : AW per row burst, W beats from the FIFO
    // =========================================================================
    wire [35:0]      wf_dout;
    wire             wf_valid, wf_pop;
    wire [WF_AW+1:0] wf_count;

    blit_fifo #(.DW(36), .AW(WF_AW)) u_wf (
        .clk(clk), .rst(rst),
        .push(p6_v & ~bm), .din({p6_strb, p6_data}), .full(), .space(),
        .pop(wf_pop), .dout(wf_dout), .valid(wf_valid), .count(wf_count)
    );

    // burst length FIFO (AW -> W, generates WLAST)
    reg  [6:0] wl_mem [0:15];
    reg  [4:0] wl_wp, wl_rp;
    wire [4:0] wl_used  = wl_wp - wl_rp;
    wire       wl_full  = wl_used[4];
    wire       wl_empty = (wl_used == 5'd0);
    wire [6:0] wl_head  = wl_mem[wl_rp[3:0]];
    reg  [6:0] w_beat;

    assign m_wvalid = wf_valid & ~wl_empty;
    assign m_wdata  = wf_dout[31:0];
    assign m_wstrb  = wf_dout[35:32];
    assign m_wlast  = (w_beat == wl_head - 7'd1);
    wire   w_hs     = m_wvalid & m_wready;
    assign wf_pop   = w_hs;

    localparam W_IDLE = 2'd0, W_SETUP = 2'd1, W_ISSUE = 2'd2;
    reg  [1:0]  w_state;
    reg  [31:0] w_drow;
    reg  [15:0] w_dstr;
    reg  [10:0] w_w, w_rows;
    reg  [29:0] w_addr;
    reg  [11:0] w_left;
    reg  [WF_AW+1:0] w_uncommitted;     // words in the W FIFO not yet covered by an AW
                                        // (bcount: declared with the C2 wires, FETCH)

    wire        w_dpar   = w_drow[1];
    wire [11:0] w_nout   = ({1'b0, w_w} + {11'd0, w_dpar} + 12'd1) >> 1;
    wire [6:0]  w_to_bnd = 7'd64 - {1'b0, w_addr[5:0]};
    wire [6:0]  w_len    = (w_left < {5'd0, w_to_bnd}) ? w_left[6:0] : w_to_bnd;
    wire        w_issue  = (w_state == W_ISSUE) & ~m_awvalid & ~wl_full & (bcount < r_wrmax) &
                           (w_uncommitted >= {{(WF_AW-5){1'b0}}, w_len});
    wire        w_last_bst = ({5'd0, w_len} == w_left);

    assign h2_pop = (w_state == W_IDLE) & h2_valid;
    wire   w_cmd_done = w_issue & w_last_bst & (w_rows == 11'd1);

    always @(posedge clk) begin
        if (rst) begin
            w_state   <= W_IDLE;
            m_awvalid <= 1'b0;
            m_awaddr  <= 32'd0;
            m_awlen   <= 8'd0;
            wl_wp     <= 5'd0;
            w_drow <= 32'd0; w_dstr <= 16'd0; w_w <= 11'd0; w_rows <= 11'd0;
            w_addr <= 30'd0; w_left <= 12'd0;
        end else begin
            if (m_awvalid & m_awready)
                m_awvalid <= 1'b0;

            case (w_state)
                W_IDLE: begin
                    if (h2_valid) begin
                        w_drow  <= h2_dout[31:0];
                        w_dstr  <= h2_dout[47:32];
                        w_w     <= h2_dout[58:48];
                        w_rows  <= h2_dout[69:59];
                        w_state <= W_SETUP;
                    end
                end

                W_SETUP: begin
                    w_addr  <= w_drow[31:2];
                    w_left  <= w_nout;
                    w_state <= W_ISSUE;
                end

                W_ISSUE: begin
                    if (w_issue) begin
                        m_awvalid <= 1'b1;
                        m_awaddr  <= {w_addr, 2'b00};
                        m_awlen   <= {1'b0, w_len} - 8'd1;
                        wl_mem[wl_wp[3:0]] <= w_len;
                        wl_wp     <= wl_wp + 5'd1;
                        w_addr    <= w_addr + {23'd0, w_len};
                        w_left    <= w_left - {5'd0, w_len};
                        if (w_last_bst) begin
                            if (w_rows == 11'd1) begin
                                w_state <= W_IDLE;
                            end else begin
                                w_rows  <= w_rows - 11'd1;
                                w_drow  <= w_drow + {16'd0, w_dstr};
                                w_state <= W_SETUP;
                            end
                        end
                    end
                end

                default: w_state <= W_IDLE;
            endcase
        end
    end

    // W beat counter, credits, outstanding writes
    wire b_hs = m_bvalid & m_bready;
    always @(posedge clk) begin
        if (rst) begin
            w_beat        <= 7'd0;
            wl_rp         <= 5'd0;
            w_uncommitted <= {(WF_AW+2){1'b0}};
            wf_inflight   <= {(WF_AW+2){1'b0}};
            bcount        <= 8'd0;
        end else begin
            if (w_hs) begin
                if (m_wlast) begin
                    w_beat <= 7'd0;
                    wl_rp  <= wl_rp + 5'd1;
                end else begin
                    w_beat <= w_beat + 7'd1;
                end
            end
            w_uncommitted <= w_uncommitted + {{(WF_AW+1){1'b0}}, p6_v & ~bm}
                           - (w_issue ? {{(WF_AW-5){1'b0}}, w_len} : {(WF_AW+2){1'b0}});
            wf_inflight   <= wf_inflight + {{(WF_AW+1){1'b0}}, c_issue & ~bm}
                           - {{(WF_AW+1){1'b0}}, w_hs};
            bcount        <= bcount + {7'd0, w_issue} + {7'd0, xa_iss} + {7'd0, bn_awi}
                                    + {7'd0, fu_awi} - {7'd0, b_hs};
        end
    end

    // =========================================================================
    // Band mode (D, CAPS[31])
    // =========================================================================
    // A frame between BANDS and ENDB is drawn 16 rows at a time into an on-chip
    // window (8 x 16-bit lanes x WIN_BYTES beats = 16 rows x WIN_BYTES bytes):
    //   phase 1  every record of the segment goes through the Y pipeline and is
    //            appended (8 words, "RES") to the bin of every band it touches;
    //            records outside the frame (another target) are queued normally.
    //   phase 2  per band: the bin is read back, each command is clipped to the
    //            16 rows of the band and drawn into the window (K path: cached
    //            COPY / FILL at 8 px per clock; wide path: R beats straight in;
    //            narrow path: blends read the window, P6 writes it), then the
    //            window is written to DDR with 16-beat bursts and cleared.
    // ---- Y pipeline: rows of a command (shared by both phases) -------------------
    // y = (dst - frame) / stride = ((dst - frame) >> 4) * BAND_RECIP >> 24 with
    // BAND_RECIP = floor(2^24 / (stride / 16)) + 1: exact for offsets < 2 MB and
    // strides 256..1280, and for strides up to 2560 at least for rows 0..720 (all
    // 2^17 offsets checked for every stride; the result is never too small, so a
    // row below a frame of <= 720 rows can not land inside it). MAXB = 45. Stages: Y0 offset, Y1 two DSP-sized
    // multiplies, Y2 add -> y, Y3 row end / frame checks, Y4 clip to the band +
    // kind, Y5 row skip multiplies, Y6 adds (output). The decoder writes into a
    // 2-entry queue, so it only sees a registered "full".
    wire         yq_full, yq_v;
    wire [CQ_W-1:0] yq_r;
    wire [2:0]   yq_cnt;
    reg          y0_v, y1_v, y2_v, y3_v, y4_v, y5_v, y6_v;
    reg  [CQ_W-1:0] y0_r, y1_r, y2_r, y3_r, y4_r, y5_r, y6_r;
    reg  [31:0]  y0_off;
    reg          y1_in, y2_in;
    reg  [27:0]  y1_ph;                 // n * RECIP[20:10]
    reg  [26:0]  y1_pl;                 // n * RECIP[9:0]
    reg  [13:0]  y2_y, y3_y, y3_end, y3_bot;
    reg          y3_inw, y3_nul, y3_rok;
    reg  [1:0]   y4_kind, y5_kind, y6_kind;   // 0 drop, 1 command queue, 2 bins
    reg  [10:0]  y4_k0, y4_n, y5_n;
    reg  [5:0]   y4_b0, y4_b1, y5_b0, y5_b1, y6_b0, y6_b1;
    reg  [26:0]  y5_dad, y5_sad, y5_aad;
    wire         y_any  = (yq_cnt != 3'd0) | y0_v | y1_v | y2_v | y3_v | y4_v | y5_v | y6_v;
    wire         y_in_v = (l_push & (bc_state == BC_P1)) | res_push;
    wire [CQ_W-1:0] y_in_r = res_push ? res_rec[CQ_W-1:0] : cq_ldin;

    wire         bn_ready;
    assign       y_cq_push = y6_v & (y6_kind == 2'd1) & ~cq_full;
    wire         y_bin   = y6_v & (y6_kind == 2'd2) & bn_ready;
    wire         y_take  = y6_v & ((y6_kind == 2'd0) | y_cq_push | y_bin);
    wire         y_adv   = ~y6_v | y_take;
    assign       y_in_ok = ~yq_full;
    assign       y_out   = y6_r;

    blit_rfifo #(.DW(CQ_W), .AW(1)) u_yq (
        .clk(clk), .rst(rst),
        .push(y_in_v), .din(y_in_r), .full(yq_full), .space(),
        .pop(y_adv & yq_v), .dout(yq_r), .valid(yq_v), .count(yq_cnt)
    );

    wire [37:0] y1_sum  = {y1_ph, 10'd0} + {11'd0, y1_pl};
    // Y2 -> Y3
    wire [10:0] y2_w    = y2_r[154:144];
    wire [10:0] y2_h    = y2_r[165:155];
    wire [13:0] y2_lim  = {4'd0, bg_nb, 4'd0};             // rows of the frame
    // Y3 -> Y4: kind + bands (phase 1) / rows inside band bc_b (phase 2)
    wire [13:0] y3_r0   = {4'd0, bc_b, 4'd0};
    wire [13:0] y3_r1   = y3_r0 + 14'd16;
    wire [13:0] y3_lm1  = {4'd0, bg_nb, 4'd0} - 14'd1;
    wire [13:0] y3_s    = (y3_y > y3_r0) ? y3_y : y3_r0;
    wire [13:0] y3_e    = (y3_end < y3_r1) ? y3_end : y3_r1;
    wire [13:0] y3_bl   = (y3_bot < y3_lm1) ? y3_bot : y3_lm1;
    wire        y3_hit  = (y3_e > y3_s);
    wire [13:0] y3_k0   = y3_s - y3_y;
    wire [1:0]  y3_kind = (bc_state == BC_BAND) ? ((y3_nul | ~y3_inw | ~y3_hit) ? 2'd0 : 2'd1) :
                          y3_nul ? (y3_r[211] ? 2'd1 : 2'd0) :    // size 0: only an INV matters
                          y3_inw ? (y3_rok ? 2'd2 : 2'd0) : 2'd1;
    wire        y3_bad  = (bc_state != BC_BAND) & ~y3_nul & y3_inw & ~y3_rok;

    always @(posedge clk) begin
        if (rst) begin
            y0_v <= 1'b0; y1_v <= 1'b0; y2_v <= 1'b0; y3_v <= 1'b0;
            y4_v <= 1'b0; y5_v <= 1'b0; y6_v <= 1'b0;
        end else if (y_adv) begin
            y0_v <= yq_v; y1_v <= y0_v; y2_v <= y1_v; y3_v <= y2_v;
            y4_v <= y3_v; y5_v <= y4_v; y6_v <= y5_v;
        end
        if (y_adv) begin
            y0_r    <= yq_r;
            y0_off  <= yq_r[31:0] - bc_fb;
            y1_r    <= y0_r;
            y1_in   <= (y0_off[31:21] == 11'd0);
            y1_ph   <= y0_off[20:4] * bg_recip[20:10];
            y1_pl   <= y0_off[20:4] * bg_recip[9:0];
            y2_r    <= y1_r;
            y2_in   <= y1_in;
            y2_y    <= y1_sum[37:24];
            y3_r    <= y2_r;
            y3_y    <= y2_y;
            y3_end  <= y2_y + {3'd0, y2_h};
            y3_bot  <= y2_y + {3'd0, y2_h} - 14'd1;
            y3_inw  <= y2_in & (y2_y < y2_lim);
            y3_nul  <= (y2_w == 11'd0) | (y2_h == 11'd0);
            y3_rok  <= (y2_h == 11'd1) | (y2_r[47:32] == bg_s);   // rows on the frame stride
            y4_r    <= y3_r;
            y4_kind <= y3_kind;
            y4_k0   <= y3_k0[10:0];
            y4_n    <= y3_e[10:0] - y3_s[10:0];
            y4_b0   <= y3_y[9:4];
            y4_b1   <= y3_bl[9:4];
            y5_r    <= y4_r;                         // skip the k0 rows above the band (phase 2)
            y5_kind <= y4_kind;
            y5_n    <= y4_n;
            y5_b0   <= y4_b0;
            y5_b1   <= y4_b1;
            y5_dad  <= y4_k0 * y4_r[47:32];
            y5_sad  <= y4_k0 * y4_r[95:80];
            y5_aad  <= y4_k0 * y4_r[143:128];
            y6_kind <= y5_kind;
            y6_b0   <= y5_b0;
            y6_b1   <= y5_b1;
            y6_r    <= (bc_state == BC_BAND) ?
                       {y5_r[CQ_W-1:166], y5_n, y5_r[154:128], y5_r[127:96] + {5'd0, y5_aad},
                        y5_r[95:80], y5_r[79:48] + {5'd0, y5_sad}, y5_r[47:32],
                        y5_r[31:0] + {5'd0, y5_dad}} : y5_r;
        end
    end

    // ---- binner (phase 1): one 2-beat x_* burst per band the record touches -----
    // bin of band b = BAND_BIN + b << shift; record n of it at + n * 32:
    //   beat 0 = {cmd[95:0], RES header 0x50000000}, beat 1 = {11'd0, cmd[212:96]}
    // AW side: one AW per band of the record (every 2nd clock); each AW puts the
    // record into a small queue, the W side sends its 2 beats from there.
    reg          bn_v, bn_awq, bn_wbeat;
    reg  [CQ_W-1:0] bn_r;
    reg  [5:0]   bn_ab, bn_b1;
    reg  [31:0]  bn_awaddr;
    reg  [15:0]  bn_cnt [0:MAXB-1];     // records in the bin of band b
    reg  [MAXB-1:0] bn_cv;              // bn_cnt[b] belongs to this frame (else 0)
    wire         bn_own  = bc_p1 & ~x_mode;       // x_* belongs to the binner
    reg  [31:0]  bn_base;               // bin of band bn_ab
    reg  [15:0]  bn_c;                  // records in it (read one clock after bn_ab changed)
    reg          bn_cok;                // bn_c belongs to bn_ab
    wire         bn_full = ((bn_c & bg_capm) != 16'd0);     // 2^(shift - 5) records in it
    wire         bn_wqf;                // W record queue full
    wire [CQ_W-1:0] bn_wr;
    wire         bn_wv;
    wire [2:0]   bn_wqc;
    wire         bn_iss  = bn_own & bn_v & bn_cok & ~bn_awq & ~bn_wqf & (bcount < r_wrmax);
    wire         bn_fin  = bn_iss & (bn_ab == bn_b1);    // last band of the record
    assign       bn_ready = ~bn_v;
    assign       bn_awi  = bn_iss & ~bn_full;
    wire         bn_whs  = bn_own & bn_wv & x_wready;
    wire [127:0] bn_wd   = bn_wbeat ? {11'd0, bn_wr[CQ_W-1:96]} : {bn_wr[95:0], 32'h50000000};
    wire         bn_busy = bn_v | bn_awq | (bn_wqc != 3'd0);

    blit_rfifo #(.DW(CQ_W), .AW(1)) u_bnw (
        .clk(clk), .rst(rst),
        .push(bn_awi), .din(bn_r), .full(bn_wqf), .space(),
        .pop(bn_whs & bn_wbeat), .dout(bn_wr), .valid(bn_wv), .count(bn_wqc)
    );

    always @(posedge clk) begin
        if (rst) begin
            bn_v <= 1'b0; bn_awq <= 1'b0; bn_wbeat <= 1'b0;
            bn_ab <= 6'd0; bn_b1 <= 6'd0; bn_awaddr <= 32'd0; bn_base <= 32'd0;
            bn_cv <= {MAXB{1'b0}}; bn_r <= {CQ_W{1'b0}}; bn_c <= 16'd0; bn_cok <= 1'b0;
        end else begin
            if (bn_awq & x_awready & bn_own)
                bn_awq <= 1'b0;
            if (bn_iss) begin
                if (~bn_full) begin
                    bn_awq        <= 1'b1;
                    bn_awaddr     <= bn_base + {11'd0, bn_c, 5'd0};
                    bn_cnt[bn_ab] <= bn_c + 16'd1;
                    bn_cv         <= bn_cv | ({{(MAXB-1){1'b0}}, 1'b1} << bn_ab);
                end
                if (~bn_fin)                    // stays inside 0..MAXB-1 (bn_cnt index)
                    bn_ab <= bn_ab + 6'd1;
                bn_base <= bn_base + bg_binsz;
                if (bn_fin)
                    bn_v <= 1'b0;
            end
            if (y_bin) begin
                bn_v    <= 1'b1;
                bn_r    <= y6_r;
                bn_ab   <= y6_b0;
                bn_b1   <= y6_b1;
                bn_base <= bg_bin + ({26'd0, y6_b0} << bg_sh);
            end
            bn_c   <= bn_cv[bn_ab] ? bn_cnt[bn_ab] : 16'd0;
            bn_cok <= ~(bn_iss | y_bin);            // bn_ab / its count change this clock
            if (bands_go)
                bn_cv <= {MAXB{1'b0}};
            if (bn_whs)
                bn_wbeat <= ~bn_wbeat;
        end
    end

    // ---- band sequence --------------------------------------------------------
    reg          win_clean;             // window cleared once since reset
    reg  [11:0]  cl_a;                  // BC_CLR: beat being cleared
    wire         fu_done;
    wire         fu_awq_w;
    wire [5:0]   bc_last   = bg_nb - 6'd1;          // bg_nb 1..MAXB
    wire         p1_quiet  = (d_state == D_HDR) & ~y_any & ~bn_busy & (cq_count == {(CQ_AW+2){1'b0}}) &
                             (f_state == F_IDLE) & drained & k_idle;
    wire         band_quiet = ~dma_active & (d_state == D_HDR) & ~y_any &
                              (cq_count == {(CQ_AW+2){1'b0}}) & (f_state == F_IDLE) & n_idle & k_idle;
    wire         bc_s0 = ((bc_state == BC_P1E) & p1_quiet & win_clean) |
                         ((bc_state == BC_CLR) & (cl_a == WIN_BYTES - 1));
    wire         bc_sn = (bc_state == BC_FLUSH) & fu_done & (bc_b != bc_last);
    assign       bc_bcall = bc_s0 | bc_sn;
    // the next band's bin, worked out ahead (inputs are stable for > 1 clock
    // before bc_bcall: P1E lasts >= 2 clocks, CLR / FLUSH >= 256); bc_nx stays a
    // valid band (bn_cnt index) also while the last band is written back
    wire         bc_fl = bc_fl_r;                    // = (bc_state == BC_FLUSH)
    wire [5:0]   bc_nx = (bc_fl & (bc_b != bc_last)) ? (bc_b + 6'd1) : 6'd0;
    reg  [31:0]  bc_binp;               // bin of band bc_b
    reg  [31:0]  bc_ba;                 // bin of band bc_nx
    reg  [29:0]  bc_be;                 // its end (word address)
    wire [31:0]  bc_bai = bc_fl ? (bc_binp + bg_binsz) : bg_bin;
    // bc_nx is stable for >= 256 clocks before bc_bcall (or 0 for a long time): its bin
    // count is looked up with a registered copy (timing: bn_cnt is a 45:1 mux)
    reg  [5:0]   bc_nxq;
    wire [15:0]  bc_nc  = bn_cv[bc_nxq] ? bn_cnt[bc_nxq] : 16'd0;
    assign       bc_bin_a = bc_ba[31:2];
    assign       bc_bin_e = bc_be;
    assign       bc_ret   = (bc_state == BC_FIN) & (bcount == 8'd0) & ~fu_awq_w;

    // next band state (registered decodes bc_p1_r / bm_r / bc_fl_r are written from it)
    reg  [2:0]   bc_nst;
    always @(*) begin
        bc_nst = bc_state;
        case (bc_state)
            BC_OFF:   if (bands_go) bc_nst = BC_P1;
            BC_P1:    if (endb_call) bc_nst = BC_P1E;
            BC_P1E:   if (p1_quiet & ~win_clean) bc_nst = BC_CLR;
            BC_CLR:   bc_nst = BC_CLR;
            BC_BAND:  if (band_quiet) bc_nst = BC_FLUSH;
            BC_FLUSH: if (fu_done & (bc_b == bc_last)) bc_nst = BC_FIN;
            BC_FIN:   if (bc_ret) bc_nst = BC_OFF;
            default:  bc_nst = BC_OFF;
        endcase
        if (bc_bcall) bc_nst = BC_BAND;
    end

    always @(posedge clk) begin
        if (rst) begin
            bc_p1_r <= 1'b0; bm_r <= 1'b0; bc_fl_r <= 1'b0; bc_nxq <= 6'd0;
        end else begin
            bc_p1_r <= (bc_nst == BC_P1) | (bc_nst == BC_P1E);
            bm_r    <= (bc_nst == BC_BAND);
            bc_fl_r <= (bc_nst == BC_FLUSH);
            bc_nxq  <= bc_nx;
        end
    end

    always @(posedge clk) begin
        if (rst) begin
            bc_state  <= BC_OFF;
            bc_b      <= 6'd0;
            bc_fb     <= 32'd0;
            bc_wbase  <= 32'd0;
            bc_frames <= 32'd0;
            bc_binp   <= 32'd0;
            bc_ba     <= 32'd0;
            bc_be     <= 30'd0;
            win_clean <= 1'b0;
            cl_a      <= 12'd0;
            bovf      <= 1'b0;
            berr      <= 1'b0;
        end else begin
            case (bc_state)
                BC_OFF:   if (bands_go) begin
                              bc_fb    <= {lw[31:8], 8'd0};    // 256-byte aligned
                              bc_state <= BC_P1;
                          end
                BC_P1:    if (endb_call) bc_state <= BC_P1E;
                BC_P1E:   if (p1_quiet & ~win_clean) begin
                              cl_a     <= 12'd0;
                              bc_state <= BC_CLR;
                          end
                BC_CLR:   begin
                              cl_a <= cl_a + 12'd1;
                              if (cl_a == WIN_BYTES - 1) win_clean <= 1'b1;
                          end
                BC_BAND:  if (band_quiet) bc_state <= BC_FLUSH;
                BC_FLUSH: if (fu_done & (bc_b == bc_last)) bc_state <= BC_FIN;
                BC_FIN:   if (bc_ret) begin
                              bc_frames <= bc_frames + 32'd1;
                              bc_state  <= BC_OFF;
                          end
                default:  bc_state <= BC_OFF;
            endcase
            bc_ba <= bc_bai;
            bc_be <= bc_bai[31:2] + {11'd0, bc_nc, 3'd0};
            if (bc_bcall) begin
                bc_state <= BC_BAND;
                bc_b     <= bc_nx;
                bc_binp  <= bc_ba;
                bc_wbase <= bc_sn ? (bc_wbase + {12'd0, bg_s, 4'd0}) : bc_fb;
            end
            if (apb_wr & (apb_reg == 6'h25))        // new geometry: clear the whole window again
                win_clean <= 1'b0;
            if (bn_iss & bn_full)
                bovf <= 1'b1;
            else if (err_clr)
                bovf <= 1'b0;
            if (y_adv & y3_v & y3_bad)
                berr <= 1'b1;
            else if (err_clr)
                berr <= 1'b0;
        end
    end

    // ---- flush: window -> DDR (16-beat bursts), each beat cleared after its read --
    reg  [11:0]  fu_rb, fu_wc;          // window beats read / W beats accepted
    reg  [7:0]   fu_ab;                 // AW bursts issued
    reg          fu_awq, fu_rq, fu_clr;
    reg  [31:0]  fu_awaddr;
    reg  [11:0]  fu_clra;
    wire         fu_on   = bc_fl_r;              // = (bc_state == BC_FLUSH)
    wire         fu_go   = (bc_state == BC_BAND) & band_quiet;
    wire [11:0]  fu_nb   = bg_s[11:0];  // beats per band = 16 rows x stride / 16 bytes
    wire [7:0]   fu_nbu  = bg_s[11:4];  // bursts per band
    wire [127:0] fu_q;
    wire         fu_qv;
    wire [3:0]   fu_qc;
    wire         fu_whs  = fu_on & fu_qv & x_wready;
    wire         fu_rd   = fu_on & (fu_rb != fu_nb) & (fu_qc + {3'd0, fu_rq} < 4'd4);
    wire [7:0]   fu_cb   = fu_ab - fu_wc[11:4];           // bursts issued, W not complete
    assign       fu_awi  = fu_on & ~fu_awq & (fu_ab != fu_nbu) & (fu_cb < 8'd2) & (bcount < r_wrmax);
    assign       fu_done = fu_on & (fu_wc == fu_nb);
    assign       fu_awq_w = fu_awq;

    blit_rfifo #(.DW(128), .AW(2)) u_fu (
        .clk(clk), .rst(rst),
        .push(fu_rq), .din(wn_q), .full(), .space(),
        .pop(fu_whs), .dout(fu_q), .valid(fu_qv), .count(fu_qc)
    );

    always @(posedge clk) begin
        if (rst) begin
            fu_rb <= 12'd0; fu_wc <= 12'd0; fu_ab <= 8'd0;
            fu_awq <= 1'b0; fu_rq <= 1'b0; fu_clr <= 1'b0;
            fu_awaddr <= 32'd0; fu_clra <= 12'd0;
        end else begin
            if (fu_awq & x_awready & fu_on)
                fu_awq <= 1'b0;
            if (fu_go) begin
                fu_rb <= 12'd0; fu_wc <= 12'd0; fu_ab <= 8'd0;
            end else begin
                if (fu_rd)  fu_rb <= fu_rb + 12'd1;
                if (fu_whs) fu_wc <= fu_wc + 12'd1;
                if (fu_awi) begin
                    fu_awq    <= 1'b1;
                    fu_awaddr <= bc_wbase + {16'd0, fu_ab, 8'd0};
                    fu_ab     <= fu_ab + 8'd1;
                end
            end
            fu_rq   <= fu_rd;
            fu_clr  <= fu_rd;
            fu_clra <= fu_rb;
        end
    end

    // ---- x_* owner: flush (BC_FLUSH), binner (phase 1), else the wide path ---------
    assign x_awvalid = fu_on ? fu_awq    : bn_own ? bn_awq    : xa_awv;
    assign x_awaddr  = fu_on ? fu_awaddr : bn_own ? bn_awaddr : xa_awaddr;
    assign x_awlen   = fu_on ? 4'd15     : bn_own ? 4'd1      : xa_awlen;
    assign x_wvalid  = fu_on ? fu_qv     : bn_own ? bn_wv     : xw_vi;
    assign x_wdata   = fu_on ? fu_q      : bn_own ? bn_wd     : xw_data;
    assign x_wstrb   = (fu_on | bn_own) ? 16'hFFFF : xw_strb;
    assign x_wlast   = fu_on ? (fu_wc[3:0] == 4'hF) : bn_own ? bn_wbeat : xw_last;

    // ---- K path: cached COPY (opaque / keyed) and opaque FILL, 8 px per clock ------
    // WALK   one token per clock: a source cache line of the current row (lines
    //        already checked for the row before are skipped), or the row alone
    //        (FILL, or no new line). Stage B looks the line up in the tag RAM;
    //        a miss reads the line (router: cache only). The last token of a row
    //        pushes the row (geometry + fill count) into the row queue.
    // COMPOSE one output beat per clock: the cache data RAMs give two neighbouring
    //        16-byte beats, a 256 -> 128 bit shift aligns them to the window lanes,
    //        colour key / row edges give the lane enables. A row starts once the
    //        fills issued up to its last line are written (kf_done), and a line
    //        another row may still need is only replaced when the queue is empty.
    // Everything the token needs is a register: the row values (first line to
    // check, last line, row-only flag) are worked out when the row before ends.
    reg         kw_v, kw_op, kw_keyen, kw_ro, kw_add;
    reg  [15:0] kw_key, kw_color, kw_sstr, kw_dstr;
    reg  [10:0] kw_w, kw_rows;
    reg  [31:0] kw_sr, kw_er, kw_dr;    // source row: first byte, last byte; destination row
    reg  [26:0] kw_Lc;                  // line of this token
    wire [26:0] kw_L1   = kw_er[31:5];                       // last line of the row
    wire        kw_rl   = kw_ro | (kw_Lc == kw_L1);          // last token of the row
    // next row: its first line was already checked if it is this row's last line
    // (rows packed back to back share a line), then start one line later, or -
    // a row inside that one line - no line at all (row-only token)
    wire [31:0] kw_nsr  = kw_sr + {16'd0, kw_sstr};
    wire [31:0] kw_ner  = kw_er + {16'd0, kw_sstr};
    wire [26:0] kw_nL0  = kw_nsr[31:5];
    wire        kw_ndup = (kw_nL0 == kw_L1);
    wire [31:0] kw_dx   = kw_dr - bc_wbase;
    wire [11:0] kw_nb   = ({9'd0, kw_dr[3:1]} + {1'b0, kw_w} + 12'd7) >> 3;
    wire [11:0] kw_ll   =  {9'd0, kw_dr[3:1]} + {1'b0, kw_w} - 12'd1;
    // row info {op, keyen, key, color, window beat, dst lane, beats, last lane,
    //           source beat (cache index), source lane}
    wire [74:0] kw_info = {kw_add, kw_op, kw_keyen, kw_key, kw_color, kw_dx[15:4], kw_dr[3:1],
                           kw_nb[8:0], kw_ll[2:0], kw_sr[13:4], kw_sr[3:1]};
    wire [31:0] kq_se   = q_src + {20'd0, q_w, 1'b0} - 32'd1;    // first row of a new command
    wire        kb_adv;
    wire        kw_emit = kw_v & kb_adv;
    assign      kw_free = ~kw_v;

    always @(posedge clk) begin
        if (rst) begin
            kw_v <= 1'b0; kw_op <= 1'b0; kw_keyen <= 1'b0; kw_ro <= 1'b0; kw_add <= 1'b0;
            kw_key <= 16'd0; kw_color <= 16'd0; kw_sstr <= 16'd0; kw_dstr <= 16'd0;
            kw_w <= 11'd0; kw_rows <= 11'd0; kw_sr <= 32'd0; kw_er <= 32'd0; kw_dr <= 32'd0;
            kw_Lc <= 27'd0;
        end else if (k_start) begin
            kw_v     <= 1'b1;
            kw_op    <= q_op;
            kw_keyen <= q_keyen & q_op;
            kw_add   <= q_add;
            kw_key   <= q_key;
            kw_color <= q_color;
            kw_sstr  <= q_sstr;
            kw_dstr  <= q_dstr;
            kw_w     <= q_w;
            kw_rows  <= q_h;
            kw_sr    <= q_src;
            kw_er    <= kq_se;
            kw_dr    <= q_dst;
            kw_Lc    <= q_src[31:5];
            kw_ro    <= ~q_op;                  // FILL: no source lines
        end else if (kw_emit) begin
            if (kw_rl) begin
                if (kw_rows == 11'd1) begin
                    kw_v <= 1'b0;
                end else begin
                    kw_rows <= kw_rows - 11'd1;
                    kw_sr   <= kw_nsr;
                    kw_er   <= kw_ner;
                    kw_dr   <= kw_dr + {16'd0, kw_dstr};
                    kw_Lc   <= kw_ndup ? (kw_nL0 + 27'd1) : kw_nL0;
                    kw_ro   <= ~kw_op | (kw_ndup & (kw_ner[31:5] == kw_L1));
                end
            end else begin
                kw_Lc <= kw_Lc + 27'd1;
            end
        end
    end

    // stage B: tag lookup (the tag RAM was read in the clock the token was made)
    reg         kb_v, kb_line, kb_rl, kb_res, kb_hit, kb_ev;
    reg  [26:0] kb_L;
    reg  [74:0] kb_info;
    reg  [7:0]  kf_iss, kf_done;        // cache line fills issued / written (any path)
    wire        kq_full, kq_valid, kc_pop;
    wire [3:0]  kq_count;
    wire [82:0] kq_dout;
    reg         kc_v, k1_v, k2_v, k3_v;
    // Timing (2026-10-11): row-queue room and "compose quiet" are registered,
    // conservatively (a push in the same clock counts, a pop is ignored):
    //   kq_room  = at most 3 of the 4 queue RAM entries used -> a push fits
    //   kc_quiet = no row in the queue / compose (eviction may replace a line)
    reg         kq_room, kc_quiet;
    wire        kb_look  = kb_v & ~kb_res & c_qok & (c_qidx == kb_L[8:0]);
    wire        kb_hn    = c_vq & (c_tq == kb_L[26:9]);
    // the lookup result is registered (kb_res) before anything depends on it:
    // a line token takes 2 clocks in stage B, a row-only token 1
    wire        kb_known = kb_res;
    wire        kb_ishit = kb_hit;
    wire        kb_isev  = kb_ev;                        // (a miss on) a valid line: evict
    wire        kb_qok   = ~kb_rl | kq_room;
    assign      kb_fill_raw = kb_v & kb_known & ~kb_ishit & (~kb_isev | kc_quiet) & kb_qok &
                              ~m_arvalid & rd_room & rf_ok & ~bd_full;
    // AR priority: an urgent list fetch (its FIFO ran dry) first, then the K fill
    assign      kb_fill  = kb_fill_raw & ~(l_len_v & l_urgent);
    assign      kb_lw    = kb_L;
    wire        kb_go    = kb_v & kb_known & (kb_ishit | kb_fill) & kb_qok;
    assign      kb_adv   = ~kb_v | kb_go;
    wire        kq_push  = kb_go & kb_rl;
    wire        k_hitev  = kb_go & kb_line & kb_ishit;
    assign      k_act    = kw_v | kb_v;
    assign      k_ra     = (kb_v & ~kb_res) ? kb_L[8:0] : kw_Lc[8:0];
    wire [7:0]  kq_fs    = kf_iss + {7'd0, kb_fill};    // fills up to this token

    always @(posedge clk) begin
        if (rst) begin
            kb_v <= 1'b0; kb_line <= 1'b0; kb_rl <= 1'b0; kb_res <= 1'b0;
            kb_hit <= 1'b0; kb_ev <= 1'b0; kb_L <= 27'd0; kb_info <= 75'd0;
            kf_iss <= 8'd0; kf_done <= 8'd0;
            kq_room <= 1'b0; kc_quiet <= 1'b0;
        end else begin
            if (kw_emit) begin
                kb_v    <= 1'b1;
                kb_line <= ~kw_ro;
                kb_L    <= kw_Lc;
                kb_rl   <= kw_rl;
                kb_info <= kw_info;
                kb_res  <= kw_ro;               // row-only: nothing to look up
                kb_hit  <= 1'b1;
                kb_ev   <= 1'b0;
            end else if (kb_go) begin
                kb_v    <= 1'b0;
            end else if (kb_look) begin
                kb_res  <= 1'b1;
                kb_hit  <= kb_hn;
                kb_ev   <= c_vq & ~kb_hn;
            end
            kf_iss  <= kf_iss + {7'd0, c_we};
            kq_room  <= ({1'b0, kq_count} + {4'd0, kq_push}) <= 5'd3;
            kc_quiet <= (kq_count == 4'd0) & ~kq_push & ~kc_v & ~k1_v;
            kf_done <= kf_done + {7'd0, bd_pop & t_fill};
        end
    end

    blit_rfifo #(.DW(83), .AW(2)) u_kq (
        .clk(clk), .rst(rst),
        .push(kq_push), .din({kq_fs, kb_info}), .full(kq_full), .space(),
        .pop(kc_pop), .dout(kq_dout), .valid(kq_valid), .count(kq_count)
    );

    // compose
    reg  [74:0] kc_i;
    reg  [11:0] kc_war;                 // window beat of the current output beat
    reg  [11:0] k1_wa, k2_wa, k3_wa;
    reg  [8:0]  kc_k;                   // output beat of the row
    wire [2:0]  kc_slo   = kc_i[2:0];
    wire [9:0]  kc_sb    = kc_i[12:3];
    wire [2:0]  kc_ll    = kc_i[15:13];
    wire [8:0]  kc_nob   = kc_i[24:16];
    wire [2:0]  kc_dlo   = kc_i[27:25];
    wire [11:0] kc_wb    = kc_i[39:28];
    wire [15:0] kc_color = kc_i[55:40];
    wire [15:0] kc_key   = kc_i[71:56];
    wire        kc_keyen = kc_i[72];
    wire        kc_op    = kc_i[73];
    wire        kc_add   = kc_i[74];
    wire        kc_last  = (kc_k == kc_nob - 9'd1);
    wire [7:0]  kc_fd    = kf_done - kq_dout[82:75];
    wire        kc_fok   = ~kc_fd[7];                   // fills of the row are written
    // additive beats read the window (address kc_war, data in K1): a beat whose
    // window beat is still being written by K1..K3 waits (overlapping sprites)
    wire        kc_hz    = kc_v & kc_add & ((k1_v & (k1_wa == kc_war)) | (k2_v & (k2_wa == kc_war)) |
                                            (k3_v & (k3_wa == kc_war)));
    assign      kc_pop   = kq_valid & kc_fok & (~kc_v | (kc_last & ~kc_hz));
    // out beat k = source pixels from beat (sb + k + floor((slo - dlo) / 8)) on,
    // shifted by (slo - dlo) mod 8 lanes
    wire [9:0]  kc_ja    = kc_sb + {1'b0, kc_k} - {9'd0, (kc_slo < kc_dlo)};
    wire [2:0]  kc_sh    = kc_slo - kc_dlo;
    assign      k_rd     = kc_v;
    assign      k_oa     = kc_ja[9:1];
    assign      k_ea     = kc_ja[9:1] + {8'd0, kc_ja[0]};
    wire [7:0]  kc_ml    = (kc_k == 9'd0) ? (8'hFF << kc_dlo) : 8'hFF;
    wire [7:0]  kc_mh    = kc_last ? (8'hFF >> (3'd7 - kc_ll)) : 8'hFF;

    reg         k1_sel, k1_op, k1_keyen, k2_ke, k1_add, k2_add;
    reg  [2:0]  k1_sh;
    reg  [15:0] k1_key, k1_color, k2_key;
    reg  [7:0]  k1_m, k2_m, k3_en;
    reg  [127:0] k2_d, k3_d, k2_dst;
    // K1: cache data out -> 256 -> 128 bit shift (window data of additive beats).
    // K2: colour key, saturating add.  K3: window write.
    wire [127:0] k1_e  = {cd3_q, cd2_q, cd1_q, cd0_q};     // even beat of the line
    wire [127:0] k1_o  = {cd7_q, cd6_q, cd5_q, cd4_q};     // odd beat
    wire [255:0] k1_pr = k1_sel ? {k1_e, k1_o} : {k1_o, k1_e};
    wire [255:0] k1_sf = k1_pr >> {k1_sh, 4'd0};
    wire [127:0] k1_px = k1_op ? k1_sf[127:0] : {8{k1_color}};
    wire [15:0]  k2_s0, k2_s1, k2_s2, k2_s3, k2_s4, k2_s5, k2_s6, k2_s7;   // dst + src, saturated
    blit_add565 u_ka0 (.a(k2_dst[15:0]), .b(k2_d[15:0]), .y(k2_s0));
    blit_add565 u_ka1 (.a(k2_dst[31:16]), .b(k2_d[31:16]), .y(k2_s1));
    blit_add565 u_ka2 (.a(k2_dst[47:32]), .b(k2_d[47:32]), .y(k2_s2));
    blit_add565 u_ka3 (.a(k2_dst[63:48]), .b(k2_d[63:48]), .y(k2_s3));
    blit_add565 u_ka4 (.a(k2_dst[79:64]), .b(k2_d[79:64]), .y(k2_s4));
    blit_add565 u_ka5 (.a(k2_dst[95:80]), .b(k2_d[95:80]), .y(k2_s5));
    blit_add565 u_ka6 (.a(k2_dst[111:96]), .b(k2_d[111:96]), .y(k2_s6));
    blit_add565 u_ka7 (.a(k2_dst[127:112]), .b(k2_d[127:112]), .y(k2_s7));
    wire [127:0] k2_sum = {k2_s7, k2_s6, k2_s5, k2_s4, k2_s3, k2_s2, k2_s1, k2_s0};
    wire [7:0]   k2_km = {k2_ke & (k2_d[127:112] == k2_key), k2_ke & (k2_d[111:96] == k2_key),
                          k2_ke & (k2_d[95:80]   == k2_key), k2_ke & (k2_d[79:64]  == k2_key),
                          k2_ke & (k2_d[63:48]   == k2_key), k2_ke & (k2_d[47:32]  == k2_key),
                          k2_ke & (k2_d[31:16]   == k2_key), k2_ke & (k2_d[15:0]   == k2_key)};

    always @(posedge clk) begin
        if (rst) begin
            kc_v <= 1'b0; k1_v <= 1'b0; k2_v <= 1'b0; k3_v <= 1'b0;
            kc_i <= 75'd0; kc_k <= 9'd0; kc_war <= 12'd0;
        end else begin
            k1_v <= kc_v & ~kc_hz;               // a waiting beat leaves a bubble
            k2_v <= k1_v;
            k3_v <= k2_v;
            if (kc_pop) begin
                kc_v   <= 1'b1;
                kc_i   <= kq_dout[74:0];
                kc_k   <= 9'd0;
                kc_war <= kq_dout[39:28];
            end else if (kc_v & ~kc_hz) begin
                if (kc_last) kc_v <= 1'b0;
                else begin
                    kc_k   <= kc_k + 9'd1;
                    kc_war <= kc_war + 12'd1;
                end
            end
        end
        k1_sel   <= kc_ja[0];
        k1_sh    <= kc_sh;
        k1_op    <= kc_op;
        k1_keyen <= kc_keyen;
        k1_key   <= kc_key;
        k1_color <= kc_color;
        k1_wa    <= kc_war;
        k1_add   <= kc_add;
        k1_m     <= kc_ml & kc_mh;
        k2_wa    <= k1_wa;
        k2_d     <= k1_px;
        k2_dst   <= wn_q;                    // window beat k1_wa (read in the kc clock)
        k2_add   <= k1_add;
        k2_m     <= k1_m;
        k2_ke    <= k1_op & k1_keyen;
        k2_key   <= k1_key;
        k3_wa    <= k2_wa;
        k3_d     <= k2_add ? k2_sum : k2_d;
        k3_en    <= k2_m & ~k2_km;
    end

    assign k_idle = ~kw_v & ~kb_v & (kq_count == 4'd0) & ~kc_v & ~k1_v & ~k2_v & ~k3_v;

    // ---- window: 8 lanes x 16 bit x WIN_BYTES beats (one writer at a time) -------
    wire         wn_x  = bm & x_mode & xr_v;                     // wide R beat
    wire [11:0]  wn_wa = k3_v ? k3_wa : p6_win ? p6_wq[13:2] : wn_x ? xw_wb :
                         fu_clr ? fu_clra : cl_a;
    wire [7:0]   wn_we = k3_v ? k3_en : p6_win ? p6_wen : wn_x ? xw_pm :
                         (fu_clr | (bc_state == BC_CLR)) ? 8'hFF : 8'h00;
    wire [127:0] wn_wd = k3_v ? k3_d : p6_win ? {4{p6_data}} : wn_x ? xr_d : 128'd0;
    // read port: flush, else the K path (additive beats), else the narrow path
    // (K and narrow never run at the same time)
    wire [11:0]  wn_ra = fu_on ? fu_rb : kc_v ? kc_war : c_wq[13:2];
    blit_win #(.PARTS((WIN_BYTES + 511) / 512)) u_win (.clk(clk), .we(wn_we), .wa(wn_wa), .wd(wn_wd),
                                               .ra(wn_ra), .rd(wn_q));

    // band counters (CTRL[0] clears them too)
    always @(posedge clk) begin
        if (rst | perf_clr) begin
            perf_p1 <= 32'd0; perf_flush <= 32'd0; perf_kbeat <= 32'd0;
            perf_recs <= 32'd0; perf_band <= 32'd0; perf_kclk <= 32'd0;
        end else begin
            if (bc_p1)                             perf_p1    <= perf_p1    + 32'd1;
            if (fu_on | (bc_state == BC_CLR))      perf_flush <= perf_flush + 32'd1;
            if (k3_v)                              perf_kbeat <= perf_kbeat + 32'd1;
            if (bn_awi)                            perf_recs  <= perf_recs  + 32'd1;
            if (bc_act & ~bc_p1)                   perf_band  <= perf_band  + 32'd1;
            if (~k_idle)                           perf_kclk  <= perf_kclk  + 32'd1;
        end
    end

    // =========================================================================
    // Status, ordering, performance counters
    // =========================================================================
    assign n_idle  = (h1_count == 6'd0) & (g_state == G_IDLE) & ~s_v &
                     (c_state == C_IDLE) & ~pipe_busy &
                     (h2_count == 6'd0) & (w_state == W_IDLE) &
                     (wf_count == {(WF_AW+2){1'b0}}) & wl_empty;
    assign drained = n_idle & (bcount == 8'd0) & ~m_awvalid;

    // registered start conditions (see f_can_start)
    always @(posedge clk) begin
        if (rst) begin
            n_idle_q <= 1'b0; k_idle_q <= 1'b0; drained_q <= 1'b0; flz_q <= 1'b0;
            cl_k <= 1'b0; cl_wide <= 1'b0; cl_nop <= 1'b0; cl_sb <= 1'b0;
            cl_ok <= 1'b0; cl_bc <= 3'd0;
        end else begin
            n_idle_q  <= n_idle  & ~h1_push;
            k_idle_q  <= k_idle  & ~k_start;
            drained_q <= drained & ~h1_push & ~bn_awi & ~fu_awi;
            flz_q     <= (fl_cnt == 7'd0) & ~c_we;
            cl_k      <= q_k;
            cl_wide   <= q_wide;
            cl_nop    <= q_nop;
            cl_sb     <= q_sync | q_blend;
            cl_bc     <= bc_state;
            // same head next clock (no pop) and no WIDE / BAND_GEO write now
            cl_ok     <= cq_valid & ~cq_pop &
                         ~(apb_wr & ((apb_reg == 6'h21) | (apb_reg == 6'h25)));
        end
    end

    assign busy = (cq_count != {(CQ_AW+2){1'b0}}) | (f_state != F_IDLE) |
                  m_arvalid | (bd_count != 8'd0) | r_v | ~drained | list_active;

    // stall classes (CAPS[29]); the PIXEL ones are exclusive per clock
    wire st_wstall  = m_wvalid & ~m_wready;
    wire st_awwait  = wf_valid & wl_empty;
    wire st_awstall = m_awvalid & ~m_awready;
    wire st_arstall = m_arvalid & ~m_arready;
    // (band frames: PERF_P1 / PERF_BAND instead, the PIXEL classes stop)
    wire px_none    = (c_state == C_IDLE) & ~s_v & (g_state == G_IDLE);  // nothing to issue
    wire st_pxwait  = ~bc_act & (c_state == C_IDLE) & s_v & ~s_ok;
    wire st_pxfull  = ~bc_act & (((c_state == C_RUN) & ~wf_credit) | (px_none & h1_valid & h2_full));
    wire st_pxbub   = ~bc_act & (((c_state == C_IDLE) & s_ok) |
                                 ((c_state == C_IDLE) & ~s_v & (g_state == G_ROW)) |
                                 (px_none & h1_valid & ~h2_full));
    wire st_pxidle  = ~bc_act & px_none & ~h1_valid & busy & ~x_mode;   // wide mode: PERF_WCLK

    always @(posedge clk) begin
        if (rst) begin
            err        <= 1'b0;
            lerr       <= 1'b0;
            perf_busy  <= 32'd0;
            perf_cmds  <= 32'd0;
            perf_rbeat <= 32'd0;
            perf_wbeat <= 32'd0;
            perf_chit  <= 32'd0;
            perf_cmiss <= 32'd0;
            perf_wstall <= 32'd0; perf_awwait <= 32'd0; perf_awstall <= 32'd0; perf_arstall <= 32'd0;
            perf_pxwait <= 32'd0; perf_pxfull <= 32'd0; perf_pxbub   <= 32'd0; perf_pxidle  <= 32'd0;
            perf_wclk  <= 32'd0; perf_xwbeat <= 32'd0; perf_xrbeat <= 32'd0;
            cmd_pushed <= 32'd0;
        end else begin
            if ((m_rvalid & (m_rresp != 2'b00)) | (b_hs & (m_bresp != 2'b00)) |
                (x_rvalid & (x_rresp != 2'b00)))
                err <= 1'b1;
            else if (err_clr)
                err <= 1'b0;

            if (lerr_set)
                lerr <= 1'b1;
            else if (err_clr)
                lerr <= 1'b0;

            if (cq_push)
                cmd_pushed <= cmd_pushed + 32'd1;

            if (perf_clr) begin
                perf_busy  <= 32'd0;
                perf_cmds  <= 32'd0;
                perf_rbeat <= 32'd0;
                perf_wbeat <= 32'd0;
                perf_chit  <= 32'd0;
                perf_cmiss <= 32'd0;
                perf_wstall <= 32'd0; perf_awwait <= 32'd0; perf_awstall <= 32'd0; perf_arstall <= 32'd0;
                perf_pxwait <= 32'd0; perf_pxfull <= 32'd0; perf_pxbub   <= 32'd0; perf_pxidle  <= 32'd0;
                perf_wclk  <= 32'd0; perf_xwbeat <= 32'd0; perf_xrbeat <= 32'd0;
            end else begin
                if (busy)       perf_busy  <= perf_busy  + 32'd1;
                if (w_cmd_done | x_done) perf_cmds <= perf_cmds + 32'd1;
                if (x_mode & ~bc_act) perf_wclk <= perf_wclk + 32'd1;
                if (x_wvalid & x_wready) perf_xwbeat <= perf_xwbeat + 32'd1;   // incl. flush / bins
                if (x_rvalid)   perf_xrbeat <= perf_xrbeat + 32'd1;
                if (m_rvalid)   perf_rbeat <= perf_rbeat + 32'd1;   // AXI beats only
                if (w_hs)       perf_wbeat <= perf_wbeat + 32'd1;
                if (f_chit | k_hitev) perf_chit <= perf_chit + 32'd1;
                if (c_we)       perf_cmiss <= perf_cmiss + 32'd1;
                if (st_wstall)  perf_wstall  <= perf_wstall  + 32'd1;
                if (st_awwait)  perf_awwait  <= perf_awwait  + 32'd1;
                if (st_awstall) perf_awstall <= perf_awstall + 32'd1;
                if (st_arstall) perf_arstall <= perf_arstall + 32'd1;
                if (st_pxwait)  perf_pxwait  <= perf_pxwait  + 32'd1;
                if (st_pxfull)  perf_pxfull  <= perf_pxfull  + 32'd1;
                if (st_pxbub)   perf_pxbub   <= perf_pxbub   + 32'd1;
                if (st_pxidle)  perf_pxidle  <= perf_pxidle  + 32'd1;
            end
        end
    end

    // debug readback (2026-10-20, board-only K path hang): plain register / FIFO
    // state, no logic of its own; tools/board_diag.py decodes it.
    // DBG0 [31:21] rd_words [20:14] rd_pend [13:4] rf_res [3] m_arvalid [2] rd_room [1] rf_ok [0] bd_full
    assign dbg0 = {rd_words, rd_pend, rf_eff, m_arvalid, rd_room, rf_ok, bd_full};
    // DBG1 [31:27] ax_out [26:24] 0 [23:16] bd_count [15:8] lf_inflight [7:1] t_cnt [0] bd_v
    assign dbg1 = {ax_out, 3'd0, bd_count, lf_inflight, t_cnt, bd_v};
    // DBG2 [31:24] kf_iss [23:16] kf_done [15:8] fills the queue head waits for
    //      [7:4] kq_count [3] kq_valid [2] kc_v [1] kc_fok [0] kc_hz
    assign dbg2 = {kf_iss, kf_done, kq_dout[82:75], kq_count, kq_valid, kc_v, kc_fok, kc_hz};
    // DBG3 [31] kw_v [30] kb_v [29] kb_res [28] kb_hit [27] kb_ev [26] kb_rl [25] kb_line
    //      [24] kq_room [23] kc_quiet [22] c_qok [21] kb_fill_raw [20] l_len_v [19] l_urgent
    //      [18:17] f_state [16:14] bc_state [13] k1_v [12] k2_v [11] k3_v [10] x_mode
    //      [9] rf_v [8] t_fill [7] t_hit [6] kc_last [5] c_vq [4:0] 0
    assign dbg3 = {kw_v, kb_v, kb_res, kb_hit, kb_ev, kb_rl, kb_line, kq_room, kc_quiet, c_qok,
                   kb_fill_raw, l_len_v, l_urgent, f_state, bc_state, k1_v, k2_v, k3_v, x_mode,
                   rf_v, t_fill, t_hit, kc_last, c_vq, 5'd0};

endmodule


// =============================================================================
// blit_rfifo: same ports / timing as blit_fifo, storage in flip-flops instead
// of block RAM (for wide, shallow queues: one Ti60 RAM block is at most 20 bits
// wide, so a 212-bit queue costs 11 blocks whatever its depth).
// Capacity = 2^AW + 1 (output register).
// =============================================================================
module blit_rfifo #(
    parameter DW = 32,
    parameter AW = 4
)(
    input  wire          clk,
    input  wire          rst,
    input  wire          push,
    input  wire [DW-1:0] din,
    output wire          full,
    output wire [AW:0]   space,
    input  wire          pop,
    output reg  [DW-1:0] dout,
    output reg           valid,
    output wire [AW+1:0] count
);
    (* syn_ramstyle = "registers" *) reg [DW-1:0] mem [0:(1<<AW)-1];
    reg  [AW:0]   wp, rp;
    wire [AW:0]   used      = wp - rp;
    wire          ram_empty = (used == {(AW+1){1'b0}});
    wire          wr        = push & ~full;
    wire          rd        = ~ram_empty & (~valid | pop);

    assign full  = used[AW];
    assign space = {1'b1, {AW{1'b0}}} - used;
    assign count = {1'b0, used} + {{(AW+1){1'b0}}, valid};

    always @(posedge clk) begin
        if (wr) mem[wp[AW-1:0]] <= din;
        if (rd) dout <= mem[rp[AW-1:0]];
    end

    always @(posedge clk) begin
        if (rst) begin
            wp    <= {(AW+1){1'b0}};
            rp    <= {(AW+1){1'b0}};
            valid <= 1'b0;
        end else begin
            if (wr) wp <= wp + {{AW{1'b0}}, 1'b1};
            if (rd) rp <= rp + {{AW{1'b0}}, 1'b1};
            if (rd)       valid <= 1'b1;
            else if (pop) valid <= 1'b0;
        end
    end
endmodule


// =============================================================================
// First-word-fall-through FIFO on block RAM (registered read port).
// Capacity = 2^AW (RAM) + 1 (output register).
// =============================================================================
module blit_fifo #(
    parameter DW = 32,
    parameter AW = 4
)(
    input  wire          clk,
    input  wire          rst,
    input  wire          push,
    input  wire [DW-1:0] din,
    output wire          full,
    output wire [AW:0]   space,       // free RAM entries
    input  wire          pop,
    output reg  [DW-1:0] dout,
    output reg           valid,
    output wire [AW+1:0] count        // RAM entries + output register
);
    reg  [DW-1:0] mem [0:(1<<AW)-1];
    reg  [AW:0]   wp, rp;
    wire [AW:0]   used      = wp - rp;
    wire          ram_empty = (used == {(AW+1){1'b0}});
    wire          wr        = push & ~full;
    wire          rd        = ~ram_empty & (~valid | pop);

    assign full  = used[AW];
    assign space = {1'b1, {AW{1'b0}}} - used;
    assign count = {1'b0, used} + {{(AW+1){1'b0}}, valid};

    always @(posedge clk) begin
        if (wr) mem[wp[AW-1:0]] <= din;
        if (rd) dout <= mem[rp[AW-1:0]];
    end

    always @(posedge clk) begin
        if (rst) begin
            wp    <= {(AW+1){1'b0}};
            rp    <= {(AW+1){1'b0}};
            valid <= 1'b0;
        end else begin
            if (wr) wp <= wp + {{AW{1'b0}}, 1'b1};
            if (rd) rp <= rp + {{AW{1'b0}}, 1'b1};
            if (rd)       valid <= 1'b1;
            else if (pop) valid <= 1'b0;
        end
    end
endmodule


// =============================================================================
// Simple dual-port block RAM, registered read.
// =============================================================================
module blit_ram #(
    parameter DW = 16,
    parameter AW = 10
)(
    input  wire          clk,
    input  wire          we,
    input  wire [AW-1:0] wa,
    input  wire [DW-1:0] wd,
    input  wire [AW-1:0] ra,
    output reg  [DW-1:0] rd
);
    reg [DW-1:0] mem [0:(1<<AW)-1];
    always @(posedge clk) begin
        if (we) mem[wa] <= wd;
        rd <= mem[ra];
    end
endmodule


// =============================================================================
// blit_win: band window of band mode (D). 8 pixel lanes x 16 bit x PARTS*512
// beats: PARTS = 5 -> 2560 beats (16 rows x 2560 bytes = 1280 px rows, 720p,
// 40 blocks), PARTS = 3 -> 1536 (16 rows x 1280 bytes, 24 blocks). Each lane is
// PARTS RAMs of 512 x 16 (one Ti60 block each) with its own write enable, so any
// set of pixels of a beat can be written in one clock. Read: registered (data one
// clock after ra).
// =============================================================================
module blit_win #(
    parameter PARTS = 5             // 3..5
)(
    input  wire         clk,
    input  wire [7:0]   we,         // pixel lanes written
    input  wire [11:0]  wa,
    input  wire [127:0] wd,
    input  wire [11:0]  ra,
    output wire [127:0] rd
);
    reg  [2:0]  rs;                 // part of the read address, one clock late
    always @(posedge clk) rs <= ra[11:9];
    wire [15:0] r0, r1, r2, r3, r4, r5, r6, r7;
    assign rd = {r7, r6, r5, r4, r3, r2, r1, r0};
    blit_win_lane #(.PARTS(PARTS)) u_l0 (.clk(clk), .we(we[0]), .wa(wa), .wd(wd[15:0]),    .ra(ra), .rs(rs), .rd(r0));
    blit_win_lane #(.PARTS(PARTS)) u_l1 (.clk(clk), .we(we[1]), .wa(wa), .wd(wd[31:16]),   .ra(ra), .rs(rs), .rd(r1));
    blit_win_lane #(.PARTS(PARTS)) u_l2 (.clk(clk), .we(we[2]), .wa(wa), .wd(wd[47:32]),   .ra(ra), .rs(rs), .rd(r2));
    blit_win_lane #(.PARTS(PARTS)) u_l3 (.clk(clk), .we(we[3]), .wa(wa), .wd(wd[63:48]),   .ra(ra), .rs(rs), .rd(r3));
    blit_win_lane #(.PARTS(PARTS)) u_l4 (.clk(clk), .we(we[4]), .wa(wa), .wd(wd[79:64]),   .ra(ra), .rs(rs), .rd(r4));
    blit_win_lane #(.PARTS(PARTS)) u_l5 (.clk(clk), .we(we[5]), .wa(wa), .wd(wd[95:80]),   .ra(ra), .rs(rs), .rd(r5));
    blit_win_lane #(.PARTS(PARTS)) u_l6 (.clk(clk), .we(we[6]), .wa(wa), .wd(wd[111:96]),  .ra(ra), .rs(rs), .rd(r6));
    blit_win_lane #(.PARTS(PARTS)) u_l7 (.clk(clk), .we(we[7]), .wa(wa), .wd(wd[127:112]), .ra(ra), .rs(rs), .rd(r7));
endmodule

// one 16-bit pixel lane of the window: PARTS x (512 x 16) block RAMs
module blit_win_lane #(
    parameter PARTS = 5
)(
    input  wire         clk,
    input  wire         we,
    input  wire [11:0]  wa,
    input  wire [15:0]  wd,
    input  wire [11:0]  ra,
    input  wire [2:0]   rs,         // ra[11:9] of the previous clock
    output wire [15:0]  rd
);
    // parts above PARTS are never written and never selected: synthesis drops them
    wire        p0 = (wa[11:9] == 3'd0);
    wire        p1 = (wa[11:9] == 3'd1);
    wire        p2 = (wa[11:9] == 3'd2);
    wire        p3 = (wa[11:9] == 3'd3) & (PARTS > 3);
    wire        p4 = (wa[11:9] == 3'd4) & (PARTS > 4);
    wire [15:0] q0, q1, q2, q3, q4;
    blit_ram #(.DW(16), .AW(9)) u_p0 (.clk(clk), .we(we & p0), .wa(wa[8:0]), .wd(wd), .ra(ra[8:0]), .rd(q0));
    blit_ram #(.DW(16), .AW(9)) u_p1 (.clk(clk), .we(we & p1), .wa(wa[8:0]), .wd(wd), .ra(ra[8:0]), .rd(q1));
    blit_ram #(.DW(16), .AW(9)) u_p2 (.clk(clk), .we(we & p2), .wa(wa[8:0]), .wd(wd), .ra(ra[8:0]), .rd(q2));
    blit_ram #(.DW(16), .AW(9)) u_p3 (.clk(clk), .we(we & p3), .wa(wa[8:0]), .wd(wd), .ra(ra[8:0]), .rd(q3));
    blit_ram #(.DW(16), .AW(9)) u_p4 (.clk(clk), .we(we & p4), .wa(wa[8:0]), .wd(wd), .ra(ra[8:0]), .rd(q4));
    assign rd = (rs == 3'd0) ? q0 : (rs == 3'd1) ? q1 : (rs == 3'd2) ? q2 :
                ((rs == 3'd3) & (PARTS > 3)) ? q3 : (PARTS > 4) ? q4 : q2;
endmodule


// =============================================================================
// blit_add565: RGB565 a + b per channel, saturated (additive blend, K path)
// =============================================================================
module blit_add565 (
    input  wire [15:0] a,
    input  wire [15:0] b,
    output wire [15:0] y
);
    wire [5:0] r = {1'b0, a[15:11]} + {1'b0, b[15:11]};
    wire [6:0] g = {1'b0, a[10:5]}  + {1'b0, b[10:5]};
    wire [5:0] c = {1'b0, a[4:0]}   + {1'b0, b[4:0]};
    assign y = {r[5] ? 5'd31 : r[4:0], g[6] ? 6'd63 : g[5:0], c[5] ? 5'd31 : c[4:0]};
endmodule
