#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "terminvaders.h"
#include "game.h"
#include "settings.h"

/* Set up the game by clearing the screen, etc. */
static void setup(void) {
    srand(time(NULL));
    setbuf(stdout, NULL);  // Turn off stdout buffering
    xt_par0(XT_CLEAR_SCREEN);
    load_rc();
}

/* Clear the screen. */
static void clear(void) {
    int r, c;
    for (r = 1; r <= ROWS; r++) {
        SETPOS(r, 1);
        for (c = 1; c <= COLS; c++)
            putchar(' ');
    }
}

/* Main menu: render the menu and return an option selected by the user. */
static int menu(void) {
    int key, choice = 0;
    int cursor_r = 1, cursor_c = 1;
    clear();
    SETPOS(1, 1);

    

    while (1) {
        switch ((key = getkey())) {
            case KEY_UP:
            case 'w':
                break;
            case KEY_DOWN:
            case 's':
                break;
            case KEY_LEFT:
            case 'a':
                break;    
            case KEY_RIGHT:
            case 'd':
                break;
            case 'q':
                return MENU_QUIT;
        }
    }
}

/* Main menu loop. */
static void loop(void) {
    int option;
    while (1) {
        switch ((option = menu())) {
            case MENU_PLAY:
                play();
                break;
            case MENU_SETTINGS:// help what is this
                configloop();
                break;
            case MENU_QUIT:
                return;
        }
    }
}

/* Reset the keyboard and screen. */
static void finish(void) {
    getkey_terminate();
    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    SETPOS(1, 1);
}

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
