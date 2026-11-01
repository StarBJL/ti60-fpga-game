#include "pad.h"

/* 见 pad.h：0xA5, b0, b1, chk */
uint32_t pad_packets, pad_errors;
static uint32_t pad_state, pad_last, pad_prev, pad_pending;
static uint8_t  pad_buf[3];
static int      pad_n = -1;             /* -1 = 不在包里，0..2 = 已收到的字节数 */
static int      pad_seen;
/* 物理设备（2026-10-26 多人，input_devices.c 发来）：1 = CH9350 USB 键盘，2 = 矩阵键盘。
 * 每个设备两套"新按下"：一套给合起来的 pad_pressed（菜单 / 单人），一套给 pad_source_pressed（多人各自的输入） */
static uint32_t dev_state[2], dev_pending_all[2], dev_pending_src[2], src_prev[3], serial_pending_src;

int pad_feed(uint8_t c, uint32_t now)
{
    if (c == PAD_SYNC) { pad_n = 0; return 1; }
    if (pad_n < 0) return 0;
    if (c & 0x80u) { pad_n = -1; ++pad_errors; return 1; }   /* 包里不该有高位字节 */
    pad_buf[pad_n++] = c;
    if (pad_n < 3) return 1;
    pad_n = -1;
    if (((pad_buf[0] ^ pad_buf[1] ^ 0x5Au) & 0x7Fu) != pad_buf[2]) { ++pad_errors; return 1; }
    pad_set_state((uint32_t)pad_buf[0] | ((uint32_t)pad_buf[1] << 7), now);
    ++pad_packets;
    return 1;
}

/* 控制面板 ":id move 掩码"（2026-10-26）：同一套按住状态；两个 tick 之间按下又松开的键也记一次按下 */
void pad_set_state(uint32_t mask, uint32_t now)
{
    pad_pending |= mask & ~pad_state;
    serial_pending_src |= mask & ~pad_state;
    pad_state = mask;
    pad_last = now;
    pad_seen = 1;
}

int pad_active(uint32_t now, uint32_t ticks_per_ms)
{
    return pad_seen && now - pad_last < PAD_TIMEOUT_MS * ticks_per_ms;
}

uint32_t pad_held(uint32_t now, uint32_t ticks_per_ms)
{
    return (pad_active(now, ticks_per_ms) ? pad_state : 0u) | dev_state[0] | dev_state[1];
}

uint32_t pad_pressed(uint32_t now, uint32_t ticks_per_ms)
{
    uint32_t h = pad_held(now, ticks_per_ms), e = (h & ~pad_prev) | (pad_active(now, ticks_per_ms) ? pad_pending : 0u) |
                 dev_pending_all[0] | dev_pending_all[1];
    pad_pending = 0; dev_pending_all[0] = dev_pending_all[1] = 0;
    pad_prev = h;
    return e;
}

int pad_devices_held(void) { return (dev_state[0] | dev_state[1]) != 0; }

void pad_set_device(unsigned source, uint32_t mask)
{
    unsigned i;
    if (source < 1 || source > 2) return;
    i = source - 1;
    dev_pending_all[i] |= mask & ~dev_state[i];
    dev_pending_src[i] |= mask & ~dev_state[i];
    dev_state[i] = mask;
}

uint32_t pad_source_held(unsigned source, uint32_t now, uint32_t ticks_per_ms)
{
    if (source == 0) return pad_active(now, ticks_per_ms) ? pad_state : 0u;
    return source <= 2 ? dev_state[source - 1] : 0u;
}

uint32_t pad_source_pressed(unsigned source, uint32_t now, uint32_t ticks_per_ms)
{
    uint32_t h, pending, e;
    if (source > 2) return 0;
    h = pad_source_held(source, now, ticks_per_ms);
    pending = source ? dev_pending_src[source - 1] : (pad_active(now, ticks_per_ms) ? serial_pending_src : 0u);
    e = (h & ~src_prev[source]) | pending;
    src_prev[source] = h;
    if (source) dev_pending_src[source - 1] = 0; else serial_pending_src = 0;
    return e;
}
