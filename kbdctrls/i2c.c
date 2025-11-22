
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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
static int txlen;

static inline int i2c_wait() {
	int c = 0;
	while (!(TWCR & (1<<TWINT)))
	{
		c++;
		if (c > 1000000) {
			return -1;
		}
	}
	return 0;
}

void i2c_setup(int addr) {

	// enable pullups
	PORTC	= 3;	

	TWAR	= (addr & 0x7f) << 1;

	// to get to 100kHz, the factor must be > 160
	// bit 0/1 are bitrate prescaler bits (1/4/16/64)
	// SCL freq is (CPU clock)/(16+2*TWBR*4^TWPS)
	TWSR	= 3;	// prescaler is 3 -> factor 64
	TWBR 	= 2;	// 

	TWCR 	= (1<<TWEN) | (1<<TWINT); 

	TWCR 	= (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}


static int i2c_send_int() {

	TWCR 	= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	
	if (i2c_wait()) {
		return -1;
	}

	if ((TWSR & 0xf8) != START) {
		return -1;
	}

	TWDR 	= 0x10 << 1;	// write to 0x10
	TWCR	= (1<<TWINT) | (1<<TWEN);

	if (i2c_wait()) {
		return -1;
	}

	if ((TWSR & 0xf8) != MT_SLA_ACK) {
		return -1;
	}

	for (int txp = 0; txp < txlen; txp++) {
		TWDR = txbuf[txp];
		TWCR = (1<<TWINT | 1<<TWEN);

		if (i2c_wait()) {
			return -1;
		}
		if ((TWSR & 0xf8) != MT_DATA_ACK) {
			return -1;
		}
	}

	return 0;
}

int i2c_send(int idx) {

	txbuf[0] = 64;	//SP_KEYPRESS
	txbuf[1] = idx;
	txlen = 2;

	_delay_ms(1);

	i2c_send_int();


	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

	//i2c_wait();
}

