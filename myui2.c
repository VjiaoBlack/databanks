#include "myui2.h"

static struct Record* records;
static int grayscale = 0, n_records, selected, min_shown, max_shown;
static char *version = NULL, *author = NULL, *first_time = NULL,
            *last_time = NULL;

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
            case 'g':
                goto_entry();
                break;
            case 'f':
                // TODO
                break;
            case 'n':
                new_entry();
                break;
            case KEY_ENTER:
                edit_entry();
                break;
            case KEY_DELETE:
            case KEY_BACKSPACE:
                if (n_records > 0) 
                    delete_entry();
                break;
            case 'h':
                help_menu();
                break;
            case 'q':
                return;  // Exit the loop and run finish()
        }
    }
}

/* Reset the keyboard and screen. */
void finish(void) {
    int i;
    reset();
    getkey_terminate();

    free(version);
    free(author);
    if (first_time)
        free(first_time);
    if (last_time)
        free(last_time);
    for (i = 0; i < n_records; i++)
        dealloc_record(records[i]);
    free(records);
}

/* ----------------------- DATA RETRIEVAL FUNCTIONS ------------------------ */

/* Copy source into what dest points to, mallocing enough space for dest. */
void malloc_then_copy(char** dest, char* source) {
    if (*dest)
        free(*dest);
    *dest = malloc((strlen(source) + 1) * sizeof(char));
    strcpy(*dest, source);
}

void dealloc_record(struct Record record) {
    free(record.time);
    free(record.subject);
    free(record.body);
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
    else {
        if (first_time)
            free(first_time);
        if (last_time)
            free(last_time);
        first_time = last_time = NULL;
    }
}

/* Read the output of "mystore display" for the given record into records. */
void read_record(int id) {
    struct Record record = {id + 1, NULL, NULL, NULL, 0};
    char str_id[6];  // Basically the max number of digits in an record ID

    sprintf(str_id, "%d", id + 1);
    ReadMystoreFromChild("display", str_id, NULL, NULL);
    ParseInput();

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
    char* red_color = grayscale ? "" : XT_CH_RED;
    char* yellow_color = grayscale ? "" : XT_CH_YELLOW;

    if (highlight)
        printf("%s->%s %2d ", red_color, yellow_color, id);
    else
        printf("%s   %2d ", yellow_color, id);
    xt_par0(XT_CH_DEFAULT);
}

/* Display the header with the version, record count, times, and help. */
#define KEY_COLOR_2 green_color, XT_CH_DEFAULT
void display_header(void) {
    char* green_color = grayscale ? "" : XT_CH_GREEN;
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
           KEY_COLOR_2, KEY_COLOR_2, KEY_COLOR_2, KEY_COLOR_2);
    if (first_time != NULL) {
        xt_par2(XT_SET_ROW_COL_POS, 2, COLS - FIRST_TIME_OFFSET);
        printf("First: %s", first_time);
    }
    xt_par2(XT_SET_ROW_COL_POS, 3, 1);
    printf("%s[Enter]%s Edit  %s[Del]%s Delete  %s[H]%s Help  %s[Q]%s Quit",
           KEY_COLOR_2, KEY_COLOR_2, KEY_COLOR_2, KEY_COLOR_2);
    if (last_time != NULL) {
        xt_par2(XT_SET_ROW_COL_POS, 3, COLS - LAST_TIME_OFFSET);
        printf("Last: %s", last_time);
    }

    // Draw the fifth line (table header)
    xt_par2(XT_SET_ROW_COL_POS, 5, 1);
    if (!grayscale)
        xt_par0(XT_CH_CYAN);
    printf("   ID SUBJECT");
    xt_par2(XT_SET_ROW_COL_POS, 5, COLS - TIME_OFFSET);
    printf("TIME");
    xt_par0(XT_CH_DEFAULT);
}

/* Display all records. */
void display_records(int start) {
    int i, row = HEADER_OFFSET, margin = 3 - records[selected].body_lines;

    min_shown = start;
    for (i = start; i < n_records; i++) {
        display_record(i, &row);
        if (row > ROWS - margin)
            break;
    }
    max_shown = i;
}

