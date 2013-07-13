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
#include <errno.h>

#if defined(ANDROID)
#include <android/log.h>

#define __LOG(type, atype, format, ...)                                 \
    __android_log_print(atype, "ckit",  "%s %s: "format,                \
                        type, __func__, ##__VA_ARGS__)

#if defined(ENABLE_DEBUG)
#define LOG_DBG(format, ...)                                \
    __LOG("DBG", ANDROID_LOG_DEBUG, format, ##__VA_ARGS__)

#else
#define LOG_DBG(format, ...)
#endif /* ENABLE_DEBUG */

#define LOG_ERR(format, ...)                                \
    __LOG("ERR", ANDROID_LOG_ERROR, format, ##__VA_ARGS__)
#define LOG_INF(format, ...)                            \
    __LOG("INF", ANDROID_LOG_INFO, format, ##__VA_ARGS__)

#else /* ANDROID */
#include <sys/time.h>

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

#endif /* CKIT_DEBUG_H */
