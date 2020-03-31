#include <syslog.h>
#include <stdarg.h>

#include "ops_misc.h"
#include "ops_log.h"

#define LOG_PREFIX	"iopc"
#define LOG_STR_LEN	1024
#define FULLLOG_STR_LEN	(1024+512)
#define LOG_MASKFILE_PATH	"/var/log/logmask_file"
#define LOG_MASK_PATH	"/var/log/logmask"

static void init(void)
{
}

static void show_all(void)
{
}

/*
 * LOG_EMERG - 0
 *     The message says the system is unusable. 
 * LOG_ALERT - 1
 *     Action on the message must be taken immediately. 
 * LOG_CRIT - 2
 *     The message states a critical condition. 
 * LOG_ERR - 3
 *     The message describes an error. 
 * LOG_WARNING - 4
 *     The message is a warning. 
 * LOG_NOTICE - 5
 *     The message describes a normal but important event. 
 * LOG_INFO - 6 
 *     The message is purely informational. 
 * LOG_DEBUG - 7
 *     The message is only for debugging purposes. 
 */
#define MASK_STR_LEN	4
#if 0
static void log(uint8_t mask, uint8_t level, const char *fmt, ...)
{
	va_list arg;
	uint8_t mask_str[MASK_STR_LEN] = { 0 };
	uint8_t cfg_mask = 0;
	int priority = LOG_DEBUG;
	int ret = 0;
	memset(&mask_str[0], 0, MASK_STR_LEN);

	/* Write the error message */
	/*
	   va_start(arg, fmt);
	   sprintf(&log_str[0], fmt, arg);
	   va_end(arg);
	 */
	ret = get_cfg("logmask", &mask_str[0]);
	if (ret < 0) {
		cfg_mask = 0;
	} else {
		cfg_mask = (uint8_t) strtoul(&mask_str[0], NULL, 16);
	}

	if (level > 7)
		priority = LOG_DEBUG;
	else
		priority = level;

	if (cfg_mask & mask) {
		openlog(LOG_PREFIX, LOG_PID, 0);
		va_start(arg, fmt);
		syslog(priority, fmt, arg);
		va_end(arg);
		closelog();
	}
}
#endif
static void _log(int level, const char* _file, const char* _func, uint32_t _line, uint8_t* log_str)
{
	uint8_t buf[FULLLOG_STR_LEN] = {0};
	memset(&buf, 0, FULLLOG_STR_LEN);
	snprintf(buf, FULLLOG_STR_LEN, "[%s - %s - %d]%s\n", _file, _func, _line, log_str);

	openlog(LOG_PREFIX, LOG_PID, 0);
	syslog(level, buf);
	closelog();
}

/*
 * mask 
 *   0x00: reserved,
 *   0xFF: reserved, force to log
 */
static uint8_t _compare_logmask(uint8_t ex_mask)
{
	uint8_t mask = 0x0;
	FILE* fp;
	uint8_t buf[LOG_STR_LEN] = {0};

	if(ex_mask == 0xFF)
		return 1;

	fp = fopen(LOG_MASK_PATH, "r");
	if(fp != NULL) {
		memset(&buf, 0, LOG_STR_LEN);
		if(fgets(buf, LOG_STR_LEN, fp) != NULL) {
			mask =(uint8_t)strtoul(buf, NULL, 16);
		}
	}
	if(fp != NULL)
		fclose(fp);

	/*
	uint8_t log[LOG_STR_LEN] = {0};
	memset(&log, 0, LOG_STR_LEN);
	snprintf(log, LOG_STR_LEN, "mask(%s)[%x & %x]", LOG_MASK_PATH, mask, ex_mask);
	_log(LOG_INFO, __FILE__, __func__, __LINE__, log);
	*/

	return (mask & ex_mask);
}

static uint8_t _compare_logmask_file(const char* file)
{
	int found = 0;
	int str1_len = 0;
	int str2_len = 0;
	FILE* fp;
	uint8_t mask = 0x80;
	uint8_t buf[LOG_STR_LEN] = {0};
	uint8_t log[LOG_STR_LEN] = {0};
	memset(&buf, 0, LOG_STR_LEN);

	fp = fopen(LOG_MASKFILE_PATH, "r");
	if(fp != NULL) {
		while(fgets(buf, LOG_STR_LEN, fp) != NULL) {
			str1_len = strlen(buf) - 1;// TODO : Workaround
			str2_len = strlen(file);
			if(_compare_logmask(mask)) {
				memset(&log, 0, LOG_STR_LEN);
				snprintf(log, LOG_STR_LEN, "%s:%d - %s:%d", buf, str1_len, file, str2_len);
				_log(LOG_INFO, __FILE__, __func__, __LINE__, log);
			}
			if((str1_len <= 0) || (str1_len > str2_len)) {
				found = 0;
			} else if(strncmp(buf, file, str1_len) == 0) {
				found = 1;
				break;
			}
			memset(&buf, 0, LOG_STR_LEN);
		}
	}

	if(fp != NULL)
		fclose(fp);

	if(_compare_logmask(mask)) {
		memset(&log, 0, LOG_STR_LEN);
		snprintf(log, LOG_STR_LEN, "found: %d", found);
		_log(LOG_INFO, __FILE__, __func__, __LINE__, log);
	}
	return found;
}

static void loginfo(uint8_t mask, const char* file, const char* func, uint32_t line, const char *fmt, ...)
{
	va_list arg;
	uint8_t buf[LOG_STR_LEN]= {0};

	if ((_compare_logmask(mask)) || (_compare_logmask_file(file))) {
		memset(&buf, 0, LOG_STR_LEN);
		va_start(arg, fmt);
		vsnprintf(buf, LOG_STR_LEN, fmt, arg);
		va_end(arg);
		_log(LOG_INFO, file, func, line, buf);
	}
}

static void logdebug(uint8_t mask, const char* file, const char* func, uint32_t line, const char *fmt, ...)
{
	va_list arg;
	uint8_t buf[LOG_STR_LEN];

	if ((_compare_logmask(mask)) || (_compare_logmask_file(file))) {
		memset(&buf, 0, LOG_STR_LEN);
		va_start(arg, fmt);
		vsnprintf(buf, LOG_STR_LEN, fmt, arg);
		va_end(arg);
		_log(LOG_DEBUG, file, func, line, buf);
	}
}

static void logerror(uint8_t mask, const char* file, const char* func, uint32_t line, const char *fmt, ...)
{
	va_list arg;
	uint8_t buf[LOG_STR_LEN];

	if ((_compare_logmask(mask)) || (_compare_logmask_file(file))) {
		memset(&buf, 0, LOG_STR_LEN);
		va_start(arg, fmt);
		vsnprintf(buf, LOG_STR_LEN, fmt, arg);
		va_end(arg);
		_log(LOG_ERR, file, func, line, buf);
	}
}

static struct ops_log_t *obj;
struct ops_log_t *get_log_instance()
{
	if (!obj) {
		obj = malloc(sizeof(struct ops_log_t));
		obj->init = init;
		obj->show_all = show_all;
		//obj->log = log;
		obj->info = loginfo;
		obj->debug = logdebug;
		obj->error = logerror;
	}

	return obj;
}

void del_log_instance()
{
	if (obj)
		free(obj);
}
