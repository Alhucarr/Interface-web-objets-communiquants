CC=bcm2708hardfp-gcc
CFLAGS=-O2 -static
CARD=21
NAME=tugaye-attal
CFLAGS=-W -Wall -Wextra -Wfatal-errors -O2
APP=helloworld

all: $(APP)
		scp -P 622$(CARD) $^ pi@peri:$(NAME)

helloworld.x: helloworld.c
		bcm2708hardfp-gcc -o $@ $< -O2 -static

clean:
		rm $(APP)