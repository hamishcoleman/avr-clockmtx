/*
 * Some simple routines to handle serial IO
 *

#define BAUD    9600
#define MYUBRR  F_CPU/16/BAUD-1

 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <string.h>

#include "serial.h"
#include "clock.h"
#include "config.h"

void serial_init(unsigned int ubrr) {
        /* Set baud rate */
        UBRRH = (unsigned char)(ubrr>>8);
        UBRRL = (unsigned char)ubrr;
        /* Enable receiver and transmitter */
        UCSRB = _BV(RXCIE)| _BV(RXEN)| _BV(TXEN);
        /* Set frame format: 8data, 2stop bit */
        UCSRC = _BV(URSEL)| (3<<UCSZ0);
}

void serial_putc(unsigned char data) {
    /* TODO - need an interface that knows when to DLE */
        /* Wait for empty transmit buffer */
        loop_until_bit_is_set(UCSRA,UDRE);

        /* Put data into buffer, sends the data */
        UDR = data;
}

void serial_write(unsigned char *buf, int count) {
        while(count--) {
                serial_putc(*buf);
                buf++;
        }
}

char *message = "wall of correct clocks - Aconex Hackathon 201207";

void serial_docmd(unsigned char *input) {
	char buf[11];
	char *p = buf;
	serial_putc('=');
        unsigned char cmd = input[0];
	switch(cmd) {
		case 'p':	/* PING */
			serial_putc('P');
			break;
		case 'i':	/* ID */
			serial_putc('I');
			serial_write(message,strlen(message));
			/* TODO - output more */
			break;
		case 'C':	/* Set calibration */
			config.cal = atoi(&input[1]);
			config_dirty();
		case 'c':	/* Get calibration */
			serial_putc('C');
			itoa(config.cal,p,10);
			serial_write(p,strlen(p));
			break;
		case 'T':	/* Set Time */
			/* NOTE: 32bit value, set from interrupt context */
			time = atol(&input[1]);
		case 't':	/* Get Time */
			serial_putc('T');
			ultoa(time,p,10);
			serial_write(p,strlen(p));
			break;
		case 'Z':	/* Set TZ name */
			strncpy(config.tz,&input[1],sizeof(config.tz));
			config_dirty();
		case 'z':	/* Get TZ name */
			serial_putc('Z');
			serial_write(config.tz,strlen(config.tz));
			break;
		case 'O':	/* Set Offset */
                        {
                        unsigned char * p1 = strtok(&input[1],",");
			if (!strcmp(config.tz,p1)) {
				config.offset = atol(strtok(NULL,","));
				config_dirty();
			} else {
				/* Ignore commands not for us */
				break;
			}
                        }
		case 'o':	/* Get Offset */
			serial_putc('O');
			serial_write(config.tz,strlen(config.tz));
			serial_putc(',');
			ltoa(config.offset,p,10);
			serial_write(p,strlen(p));
			break;
	}
	serial_putc('\r');
	serial_putc('\n');
}

void serial_rx_packet(unsigned char *buf,unsigned char size) {
    /* HACK */
    if (size < 32) {
        buf[size]=0;
        serial_docmd(buf);
    }
}

unsigned char serial_flags = SERIAL_ECHO || SERIAL_DLE;
/* Start with echo on */
/* ensure sync by throwing the first char */

unsigned char serial_packetp;     /* pointer into the packet buffer */

#define PACKET_BUF      32
unsigned char serial_packet[PACKET_BUF];

ISR(USART_RXC_vect) {
    unsigned char ch;

    if (bit_is_set(UCSRA,FE)) {
        /* This is a rx frame error - read it and throw it away */
        ch = UDR;
        serial_putc(SERIAL_NAK);  /* send warning */
        serial_flags &= ~SERIAL_INPACKET;  /* stop buffering */
        return;
    }

    ch = UDR;

    if (serial_flags & SERIAL_ECHO) {
        /* TODO - _putc needs some kind of mutex */
        serial_putc(ch);
    }

    if (!(serial_flags & SERIAL_INDLE)) {
        /* If we are allowed to interpret ctrl chars */
        switch(ch) {
            case SERIAL_DLE:
                serial_flags |= SERIAL_INDLE;
                return; /* dont copy DLE into packet buffer */
            case SERIAL_STX:
                serial_packetp=0;
                serial_flags |= SERIAL_INPACKET;
                return; /* dont copy STX into packet buffer */
#if 0
            case SERIAL_ETX:
                /* TODO - add packet checksum code */
#endif
            case SERIAL_EOT:
                if (serial_flags & SERIAL_INPACKET) {
                    serial_flags &= ~SERIAL_INPACKET;
                    serial_rx_packet(serial_packet,serial_packetp);
                }
                return;
        }
    } else {
        serial_flags &= ~SERIAL_INDLE;
    }

    if (serial_flags & SERIAL_INPACKET) {
        if (serial_packetp<sizeof(serial_packet)) {
            /* There is room left in the buffer */
            serial_packet[serial_packetp++]=ch;
        } else {
            /* No room left */
            serial_putc(SERIAL_NAK);  /* send warning */
            serial_flags &= ~SERIAL_INPACKET;  /* stop buffering */
        }
    } else {
        /* Do nothing, but this is where we could hook to a non
           packetised interface */
    }
}

