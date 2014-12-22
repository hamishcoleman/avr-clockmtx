
TARGET:=clockmtx.hex
OBJECT:=clockmtx.o serial.o clock.o ht1632c.o config.o font.o calibraterc.o \
	version.o

MCU:=atmega8
LFUSE:=0xe2
HFUSE:=0xc9

ISP ?= USBasp
ISP_PORT ?= usb
MCU_AVRDUDE:=m8

all:	$(TARGET) size

CC:=avr-gcc
CFLAGS:=-g -Os -std=c99 -mmcu=$(MCU)	# -mcall-prologues
OBJCOPY:=avr-objcopy

LDFLAGS:=-Wl,-section-start=.eeprom=0x810001

TARGET_ELF:=$(basename $(TARGET)).elf

build-dep:
	apt-get install avrdude gcc-avr avr-libc

burn:	$(TARGET)
	avrdude -c $(ISP) -P $(ISP_PORT) -p $(MCU_AVRDUDE) -y -U flash:w:$<:i

osccal:
	avrdude -c $(ISP) -P $(ISP_PORT) -p $(MCU_AVRDUDE) -O

fuse:
	avrdude -c $(ISP) -P $(ISP_PORT) -p $(MCU_AVRDUDE) \
		-U lfuse:w:$(LFUSE):m \
		-U hfuse:w:$(HFUSE):m

size:	$(OBJECT) $(TARGET_ELF)
	avr-size $^

emu:	$(basename $(TARGET)).bin
	simulavr --device $(MCU) -W 0x0C,- -R 0x0C,- $^

monitor:
	jpnevulator --read --ascii --byte-count --timing-print --tty=/dev/ttyUSB0 --pty --pass


clean:
	rm -f $(TARGET) $(TARGET_ELF) $(OBJECT)

version.o: version-auto.h
version-auto.h: $(OBJECT:.o=.c)
	echo '#define TARGET "$(TARGET)"' >$@
	echo '#define VERSION "$(shell git describe --dirty)"' >>$@

$(TARGET_ELF): $(OBJECT)

%.elf: %.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) --remove-section .eeprom -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) --remove-section .eeprom -O binary $^ $@

