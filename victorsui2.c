#include "myui2.h"

static struct Record* records;
static int n_records, selected, min_shown, max_shown;
static char *version, *author, *first_time, *last_time;

/* ---------------------- FUNCTIONS CALLED BY main() ----------------------- */

/* Set up by clearing the screen, etc. */
void setup(void) {
    int i = 0;
    setbuf(stdout, NULL);  // Turn off stdout buffering
    read_stat();
    records = malloc(n_records * sizeof(struct Record));
    for (i = 0; i < n_records; i++)
        read_record(i);
}

/* Main menu loop. */
void loop(void) {
    // int mode = MODE_MAIN;
    int key;

    selected = 0;
    reset();
    display_header();
    display_records(0);
    while (1) {
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_UP:
            case 'w':
                if (selected > 0)
                    scroll_up();
                break;
            case KEY_DOWN:
            case 's':
                if (selected < n_records - 1)
                    scroll_down();
                break;
            case 'n':
                new_entry();
                break;
            case KEY_ENTER:
                edit_entry();
                break;
            case 'q':
                return;  // Exit the loop and run finish()
        }
    }
}

/* Reset the keyboard and screen. */
void finish(void) {
    reset();
    getkey_terminate();
    // TODO: dealloc record data!
}

/* ----------------------- DATA RETRIEVAL FUNCTIONS ------------------------ */

/* Copy source into what dest points to, mallocing enough space for dest. */
void malloc_then_copy(char** dest, char* source) {
    *dest = malloc((strlen(source) + 1) * sizeof(char));
    strcpy(*dest, source);
}

/* Read the output of "mystore stat" into n_records, version, author,
   first_time, and last_time. */
void read_stat(void) {
    ReadMystoreFromChild("stat", NULL, NULL, NULL);
    ParseInput();

    malloc_then_copy(&version, nvs[1].value);
    malloc_then_copy(&author, nvs[2].value);
    n_records = atoi(nvs[3].value);
    if (n_records > 0) {
        malloc_then_copy(&first_time, nvs[4].value);
        malloc_then_copy(&last_time, nvs[5].value);
    }
    else
        first_time = last_time = NULL;
}

