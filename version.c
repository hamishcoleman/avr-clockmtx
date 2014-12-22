/*
 * Use the generated version header to output the data
 */

#include <avr/pgmspace.h>

#include "serial.h"

#include "version.h"
#include "version-auto.h"

const unsigned char version[] PROGMEM = TARGET "-" VERSION;

