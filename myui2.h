#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xterm/xterm_control.h"
#include "xterm/keyboard.h"
#include "util.h"

#define ROWS 80
#define COLS 24
#define KEY_COLOR XT_CH_GREEN, XT_CH_DEFAULT

struct StringPosition {
	int row;
	int col;
	int length;
	char *name;
};

void DisplayStat(void);
void display(int mode);
void DisplayStats(char *label, char *name);
void DisplayAt(int row, int col, int maxlength, char *s);
int FindStringPosition(char *prompt);
