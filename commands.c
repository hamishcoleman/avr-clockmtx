/*
 * Handle packet commands
 *
 */

#include <stdlib.h>
#include <string.h>

#include "serial.h"
#include "clock.h"
#include "config.h"
#include "version.h"
#include "screen.h"
#include "font.h"

unsigned char hex_atoi(unsigned char *s) {
    unsigned char digit;
    unsigned char byte;

    digit = *s++;
    if (!digit) { return 0; }
    digit -= '0';
    if (digit>9) { digit -= 7; }

    byte = digit<<4;

    digit = *s++;
    if (!digit) { return 0; }
    digit -= '0';
    if (digit>9) { digit -= 7; }

    byte |= digit;

    return byte;
}

/*
 * called by the serial driver to handle a recieved packet
 */
void handle_rx_packet(unsigned char *input,unsigned char size) {

    if (size < 32) {
        /* Ensure the packet has a termination null */
        input[size]=0;
    } else {
        return;
    }

	char buf[11];
	char *p = buf;
	serial_putc('=');
        unsigned char cmd = input[0];
	switch(cmd) {
		case 'p':	/* PING */
			serial_putc('P');
			break;
		case 'i':	/* ID */
			serial_putc('I');
			serial_puts_P(version);
			/* TODO - output more */
			break;
		case 'C':	/* Set calibration */
			config.cal = atoi(&input[1]);
			config_dirty();
                        /* Falls through */
		case 'c':	/* Get calibration */
			serial_putc('C');
			itoa(config.cal,p,10);
			serial_puts(p);
			break;
		case 'T':	/* Set Time */
			/* NOTE: 32bit value, set from interrupt context */
			time = atol(&input[1]);
                        /* Falls through */
		case 't':	/* Get Time */
			serial_putc('T');
			ultoa(time,p,10);
			serial_puts(p);
			break;
		case 'Z':	/* Set TZ name */
			strncpy(config.tz,&input[1],sizeof(config.tz));
			config_dirty();
                        /* Falls through */
		case 'z':	/* Get TZ name */
			serial_putc('Z');
			serial_puts(config.tz);
			break;
		case 'O':	/* Set Offset */
                        {
                        unsigned char * p1 = strtok(&input[1],",");
			if (!strcmp(config.tz,p1)) {
				config.offset = atol(strtok(NULL,","));
				config_dirty();
			} else {
				/* Ignore commands not for us */
				break;
			}
                        }
                        /* Falls through */
		case 'o':	/* Get Offset */
			serial_putc('O');
			serial_puts(config.tz);
			serial_putc(',');
			ltoa(config.offset,p,10);
			serial_puts(p);
			break;
                case 'M':       /* Set message */
                        {
                        unsigned char * arg2 = strchrnul(&input[1],',');
                        *arg2++ = 0;
                        screen_mode = SCREEN_MODE_TEXT;
                        screen_mode_until = time+atoi(&input[1]);
                        screen_puts(arg2);
                        break;
                        }
                case 'F':       /* Set font0 data */
                        {
                        unsigned char * arg1 = strtok(&input[1],",");
                        short offset = atoi(arg1);
                        p = strtok(NULL,",");
                        while (*p) {
                            unsigned ch = hex_atoi(p);
                            font_writebyte(0,offset++,ch);
                            p+=2;
                        }
                        }
                        break;
                case 'f':       /* Get font0 data */
                        {
                        short offset = 0;
                        while (offset<FONT_RAMSIZE) {
                            itoa(font_readbyte(0,offset),p,16);
                            serial_puts(p);
                            serial_putc(',');
                            offset++;
                        }
                        }
                        break;
	}
	serial_putc('\r');
	serial_putc('\n');
}

