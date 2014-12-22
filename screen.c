/*
 * Handle screen
 */

#include <avr/pgmspace.h>
#include <stdint.h>

#include "clock.h"
#include "ht1632c.h"
#include "font.h"
#include "config.h"

#define byte uint8_t
#define word uint16_t

void screen_init(void) {
    HTpinsetup();
    HTsetup();

    /* Show an initial test pattern */
    for (byte i=0;i<32;i++) {
        leds[i]=0b01010101<<(i%2);
    }
    HTsendscreen();
}

//-------------------------------------------------------------------------------------- clock render ----------

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
    

  byte col=0;
  for (byte i=0;i<6;i++) leds[col++]=pgm_read_byte(&bigdigits[hour/10][i]);
  leds[col++]=0;
  for (byte i=0;i<6;i++) leds[col++]=pgm_read_byte(&bigdigits[hour%10][i]);
  leds[col++]=0;
  if (sec%2) {leds[col++]=0x66;leds[col++]=0x66;} else {leds[col++]=0; leds[col++]=0;}
  leds[col++]=0;
  for (byte i=0;i<6;i++) leds[col++]=pgm_read_byte(&bigdigits[minute/10][i]);
  leds[col++]=0;
  for (byte i=0;i<6;i++) leds[col++]=pgm_read_byte(&bigdigits[minute%10][i]);
  leds[col++]=0;
  leds[col++]=0;
    HTsendscreen();
}

