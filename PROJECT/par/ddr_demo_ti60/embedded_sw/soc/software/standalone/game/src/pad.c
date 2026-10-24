#include "pad.h"

/* 见 pad.h：0xA5, b0, b1, chk */
uint32_t pad_packets, pad_errors;
static uint32_t pad_state, pad_last, pad_prev,pad_pending;
static uint8_t  pad_buf[3];
static int      pad_n = -1;             /* -1 = 不在包里，0..2 = 已收到的字节数 */
static int      pad_seen, physical_seen;
static uint32_t physical_state, physical_pending;

int pad_has_state(void){return pad_seen || physical_seen;}
void pad_use_terminal(void){pad_state=pad_prev=pad_pending=0;pad_seen=0;physical_seen=(physical_state!=0);}
void pad_set_physical(uint32_t mask){
    physical_pending|=mask&~physical_state;physical_state=mask;physical_seen=1;
}
void pad_set_state(uint32_t mask,uint32_t now){
    pad_pending|=mask&~pad_state;pad_state=mask;pad_last=now;pad_seen=1;
}

int pad_feed(uint8_t c, uint32_t now)
{
    if (c == PAD_SYNC) { pad_n = 0; return 1; }
    if (pad_n < 0) return 0;
    if (c & 0x80u) { pad_n = -1; ++pad_errors; return 1; }   /* 包里不该有高位字节 */
    pad_buf[pad_n++] = c;
    if (pad_n < 3) return 1;
    pad_n = -1;
    if (((pad_buf[0] ^ pad_buf[1] ^ 0x5Au) & 0x7Fu) != pad_buf[2]) { ++pad_errors; return 1; }
    pad_set_state((uint32_t)pad_buf[0] | ((uint32_t)pad_buf[1] << 7),now);
    ++pad_packets;
    return 1;
}

int pad_active(uint32_t now, uint32_t ticks_per_ms)
{
    return pad_seen && now - pad_last < PAD_TIMEOUT_MS * ticks_per_ms;
}

uint32_t pad_held(uint32_t now, uint32_t ticks_per_ms)
{
    return (pad_active(now, ticks_per_ms) ? pad_state : 0u) | physical_state;
}

uint32_t pad_pressed(uint32_t now, uint32_t ticks_per_ms)
{
    uint32_t h = pad_held(now, ticks_per_ms), e = (h & ~pad_prev) | (pad_active(now,ticks_per_ms)?pad_pending:0) | physical_pending;
    pad_pending=0;physical_pending=0;pad_prev = h;
    return e;
}
