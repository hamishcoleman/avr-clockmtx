/*
 * Timer0 interrupt driven unix time clock
 *
 * Also, some functions to help convert this to human time and back
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"

/* these are the global clock values */
unsigned long time;
unsigned char tz[6];
unsigned long offset;
unsigned long cal;

