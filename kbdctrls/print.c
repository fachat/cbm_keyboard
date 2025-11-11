
#include <stdio.h>

#include "kbdhw.h"

typedef struct {
	char c;
	unsigned char k;
} keyinfo_t;

keyinfo_t ktab[] = {
	{ 'a', 32 },
	{ 'b', 50 },
	{ 'c', 49 },
	{ 'd', 33 },
	{ 'e', 17 },
	{ 'f', 41 },
	{ 'g', 34 },
	{ 'h', 42 },
	{ 'i', 27 },
	{ 'j', 35 },
	{ 'k', 43 },
	{ 'l', 36 },
	{ 'm', 51 },
	{ 'n', 58 },
	{ 'o', 20 },
	{ 'p', 28 },
	{ 'q', 16 },
	{ 'r', 25 },
	{ 's', 40 },
	{ 't', 18 },
	{ 'u', 19 },
	{ 'v', 57 },
	{ 'w', 24 },
	{ 'x', 56 },
	{ 'y', 26 },
	{ 'z', 48 },
	{ 0, 0 },
};

static char *txtp = NULL;
static int pos = 0;
static int cnt = 0;
static int trigger = 0;

void print_setup() {
	txtp = NULL;
	pos = 0;
	cnt = 0;
}

void print_set(char *text) {

	txtp = text;
	pos = 0;
	cnt = 0;
}

static unsigned char print_char(char c) {
	// find the actual key index from the (ascii) character
	for (int i = 0; ktab[i].c; i++) {
		if (ktab[i].c == c) {
			return ktab[i].k;
		}
	}
	return -1;
}

void print_advance() {

        // check RES                                 
        if (scanvals[9] & 0x20) {        // CTRL key  
		trigger = 1;
        } else {
		if (trigger) {
			trigger = 0;
			print_set("hello world");
		}
        }                                            

	if (txtp) {
		if (txtp[pos] == 0) {
			txtp = NULL;
			return;
		}

		if (cnt < 7) {
			set_key_down(print_char(txtp[pos]));
			//set_key_down(27+pos);
		}

		cnt++;
		if (cnt > 20) {
			cnt = 0;
			pos++;
		}
	}
}

