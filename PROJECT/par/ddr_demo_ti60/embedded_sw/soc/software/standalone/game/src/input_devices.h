#ifndef TI60_INPUT_DEVICES_H
#define TI60_INPUT_DEVICES_H
#include <stdint.h>
/* Device-independent actions. The application translates these into ControlEvent.
 * INPUT_HELD is the OR of matrix and USB keyboard held states, using PAD_* bits.
 * Toggle/next actions are resolved against application state, never a shadow copy. */
typedef enum {
    INPUT_HELD, INPUT_SCENE_NEXT, INPUT_START_PAUSE, INPUT_STRESS_MENU,
    INPUT_COUNT_DELTA, INPUT_STEP_NEXT, INPUT_STEP_SET, INPUT_SIZE_NEXT,
    INPUT_BULLET_DELTA, INPUT_SQUARE_DELTA, INPUT_RESOLUTION_NEXT,
    INPUT_MENU, INPUT_PICK, INPUT_RESTART, INPUT_AUTO_TOGGLE,
    INPUT_ALPHA_TOGGLE, INPUT_VOLUME_DELTA, INPUT_STATS_TOGGLE,
    INPUT_BACKEND_TOGGLE, INPUT_VSYNC_TOGGLE, INPUT_BGM_TOGGLE,
    INPUT_TRACK_NEXT, INPUT_STATUS, INPUT_PAUSE, INPUT_TONE_TOGGLE,
    INPUT_CH9350_STATE, INPUT_LEGACY, INPUT_SOURCE_HELD, INPUT_VIDEO_CONFIRM, INPUT_VIDEO_RECOVER
} InputAction;
typedef void (*InputEmit)(void *opaque, InputAction action, int value);
typedef struct {
    InputEmit emit; void *opaque;
    uint16_t matrix_previous, matrix_held, keyboard_held, published_held;
    uint8_t keyboard_previous[8],source;
    uint16_t published_matrix,published_keyboard;
    uint8_t rx_state, rx_tag, rx_length, rx_used, rx_data[8], ghost_latched;
    uint32_t rx_last_ms, keyboard_reports, mouse_reports, malformed_frames,
             rollover_reports, ghost_reports, status_reports;
    uint16_t matrix_raw, matrix_last, matrix_seen, cal_seen;
    uint32_t matrix_events, keyboard_bytes, keyboard_headers;
    uint8_t matrix_map[16], cal_map[16], diagnostic, calibrating;
    uint8_t cal_step, cal_wait, cal_error, map_applied;
    uint8_t rx_history[16], rx_history_used, rx_history_head, keyboard_down;
} InputDevices;
void input_devices_init(InputDevices *d, InputEmit emit, void *opaque);
/* Row-major: bit 0=S1/R0C0, bit 15=S16/R3C3. Input is already debounced.
 * A detected 2x2 rectangle releases the matrix and suppresses it until all keys
 * are released. This is conservative for a diode-less keypad. */
void input_matrix_snapshot(InputDevices *d, uint16_t keys);
void input_ch9350_feed(InputDevices *d, uint8_t byte, uint32_t now_ms);
/* Call only after draining every currently available RX FIFO byte. The time is
 * software dequeue time, not a hardware arrival timestamp: queued continuation
 * bytes must be processed before deciding an incomplete frame has expired. */
void input_devices_poll(InputDevices *d, uint32_t now_ms);
/* Call on a confirmed keyboard disconnect or UART overrun/framing error.
 * State-2 reports arrive on change, so idle time alone is NOT a disconnect. */
void input_keyboard_release(InputDevices *d);
int input_matrix_has_ghost(uint16_t keys);
/* Diagnostics suppress gameplay but continue HID parsing and state replies.
 * Calibration records physical S1..S16 -> raw bit; ghost checks stay in raw
 * electrical coordinates. Changes are staged and applied atomically. */
int input_set_diagnostic(InputDevices *d,int enabled);
int input_matrix_calibration(InputDevices *d,int operation); /* 0 cancel,1 begin,2 apply,3 undo */
int input_matrix_bind(InputDevices *d,unsigned packed); /* (physical index<<4)|raw bit */
#endif
