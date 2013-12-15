#include "myui2.h"

struct NameValue *nvs = NULL;
int n_nvs = 0;

char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte

struct TemplateString TS[] = {
	{1,1,XT_CH_YELLOW,"-------------------------- Simple Mystore Front End  -------------------------"},
	{3,34,XT_CH_YELLOW,"F9: exit"},
	{4,1,XT_CH_CYAN,"Message:"},
	{6,1,XT_CH_YELLOW,"--------------------------------- Status ------------------------------------"},
	{7,1,XT_CH_CYAN,"Number of records:"},
	{8,1,XT_CH_CYAN,"First record time:"},
	{9,1,XT_CH_CYAN,"Last  record time:"},
	{10,1,XT_CH_CYAN,"Author:"},
	{11,1,XT_CH_CYAN,"Version:"},
	{13,1,XT_CH_YELLOW,"-- DISPLAY keys: up-previous  down-next Home-first End-last  F6-DeleteRecord  -"},
	{14,1,XT_CH_CYAN,"Record #:"},
	{15,1,XT_CH_CYAN,"Time:"},
	{16,1,XT_CH_CYAN,"Subject:"},
	{17,1,XT_CH_CYAN,"Body:"},
	{20,1,XT_CH_YELLOW,"-  EDIT keys:  backspace delete Enter-Subject<->Body  F3-Clear F1-AddRecord  --"},
	{21,1,XT_CH_CYAN,"Subject: |"},
	{21,41,XT_CH_CYAN,"|"},
	{22,1,XT_CH_CYAN,"Body:    |"},
	{23,62,XT_CH_CYAN,"|"}
};
int nTS = sizeof(TS)/sizeof(TS[0]);

struct StringPosition SP[] = {
	{4,10,70,"message"},
	{7,20,3,"nitems"},
	{8,20,20,"first-record"},
	{9,20,20,"last-record"},
	{10,9,30,"author"},
	{11,10,10,"version"},
	{14,11,3,"record-num"},
	{15,7,20,"time"},
	{16,10,30,"subject"},
	{17,7,140,"body"},
	{21,11,30,"edit-subject"},
	{22,11,140,"edit-body"}
};
int nSP = sizeof(SP)/sizeof(SP[0]);

int nitems = 0;
char subject[31];
char body[141];
char errmsg[80] = "";


// ------------------------------------------------ main --------------------
int main(void) {
	int i, c;
	
	fill(subject,30);
	fill(body,140);
	
	xt_par0(XT_CLEAR_SCREEN);
	
	
	// display template
 	for (i = 0; i < nTS; ++i) {
		xt_par2(XT_SET_ROW_COL_POS,TS[i].row,TS[i].col);
		xt_par0(XT_CH_DEFAULT);
		xt_par0(TS[i].color);
		printf("%s",TS[i].string);
	}
	
	DisplayStat();
	while (TRUE) {
		while ((c = getkey()) == KEY_NOTHING) ;
		if (c == KEY_F9)  {
			xt_par0(XT_CLEAR_SCREEN);
			xt_par0(XT_CH_NORMAL);
			xt_par2(XT_SET_ROW_COL_POS,1,1);
			getkey_terminate();
			exit(0);
		}
	}
	
}

// ------------------------------------ fill --------------------------------
void fill(char *s, int n) {
	while (n--) *s++=' ';
	*s='\0';
}

// ------------------------------------------------ DisplayStat --------------
void DisplayStat(void) {
	ReadMystoreFromChild("stat",NULL,NULL,NULL);
	ParseInput(input,n_input);
	SearchDisplay("nitems","nitems",XT_CH_WHITE);
	SearchDisplay("first-record","first-time",XT_CH_WHITE);
	SearchDisplay("last-record","last-time",XT_CH_WHITE);
	SearchDisplay("author","author",XT_CH_WHITE);
	SearchDisplay("version","version",XT_CH_WHITE);
	fflush(stdout);
	//SearchDisplay("message",errmsg,XT_CH_RED);
}

// --------------------------- Display -----------------------------------
void SearchDisplay(char *prompt, char *name, char *color) {
	int loc, col, i, j;
	int instring = TRUE;
	char *value;
	
	// search for location
	loc = FindStringPosition(prompt);
	col = SP[loc].col;
	
	// search for value
	value = "";
	for (j = 0; j < n_nvs; ++j) {
		if (strcmp(nvs[j].name,name) == 0) {
			value = nvs[j].value;
			break;
		}
	}
	
	DisplayAt(SP[loc].row,SP[loc].col,XT_CH_WHITE,SP[loc].length,value);
}

// ------------------------------------- DisplayAt -------------------------
void DisplayAt(int row, int col, char *color, int maxlength, char *value) {
	int i, instring = TRUE;
	
	xt_par2(XT_SET_ROW_COL_POS,row,col);
	xt_par0(color);
	for (i = 0; i < maxlength; ++i) {
		if (value[i] == '\0') 
			instring = FALSE;
		printf("%c",instring?value[i]:' ');
		if (++col == 80) {
			xt_par2(XT_SET_ROW_COL_POS,row+1,1);
			col = 1;
		}
	}
	fflush(stdout);
}

// ---------------------------------- 	FindStringPosition ----------------
int FindStringPosition(char *prompt) {
	int i;
	
	for (i = 0; i < nSP; ++i) {
		if (strcmp(prompt,SP[i].name) == 0)
			return i;
	}
	return 0;
}
