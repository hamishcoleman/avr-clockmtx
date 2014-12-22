#include <avr/pgmspace.h>



PROGMEM const unsigned char bigdigits[12][6] = {
  {0b01111110,
    129,129,129,129,126},            // 0
  {128,132,130,255,128,128},            // 1
  {130,193,161,145,137,134},            // 2
  { 66,129,137,137,137,118},            // 3
  {0x3f,0x20,0x20,0xfc,0x20,0x20},      // 4
  {0x4f,0x89,0x89,0x89,0x89,0x71},      // 5
  {0x7e,0x89,0x89,0x89,0x89,0x72},      // 6
  {0x03,0x01,0xc1,0x31,0x0d,0x03},      // 7
  {0x76,0x89,0x89,0x89,0x89,0x76},      // 8
  {0x46,0x89,0x89,0x89,0x89,0x7e},      // 9
  {0x00,0x66,0x66,0x00,0x00,0x00},      // :
  {0x00,0x00,0x00,0x00,0x00,0x00},      // ;, but used as a " "
};

#if 0
  {0b01111110,
   0b10000001,
   0b10000001,
   0b10000001,
   0b10000001,
   0b01111110}, // 0

  {0b10000000,
   0b10000100,
   0b10000010,
   0b11111111,
   0b10000000,
   0b10000000}, // 1

  {0b10000010,
   0b11000001,
   0b10100001,
   0b10010001,
   0b10001001,
   0b10000110}, // 2

  {0b01000010,
   0b10000001,
   0b10001001,
   0b10001001,
   0b10001001,
   0b01110110}, // 3

  {0b00111111,
   0b00100000,
   0b00100000,
   0b11111100,
   0b00100000,
   0b00100000}, // 4

  {0x4f,
   0x89,
   0x89,
   0x89,
   0x89,
   0x71},
         // 5
  {0x7e,
   0x89,
   0x89,
   0x89,
   0x89,
   0x72},
         // 6
  {0x03,
   0x01,
   0xc1,
   0x31,
   0x0d,
   0x03},
         // 7
  {0x76,
   0x89,
   0x89,
   0x89,
   0x89,
   0x76},
         // 8
  {0x46,
   0x89,
   0x89,
   0x89,
   0x89,
   0x7e},
         // 9
#endif

/*
 * Need to support varying widths - otherwise the colon looks wrong
 * Also, need to support an empty column after the data, otherwise
 * the digits run together
 *
 */

unsigned char font_getwidth(unsigned char ch) {
    return 6;
}

unsigned char font_getdata(unsigned char ch,unsigned char col) {
    return pgm_read_byte(&bigdigits[ (ch-'0') ][col]);
}
