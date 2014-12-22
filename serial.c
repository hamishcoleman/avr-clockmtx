/*
 * Some simple routines to handle serial IO
 *

#define BAUD    9600
#define MYUBRR  F_CPU/16/BAUD-1

 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <string.h>

#include "serial.h"

void serial_init(unsigned int ubrr) {
        /* Set baud rate */
        UBRRH = (unsigned char)(ubrr>>8);
        UBRRL = (unsigned char)ubrr;
        /* Enable receiver and transmitter */
        UCSRB = _BV(RXCIE)| _BV(RXEN)| _BV(TXEN);
        /* Set frame format: 8data, 1stop bit */
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

void serial_puts(unsigned char *buf) {
    serial_write(buf,strlen(buf));
}

void serial_puts_P(PGM_P buf) {
    char ch;
    while((ch = pgm_read_byte(buf++))) {
        serial_putc(ch);
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
                    handle_rx_packet(serial_packet,serial_packetp);
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

