#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "ops_dbutil.h"

#define SIZE_STR_TYPE		4
#define SIZE_STR_VAL_LEN	4

/*
 * dbval : type + value string length + value string
 * bytes of type		: 4
 * bytes of value string length	: 4
 * bytes of value string	: N
 */
static uint32_t parse_value(uint8_t * dst_val, uint8_t * val_str,
			    uint32_t val_str_len, uint8_t * type)
{
	uint8_t str_type[SIZE_STR_TYPE] = { 0 };
	uint8_t str_val_len[SIZE_STR_VAL_LEN] = { 0 };
	uint32_t val_len = 0;
	uint8_t tmp_len = SIZE_STR_TYPE + SIZE_STR_VAL_LEN;

	if (val_str_len >= tmp_len) {
		memcpy(&str_type[0], val_str, SIZE_STR_TYPE);
		memcpy(&str_val_len[0], &val_str[SIZE_STR_TYPE],
		       SIZE_STR_VAL_LEN);
		val_len = strtoul(&str_val_len[0], NULL, 10);
		if (val_str_len == (tmp_len + val_len)) {
			memcpy(dst_val, &val_str[tmp_len], val_len);
		}
	}

	return val_len;
}

static uint32_t format_value(uint8_t * dst_val_str, uint8_t * val,
			     uint32_t val_len, uint8_t * type)
{
	uint32_t size = SIZE_STR_TYPE + SIZE_STR_VAL_LEN + val_len + 1;
	uint32_t format_size = 0;

	memset(dst_val_str, 0, size);
	sprintf(dst_val_str, "%4s%04d%s", type, val_len, val);
	format_size = strlen(dst_val_str);
	return format_size;
}

static struct ops_dbutil_t *obj = NULL;
struct ops_dbutil_t *get_dbutil_instance()
{
	if (!obj) {
		obj = malloc(sizeof(struct ops_dbutil_t));
		obj->parse_value = parse_value;
		obj->format_value = format_value;
	}
	return obj;
}

void del_dbutil_instance()
{
	if (obj)
		free(obj);
}
