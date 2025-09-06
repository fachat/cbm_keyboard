
BASE=kbd

all: $(BASE).lst

OFILES= $(BASE).o kbdint.o

MCU 	?= atmega32a
PROGDEV	?= avrisp2
DEVICE=
FREQ	?= 16000000

flash: $(BASE).hex
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U flash:w:$(BASE).hex -F

fuses:
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:r:con:h
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:w:0xef:m

$(BASE).lst: $(BASE).elf
	avr-objdump -DrwC -s -m avr5 $< > $@

$(BASE).hex: $(BASE).elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

$(BASE).elf: $(OFILES)
	avr-gcc -Os -g -mmcu=$(MCU) -o $@ $(OFILES)

%.o: %.c
	avr-gcc -DF_CPU=$(FREQ) -O3 -g -mmcu=$(MCU) -c $< -o $@

%.o: %.s
	avr-gcc -DF_CPU=$(FREQ) -O3 -D__AVR_ATmega32A__ -g -mmcu=$(MCU) -c $< -o $@

clean:
	rm -f *.o $(BASE).elf $(BASE).hex $(BASE).lst
