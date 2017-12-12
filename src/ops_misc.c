#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <syslog.h>

#include "ops_misc.h"
#include "ops_mq.h"

static void init(void)
{
}

static void show_all(void)
{
}

static void foo(lua_State * L, char *msg)
{
	fprintf(stderr, "\nFATAL ERROR:\n %s: %s\n\n", msg,
		lua_tostring(L, -1));
}

static int get_cfg(uint8_t * key, uint8_t * val)
{
	const int8_t *str;
	lua_State *L;

	L = luaL_newstate();
	//load Lua libraries
	luaL_openlibs(L);
	//load but do NOT run the Lua script file
	if (luaL_loadfile(L, LUA_CFG)) {
		foo(L, "luaL_loadfile() failed");
		return -1;
	}
	//run the loaded Lua script
	if (lua_pcall(L, 0, 0, 0)) {
		foo(L, "lua_pcall() failed");
		return -2;
	}

	lua_getglobal(L, "get_cfg");
	lua_pushstring(L, key);

	if (lua_pcall(L, 1, 1, 0) != 0) {
		foo(L, "pushstring() failed");
		return -3;
	}

	str = lua_tostring(L, -1);
	strcpy(val, str);

	//close the Lua state
	lua_close(L);
	return 0;
}

static int get_dbpath(uint8_t * val)
{
	return get_cfg("dbpath", val);
}

static int get_dbtype(uint32_t * val)
{
#define DB_TYPE_STR_LEN	8
	uint8_t str[DB_TYPE_STR_LEN] = { 0 };
	int ret = 0;
	memset(&str[0], 0, DB_TYPE_STR_LEN);
	ret = get_cfg("dbtype", &str[0]);
	if (ret < 0) {
		*val = 0;
	} else {
		*val = strtoul(&str[0], NULL, 10);
	}
	return ret;
}

static int get_logmask(uint8_t * val)
{
#define LOG_MASK_STR_LEN	4
	uint8_t str[LOG_MASK_STR_LEN] = { 0 };
	int ret = 0;
	memset(&str[0], 0, LOG_MASK_STR_LEN);
	ret = get_cfg("logmask", &str[0]);
	if (ret < 0) {
		*val = 0;
	} else {
		*val = (uint8_t) strtoul(&str[0], NULL, 16);
	}
	return ret;
}

static struct ops_misc_t *obj;
struct ops_misc_t *get_misc_instance()
{
	if (!obj) {
		obj = malloc(sizeof(struct ops_misc_t));
		obj->init = init;
		obj->show_all = show_all;
		obj->get_dbpath = get_dbpath;
		obj->get_dbtype = get_dbtype;
		obj->get_logmask = get_logmask;
	}

	return obj;
}

void del_misc_instance()
{
	if (obj)
		free(obj);
}
