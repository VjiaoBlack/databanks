#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xterm/xterm_control.h"
#include "xterm/keyboard.h"

#define TRUE	1
#define FALSE	0
#define ROWS 80
#define COLS 24
#define KEY_COLOR XT_CH_GREEN, XT_CH_DEFAULT


struct NameValue {
	char *name;
	char *value;
};

struct StringPosition {
	int row;
	int col;
	int length;
	char *name;
};

int ParseInput(char *in, int n_in);
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4);
void DisplayStat(void);
void display(int mode);
void clear(void);
void DisplayStats(char *label, char *name);
void DisplayAt(int row, int col, int maxlength, char *s);
int FindStringPosition(char *prompt);
