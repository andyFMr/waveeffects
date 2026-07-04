/*---------------------------------------------------------------------------------

    Wave Effect Module — Header
    HDMA-based per-scanline wave distortion for SNES backgrounds.

---------------------------------------------------------------------------------*/
#ifndef WAVE_H_
#define WAVE_H_

#include <snes.h>

#define WAVE_MAX_AMP 31
#define WAVE_FREQ_STEP 8
#define WAVE_SCREEN_H 224
#define WAVE_HDMA_ENTRY_SIZE 3
#define WAVE_HDMA_BUF_SIZE (WAVE_SCREEN_H * 2)

#define WAVE_DIR_HORIZONTAL 0
#define WAVE_DIR_VERTICAL   1

typedef struct {
    u16 amp;
    u16 freq;
    u16 phase;
    u8  dir;
    u8  bg_h; /* Background ID for Horizontal wave (0-3) */
    u8  bg_v; /* Background ID for Vertical wave (0-3) */
} WaveParams;

extern const s16 sine_lut_2d[32][256];
extern WaveParams wave;

void waveInit(u8 bg_h, u8 bg_v);
void waveUpdate(void);
void waveChangeDir(void);

void waveRebuildHdmaTable(u8 *hdma_buf, WaveParams *wp);
void waveSetupHdma(u8 *hdma_buf, u8 dir, u8 bg_id);

#endif
