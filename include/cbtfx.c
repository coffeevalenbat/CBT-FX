/*

CoffeeBaT-FX (CBT-FX)

Very simple sound effect """""Driver""""" by Coffee Bat

...And as the name implies it is very close to CBT to make the sfx...

Also thanks to bbbbbr for getting my code to ACTUALLY work

*/
#include <gb/gb.h>
const uint8_t * CBTFX_pointer;
uint8_t CBTFX_size = 0;
uint8_t CBTFX_repeater = 0;
uint8_t CBTFX_panning = 0;

//Restart values and point to the new sfx
void CBTFX_init(const unsigned char * SFX, uint8_t length){
    CBTFX_pointer = SFX;
    CBTFX_size = length + 1;
    CBTFX_repeater = 0;
    CBTFX_panning = NR51_REG;
}

void CBTFX_update(void){
    if (CBTFX_size != 0){

        NR51_REG = CBTFX_panning;

    	if(CBTFX_repeater != 0){
    		CBTFX_repeater--;
    	}else{

        	CBTFX_repeater = *CBTFX_pointer++;

            NR51_REG = (NR51_REG & 0b10101010) | *CBTFX_pointer++;

            NR21_REG = *CBTFX_pointer++;
            NR22_REG = *CBTFX_pointer++;
            NR23_REG = *CBTFX_pointer++;
            NR24_REG = *CBTFX_pointer++;

            NR41_REG = *CBTFX_pointer++;
            NR42_REG = *CBTFX_pointer++;
            NR43_REG = *CBTFX_pointer++;
            NR44_REG = *CBTFX_pointer++;

            CBTFX_size--;

            CBTFX_panning = NR51_REG;

            if(CBTFX_size == 0){
           		NR21_REG = NR22_REG = NR23_REG = NR24_REG = NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
                NR51_REG |= 0b10101010;
            }

        }
    }
}