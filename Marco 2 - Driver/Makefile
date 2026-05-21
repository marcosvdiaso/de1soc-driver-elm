CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -std=c99

all: driver

driver: main.c driver.s
	$(CC) $(CFLAGS) -o driver main.c driver.s -lm

clean:
	rm -f driver