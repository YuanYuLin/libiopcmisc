#ifndef OPS_MISC_H
#define OPS_MISC_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

struct ops_misc_t {
	void (*init) (void);
	void (*show_all) (void);
	int (*get_dbpath) (uint8_t * val);
	int (*get_dbtype) (uint32_t * val);
	int (*get_logmask) (uint8_t * val);
};

#define LUA_CFG		"/etc/lua/cfg.lua"

struct ops_misc_t *get_misc_instance();
void del_misc_instance();
#endif
