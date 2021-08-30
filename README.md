
# CBT-FX: It just works
This is my attempt at writing a sound effect driver that's natively compatible with [GBDK-2020](https://github.com/gbdk-2020/gbdk-2020/) (Instead of having to use register values or converting other assembly drivers to GBDK-2020), It's made to be used alongside the [FX Hammer editor](https://www.pouet.net/prod.php?which=17337). It's aimed at people who want good sounding sound effects but don't wanna have to do custom solutions, feel free to use the translated example SFX's too (see [here](https://github.com/datguywitha3ds/CBT-FX/blob/main/src/main.c#L15)).

# Usage
1. Create your sound effects first in the before mentioned [FX Hammer editor](https://www.pouet.net/prod.php?which=17337).
 2. Copy both files in the `include` folder into your project, be sure to also add `#include "cbtfx.h"` (Or wherever you place the files) into whatever C files will use CBT-FX.
 3. Create a `const unsigned char` 2D array, the 2nd array length should be `CBTFX_LENGTH`.
 4. Use the `CBTFX_FRAME` macro to create each frame of your sound effect, this will make translating the sound effects from FX Hammer easier;
``` c
// This will play C3 for 5 frames and then fade out

const unsigned char your_SFX_name[][CBTFX_LENGTH] = { 
/*
               (1)     (2)       (3) (4) (5)        (6)     (7)   (8)
                |       |         |   |   |          |       |     | */
    CBTFX_FRAME(05, PAN_CENTER, 0x0f, 2, FX_C_3, PAN_CENTER, 0x00, 0x00),
    CBTFX_FRAME(02, PAN_CENTER, 0x0b, 2, FX_C_3, PAN_CENTER, 0x00, 0x00),
    CBTFX_FRAME(02, PAN_CENTER, 0x04, 2, FX_C_3, PAN_CENTER, 0x00, 0x00),
    CBTFX_FRAME(01, PAN_CENTER, 0x02, 2, FX_C_3, PAN_CENTER, 0x00, 0x00)
};
/*
(1) - Frame length: How long a frame should repeat for.
(2) - Channel 2 (Pulse) panning: Where the pulse channel should play, use the PAN_CENTER, PAN_LEFT and PAN_RIGHT macros.
(3) - Channel 2 (Pulse) volume: Should be between 0x00 (No sound) and 0x0f (Full volume).
(4) - Channel 2 (Pulse) wave duty: The width of channel 2's square wave, 0 (%12.5), 1 (%25), 2 (%50) or 3 (%75).
(5) - Channel 2 (Pulse) note: the note that will play in the pulse channel, CBTFX has macros for each note ranging from FX_C_0 to FX_B_5.
(6) - Channel 4 (Noise) panning: same as (2) but for the noise channel.
(7) - Channel 4 (Noise) volume: same as (3) but for the noise channel.
(8) - Channel 4 (Noise) frequency: the frequency (in hex) that will play in the noise channel.
*/
```
5. Add a call to `CBTFX_update` in your main loop so it gets called every frame, or add it as a VBL interrupt.
6. When you need to play a sound effect, call `CBTFX_init(&your_SFX_name[0][0], ARRAY_LEN(your_SFX_name))`.

This repository comes with an example project and all the sound effects from FX Hammer converted (See `src`).

# For music driver users
If you're already using a driver for music such as [hUGEdriver](https://github.com/SuperDisk/hUGEDriver), [GBT Player](https://github.com/AntonioND/gbt-player/tree/master/legacy_gbdk) or any other, you will need to add a few lines of code to the CBTFX functions to make it play ball with this.
```c
void CBTFX_init(const unsigned char * SFX, uint8_t length){
    CBTFX_pointer = SFX;
    CBTFX_size = length + 1;
    CBTFX_repeater = *SFX;
    CBTFX_panning = NR51_REG;
    //If your driver has any "Mute channel" functions, add two of them here for channel 2 and 4, something like:
    my_music_driver_mute(channel_2);
    my_music_driver_mute(channel_4);
}



//From cbtfx.c line 66
if(CBTFX_size == 0){
    NR21_REG = NR22_REG = NR23_REG = NR24_REG = NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
    NR51_REG |= 0b10101010;
    // If your driver has any "UNmute channel" functions, add two of them for channel 2 and 4.
    my_music_driver_UNmute(channel_2);
    my_music_driver_UNmute(channel_4);
}
```

ALSO remember to update CBTFX **after** your music driver, to avoid the music driver going over the sound effects.

# Pros of this "driver"
- Works with GBDK-2020 out of the box.
- Player is tiny in size
- With experience in FX Hammer, converting sfx can be fast and easy.
- SFX's are ***almost*** 1:1 to FX Hammer (See below).
# Cons
- ...Although sound effects are almost 1:1 to FX Hammer, they tend to be the tiniest bit more clicky, due to how the Game Boy's audio works.
- Sound effects tend to be a little on the chunky side, if you're working on a big game, I'd recommend keeping the sound effects in a different bank and add some bank switching lines in the CBTFX functions.
- Converting bigger sfx's can be tedious, almost CBT-like (Heh...).
- Lacks any of FX Hammer's special things.
- Only one sfx at a time.
