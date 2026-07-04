#include "gui.h"
#include "wave.h"

/*---------------------------------------------------------------------------------
    DMA Text System (BG2)
---------------------------------------------------------------------------------*/

#define TEXT_MAP_VRAM 0x6800
#define TILE_ADDR(x, y) (TEXT_MAP_VRAM + (y) * 32 + (x))

#define FIELD_AMP_LEN 5
#define FIELD_FREQ_LEN 5
#define FIELD_DIR_LEN 10

u16 buf_amp[FIELD_AMP_LEN];
u16 buf_freq[FIELD_FREQ_LEN];
u16 buf_dir[FIELD_DIR_LEN];

u8 upd_amp;
u8 upd_freq;
u8 upd_dir;

u16 bg3_text_buffer[32 * 32];

void guiVblank(void) {
  if (upd_amp) {
    dmaCopyVram((u8 *)buf_amp, TILE_ADDR(7, 8), FIELD_AMP_LEN * 2);
    upd_amp = 0;
  }
  if (upd_freq) {
    dmaCopyVram((u8 *)buf_freq, TILE_ADDR(18, 8), FIELD_FREQ_LEN * 2);
    upd_freq = 0;
  }
  if (upd_dir) {
    dmaCopyVram((u8 *)buf_dir, TILE_ADDR(7, 7), FIELD_DIR_LEN * 2);
    upd_dir = 0;
  }
}

void fillTileBuf(u16 *buf, const char *text, u8 maxlen) {
  u8 i;
  i = 0;
  while (i < maxlen && text[i]) {
    buf[i] = (text[i] - 32) | 0x2000;
    i++;
  }
  while (i < maxlen) {
    buf[i] = 0x2000;
    i++;
  }
}

void writestring(u8 x, u8 y, const char *text) {
  u16 *ptr;
  ptr = &bg3_text_buffer[y * 32 + x];
  while (*text) {
    *ptr = (*text - 32) | 0x2000;
    ptr++;
    text++;
  }
}

void u16ToStrPad(u16 val, char *buf) {
  char tmp[6];
  u8 i, len;

  if (val == 0) {
    buf[0] = '0';
    len = 1;
  } else {
    len = 0;
    while (val > 0 && len < 5) {
      tmp[len] = '0' + (val % 10);
      val = val / 10;
      len++;
    }
    for (i = 0; i < len; i++) {
      buf[i] = tmp[len - 1 - i];
    }
  }
  while (len < 5) {
    buf[len] = ' ';
    len++;
  }
  buf[5] = 0;
}

void guiInit(void) {
  u16 i;
  for (i = 0; i < 32 * 32; i++) {
    bg3_text_buffer[i] = 0x2000;
  }

  writestring(2, 2, "--- WAVE EFFECTS DEMO ---");
  writestring(2, 7, "DIR:");
  writestring(2, 8, "AMP:");
  writestring(12, 8, "FREQ:");
  writestring(1, 10, "-- CONTROLS --");
  writestring(1, 11, "UP/DOWN - AMP  | START RESET");
  writestring(1, 12, "LEFT/RIGHT - FREQ  | L/R  - DIR");

  bgInitMapSet(2, (u8 *)bg3_text_buffer, 2048, SC_32x32, 0x6800);
}

void guiUpdateAmp(u16 amp) {
  char numbuf[6];
  u16ToStrPad(amp, numbuf);
  fillTileBuf(buf_amp, numbuf, FIELD_AMP_LEN);
  upd_amp = 1;
}

void guiUpdateFreq(u16 freq) {
  char numbuf[6];
  u16ToStrPad(freq, numbuf);
  fillTileBuf(buf_freq, numbuf, FIELD_FREQ_LEN);
  upd_freq = 1;
}

void guiUpdateDir(u8 dir) {
  if (dir == WAVE_DIR_HORIZONTAL) {
    fillTileBuf(buf_dir, "HORIZONTAL", FIELD_DIR_LEN);
  } else {
    fillTileBuf(buf_dir, "VERTICAL", FIELD_DIR_LEN);
  }
  upd_dir = 1;
}
