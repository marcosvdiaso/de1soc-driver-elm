CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -std=c99 -marm

all: driver

driver: main.c driver.s
	$(CC) $(CFLAGS) -o driver main.c driver.s -lm -lrt

clean:
	rm -f driver	