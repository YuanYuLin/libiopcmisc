//#include <lua.h>
//#include <lauxlib.h>
//#include <lualib.h>

#include <syslog.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "ops_misc.h"
#include "ops_mq.h"
#include "ops_log.h"

static void init(void)
{
}

static void show_all(void)
{
}
#if 0
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
#endif
static int get_logmask(uint8_t * val)
{
//#define LOG_MASK_STR_LEN	4
//	uint8_t str[LOG_MASK_STR_LEN] = { 0 };
	int ret = 0;
//	memset(&str[0], 0, LOG_MASK_STR_LEN);
//	ret = get_cfg("logmask", &str[0]);
//	if (ret < 0) {
//		*val = 0;
//	} else {
//		*val = (uint8_t) strtoul(&str[0], NULL, 16);
//	}

	*val = 0xFF;
	return ret;
}
/*
static int syscmd(uint8_t* cmd)
{
    //struct ops_log_t* log = get_log_instance();
    //log->debug(0x01, "syscmd : %s\n", cmd);
    system(cmd);
    return 0;
}
*/
/*
static int syscmd(uint8_t cmdlen, uint8_t* cmd)
{
    struct ops_net_t* net = get_net_instance();
    struct msg_t req;
    struct msg_t res;
    struct shell_cmd_t *req_cmd = (struct shell_cmd_t*)&req.data;
    struct shell_cmd_t *res_cmd = (struct shell_cmd_t*)&res.data;
    memset(&req, 0, sizeof(struct msg_t));
    memset(&res, 0, sizeof(struct msg_t));
    req.fn = 0xFF;
    req.cmd = 0xFF;
    req.data_size = sizeof(struct shell_cmd_t);
    req_cmd->cmdlen = cmdlen;
    memcpy(&req_cmd->cmd[0], cmd, cmdlen);
    net->uds_client_send_and_recv(SOCKET_PATH_SHELL, &req, &res);
}
*/

static uint8_t get_macaddress_by_interface(uint8_t* interface, uint8_t* mac_address)
{
#define MAC_LEN	6
	struct ifreq ifr;
	int success = 0;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	
	if (sock < 0) {
		return 0;
	};

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, interface, strlen(interface));

	if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
		success = 1;
	}

	if (success) {
		memcpy(mac_address, ifr.ifr_hwaddr.sa_data, MAC_LEN);
		return MAC_LEN;
	}
	return 0;
}

static uint8_t get_ipaddress_by_interface(uint8_t* interface, uint8_t* ip_address)
{
	struct ifreq ifr;
	int success = 0;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	uint8_t* ptr = NULL;
	
	if (sock < 0) {
		return 0;
	};

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, interface, strlen(interface));

	if (ioctl(sock, SIOCGIFADDR, &ifr) == 0) {
		success = 1;
	}

	if (success) {
		ptr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
		memcpy(ip_address, ptr, strlen(ptr));
		return strlen(ptr);
	}
	return 0;
}

static uint32_t get_pid_by_path(uint8_t *path)
{
	uint32_t pid = 0;
	FILE *fp = NULL;
	fp = fopen(path, "r");
        if(!fp) {
		return 0;
	}
	fscanf(fp, "%d", &pid);
	fclose(fp);

	return pid;
}

static void create_dir_recursive(const char *dir, mode_t mode)
{
    #undef STR_LEN
    #define STR_LEN	250
    uint8_t tmp[STR_LEN];
    uint8_t *p = NULL;
    size_t len;

    memset(&tmp[0], 0, STR_LEN);
    snprintf(tmp, STR_LEN, "%s", dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, mode);
            *p = '/';
    }
    mkdir(tmp, mode);
}

#if 0
static uint8_t is_exist(uint8_t* path)
{
    struct stat st;
    if(stat(path, &st) != 0) {
        printf("[%s] is not exist\n", path);
        return 0;
    }
    return 1;
}
#endif
static struct ops_misc_t *obj;
struct ops_misc_t *get_misc_instance()
{
	if (!obj) {
		obj = malloc(sizeof(struct ops_misc_t));
		obj->init = init;
		obj->show_all = show_all;
		//obj->get_dbpath = get_dbpath;
		//obj->get_dbtype = get_dbtype;
		//obj->is_file_exist = is_exist;
		obj->get_logmask = get_logmask;
		//obj->syscmd = syscmd;
		obj->get_macaddress_by_interface = get_macaddress_by_interface;
		obj->get_ipaddress_by_interface = get_ipaddress_by_interface;
		obj->create_dir_recursive = create_dir_recursive;
		obj->get_pid_by_path = get_pid_by_path;
	}

	return obj;
}

void del_misc_instance()
{
	if (obj)
		free(obj);
}