/* Display a certain record by ID at the given row. Increases the row. */
void display_record(int id, int* row) {
    struct Record* record = &records[id];
    int i;

    xt_par2(XT_SET_ROW_COL_POS, *row, 1);
    if (id == selected)
        xt_par0(XT_CH_BOLD);
    print_id(record->id, id == selected);
    printf("%s", record->subject);
    xt_par2(XT_SET_ROW_COL_POS, (*row)++, COLS - TIME_OFFSET);
    printf("%s", record->time);
    if (id == selected) {
        xt_par0(XT_CH_NORMAL);
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

void display_record_to_edit(int subjpos, int bodypos, char* subject, char* body){
    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
    xt_par0(XT_CH_UNDERLINE);
    int textpos = 0;
    while (textpos < 31) { // subject 
        if (textpos > subjpos) 
            putchar(' ');
        else {
            putchar(subject[textpos]);
        }
        textpos++;
    }

    textpos = 0;
    xt_par2(XT_SET_ROW_COL_POS, 9, 23);
    while (textpos <= 46) {// i'm  printing out the data
        if (textpos == bodypos) 
            putchar(' ');
        else {
            putchar(body[textpos]);
        }
        textpos++;
    }

    xt_par2(XT_SET_ROW_COL_POS, 10, 23);
    while (textpos <= 93) {// i'm  printing out the data
        if (textpos == bodypos ) 
            putchar(' ');
        else {
            putchar(body[textpos]);
        }
        textpos++;
    }

    xt_par2(XT_SET_ROW_COL_POS, 11, 23);
    while (textpos <= 139) {// i'm  printing out the data
        if (textpos == bodypos ) //how to stop last char from being space, and from sticky chars at end of line

            putchar(' ');
        else {
            putchar(body[textpos]);
        }
        textpos++;
    }

    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
    textpos = 0;
}

/* ---------------------------- GOTO ENTRY CODE ---------------------------- */

void goto_entry(void) {
    int entry, offset;

    if (n_records == 0)
        return;
    if ((entry = display_gotobox()) < 0) {  // User canceled
        reset();
        display_header();
        display_records(min_shown);
        return;
    }
    selected = entry < n_records ? entry : n_records - 1;
    offset = selected - ROWS + HEADER_OFFSET + 3;
    if (offset < 0)
        offset = 0;
    reset();
    display_header();
    display_records(offset);
}

int display_gotobox(void) {
    int key, cursorpos = 0, cursorr, cursorc, i;
    char input[7] = {0, 0, 0, 0, 0, 0, 0};

    grayscale = 1;
    reset();
    display_header();
    display_records(min_shown);
    grayscale = 0;
    xt_par2(XT_SET_ROW_COL_POS, 7, 10);
    printf(XT_CH_INVERSE);
    printf("                         GO TO ENTRY                          \n");
    xt_par2(XT_SET_ROW_COL_POS, 8, 10);
    printf("  %s                                                          %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 9, 10);
    printf("  %s                     Entry ID: %s      %s                     %s  \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 10, 10);
    printf("  %s               %s[Enter]%s Go To   %s[F5]%s Cancel                %s  \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 11, 10);
    printf("  %s                                                          %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 12, 10);
    printf("                                                              \n");
    printf(XT_CH_NORMAL);
    xt_par2(XT_SET_ROW_COL_POS, cursorr = 9, cursorc = 43);
    printf(XT_CH_UNDERLINE);

    while (1) {
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_ENTER:
                return atoi(input) - 1;
            case KEY_F5:
            case 'q':
                return -1;
            case KEY_RIGHT:
                if (input[cursorpos] != '\0') {
                    cursorpos++;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, ++cursorc);
                }
                break;
            case KEY_LEFT:
                if (cursorpos > 0) {
                    cursorpos--;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, --cursorc);
                }
                break;
            case KEY_BACKSPACE:
            case KEY_DELETE:
                if ((key == KEY_BACKSPACE && cursorpos == 0) ||
                    (key == KEY_DELETE && input[cursorpos] == '\0'))
                    break;
                i = key == KEY_BACKSPACE ? cursorpos - 1 : cursorpos;
                while (input[i] != '\0') {
                    input[i] = input[i + 1];
                    i++;
                }
                if (key == KEY_BACKSPACE)
                    cursorpos--;
                xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc = 43);
                printf("%-6s", input);
                xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc += cursorpos);
                break;
            default:
                if (key >= '0' && key <= '9' && cursorpos < 6 && input[5] == '\0') {
                    for (i = 5; i > cursorpos; i--)
                        input[i] = input[i - 1];
                    input[cursorpos++] = key;
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc = 43);
                    printf("%-6s", input);
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc += cursorpos);
                }
        }
    }
}

