#ifndef OPS_MISC_H
#define OPS_MISC_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>

struct ops_misc_t {
	void (*init) (void);
	void (*show_all) (void);
	//int (*get_dbpath) (uint8_t * val);
	//int (*get_dbtype) (uint32_t * val);
	int (*get_logmask) (uint8_t * val);
	int (*syscmd)(uint8_t* cmd);
	uint8_t (*get_macaddress_by_interface)(uint8_t* interface, uint8_t* mac_address);
	uint8_t (*get_ipaddress_by_interface)(uint8_t* interface, uint8_t* ip_address);
	uint8_t (*is_file_exist)(uint8_t *path);
};

#define CMDLEN		0xff
#define LUA_CFG		"/etc/lua/cfg.lua"

struct ops_misc_t *get_misc_instance();
void del_misc_instance();
#endif
