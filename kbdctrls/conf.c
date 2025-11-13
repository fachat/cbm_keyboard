
#include <avr/eeprom.h>
#include <util/crc16.h>
#include <string.h>

#define	conflen	8
static uint8_t conf[conflen+1];

static uint8_t calc_checksum(char *input, int len) {

	uint16_t crc = 0;

	for (int i = 0; i < len; i++) {

		crc = _crc16_update(crc, input[i]);
	}
	return (crc >> 8) ^ (crc & 0xff); 
}

void conf_setup() {

	eeprom_read_block(conf, 0, conflen+1);

	uint8_t crc = calc_checksum(conf, conflen);

	if (crc != conf[conflen]) {

		memset(conf, 0, conflen+1);
	}
}


unsigned char conf_get_map() {
	return conf[0];
}

void conf_set_map(unsigned char map) {
	conf[0] = map;
}

void conf_commit() {

	conf[conflen] = calc_checksum(conf, conflen);

	eeprom_update_block(conf, 0, conflen+1);
}

