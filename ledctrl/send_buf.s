
#include <avr/io.h>



.extern buf

.global send_buf

#.section text

PORTD	=0x12

send_buf:
bitloop:
	ldi	r20,0
	ldi	r19,55
	ldi 	r18,255
	out	PORTD, r18
	out	PORTD, r19
	out	PORTD, r20
	eor	r19,r18
	nop
	nop
	jmp bitloop

