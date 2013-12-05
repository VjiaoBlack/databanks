#include <stdio.h>
#include "xterm/xterm_control.h"

void design() {
    printf(XT_CH_INVERSE);
    printf(" MYSTORE UI2 :: PBrooks :: Version 0.85                               5 Records \n");
    printf(XT_CH_NORMAL);
    printf("                                                      First: 2013-11-27 11:31:11\n");
    printf("[w/s] Scroll   [Enter] Edit   [n] New  [h] Help       Last:  2013-11-27 12:11:41\n");
    printf("\n");
    printf("        7 Lorem ipsum dolor sit amet                                            \n");
    printf("        8 Ut enim ad minim veniam, qui                                          \n");
    printf("        9 Duis aute irure dolor in rep                                          \n");
    printf("    >  10 Amet sit dolor ipsum lorem                                            \n");
    printf("             -Ut enim ad minim veniam, quis nostrud exercitation ullamco        \n");
    printf("              laboris nisi ut aliquip ex ea commodo consequat. Duis aute        \n");
    printf("              irure dolor in reprehenderit in voluptate.                        \n");
    printf("       11 Ut enim ad minim veniam, qui                                          \n");
    printf("       12 Duis aute irure dolor in rep                                          \n");
    printf("       13 Ut enim ad minim veniam, qui      Enter: select                       \n");
    printf("       14 Duis aute irure dolor in rep      Up/w: previous entry                \n");
    printf("       15 Ut enim ad minim veniam, qui      Down/s: next entry                  \n");
    printf("       16 Duis aute irure dolor in rep      g: go to                            \n");
    printf("       17 Ut enim ad minim veniam, qui      f: find                             \n");
    printf("       18 Duis aute irure dolor in rep                                          \n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");


    printf(XT_CH_INVERSE);
    printf(" MYSTORE UI2 :: PBrooks :: Version 0.85                               5 Records \n");
    printf(XT_CH_NORMAL);
    printf("                                                      First: 2013-11-27 11:31:11\n");
    printf("[w/s] Scroll   [Enter] Edit   [n] New  [h] Help       Last:  2013-11-27 12:11:41\n");
    printf("\n");
    printf("        7 Lorem ipsum dolor sit amet                                            \n");
    printf("        8 Ut enim                                                               \n");
    printf("        9 Duis aut  +--------------------------------------+                    \n");
    printf("    >  10 Amet sit  | Amet sit dolor ipsum lorem           |                    \n");
    printf("             -Ut e  |                                      |  on ullamco        \n");
    printf("              labo  | Ut enim ad minim veniam, quis        |   Duis aute        \n");
    printf("              irur  | nostrud exercitation ullamco laboris |                    \n");
    printf("       11 Ut enim   | nisi ut aliquip ex ea commodo        |                    \n");
    printf("       12 Duis aut  | consequat. Duis aute irure dolor in  |                    \n");
    printf("       13 Ut enim   | reprehenderit in voluptate.          |                    \n");
    printf("       14 aute fac  |                                      |  ry                \n");
    printf("       15 Ut enim   | Press h for Help          esc: cancel|                    \n");
    printf("       16 Duis aut  +--------------------------------------+                    \n");
    printf("       17 Ut enim                                                               \n");
    printf("       18 Duis aute irure dolor in rep                                          \n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    // start info here
    printf(XT_CH_NORMAL);

    printf(XT_CH_INVERSE);
    printf(" MYSTORE UI2 :: PBrooks :: Version 0.85                               5 Records \n");
    printf(XT_CH_NORMAL);
    printf("                                                      First: 2013-11-27 11:31:11\n");
    printf("[w/s] Scroll   [Esc] Back                             Last:  2013-11-27 12:11:41\n");
    printf("\n");
    printf("        Welcome to the HelpMenu!!!                                              \n");
    printf("                                                                                \n");
    printf("        Helpmenustuff help help menu menumenu stuff help help menu menu         \n");
    printf("        menu menu stuff help help menuu menu stuffmenu helpmenu stuffmenu       \n");
    printf("                                                                                \n");
    printf("        Note: Pressing 'h' from the edit screen will bring u to the same        \n");
    printf("              menu, but just scroll it to the edit section.                     \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("                                                                                \n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    // start info here
    printf(XT_CH_NORMAL);
}

int main() {
    xt_par0(XT_CLEAR_SCREEN);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    design();
    return 0;
}

