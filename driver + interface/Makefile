CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -std=c99 -marm -Ilib
LDLIBS  = -lm -lrt

all: driver

driver: main.c src/vga.c src/mouse.c src/csv.c driver.s
	$(CC) $(CFLAGS) -o $@ main.c src/vga.c src/mouse.c src/csv.c driver.s $(LDLIBS)

clean:
	rm -f driver