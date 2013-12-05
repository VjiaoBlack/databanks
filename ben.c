#include <stdio.h>
#include "xterm/xterm_control.h"

#define KEY_COLOR XT_CH_GREEN, XT_CH_DEFAULT

void print_id(char* id, int highlight) {
    if (highlight)
        printf("%s->%s %2s ", XT_CH_RED, XT_CH_YELLOW, id);
    else
        printf("%s   %2s ", XT_CH_YELLOW, id);
    printf(XT_CH_DEFAULT);
}

void design() {
    printf(XT_CH_INVERSE);
    printf(" MYSTORE :: PBrooks :: Version 0.90                                   5 Records \n");
    printf(XT_CH_NORMAL);
    printf("%s[W/S]%s Scroll   %s[N]%s New   %s[F]%s Find   %s[G]%s Go To         First: 2013-11-04 20:36:55\n", KEY_COLOR, KEY_COLOR, KEY_COLOR, KEY_COLOR);
    printf("%s[Enter]%s Edit                        %s[H]%s Help           Last: 2013-11-04 21:10:10\n", KEY_COLOR, KEY_COLOR);
    printf("\n");
    printf(XT_CH_CYAN);
    printf("   ID SUBJECT                                                TIME               \n");
    printf(XT_CH_DEFAULT);
    print_id("1", 0); printf("Hello, world!                                          2013-11-04 20:36:55\n");
    print_id("2", 1); printf("Lorem ipsum dolor sit amet                             2013-11-04 20:43:04\n");
    printf("      Lorem ipsum dolor sit amet, consectetur adipisicing\n");
    printf("      elit, sed do eiusmod tempor incididunt ut labore et\n");
    printf("      dolore magna aliqua.\n");
    print_id("3", 0); printf("Another Entry                                          2013-11-04 20:46:25\n");
    print_id("4", 0); printf("A Fourth Entry                                         2013-11-04 20:46:29\n");
    print_id("5", 0); printf("Entry Five                                             2013-11-04 21:10:10\n");
}

int main() {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    design();
    xt_par2(XT_SET_ROW_COL_POS, 25, 1);
    return 0;
}
