#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ckit/debug.h>
#include <ckit/log.h>
#if defined(ANDROID)
#include <android/log.h>
#endif

static struct ck_dbg default_dbg = { 
	.log = { .fp = NULL, 
			 .mode = CK_LOG_APPEND },
    .level = LOG_LVL_DBG
};

struct ck_dbg *dbg_log = &default_dbg;

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

void ck_dbg_set_default_log(struct ck_dbg *dbg)
{
	dbg_log = dbg;
}

int ck_dbg_init(struct ck_dbg *dbg, FILE *fp)
{
	memset(dbg, 0, sizeof(struct ck_dbg));
	dbg->level = LOG_LVL_DBG;
	return ck_log_init(&dbg->log, fp, CK_LOG_APPEND);
}

int ck_dbg_open(struct ck_dbg *dbg, const char *path)
{ 
	int ret;
	memset(dbg, 0, sizeof(struct ck_dbg));
	dbg->level = LOG_LVL_DBG;
	ret = ck_log_open(&dbg->log, path, CK_LOG_APPEND);

	if (ret == 0) {
		struct timeval now;
		gettimeofday(&now, NULL);
		ck_dbg_print(dbg, LOG_LVL_INF, 
					 "START", "%ld.%06ld %-4s %s\n",
					 (long)now.tv_sec, (long)now.tv_usec, "INF",
					 "<<<<<< Log start >>>>>>");
	}
	return ret;
}

void ck_dbg_close(struct ck_dbg *dbg)
{
    if (dbg) {
		/* Ensure we do not close stdout */
		if (dbg != &default_dbg)
			ck_log_close(&dbg->log);
		if (dbg == dbg_log)
			dbg_log = NULL;
    } else if (dbg_log) {
		/* Ensure we do not close stdout */
		if (dbg != &default_dbg)
			ck_log_close(&dbg_log->log);
		dbg_log = NULL;
    }
}

bool ck_dbg_is_open(struct ck_dbg *dbg)
{
	return (dbg->log.fp != NULL || dbg == &default_dbg);
}

int ck_dbg_print(struct ck_dbg *dbg, 
				 enum ck_dbg_level level,
				 const char *tag, 
				 const char *format, ...)
{
    int ret = 0;
    va_list ap;

    if (level < dbg->level)
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
	
	if (dbg == &default_dbg) 
		ret = __android_log_vprint(ret, tag, format, ap);
	else
		ret = ck_log_vprint(&dbg->log, format, ap);
#else
	/* Write to stderr for LOG_LVL_ERR, but only if we are writing to
	 * the default log. */

	if (dbg == &default_dbg) {
		if (level == LOG_LVL_ERR)
			dbg->log.fp = stderr;
		else
			dbg->log.fp = stdout;
	}
	
	ret = ck_log_vprint(&dbg->log, format, ap);
	
	if (dbg == &default_dbg)
		dbg->log.fp = NULL;
#endif
    va_end(ap);

    return ret;
}

void ck_dbg_set_level(struct ck_dbg *dbg, enum ck_dbg_level level)
{
    dbg->level = level;
}	

void ck_dbg_set_sync_mode(struct ck_dbg *dbg)
{
	ck_log_set_flag(&dbg->log, CK_LOG_F_SYNC);
}
