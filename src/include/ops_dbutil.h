#ifndef OPS_DBUTIL_H
#define OPS_DBUTIL_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

struct ops_dbutil_t {
	uint32_t(*parse_value) (uint8_t * dst_val, uint8_t * val_str,
				uint32_t val_str_len, uint8_t * type);
	uint32_t(*format_value) (uint8_t * dst_val_str, uint8_t * val,
				 uint32_t val_len, uint8_t * type);
};

struct ops_dbutil_t *get_dbutil_instance();
void del_dbutil_instance();
#endif
