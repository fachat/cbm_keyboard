
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "kbdhw.h"
#include "i2c.h"
#include "print.h"
#include "menu.h"
#include "conf.h"
#include "map.h"


void main() {
	int reset = 0;

	i2c_setup(0x10);

	host_setup();

	print_setup();

	menu_setup();

	// read config from eeprom
	conf_setup();
	map_enable(conf_get_map());

	while (1) {

		kbd_scan(&reset);

		set_reset(reset);

		//joy_scan();

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


