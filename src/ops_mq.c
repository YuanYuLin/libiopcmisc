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

static int get(mqd_t mqd, struct queue_msg_t* msg)
{
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

static int set(mqd_t mqd, struct queue_msg_t* msg)
{
    //uint32_t size = sizeof(struct msg_hdr_t) + msg->hdr.data_size;
    uint32_t size = sizeof(struct queue_msg_t) - MAX_MSG_DATA_SIZE + msg->msg.data_size;
    int ret = 0;

    ret = mq_send(mqd, (const char*)msg, size, 0);
    return ret;
}

static void init()
{
    //mqd_list[i] = create(id);
}

static void show_all()
{
}

static int get_from(uint8_t * id, struct queue_msg_t *msg)
{
	int ret = 0;
	mqd_t mqd = create(id);
	ret = get(mqd, msg);
	mq_close(mqd);
	return ret;
}

static int set_to(uint8_t * id, struct queue_msg_t *msg)
{
	int ret = 0;
	mqd_t mqd = create(id);
	ret = set(mqd, msg);
	mq_close(mqd);
	return ret;
}


static json_reader_t* create_reader(uint8_t* json_str)
{
	json_reader_t* jobj = json_tokener_parse(json_str);
	return jobj;
}

static uint8_t* get_string(json_reader_t* _reader, uint8_t* key, uint8_t* defval)
{
	struct json_object* reader = (struct json_object*)_reader;
	uint8_t* val = NULL;
	struct json_object* jobj_val = NULL;
	if(json_object_object_get_ex(reader, key, &jobj_val)) {
		val = (uint8_t*)json_object_get_string(jobj_val);
	} else {
		val = defval;
	}
	return val;
}

static int get_int(json_reader_t* _reader, uint8_t* key, int defval)
{
	struct json_object* reader = (struct json_object*)_reader;
	int val = 0;
	struct json_object* jobj_val;
	if(json_object_object_get_ex(reader, key, &jobj_val)) {
		val = (int)json_object_get_int(jobj_val);
	} else {
		val = defval;
	}
	return val;
}

static json_writer_t* create_writer()
{
	json_writer_t* jobj = json_object_new_object();
	return jobj;
}

static void set_string(json_writer_t* _writer, uint8_t* key, uint8_t* val)
{
	struct json_object* writer = (struct json_object*)_writer;
	json_object_object_add(writer, key, json_object_new_string(val));
}

static void set_int(json_writer_t* _writer, uint8_t* key, int val)
{
	struct json_object* writer = (struct json_object*)_writer;
	json_object_object_add(writer, key, json_object_new_int(val));
}

static uint32_t to_bytes(json_writer_t* _writer, uint8_t* data)
{
	struct json_object* writer = (struct json_object*)_writer;
	uint8_t* ptr = NULL;
	uint32_t ptr_len = 0;
	ptr = (uint8_t*)json_object_to_json_string(writer);
	ptr_len = strlen(ptr);
	memcpy(data, ptr, ptr_len);
	return ptr_len;
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
		obj->create_json_reader = create_reader;
		obj->get_json_string = get_string;
		obj->get_json_int = get_int;
		obj->create_json_writer = create_writer;
		obj->set_json_string = set_string;
		obj->set_json_int = set_int;
		obj->out_json_to_bytes = to_bytes;

	}
	return obj;
}

void del_mq_instance()
{
	if (obj)
		free(obj);
}
