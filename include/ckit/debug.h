/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Macros for printing debug information, which can be disabled at
 * compile time.
 *
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 *
 */
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <errno.h>

#if defined(__ANDROID__)
#include <android/log.h>

#define __LOG(type, atype, format, ...)                                 \
    __android_log_print(atype, "ckit",  "%s %s: "format,                \
                        type, __func__, ##__VA_ARGS__)

#if defined(ENABLE_DEBUG)
#define LOG_DBG(format, ...)                                \
    __LOG("DBG", ANDROID_LOG_DEBUG, format, ##__VA_ARGS)

#else
#define LOG_DBG(format, ...)
#endif /* ENABLE_DEBUG */

#define LOG_ERR(format, ...)                                \
    __LOG("ERR", ANROID_LOG_ERROR, format, ##__VA_ARGS)
#define LOG_INF(format, ...)                            \
    __LOG("INF", ANROID_LOG_INFO, format, ##__VA_ARGS__)

#else /* ANDROID */

#define __LOG(type, format, ...) ({                                     \
            struct timeval now;                                         \
            gettimeofday(&now, NULL);                                   \
            printf("%ld.%06ld %s %s: "format, (long)now.tv_sec,         \
                   (long)now.tv_usec, type, __func__, ##__VA_ARGS__);  \
        })


#if defined(ENABLE_DEBUG)
#include <sys/time.h>
#define LOG_DBG(format, ...)                    \
    __LOG("DBG", format, ##__VA_ARGS__)
#else
#define LOG_DBG(format, ...)
#endif /* ENABLE_DEBUG */

#define LOG_ERR(format, ...)                    \
    __LOG("ERR", format, ##__VA_ARGS__)

#define LOG_INF(format, ...)                    \
    __LOG("INF", format, ##__VA_ARGS__)

#endif /* ANDROID */

#endif /* _DEBUG_H_ */
