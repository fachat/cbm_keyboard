
all: led.lst

FILES=led.c
ASMFILES=send_buf.s

MCU 	?= atmega32a
PROGDEV	?= avrisp2
DEVICE=
FREQ	?= 16000000

flash: led.hex
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U flash:w:led.hex -F

fuses:
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:r:con:h
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:w:0xef:m

led.lst: led.elf
	avr-objdump -DrwC -s -m avr5 $< > $@

led.hex: led.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

led.elf: led.o send_buf.o
	#avr-gcc -g -mmcu=$(MCU) -o $@ led.o send_buf.o
	avr-gcc -Os -g -mmcu=$(MCU) -o $@ led.o send_buf.o
	#avr-gcc -Os -Wl --gc-sections -g -mmcu=$(MCU) led.o send_buf.o -o $@ 

%.o: %.c
	avr-gcc -DF_CPU=$(FREQ) -O3 -g -mmcu=$(MCU) -c $< -o $@

%.o: %.s
	avr-gcc -DF_CPU=$(FREQ) -O3 -D__AVR_ATmega32A__ -g -mmcu=$(MCU) -c $< -o $@

clean:
	rm -f *.o led.elf led.hex led.lst
