COMPILE=gcc -g  -c
LINK=gcc -g 

all: myui2 mystore ben

design: build build/keyboard.o build/xterm_control.o build/ben.o mystore
	$(LINK) build/keyboard.o build/xterm_control.o build/ben.o -o ben

mystore: build build/mystore.o
	$(LINK) build/mystore.o -o mystore

myui2: build/myui2.o build/myui2_util.o build/keyboard.o build/xterm_control.o
	$(COMPILE) build/myui2.o build/myui2_util.o build/keyboard.o build/xterm_control.o -o myui2
	
build/myui2.o: myui2.c myui2.h
	$(COMPILE) myui2.c -o build/myui2.o

build/myui2_util.o: myui2_util.c myui2.h
	$(COMPILE) myui2_util.c -o build/myui2_util.o
	
build/keyboard.o: xterm/keyboard.c xterm/keyboard.h
	$(COMPILE) xterm/keyboard.c -o build/keyboard.o
	
build/xterm_control.o: xterm/xterm_control.c xterm/xterm_control.h
	$(COMPILE) xterm/xterm_control.c -o build/xterm_control.o
	
build/mystore.o: mystore.c
	$(COMPILE) mystore.c -o build/mystore.o
