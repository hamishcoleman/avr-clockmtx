/*
 * Some simple routines to handle serial IO
 *

#define BAUD    9600
#define MYUBRR  F_CPU/16/BAUD-1

 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"

void serial_init(unsigned int ubrr) {
        /* Set baud rate */
        UBRRH = (unsigned char)(ubrr>>8);
        UBRRL = (unsigned char)ubrr;
        /* Enable receiver and transmitter */
        UCSRB = (1<<RXEN)|(1<<TXEN);
        /* half the ubrr */
        UCSRA = (1<<U2X);
        /* Set frame format: 8data, 2stop bit */
        UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

void serial_putc(unsigned char data) {
        /* Wait for empty transmit buffer */
        while ( !( UCSRA & (1<<UDRE)) )
                ;
        /* Put data into buffer, sends the data */
        UDR = data;
}

void serial_write(unsigned char *buf, int count) {
        while(count--) {
                serial_putc(*buf);
                buf++;
        }
}

