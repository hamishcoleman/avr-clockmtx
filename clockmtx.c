// Clock programm for DX 32*8 LED Matrix + HT1632C + ATmega8; 
// DrJones 2012
//
// button1: adjust time forward, keep pressed for a while for fast forward
// button2: adjust time backward, keep pressed for a while for fast backward
// button3: adjust brightness in 4 steps





#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include "serial.h"
#include "clock.h"
#include "ht1632c.h"
#include "config.h"
#include "font.h"

#define byte uint8_t
#define word uint16_t



#define key1 ((PIND&_BV(7))==0)
#define key2 ((PIND&_BV(6))==0)
#define key3 ((PIND&_BV(5))==0)
#define keysetup() do{ DDRD&=0xff-_BV(7)-_BV(6)-_BV(5); PORTD|=_BV(7)+_BV(6)+_BV(5); }while(0)  //input, pull up

//-------------------------------------------------------------------------------------- clock render ----------

void renderclock(void) {
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
}



byte changing, bright=3;
byte brights[4]={0,2,6,15}; //brightness levels

int main(void) {  //==================================================================== main ==================

  HTpinsetup();
  HTsetup();
  keysetup();

  config_load();
  clock_init();

  serial_init(12); // at 2Mhz, this gives 9k6bps (at 1Mhz, 4k8)
  //serial_init(5); // at 2Mhz, this gives 20.8Kbps, which works for me as 19.2K
  //serial_init(2); // at 2Mhz, this is 38.4Kbps
  //serial_init(1); // at 2Mhz, this is 57.6Kbps
  //serial_init(0); // at 2Mhz, this is 115.2Kbps
  serial_write("Hello World\r\n",13);

  for (byte i=0;i<32;i++) leds[i]=0b01010101<<(i%2);  HTsendscreen();


  while(1){ 
         if (key1) {if (changing>250) incsec(20); else {changing++; incsec(1);} }
    else if (key2) {if (changing>250) decsec(20); else {changing++; decsec(1);} }
    else if (key3) {if (!changing) {changing=1; bright=(bright+1)%4; HTbrightness(brights[bright]);} } //only once per press
    else changing=0;

    if (clockhandler()) {
	renderclock();
	HTsendscreen();
	config_save_if_dirty();
    }
  }
  return(0);
}//main
