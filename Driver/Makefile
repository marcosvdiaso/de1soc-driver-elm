CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -nostdlib -e main

all: driver

driver: main.s driver.s
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f driver