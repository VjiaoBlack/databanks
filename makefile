COMPILE=gcc -g -O2 -Wall -c
LINK=gcc -g -O2 -Wall

design: build build/keyboard.o build/xterm_control.o build/ben.o
	$(LINK) build/keyboard.o build/xterm_control.o build/ben.o -o ben

build/keyboard.o: xterm/keyboard.c
	$(COMPILE) xterm/keyboard.c -o build/keyboard.o

build/xterm_control.o: xterm/xterm_control.c
	$(COMPILE) xterm/xterm_control.c -o build/xterm_control.o

build/ben.o: ben.c
	$(COMPILE) ben.c -o build/ben.o

build:
	mkdir build
