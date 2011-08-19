# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected.
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = attiny2313
PROGRAMMER = -c usbtiny 
OBJECTS    = main.o serial.o

# Suited to 4MHz Crystal Oscillator, Clock out on PD2
FUSES = -U lfuse:w:0xbd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
# as above, but no clock out on PD2
#FUSES = -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m


# Tune the lines below only if you know what you are doing:
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
OBJDUMP = avr-objdump
#COMPILE = avr-gcc -Wall -Os -mmcu=$(DEVICE) -Wl,-u,vfprintf -lprintf_flt -lm -finline-limit=10 -fno-if-conversion -mcall-prologues
COMPILE = avr-gcc -Wall -Os -mmcu=$(DEVICE)  -finline-limit=10 -fno-if-conversion -mcall-prologues

# symbolic targets:
all:	main.hex main.lss

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

clean:
	rm -f main.hex main.elf main.lss $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

%.lss: %.elf
	@echo
	$(OBJDUMP) -h -S $< > $@

cpp:
	$(COMPILE) -E main.c
