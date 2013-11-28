#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct element_t {
	char *name;
	char *value;
};

typedef struct element_t element_t;



int main(int argc, char *argv[]);
void readstdin(char *);
int parseinput(char *, element_t *);




int main(int argc, char *argv[]) {
	
	char *data = calloc(300, sizeof(char));
	element_t *entries = calloc(400, sizeof(char)); // help, 400 is guestimating,
	readstdin(data);								// and although it should work
	int num_element_ts = parseinput(data, entries); // all the time, is there a better
	int i;				
				
	entries += num_element_ts;
	while (entries->name != NULL) {
		printf("%s=%s\n",  entries->name,  entries->value);
		entries--;
	}

	
}

void readstdin(char *input) {
	while (!feof(stdin))
		*(input++)=fgetc(stdin);
}

int parseinput(char *input, element_t *output) {
	int isreading = 0;
	int num_element_ts = 0;
	char *pname = calloc(300, sizeof(char));		// here too
	char *pvalue = calloc(300, sizeof(char));

	int namelen = 0;
	int valuelen = 0;

	// this should put the input into the output
	while (*input != EOF) {
		namelen = 0;
		valuelen = 0;
		if (*input == '|') {
			isreading = (isreading + 1) % 2;
		} else if (isreading) {
			num_element_ts++;
			while (*input != EOF) {
				*(pname++) = *(input++);
				namelen++;
				if (*input == ':')
					break;
			}
			*(pname++) = '\0';
			input += 2;
			while (*input != EOF) {
				*(pvalue++) = *(input++);
				valuelen++;
				if (*input == '|')
					break;
			}
			*(pvalue++) = '\0';

			*(output) = (element_t) {pname-namelen-1, pvalue-valuelen-1}; 
			

			output++;

			input--;
		}
		input++;

	}
	output--;
	return --num_element_ts;

}