/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Macros for printing debug information, which can be disabled at
 * compile time.
 *
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 *
 */
#ifndef CKIT_DEBUG_H
#define CKIT_DEBUG_H

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <ckit/log.h>

#ifndef LOG_TAG
#define LOG_TAG "ckit"
#endif

typedef enum ck_dbg_level {
    LOG_LVL_VERB,
    LOG_LVL_DBG,
    LOG_LVL_INF,
    LOG_LVL_WARN,
    LOG_LVL_ERR,
} ck_dbg_level_t;

struct ck_dbg {
    struct ck_log log;
    enum ck_dbg_level level;
};

/* 
   The singleton default log (which is stdout/stderr 
   unless set explicitly).
*/
extern struct ck_dbg *dbg_log;
extern struct ck_dbg deflog;

void ck_dbg_set_default_log(struct ck_dbg *log);
int ck_dbg_open(struct ck_dbg *log, const char *path);
void ck_dbg_close(struct ck_dbg *log);
bool ck_dbg_is_open(struct ck_dbg *log);

const char *ck_dbg_level_to_str(enum ck_dbg_level lvl);
int ck_dbg_print(struct ck_dbg *log, 
                 enum ck_dbg_level level, 
                 const char *tag,
                 const char *format, ...);
void ck_dbg_set_level(struct ck_dbg *log, enum ck_dbg_level level);

#define DBG_SET_LEVEL(level)                    \
    ck_dbg_set_level(dbg_log, level)

#define __LOG(level, format, ...) ({                                    \
            struct timeval now;                                         \
            gettimeofday(&now, NULL);                                   \
            ck_dbg_print(dbg_log, level, LOG_TAG,                       \
                         "%ld.%06ld %-4s %s %s: "format,                \
                         (long)now.tv_sec, (long)now.tv_usec,           \
                         ck_dbg_level_to_str(level),                    \
                         LOG_TAG, __func__,                             \
                         ##__VA_ARGS__);                                \
        })

#if defined(ENABLE_DEBUG)
#define LOG_DBG(format, ...)                    \
    __LOG(LOG_LVL_DBG, format, ##__VA_ARGS__)
#define LOG_VERB(format, ...)                   \
    __LOG(LOG_LVL_VERB, format, ##__VA_ARGS__)
#else /* ENABLE_DEBUG */
#define LOG_DBG(format, ...)
#define LOG_VERB(format, ...)
#endif /* ENABLE_DEBUG */

#define LOG_INF(format, ...)                    \
    __LOG(LOG_LVL_INF, format, ##__VA_ARGS__)
#define LOG_ERR(format, ...)                    \
    __LOG(LOG_LVL_ERR, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)                    \
    __LOG(LOG_LVL_WARN, format, ##__VA_ARGS__)

static inline const char *hexdump(const void *data, int datalen, 
                                  char *buf, int buflen)
{
        int i = 0, len = 0;
        const unsigned char *h = (const unsigned char *)data;
        
        while (i < datalen) {
                unsigned char c = (i + 1 < datalen) ? h[i+1] : 0;
                len += snprintf(buf + len, buflen - len, 
                                "%02x%02x ", h[i], c);
                i += 2;
        }
        return buf;
}

#endif /* CKIT_DEBUG_H */
