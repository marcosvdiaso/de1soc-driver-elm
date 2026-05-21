CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -nostdlib -e main

all: driver

driver: main.s driver.s
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f driver

#CC = arm-linux-gnueabihf-gcc
#CFLAGS = -g -std=c99

#all: driver

#driver: main.c utils.c driver.s
#	$(CC) $(CFLAGS) -o driver main.c utils.c driver.s

#clean:
#	rm -f driver
