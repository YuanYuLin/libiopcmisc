#ifndef OPS_MQ_H
#define OPS_MQ_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_MSG_DATA_SIZE	0x08FF
#define json_reader_t   void
#define json_writer_t   void

struct msg_t {
	//struct msg_hdr_t hdr;
	uint16_t data_size;
	uint8_t fn;
	uint8_t cmd;
	uint32_t crc32;
	uint8_t data[MAX_MSG_DATA_SIZE];
} __attribute__ ((packed));

struct queue_msg_t {
	uint32_t index;
	uint32_t magic;
	uint8_t src[20];
	uint8_t dst[20];
	struct msg_t msg;
} __attribute__ ((packed));

struct ops_mq_t {
	void (*init) (void);
	void (*show_all) (void);
	int (*create) (uint8_t * id);
	int (*get_from) (uint8_t * id, struct queue_msg_t * msg);
	int (*set_to) (uint8_t * id, struct queue_msg_t * msg);
	int (*destroy) (uint8_t * id);
	json_reader_t* (*create_json_reader)(uint8_t* json_str);
	uint8_t* (*get_json_string)(json_reader_t* reader, uint8_t* key, uint8_t* defval);
	int (*get_json_int)(json_reader_t* reader, uint8_t* key, int defval);
	json_writer_t* (*create_json_writer)();
	void (*set_json_string)(json_writer_t* writer, uint8_t* key, uint8_t* val);
	void (*set_json_int)(json_writer_t* writer, uint8_t* key, int val);
	uint32_t (*out_json_to_bytes)(json_writer_t* writer, uint8_t* data);
};

struct ops_mq_t *get_mq_instance();
void del_mq_instance();
#endif
