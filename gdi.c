#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "video.h"
#include "gdi.h"
#include "IBM_VGA_8x8.h"


/* PRNG */
uint16_t rnd(void)
{
	static uint32_t n = 5381L;
	uint32_t hi, lo;
	
	hi = n / 127773L;
	lo = n % 127773L;
	n = 16807L * lo - 2836L * hi;
	
	return n & 0xffff;
	
}


void video_sync()
{
	while (vline != VBOTTOM + 1);
}


/* plot one pixel on vram */
void video_set(uint16_t x, uint16_t y)
{
	if (x >= VIDEO_RES_X || y >= VIDEO_RES_Y) return;
	
#if VIDEO_RES_X == 144
	int vram_i = (y << 4) + (y << 1) + (x >> 3);
#else
	int vram_i = (y << 4) + (x >> 3);
#endif
	vram[vram_i] |= 1 << (7 - (x & 0x7));
}


/* clear one pixel on vram */
void video_clr(uint16_t x, uint16_t y)
{
	if (x >= VIDEO_RES_X || y >= VIDEO_RES_Y) return;
	
#if VIDEO_RES_X == 144
	int vram_i = (y << 4) + (y << 1) + (x >> 3);
#else
	int vram_i = (y << 4) + (x >> 3);
#endif
	vram[vram_i] &= ~(1 << (7 - (x & 0x7)));
}


/* invert one pixel on vram */
void video_xor(uint16_t x, uint16_t y)
{
	if (x >= VIDEO_RES_X || y >= VIDEO_RES_Y) return;
	
#if VIDEO_RES_X == 144
	int vram_i = (y << 4) + (y << 1) + (x >> 3);
#else
	int vram_i = (y << 4) + (x >> 3);
#endif
	vram[vram_i] ^= 1 << (7 - (x & 0x7));
}


/* return value of pixel on vram */
char video_get(uint16_t x, uint16_t y)
{
	if (x >= VIDEO_RES_X || y >= VIDEO_RES_Y) return -1;
	
#if VIDEO_RES_X == 144
	int vram_i = (y << 4) + (y << 1) + (x >> 3);
#else
	int vram_i = (y << 4) + (x >> 3);
#endif
	return (vram[vram_i] & 1 << (7 - (x & 0x7)));
}


void video_clear()
{
	uint16_t i;
	
	video_sync();
	
	for (i = 0; i < sizeof(vram); i++)
		vram[i] = 0;
}


void video_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	int x = x1 - x0;
	int y = y1 - y0;
	int dx = abs(x);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y);
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	void (*video_pixel)();
	
	switch (color) {
	case 0 : video_pixel = video_clr; break;
	case 1 : video_pixel = video_set; break;
	default : video_pixel = video_xor; break;
	}
	
	for (;;) {
		video_pixel(x0, y0);
		
		e2 = 2 * err;
		if (e2 >= dy) {
			if (x0 == x1)
				break;
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			if (y0 == y1) break;
			err += dx;
			y0 += sy;
		}
	}
}


void video_hline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color)
{
	int i;
	void (*video_pixel)();
	
	switch (color) {
	case 0 : video_pixel = video_clr; break;
	case 1 : video_pixel = video_set; break;
	default : video_pixel = video_xor; break;
	}
	
	for (i = 0; i < length; i++)
		video_pixel(x0 + i, y0);
}


void video_vline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color)
{
	int i;
	void (*video_pixel)();
	
	switch (color) {
	case 0 : video_pixel = video_clr; break;
	case 1 : video_pixel = video_set; break;
	default : video_pixel = video_xor; break;
	}

	for (i = 0; i < length; i++)
		video_pixel(x0, y0 + i);
}


void video_rectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color)
{
	video_hline(x0, y0, length, color);
	video_hline(x0, y0 + width, length, color);
	video_vline(x0, y0 + 1, width - 1, color);
	video_vline(x0 + length, y0, width + 1, color);
}


