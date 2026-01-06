
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

static inline int i2c_wait() {
	uint32_t c = 0;
	while (!(TWCR & (1<<TWINT)))
	{
		c++;
		if (c > 100000) {
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
	// to get to 20kHz, the factor must be > 800
	// bit 0/1 are bitrate prescaler bits (1/4/16/64)
	// SCL freq is (CPU clock)/(16+2*TWBR*4^TWPS)
	TWSR	= 3;	// prescaler is 3 -> factor 64
	TWBR 	= 2;	// 16+2*2*64 = 16+256
	//TWBR 	= 5;	// 16+2*5*64 = 16+256 = 640

	TWCR 	= (1<<TWEN) | (1<<TWINT); 
}

static void i2c_reset() {

	TWCR 	= (1<<TWINT);

	_delay_ms(1);

	TWCR 	= (1<<TWEN) | (1<<TWINT); 

}

static void i2c_stop() {

	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

static int i2c_send_int(char *buf, int len) {

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

	for (int txp = 0; txp < len; txp++) {
		TWDR = buf[txp];
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

	_delay_ms(1);

	if (i2c_send_int(txbuf, 2)) {
		i2c_reset();
	} else {
		i2c_stop();
	}
}

int i2c_send_slock(int idx, int slock) {

	txbuf[0] = 0;	//SP_CONST
	txbuf[1] = idx;
	txbuf[2] = slock ? 255 : 0;
	txbuf[3] = 0;	
	txbuf[4] = 0;	
	txbuf[5] = 0;	

	if (i2c_send_int(txbuf, 6)) {
		i2c_reset();
	} else {
		i2c_stop();
	}
}

