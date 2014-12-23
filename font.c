#include <avr/pgmspace.h>

#include <string.h>
#include "font.h"

unsigned char font_0[FONT_RAMSIZE];

PROGMEM const unsigned char font_1[] = {
#include "font-1.h"
};

#define FONT_MAX        1 /* only two fonts */

void font_init(void) {
    memset(&font_0,0,sizeof(font_0));
}

/*
 * grumble, harvard architecture
 */
unsigned char font_readbyte(char fontnr,short offset) {
    switch (fontnr) {
        case 0: /* ram font patch */
            if (offset>sizeof(font_0)) {
                return 0;
            }
            return font_0[offset];
        case 1: /* flash font */
            if (offset>sizeof(font_1)) {
                return 0;
            }
            return pgm_read_byte(&font_1[offset]);
    }
    return 0;
}

void font_writebyte(char fontnr, short offset, unsigned char ch) {
    if (fontnr!=0) {
        return;
    }
    if (offset>FONT_RAMSIZE) {
        return;
    }
    font_0[offset] = ch;
}

short font_findheader(char fontnr, unsigned char ch) {
    unsigned short p = 0;
    unsigned char flags = font_readbyte(fontnr,p);

    while(flags!=0) {
        unsigned char start = font_readbyte(fontnr,p+1);
        unsigned char count = flags >>4;
        unsigned char width = (flags & 0x7) +1;

        if (ch >= start && ch < start+count) {
            /* we have found the right header */
            return p;
        }

        /* not this header, skip to the next one */
        p += 2; /* skip header */
        p += (count*width); /* skip data */

        flags = font_readbyte(fontnr,p);
    }

    /* didnt find a header before we fell off the end of the list */
    return -1;
}

unsigned char font_getwidth(unsigned char ch) {
    short p;

    char fontnr = 0;
    p=font_findheader(fontnr,ch);
    if (p==-1) {
        fontnr++;
        p=font_findheader(fontnr,ch);
    }
    /* FIXME - if another font is added, this needs to turn into a loop */

    /* TODO - do missing chars have nonzero width? */
    if (p==-1) {
        return 0;
    }

    unsigned char flags = font_readbyte(fontnr,p);
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
    short p;

    char fontnr = 0;
    p=font_findheader(fontnr,ch);
    if (p==-1) {
        fontnr++;
        p=font_findheader(fontnr,ch);
    }
    /* FIXME - if another font is added, this needs to turn into a loop */

    if (p==-1) {
        /* missing chars have blank data */
        return 0;
    }

    unsigned char flags = font_readbyte(fontnr,p);
    unsigned char count = flags >>4;
    unsigned char width = (flags & 0x7) +1;
    unsigned char start = font_readbyte(fontnr,p+1);

    if (col >= width) {
        /* anything out of bounds is padding */
        return 0;
    }

    p+=2; /* skip header */
    p+= (ch - start)*width +col;
    return font_readbyte(fontnr,p);
}
