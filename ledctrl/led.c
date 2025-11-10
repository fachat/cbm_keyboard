
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "defs.h"
#include "buf.h"
#include "kbdprog.h"
#include "ledprog.h"
#include "i2c.h"

void main() {

	i2c_setup(0x10);

	buf_setup();

	// setup LEDs and LED programs
	prog_setup();
	kprog_setup();

	// clear LEDs 
	buf_from_rgb();
	buf_send();

#if 0
	prog_set(0, LP_LINDECAYLOOP, (char[]){ 255,0,0, 0});
	prog_set(1, LP_LINDECAYLOOP, (char[]){ 0,100,100, 0});
	prog_set(2, LP_LINDECAYLOOP, (char[]){ 100,100,0, 0});
	prog_set(3, LP_CONST, (char[]){ 128,0,0, 0});
#endif
	kprog_set(3, LP_CONST, (char[]){ 128,0,0, 0});

	while (1) {

		buf_from_rgb();
		buf_send();

		kprog_advance();

		prog_advance();

		while (i2c_check());
	
		_delay_ms(20);
	}
}


