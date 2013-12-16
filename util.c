#include "util.h"

static char input[1001];
static int n_input;

int ReadMystoreFromChild (char* argv1, char* argv2, char* argv3, char* argv4) {
	int pid, mypipe[2], i; // mypipe = {read,write} | i = counter
	char *newargv[7];

	if (nvs != NULL) { // makes sure nvs is deallocated and points to NULL
		free(nvs);
		nvs = NULL;
	}
	n_nvs = 0; // reset global variables
	n_input = 0;

	// getkey_terminate(); // this is problematic

	if (pipe(mypipe) == -1) {
		printf("Problem with pipe creation");
		exit(1);
	}

	pid = fork();

	if (pid == -1) { // error
		printf("Problem with forking");
		exit(1);
	}

	if (pid == 0) { // child
		close(mypipe[0]); // close reading end of pipe
		dup2(mypipe[1],STDOUT_FILENO); // connects "write-end" of parent's pipe to child's STDOUT

		for (i = 2; i < 7; ++i) {
			newargv[i] = NULL;
		}

		newargv[0] = "./mystore";
		newargv[1] = argv1;
		newargv[2] = argv2;
		newargv[3] = argv3;
		newargv[4] = argv4;
		newargv[5] = NULL;

		execvp(newargv[0], newargv); // child executes arguments obtained from method input
		exit(0);
	} else if (pid > 0) { // parent
		int c;//, i, n;
		close(mypipe[1]); // close the parent's write pipe

		// read data from read end of pipe into input array
		FILE *fpin;
		if ((fpin = fdopen(mypipe[0],"r")) == NULL) {
			printf("Parent cannot read from pipe\n");
			exit(1);
		}
		for (n_input = 0; n_input < sizeof(input) - 1; n_input++) {
			if ((c = getc(fpin)) == EOF) break;
			input[n_input] = c;
		}
		input[n_input++] = '\0'; //end string with nullbyte
	}

	return n_input;
}

int ParseInput() {
	int num_nvs, i_nvs; // number of pairs and a counter
	char *p;

	if (nvs != NULL) { // make sure the name value pair structs are cleared
		free(nvs);
	}
	nvs = NULL;
	n_nvs = 0;

	if (n_input < 7) // input is n_input
		return 0;

	for (num_nvs = 0, p = input; *p; p++) { // goes through array of p until *p = 0
		if (*p == '|')
			num_nvs++;
	}
	num_nvs /= 2; // gets number of pairs

	if ((nvs = calloc(num_nvs, sizeof(struct NameValue))) == NULL)
		return 0;

	for (i_nvs = 0, p = input; i_nvs < num_nvs; ++i_nvs) {
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
