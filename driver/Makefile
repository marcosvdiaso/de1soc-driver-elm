CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -std=c99 -marm
LDLIBS  = -lm -lrt

all: driver

driver: main.c vga.c mouse.c csv.c driver.s
	$(CC) $(CFLAGS) -o $@ main.c vga.c mouse.c csv.c driver.s $(LDLIBS)

clean:
	rm -f driver