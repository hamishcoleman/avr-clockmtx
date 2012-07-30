/*
 * Some simple routines to handle serial IO
 *
 */
#ifndef SERIAL_H
#define SERIAL_H

void serial_init(unsigned int);

void serial_putc(unsigned char);

void serial_write(unsigned char*, int);

#endif
