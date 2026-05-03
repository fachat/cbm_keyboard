
BASE=kbd

all: $(BASE).lst

OFILES= $(BASE).o kbdint.o kbdhw.o map.o i2c.o print.o menu.o conf.o
SCAN31=scan.o
SCAN30=scan30.o

MCU 	?= atmega32a
PROGDEV	?= avrisp2
DEVICE=
FREQ	?= 16000000

flash30: $(BASE)30.hex
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U flash:w:$< -F

flash: $(BASE).hex
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U flash:w:$< -F

fuses:
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:r:conl:h
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U hfuse:r:conh:h
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U lfuse:w:0xef:m
	avrdude -c $(PROGDEV) $(DEVICE) -p m32 -B10 -U hfuse:w:0xd9:m

$(BASE).lst: $(BASE).elf
	avr-objdump -DrwC -s -m avr5 $< > $@

$(BASE).hex: $(BASE).elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

$(BASE)30.hex: $(BASE)30.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

$(BASE).elf: $(OFILES) $(SCAN31)
	avr-gcc -Os -g -mmcu=$(MCU) -o $@ $(OFILES) $(SCAN31)

$(BASE)30.elf: $(OFILES) $(SCAN30)
	avr-gcc -Os -g -mmcu=$(MCU) -o $@ $(OFILES) $(SCAN30)

scan30.o: scan.c
	avr-gcc -DF_CPU=$(FREQ) -O3 -g -mmcu=$(MCU) -c -DREV30 $< -o $@

%.o: %.c
	avr-gcc -DF_CPU=$(FREQ) -O3 -g -mmcu=$(MCU) -c $< -o $@

%.o: %.s
	avr-gcc -DF_CPU=$(FREQ) -O3 -D__AVR_ATmega32A__ -g -mmcu=$(MCU) -c $< -o $@

clean:
	rm -f *.o $(BASE).elf $(BASE).hex $(BASE).lst
