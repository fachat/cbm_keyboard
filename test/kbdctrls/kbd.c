
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "kbd.h"

unsigned char rowvals[16];

/*
 * Connection to the actual host
 */
void host_setup() {

	memset(rowvals, 0, 16);

	// disable UART (alt fun conflicts with I/O)
	UCSRB = 0;
	// JTAG needs to be disabled by HFUSE

	// port B0-3 is select lines in from host
	DDRB &= 0xf0;
	// no pullups
	PORTB &= 0xf0;
	// PB 5-7 are SPI prog lines, doubling as output to Host
	// test for moving PA0-7 to PB0-7
	DDRB &= 0x1f;
	// no pullups, set to output 0 when active to pull low
	PORTB & 0x1f;

	// PB4 is output to /RES when pulled low
	PORTB |= 0x10;
	DDRB |= 0x10;

	// PA is column out, using open-collector
	// no pullups. So, output is sent by
	// setting port to 0, and input.
	// then switching to output 
	// for those bits that should be pulled low
	DDRA = 0;
	PORTA = 0; 

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

	// row select bits 0/1
	DDRD |= 0x03;
	PORTD &= 0xfc;
	// row select bits 2/3
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
	ADMUX = 0x60;
	// enable ADC, single ended PA0
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
	DDRA = rowvals[PINB & 0x0f] & 0xfe;
	//PORTD ^= 0x01;
}

void kbd_scan() {
	for (unsigned char row = 0; row < 16; row++) {
		// set row selector
		PORTD = (PORTD & 0xfc) | (row & 0x03);
		PORTC = (PORTC & 0xf3) | (row & 0x0c);

		// read col values for row
		unsigned char v = 0;
		unsigned char ind = PIND;
		unsigned char inc = PINC;
		v = (ind & 0xf0) | ((inc >> 4) & 0x0f);

		v |= 3; 

		rowvals[row] = v ^ 255;
	}
}

void check_res() {
	// check RES
	if (rowvals[9] & 0x20) {
		// res low
		PORTB &= 0xef;
	} else {
		PORTB |= 0x10;
	}
}

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
			rowvals[1] |= 0x40;
			// set ">" 
			//rowvals[8] |= 0x10;
		} else
		if (val > 0xc0) {
			// set crsr up
			rowvals[1] |= 0x40;
			rowvals[8] |= 0x20;
			// set "<" 
			//rowvals[9] |= 0x08;
		}
	}	

}

void main() {

	host_setup();

	// fake key for test
	//rowvals[1] = 0x01;

	while (1) {
		kbd_scan();
		joy_scan();
		check_res();
		_delay_ms(10);
	}
}


