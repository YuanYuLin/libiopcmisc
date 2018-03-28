#include <mqueue.h>
#include <errno.h>
#include <json.h>

#include "ops_mq.h"
#include "ops_log.h"

#define MQ_PATH_STR_LEN	30
#define MQ_PATH_NAME	"/MQ%s"

//static mqd_t mqd_list[10] = { 0 };

static int create(uint8_t * id)
{
	struct ops_log_t* log = get_log_instance();
	char path[MQ_PATH_STR_LEN] = { 0 };
	mqd_t mqd;

	memset(path, 0, sizeof(path));
	sprintf(path, MQ_PATH_NAME, id);
	mqd =
	    mq_open(path, O_RDWR | O_CREAT | O_EXCL,
		    S_IRWXU | S_IRWXG | S_IRWXO, NULL);

	if ((mqd < 0) && (errno == EEXIST)) {
		mqd = mq_open(path, O_RDWR, NULL);
	}

	if (mqd < 0) {
		log->error(0x01, "MQ created ERROR :(%d) %s\n", errno, path);
		return -1;
	}
	return mqd;
}

static int destroy(uint8_t * id)
{
	char path[MQ_PATH_STR_LEN];
	memset(path, 0, sizeof(path));
	sprintf(path, MQ_PATH_NAME, id);
	mq_unlink(path);
	return 0;
}

static int get_qmsg(mqd_t mqd, struct queue_msg_t* msg)
{
	struct ops_log_t* log = get_log_instance();
    struct mq_attr attr;
    ssize_t msg_size = 0;
    char *ptr = (char*)msg;

    mq_getattr(mqd, &attr);
    memset(msg, 0, sizeof(struct queue_msg_t));

    msg_size = mq_receive(mqd, ptr, attr.mq_msgsize, NULL);
    if(msg_size < 0) {
        log->error(0x01, "MQ_RCV:%d:%ld\n", errno, attr.mq_msgsize);
        return 0;
    }

    return msg_size;
}
#if 0
static int get_syscmd(mqd_t mqd, struct syscmd_msg_t* msg)
{
	struct ops_log_t* log = get_log_instance();
    struct mq_attr attr;
    ssize_t msg_size = 0;
    char *ptr = (char*)msg;

    mq_getattr(mqd, &attr);
    memset(msg, 0, sizeof(struct syscmd_msg_t));

    msg_size = mq_receive(mqd, ptr, attr.mq_msgsize, NULL);
    if(msg_size < 0) {
        log->error(0x01, "MQ_RCV:%d:%ld\n", errno, attr.mq_msgsize);
        return 0;
    }

    return msg_size;
}
#endif
static int set_qmsg(mqd_t mqd, struct queue_msg_t* msg)
{
    //uint32_t size = sizeof(struct msg_hdr_t) + msg->hdr.data_size;
    uint32_t size = sizeof(struct queue_msg_t) - MAX_MSG_DATA_SIZE + msg->msg.data_size;
    int ret = 0;

    ret = mq_send(mqd, (const char*)msg, size, 0);
    return ret;
}
#if 0
static int set_syscmd(mqd_t mqd, struct syscmd_msg_t* msg)
{
    //uint32_t size = sizeof(struct msg_hdr_t) + msg->hdr.data_size;
    uint32_t size = sizeof(struct syscmd_msg_t) - ARGV_LEN + strlen(msg->param);
    int ret = 0;

    ret = mq_send(mqd, (const char*)msg, size, 0);
    return ret;
}
#endif
static void init()
{
    //mqd_list[i] = create(id);
}

static void show_all()
{
}

static int get_from(uint8_t * id, struct queue_msg_t *msg)
//static int get_from(uint8_t * id, void *msg)
{
	int ret = 0;
	mqd_t mqd = create(id);
	//if(strcmp(id, QUEUE_NAME_SYSCMD) == 0) {
	//	ret = get_syscmd(mqd, msg);
	//} else {
	    ret = get_qmsg(mqd, msg);
	//}
	mq_close(mqd);
	return ret;
}

static int set_to(uint8_t * id, struct queue_msg_t *msg)
//static int set_to(uint8_t * id, void *msg)
{
	int ret = 0;
	mqd_t mqd = create(id);
	//if(strcmp(id, QUEUE_NAME_SYSCMD) == 0) {
	//	ret = set_syscmd(mqd, msg);
	//} else {
		ret = set_qmsg(mqd, msg);
	//}
	mq_close(mqd);
	return ret;
}

static struct ops_mq_t *obj = NULL;
struct ops_mq_t *get_mq_instance()
{
	if (!obj) {
		obj = malloc(sizeof(struct ops_mq_t));
		obj->init = init;
		obj->show_all = show_all;
		obj->create = create;
		obj->get_from = get_from;
		obj->set_to = set_to;
		obj->destroy = destroy;
	}
	return obj;
}

void del_mq_instance()
{
	if (obj)
		free(obj);
}
