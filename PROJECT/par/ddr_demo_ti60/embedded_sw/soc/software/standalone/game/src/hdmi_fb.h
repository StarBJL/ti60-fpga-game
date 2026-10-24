#ifndef HDMI_FB_H
#define HDMI_FB_H
/*
 * Driver for rtl/hdmi/hdmi_fb.v (DDR framebuffer -> HDMI 640x480@60, RGB565).
 * The block sits on SoC io_apbSlave_0 at 0xF810_0000 and reads the framebuffer
 * from DDR through io_ddrMasters_0.
 *
 *   0x00 CTRL      [0] enable scan-out  [1] force colour-bar  [2] vsync IRQ enable
 *   0x04 FB_ADDR   write = request flip to this base at the next vsync
 *   0x08 FB_CUR    base currently being scanned out
 *   0x0C STATUS    [0] flip_pending [1] vblank [2] underflow(W1C) [3] irq(W1C)
 *   0x10 FRAME_CNT frames since reset
 *   0x14 RES       {V_ACT, H_ACT}
 *   0x18 UF_CNT    lines with FIFO underflow
 *   0x1C ID        0x48444D49 ("HDMI")
 *
 * The flip is latched at the end of vsync. From then on the previously shown
 * buffer is no longer read by the DMA, so the CPU may draw into it.
 */
#include <stdint.h>

#ifndef HDMI_FB_BASE
#define HDMI_FB_BASE 0xF8100000u
#endif

#define HDMI_REG(off)      (*(volatile uint32_t *)(HDMI_FB_BASE + (off)))
#define HDMI_CTRL          HDMI_REG(0x00)
#define HDMI_FB_ADDR       HDMI_REG(0x04)
#define HDMI_FB_CUR        HDMI_REG(0x08)
#define HDMI_STATUS        HDMI_REG(0x0C)
#define HDMI_FRAME_CNT     HDMI_REG(0x10)
#define HDMI_RES           HDMI_REG(0x14)
#define HDMI_UF_CNT        HDMI_REG(0x18)
#define HDMI_ID            HDMI_REG(0x1C)

#define HDMI_MODE          HDMI_REG(0x50)
#define HDMI_VIDEO_ID      HDMI_REG(0x54)
#define HDMI_CANVAS        HDMI_REG(0x58)
#define HDMI_VIDEO_REV     HDMI_REG(0x5c) /* R5.2.1 = 0x00050201; older bit = 0 */
#define HDMI_VIDEO_ID_VALUE 0x56494435u
static inline int hdmi_dual_supported(void){return HDMI_VIDEO_ID==HDMI_VIDEO_ID_VALUE;}
static inline int hdmi_mode_1080(void){return hdmi_dual_supported() && (HDMI_MODE&2u)!=0;}
#define HDMI_ID_VALUE      0x48444D49u

#define HDMI_CTRL_EN       (1u << 0)
#define HDMI_CTRL_TEST     (1u << 1)
#define HDMI_CTRL_IRQ_EN   (1u << 2)

#define HDMI_ST_FLIP_PEND  (1u << 0)
#define HDMI_ST_VBLANK     (1u << 1)
#define HDMI_ST_UNDERFLOW  (1u << 2)
#define HDMI_ST_IRQ        (1u << 3)

static inline uint32_t hdmi_id(void)            { return HDMI_ID; }
static inline uint32_t hdmi_frame_cnt(void)     { return HDMI_FRAME_CNT; }
static inline uint32_t hdmi_uf_cnt(void)        { return HDMI_UF_CNT; }
static inline uint32_t hdmi_cur(void)           { return HDMI_FB_CUR; }
static inline int      hdmi_flip_pending(void)  { return (HDMI_STATUS & HDMI_ST_FLIP_PEND) != 0; }
static inline int      hdmi_in_vblank(void)     { return (HDMI_STATUS & HDMI_ST_VBLANK) != 0; }

static inline void hdmi_enable(uint32_t first_fb)
{
    HDMI_FB_ADDR = first_fb;
    HDMI_CTRL    = HDMI_CTRL_EN;
}

static inline void hdmi_test_pattern(int on)
{
    HDMI_CTRL = on ? (HDMI_CTRL_EN | HDMI_CTRL_TEST) : HDMI_CTRL_EN;
}

/* Make all CPU stores to the framebuffer visible (D$ is write-through, so a
 * fence that drains the store buffer is enough). */
static inline void hdmi_fence(void)
{
#ifdef __riscv
    __asm__ volatile ("fence" ::: "memory");
#else
    __sync_synchronize();                    /* host test build */
#endif
}

/* Publish the finished frame and request the flip at the next vsync. */
static inline void hdmi_flip(uint32_t fb)
{
    hdmi_fence();
    HDMI_FB_ADDR = fb;
}

static inline void hdmi_wait_flip(void)
{
    while (hdmi_flip_pending()) { }
}

static inline void hdmi_wait_vsync(void)
{
    uint32_t f = hdmi_frame_cnt();
    while (hdmi_frame_cnt() == f) { }
}

#endif
