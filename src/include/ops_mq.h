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

#define QUEUE_NAME_MAIN		"main"
//#define QUEUE_NAME_UDS_DB	"udsdb"
//#define QUEUE_NAME_MAIN_DB	"maindb"
#define QUEUE_NAME_UDS_WWW	"udswww"
#define QUEUE_NAME_SYSCMD	"syscmd"
#define QUEUE_NAME_SYSINIT	"sysinit"
#define QUEUE_NAME_RFBSERVER	"rfbserver"
#define QUEUE_NAME_RFBCLIENT	"rfbclient"

struct msg_t {
	//struct msg_hdr_t hdr;
	uint16_t data_size;
	uint8_t fn;
	uint8_t cmd;
	uint8_t status;
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
	//int (*get_from) (uint8_t * id, void * msg);
	//int (*set_to) (uint8_t * id, void * msg);
	int (*get_from) (uint8_t * id, struct queue_msg_t * msg);
	int (*set_to) (uint8_t * id, struct queue_msg_t * msg);
	int (*destroy) (uint8_t * id);
};

struct ops_mq_t *get_mq_instance();
void del_mq_instance();
#endif
