#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_draw(uint8_t *img);
void vga_pixel(int x, int y, int r, int g, int b);
int  vga_start(void);
int vga_reset(void);
void vga_draw_mouse(signed int xa, signed int ya, signed int oldx, signed int oldy);
void vga_char(char c, int x, int y, int cl);
void vga_str(char *c, int x, int y, int cl);
void vga_border();

#endif