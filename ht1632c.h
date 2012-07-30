/*
 * Driver routines for a ht1632c LED matrix
 */
#ifndef HT1632C_H
#define HT1632C_H

extern char leds[32];  //the screen array, 1 byte = 1 column, left to right, lsb at top. 

void HTsendscreen(void);
void HTpinsetup(void);
void HTsetup();
void HTbrightness(char);

#endif
