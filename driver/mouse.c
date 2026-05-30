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

#define X_AXIS 480
#define Y_AXIS 320
#define PATH "img/cursor.bin"

void init_mouse()
{
    const char *path = "/dev/input/event0";
    int fd = open(path, O_RDONLY);
    
    if (fd == -1) {
        perror("erro");
        return 1;
    }

    struct input_event mouse;
    signed int mouse_x = 0;
    signed int mouse_y = 0;

    uint8_t img[576];
    FILE *f = fopen(PATH, "rb");
    if (f) {
        if (fread(img, 1, 576, f) == 576) {
            vga_draw(img);
            printf("cursor foi\n");
        } else {
            printf("erro acessar bytes imagemcursor\n");
        }
        fclose(f);
    } else {
        printf("error abrir imagenscursor\n");
    }


    while (1) {
        ssize_t bytes = read(fd, &mouse, sizeof(mouse));
        if (bytes < (ssize_t)sizeof(mouse)) {
            perror("erro");
            break;
        }

        if (mouse.type == EV_REL) {
            if (mouse.code == REL_X) {
                mouse_x += mouse.value;
                vga_draw_mouse(img, mouse_x, mouse_y);
            } else if (mouse.code == REL_Y) {
                mouse_y += mouse.value;
                vga_draw_mouse(img, mouse_x, mouse_y);
            }
        } else if (mouse.type == EV_SYN && mouse.code == SYN_REPORT) {
            printf("posição %dx %dy", mouse_x, mouse_y);
        }
    }

    close(fd);
}
