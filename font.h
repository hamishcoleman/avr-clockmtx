#ifndef FONT_H
#define FONT_H

#define FONT_RAMSIZE    46 /* enough for filling the screen and then a little more */
extern unsigned char font_0[FONT_RAMSIZE];

void font_init(void);
void font_writebyte(char, unsigned short, unsigned char);
unsigned char font_getwidth(unsigned char);
unsigned char font_getdata(unsigned char,unsigned char);

#endif
