
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "kbdhw.h"
#include "i2c.h"
#include "print.h"
#include "menu.h"


void main() {

	i2c_setup(0x10);

	host_setup();

	print_setup();

	menu_setup();

	// fake key for test
	//rowvals[1] = 0x01;

	while (1) {

		kbd_scan();

		//joy_scan();
		check_res();

		if (menu_advance()) {
			// menu has taken over, so no output to host
			scan_clear();
		}

		// print output to host
		print_advance();

		// copy over the input to the host
		key_swap();

		_delay_ms(10);
	}
}


