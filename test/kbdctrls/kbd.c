
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

	// port B0-3 is select lines in from host
	DDRB &= 0xf0;
	// no pullups
	PORTB &= 0xf0;

	// PA is column out, using open-collector
	// no pullups. So, output is sent by
	// setting port to 0, and switching to output 
	// for those bits that should be pulled low
	DDRA = 0;
	PORTA = 0; 

	// interrupt handling
	// PD2 and PD3 are interrupt sources, PD2 is INT0 
	// and used as host select change int
	DDRD &= 0xf3;
	// no pullups
	PORTD &= 0xf3;

	// configure int0 on falling edge
	MCUCR = (MCUCR & 0xfc) | 0x02;
	// enable int0
	GICR |= 0x40;

	// debug pin out
	DDRD |= 0x01;
	PORTD |= 0x01;

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
	DDRA = rowvals[PINB & 0x0f];
	//PORTD ^= 0x01;
}

void main() {

	host_setup();

	// fake key for test
	rowvals[1] = 0x01;

	while (1) {
	//	PORTD ^= 0x01;
	}
}