/* ---------------------------- NEW ENTRY CODE ----------------------------- */

void new_entry(void) {
    char subject[31] = "\0";
    char body[141] = "\0";

    display_editbox();
    if (editbox_input(subject, body)) {
        ReadMystoreFromChild("add", subject, body, NULL);
        clean_up_editbox(1);
    }
    else
        clean_up_editbox(0);
}

int editbox_input(char* subject, char* body) {
    int key, cursorpos = 0;
    int cursorr = 8, cursorc = 23;
    int textpos = 0; // where cursor is in current piece of text (either subj or body)
    int subjpos = strlen(subject), bodypos = strlen(body); // Where subject and body in text ends
    int i = 0;
    int warningtriggered = 0;
    xt_par2(XT_SET_ROW_COL_POS, cursorr = 8, cursorc = 23);
    xt_par0(XT_CH_UNDERLINE);

    display_record_to_edit(subjpos, bodypos, subject, body);

    while (1) {  // all the switching logic
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_ENTER:
                if (strlen(subject) == 0 && strlen(body) == 0) {
                    warningtriggered = 1;
                    xt_par2(XT_SET_ROW_COL_POS, 7, 20);
                    printf(XT_CH_NORMAL);
                    printf(XT_CH_RED);
                    printf(XT_CH_BOLD);
                    printf("COMPLETELY BLANK ENTRIES NOT ALLOWED");
                    printf(XT_CH_NORMAL);
                    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
                    break;
                }
                return 1;
            case KEY_F5:
                return 0;
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
                if (cursorpos == 0) {
                    xt_par2(XT_SET_ROW_COL_POS, 9, 23);
                    cursorpos = 1;
                    cursorr = 9;
                    cursorc = 23;
                    textpos = 0;
                }
                break;
            case KEY_UP:
                if (cursorpos == 1) {
                    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
                    cursorpos = 0;
                    cursorr = 8;
                    cursorc = 23;
                    textpos = 0;
                }
                break;
            case KEY_DELETE:
                // Move cursor to the right 1 space
                if (cursorpos) { // body
                    if (textpos == bodypos){
                        break;
                    }
                } else { // subject
                    if (textpos == subjpos){
                        break;
                    }
                }
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

            case KEY_BACKSPACE:
                i = 0;
                if (cursorc >= 23) {
                    if (cursorc == 23 && cursorr > 9) { // backspace triggers line jump
                        textpos--;
                        cursorc = 69;
                        cursorr--;
                    } else if (cursorc == 23 && cursorr <= 9) {
                        break;
                    } else { // default backspace.
                        cursorc--;
                        textpos--;
                    }
                    if (cursorpos) {  // Body
                        if (body[cursorc-22] == '\0')
                            body[cursorc-23] = '\0';
                        else {
                            i = textpos;
                            while (body[i] != '\0' && i <= bodypos) { 
                                body[i] = body[i+1];
                                i++;
                            }
                        }
                        --bodypos;
                    } else {  // Subject
                        if (subject[cursorc-22] == '\0')
                            subject[cursorc-23] = '\0';
                        else {
                            i = cursorc-23;
                            while (subject[i] != '\0' && i <= subjpos) { // <= is no error, but not sure if just < is enough
                                subject[i] = subject[i+1];
                                i++;
                            }
                        }
                        --subjpos;
                    }
                    xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                }
                display_record_to_edit(subjpos, bodypos, subject, body);
                xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                break;



            default: 
                if (warningtriggered) {
                    xt_par2(XT_SET_ROW_COL_POS, 7, 20);
                    printf(XT_CH_NORMAL);
                    printf("                                        ");
                    printf(XT_CH_UNDERLINE);
                    xt_par2(XT_SET_ROW_COL_POS, 8, 23);
                    warningtriggered = 0;
                }
 
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
                    if (cursorpos) {  // Body 
                        if (body[textpos-1] == '\0')
                            body[textpos] = '\0';
                        else {
                            i = bodypos + 1;
                            while (i >= textpos) { // dont worry about carrying '\0'
                                body[i] = body[i-1];
                                i--;
                            }   
                        }
                        body[textpos-1] = (char)key;
                        bodypos++;
                        display_record_to_edit(subjpos, bodypos, subject, body);
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                    else {  // Subject
                        if (subject[textpos-1] == '\0')
                            subject[textpos] = '\0';
                        else {
                            i = subjpos + 1;
                            while (i >= textpos) { // dont worry about carrying '\0'
                                subject[i] = subject[i-1];
                                i--;
                            } 
                        }
                        subject[textpos-1] = (char)key;
                        subjpos++;
                        display_record_to_edit(subjpos, bodypos, subject, body);
                        xt_par2(XT_SET_ROW_COL_POS, cursorr, cursorc);
                    }
                }
                break;
        }
    }
}

