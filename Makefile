
all:	clockmtx.hex

clockmtx.elf: clockmtx.o serial.o clock.o ht1632c.o

MCU:=atmega8
LFUSE:=0xe2
HFUSE:=0xc9

ISP:=USBasp
MCU_AVRDUDE:=m8

CC:=avr-gcc
CFLAGS:=-Os -std=c99 -mmcu=$(MCU)
OBJCOPY:=avr-objcopy

burn:	clockmtx.hex
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) -U flash:w:$<:i

fuse:
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) \
		-U lfuse:w:$(LFUSE):m \
		-U hfuse:w:$(HFUSE):m

%.elf: %.o
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@
