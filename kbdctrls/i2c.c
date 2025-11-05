
#include <avr/io.h>

/* commands have:
 * 0: command byte
 * 1: led number (chain * LEDSPERCHAIN + led)
 * 2-4: RGB
 * 5: parameter
 */

#define	LP_CMDLEN		6
#define	SP_CMDLEN		2

#define	START			8
#define	MT_SLA_ACK		0x18
#define	MT_DATA_ACK		0x28

static char txbuf[LP_CMDLEN];
static int txp;

void i2c_setup(int addr) {

	// enable pullups
	PORTC	= 3;	

	TWAR	= (addr & 0x7f) << 1;

	TWCR 	= (1<<TWEN) | (1<<TWINT); 
}

int i2c_handle_tx() {
	unsigned char byt = txbuf[txp];
	txp++;

	TWDR 	= byt;
	TWCR	= (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1<<TWINT)));

	return txp < 2;
}



static int i2c_send_int() {

	TWCR 	= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));

	if ((TWSR & 0xf8) != START) {
		return -1;
	}
	
	TWDR 	= 0x10 << 1;	// write to 0x10
	TWCR	= (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1<<TWINT)));

	if ((TWSR & 0xf8) != MT_SLA_ACK) {
		return -1;
	}

	while (i2c_handle_tx()) {
		if ((TWSR & 0xf8) != MT_DATA_ACK) {
			return -1;
		}
	}

	return 0;
}

int i2c_send(int idx) {

	txbuf[0] = 64;	//SP_KEYPRESS
	txbuf[1] = idx;
	txp = 0;

	i2c_send_int();

	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}
