#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <linux/input.h>
#include <stdint.h>
#include "vga.h"
// https://sites.uclouvain.be/SystInfo/usr/include/linux/input.h.html
// https://docs.huihoo.com/doxygen/linux/kernel/3.7/structinput__event.html
// cat /proc/bus/input/devices

void draw(uint8_t *img)
{
    const char *path = "/dev/input/event0";
    int fd = open(path, O_RDONLY);

    if (fd == -1) {
        perror("erro");
        return;
    }

    struct input_event mouse;

    signed int mouse_x = 62;
    signed int mouse_y = 22;

    signed int oldx = mouse_x;
    signed int oldy = mouse_y;

    int dx = 0;
    int dy = 0;

    int modo = 0;

    vga_draw_mouse(mouse_x, mouse_y);

    while (!(mouse.type == EV_KEY &&
             mouse.code == BTN_MIDDLE &&
             mouse.value == 1))
    {
        ssize_t bytes = read(fd, &mouse, sizeof(mouse));

        if (bytes < (ssize_t)sizeof(mouse)) {
            perror("erro");
            break;
        }

        /* acumula movimento */
        if (mouse.type == EV_REL) {

            if (mouse.code == REL_X) {
                dx += mouse.value;
            }
            else if (mouse.code == REL_Y) {
                dy += mouse.value;
            }
        }

        /* troca de modo */
        else if (mouse.type == EV_KEY) {

            if (mouse.code == BTN_LEFT) {
                modo = mouse.value ? 1 : 0;
            }

            else if (mouse.code == BTN_RIGHT) {
                modo = mouse.value ? 2 : 0;
            }
        }

        /* aplica tudo sincronizado */
        else if (mouse.type == EV_SYN) {

            oldx = mouse_x;
            oldy = mouse_y;

            mouse_x += dx;
            mouse_y += dy;

            dx = 0;
            dy = 0;

            if (mouse_x < 62)  mouse_x = 62;
            if (mouse_x > 251) mouse_x = 251;

            if (mouse_y < 22)  mouse_y = 22;
            if (mouse_y > 211) mouse_y = 211;

            vga_restore(img, oldx, oldy);
            vga_draw_mouse(mouse_x, mouse_y);

            int px = (mouse_x - 62) / 7;
            int py = (mouse_y - 22) / 7;

            if (modo == 1) {

                img[py * 28 + px] = 255;
                vga_draw_cell(px, py, 255);

                if (px > 0 && img[py * 28 + (px - 1)] < 120) {
                    img[py * 28 + (px - 1)] = 120;
                    vga_draw_cell(px - 1, py, 120);
                }

                if (px < 27 && img[py * 28 + (px + 1)] < 120) {
                    img[py * 28 + (px + 1)] = 120;
                    vga_draw_cell(px + 1, py, 120);
                }

                if (py > 0 && img[(py - 1) * 28 + px] < 120) {
                    img[(py - 1) * 28 + px] = 120;
                    vga_draw_cell(px, py - 1, 120);
                }

                if (py < 27 && img[(py + 1) * 28 + px] < 120) {
                    img[(py + 1) * 28 + px] = 120;
                    vga_draw_cell(px, py + 1, 120);
                }
            }

            else if (modo == 2) {

                img[py * 28 + px] = 0;
                vga_draw_cell(px, py, 0);
            }
        }
    }

    close(fd);
}