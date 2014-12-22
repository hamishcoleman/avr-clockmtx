/*
 * calibrate the RC clock
 *
 * lifted from AVR055, which has no clear licence details :-(
 */

#include <avr/io.h>

#define ASYNC_TIMER                        AS2
#define NO_PRESCALING                      CS20
#define ASYNC_TIMER_CONTROL_REGISTER       TCCR2
#define ASYNC_TIMER_CONTROL_UPDATE_BUSY    TCR2UB
#define OUTPUT_COMPARE_UPDATE_BUSY         OCR2UB
#define TIMER_UPDATE_BUSY                  TCN2UB
#define TIMER                              TCNT2
#define LOOP_CYCLES                        6

#define EXTERNAL_TICKS 100
#define CALIBRATION_FREQUENCY 2000000
#define XTAL_FREQUENCY 32768
#define DEFAULT_OSCCAL 0x80
#define NOP() __asm__ volatile("nop")

unsigned int Counter(void){
  unsigned int cnt;

  cnt = 0;            // Reset counter
  TIMER = 0x00;       // Reset async timer/counter

  while (ASSR & ((1<<OUTPUT_COMPARE_UPDATE_BUSY)|(1<<TIMER_UPDATE_BUSY)|(1<<ASYNC_TIMER_CONTROL_UPDATE_BUSY)));
                      // Wait until async timer is updated
                      // (Async Status reg. busy flags).

  do{                 // cnt++: Increment counter - the add immediate
                      // to word (ADIW) takes 2 cycles of code.
    cnt++;            // Devices with async TCNT in I/0 space use 1 cycle
                      // reading, 2 for devices with async TCNT in extended
                      // I/O space
  } while (TIMER < EXTERNAL_TICKS);
                      // CPI takes 1 cycle, BRCS takes 2 cycles, resulting in:
                      // 2+1(or 2)+1+2=6(or 7) CPU cycles
  return cnt;         // NB! Different compilers may give different CPU cycles!
}                     // Until 32.7KHz (XTAL FREQUENCY) * EXTERNAL TICKS


void calibraterc(void) {
    unsigned int count;
    unsigned char cycles = 0x80;
    unsigned int countVal;

    countVal = ((EXTERNAL_TICKS*CALIBRATION_FREQUENCY)/(XTAL_FREQUENCY*LOOP_CYCLES));
    OSCCAL = DEFAULT_OSCCAL;
    NOP();

    ASSR |= (1<<ASYNC_TIMER); \
    ASYNC_TIMER_CONTROL_REGISTER = (1<<NO_PRESCALING);

    do{
        count = Counter();
        if (count > countVal)
            OSCCAL--;
        NOP();
        if (count < countVal)
            OSCCAL++;
        NOP();
        if (count == countVal)
            cycles=1;
    } while(--cycles);

}

