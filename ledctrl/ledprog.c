
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "defs.h"
#include "led.h"

// LED colours, one byte per color (r/g/b), one colorset per key
unsigned char rgb[CHAINS][LEDSPERCHAIN][COLSANDPARS];

// different programs can be set per LED
unsigned char progs[CHAINS][LEDSPERCHAIN];
unsigned char par[CHAINS][LEDSPERCHAIN][COLSANDPARS];

/*
 * Program handling per LED
 *
 */
void prog_setup() {

	memset(rgb, 0, CHAINS * LEDSPERCHAIN * COLSANDPARS * sizeof(char));
	memset(par, 0, CHAINS * LEDSPERCHAIN * COLSANDPARS * sizeof(char));
	memset(progs, 0, CHAINS * LEDSPERCHAIN * sizeof(char));
}

void lp_init_cols(int chain, int led, char pars[COLSANDPARS]) {
	for (int i = 0; i < COLSANDPARS; i++) {	
		rgb[chain][led][i] = pars[i];
	}
}

void prog_set(int chain, int led, int prog, char pars[COLSANDPARS]) {

	progs[chain][led] = prog;
	for (int i = 0; i < COLSANDPARS; i++) {	
		par[chain][led][i] = pars[i];
	}

	switch(prog) {
	case LP_CONST:
	case LP_LINDECAY:
	case LP_LINDECAYLOOP:
		// set LED colour to initial par value
		lp_init_cols(chain, led, pars);
		break;
	}
}

int lp_lindecay(int chain, int led) {

	int f = 1;

	for (int col = 0; col < COLS; col++) {
		if (rgb[chain][led][col]) {
			rgb[chain][led][col]--;
			if (rgb[chain][led][col]) {
				f = 0;
			}
		}
	}
	// are all bits zero?
	if (f) {
		progs[chain][led] = LP_CONST;
	}
	return f;
}

int lp_lindecayloop(int chain, int led) {

	if (lp_lindecay(chain, led)) {
		lp_init_cols(chain, led, par[chain][led]);
		progs[chain][led] = LP_LINDECAYLOOP;
	}
}


void prog_advance() {

	for (int chain = 0; chain < CHAINS; chain++) {
		for (int led = 0; led < LEDSPERCHAIN; led++) {
			switch(progs[chain][led]) {
			case LP_CONST:
				break;
			case LP_LINDECAY:
				lp_lindecay(chain, led);
				break;
			case LP_LINDECAYLOOP:
				lp_lindecayloop(chain, led);
				break;
			}
		}
	}
}