void video_frectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color)
{
	int i;
	
	for (i = 0; i < width; i++)
		video_hline(x0, y0 + i, length, color);
}


void video_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int x = -r, y = 0, err = 2 - 2 * r, e2;
	void (*video_pixel)();
	
	switch (color) {
	case 0 : video_pixel = video_clr; break;
	case 1 : video_pixel = video_set; break;
	default : video_pixel = video_xor; break;
	}
	
	do {
		video_pixel(x0 - x, y0 + y);
		video_pixel(x0 + x, y0 + y);
		video_pixel(x0 + x, y0 - y);
		video_pixel(x0 - x, y0 - y);
		
		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x)
				e2 = 0;
		}
		if (e2 > x)
			err += ++x * 2 + 1;
	} while (x <= 0);
}


void video_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	video_line(x0, y0, x1, y1, color);
	video_sync();
	video_line(x1, y1, x2, y2, color);
	video_sync();
	video_line(x2, y2, x0, y0, color);
}


void video_char(uint8_t ascii, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor)
{
	uint8_t c, b;
	int i;
	
	video_sync();
	
	for (i = 0; i < FONT_Y; i++) {
		c = pgm_read_byte(&ibm_8x8[ascii][i]);
		for (b = 0; b < 8; b++) {
			if ((c >> b) & 0x01)
				video_frectangle(x0 + (8 - b) * size, y0 + i * size, size, size, fgcolor);
		}
	}
}


void video_print(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor)
{
	while (*string) {
		video_char(*string, x0, y0, size, fgcolor);
		string++;

		if (x0 < VIDEO_RES_X - 1 - (FONT_X + FONT_X) * size)
			x0 += FONT_X * size;
	}
}


static int32_t ftoa(float f, char *outbuf, int32_t precision)
{
	int32_t mantissa, int_part, frac_part, exp2, i;
	char *p;
	union float_long {
		float f;
		int32_t l;
		uint32_t u;
	} fl;

	p = outbuf;

	if (f < 0.0) {
		*p = '-';
		f = -f;
		p++;
	}

	fl.f = f;

	exp2 = (fl.l >> 23) - 127;
	mantissa = (fl.l & 0xffffff) | 0x800000;
	frac_part = 0;
	int_part = 0;

	if (exp2 >= 31) {
		return -1;	/* too large */
	} else {
		if (exp2 >= -23) {
			if (exp2 >= 23) {
				int_part = mantissa << (exp2 - 23);
			} else {
				if (exp2 >= 0){
					int_part = mantissa >> (23 - exp2);
					frac_part = (mantissa << (exp2 + 1)) & 0xffffff;
				} else {
					frac_part = (mantissa & 0xffffff) >> (-(exp2 + 1));
				}
			}
		}
	}

	if (int_part == 0) {
		*p = '0';
		p++;
	} else {
		itoa(int_part, p, 10);
		while (*p) p++;
	}
	*p = '.';
	p++;

	for (i = 0; i < precision; i++) {
		frac_part = (frac_part << 3) + (frac_part << 1);
		*p = (frac_part >> 24) + '0';
		p++;
		frac_part = frac_part & 0xffffff;
	}

	*p = 0;

	return 0;
}


void video_int(uint32_t n, uint16_t x, uint16_t y, uint16_t size, uint16_t fgcolor)
{
	char buf[30];

	itoa(n, buf, 10);
	video_print(buf, x, y, size, fgcolor);
}


void video_hex(uint32_t n, uint16_t x, uint16_t y, uint16_t size, uint16_t fgcolor)
{
	char buf[30];

	itoa(n, buf, 16);
	video_print(buf, x, y, size, fgcolor);
}

void video_float(float n, uint16_t x, uint16_t y, uint16_t size, uint16_t fgcolor)
{
	char buf[30];

	ftoa(n, buf, 3);
	video_print(buf, x, y, size, fgcolor);
}
