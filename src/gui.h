#ifndef GUI_H_
#define GUI_H_

#include <snes.h>

void guiInit(void);
void guiUpdateAmp(u16 amp);
void guiUpdateFreq(u16 freq);
void guiUpdateDir(u8 dir);
void guiVblank(void);

#endif
