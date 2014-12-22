/*
 * Driver routines for a ht1632c LED matrix
 */
#ifndef HT1632C_H
#define HT1632C_H

void HTsendscreen(char []);
void HTpinsetup(void);
void HTsetup();
void HTbrightness(char);

#endif
