COMPILE=gcc -g -O2 -Wall -c
LINK=gcc -g -O2 -Wall

# General

all: mystore myui2
	cp mystore ~/custom/databankdata/mystore
	cp myui2 ~/custom/databanks

clean:
	rm -rf build mystore myui2
	rm ~/custom/databankdata/mystore
	rm ~/custom/databanks

build:
	mkdir build

# Link

mystore: build/mystore.o
	$(LINK) build/mystore.o -o mystore

myui2: build/keyboard.o build/xterm_control.o build/myui2.o build/util.o
	$(LINK) build/keyboard.o build/xterm_control.o build/myui2.o build/util.o -o myui2

# Compile

build/mystore.o: build mystore.c
	$(COMPILE) mystore.c -o build/mystore.o

build/keyboard.o: build xterm/keyboard.c xterm/keyboard.h
	$(COMPILE) xterm/keyboard.c -o build/keyboard.o

build/xterm_control.o: build xterm/xterm_control.c xterm/xterm_control.h
	$(COMPILE) xterm/xterm_control.c -o build/xterm_control.o

build/myui2.o: build myui2.c myui2.h util.h
	$(COMPILE) myui2.c -o build/myui2.o

build/util.o: build util.c util.h
	$(COMPILE) util.c -o build/util.o
