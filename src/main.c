#include <gb/gb.h>
#include <stdio.h>
#include <gbdk/console.h>
#include <stddef.h>
#include "../include/cbtfx.h"
#include "../include/hUGEDriver.h"
#include "../include/sfx/SFX_00.h"
#include "../include/sfx/SFX_01.h"
#include "../include/sfx/SFX_02.h"
#include "../include/sfx/SFX_03.h"
#include "../include/sfx/SFX_04.h"
#include "../include/sfx/SFX_05.h"
#include "../include/sfx/SFX_06.h"
#include "../include/sfx/SFX_07.h"
#include "../include/sfx/SFX_08.h"
#include "../include/sfx/SFX_09.h"

const unsigned char * SFX_list[] = { &SFX_00[0], &SFX_01[0], &SFX_02[0], &SFX_03[0], &SFX_04[0], &SFX_05[0], &SFX_06[0], &SFX_07[0], &SFX_08[0], &SFX_09[0] };

extern const hUGESong_t bg_song;

int joypad_p();
uint8_t menu_counter = 0;
uint8_t joypad_last_frame = 0;
uint8_t joypad_current_frame = 0;
uint8_t music = 0;

int joypad_p(){
    joypad_last_frame = joypad_current_frame;
    joypad_current_frame = joypad();
    return (joypad_current_frame & ~joypad_last_frame);
}

void update_screen(){
	cls();
	gotoxy(0, 0);
    printf(" \n  ");
	printf("CBT-FX DEMO CART");

	printf("\n\n\n\n\n\n       SFX_%hx", menu_counter);
	
	printf("\n\n\n\n   Press START to\n    toggle music\n\n\n Press <- and -> to  pick and A to play");
}
void main(void){

    // Enable audio output
    NR52_REG = 0x80;
    NR51_REG = 0xFF;
    NR50_REG = 0x77;
    
    __critical {
        add_VBL(CBTFX_update);
    }

	update_screen();

	while(1){
		
		uint8_t controller = joypad_p();

		switch(controller){

			case J_A:
				CBTFX_init(SFX_list[menu_counter]);
				break;

			case J_LEFT:
				if(menu_counter == 0){
					menu_counter = 9;
				}
				else{
					menu_counter--;
				}
				update_screen();
				break;

			case J_RIGHT:
				if(menu_counter == 9){
					menu_counter = 0;
				}
				else{
					menu_counter++;
				}
				update_screen();
				break;
			case J_START:
				// Very dirty fix to stop hUGE...
				if(music){
					remove_VBL(hUGE_dosound);
					NR52_REG = 0;
					// Settings NR52 to 0 clears all sound regs
				    NR52_REG = 0x80;
				    NR51_REG = 0xFF;
				    NR50_REG = 0x77;
				}else{
					remove_VBL(CBTFX_update);
				    add_VBL(hUGE_dosound);
			        add_VBL(CBTFX_update);
					hUGE_init(&bg_song);
				}
				music = ~music;
				break;

			default:
			break;

		}
		
		wait_vbl_done();
	}
}