CC      = arm-linux-gnueabihf-gcc
CFLAGS  = -g -std=c99 -marm
LDLIBS  = -lm -lrt

all: driver teste_vga

# Programa principal (benchmark com metricas)
driver: main.c vga.c driver.s
	$(CC) $(CFLAGS) -o $@ main.c vga.c driver.s $(LDLIBS)

# Teste: roda 1 inferencia e exibe a imagem no VGA
teste_vga: teste_vga.c vga.c driver.s
	$(CC) $(CFLAGS) -o $@ teste_vga.c vga.c driver.s $(LDLIBS)

clean:
	rm -f driver teste_vga