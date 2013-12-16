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
    int body_lines;
};

int ReadMystoreFromChild(char*, char*, char*, char*);
int ParseInput();

struct NameValue *nvs;
int n_nvs;
