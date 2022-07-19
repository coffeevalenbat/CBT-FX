/*

CoffeeBaT-FX (CBT-FX)

Very simple sound effect driver by Coffee 'Valen' Bat

Thanks to bbbbbr for getting my code to ACTUALLY work

*/

#include <gb/gb.h>
#include <gb/sgb.h>
#include "cbtfx.h"
#include "hUGEDriver.h"
#define MUSIC_DRIVER_CH2_ON hUGE_mute_channel(HT_CH2, 0);
#define MUSIC_DRIVER_CH2_OFF hUGE_mute_channel(HT_CH2, 1);
#define MUSIC_DRIVER_CH4_ON hUGE_mute_channel(HT_CH4, 0);
#define MUSIC_DRIVER_CH4_OFF hUGE_mute_channel(HT_CH4, 1);

const unsigned char CBTFX_HEADER[] = "CBT-FX BY COFFEEBAT 2021-22";
const uint8_t * CBTFX_pointer;
uint8_t CBTFX_size = 0;
uint8_t CBTFX_repeater = 0;
uint8_t CBTFX_panning = 0;
uint8_t CBTFX_priority = 0;
uint8_t CBTFX_ch_used = 0;
uint8_t CBTFX_SGB = 0;

//Restart values and point to the new sfx
void CBTFX_init(const unsigned char * SFX) NONBANKED {
    if ((*SFX & 0x0f) < CBTFX_priority) return;

    // Check SGB
    if((*SFX & CBTFX_SGB) & 64){
            *SFX++;
            *SFX++;
            sgb_transfer(SFX);
            return;
    }

    // To avoid hanging notes
    if (CBTFX_ch_used & 128) NR21_REG = NR22_REG = NR23_REG = NR24_REG = 0;
    if (CBTFX_ch_used & 32) NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
    
    // To avoid the priority system leaving some channels turned off (Don't ask how I discovered this...)
    MUSIC_DRIVER_CH2_ON;
    MUSIC_DRIVER_CH4_ON;
    
    CBTFX_priority = *SFX & 0x0f;
    CBTFX_ch_used = *SFX++;
    CBTFX_size = *SFX++;
    
    // Go back to the header and then skip 5 SGB bytes on non SGB mode
    // Since we just copied the first SFX byte to ch_used, we can look IT up instead of going back and forth
    if(CBTFX_ch_used & 64){
            *SFX++;
            *SFX++;
            *SFX++;
            *SFX++;
            *SFX++; 
    }
    
    CBTFX_repeater = 0;
    CBTFX_pointer = SFX;
    if (CBTFX_ch_used & 128) MUSIC_DRIVER_CH2_OFF;
    if (CBTFX_ch_used & 32) MUSIC_DRIVER_CH4_OFF;
}

void CBTFX_update(void) NONBANKED {
    if (CBTFX_size != 0){ // If we have an SFX to play...

    	if(CBTFX_repeater != 0){ // If we are still playing a frame
    		CBTFX_repeater--; // Remove one from the frame counter
    	}else{

        	CBTFX_repeater = *CBTFX_pointer++; // Load the frame's length
			if(CBTFX_repeater & 0x80){
    	        uint8_t mask = 0; // Mask to avoid muting an unused channel
    	        if (CBTFX_ch_used & 128) mask |= 0x22;
    	        if (CBTFX_ch_used & 32) mask |= 0x88;
    	        NR51_REG &= ~mask; // Mask out the CH2 and CH4 pan values
    	        NR51_REG |= mask & *CBTFX_pointer++; // And write ours
            }
            
            // AND out the pan byte after we used it
            CBTFX_repeater &= 0x7f;

            
            if (CBTFX_ch_used & 128){
                NR21_REG = *CBTFX_pointer++;
                NR22_REG = *CBTFX_pointer & 0xf0; // To assure the envelope is set to 0
            }
            
            if (CBTFX_ch_used & 32){
                NR42_REG = *CBTFX_pointer << 4; // Volume for the noise channel is the lower 4 bits of the same byte
            }

            CBTFX_pointer++;

            // If CH2 isn't used, we omit this data
            if (CBTFX_ch_used & 128) {
                NR23_REG = *CBTFX_pointer++; // The lower 8 bits of the frequency
                NR24_REG = *CBTFX_pointer++; // Higher 3 bits of the frequency + making sure the length isn't used and triggering the channel
            }

            // If CH4 isn't used, we omit this data
            if (CBTFX_ch_used & 32) {
                NR43_REG = *CBTFX_pointer++; // Noise freq
                NR44_REG = 0x80; // Trigger the noise channel
            }

            CBTFX_size--;

            if(CBTFX_size == 0){
                MUSIC_DRIVER_CH2_ON;
                MUSIC_DRIVER_CH4_ON;
                if (CBTFX_ch_used & 128){
                    NR21_REG = NR22_REG = NR23_REG = NR24_REG = 0;
                }
                if (CBTFX_ch_used & 32){
                    NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
                }
                CBTFX_ch_used = 0;
                CBTFX_priority = 0;
                #if (MONO_MUSIC==1)  
					NR51_REG = 0xff;
				#endif
            }

        }
    }
}
