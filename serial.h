/*
 * Some simple routines to handle serial IO
 *
 */
#ifndef SERIAL_H
#define SERIAL_H

#include <avr/pgmspace.h>

/* State flags */
#define SERIAL_ECHO     0x01
#define SERIAL_INDLE    0x02
#define SERIAL_INPACKET 0x04


/* special character codes */
#define SERIAL_DLE      0x10    /* data link escape */

#define SERIAL_STX      0x02    /* Start of packet char */
#define SERIAL_ETX      0x03    /* End of packet data+checksum */
#define SERIAL_EOT      0x04    /* End of packet data only */

#define SERIAL_NAK      0x15    /* "something went wrong" */

void serial_init(unsigned int);

void serial_putc(unsigned char);

void serial_write(unsigned char*, int);
void serial_puts(unsigned char *);
void serial_puts_P(PGM_P);

void handle_rx_packet(unsigned char *,unsigned char);

#endif
