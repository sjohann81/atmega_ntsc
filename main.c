#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "video.h"
#include "gdi.h"

uint16_t i, j, k, l, m, n, o, p, r;

void text_demo()
{
	int size = 1;
	
	for (j = 0, k = 0; k < 127; j += FONT_Y * size) {
		for (i = 0; i < VIDEO_RES_X - 1 - FONT_X * size && k < 255; i += FONT_X * size, k++) {
			video_char(k, i, j, size, 1);
		}
	}
			
	_delay_ms(1500);
	
	video_clear();
	
	for (j = 0, k = 128; k < 255; j += FONT_Y * size) {
		for (i = 0; i < VIDEO_RES_X - 1 - FONT_X * size && k < 255; i += FONT_X * size, k++) {
			video_char(k, i, j, size, 1);
		}
	}

	_delay_ms(1500);

	video_clear();

	video_print("The quick brown", 0, VIDEO_RES_Y / 2, 1, 1);
	video_print("fox jumps over", 0, VIDEO_RES_Y / 2 + FONT_Y, 1, 1);
	video_print("the lazy dog", 0, VIDEO_RES_Y / 2 + FONT_Y * 2, 1, 1);
	
	_delay_ms(1500);
}

int xor_demo()
{
	int i = 0;
	
	for (;;) {
		video_sync();
		
		k = rnd() % VIDEO_RES_X;
		l = rnd() % VIDEO_RES_Y;

		video_xor(k, l);
			
		if (i > 500)
			break;
				
		i++;
	}
	
	return i;
}

int line_demo()
{
	int i = 0;
	
	for (;;) {
		video_sync();
		
		k = rnd() % VIDEO_RES_X;
		l = rnd() % VIDEO_RES_Y;
		m = rnd() % VIDEO_RES_X;
		n = rnd() % VIDEO_RES_Y;

		video_line(k, l, m, n, 1);
			
		_delay_ms(10);
			
		if (i > 100)
			break;
				
		i++;
	}
	
	return i;
}

int rectangle_demo()
{
	int i = 0;
	
	for (;;) {
		do {
			m = rnd() % (VIDEO_RES_X / 4);
		} while (m < 2);
		do {
			n = rnd() % (VIDEO_RES_Y / 4);
		} while (n < 2);
		
		k = rnd() % (VIDEO_RES_X - m);
		l = rnd() % (VIDEO_RES_Y - n);
		
		video_sync();
		
		if (i & 1)
			video_rectangle(k, l, m, n, 2);
		else
			video_frectangle(k, l, m / 2, n / 2, 1);
		
		_delay_ms(10);
		
		if (i > 100)
			break;
			
		i++;
	}
	
	return i;
}

int circle_demo()
{
	int i = 0;
	
	for (;;) {
		do {
			r = rnd() % (VIDEO_RES_Y / 4);
		} while (r < 3);

		k = rnd() % (VIDEO_RES_X - r * 2) + r;
		l = rnd() % (VIDEO_RES_Y - r * 2) + r; 

		video_sync();

		video_circle(k, l, r, 2);
		
		_delay_ms(10);
		
		if (i > 100)
			break;
			
		i++;
	}
	
	return i;
}

int triangle_demo()
{
	int i = 0;
	
	for (;;) {
		k = rnd() % VIDEO_RES_X;
		l = rnd() % VIDEO_RES_Y;
		m = rnd() % VIDEO_RES_X;
		n = rnd() % VIDEO_RES_Y;
		o = rnd() % VIDEO_RES_X;
		p = rnd() % VIDEO_RES_Y;
		
		video_sync();
		
		video_triangle(k, l, m, n, o, p, 2);
		
		_delay_ms(10);
		
		if (i > 100)
			break;
			
		i++;
	}
	
	return i;
}

int main(void)
{
	video_setup();
	
	while (1) {
		video_clear();
		text_demo();
		video_clear();
		xor_demo();
		video_clear();
		line_demo();
		video_clear();
		rectangle_demo();
		video_clear();
		circle_demo();
		video_clear();
		triangle_demo();
	}
}
