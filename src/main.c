#include <gb/gb.h>
#include <gb/sgb.h>
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

// Add a mention to this and modify it if SGB mode is on
extern uint8_t CBTFX_SGB;

uint8_t joypad_p();
uint8_t menu_counter = 0;
uint8_t joypad_last_frame = 0;
uint8_t joypad_current_frame = 0;
uint8_t music = 0;

uint8_t joypad_p(){
    joypad_last_frame = joypad_current_frame;
    joypad_current_frame = joypad();
    return (joypad_current_frame & ~joypad_last_frame);
}

void update_screen(){
	gotoxy(11,7);
	printf("%hx", menu_counter);
}

void update_audio(){
	if(music) hUGE_dosound();
	CBTFX_update();
}

void main(void){
	
	// Check SGB (bit 6 should at least be on if SGB)
	if(sgb_check()){
		CBTFX_SGB = 0xff;
	}

    // Enable audio output
    NR52_REG = 0x80;
    NR51_REG = 0xff;
    NR50_REG = 0x77;
    
    __critical {
        add_VBL(update_audio);
    }

    printf(" \n  ");
	printf("CBT-FX DEMO CART");
	printf("\n\n\n\n\n\n       SFX_");	
	printf("\n\n\n\n   Press START to\n    toggle music\n\n\n Press <- and -> to  pick and A to play");

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
				if(music){
					// Settings NR52 to 0 clears all sound regs
					NR52_REG = 0;
				    NR52_REG = 0x80;
				    NR51_REG = 0xff;
				    NR50_REG = 0x77;
				}else{
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