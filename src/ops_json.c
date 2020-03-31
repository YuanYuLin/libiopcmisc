#include <errno.h>
#include <json.h>

#include "ops_json.h"
#include "ops_log.h"

static int create(uint8_t * id)
{
	return 0;
}

static int destroy(uint8_t * id)
{
	return 0;
}

static void init()
{
}

static void show_all()
{
}

static json_reader_t* create_reader_by_file(uint8_t* file_path)
{
	json_reader_t* jobj = json_object_from_file(file_path);
	return jobj;
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
	val = defval;
	if(key == NULL) {
		val = (uint8_t*)json_object_get_string(reader);
	}
	if(json_object_object_get_ex(reader, key, &jobj_val)) {
		val = (uint8_t*)json_object_get_string(jobj_val);
	}
	return val;
}

static int get_int(json_reader_t* _reader, uint8_t* key, int defval)
{
	struct json_object* reader = (struct json_object*)_reader;
	int val = 0;
	struct json_object* jobj_val;
	val = defval;
	if(key == NULL) {
		val = (int)json_object_get_int(reader);
	}
	if(json_object_object_get_ex(reader, key, &jobj_val)) {
		val = (int)json_object_get_int(jobj_val);
	}
	return val;
}

static uint8_t get_boolean(json_reader_t* _reader, uint8_t* key, uint8_t defval)
{
	struct json_object* reader = (struct json_object*)_reader;
	uint8_t val = 0;
	struct json_object* jobj_val;
	val = defval;
	if(key == NULL) {
		val = (int)json_object_get_boolean(reader);
	}
	if(json_object_object_get_ex(reader, key, &jobj_val)) {
		val = (int)json_object_get_boolean(jobj_val);
	}
	return val;
}

static json_writer_t* create_writer()
{
	json_writer_t* jobj = json_object_new_object();
	return jobj;
}

static json_writer_t* create_array_writer()
{
	json_writer_t* jobj = json_object_new_array();
	return jobj;
}

static void set_array(json_writer_t* _array_writer, json_writer_t* _writer)
{
	struct json_object* array_writer = (struct json_object*)_array_writer;
	struct json_object* writer = (struct json_object*)_writer;
	json_object_array_add(array_writer, writer);
}

static json_reader_t* get_array(json_reader_t* _reader, uint8_t* _key, uint8_t* defval)
{
	struct json_object* reader = (struct json_object*)_reader;
	//struct ops_log_t* log = get_log_instance();
	json_object_object_foreach(reader, key, val) {
		//log->debug(0x01, __FILE__, __func__, __LINE__, "%s, %d, %s, %d %x\n", key, strlen(key), _key, strlen(_key), json_object_get_type(val));
		if( (strlen(_key) == strlen(key)) && (memcmp(_key, key, strlen(key)) == 0) ) {
		//log->debug(0x01, __FILE__, __func__, __LINE__, "%s, %s, %x\n", key, _key, json_object_get_type(val));
			return val;
		}
	}
	return defval;
}

static int get_array_count(json_reader_t* _array_reader)
{
	struct json_object* reader = (struct json_object*)_array_reader;
	return json_object_array_length(reader);
}

static json_reader_t* get_array_object_by_index(json_reader_t* _array_reader, int index)
{
	struct json_object* reader = (struct json_object*)_array_reader;
	struct json_object* jobj_val = NULL;
	jobj_val = json_object_array_get_idx(reader, index);
	if(jobj_val)
		return jobj_val;
	return NULL;
}
/*
static uint8_t* get_array_string_by_index(json_reader_t* _array_reader, int index, uint8_t* defval)
{
	struct json_object* reader = (struct json_object*)_array_reader;
	uint8_t* val = NULL;
	struct json_object* jobj_val = NULL;
	jobj_val = json_object_array_get_idx(reader, index);
	if(jobj_val) {
		val = (uint8_t*)json_object_get_string(jobj_val);
	} else {
		val = defval;
	}
	return val;
}
*/
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

static void set_boolean(json_writer_t* _writer, uint8_t* key, uint8_t val)
{
	struct json_object* writer = (struct json_object*)_writer;
	json_object_object_add(writer, key, json_object_new_boolean(val));
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

static void debug(json_reader_t* _reader)
{
	struct json_object* reader = (struct json_object*)_reader;
	struct ops_log_t* log = get_log_instance();
	json_object_object_foreach(reader, key, val) {
		log->debug(0x01, "%s, %d, %s, %x\n", __func__, __LINE__, key, json_object_get_type(val));
	}
}

static struct ops_json_t *obj = NULL;
struct ops_json_t *get_json_instance()
{
	if (!obj) {
		obj = malloc(sizeof(struct ops_json_t));
		obj->init = init;
		obj->show_all = show_all;
		obj->create = create;
		obj->destroy = destroy;
		obj->create_json_reader = create_reader;
		obj->create_json_reader_by_file = create_reader_by_file;
		obj->debug_json = debug;
		obj->get_json_string = get_string;
		obj->get_json_int = get_int;
		obj->get_json_boolean = get_boolean;
		obj->create_json_writer = create_writer;
		obj->create_json_array_writer = create_array_writer;
		obj->get_json_array_count = get_array_count;
		obj->set_json_array = set_array;
		obj->get_json_array = get_array;
		//obj->get_json_array_string_by_index = get_array_string_by_index;
		obj->get_json_array_object_by_index = get_array_object_by_index;
		obj->set_json_string = set_string;
		obj->set_json_int = set_int;
		obj->set_json_boolean = set_boolean;
		obj->out_json_to_bytes = to_bytes;

	}
	return obj;
}

void del_json_instance()
{
	if (obj)
		free(obj);
}
