#include "myui2.h"

static struct Record* records;
static int n_records, selected;
static char *version, *author, *first_time, *last_time;

/* ---------------------- FUNCTIONS CALLED BY main() ----------------------- */

/* Set up by clearing the screen, etc. */
void setup(void) {
    int i = 0;
    setbuf(stdout, NULL);  // Turn off stdout buffering
    read_stat();
    records = malloc(n_records * sizeof(struct Record));
    for (i = 1; i <= n_records; i++)
        read_record(i);
}

/* Main menu loop. */
void loop(void) {
    int mode = MODE_MAIN;
    int key;

    selected = 0;
    reset();
    display_header();
    display_records();
    while (1) {
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case 'q':
                return;  // Exit the loop and run finish()
            case 'n':
                new_entry();
        }
    }
}

/* I think this requires a new loop */ //edited@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void new_entry() {
    int key, cursorpos = 0;
    int cursorr = 8, cursorc = 23;
    int textpos = 0;
    int subjpos = 0, bodypos = 0; //where subject and body in text ends

    char* subject = malloc(sizeof(char) * 31);
    char* body = malloc(sizeof(char) * 141);

    char* subjectbase = subject;
    char* bodybase = body;

    subject[0] = body[0] = '\0';

    display_header();
    display_records();
    display_editbox(); 

    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
    xt_par0(XT_CH_UNDERLINE);

    while (1) {  // all the switching logic
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_F5:
                clear_editbox();
                display_header();
                display_records();
                return;
            case KEY_ENTER:
                printf("111111111111\n");
                ReadMystoreFromChild("add", subject, body, NULL);
                clear_editbox();
                display_header();
                display_records();
                return;
            case KEY_RIGHT:
                if (1) {
                    if (cursorpos) {
                        if (textpos >= bodypos) {
                            break;
                        }
                    } else if (textpos >= subjpos) {
                        break;
                    }


                    if (cursorc > 68 && cursorr == 11) {
                        break;
                    } else if (cursorc > 69 && cursorr < 11) {
                        cursorc = 23;
                        cursorr++;
                        textpos++;
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    } else {
                        cursorc++;
                        textpos++;
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                break;
            case KEY_LEFT:
                if (cursorc > 23) {
                    cursorc--;
                    textpos--;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }
                break;
            case KEY_DOWN:
                if (cursorpos == 0) 
                    xt_par2(XT_SET_ROW_COL_POS, 9, 23);
                cursorpos = 1;
                cursorr = 9;
                cursorc = 23;
                textpos = 0;
                break;
            case KEY_UP:
                if (cursorpos == 1)
                    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
                cursorpos = 0;
                cursorr = 8;
                cursorc = 23;
                textpos = 0;
                break;
            case KEY_BACKSPACE:
                if (cursorc > 23) {
                    cursorc--;
                    textpos--;
                    if (cursorpos) { //body
                        if (body[textpos+1] == '\0')
                            body[textpos] = '\0';
                        else 
                            body[textpos] = ' ';
                        bodypos--;
                    } else { //subject
                        if (subject[textpos+1] == '\0')
                            subject[textpos] = '\0';
                        else
                            subject[textpos] = ' ';
                        subjpos--;
                    }
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    putchar(' ');
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                } else if (cursorc == 23 && cursorr > 9) {
                    textpos--;
                    cursorc = 69;
                    cursorr--;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    putchar(' ');
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }
                break;
            default:
                if (cursorpos == 1) {
                    if (cursorc > 68 && cursorr == 11) {
                        break;
                    } else if (cursorc > 69 && cursorr < 11) {
                        cursorc = 23;
                        cursorr++;
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                if (cursorpos == 0 && cursorc > 52) {
                    break;
                }
                if ((key >= 'A' && key <= '~') || (key >= '0' && key <= '9' )) {
                    putchar((char)key);
                    cursorc++;
                    textpos++;
                    if (cursorpos) { //body
                        if (body[textpos-1] == '\0')
                            body[textpos] = '\0';
                        body[textpos-1] = (char)key;
                        bodypos++;
                    } else { //subject
                        if (subject[textpos-1] == '\0')
                            subject[textpos] = '\0';
                        subject[textpos-1] = (char)key;
                        subjpos++;
                    }
                }
                break;
        }
    }
}

/* Reset the keyboard and screen. */
void finish(void) {
    reset();
    getkey_terminate();
}

/* ----------------------- DATA RETRIEVAL FUNCTIONS ------------------------ */

/* Read the output of "mystore stat" into n_records, version, author,
   first_time, and last_time. */
void read_stat(void) {
    ReadMystoreFromChild("stat", NULL, NULL, NULL);
    ParseInput();

    version = malloc((strlen(nvs[1].value) + 1) * sizeof(char));
    author = malloc((strlen(nvs[2].value) + 1) * sizeof(char));

    strcpy(version, nvs[1].value);
    strcpy(author, nvs[2].value);
    n_records = atoi(nvs[3].value);

    if (n_records > 0) {
        first_time = malloc((strlen(nvs[4].value) + 1) * sizeof(char));
        last_time = malloc((strlen(nvs[5].value) + 1) * sizeof(char));
        strcpy(first_time, nvs[4].value);
        strcpy(last_time, nvs[5].value);
    }
}

/* Read the output of "mystore display" for the given record into records. */
void read_record(int id) {
    struct Record record;
    char str_id[6];  // Basically the max number of digits in an record ID

    sprintf(str_id, "%d", id);
    ReadMystoreFromChild("display", str_id, NULL, NULL);
    ParseInput();

    record.id = id;
    record.time = malloc((strlen(nvs[2].value) + 1) * sizeof(char));
    record.subject = malloc((strlen(nvs[3].value) + 1) * sizeof(char));
    record.body = malloc((strlen(nvs[4].value) + 1) * sizeof(char));

    strcpy(record.time, nvs[2].value);
    strcpy(record.subject, nvs[3].value);
    strcpy(record.body, nvs[4].value);

    records[id] = record;
}

/* --------------------------- DRAWING FUNCTIONS --------------------------- */

/* Reset the screen to get ready for drawing a new display. */
void reset(void) {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
}

/* Print a record ID, depending on whether it is highlighted. */
void print_id(int id, int highlight) {
    if (highlight)
        printf("%s->%s %2d ", XT_CH_RED, XT_CH_YELLOW, id);
    else
        printf("%s   %2d ", XT_CH_YELLOW, id);
    xt_par0(XT_CH_DEFAULT);
}

/* Display the header with the version, record count, times, and help. */
#define KEY_COLOR XT_CH_GREEN, XT_CH_DEFAULT
void display_header(void) {
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    xt_par0(XT_CH_INVERSE);
    printf(" MYSTORE :: PBrooks :: Version 0.90                                   5 Records ");
    xt_par0(XT_CH_NORMAL);
    xt_par2(XT_SET_ROW_COL_POS, 2, 1);
    printf("%s[W/S]%s Scroll   %s[N]%s New   %s[F]%s Find   %s[G]%s Go To         First: 2013-11-04 20:36:55", KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
    xt_par2(XT_SET_ROW_COL_POS, 3, 1);
    printf("%s[Enter]%s Edit             %s[H]%s Help   %s[Q]%s Quit           Last: 2013-11-04 21:10:10", KEY_COLOR, KEY_COLOR, KEY_COLOR);
    xt_par2(XT_SET_ROW_COL_POS, 5, 1);
    xt_par0(XT_CH_CYAN);
    printf("   ID SUBJECT                                                TIME               ");
    xt_par0(XT_CH_DEFAULT);
}

/* Display all records. */
void display_records(void) {
    struct Record* record;
    int i, row = HEADER_OFFSET;

    for (i = 0; i < n_records; i++) {
        record = &records[i];
        xt_par2(XT_SET_ROW_COL_POS, row, 1);
        print_id(record->id, i == selected);
        printf("%s", record->subject);
        xt_par2(XT_SET_ROW_COL_POS, row++, COLS - 18);
        printf("%s", record->time);
        if (i == selected) {
            xt_par2(XT_SET_ROW_COL_POS, row++, BODY_OFFSET);
            printf("%s", record->body);
        }
    }
}

/* we also need this edit box */ //EDIT@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void display_editbox() {
    xt_par2(XT_SET_ROW_COL_POS, 6, 10);
    printf(XT_CH_INVERSE);
    printf("                        CREATE NEW ENTRY                        \n");
    xt_par2(XT_SET_ROW_COL_POS, 7, 10);
    printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 8, 10);
    printf("  %s  %sSubject:%s %s                              %s                   %s  \n", XT_CH_NORMAL, XT_CH_YELLOW, XT_CH_DEFAULT, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 9, 10);
    printf("  %s   %sBody:%s   %s                                               %s  %s  \n", XT_CH_NORMAL, XT_CH_YELLOW, XT_CH_DEFAULT, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 10, 10);
    printf("  %s           %s                                               %s  %s  \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 11, 10);
    printf("  %s           %s                                              %s   %s  \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 12, 10);
    printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 13, 10);
    printf("  %s     %s[Arrow Keys]%s Navigate   %s[Enter]%s Save   %s[F5]%s Cancel     %s  \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 14, 10);
    printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 15, 10);
    printf("                                                                \n");
    printf(XT_CH_NORMAL);
}

void clear_editbox() {
    xt_par0(XT_CH_NORMAL);
    int startc = 10, endc = 75, startr = 6, endr = 16;
    int i, j;
    for (i = startr; i < endr; i++) {
        xt_par2(XT_SET_ROW_COL_POS, i, startc);
        for (j = startc; j < endc; j++) {
            printf(" ");
            xt_par2(XT_SET_ROW_COL_POS, i, j);
        }
    }
}


/* ----------------------------- MAIN FUNCTION ----------------------------- */

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
