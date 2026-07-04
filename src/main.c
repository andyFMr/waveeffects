/*---------------------------------------------------------------------------------

    Wave Effects Demo — PVSnesLib
    HDMA-based per-scanline wave distortion on BG0 (Mode 1)
    with text overlay on BG2 for parameter display.

    Controls:
      Up/Down    — Wave amplitude (+/-)
      Left/Right — Wave frequency (+/-)
      L/R        — Toggle H/V direction
      Start      — Reset to defaults

---------------------------------------------------------------------------------*/

#include "../res/gfx/bgs/pvsneslib.inc"
#include "../res/gfx/bgs/pvsneslibfont.inc"
#include "gui.h"
#include "wave.h"
#include <snes.h>

/*---------------------------------------------------------------------------------
    VBlank Handler — DMAs + HDMA
---------------------------------------------------------------------------------*/
void myVblank(void) {
  guiVblank();

  REG_HDMAEN = HDMA_CHANNEL1;
}

/*---------------------------------------------------------------------------------
    Main
---------------------------------------------------------------------------------*/
int main(void) {
  u16 pad;
  u16 pad_down;

  guiInit();

  /* Init BG0: background tiles and map */
  bgInitTileSet(0, &pvsneslib_til, &pvsneslib_pal, 2,
                (&pvsneslib_tilend - &pvsneslib_til),
                (&pvsneslib_palend - &pvsneslib_pal), BG_16COLORS, 0x4000);
  bgInitMapSet(0, &pvsneslib_map, 2048, SC_32x32, 0x0000);

  /* Init BG2 (Mode 1 BG3): font tiles for text overlay */
  bgInitTileSet(2, &pvsneslibfont_til, &pvsneslibfont_pal, 0,
                (&pvsneslibfont_tilend - &pvsneslibfont_til),
                (&pvsneslibfont_palend - &pvsneslibfont_pal), BG_4COLORS,
                0x6000);

  /* Custom VBlank handler */
  nmiSet(myVblank);

  setPaletteColor(0, 0);

  /* Mode 1, BG3 high priority (text on top) */
  setMode(BG_MODE1, BG3_MODE1_PRIORITY_HIGH);
  bgSetDisable(1); /* Disable BG1 */

  /* Initialize wave parameters (0 = BG1, 1 = BG2, etc)
     Here we apply both H and V waves to BG1 (index 0) */
  waveInit(0, 0);

  setScreenOn();

  /* Initial field buffers (triggers DMA on first VBlank) */
  guiUpdateAmp(wave.amp);
  guiUpdateFreq(wave.freq);
  guiUpdateDir(wave.dir);

  /*--- Main Loop ---*/
  while (1) {
    pad = padsCurrent(0);
    pad_down = padsDown(0);

    /* --- Amplitude: Up (+1), Down (-1) --- */
    if (pad & KEY_UP) {
      if (wave.amp < WAVE_MAX_AMP) {
        wave.amp++;
        guiUpdateAmp(wave.amp);
      }
    }
    if (pad & KEY_DOWN) {
      if (wave.amp > 0) {
        wave.amp--;
        guiUpdateAmp(wave.amp);
      }
    }

    /* --- Frequency: Right (+step), Left (-step) --- */
    if (pad & KEY_RIGHT) {
      wave.freq += WAVE_FREQ_STEP;
      guiUpdateFreq(wave.freq);
    }
    if (pad & KEY_LEFT) {
      if (wave.freq >= WAVE_FREQ_STEP) {
        wave.freq -= WAVE_FREQ_STEP;
      } else {
        wave.freq = 0;
      }
      guiUpdateFreq(wave.freq);
    }

    /* --- Direction toggle: L or R --- */
    if (pad_down & (KEY_L | KEY_R)) {
      waveChangeDir();
      guiUpdateDir(wave.dir);
    }

    /* --- Reset: Start --- */
    if (pad_down & KEY_START) {
      waveInit(0, 0);
      guiUpdateAmp(wave.amp);
      guiUpdateFreq(wave.freq);
      guiUpdateDir(wave.dir);
    }

    /* Update wave and HDMA table */
    waveUpdate();

    WaitForVBlank();
  }
  return 0;
}