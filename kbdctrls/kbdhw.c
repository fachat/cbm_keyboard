
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "kbdhw.h"
#include "map.h"

unsigned char scanvals[16];
static unsigned char rowvals[16];

/*
 * Connection to the actual host
 */
void host_setup() {

	memset(rowvals, 0, 16);

	// disable UART (alt fun conflicts with I/O)
	UCSRB = 0;
	// JTAG needs to be disabled by HFUSE

	// port A0-3 is select lines in from host
	DDRA &= 0xf0;
	// no pullups
	PORTA &= 0xf0;

	// PB 0-7 are "open collector" outputs; 
	// i.e. only set to output 0 when active, otherwise input
	// PB 5-7 are SPI prog lines, doubling as output to Host
	DDRB &= 0x00;
	// no pullups, set to output 0 when active to pull low
	PORTB &= 0x00;

	// PD3 is output to /RES when pulled low
	PORTD |= 0x08;
	DDRD |= 0x08;

	// interrupt handling
	// PD2 and PD3 are interrupt sources, PD2 is INT0 
	// and used as host select change int
	DDRD &= 0xfb;
	// no pullups
	PORTD &= 0xfb;

	// configure int0 on falling edge
	MCUCR = (MCUCR & 0xfc) | 0x02;
	// enable int0
	GICR |= 0x40;

	// own keyboard scan

	// row select bits 0/1 PD0/1
	DDRD |= 0x03;
	PORTD &= 0xfc;
	// row select bits 2/3 PC2/3
	DDRC |= 0x0c;
	PORTC &= 0xf3;

	// column value inputs
	// split on two ports due to needed alt functions for PC0/1, PD2

	// bits 0-3 are PC4-7, input with pullup
	DDRC &= 0x0f;
	PORTC |= 0xf0;
	// bits 4-7 are PD4-7, input with pullup
	DDRD &= 0x0f;
	PORTD |= 0xf0;

	// set PA0 to analog input,

	// AREF = AVCC
	// left adjust so we only need to use the
	// high byte with 8 bit resolution
	// select A7
	ADMUX = 0x67;
	// enable ADC
	ADCSRA = 0x80;

	sei();
}

/*
 * note this ISR sets DDRA in less than 2.3us
 * which is very much enough for a 1-4 MHz PET scan routine
 * but not for a 13.5 MHz speed machine. 
 *
 * The normal scan reads the byte, then compares it again.
 * The first read happens on cycle 8 after change, the second on cycle 11.
 * See $e4be and following in the BASIC 4
 *
 * So, we have 11 cycles at 13.5 MHz, or 810ns only to write the correct
 * value ...
 *
 */
ISR( INT0_vect ) {
	DDRB = rowvals[PINA & 0x0f];
	//PORTD ^= 0x01;
}

/*
 * set the RES line active to reset the host
 */
void set_reset(int reset) {
	if (reset) {
		// res low
		PORTD &= 0xf7;
	} else {
		PORTD |= 0x08;
	}
}

/*
 * scan the joystick
 */
void joy_scan() {
	// prescaler 7 = 1/128 = 125kHz
	ADCSRA = 0x87;

	//start conversion
	ADCSRA |= 1 << ADSC;

	while (!(ADCSRA & 0x10)) {
		// ADIF set
	
		// we use left adjust, so only high byte needed	
		unsigned char val = ADCH;

		// max value is 0xff
		// mid value is 0x80
		// low water we define as 0x40
		// high water we set to 0xc0
		if (val < 0x40) {
			// set crsr down
			scanvals[1] |= 0x40;
			// set ">" 
			//scanvals[8] |= 0x10;
		} else
		if (val > 0xc0) {
			// set crsr up
			scanvals[1] |= 0x40;
			scanvals[8] |= 0x20;
			// set "<" 
			//scanvals[9] |= 0x08;
		}
	}	
}

void key_swap() {

	memcpy(rowvals, scanvals, sizeof(rowvals));
}

void scan_clear() {

	memset(scanvals, 0, sizeof(rowvals));
}

