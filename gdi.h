uint16_t rnd(void);
void video_sync();
void video_set(uint16_t x, uint16_t y);
void video_clr(uint16_t x, uint16_t y);
void video_xor(uint16_t x, uint16_t y);
char video_get(uint16_t x, uint16_t y);
void video_clear();
void video_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void video_hline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
void video_vline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
void video_rectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color);
void video_frectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color);
void video_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void video_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void video_char(uint8_t ascii, uint16_t x0, uint16_t y0,uint16_t size, uint16_t fgcolor);
void video_print(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor);
void video_int(uint32_t n, uint16_t x, uint16_t y, uint16_t size, uint16_t fgcolor);
void video_hex(uint32_t n, uint16_t x, uint16_t y, uint16_t size, uint16_t fgcolor);
void video_float(float n, uint16_t x, uint16_t y, uint16_t size, uint16_t fgcolor);
