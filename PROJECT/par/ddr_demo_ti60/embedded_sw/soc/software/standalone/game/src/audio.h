#ifndef AUDIO_H
#define AUDIO_H
/*
 * HDMI 音频驱动（2026-10-19）。
 *
 * 硬件（rtl/hdmi/hdmi_island.v + rtl/audio/audio_mixer.v）：
 *   hdmi_fb（0xF810_0000）新增 0x20.. 寄存器：HDMI 模式 / 音频包开关 / 1 kHz 测试音 / 采样 FIFO；
 *   混音器（0xF810_2000）：16 个声部，每个声部从 DDR 读 PCM（s8 / s16 / s16 立体声），
 *   音高（任意采样率，线性插值）、左右音量、循环；输出 48 kHz 立体声进 HDMI。
 *
 * 声音库（tools/wav2bank.py 由 WAV 生成）放在 DDR 0x0200_0000：
 *   头 16 B：'SBK1' / 条目数 / 总字节 / 0，然后每条 32 B：
 *   { 数据偏移, 帧数, 循环起点（0xFFFFFFFF = 不循环）, [16:0] 采样率 [25:24] 格式, 名字[16] }
 *   数据按 32 B 对齐（混音器的缓存行）。
 *   makefile SOUND=1（默认）把 sound/sound_bank.bin 链接进 elf（JTAG 下载时一起写进 DDR）；
 *   SOUND=0 时可以用 OpenOCD 单独 load_image 一次（不断电就一直在），程序只认 0x0200_0000 处的头。
 *
 * 声部分配：0、1 = BGM（切歌时交替，旧的淡出），2..15 = 音效：每个音效固定占一个声部
 * （同一个音效再响就从头重放，像东方原作那样），音效种类多于 14 个时挤掉最早响的那个。
 *
 * 旧位流（没有音频硬件）上 aud_init() 返回 -1，其余函数都是空操作。
 */
#include <stdint.h>

#ifndef AUD_HDMI_BASE
#define AUD_HDMI_BASE  0xF8100000u
#endif
#ifndef AUD_MIX_BASE
#define AUD_MIX_BASE   0xF8102000u
#endif
#ifndef AUD_BANK_ADDR
#define AUD_BANK_ADDR  0x02000000u
#endif

#define AUD_VOICES     16
#define AUD_BGM_VOICES 2
#define AUD_RATE       48000

/* hdmi_fb audio registers */
#define AUD_R_CTRL     0x20         /* [0] HDMI [1] audio [2] tone [5:4] tone L/R */
#define AUD_R_LEVEL    0x24
#define AUD_R_SENT     0x28
#define AUD_R_UF       0x2C
#define AUD_R_DATA     0x30
#define AUD_R_CAPS     0x34
/* mixer registers */
#define MIX_R_ID       0x000
#define MIX_R_CTRL     0x004
#define MIX_R_MVOL     0x008
#define MIX_R_TARGET   0x00C
#define MIX_R_KEYON    0x010
#define MIX_R_KEYOFF   0x014
#define MIX_R_ACTIVE   0x018
#define MIX_R_SAMPLES  0x01C
#define MIX_R_MISSES   0x020
#define MIX_R_FETCHES  0x024
#define MIX_R_CLIPS    0x028
#define MIX_R_CAPS     0x02C
#define MIX_V(v, w)    (0x200u + (uint32_t)(v) * 0x20u + (w))
#define MIX_V_ADDR     0x00
#define MIX_V_LEN      0x04
#define MIX_V_LOOP     0x08
#define MIX_V_PITCH    0x0C
#define MIX_V_VOL      0x10
#define MIX_V_START    0x14
#define MIX_ID_VALUE   0x4D495831u  /* "MIX1" */
#define SBK_MAGIC      0x314B4253u  /* "SBK1" */

enum { AUD_FMT_S8, AUD_FMT_S16, AUD_FMT_S16ST };

typedef struct {
    uint32_t off, frames, loop, rate_fmt;
    char name[16];
} AudEntry;

typedef struct {
    int hdmi, mixer;                /* hardware present */
    int nsnd;                       /* sounds in the bank (0 = no bank) */
    uint32_t bank_bytes;
    int master;                     /* 0..256 */
    int tone;
    int bank_new;                   /* aud_check_bank() just found a bank (main.c prints it, clears) */
} AudState;
extern AudState aud;

int  aud_init(void);                /* 0 = mixer + HDMI audio present */
/* no bank yet: look at 0x02000000 again (tools/load_sound.py may have written it while the
 * program runs); 1 = found now. Cheap (D$ invalidate + 3 reads), th_sound_tick calls it once a second. */
int  aud_check_bank(void);
void aud_tone(int on);              /* HW 1 kHz test tone (left + right); needs only the HDMI part */
void aud_master(int vol);           /* 0..256 */
int  aud_find(const char *name);    /* bank index or -1 */
const char *aud_name(int snd);
uint32_t aud_frames(int snd);
uint32_t aud_rate(int snd);
/* sound effect: own voice per sound (retriggered), vol 0..256, pan -128 (left) .. 127 */
int  aud_play(int snd, int vol, int pan);
/* raw: voice 0..15, pitch_q12 4096 = original speed, loop: 1 = use the bank's loop */
void aud_voice(int voice, int snd, int vol, int pan, int pitch_q12, int loop);
void aud_voice_vol(int voice, int vol, int pan);
void aud_stop(int voice);
/* background music: snd -1 = stop. A different song fades the old one out over fade_ticks. */
void aud_bgm(int snd, int vol, int fade_ticks);
void aud_bgm_volume(int vol);       /* e.g. lower while paused */
int  aud_bgm_current(void);
void aud_tick(void);                /* once per 1/60 s game tick: fades */
uint32_t aud_active(void);
void aud_status_line(char *buf, int n);   /* one-line status for the UART */

#endif
