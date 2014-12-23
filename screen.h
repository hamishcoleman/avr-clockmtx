/*
 * Handle the screen
 *
 */
#ifndef SCREEN_H
#define SCREEN_H

extern char screen_mode;
#define SCREEN_MODE_CLOCK       0
#define SCREEN_MODE_TEXT        1
extern unsigned long screen_mode_until; /* timestamp to show the clock */

void screen_init(void);
void screen_puts(unsigned char*);
void screen_showclock(unsigned long time);

#endif

