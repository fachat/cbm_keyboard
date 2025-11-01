
#include <avr/io.h>
#include <avr/interrupt.h>


.extern rowvals

.text 

PORTD=0x12
PINB=0x16
DDRA=0x1a

;---------------------------------------------------------------------------
; TIMER underflow interrupt (timer 1)
.global INT0_vect							; cycles in IRQ:

INT0_vect:
;	push 	r2		; 	make place for status register		; 2
;	in	r2, SREG	; save status register			; 1

	push	r30		; 	save working register			
	push	r31		; 	save working register			

	in	r30, PINB
	andi	r30, 0x0f
 	ldi	r31, 0
	subi	r30, 0xa0
	sbci	r31, 0xff
	ld	r30, Z
	out	DDRA, r30

	; end of interrupt
	pop	r31
	pop	r30

;	out	SREG, r2
;	pop	r2
	reti
.end
	
