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
    printf(XT_CH_INVERSE);
    printf(" MYSTORE :: PBrooks :: Version 0.90                                   5 Records \n");
    printf(XT_CH_NORMAL);
    printf("%s[W/S]%s Scroll   %s[N]%s New   %s[F]%s Find   %s[G]%s Go To         First: 2013-11-04 20:36:55\n", KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
    printf("%s[Enter]%s Edit             %s[H]%s Help   %s[Q]%s Quit           Last: 2013-11-04 21:10:10\n", KEY_COLOR, KEY_COLOR, KEY_COLOR);
    printf("\n");
    printf(XT_CH_CYAN);
    printf("   ID SUBJECT                                                TIME               \n");
    printf(XT_CH_DEFAULT);
}

/* Display all records. */
void display_records(void) {
    struct Record* record;
    int i, row = HEADER_OFFSET;

    for (i = 0; i < n_records; i++) {
        record = &records[i];
        xt_par2(XT_SET_ROW_COL_POS, row++, 1);
        print_id(record->id, i == selected);
        printf("%s", record->subject);
        xt_par1(XT_SET_COL_POS, COLS - 20);
        printf("%s", record->time);
        if (i == selected) {
            xt_par2(XT_SET_ROW_COL_POS, row++, BODY_OFFSET);
            printf("%s", record->body);
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
