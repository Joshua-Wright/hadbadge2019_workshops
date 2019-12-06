/* vim: set ts=4 st=4 : */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "mach_defines.h"
#include "sdk.h"
#include "gfx_load.h"
#include "cache.h"

//Pointer to the framebuffer memory.
uint8_t *fbmem;

//The dimensions of the framebuffer
#define FB_WIDTH 512
#define FB_HEIGHT 320

//Convenience macros
#define COMP_COLOR(A, R, G, B) ((((A) & 0xFF) << 24) | \
								(((B) & 0xFF) << 16) | \
								(((G) & 0xFF) <<  8) | \
								(((R) & 0xFF) <<  0))
#define FB_PIX(X, Y) fbmem[(X) + ((Y) * FB_WIDTH)]

void iter_fire();

void create_fire_palette(void) {

	// transparent to blue (leaving the first 16 for the tileset)
	// this could be as well just black to blue, but why not. :)
	for (int i = 0; i < 16; i++) {
		GFXPAL[i+17] = COMP_COLOR(i << 2, 0, 0, i << 2);
	}

	// setting the remaining palette in one go
	for (uint32_t i = 0; i < 32; i++) {
		// blue to red
		GFXPAL[i +  32] = COMP_COLOR(0xFF, i << 3, 0, 64 - (i << 1));
		// red to yellow
		GFXPAL[i +  64] = COMP_COLOR(0xFF, 0xFF, i << 3, 0);
		// yellow to white
		GFXPAL[i +  96] = COMP_COLOR(0xFF, 0xFF, 0xFF,   0 + (i << 2));
		GFXPAL[i + 128] = COMP_COLOR(0xFF, 0xFF, 0xFF,  64 + (i << 2));
		GFXPAL[i + 160] = COMP_COLOR(0xFF, 0xFF, 0xFF, 128 + (i << 2));
		GFXPAL[i + 192] = COMP_COLOR(0xFF, 0xFF, 0xFF, 192 + i);
		GFXPAL[i + 224] = COMP_COLOR(0xFF, 0xFF, 0xFF, 224 + i);
	}
}

//Here is where the party begins
void main(int argc, char **argv) {

	// // Blank out fb while we're loading stuff by disabling all layers. This
	// // just shows the background color.
	// GFX_REG(GFX_BGNDCOL_REG)=COMP_COLOR(0xFF, 0x80, 0x0, 0x0); //a soft gray
	// GFX_REG(GFX_LAYEREN_REG)=0; //disable all gfx layers

	// FILE *f;
	// f=fopen("/dev/console", "w");
	// setvbuf(f, NULL, _IONBF, 0); //make console line unbuffered
	// // Note that without the setvbuf command, no characters would be printed
	// // until 1024 characters are buffered. You normally don't want this.
	// fprintf(f, "\033C"); //clear the console. Note '\033' is the escape character.
	// fprintf(f, "HEY!\n");
	// fprintf(f, "We are L33T Hackers, this Hack Day 2019\n");
	// fprintf(f, "asdf\n");
	// fprintf(f, "asdf\n");
	// fprintf(f, "asdf\n");

	// for (int i=0; i<24; i++) {
	// 	fprintf(f, "loop count %i\n", i);
	// }

	// fprintf(f, "\0330X\0330Y");
	// fprintf(f, "moved the cursor and wrote stuff");


	// // Reenable the tile layer to show the above text
	// GFX_REG(GFX_LAYEREN_REG)=GFX_LAYEREN_TILEA;

	// allocate framebuffer
	fbmem=calloc(FB_WIDTH, FB_HEIGHT);
	// use colors 17-256 in the colormap
	GFX_REG(GFX_FBPITCH_REG)=(17<<GFX_FBPITCH_PAL_OFF)|(FB_WIDTH<<GFX_FBPITCH_PITCH_OFF);
	// tell graphics where framebuffer is located
	GFX_REG(GFX_FBADDR_REG)=((uint32_t)fbmem);

	puts("use the color palette");
	create_fire_palette();

	// printf("write some pattern in the pixels\n");
	// for (int y=0; y < FB_HEIGHT; y++){
	// 	for (int x=0; x < FB_WIDTH; x++){
	// 		FB_PIX(x,y) = (x * y) % 239;
	// 	}
	// }
	puts("flush the cache from the CPU to the framebuffer memory");
	cache_flush(fbmem, fbmem+FB_WIDTH*FB_HEIGHT);

	GFX_REG(GFX_LAYEREN_REG)=GFX_LAYEREN_FB_8BIT|GFX_LAYEREN_FB|GFX_LAYEREN_TILEA;


	// fire effect
	
	puts("initialize fire effect on last row");
	// for (int x=0; x < FB_WIDTH; x++){
	// 	if ((MISC_REG(MISC_RNG_REG) & 0x1) == 1) {
	// 		FB_PIX(x,FB_HEIGHT -1) = 0;
	// 	} else {
	// 		FB_PIX(x,FB_HEIGHT -1) = 238;
	// 	}
	// }

	iter_fire();
	for (int i = 0; i < FB_HEIGHT; i++) {
		printf("fire iteration %i\n", i);
		iter_fire();
		cache_flush(fbmem, fbmem+FB_WIDTH*FB_HEIGHT);
		// if A button pressed, exit
		if ((MISC_REG(MISC_BTN_REG) & BUTTON_A)) {
			puts("A button pressed, exiting fire loop");
			break;
		}
	}

	printf("flush the cache from the CPU to the framebuffer memory\n");
	cache_flush(fbmem, fbmem+FB_WIDTH*FB_HEIGHT);

	printf("Wait until button A is released\n");
	while (MISC_REG(MISC_BTN_REG)) ;
	printf("Wait until button A is pressed\n");
	while ((MISC_REG(MISC_BTN_REG) & BUTTON_A)==0) ;
}

void iter_fire() {

	for (int x=0; x < FB_WIDTH; x++){
		if ((MISC_REG(MISC_RNG_REG) & 0x1) == 1) {
			FB_PIX(x,FB_HEIGHT -1) = 0;
		} else {
			FB_PIX(x,FB_HEIGHT -1) = 238;
		}
	}

	for (int y=FB_HEIGHT-1; y >= 1; y--){
	// for (int y=0; y < FB_HEIGHT; y++) {
		int any_nonzero = 0;
		for (int x=0; x < FB_WIDTH-1; x++) {
			int at = FB_PIX(x, y), left = 0, right = 0, above = 0, count=1;
			if (x > 0) {
				left = FB_PIX(x-1, y);
				count++;
			}
			if (x < FB_WIDTH - 1) {
				right = FB_PIX(x+1, y);
				count++;
			}
			if (y > 0) {
				above = FB_PIX(x, y-1);
				count++;
			}
			int avg = (at + left + right + above)/count;
			if (avg > 0) {
				avg--;
				any_nonzero = 1;
			}
			FB_PIX(x,y-1) = avg;
		}
		if (!any_nonzero) {
			break;
		}
	}
}