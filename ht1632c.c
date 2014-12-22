/*
 * Driver routines for a ht1632c LED matrix
 */




#include <avr/pgmspace.h>
#include <avr/io.h>


#define byte uint8_t
#define word uint16_t

//pins and macros

#define HTport   PORTB
#define HTddr    DDRB
#define HTstrobe 3
#define HTclk    4
#define HTdata   5

#define HTclk0    HTport&=~(_BV(HTclk))
#define HTclk1    HTport|= (_BV(HTclk))
#define HTstrobe0 HTport&=~(_BV(HTstrobe))
#define HTstrobe1 HTport|= (_BV(HTstrobe))
#define HTdata0   HTport&=~(_BV(HTdata))
#define HTdata1   HTport|= (_BV(HTdata))
        // set as output and all high


char leds[32];  //the screen array, 1 byte = 1 column, left to right, lsb at top.


#define HTstartsys   0b100000000010 //start system oscillator
#define HTstopsys    0b100000000000 //stop sytem oscillator and LED duty    <default
#define HTsetclock   0b100000110000 //set clock to master with internal RC  <default
#define HTsetlayout  0b100001000000 //NMOS 32*8 // 0b100-0010-ab00-0  a:0-NMOS,1-PMOS; b:0-32*8,1-24*16   default:ab=10
#define HTledon      0b100000000110 //start LEDs
#define HTledoff     0b100000000100 //stop LEDs    <default
#define HTsetbright  0b100101000000 //set brightness b=0..15  add b<<1  //0b1001010xxxx0 xxxx:brightness 0..15=1/16..16/16 PWM
#define HTblinkon    0b100000010010 //Blinking on
#define HTblinkoff   0b100000010000 //Blinking off  <default
#define HTwrite      0b1010000000   // 101-aaaaaaa-dddd-dddd-dddd-dddd-dddd-... aaaaaaa:nibble adress 0..3F   (5F for 24*16)

//ADRESS: MSB first
//DATA: LSB first     transferring a byte (msb first) fills one row of one 8*8-matrix, msb left, starting with the left matrix
//timing: pull strobe LOW, bits evaluated at rising clock edge, strobe high
//commands can be queued: 100-ccccccccc-ccccccccc-ccccccccc-... (ccccccccc: without 100 at front)
//setup: cast startsys, setclock, setlayout, ledon, brightness+(15<<1), blinkoff



void HTsend(word data, byte bits) {  //MSB first
  word bit=((word)1)<<(bits-1);
  while(bit) {
    HTclk0;
    if (data & bit) HTdata1; else HTdata0;
    HTclk1;
    bit>>=1;
  }
}

void HTcommand(word data) {
  HTstrobe0;
  HTsend(data,12);
  HTstrobe1;
}

void HTsendscreen(void) {
  HTstrobe0;
  HTsend(HTwrite,10);
  for (byte mtx=0;mtx<4;mtx++)  //sending 8x8-matrices left to right, rows top to bottom, MSB left
    for (byte row=0;row<8;row++) {  //while leds[] is organized in columns for ease of use.
      byte q=0;
      for (byte col=0;col<8;col++)  q = (q<<1) | ( (leds[col+(mtx<<3)]>>row)&1 ) ;
      HTsend(q,8);
    }
  HTstrobe1;
}

void HTpinsetup() {
	HTddr |=_BV(HTstrobe)|_BV(HTclk)|_BV(HTdata); HTport|=_BV(HTstrobe)|_BV(HTclk)|_BV(HTdata);
}

void HTsetup() {  //setting up the display
  HTcommand(HTstartsys);
  HTcommand(HTledon);
  HTcommand(HTsetclock);
  HTcommand(HTsetlayout);
  HTcommand(HTsetbright+(8<<1));
  HTcommand(HTblinkoff);
}

void HTbrightness(byte b) {
  HTcommand(HTsetbright + ((b&15)<<1) );
}


/*
 * Holtek HT1632

CS/     high disables and resets io circuit, low enables comms
WR/     write clock input - data is clocked in from DATA on rising edge
RD/     read clock input - data is clocked out from DATA on falling edge
DATA

After selecting chip, send operation, then successive commands can be sent
with the same operation until the chip is deselected.

Operation
110     Read
101     Write
100     Command

Commands
100-0000-0000-X SYS-DIS         system OSC and LED duty cycle both off
100-0000-0001-X SYS-EN          system OSC on
100-0000-0010-X LED off
100-0000-0011-X LED on
100-0000-1000-X BLINK OFF
100-0000-1001-X BLINK ON
100-0001-00XX-X SLAVE MODE
100-0001-01XX-X MASTER MODE
100-0001-10XX-X RC              (Onboard RC timebase)
100-0001-11XX-X EXT CLK
100-0010-abXX-X COM options     a:0=N-MOS/1=P-MOS, b:0=8-COM/1=16-COM
100-101X-dddd-X PWM Duty        dddd = 1 - 16 (out of 16)

Write
101-aaaaaaadddd

 */
