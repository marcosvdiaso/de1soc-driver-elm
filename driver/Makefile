CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -std=c99 -marm
LDLIBS  = -lm -lrt

all: driver runteste

driver: main.c vga.c driver.s
	$(CC) $(CFLAGS) -o $@ main.c vga.c driver.s $(LDLIBS)

runteste: teste.c mouse.c vga.c driver.s
	$(CC) $(CFLAGS) -o $@ teste.c mouse.c vga.c driver.s $(LDLIBS)

clean:
	rm -f driver runteste