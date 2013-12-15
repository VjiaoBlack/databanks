#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xterm/xterm_control.h"
#include "xterm/keyboard.h"

struct NameValue {
    char *name;
    char *value;
};

struct Record {
    int id;
    char *time;
    char *subject;
    char *body;
};

int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4);
int ParseInput(char *in, int n_in);
