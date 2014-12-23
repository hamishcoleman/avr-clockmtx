/*
 * Timer0 interrupt driven unix time clock
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"

/* these are the global clock values */
unsigned volatile long time;

void clock_init(void) {
	ASSR |= _BV(AS2);
	/* TODO - try again to use the CTC mode, which would allow us to
         * try storing the xtal trimming config - and autocalc it from a
         * series of time set commands
         *
	 * TCCR2 = (1<<WGM21) | (6<<CS20);	/* CTC mode and clock/256
	 * OCR2 = 128;		/* TODO - set this from config.cal
         */
        /* _BV(CS22) | _BV(CS20) == clock/128 */
	TCCR2=0b00000101;
	TIMSK |= _BV(TOIE2);	/* timer2 overflow interrupt enable */
	sei();
}


/* Timer2 overflow interrupt - our 1pps signal */
ISR(TIMER2_OVF_vect) {
	time++;
	/* serial_putc('.'); DEBUG */
}

