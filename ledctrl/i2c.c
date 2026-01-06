
#include <avr/io.h>

#include "defs.h"
#include "ledprog.h"
#include "kbdprog.h"

/* commands have:
 * 0: command byte
 * 1: led number (chain * LEDSPERCHAIN + led)
 * 2-4: RGB
 * 5: parameter
 */

#define	LP_CMDLEN		6
#define	SP_CMDLEN		2

static unsigned char rxbuf[LP_CMDLEN];
static int rxp;
static int active = 0;

void i2c_setup(int addr) {

	active = 0;

	// enable pullups
	PORTC	= 3;	

	TWAR	= (addr & 0x7f) << 1;

	TWCR 	= (1<<TWEN) | (1<<TWEA) | (1<<TWINT); 

	rxp = 0;
}

void i2c_lp_cmd() {

#if 0
	prog_set(rxbuf[1], rxbuf[0], rxbuf+2);
#else
	if (rxbuf[0] < 128) {
		prog_set(rxbuf[1], rxbuf[0], rxbuf+2);
	} else {
		kprog_set(rxbuf[1], rxbuf[0], rxbuf+2);
	}
#endif
}

void i2c_sp_cmd() {
	char pars[] = { 128,128,128,2 };

	switch(rxbuf[0]) {
	case SP_KEYPRESS:
		//if (rxbuf[1] == 77) {	// SRQ key
		if (rxbuf[1] == 74) {	// space key
			kprog_set(rxbuf[1], KP_LOGO, pars);
		} else {
			kprog_set(rxbuf[1], KP_DILUTE, pars);
		}
		break;
	default:
		break;
	}
}

static int i2c_handle_start() {
	rxp = 0;
	return 1;
}

int i2c_handle_rx(unsigned char byt) {
	rxbuf[rxp] = byt;
	rxp++;

	// first byte determines packet length
	int maxlen = LP_CMDLEN;
	if (rxbuf[0] == SP_KEYPRESS) {
		maxlen = SP_CMDLEN;
	}

	// handle data
	if (rxp >= maxlen) {
		if (rxbuf[0] == SP_KEYPRESS) {
			//rxbuf[1] = 79 - rxbuf[1];
			i2c_sp_cmd();
		} else {
			//rxbuf[1] = 79 - rxbuf[1];
			i2c_lp_cmd();
		}
		rxp = 0;
		return 0;
	}
	return 1;
}


int i2c_check() {

	if (TWCR & (1<<TWINT)) {
		// debug
		//prog_set(0,0, LP_LINDECAY, (char[]){ 32,0,0, 0});
		// we did get an interrupt
		int stat = TWSR & 0xf8;

		switch(stat) {
		case 0x60:
		case 0x68:
			// write to own address rx'd and ACK'd
			active = i2c_handle_start();
			break;
		case 0xA8:
		case 0xB0:
			// read to own address 
			break;
		case 0x70:
		case 0x78:
			// general cal rx'd
			break;
		case 0x90:
		case 0x98:
		case 0x80:
		case 0x88:
			// data byte received and ACK'd
			if (active) {
				active = i2c_handle_rx(TWDR);
			}
			if (stat == 0x90 || stat == 0x98) {
				active = 0;
			}
			break;
		case 0xa0:
			// STOP / repeated START
			active = 0;
			break;
		default:
			break;
		}
		TWCR 	= (1<<TWEN) | (1<<TWEA) | (1<<TWINT); 
	}
	return active;
}

