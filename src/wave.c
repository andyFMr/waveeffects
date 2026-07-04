#include "wave.h"

u8 wave_hdma_table[WAVE_HDMA_BUF_SIZE];
WaveParams wave;

void waveInit(u8 bg_h, u8 bg_v) {
    wave.amp = 12;
    wave.freq = 0x0300;
    wave.phase = 0;
    wave.dir = WAVE_DIR_HORIZONTAL;
    wave.bg_h = bg_h;
    wave.bg_v = bg_v;
    
    waveRebuildHdmaTable(wave_hdma_table, &wave);
    waveSetupHdma(wave_hdma_table, wave.dir, wave.bg_h);
}

void waveUpdate(void) {
    wave.phase += wave.freq;
    waveRebuildHdmaTable(wave_hdma_table, &wave);
}

void waveChangeDir(void) {
    if (wave.dir == WAVE_DIR_HORIZONTAL) {
        wave.dir = WAVE_DIR_VERTICAL;
        waveSetupHdma(wave_hdma_table, wave.dir, wave.bg_v);
    } else {
        wave.dir = WAVE_DIR_HORIZONTAL;
        waveSetupHdma(wave_hdma_table, wave.dir, wave.bg_h);
    }
}

void waveSetupHdma(u8 *hdma_buf, u8 dir, u8 bg_id) {
    REG_HDMAEN &= ~HDMA_CHANNEL1; /* Disable before configuring */
    REG_DMAP1 = 0x02;

    /* 
     * Registers for BG offsets:
     * BG1HOFS = 0x0D, BG1VOFS = 0x0E
     * BG2HOFS = 0x0F, BG2VOFS = 0x10
     * BG3HOFS = 0x11, BG3VOFS = 0x12
     * BG4HOFS = 0x13, BG4VOFS = 0x14
     * (bg_id is 0-3 for BG1-BG4 respectively)
     */
    REG_BBAD1 = 0x0D + (bg_id * 2) + dir;

    REG_A1T1LH = (u16)hdma_buf;
    REG_A1B1 = 0x7E;

    REG_HDMAEN |= HDMA_CHANNEL1;
}
