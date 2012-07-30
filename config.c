/*
 * Non volatile configuration management functions
 *
 */

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "config.h"
#include "clock.h"

struct config config;
struct config EEMEM config_saved;

const struct config PROGMEM defaults = {
	.magic	= 0xc10c,
	.serial	= 0,
	.last_time = 0,
	.tz	= "UTC",
	.offset	= 0,
	.cal	= 128,
};

int config_load(void) {
	eeprom_busy_wait();
	eeprom_read_block(&config,&config_saved,sizeof(config));
	if (config.magic != CONFIG_MAGIC) {
		memcpy_P(&config,&defaults,sizeof(config));
		return -1;
	}
	return 0;
}

/* Mark the ram config as 'dirty' */
void config_dirty(void) {
	config.last_time = -1;
}

/* Save the ram config, if it is dirty */
int config_save_if_dirty(void) {
	if (config.last_time != -1) {
		return -1;
	}

	config.last_time = time;
	eeprom_busy_wait();
	eeprom_write_block(&config,&config_saved,sizeof(config));
	return 0;
}
