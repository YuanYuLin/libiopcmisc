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

#define QUEUE_NAME_MAIN		"01"//"main"
//#define QUEUE_NAME_UDS_DB	"02"//"udsdb"
//#define QUEUE_NAME_MAIN_DB	"03"//"maindb"
#define QUEUE_NAME_UDS_WWW	"04"//"udswww"
#define QUEUE_NAME_SYSCMD	"05"//"syscmd"
#define QUEUE_NAME_SYSINIT	"06"//"sysinit"
#define QUEUE_NAME_RFBSERVER	"07"//"rfbserver"
#define QUEUE_NAME_RFBCLIENT	"08"//"rfbclient"
#define QUEUE_NAME_SHELLMGR	"09"//"shellmgr"

struct msg_t {
	//struct msg_hdr_t hdr;
	uint8_t resv;
	uint8_t fn;
	uint8_t cmd;
	uint8_t status;
	uint32_t crc32;
	uint32_t data_size;
	uint8_t data[MAX_MSG_DATA_SIZE];
} __attribute__ ((packed));

struct queue_msg_t {
	uint32_t index;
	uint32_t magic;
	uint8_t src[8];
	uint8_t dst[8];
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
