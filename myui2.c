#include "myui2.h"

static struct Record* records;
static int n_records, selected, offset, min_shown, max_shown;
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

    selected = offset = 0;
    reset();
    display_header();
    display_records();
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
void display_records(void) {
    int i, row = HEADER_OFFSET;

    min_shown = 0;
    for (i = 0; i < n_records; i++) {
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

/* Shift the screen of displayed records up, incrementing offset. */
void shift_up(void) {
    int row = ROWS - 2;

    offset++;
    xt_par2(XT_SET_ROW_COL_POS, HEADER_OFFSET, 1);
    xt_par1(XT_DELETE_LINES, 1);
    min_shown++;
    display_record(++max_shown, &row);
}

/* Shift the screen of displayed records down, decrementing offset. */
void shift_down(void) {
    int row = HEADER_OFFSET;

    offset--;
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
    int row = HEADER_OFFSET + selected - offset - 1, start = row;

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
    int row = HEADER_OFFSET + selected - offset, start = row;

    xt_par2(XT_SET_ROW_COL_POS, row, 1);
    xt_par1(XT_DELETE_LINES, 2 + old->body_lines);
    xt_par1(XT_INSERT_LINES, 2 + new->body_lines);
    selected++;
    display_record(selected - 1, &row);
    display_record(selected, &row);
    if (start > MIN_SHIFT_DOWN_ROW && n_records > max_shown + 1)
        shift_up();
}

/* ----------------------------- MAIN FUNCTION ----------------------------- */

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
