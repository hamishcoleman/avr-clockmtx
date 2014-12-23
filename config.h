/*
 * Non volatile configuration management
 *
 */
#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_MAGIC	0xc10c		/* "Clock" */

struct config {
	unsigned int	magic;		/* CONFIG_MAGIC */
	unsigned long	serial;		/* serial number of this unit */
	unsigned long	last_time;	/* last time config saved */
	unsigned char	tz[8];		/* asciiz name for this timezone */
	long	offset;			/* offset from UTC for this timezone */
	unsigned char	cal;		/* onboard RC oscillator calibration value */
        /* TODO: drift or 32.768Khz trimming value */
};

extern struct config config;

int config_load(void);
void config_dirty(void);
int config_save_if_dirty(void);

#endif

