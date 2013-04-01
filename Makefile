
TARGET:=clockmtx.hex
OBJECT:=clockmtx.o serial.o clock.o ht1632c.o config.o

MCU:=atmega8
LFUSE:=0xe2
HFUSE:=0xc9

ISP:=USBasp
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
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) -U flash:w:$<:i

fuse:
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) \
		-U lfuse:w:$(LFUSE):m \
		-U hfuse:w:$(HFUSE):m

size:	$(OBJECT) $(TARGET_ELF)
	avr-size $^

emu:	$(basename $(TARGET)).bin
	simulavr --device $(MCU) -W 0x0C,- -R 0x0C,- $^

clean:
	rm -f $(TARGET) $(TARGET_ELF) $(OBJECT)

$(TARGET_ELF): $(OBJECT)

%.elf: %.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) --remove-section .eeprom -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) --remove-section .eeprom -O binary $^ $@

