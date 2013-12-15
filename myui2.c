#include "myui2.h"

struct NameValue *nvs = NULL;
int n_nvs = 0;

char input[1000];
int n_input = 0;


struct StringPosition SP[] = {
    {1,12,3,"nitems"},
    {2,60,20,"First"},
    {3,60,20,"Last"},
    {1,8,30,"author"},
    {1,16,10,"version"},
};

int nSP = sizeof(SP)/sizeof(SP[0]);


int nitems = 0;
char subject[31];
char body[141];
char errmsg[80] = ""; // default to no error message

/* Set up by clearing the screen, etc. */
void setup(void) {
    srand(time(NULL));
    setbuf(stdout, NULL); // Turn off stdout buffering
    xt_par0(XT_CLEAR_SCREEN);

    ReadMystoreFromChild("stat",NULL,NULL,NULL);
    ParseInput(input, n_input);
}

/* Clear the screen. */
void clear(void) {
    int r, c;
    for (r = 1; r <= ROWS; r++) {
        xt_par2(XT_SET_ROW_COL_POS, r, 1);
        for (c = 1; c <= COLS; c++)
            putchar(' ');
    }
}

/* Main menu loop. */
void loop(void) {
    int mode = 0;
    int key;
    while (1) {
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_F9:
                xt_par0(XT_CLEAR_SCREEN);
                xt_par0(XT_CH_NORMAL);
                xt_par2(XT_SET_ROW_COL_POS,1,1);
                getkey_terminate();
                exit(0);
        }
        display(mode);
    }
}

void print_id(char* id, int highlight) {
    if (highlight)
        printf("%s->%s %2s ", XT_CH_RED, XT_CH_YELLOW, id);
    else
        printf("%s   %2s ", XT_CH_YELLOW, id);
    printf(XT_CH_DEFAULT);
}

/* displays the stuff */
void display(int mode) {
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

/* Reset the keyboard and screen. */
static void finish(void) {
    getkey_terminate();
    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
}

void DisplayStats(char *label, char *name) {
    int loc, /*col, i,*/j; // loc gives label pos in SP[]
    //int instring = 1; //help what is this
    char *value;

    loc = FindStringPosition(label);
    //col = SP[loc].col;

    value = "";
    for (j = 0; j < n_nvs; ++j) {
        if (strcmp(nvs[j].name,name) == 0) { // if equal
            value = nvs[j].value;
            break;
        }
    }

    DisplayAt(SP[loc].row, SP[loc].col, SP[loc].length, value);
}

void DisplayAt(int row, int col, int maxlength, char *value) {
    int i, instring = 1; //what is dis

    xt_par2(XT_SET_ROW_COL_POS,row,col);
    for (i = 0; i < maxlength; i++) {
        if (value[i] == '\0')
            instring = 0;
        printf("%c",instring?value[i]:' ');
    }
}

int FindStringPosition(char *label) {
    int i;

    for (i = 0; i < nSP; i++) {
        if (strcmp(label,SP[i].name) == 0)
            return i;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
