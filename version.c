/*
 * Use the generated version header to output the data
 */

#include <avr/pgmspace.h>

#include "serial.h"

#include "version.h"
#include "version-auto.h"

const unsigned char version[] PROGMEM = TARGET "-" VERSION;

void serial_write_version(void) {
    const char *buf = version;
    char ch;
    while((ch = pgm_read_byte(buf++))) {
        serial_putc(pgm_read_byte(buf));
    }
}