/* Read the output of "mystore display" for the given record into records. */
void read_record(int id) {
    struct Record record;
    char str_id[6];  // Basically the max number of digits in an record ID

    sprintf(str_id, "%d", id + 1);
    ReadMystoreFromChild("display", str_id, NULL, NULL);
    ParseInput();

    record.id = id + 1;
    malloc_then_copy(&record.time, nvs[2].value);
    malloc_then_copy(&record.subject, nvs[3].value);
    malloc_then_copy(&record.body, nvs[4].value);
    record.body_lines = (strlen(record.body) - 1) / MAX_BODY_LINE + 1;
    if (record.body_lines < 0)
        record.body_lines = 0;

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
    int len = strlen(author) + strlen(version) + NAME_OFFSET, i;

    // Draw the first line (program name, author, version, number of records)
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    xt_par0(XT_CH_INVERSE);
    printf(" MYSTORE :: %s :: Version %s", author, version);
    // Make sure that the white top bar fills the width of the row
    for (i = len; i < COLS - NUM_RECORDS_OFFSET; i++)
        putchar(' ');
    printf("%6d Records ", n_records);
    xt_par0(XT_CH_NORMAL);

    // Draw the second and third lines (help info and first/last edit times)
    xt_par2(XT_SET_ROW_COL_POS, 2, 1);
    printf("%s[W/S]%s Scroll  %s[G]%s Go To     %s[F]%s Find  %s[N]%s New",
           KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
    if (first_time != NULL) {
        xt_par2(XT_SET_ROW_COL_POS, 2, COLS - FIRST_TIME_OFFSET);
        printf("First: %s", first_time);
    }
    xt_par2(XT_SET_ROW_COL_POS, 3, 1);
    printf("%s[Enter]%s Edit  %s[Del]%s Delete  %s[H]%s Help  %s[Q]%s Quit",
           KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
    if (last_time != NULL) {
        xt_par2(XT_SET_ROW_COL_POS, 3, COLS - LAST_TIME_OFFSET);
        printf("Last: %s", last_time);
    }

    // Draw the fifth line (table header)
    xt_par2(XT_SET_ROW_COL_POS, 5, 1);
    xt_par0(XT_CH_CYAN);
    printf("   ID SUBJECT");
    xt_par2(XT_SET_ROW_COL_POS, 5, COLS - TIME_OFFSET);
    printf("TIME");
    xt_par0(XT_CH_DEFAULT);
}

/* Display all records. */
void display_records(int start) {
    int i, row = HEADER_OFFSET;

    min_shown = start;
    for (i = start; i < n_records; i++) {
        display_record(i, &row);
        if (row > ROWS - 2)
            break;
    }
    max_shown = i;
}

/* Display a certain record by ID at the given row. Increases the row. */
void display_record(int id, int* row) {
    struct Record* record = &records[id];
    int i;

    xt_par2(XT_SET_ROW_COL_POS, *row, 1);
    print_id(record->id, id == selected);
    printf("%s", record->subject);
    xt_par2(XT_SET_ROW_COL_POS, (*row)++, COLS - TIME_OFFSET);
    printf("%s", record->time);
    if (id == selected) {
        for (i = 0; i < record->body_lines; i++) {
            xt_par2(XT_SET_ROW_COL_POS, (*row)++, BODY_OFFSET);
            printf("%.*s", MAX_BODY_LINE, record->body + (i * MAX_BODY_LINE));
        }
    }
}

/* Shift the screen of displayed records up. */
void shift_up(void) {
    int row = ROWS - 2;

    xt_par2(XT_SET_ROW_COL_POS, HEADER_OFFSET, 1);
    xt_par1(XT_DELETE_LINES, 1);
    min_shown++;
    display_record(++max_shown, &row);
}

/* Shift the screen of displayed records down. */
void shift_down(void) {
    int row = HEADER_OFFSET;

    xt_par2(XT_SET_ROW_COL_POS, ROWS - 2, 1);
    xt_par1(XT_DELETE_LINES, 1);
    max_shown--;
    xt_par2(XT_SET_ROW_COL_POS, row, 1);
    xt_par1(XT_INSERT_LINES, 1);
    display_record(--min_shown, &row);
}

/* Scroll the screen up one record. */
void scroll_up(void) {
    struct Record *old = &records[selected], *new = &records[selected - 1];
    int row = HEADER_OFFSET + selected - min_shown - 1, start = row;

    xt_par2(XT_SET_ROW_COL_POS, row, 1);
    xt_par1(XT_DELETE_LINES, 2 + old->body_lines);
    xt_par1(XT_INSERT_LINES, 2 + new->body_lines);
    selected--;
    display_record(selected, &row);
    display_record(selected + 1, &row);
    if (start < MAX_SHIFT_UP_ROW && min_shown > 0)
        shift_down();
}

/* Scroll the screen down one record. */
void scroll_down(void) {
    struct Record *old = &records[selected], *new = &records[selected + 1];
    int row = HEADER_OFFSET + selected - min_shown, start = row;

    xt_par2(XT_SET_ROW_COL_POS, row, 1);
    xt_par1(XT_DELETE_LINES, 2 + old->body_lines);
    xt_par1(XT_INSERT_LINES, 2 + new->body_lines);
    selected++;
    display_record(selected - 1, &row);
    display_record(selected, &row);
    if (start > MIN_SHIFT_DOWN_ROW && n_records > max_shown + 1)
        shift_up();
}


/* ---- Edit Entry Code ----------------- */

void edit_entry(void) {


    //vvvvvvvvvvvvvvvvvvavvvvvvvvvvvvvv
    char* subject = malloc(sizeof(char) * 31);
    char* body = malloc(sizeof(char) * 141);

    strcpy(subject, records[selected].subject);
    strcpy(body, records[selected].body);

    int key, cursorpos = 0;
    int cursorr = 8, cursorc = 23;
    int textpos = 0; // where cursor is in text (body)
    int subjpos = strlen(subject), bodypos = strlen(body); // Where subject and body in text ends
    int i = 0;
    int r = 0;
    int c = 0;

    display_editbox();

    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
    xt_par0(XT_CH_UNDERLINE);
    while (textpos < subjpos) {
        putchar(subject[textpos]);
        textpos++;
    }
    textpos = 0;
    while (textpos < )// i'm  printing out the data





    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
    textpos = 0;

    // following comment is invalid, for editing is in progress.
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ALL needs to be changed, loaded from whatever.



    while (1) {  // all the switching logic
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_ENTER:
                ReadMystoreFromChild("add", subject, body, NULL);
            case KEY_F5:
                clean_up_editbox();
                return;
            case KEY_RIGHT:
                if (cursorpos) {
                    if (textpos >= bodypos)
                        break;
                }
                else if (textpos >= subjpos)
                    break;
                if (cursorc > 68 && cursorr == 11)
                    break;
                else if (cursorc > 69 && cursorr < 11) {
                    cursorc = 23;
                    cursorr++;
                    textpos++;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }
                else {
                    cursorc++;
                    textpos++;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
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
                i = 0;
                if (cursorc > 23) {
                    cursorc--;
                    textpos--;
                    if (cursorpos) {  // Body   @@@@@@@@@@@@@@@BROKENNNN
                        if (body[cursorc-22] == '\0')
                            body[cursorc-23] = '\0';
                        else {
                            i = cursorc-23;
                            r = cursorr;
                            c = cursorc;
                            while (body[i] != '\0' && i <= bodypos) { // <= is no error, but not sure if just < is enough
                                body[i] = body[i+1];
                                xt_par2(XT_SET_ROW_COL_POS, r, c++);
                                putchar(body[i+1]);
                                xt_par2(XT_SET_ROW_COL_POS, r, c);
                                i++;
                            }


                        }
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, 23 + --bodypos);
                        putchar(' ');
                    }


                    else {  // Subject
                        if (subject[cursorc-22] == '\0')
                            subject[cursorc-23] = '\0';
                        else {
                            i = cursorc-23;
                            r = cursorr;
                            c = cursorc;
                            while (subject[i] != '\0' && i <= subjpos) { // <= is no error, but not sure if just < is enough
                                subject[i] = subject[i+1];
                                xt_par2(XT_SET_ROW_COL_POS, r, c++);
                                putchar(subject[i+1]);
                                xt_par2(XT_SET_ROW_COL_POS, r, c);
                                i++;
                            }


                        }
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, 23 + --subjpos);
                        putchar(' ');

                    }

                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);

                }

                else if (cursorc == 23 && cursorr > 9) { // backspace triggers line jump
                    textpos--;
                    cursorc = 69;
                    cursorr--;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    putchar(' ');
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }

                break;



            default: 


                if (cursorpos == 1) { // body break, update
                    if (cursorc > 68 && cursorr == 11)
                        break;
                    if (textpos == 0 && (char)key == ' ')
                        break;
                    else if (bodypos == 140) 
                        break;
                    else if (cursorc > 69 && cursorr < 11) {
                        cursorc = 23;
                        cursorr++;
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                if (cursorpos == 0) {
                    if (cursorc > 52)
                        break;
                    if (subjpos == 30) 
                        break;
                    if (textpos == 0 && (char)key == ' ') 
                        break;
                }



                if (key >= ' ' && key <= '~') {
                    putchar((char) key);
                    cursorc++;
                    textpos++;
                    if (cursorpos) {  // Body @@@@@@@@@@@@@@BROOOOKENNNNNN
                        if (body[textpos-1] == '\0')
                            body[textpos] = '\0';
                        else {
                            i = bodypos + 1;
                            r = cursorr;
                            while (i >= textpos) { // dont worry about carrying '\0'
                                body[i] = body[i-1];
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                putchar(body[i]);
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                i--;
                            }
                            
                        }

                        body[textpos-1] = (char)key;
                        bodypos++;

                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                    else {  // Subject
                        if (subject[textpos-1] == '\0')
                            subject[textpos] = '\0';
                        else {
                            i = subjpos + 1;
                            r = cursorr;
                            while (i >= textpos) { // dont worry about carrying '\0'
                                subject[i] = subject[i-1];
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                putchar(subject[i]);
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                i--;
                            }
                            
                        }

                        subject[textpos-1] = (char)key;
                        subjpos++;

                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                break;
        }
    }
}


/* ----------------------------- NEW ENTRY CODE ---------------------------- */

void new_entry(void) {
    int key, cursorpos = 0;
    int cursorr = 8, cursorc = 23;
    int textpos = 0; // where cursor is in text (body)
    int subjpos = 0, bodypos = 0; // Where subject and body in text ends
    int i = 0;
    int r = 0;
    int c = 0;

    char* subject = malloc(sizeof(char) * 31);
    char* body = malloc(sizeof(char) * 141);

    subject[0] = body[0] = '\0';

    display_editbox();
    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
    xt_par0(XT_CH_UNDERLINE);

    while (1) {  // all the switching logic
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_ENTER:
                ReadMystoreFromChild("add", subject, body, NULL);
            case KEY_F5:
                clean_up_editbox();
                return;
            case KEY_RIGHT:
                if (cursorpos) {
                    if (textpos >= bodypos)
                        break;
                }
                else if (textpos >= subjpos)
                    break;
                if (cursorc > 68 && cursorr == 11)
                    break;
                else if (cursorc > 69 && cursorr < 11) {
                    cursorc = 23;
                    cursorr++;
                    textpos++;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }
                else {
                    cursorc++;
                    textpos++;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
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
                i = 0;
                if (cursorc > 23) {
                    cursorc--;
                    textpos--;
                    if (cursorpos) {  // Body   @@@@@@@@@@@@@@@BROKENNNN
                        if (body[cursorc-22] == '\0')
                            body[cursorc-23] = '\0';
                        else {
                            i = cursorc-23;
                            r = cursorr;
                            c = cursorc;
                            while (body[i] != '\0' && i <= bodypos) { // <= is no error, but not sure if just < is enough
                                body[i] = body[i+1];
                                xt_par2(XT_SET_ROW_COL_POS, r, c++);
                                putchar(body[i+1]);
                                xt_par2(XT_SET_ROW_COL_POS, r, c);
                                i++;
                            }


                        }
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, 23 + --bodypos);
                        putchar(' ');
                    }


                    else {  // Subject
                        if (subject[cursorc-22] == '\0')
                            subject[cursorc-23] = '\0';
                        else {
                            i = cursorc-23;
                            r = cursorr;
                            c = cursorc;
                            while (subject[i] != '\0' && i <= subjpos) { // <= is no error, but not sure if just < is enough
                                subject[i] = subject[i+1];
                                xt_par2(XT_SET_ROW_COL_POS, r, c++);
                                putchar(subject[i+1]);
                                xt_par2(XT_SET_ROW_COL_POS, r, c);
                                i++;
                            }


                        }
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, 23 + --subjpos);
                        putchar(' ');

                    }

                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);

                }

                else if (cursorc == 23 && cursorr > 9) { // backspace triggers line jump
                    textpos--;
                    cursorc = 69;
                    cursorr--;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    putchar(' ');
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }

                break;



            default: 


                if (cursorpos == 1) { // body break, update
                    if (cursorc > 68 && cursorr == 11)
                        break;
                    if (textpos == 0 && (char)key == ' ')
                        break;
                    else if (bodypos == 140) 
                        break;
                    else if (cursorc > 69 && cursorr < 11) {
                        cursorc = 23;
                        cursorr++;
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                if (cursorpos == 0) {
                    if (cursorc > 52)
                        break;
                    if (subjpos == 30) 
                        break;
                    if (textpos == 0 && (char)key == ' ') 
                        break;
                }



                if (key >= ' ' && key <= '~') {
                    putchar((char) key);
                    cursorc++;
                    textpos++;
                    if (cursorpos) {  // Body @@@@@@@@@@@@@@BROOOOKENNNNNN
                        if (body[textpos-1] == '\0')
                            body[textpos] = '\0';
                        else {
                            i = bodypos + 1;
                            r = cursorr;
                            while (i >= textpos) { // dont worry about carrying '\0'
                                body[i] = body[i-1];
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                putchar(body[i]);
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                i--;
                            }
                            
                        }

                        body[textpos-1] = (char)key;
                        bodypos++;

                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                    else {  // Subject
                        if (subject[textpos-1] == '\0')
                            subject[textpos] = '\0';
                        else {
                            i = subjpos + 1;
                            r = cursorr;
                            while (i >= textpos) { // dont worry about carrying '\0'
                                subject[i] = subject[i-1];
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                putchar(subject[i]);
                                xt_par2(XT_SET_ROW_COL_POS, cursorr, i+23);
                                i--;
                            }
                            
                        }

                        subject[textpos-1] = (char)key;
                        subjpos++;

                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                break;
        }
    }
}

void display_body(char* text, int cursorr, int cursorc, int textpos) { // after backspace; cursorr and cursorc are locations of cursor AFTER backspace is pressed. textpos parallels.
    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
    int i;

    for (i = textpos; i < 141; i++) {
        if (text[i] == '\0') 
            break;
        else 
            text[i] = text[i+1];
    }



}

/* Display the edit box. */
void display_editbox(void) {
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

/* Clear the edit box and restore the previous state. */
void clean_up_editbox(void) {
    int offset;

    read_stat();
    selected = n_records - 1;
    offset = selected - ROWS + HEADER_OFFSET + 3;
    if (offset < 0)
        offset = 0;
    records = realloc(records, n_records * sizeof(struct Record));
    read_record(selected);

    reset();
    display_header();
    display_records(offset);
}

/* ----------------------------- MAIN FUNCTION ----------------------------- */

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
