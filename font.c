#include <avr/pgmspace.h>

PROGMEM const unsigned char font_1[] = {
#include "font-1.h"
};

short font_getheader(unsigned char ch) {
    unsigned short p = 0;
    unsigned char flags = pgm_read_byte(&font_1[p]);

    while(flags!=0) {
        unsigned char start = pgm_read_byte(&font_1[p+1]);
        unsigned char count = flags >>4;
        unsigned char width = (flags & 0x7) +1;

        if (ch >= start && ch < start+count) {
            /* we have found the right header */
            return p;
        }

        /* not this header, skip to the next one */
        p += 2; /* skip header */
        p += (count*width); /* skip data */

        flags = pgm_read_byte(&font_1[p]);
    }

    /* didnt find a header before we fell off the end of the list */
    return -1;
}

unsigned char font_getwidth(unsigned char ch) {
    unsigned short p = font_getheader(ch);

    /* TODO - better error handling? */
    if (p<0) {
        return 0;
    }

    unsigned char flags = pgm_read_byte(&font_1[p]);
    unsigned char count = flags >>4;
    unsigned char width = (flags & 0x7) +1;
    unsigned char nogap = flags >>3 & 1;

    if (nogap) {
        /* this glyph has any needed gap built in, so width is right */
        return width;
    }

    /* Otherwise, add in the padding between chars */
    width++;

    return width;
}

unsigned char font_getdata(unsigned char ch,unsigned char col) {
    unsigned short p = font_getheader(ch);

    /* TODO - better error handling? */
    if (p<0) {
        return 0;
    }

    unsigned char flags = pgm_read_byte(&font_1[p]);
    unsigned char count = flags >>4;
    unsigned char width = (flags & 0x7) +1;
    unsigned char start = pgm_read_byte(&font_1[p+1]);

    if (col >= width) {
        /* anything out of bounds is padding */
        return 0;
    }

    p+=2; /* skip header */
    p+= (ch - start)*width +col;
    return pgm_read_byte(&font_1[p]);
}
