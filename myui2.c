#include "myui2.h"

/* Set up by clearing the screen, etc. */
void setup(void) {
    setbuf(stdout, NULL);  // Turn off stdout buffering
    // ReadMystoreFromChild("stat", NULL, NULL, NULL);
    // ParseInput(input, n_input);
}

/* Main menu loop. */
void loop(void) {
    int mode = 0;
    int key;
    while (1) {
        display(mode);
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case 'q':
                return;
        }
    }
}

/* Reset the keyboard and screen. */
static void finish(void) {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    getkey_terminate();
}

/* Print a record ID, depending on whether it is highlighted. */
void print_id(char* id, int highlight) {
    if (highlight)
        printf("%s->%s %2s ", XT_CH_RED, XT_CH_YELLOW, id);
    else
        printf("%s   %2s ", XT_CH_YELLOW, id);
    xt_par0(XT_CH_DEFAULT);
}

/* Displays the current UI mode. */
void display(int mode) {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    switch (mode) {
        case 0:
            printf(XT_CH_INVERSE);
            printf(" MYSTORE :: PBrooks :: Version 0.90                                   5 Records \n");
            printf(XT_CH_NORMAL);
            printf("%s[W/S]%s Scroll   %s[N]%s New   %s[F]%s Find   %s[G]%s Go To         First: 2013-11-04 20:36:55\n", KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
            printf("%s[Enter]%s Edit             %s[H]%s Help   %s[Q]%s Quit           Last: 2013-11-04 21:10:10\n", KEY_COLOR, KEY_COLOR, KEY_COLOR);
            printf("\n");
            printf(XT_CH_CYAN);
            printf("   ID SUBJECT                                                TIME               \n");
            printf(XT_CH_DEFAULT);
            print_id("1", 0); printf("Hello, world!                                          2013-11-04 20:36:55\n");
            print_id("2", 1); printf("Lorem ipsum dolor sit amet                             2013-11-04 20:43:04\n");
            printf("        Lorem ipsum dolor sit amet, consectetur adipisicing\n");
            printf("        elit, sed do eiusmod tempor incididunt ut labore et\n");
            printf("        dolore magna aliqua.\n");
            print_id("3", 0); printf("Another Entry                                          2013-11-04 20:46:25\n");
            print_id("4", 0); printf("A Fourth Entry                                         2013-11-04 20:46:29\n");
            print_id("5", 0); printf("Entry Five                                             2013-11-04 21:10:10\n");
    }
}

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
