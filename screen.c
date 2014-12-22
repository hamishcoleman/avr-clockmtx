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

#define byte uint8_t
#define word uint16_t

char leds[32];  //the screen array, 1 byte = 1 column, left to right, lsb at top.

void screen_init(void) {
    HTpinsetup();
    HTsetup();

    /* Show an initial test pattern */
    for (byte i=0;i<32;i++) {
        leds[i]=0b01010101<<(i%2);
    }
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

void renderclock(unsigned long time) {
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
        *p=';';
    }
    p++;
    itoa_zerofill(minute,p);

    /* render the screen buffer to the bitmap */
    unsigned char col=0;
    p=buf;
    unsigned char ch;
    while(ch=*p++) {
        unsigned char width = font_getwidth(ch);
        for (unsigned char i=0; i<width; i++) {
            if (col>32) {
                /* no more room on the display, stop adding data  */
                continue;
            }
            leds[col++] = font_getdata(ch,i);
        }
    }

    /* send the bitmap to the screen controller */
    HTsendscreen(leds);
}

