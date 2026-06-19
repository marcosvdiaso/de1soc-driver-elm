// https://github.com/dhepper/font8x8/blob/master/font8x8_basic.h

#include <stdint.h>
#include "driver.h"
#include <stdio.h>
#include "font8x8_basic.h"

#define MMAP_BASE 0xFF200000
#define VGA_BASE 0x0030
#define VGA_DONE 0x0040
#define VGA_SPAN 0x1000

volatile uint32_t *vga_ctrl;

/*inicia vga*/
int vga_start()
{
    volatile uint8_t *base =
        (volatile uint8_t *)elm_mmap();

    vga_ctrl =
        (volatile uint32_t *)(base + VGA_BASE);

    return 0;
}

/*montar pixel*/
void vga_pixel(int x, int y, int r, int g, int b)
{
    uint32_t v = 0;

    v |= (x & 0b111111111);
    v |= ((y & 0b11111111) << 9);
    v |= ((r & 0b111) << 18);
    v |= ((g & 0b111) << 21);
    v |= ((b & 0b111) << 24);
    v |= (1 << 27);

    *vga_ctrl = v;
    *vga_ctrl = v & ~(1 << 27);
}

/*reseta vga */
void vga_reset()
{
  for (int x = 0;x<320;x++){
    for (int y = 0;y<240;y++){
      vga_pixel(x, y, 0,0,0);
    }
  }
}

/*exibir imagem*/
void vga_draw(uint8_t *img)
{
  for (int y =0;y<28;y++){
    for (int x =0;x<28;x++){
      uint8_t px = img[y * 28 + x];
      int c = px >>5;

      for(int yy = 0; yy < 7; yy++){
        for(int xx = 0; xx < 7; xx++){
          int xtotal = 62 + x*7 + xx;
          int ytotal = 22 + y*7 + yy;

          vga_pixel(xtotal, ytotal, c,c,c);
        }
      }
    }
  }
}

/*desenhar cursor*/
void vga_draw_mouse(signed int xa, signed int ya)
{
    for (int y = 0; y < 7; y++) {
        for (int x = 0; x < 7; x++) {
            vga_pixel(xa + x, ya + y, 255,0,0);
        }
    }
}

/*desenhar char*/
void vga_char(char c, int x, int y, int cl){
  char* font = font8x8_basic[(int)c];
  for (int xx = 0; xx < 8; xx++){
    for (int yy =0; yy < 8; yy++){
      if (font[(xx)] & (1 <<yy)){
        vga_pixel(x+xx, y+yy, cl,cl,cl);
      }
    }
  }
}

/*desenhar string*/
void vga_str(char *c, int x, int y, int cl){
  for (int i = 0; c[i] != '\0'; i++){
    vga_char(c[i], x + (i*9),y, cl);
  }
}

/* os pixels que a imagem não ocupa são:
do (y0-22 x0-320)U(y218-240 x0-320)U(y0-240 x0-62)U(y0-240 x258-320) */
void vga_border(){
  int y, x;
  for (x=0;x<320;x++){
    for (y=0;y<22;y++){
      vga_pixel(x, y, 7,2,1);
    }
    for (y=218;y<240;y++){
      vga_pixel(x, y, 7,2,1);
    }
  }
  for (y=0; y<240;y++){
    for (x=0;x<62;x++){
      vga_pixel(x, y, 7,2,1);
    }
    for (x=258;x<320;x++){
      vga_pixel(x, y, 7,2,1);
    }
  }
}

void vga_draw_cell(int px, int py, uint8_t cor)
{
    int c = cor >> 5;

    for (int yy = 0; yy < 7; yy++) {
        for (int xx = 0; xx < 7; xx++) {
            vga_pixel(
                62 + px * 7 + xx,
                22 + py * 7 + yy,
                c, c, c
            );
        }
    }
}

void vga_restore(uint8_t *img, int mx, int my)
{
    int px0 = (mx - 62) / 7;
    int py0 = (my - 22) / 7;

    int px1 = (mx + 6 - 62) / 7;
    int py1 = (my + 6 - 22) / 7;

    for (int py = py0; py <= py1; py++) {
        for (int px = px0; px <= px1; px++) {

            if (px >= 0 && px < 28 &&
                py >= 0 && py < 28) {

                vga_draw_cell(px, py,
                              img[py * 28 + px]);
            }
        }
    }
}