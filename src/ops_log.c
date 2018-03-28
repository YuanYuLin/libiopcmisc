#include <syslog.h>
#include <stdarg.h>

#include "ops_misc.h"
#include "ops_log.h"

#define LOG_PREFIX	"iopc"
#define LOG_STR_LEN	1024

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
static void _log(int level, uint8_t* log_str)
{
	openlog(LOG_PREFIX, LOG_PID, 0);
	syslog(level, log_str);
	closelog();
}

static void loginfo(uint8_t mask, const char *fmt, ...)
{
	va_list arg;
	uint8_t buf[LOG_STR_LEN];
	uint8_t log_mask = 0;
	struct ops_misc_t *misc = get_misc_instance();
	misc->get_logmask(&log_mask);

	if (log_mask & mask) {
		memset(&buf, 0, LOG_STR_LEN);
		va_start(arg, fmt);
		vsnprintf(buf, LOG_STR_LEN, fmt, arg);
		va_end(arg);
		_log(LOG_INFO, buf);
	}
}

static void logdebug(uint8_t mask, const char *fmt, ...)
{
	va_list arg;
	uint8_t buf[LOG_STR_LEN];
	uint8_t log_mask = 0;
	struct ops_misc_t *misc = get_misc_instance();
	misc->get_logmask(&log_mask);

	if (log_mask & mask) {
		memset(&buf, 0, LOG_STR_LEN);
		va_start(arg, fmt);
		vsnprintf(buf, LOG_STR_LEN, fmt, arg);
		va_end(arg);
		_log(LOG_DEBUG, buf);
	}
}

static void logerror(uint8_t mask, const char *fmt, ...)
{
	va_list arg;
	uint8_t buf[LOG_STR_LEN];
	uint8_t log_mask = 0;
	struct ops_misc_t *misc = get_misc_instance();
	misc->get_logmask(&log_mask);

	if (log_mask & mask) {
		memset(&buf, 0, LOG_STR_LEN);
		va_start(arg, fmt);
		vsnprintf(buf, LOG_STR_LEN, fmt, arg);
		va_end(arg);
		_log(LOG_ERR, buf);
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
