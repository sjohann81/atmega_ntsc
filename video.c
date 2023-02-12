#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "video.h"

static int8_t vsync_on, vsync_off;
volatile int16_t vline;
int8_t *vram_p;

#if VIDEO_RES_X == 144
int8_t vram[1800];
#else
int8_t vram[1600];
#endif

// put the MCU to sleep JUST before the CompA ISR goes off
ISR(TIMER1_COMPB_vect, ISR_NAKED)
{
	asm(
		"sei" "\n\t"
		"sleep" "\n\t"
		"reti" "\n\t"
	);
}


/* macro to put a byte to the screen */

asm(
	".macro videobits\n\t"
  		
	"nop  \n\t"
	"bst  r4,7\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,6\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,5\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,4\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,3\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,2\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,1\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	"nop  \n\t"
	"bst  r4,0\n\t"
	"bld  r30,5\n\t"
	"out  0x0b,r30\n\t"

	".endm\n\t"
);


/* put 18 bytes (1 line, 144 pixels) to the screen */

static void pixelblast()
{
	asm(	"lds  r26,vram_p\n\t"
		"lds  r27,vram_p+1\n\t"
			
		"ldi  r30,0x40\n\t"

		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
#if VIDEO_RES_X == 144
		"ld  r4,X+\n\t"
		"videobits\n\t"
		"ld  r4,X+\n\t"
		"videobits\n\t"
#endif
		"clt  \n\t"
		"ldi  r30,0x40\n\t"

		"bld  r30,5\n\t"
		"out  0x0b,r30\n\t"	
	   );
}


/* 
 * sync generator and raster generator
 * 
 * it must be entered from sleep mode to get accurate timing
 * of the sync pulses
 */
 
ISR (TIMER1_COMPA_vect)
{
	/* start the horizontal sync pulse */
	PORTD = vsync_on;

	/* update the current scanline number */
	vline++;   
  
	/* begin inverted (vertical) synch after line 247 */
	if (vline == 248) { 
		vsync_on = 0x40;
		vsync_off = 0;
  	}
  
	/* back to regular sync after line 250 */
	if (vline == 251) {
		vsync_on = 0;
		vsync_off = 0x40;
	}  
  
  	/* start new frame after line 262 */
	if (vline == 263)
		vline = 1;
      
	/* adjust to make 5 us pulses */
	_delay_us(3);

	/* end sync pulse */
	PORTD = vsync_off;   

	if (vline >= VTOP && vline < VBOTTOM) {
		/* compute offset into VRAM */

		/* each line is repeated (200 lines, 100 pixels vertical resolution) */
		int line = (vline - VTOP) >> 1;
#if VIDEO_RES_X == 144
		/* compute offset into VRAM */
		/* each line has 18 bytes (144 pixels horizontal resolution) */
		vram_p = vram + (line << 4) + (line << 1);

		/* center image on screen */
		_delay_us(9);
#else
		/* compute offset into VRAM */
		/* each line has 16 bytes (128 pixels horizontal resolution) */
		vram_p = vram + (line << 4);
		
		_delay_us(11);
#endif

		/* blast the data to the screen */
		pixelblast();
	}         
}

int video_setup(void)
{
	/* init timer 1 to generate sync */
	OCR1A = VLINE_CLKS;
	OCR1B = SLEEP_CLKS;
	TCCR1B = (1 << WGM12) | (1 << CS10);
	TCCR1A = 0x00;
	TIMSK1 = (1 << OCIE1B) | (1 << OCIE1A);

	/* init port D */
	DDRD = 0xf0;

	/* initialize video sync variables */
	vline = 1;
	vsync_on = 0x00;
	vsync_off = 0x40;

	/* global interrupt enable */
	sei();

	/* enable sleep mode */
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();

	return 0;
}
