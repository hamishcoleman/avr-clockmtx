/*
 * Timer0 interrupt driven unix time clock
 *
 * Also, some functions to help convert this to human time and back
 *
 */
#ifndef CLOCK_H
#define CLOCK_H

extern unsigned long time;
extern unsigned char tz[6];
extern unsigned long offset;
extern unsigned long cal;

#endif
