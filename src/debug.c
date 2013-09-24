#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ckit/debug.h>
#include <ckit/log.h>
#if defined(ANDROID)
#include <android/log.h>
#endif

struct ck_dbg deflog = { 
	.log = { .fd = STDOUT_FILENO, 
			 .mode = CK_LOG_APPEND },
    .level = LOG_LVL_DBG
};

struct ck_dbg *dbg_log = &deflog;

const char *levelstr[] = {
    [LOG_LVL_VERB] = "VERB",
    [LOG_LVL_DBG]  = "DBG",
    [LOG_LVL_INF]  = "INF",
    [LOG_LVL_WARN] = "WARN",
    [LOG_LVL_ERR]  = "ERR",
};

const char *ck_dbg_level_to_str(enum ck_dbg_level lvl)
{
	return levelstr[lvl];
}

void ck_dbg_set_default_log(struct ck_dbg *log)
{
	dbg_log = log;
}

int ck_dbg_open(struct ck_dbg *log, const char *path)
{ 
	int ret;
	memset(log, 0, sizeof(struct ck_dbg));
	log->level = LOG_LVL_DBG;
	ret = ck_log_open(&log->log, path, CK_LOG_APPEND);

	if (ret == 0) {
		struct timeval now;
		gettimeofday(&now, NULL);
		ck_dbg_print(log, LOG_LVL_INF, 
					 "START", "%ld.%06ld %-4s %s\n",
					 (long)now.tv_sec, (long)now.tv_usec, "INF",
					 "<<<<<< Log start >>>>>>");
	}
	return ret;
}

void ck_dbg_close(struct ck_dbg *log)
{
    if (log) {
		/* Ensure we do not close stdout */
		if (log != &deflog)
			ck_log_close(&log->log);
		if (log == dbg_log)
			dbg_log = NULL;
    } else if (dbg_log) {
		/* Ensure we do not close stdout */
		if (log != &deflog)
			ck_log_close(&dbg_log->log);
		dbg_log = NULL;
    }
}

bool ck_dbg_is_open(struct ck_dbg *log)
{
	return (log->log.fd > 2);
}

int ck_dbg_print(struct ck_dbg *log, 
				 enum ck_dbg_level level,
				 const char *tag, 
				 const char *format, ...)
{
    int ret = 0;
    va_list ap;

    if (level < log->level)
		return 0;

	va_start(ap, format);

#if defined(ANDROID)
    
    switch (level) {
    case LOG_LVL_VERB:
		ret = ANDROID_LOG_VERBOSE;
		break;
    case LOG_LVL_DBG:
		ret = ANDROID_LOG_DEBUG;
		break;
    case LOG_LVL_INF:
		ret = ANDROID_LOG_INFO;
		break;
    case LOG_LVL_WARN:
		ret = ANDROID_LOG_WARN;
		break;
    case LOG_LVL_ERR:
		ret = ANDROID_LOG_ERROR;
		break;
    };
	
	if (log == &deflog) 
		ret = __android_log_vprint(ret, tag, format, ap);
	else
		ret = ck_log_vprint(&log->log, format, ap);
#else
	/* Write to stderr for LOG_LVL_ERR, but only if we are writing to
	 * the default log. */
	if (log == &deflog && 
		log->log.fd == 1 && 
		level == LOG_LVL_ERR)
		log->log.fd = 2;

	ret = ck_log_vprint(&log->log, format, ap);

	/* Switch back to stdout if necessary */
	if (log == &deflog && 
		log->log.fd == 2 && 
		level == LOG_LVL_ERR)
		log->log.fd = 1;
#endif
    va_end(ap);

    return ret;
}

void ck_dbg_set_level(struct ck_dbg *log, enum ck_dbg_level level)
{
    log->level = level;
}	
