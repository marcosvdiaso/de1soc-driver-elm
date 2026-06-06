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
    signed int mouse_x = 0;
    signed int mouse_y = 0;
    signed int oldx;
    signed int oldy;
    int dprs = 0, eprs = 0;

    while (!(mouse.type == EV_KEY && mouse.code == BTN_MIDDLE && mouse.value == 1)) {
        ssize_t bytes = read(fd, &mouse, sizeof(mouse));
        if (bytes < (ssize_t)sizeof(mouse)) {
            perror("erro");
            break;
        }

        if (mouse.type == EV_REL) {
            oldx = mouse_x;
            oldy = mouse_y;

            if (mouse.code == REL_X) {
                mouse_x += mouse.value;
            } else if (mouse.code == REL_Y) {
                mouse_y += mouse.value;
            }

            if (mouse_x < 48) mouse_x = 48;
            if (mouse_x > 264) mouse_x = 264;
            if (mouse_y < 8) mouse_y = 8;
            if (mouse_y > 224) mouse_y = 224;

            vga_draw_mouse(mouse_x, mouse_y, oldx, oldy);
        }

        if (mouse.type == EV_KEY && (mouse.code == BTN_LEFT)) {
            dprs = mouse.value;
        } else  if (mouse.type == EV_KEY && (mouse.code == BTN_RIGHT)) {
            eprs = mouse.value;
        }

        if (mouse.type == EV_SYN && dprs) {   
            int px = (mouse_x - 48) / 8;
            int py = (mouse_y - 8) / 8;
            img[py * 28 + px] = 255;
            if (px > 0) img[py * 28 + (px - 1)] = 120;
            if (px < 27) img[py * 28 + (px + 1)] = 120;
            if (py > 0) img[(py - 1) * 28 + px] = 120;
            if (py < 27) img[(py + 1) * 28 + px] = 120;
            vga_draw(img);
        } else if (mouse.type == EV_SYN && eprs) {
            int px = (mouse_x - 48) / 8;
            int py = (mouse_y - 8) / 8;
            img[py * 28 + px] = 0;
            vga_draw(img);
        }

    }
    close(fd);
}