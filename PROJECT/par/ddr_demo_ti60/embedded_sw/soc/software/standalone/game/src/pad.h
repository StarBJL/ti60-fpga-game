#ifndef PAD_H
#define PAD_H
/*
 * 键盘"按住"状态，经串口从 PC 转发过来（2026-10-11）。
 *
 * 串口终端只能收到按下的字符，判断不了"按住"和"同时按"（东方式弹幕要：方向 + Shift 低速 +
 * Z 射击同时按住）。所以 PC 上跑 tools/pc_pad.py：它占用串口，每帧把按键状态打成 4 个字节发来，
 * 同时把板子的串口输出显示出来（代替串口终端）；其余字母 / 数字键照旧当字符发（调试键）。
 *
 * 包格式（4 字节，60 次 / 秒；终端字符都 < 0x80，所以不会混淆）：
 *     0xA5, b0, b1, chk      b0 = 状态 [6:0]，b1 = 状态 [13:7]，chk = (b0 ^ b1 ^ 0x5A) & 0x7F
 * 超过 PAD_TIMEOUT_MS 没收到包（PC 程序关了 / 换回普通终端）就当全部松开。
 */
#include <stdint.h>

#define PAD_UP     (1u << 0)
#define PAD_DOWN   (1u << 1)
#define PAD_LEFT   (1u << 2)
#define PAD_RIGHT  (1u << 3)
#define PAD_SHOT   (1u << 4)    /* Z / 空格：射击（菜单里 = 确定） */
#define PAD_BOMB   (1u << 5)    /* X：符卡（菜单里 = 取消） */
#define PAD_SLOW   (1u << 6)    /* Shift：低速（显示判定点） */
#define PAD_PAUSE  (1u << 7)    /* Esc：暂停 */
#define PAD_START  (1u << 8)    /* Enter：开始 */
#define PAD_SKIP   (1u << 9)    /* C / Ctrl：跳过对话、备用 */

#define PAD_SYNC        0xA5u
#define PAD_TIMEOUT_MS  300u

/* 每个串口收到的字节先交给它：是状态包的一部分就吃掉返回 1，否则返回 0（照普通字符处理）。
 * now = CLINT 时间（ticks），ticks_per_ms = 每毫秒的 tick 数。 */
int      pad_feed(uint8_t c, uint32_t now);
/* Common held-state event for framed hosts and future GPIO/USB decoders. */
void pad_set_physical(uint32_t mask);
void pad_set_state(uint32_t mask,uint32_t now);
void pad_set_device(unsigned source,uint32_t mask);
uint32_t pad_source_held(unsigned source,uint32_t now,uint32_t ticks_per_ms);
uint32_t pad_source_pressed(unsigned source,uint32_t now,uint32_t ticks_per_ms);
int pad_has_state(void);
void pad_use_terminal(void);
/* 当前按住的键（PAD_*）；超时（PC 程序没在发）返回 0。 */
uint32_t pad_held(uint32_t now, uint32_t ticks_per_ms);
/* PC 程序是否在发包（最近 PAD_TIMEOUT_MS 内收到过）。 */
int      pad_active(uint32_t now, uint32_t ticks_per_ms);
/* 自上次调用以来新按下的键（上升沿，每个 tick 取一次）。 */
uint32_t pad_pressed(uint32_t now, uint32_t ticks_per_ms);
/* 统计：收到的包数 / 校验错的包数。 */
extern uint32_t pad_packets, pad_errors;

#endif
