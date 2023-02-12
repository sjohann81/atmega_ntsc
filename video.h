#define VIDEO_RES_X		144		/* 144 or 128 pixels */
#define VIDEO_RES_Y		100
#define CLKS			(F_CPU / 1000 / 5 - 1)
#define SLEEP_CLKS		1000
#define VLINE_CLKS		1018		/* cycles = 63.5 * 16 (in fact, 63.625 * 8 for 60Hz) */
#define VTOP			25		/* should be 30 instead? */
#define VBOTTOM			225		/* should be 230 instead? */

#define FONT_X			8
#define FONT_Y			8

extern volatile int16_t vline;

#if VIDEO_RES_X == 144
extern int8_t vram[1800];
#else
extern int8_t vram[1600];
#endif

int video_setup(void);
