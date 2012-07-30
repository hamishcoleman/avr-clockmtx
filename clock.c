/*
 * Timer0 interrupt driven unix time clock
 *
 * Also, some functions to help convert this to human time and back
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "serial.h"

/* these are the global clock values */
unsigned volatile long time;
unsigned char tz[6];
unsigned long offset;
unsigned char cal;

void clock_init(void) {
	/* TODO - load tz, offset and cal from eeprom */
	ASSR |= (1<<AS2);
	/* TODO - try again to use the CTC mode */
	/* TCCR2 = (1<<WGM21) | (6<<CS20);	/* CTC mode and clock/256 */
	/* OCR2 = 128;		/* TODO - set this from cal */
	TCCR2=0b00000101;
	TIMSK |= (1<<TOIE2);	/* timer2 overflow interrupt enable */
	sei();
}


/* Timer2 overflow interrupt - our 1pps signal */
ISR(TIMER2_OVF_vect) {
	time++;
	/* serial_putc('.'); DEBUG */
}

char sec, minute, hour;
unsigned long time_last = 200;

/* Convert the UTC unixtime into hours minutes and seconds */
void time2hms(void) {
	unsigned long time0 = time; /* Capture the current time */
	unsigned long secs,mins;

	time0 += offset;

	secs   = time0 % (24L*60*60);
	sec    = secs % 60;

	mins   = secs / 60;
	minute = mins % 60;

	hour   = mins / 60;
}

void incsec(char add) {
	time+=add*60;
	time2hms();
}

void decsec(char sub) {
	incsec(-sub);
}

char clockhandler(void) {
	if (time==time_last)
		return 0;   //check if something changed
	time_last=time;
	time2hms();
	return 1;
}

