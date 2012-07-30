/*
 * Timer0 interrupt driven unix time clock
 *
 * Also, some functions to help convert this to human time and back
 *
 */
#ifndef CLOCK_H
#define CLOCK_H

extern volatile unsigned long time;

void clock_init(void);

extern char sec, minute, hour;

void incsec(char);
void decsec(char);
char clockhandler(void);

#endif
