/*
 * Handle screen
 */

#include <avr/pgmspace.h>
#include <stdint.h>
#include <string.h>

#include "clock.h"
#include "ht1632c.h"
#include "font.h"
#include "config.h"

/* This is kind of an orphan variable - we need it somewhere, but it
 * doesnt really live here
 */
char screen_mode;
unsigned long screen_mode_until;

void screen_init(void) {
    HTpinsetup();
    HTsetup();

    char leds[32];  //the screen array, 1 byte = 1 column, left to right, lsb at top.

    /* Show an initial test pattern */
    for (char i=0;i<32;i++) {
        leds[i]=0b01010101<<(i%2);
    }
    HTsendscreen(leds);
}

void screen_puts(unsigned char *s) {
    /* TODO : pass in number of pixels to start at the beginning of the string */

    /* render the screen buffer to the bitmap */
    char leds[32];  //the screen array, 1 byte = 1 column, left to right, lsb at top.
    memset(leds,0,sizeof(leds));

    unsigned char col=0;
    unsigned char ch;
    unsigned char kerning_mask = 0xff;
    while(ch=*s++) {
        unsigned char width = font_getwidth(ch);
        for (unsigned char i=0; i<width; i++) {
            if (col>31) {
                /* no more room on the display, stop adding data  */
                continue;
            }
            unsigned char data = font_getdata(ch,i);

            if (font_isnokern(ch)) {
                /* this glyph should not kern, so create a match any mask */
                kerning_mask = 0xff;
            } else {
                if (i==(width-2)) {
                    /* This is the last data column in the glyph */
                    kerning_mask = data|(data<<1)|(data>>1);
                } else if (i==0 && (kerning_mask&data)==0) {
                    /* This is the first new column, and there is no mask overlap */
                    col--;
                }
            }

            leds[col++] = data;
        }
    }

    /* send the bitmap to the screen controller */
    HTsendscreen(leds);
}

//-------------------------------------------------------------------------------------- clock render ----------

/* Instead of dragging in printf, use this simple two digit zero-fill routine */
char * itoa_zerofill(unsigned char val, char *p) {
    *p++ = (val/10) + '0';
    *p++ = (val%10) + '0';
    *p=0;
    return p;
}

void screen_showclock(unsigned long time) {
    char sec, minute, hour;
    unsigned long secs,mins;

    time += config.offset;

    /* Convert the UTC unixtime into hours minutes and seconds */
    secs   = time % (24L*60*60);
    sec    = secs % 60;

    mins   = secs / 60;
    minute = mins % 60;

    hour   = mins / 60;

    unsigned char buf[8]; /* screen buffer */
    unsigned char *p = buf;

    memset(&buf,';',sizeof(buf));

    /* populate the screen buffer */
    p=itoa_zerofill(hour,p);
    if (sec%2) {
        *p=':';
    } else {
        *p=' ';
    }
    p++;
    itoa_zerofill(minute,p);

    screen_puts(buf);
}
