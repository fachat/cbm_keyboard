
#include <util/delay.h>
#include <avr/io.h>

#define	ROWS	10
#define	COLS	8

extern unsigned char scanvals[16];

static inline unsigned char kbd_read(int row) {

        // set row selector                      
        PORTD = (PORTD & 0xfc) | (row & 0x03);   
        PORTC = (PORTC & 0xf3) | (row & 0x0c);   
                                                        
        _delay_us(10);                           
                                                       
        // read col values for row               
        unsigned char v = 0;                     
        unsigned char ind = PIND;                
        unsigned char inc = PINC;                
        v = (ind & 0xf0) | ((inc >> 4) & 0x0f);  
        v ^= 255;                                
	return v;
}

static inline int is_key_down(int idx) {
	return scanvals[idx/8] & (1 << (idx%8));
}

static inline void set_key_down(int idx) {
	scanvals[idx/8] |= (1<< (idx%8));
}

static inline void set_key_up(int idx) {
	scanvals[idx/8] &= 255 - (1<< (idx%8));
}

void host_setup();

void kbd_scan();

void check_res();

void joy_scan();

void key_swap();

