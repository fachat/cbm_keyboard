
all: led.lst

OFILES=led.o i2c.o

MCU 	?= atmega32a
PROGDEV	?= avrisp2
DEVICE=
FREQ	?= 16000000

flash: led.hex
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U flash:w:led.hex -F

fuses:
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:r:con:h -F
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:w:0xef:m -F

led.lst: led.elf
	avr-objdump -DrwC -s -m avr5 $< > $@

led.hex: led.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

led.elf: $(OFILES)
	avr-gcc -Os -g -mmcu=$(MCU) -o $@ $(OFILES)

%.o: %.c
	avr-gcc -DF_CPU=$(FREQ) -O3 -g -mmcu=$(MCU) -c $< -o $@

%.o: %.s
	avr-gcc -DF_CPU=$(FREQ) -O3 -D__AVR_ATmega32A__ -g -mmcu=$(MCU) -c $< -o $@

clean:
	rm -f *.o led.elf led.hex led.lst
