#include "util.h"

extern struct NameValue *nvs; // pointer to namevalue pair structs
extern int n_nvs; // number of namevalue pair structs

extern char input[1000];
extern int n_input;

extern int nitems; //
extern char subject[31]; // subject (might need modification)
extern char body[141]; // body
extern char errmsg[80]; // error message

int ReadMystoreFromChild (char* argv1, char* argv2, char* argv3, char* argv4) {
	int pid, mypipe[2], i; // mypipe = {read,write} | i = counter
	char *newargv[7];

	if (nvs != NULL) { // makes sure nvs is deallocated and points to NULL
		free(nvs);
		nvs = NULL;
	}
	n_nvs = 0; // reset global variables
	n_input = 0;

	getkey_terminate(); // what does this do

	if (pipe(mypipe) == -1) {
		strcpy(errmsg,"Problem with pipe creation");
		return 0;
	}

	pid = fork();

	if (pid == -1) { // error
		strcpy(errmsg, "Problem with forking");
		return 0;
	}

	if (pid == 0) { // child
		close(mypipe[0]); // close reading end of pipe
		dup2(mypipe[1],STDOUT_FILENO); // connects "write-end" of parent's pipe to child's STDOUT

		for (i = 2; i < 7; ++i) {
			newargv[i] = NULL;
		}

		newargv[0] = newargv[1] = "./mystore"; // why do we need two?
		newargv[2] = argv1;
		newargv[3] = argv2;
		newargv[4] = argv3;
		newargv[5] = argv4;
		newargv[6] = NULL;

		execvp(newargv[0],newargv+1); // child executes arguments obtained from method input
		exit(0);
	} else if (pid > 0) { // parent
		char *s = input;
		int c;//, i, n;
		close(mypipe[1]); // close the parent's write pipe

		// read data from read end of pipe into input array
		FILE *fpin;
		if ((fpin = fdopen(mypipe[0],"r")) == NULL) {
			printf("Parent cannot read from pipe\n");
			exit(1);
		}
		for (n_input = 0; n_input < sizeof(input)-1; n_input++) {
			if ((c = getc(fpin)) == EOF) break;
			*s++ = c;
		}
		input[n_input] = '\0'; //end string with nullbyte
	}

	return n_input;
}

int ParseInput(char *in, int n_in) {
	int num_nvs, i_nvs; // number of pairs and a counter
	char *p;

	if (nvs != NULL) { // make sure the name value pair structs are cleared
		free(nvs);
	}
	nvs = NULL;
	n_nvs = 0;

	if (n_in < 7) // ??checks for authenticity??? input is n_input
		return 0;

	for (num_nvs = 0, p = in; *p; p++) { // goes through array of p until *p = 0
		if (*p == '|')
			num_nvs++;
	}
	num_nvs /= 2; // gets number of pairs

	if ((nvs = calloc(num_nvs, sizeof(struct NameValue))) == NULL) // ??checks for error??
		return 0;

	for (i_nvs = 0, p = in; i_nvs < num_nvs; ++i_nvs) {
		// until record
		while (*p++ != '|');

		// start of name
		nvs[i_nvs].name = p;
		while (*p != ':')
			p++;
		*p++ = '\0';

		// name finished, looking for value...
		while (*p == ' ')
			p++;
		nvs[i_nvs].value = p;
		while (*p != '|')
			p++;
		*p = '\0';
		// value finished
	}
	n_nvs = num_nvs;
	return n_nvs;
}
