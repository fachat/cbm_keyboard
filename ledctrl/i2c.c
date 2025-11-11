
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

static char rxbuf[LP_CMDLEN];
static int rxp;

void i2c_setup(int addr) {

	// enable pullups
	PORTC	= 3;	

	TWAR	= (addr & 0x7f) << 1;

	TWCR 	= (1<<TWEN) | (1<<TWEA) | (1<<TWINT); 

	rxp = 0;
}

void i2c_lp_cmd() {

	prog_set(rxbuf[1], rxbuf[0], rxbuf+2);
}

void i2c_sp_cmd() {
	char pars[] = { 128,128,128,0 };

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

int i2c_handle_rx(unsigned char byt) {
	rxbuf[rxp] = byt;
	rxp++;

	int maxlen = LP_CMDLEN;
	if (rxbuf[0] == SP_KEYPRESS) {
		maxlen = SP_CMDLEN;
	}
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

static int active = 0;

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
			rxp = 0;
			TWCR 	= (1<<TWEN) | (1<<TWEA) | (1<<TWINT); 
			active = 1;
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
			active = i2c_handle_rx(TWDR);
			TWCR 	= (1<<TWEN) | (1<<TWEA) | (1<<TWINT); 
			if (stat == 0x90 || stat == 0x98) {
				active = 0;
			}
			break;
		case 0xa0:
			// STOP / repeated START
			TWCR |= (1<<TWINT);
			break;
		}
	}
	return active;
}

