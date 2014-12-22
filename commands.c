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
		case 'c':	/* Get calibration */
			serial_putc('C');
			itoa(config.cal,p,10);
			serial_puts(p);
			break;
		case 'T':	/* Set Time */
			/* NOTE: 32bit value, set from interrupt context */
			time = atol(&input[1]);
		case 't':	/* Get Time */
			serial_putc('T');
			ultoa(time,p,10);
			serial_puts(p);
			break;
		case 'Z':	/* Set TZ name */
			strncpy(config.tz,&input[1],sizeof(config.tz));
			config_dirty();
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
		case 'o':	/* Get Offset */
			serial_putc('O');
			serial_puts(config.tz);
			serial_putc(',');
			ltoa(config.offset,p,10);
			serial_puts(p);
			break;
	}
	serial_putc('\r');
	serial_putc('\n');
}

