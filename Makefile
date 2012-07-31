
TARGET:=clockmtx.hex
OBJECT:=clockmtx.o serial.o clock.o ht1632c.o config.o

MCU:=atmega8
LFUSE:=0xe2
HFUSE:=0xc9

ISP:=USBasp
MCU_AVRDUDE:=m8

all:	$(TARGET) size

CC:=avr-gcc
CFLAGS:=-g -Os -std=c99 -mmcu=$(MCU)
OBJCOPY:=avr-objcopy

TARGET_ELF:=$(basename $(TARGET)).elf

burn:	$(TARGET)
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) -U flash:w:$<:i

fuse:
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) \
		-U lfuse:w:$(LFUSE):m \
		-U hfuse:w:$(HFUSE):m

size:	$(OBJECT) $(TARGET_ELF)
	avr-size $^

clean:
	rm -f $(TARGET) $(TARGET_ELF) $(OBJECT)

$(TARGET_ELF): $(OBJECT)

%.elf: %.o
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) --remove-section .eeprom -O ihex $^ $@
