#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xterm/xterm_control.h"
#include "xterm/keyboard.h"
#include "util.h"

#define ROWS 24
#define COLS 80

#define HEADER_OFFSET 6
#define BODY_OFFSET 9

#define MODE_MAIN 0
#define MODE_NEW 1
#define MODE_FIND 2
#define MODE_GOTO 3
#define MODE_EDIT 4
#define MODE_HELP 5

void read_stat(void);
void read_record(int id);

void reset(void);
void display_header(void);
void display_records(void);
