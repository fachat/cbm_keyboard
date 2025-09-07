
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "led.h"
#include "i2c.h"

#define	NUMLED		(CHAINS * LEDSPERCHAIN)

// 8 chains of LEDs, 8 bits per color
// Number of bytes is LEDSPERCHAIN * 3 colors * 8 bits 
#define	BUFLEN		(LEDSPERCHAIN * COLS * 8)

// prepared output bit chain, 1 bit per chain in every byte
unsigned char buf[BUFLEN];

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

/*
 * this probably at some point goes into machine language
 *
 * implements the LED control protocol of the SK6812MINI-E 
 * controllable RGB LED:
 *
 * Signal line is a single data line that is passed from 
 * controller to first LED, then from there to the second
 * LED and so on.
 * 
 * Signal reset condition is >80us of low.
 *
 * The protocol transmits 24 bits per LED, with the first
 * 24 bits for the first LED in the chain, the second 24 bits
 * for the 2nd LED and so on.
 * 
 * Each bit is a period of the data line going high, with
 * the value being determined by the duration of the
 * high period:
 *
 * value   min   std   max
 *   0     0.2   0.32  0.4
 *   1     0.58  0.64  1.0
 *
 * The approach chosen here is that we have 8 LED chains
 * of 10 LEDs max each, so that we can set the signal levels
 * for all chains with a single IO byte store.
 * 
 * Timing is selected (planned) as 
 *
 * T=0		: set all I/O bits high
 * T=0.35	: set the I/O bits to the value of the data
 * T=0.7	: set all I/O bits low
 * T=1.4	: end of cycle
 */


extern void send_buf();


void buf_send() {
	int i = 0;

	cli();

	PORTD = 0;

	__builtin_avr_delay_cycles(80*16);

	do {
		PORTD = 255;
		__builtin_avr_delay_cycles(2);
		//_delay_loop_1(1);
		//_delay_ms(1);
		PORTD = buf[i];
		__builtin_avr_delay_cycles(5);
		//_delay_loop_1(1);
		//_delay_ms(1);
		PORTD = 0;
		__builtin_avr_delay_cycles(10);
		i++;
	} while (i < BUFLEN-1);


	sei();
}


/* 
 * transfer the color per led into the bitstream buffer to prep for
 * transmission to the LED.
 *
 * Note that the storage is R/G/B, but the order of transmission
 * is G/R/B. Also MSB first transmission.
 */
void buf_from_led() {

	// shift buffer for current color, one per chain
	char sr[CHAINS];
	//memset(sr, 0, CHAINS * sizeof(char));

	// pointer into buf
	int p = 0;

	// LEDs per chain
	for (int led = 0; led < LEDSPERCHAIN; led++) {

		// all colors
		for (int col = 0; col < COLS; col++) {

			// all chains
			for (int chain = 0; chain < CHAINS; chain++) {

				// gather color values per chain
				switch(col) {
				case 0:
					sr[chain] = rgb[chain][led][1];
					break;
				case 1:
					sr[chain] = rgb[chain][led][0];
					break;
				case 2:
					sr[chain] = rgb[chain][led][2];
					break;
				}
			}

			// 8 bits in a color value		
			for (int bit = 0; bit < 8; bit++) {

				unsigned char tmp = 0;
				// 8 chains
				for (int chain = 0; chain < CHAINS; chain++) {
					tmp = tmp >> 1 | (sr[chain] & 128);
					sr[chain] <<= 1;
				}
				buf[p++] = tmp;
			}
		}
	}
}

void led_setup() {

	UCSRB = 0;
	PORTD = 255;
	DDRD = 255;	// all ports out

	memset(rgb, 0, CHAINS * LEDSPERCHAIN * COLSANDPARS * sizeof(char));
}

void main() {

	i2c_setup(0x10);

	led_setup();

	// clear LEDs 
	buf_from_led();
	buf_send();

	// setup LED programs
	prog_setup();

	prog_set(0, 0, LP_LINDECAYLOOP, (char[]){ 255,0,0, 0});
	prog_set(0, 1, LP_LINDECAYLOOP, (char[]){ 0,100,100, 0});
	prog_set(0, 2, LP_LINDECAYLOOP, (char[]){ 100,100,0, 0});
	prog_set(0, 3, LP_CONST, (char[]){ 128,0,0, 0});

	prog_set(1, 0, LP_LINDECAYLOOP, (char[]){ 0,0,128, 0});
	prog_set(1, 1, LP_LINDECAYLOOP, (char[]){ 128,0,32, 0});
	prog_set(1, 2, LP_LINDECAY, (char[]){ 32,128,0, 0});
	prog_set(1, 3, LP_CONST, (char[]){ 0,128,32, 0});

	prog_set(2, 4, LP_LINDECAY, (char[]){ 0,128,0, 0});

	while (1) {

		buf_from_led();
		buf_send();

		prog_advance();

		while (i2c_check());
	
		_delay_ms(20);
	}
}


