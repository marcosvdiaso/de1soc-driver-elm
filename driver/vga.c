#include <stdint.h>
#include "driver.h"
#include <stdio.h>

#define MMAP_BASE 0xFF200000
#define VGA_BASE 0x0030
#define VGA_DONE 0x0040
#define VGA_SPAN 0x1000

volatile uint32_t *vga_ctrl;


int vga_start()
{
    volatile uint8_t *base =
        (volatile uint8_t *)elm_mmap();

    vga_ctrl =
        (volatile uint32_t *)(base + VGA_BASE);

    return 0;
}


void vga_pixel(int x, int y, int color)
{
    uint32_t v = 0;

    v |= (x & 0b111111111);
    v |= ((y & 0b11111111) << 9);
    v |= ((color & 0b111) << 18);
    v |= ((color & 0b111) << 21);
    v |= ((color & 0b111) << 24);
    v |= (1 << 27);

    *vga_ctrl = v;
    *vga_ctrl = v & ~(1 << 27);
}

void vga_reset()
{
  for (int x = 0;x<320;x++){
    for (int y = 0;y<240;y++){
      vga_pixel(x, y, 0);
    }
  }
}

void vga_draw(uint8_t *img)
{
  for (int y =0;y<28;y++){
    for (int x =0;x<28;x++){
      uint8_t px = img[y * 28 + x];

      int c = px >>5;

      for(int yy = 0; yy <8; yy++){
        for(int xx = 0; xx <8; xx++){
          int xtotal = 48 + x*8 + xx;
          int ytotal = 8+y*8+yy;

          vga_pixel(xtotal, ytotal, c);
        }
      }
    }
  }
}

void vga_drawing(uint8_t *img)
{
  for (int y =0;y<28;y++){
    for (int x =0;x<28;x++){
      uint8_t px = img[y * 28 + x];

      for(int yy = 0; yy <8; yy++){
        for(int xx = 0; xx <8; xx++){
          int xtotal = 48 + x*8 + xx;
          int ytotal = 8+y*8+yy;

          vga_pixel(xtotal, ytotal, px);
        }
      }
    }
  }
}

void vga_draw_mouse(signed int xa, signed int ya, signed int oldx, signed int oldy)
{
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 24; x++) {
            vga_pixel(oldx + x, oldy + y, 0);
            vga_pixel(xa + x, ya + y, 255);
        }
    }
}