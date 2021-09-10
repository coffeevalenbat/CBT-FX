#include <gb/gb.h>
#include <stdio.h>
#include <gb/console.h>
#include <stddef.h>
#include "../include/cbtfx.h"
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

int joypad_p();
uint8_t menu_counter = 0;
uint8_t joypad_last_frame = 0;
uint8_t joypad_current_frame = 0;

const unsigned char * SFX_list[] = { &SFX_00[0][0], &SFX_01[0][0], &SFX_02[0][0], &SFX_03[0][0], &SFX_04[0][0], &SFX_05[0][0], &SFX_06[0][0], &SFX_07[0][0], &SFX_08[0][0], &SFX_09[0][0] };
const unsigned char SFX_length[] = {SFX_00_Length, SFX_01_Length, SFX_02_Length, SFX_03_Length, SFX_04_Length, SFX_05_Length, SFX_06_Length, SFX_07_Length, SFX_08_Length, SFX_09_Length};

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

	printf("\n\n\n\n\n\n\n\n Press <- and -> to  pick and A to play");
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
				CBTFX_init(SFX_list[menu_counter], SFX_length[menu_counter]);
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

			default:
			break;

		}
		
		wait_vbl_done();
	}
}

