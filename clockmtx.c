// Clock programm for DX 32*8 LED Matrix + HT1632C + ATmega8; 
// DrJones 2012
//
// button1: adjust time forward, keep pressed for a while for fast forward
// button2: adjust time backward, keep pressed for a while for fast backward
// button3: adjust brightness in 4 steps





#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdlib.h>

#include "serial.h"
#include "clock.h"
#include "config.h"
#include "calibraterc.h"
#include "version.h"
#include "screen.h"
#include "font.h"
#include "ht1632c.h"

#define byte uint8_t

#define key1 ((PIND&_BV(7))==0)
#define key2 ((PIND&_BV(6))==0)
#define key3 ((PIND&_BV(5))==0)
#define keysetup() do{ DDRD&=0xff-_BV(7)-_BV(6)-_BV(5); PORTD|=_BV(7)+_BV(6)+_BV(5); }while(0)  //input, pull up



byte changing, bright=3;
byte brights[4]={0,2,6,15}; //brightness levels

int main(void) {  //==================================================================== main ==================

    config_load();
    // TODO: Possibly set OSCCAL from config

    /* Use watch crystal to set OSCCAL */
    calibraterc();

    /* Record the new OSCCAL in our config */
    config.cal = OSCCAL;

    screen_init();
    clock_init();
    font_init();
    serial_init(12); // at 2Mhz, this gives 9k6bps (at 1Mhz, 4k8)

    keysetup();

    serial_puts_P(version);
    serial_puts(" Hello World\r\n");

    /* TODO: show the version here */
    screen_puts("HELLO WORLD");
    screen_mode=SCREEN_MODE_TEXT;
    screen_mode_until=time+10;

    unsigned long time_last=200;

  while(1){ 
    if (key1) {
        if (!changing) {
            changing=1;
            screen_puts(config.tz);
            screen_mode=SCREEN_MODE_TEXT;
            screen_mode_until=time+10;
        }
    } else if (key2) {
        if (!changing) {
            changing=1;
            screen_puts("ClockMTX");
            screen_mode=SCREEN_MODE_TEXT;
            screen_mode_until=time+10;
        }
    } else if (key3) {
        if (!changing) {
            changing=1;
            bright=(bright+1)%4;
            HTbrightness(brights[bright]);
        }
        //only once per press
    } else {
        changing=0;
    }

    if (time!=time_last) {
        /* Run the rendering only once a second */
	time_last=time;

        if (time > screen_mode_until) {
            screen_mode=0;
        }

        if (screen_mode==0) {
            screen_showclock(time);
        }

	config_save_if_dirty();
    }
  }
  return(0);
}//main

