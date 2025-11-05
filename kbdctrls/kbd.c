
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "kbdhw.h"
#include "i2c.h"


void main() {

	i2c_setup(0x10);

	host_setup();

	// fake key for test
	//rowvals[1] = 0x01;

	while (1) {
		kbd_scan();
		//joy_scan();
		check_res();
		_delay_ms(10);
	}
}


