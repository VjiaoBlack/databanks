#include <stdio.h>
#include "xterm/xterm_control.h"

void design() {
    printf(XT_CH_INVERSE);
    printf(" MYSTORE UI2 :: PBrooks :: Version 0.85                               5 Records \n");
    printf(XT_CH_NORMAL);
    printf("                                                      First: 2013-11-27 11:31:11\n");
    printf("[W/S] Scroll   [Enter] Edit   [N] New  [H] Help       Last:  2013-11-27 12:11:41\n");
    printf("\n");
    // start info here
}

int main() {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    design();
    xt_par2(XT_SET_ROW_COL_POS, 25, 1);
    return 0;
}
