#ifndef CBTFX_H_INCLUDE
#define CBTFX_H_INCLUDE

#include <gb/gb.h>

void CBTFX_update(void);
void CBTFX_init(const unsigned char * SFX);

// 0 = Panning won't be reset after an SFX, 1 = Panning will be set to 0XFF after an SFX plays.
#define MONO_MUSIC 0

#endif
