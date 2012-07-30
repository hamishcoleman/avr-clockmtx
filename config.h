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
	unsigned long	offset;		/* offset from UTC for this timezone */
	unsigned char	cal;		/* clock crystal calibration value */
};

extern struct config config;

int config_load(void);
void config_dirty(void);
int config_save_if_dirty(void);

#endif
