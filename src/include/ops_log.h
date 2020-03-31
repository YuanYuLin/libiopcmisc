#ifndef OPS_LOG_H
#define OPS_LOG_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

struct ops_log_t {
	void (*init) (void);
	void (*show_all) (void);
	void (*info)  (uint8_t mask, const char* file, const char* func, uint32_t line, const char *fmt, ...);
	void (*debug) (uint8_t mask, const char* file, const char* func, uint32_t line, const char *fmt, ...);
	void (*error) (uint8_t mask, const char* file, const char* func, uint32_t line, const char *fmt, ...);
};

struct ops_log_t *get_log_instance();
void del_log_instance();
#endif
