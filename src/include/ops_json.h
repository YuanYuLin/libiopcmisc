#ifndef OPS_JSON_H
#define OPS_JSON_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define json_reader_t   void
#define json_writer_t   void

struct ops_json_t {
	void (*init) (void);
	void (*show_all) (void);
	int (*create) (uint8_t * id);
	int (*destroy) (uint8_t * id);
	json_reader_t* (*create_json_reader)(uint8_t* json_str);
	json_reader_t* (*create_json_reader_by_file)(uint8_t* file_path);
	uint8_t* (*get_json_string)(json_reader_t* reader, uint8_t* key, uint8_t* defval);
	int (*get_json_int)(json_reader_t* reader, uint8_t* key, int defval);
	uint8_t (*get_json_boolean)(json_reader_t* reader, uint8_t* key, uint8_t defval);
	json_writer_t* (*create_json_writer)();
	json_writer_t* (*create_json_array_writer)();
	void (*set_json_array)(json_writer_t* _array_writer, json_writer_t* _writer);
	void (*set_json_string)(json_writer_t* writer, uint8_t* key, uint8_t* val);
	void (*set_json_int)(json_writer_t* writer, uint8_t* key, int val);
	void (*set_json_boolean)(json_writer_t* writer, uint8_t* key, uint8_t val);
	json_reader_t* (*get_json_array)(json_reader_t* _reader, uint8_t* key, uint8_t* defval);
	int (*get_json_array_count)(json_reader_t* _array_reader);
	uint8_t* (*get_json_array_string_by_index)(json_reader_t* _array_reader, int index, uint8_t* defval);
	uint32_t (*out_json_to_bytes)(json_writer_t* writer, uint8_t* data);
	void (*debug_json)(json_reader_t* _reader);
};

struct ops_json_t *get_json_instance();
void del_json_instance();
#endif
