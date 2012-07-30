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
        UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
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

enum serial_state {
	NONE =0,
	INTR,
	PARAM1,
	PARAM2,
	DONE,
};
enum serial_state state;
unsigned char cmd;
#define PARAM_SIZE	11
unsigned char param1[PARAM_SIZE];
unsigned char param2[PARAM_SIZE];
unsigned char count;

void serial_docmd(unsigned char ch) {
	serial_write("Got: ",5);
	serial_putc(cmd);
	serial_putc(',');
	serial_write(param1,sizeof(param1));
	serial_putc(',');
	serial_write(param2,sizeof(param2));
	serial_putc('\r');
	serial_putc('\n');
}

ISR(USART_RXC_vect) {
	unsigned char ch = UDR;
	unsigned char *p;

	switch(state) {
		case NONE:
			if (ch == 1) { state = INTR; }
			return;
		case INTR:
			serial_putc(ch);
			cmd = ch;
			param1[0]=0;
			param2[0]=0;
			count = 0;
			state = PARAM1;
			return;
		case PARAM1:
			p = param1;
			break;
		case PARAM2:
			p = param2;
			break;
	}

	if (count<=PARAM_SIZE) {
		serial_putc(ch);
		p[count++]=ch;
	}

	if (ch == '\r' || ch == '\n') {
		p[count]=0;
		state=DONE;
	}
	if (ch == ',') {
		p[count]=0;
		count = 0;
		state = PARAM2;
	}

	if (state == DONE) {
		serial_docmd(ch);
		state = NONE;
	}
}

