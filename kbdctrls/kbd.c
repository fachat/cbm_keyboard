
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "kbdhw.h"
#include "i2c.h"
#include "print.h"


void main() {

	i2c_setup(0x10);

	host_setup();

	// fake key for test
	//rowvals[1] = 0x01;

	while (1) {
		kbd_scan();
		print_advance();
		//joy_scan();
		check_res();
		print_advance();
		key_swap();

		_delay_ms(10);
	}
}


