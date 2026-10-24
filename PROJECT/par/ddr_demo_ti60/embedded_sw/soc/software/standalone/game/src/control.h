#ifndef TI60_CONTROL_H
#define TI60_CONTROL_H
#include <stdint.h>
typedef enum {
 CT_NONE,CT_STATUS,CT_SCENE,CT_START,CT_PAUSE,CT_RESTART,CT_MENU,CT_STRESS,
 CT_SKILL,CT_PICK,CT_MOVE,CT_STOP,CT_STEP,CT_DELTA,CT_COUNT,CT_BULLET_SIZE,
 CT_SQUARE_SIZE,CT_PERIOD,CT_DENSITY,CT_AUTO,CT_VSYNC,CT_ALPHA,CT_BACKEND,CT_MOTION,
 CT_TABLE,CT_BGM,CT_NEXT_TRACK,CT_VOLUME,CT_TONE,CT_STATS,CT_VERBOSE,CT_BENCH,
 CT_RESOLUTION,CT_FAST,CT_BARS,CT_VOLUME_DELTA,CT_PREVIOUS_TRACK,CT_MUSIC_SELECT,CT_LEGACY,CT_VIDEO_CONFIRM,CT_INVALID
} ControlType;
typedef struct {ControlType type;int value;unsigned id;int has_value;} ControlEvent;
typedef struct {char data[64];unsigned used;int active,overflow;} ControlParser;
/* ':' begins an atomic command, then: request-id command [integer] LF.
 * 0=legacy byte, 1=consumed, 2=complete event. Malformed frames never leak keys. */
int control_feed(ControlParser *p,unsigned char byte,ControlEvent *out);
int control_parse(const char *text,ControlEvent *out);
int control_is_step(int value);
#endif