/* ---------------------------- EDIT ENTRY CODE ---------------------------- */

void edit_entry(void) {
    char str_id[6];  // Basically the max number of digits in an record ID


    char* subject = malloc(sizeof(char) * 31);
    char* body = malloc(sizeof(char) * 141);

    strcpy(subject, records[selected].subject);
    strcpy(body, records[selected].body);

    display_editbox();
    if (editbox_input(subject, body)) {
        sprintf(str_id, "%d", selected + 1);
        ReadMystoreFromChild("edit", str_id, subject, body);
        clean_up_editbox(0);
    }
    else
        clean_up_editbox(0);
}

/* Display the edit box. */
void display_editbox(void) {
    grayscale = 1;
    reset();
    display_header();
    display_records(min_shown);
    grayscale = 0;
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
void clean_up_editbox(int update_new) {
    int offset;

    //if (update_new) {
        read_stat();
    //    selected = n_records - 1;

        if (update_new == 1) { //if is new entry
            selected = n_records - 1;
        }

        offset = selected - ROWS + HEADER_OFFSET + 3;
        if (offset < 0)
            offset = 0;
        records = realloc(records, n_records * sizeof(struct Record));
        read_record(selected);
    //}
    //else
    //    offset = min_shown;
    reset();
    display_header();
    display_records(offset);
}

/* --------------------------- DELETE ENTRY CODE --------------------------- */

void delete_entry(void) {
    int do_it, i;
    char str_id[6];  // Basically the max number of digits in an record ID

    if (selected < n_records) {
        do_it = display_deletebox();
        if (do_it) {
            sprintf(str_id, "%d", selected + 1);
            ReadMystoreFromChild("delete", str_id, NULL, NULL);
            dealloc_record(records[selected]);
            read_stat();
            for (i = selected; i < n_records; i++) {
                records[i] = records[i + 1];
                records[i].id--;
            }
            if (selected == n_records && selected > 0) {
                selected--;
                if (min_shown > 0)
                    min_shown--;
            }
        }
        reset();
        display_header();
        display_records(min_shown);
    }
}

int display_deletebox(void) {
    int key;

    grayscale = 1;
    reset();
    display_header();
    display_records(min_shown);
    grayscale = 0;
    xt_par2(XT_SET_ROW_COL_POS, 7, 10);
    printf(XT_CH_INVERSE);
    printf("                        DELETE ENTRY?                         \n");
    xt_par2(XT_SET_ROW_COL_POS, 8, 10);
    printf("  %s                                                          %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 9, 10);
    printf("  %s       Are you sure you want to delete this entry?        %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 10, 10);
    printf("  %s             %s[Enter]%s Delete   %s[Other]%s Cancel              %s  \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 11, 10);
    printf("  %s                                                          %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 12, 10);
    printf("                                                              \n");
    printf(XT_CH_NORMAL);

    while ((key = getkey()) == KEY_NOTHING);
    return key == KEY_ENTER;
}

/* ----------------------------- HELP MENU CODE ---------------------------- */
void help_menu(void) {
    display_helpmenu(1);    
    helpmenu_run();
    clean_up_helpmenu();
}

/* Display the help menu. */
void display_helpmenu(int page) {
    grayscale = 1;
    reset();
    display_header();
    display_records(min_shown);
    grayscale = 0;


    if (page == 1) {
        xt_par2(XT_SET_ROW_COL_POS, 6, 9);
        printf(XT_CH_INVERSE);
        printf("                           HELP MENU                            \n");
        xt_par2(XT_SET_ROW_COL_POS, 7, 9);
        printf("  %s         _|              _|                                 %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 8, 9);
        printf("  %s     _|_|_|    _|_|_|  _|_|_|_|    _|_|_|   Ben Kurtovic    %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 9, 9);
        printf("  %s   _|    _|  _|    _|    _|      _|    _|      and          %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 10, 9);
        printf("  %s   _|    _|  _|    _|    _|      _|    _|   Victor Jiao     %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 11, 9);
        printf("  %s     _|_|_|    _|_|_|      _|_|    _|_|_|                   %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 12, 9);
        printf("  %s        _|                            _|                    %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 13, 9);
        printf("  %s        _|_|_|      _|_|_|  _|_|_|    _|  _|      _|_|_|    %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 14, 9);
        printf("  %s        _|    _|  _|    _|  _|    _|  _|_|      _|_|        %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 15, 9);
        printf("  %s        _|    _|  _|    _|  _|    _|  _|  _|        _|_|    %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 16, 9);
        printf("  %s        _|_|_|      _|_|_|  _|    _|  _|    _|  _|_|_|      %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 17, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 18, 9);
        printf("  %s     %s[Arrow Keys]%s Navigate Pages            %s[F5]%s Back       %s  \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 19, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 20, 9);
        printf("                            PAGE %d/2                            \n", page);
        printf(XT_CH_NORMAL);

    } else if (page == 2) {
        xt_par2(XT_SET_ROW_COL_POS, 6, 9);
        printf(XT_CH_INVERSE);
        printf("                           HELP MENU                            \n");
        xt_par2(XT_SET_ROW_COL_POS, 7, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 8, 9);
        printf("  %s * Please note that sometimes, you must hold down the 'fn'  %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 9, 9);
        printf("  %s   key to appropriately use the function (F5) keys. Refer   %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 10, 9);
        printf("  %s   to your keyboard manual or check online for more help.   %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 11, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 12, 9);
        printf("  %s * In addition, to navigate through font in the body of an  %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 13, 9);
        printf("  %s   entry, you may only use the left and right arrow keys.   %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 14, 9);
        printf("  %s   This is to help prevent excessive, unintentional edits.  %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 15, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 16, 9);
        printf("  %s * We hope you enjoy using databanks!                       %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 17, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 18, 9);
        printf("  %s     %s[Arrow Keys]%s Navigate Pages            %s[F5]%s Back       %s  \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 19, 9);
        printf("  %s                                                            %s  \n", XT_CH_NORMAL, XT_CH_INVERSE);
        xt_par2(XT_SET_ROW_COL_POS, 20, 9);
        printf("                            PAGE %d/2                            \n", page);
        printf(XT_CH_NORMAL);
    }

}

void helpmenu_run(void) {
    int page = 1;
    int key;

    while (1) {  // all the switching logic
        while ((key = getkey()) == KEY_NOTHING);
        switch (key) {
            case KEY_F5:
                return;
            case KEY_RIGHT:
                if (page < 2) {
                    page++;
                    display_helpmenu(page);
                }
                break;
            case KEY_LEFT:
                if (page > 1) {
                    page--;
                    display_helpmenu(page);
                }
                break;
        }
    }

}

/* Clear the edit box and restore the previous state. */
void clean_up_helpmenu() {
    int offset;
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
