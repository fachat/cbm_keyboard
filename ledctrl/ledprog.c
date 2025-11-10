
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "defs.h"
#include "ledprog.h"

// LED colours, one byte per color (r/g/b), one colorset per key
unsigned char rgb[MAXLED][COLSANDPARS];

// different programs can be set per LED
unsigned char progs[MAXLED];
unsigned char par[MAXLED][COLSANDPARS];

/*
 * Program handling per LED
 *
 */
void prog_setup() {

	memset(rgb, 0, MAXLED * COLSANDPARS * sizeof(char));
	memset(par, 0, MAXLED * COLSANDPARS * sizeof(char));
	memset(progs, 0, MAXLED * sizeof(char));
}

void lp_init_cols(int key, char pars[COLSANDPARS]) {
	for (int i = 0; i < COLSANDPARS; i++) {	
		rgb[key][i] = pars[i];
	}
}

void prog_set(int k, int prog, char pars[COLSANDPARS]) {
	int chain, led;

	progs[k] = prog;
	for (int i = 0; i < COLSANDPARS; i++) {	
		par[k][i] = pars[i];
	}

	switch(prog) {
	case LP_CONST:
	case LP_LINDECAY:
	case LP_LINDECAYLOOP:
		// set LED colour to initial par value
		lp_init_cols(k, pars);
		break;
	}
}

int lp_lindecay(int key) {

	int f = 1;

	for (int col = 0; col < COLS; col++) {
		if (rgb[key][col]) {
			rgb[key][col]--;
			if (rgb[key][col]) {
				f = 0;
			}
		}
	}
	// are all bits zero?
	if (f) {
		progs[key] = LP_CONST;
	}
	return f;
}

int lp_lindecayloop(int key) {

	if (lp_lindecay(key)) {
		lp_init_cols(key, par[key]);
		progs[key] = LP_LINDECAYLOOP;
	}
}


void prog_advance() {

	for (int key = 0; key < MAXLED; key++) {
		switch(progs[key]) {
		case LP_CONST:
			break;
		case LP_LINDECAY:
			lp_lindecay(key);
			break;
		case LP_LINDECAYLOOP:
			lp_lindecayloop(key);
			break;
		}
	}
}



