#include <stdio.h>
#include "xterm/xterm_control.h"

#define ROWS 24
#define COLS 80

#define KEY_COLOR XT_CH_GREEN, XT_CH_DEFAULT

#define PREPARE() xt_par2(XT_SET_ROW_COL_POS, 25, 1); printf(" \n \n \n "); xt_par0(XT_CLEAR_SCREEN); xt_par2(XT_SET_ROW_COL_POS, 1, 1);

void print_id(char* id, int highlight) {
    if (highlight)
        printf("%s->%s %2s ", XT_CH_RED, XT_CH_YELLOW, id);
    else
        printf("%s   %2s ", XT_CH_YELLOW, id);
    printf(XT_CH_DEFAULT);
}

void ex1_plain() {
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

void ex2_scrolling() {
    printf(XT_CH_INVERSE);
    printf(" MYSTORE :: PBrooks :: Version 0.90                                  25 Records \n");
    printf(XT_CH_NORMAL);
    printf("%s[W/S]%s Scroll   %s[N]%s New   %s[F]%s Find   %s[G]%s Go To         First: 2013-11-04 22:00:00\n", KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
    printf("%s[Enter]%s Edit                        %s[H]%s Help           Last: 2013-11-04 22:00:00\n", KEY_COLOR, KEY_COLOR);
    printf("\n");
    printf(XT_CH_CYAN);
    printf("   ID SUBJECT                                                TIME               \n");
    printf(XT_CH_DEFAULT);
    print_id("9", 0);  printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("10", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("11", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("12", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("13", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("14", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("15", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("16", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("17", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("18", 1); printf("Entry                                                  2013-11-04 22:00:00\n");
    printf("        This entry is boring. There's nothing in it. Why\n");
    printf("        would anyone want to read this entry?\n");
    print_id("19", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("20", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("21", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("22", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("23", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("24", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
    print_id("25", 0); printf("Entry                                                  2013-11-04 22:00:00\n");
}

void ex3_new() {
    printf(XT_CH_INVERSE);
    printf(" MYSTORE :: PBrooks :: Version 0.90                                   5 Records \n");
    printf(XT_CH_NORMAL);
    printf("[W/S] Scroll   [N] New   [F] Find   [G] Go To         First: 2013-11-04 20:36:55\n");
    printf("[Enter] Edit             [H] Help   [Q] Quit           Last: 2013-11-04 21:10:10\n");
    printf("\n");
    printf("   ID SUBJECT                                                TIME               \n");
    printf("    1 Hello, world!                                          2013-11-04 20:36:55\n");
    printf("->  2 Lorem ipsum dolor sit amet                             2013-11-04 20:43:04\n");
    printf("        Lorem ipsum dolor sit amet, consectetur adipisicing\n");
    printf("        elit, sed do eiusmod tempor incididunt ut labore et\n");
    printf("        dolore magna aliqua.\n");
    printf("    3 Another Entry                                          2013-11-04 20:46:25\n");
    printf("    4 A Fourth Entry                                         2013-11-04 20:46:29\n");
    printf("    5 Entry Five                                             2013-11-04 21:10:10\n");
    xt_par2(XT_SET_ROW_COL_POS, 6, 11);
    printf(XT_CH_INVERSE);
    printf("                       CREATE NEW ENTRY                       \n");
    xt_par2(XT_SET_ROW_COL_POS, 7, 11);
    printf(" %s                                                            %s \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 8, 11);
    printf(" %s  %sSubject:%s %sEntry About%s%s %s%s              %s                       %s \n", XT_CH_NORMAL, XT_CH_YELLOW, XT_CH_DEFAULT, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 9, 11);
    printf(" %s   %sBody:%s   %s                                               %s  %s \n", XT_CH_NORMAL, XT_CH_YELLOW, XT_CH_DEFAULT, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 10, 11);
    printf(" %s           %s                                               %s  %s \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 11, 11);
    printf(" %s           %s                                              %s   %s \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 12, 11);
    printf(" %s                                                            %s \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 13, 11);
    printf(" %s     %s[Arrow Keys]%s Navigate   %s[Enter]%s Save   %s[F5]%s Cancel     %s \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 14, 11);
    printf("                                                              \n");
    printf(XT_CH_NORMAL);
}

void ex4_edit() {
    printf(XT_CH_INVERSE);
    printf(" MYSTORE :: PBrooks :: Version 0.90                                   5 Records \n");
    printf(XT_CH_NORMAL);
    printf("[W/S] Scroll   [N] New   [F] Find   [G] Go To         First: 2013-11-04 20:36:55\n");
    printf("[Enter] Edit             [H] Help   [Q] Quit           Last: 2013-11-04 21:10:10\n");
    printf("\n");
    printf("   ID SUBJECT                                                TIME               \n");
    printf("    1 Hello, world!                                          2013-11-04 20:36:55\n");
    printf("->  2 Lorem ipsum dolor sit amet                             2013-11-04 20:43:04\n");
    printf("        Lorem ipsum dolor sit amet, consectetur adipisicing\n");
    printf("        elit, sed do eiusmod tempor incididunt ut labore et\n");
    printf("        dolore magna aliqua.\n");
    printf("    3 Another Entry                                          2013-11-04 20:46:25\n");
    printf("    4 A Fourth Entry                                         2013-11-04 20:46:29\n");
    printf("    5 Entry Five                                             2013-11-04 21:10:10\n");
    xt_par2(XT_SET_ROW_COL_POS, 6, 11);
    printf(XT_CH_INVERSE);
    printf("                          EDIT ENTRY                          \n");
    xt_par2(XT_SET_ROW_COL_POS, 7, 11);
    printf(" %s                                                            %s \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 8, 11);
    printf(" %s  %sSubject:%s %sLorem ipsum %s%sd%s%solor sit amet               %s        %s \n", XT_CH_NORMAL, XT_CH_YELLOW, XT_CH_DEFAULT, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 9, 11);
    printf(" %s   %sBody:%s   %sLorem ipsum dolor sit amet, consectetur adipis%s   %s \n", XT_CH_NORMAL, XT_CH_YELLOW, XT_CH_DEFAULT, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 10, 11);
    printf(" %s           %sicing elit, sed do eiusmod tempor incididunt u%s   %s \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 11, 11);
    printf(" %s           %st labore et dolore magna aliqua.              %s   %s \n", XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 12, 11);
    printf(" %s                                                            %s \n", XT_CH_NORMAL, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 13, 11);
    printf(" %s     %s[Arrow Keys]%s Navigate   %s[Enter]%s Save   %s[F5]%s Cancel     %s \n", XT_CH_NORMAL, KEY_COLOR, KEY_COLOR, KEY_COLOR, XT_CH_INVERSE);
    xt_par2(XT_SET_ROW_COL_POS, 14, 11);
    printf("                                                              \n");
    printf(XT_CH_NORMAL);
}
int main() {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    ex1_plain();
    PREPARE()
    ex2_scrolling();
    PREPARE()
    ex3_new();
    PREPARE()
    ex4_edit();
    // ex5_find();
    // ex6_goto();
    // ex7_help();
    xt_par2(XT_SET_ROW_COL_POS, 25, 1);
    return 0;
}
