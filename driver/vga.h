#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_draw(uint8_t *img);
void vga_pixel(int x, int y, int color);
int  vga_start(void);

#endif