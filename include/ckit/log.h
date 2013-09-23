/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * 
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 *
 */
#ifndef CKIT_LOG_H
#define CKIT_LOG_H

#include <stdarg.h>

typedef enum ck_log_mode {
    CK_LOG_APPEND,
    CK_LOG_TRUNC,
} ck_log_mode_t;

typedef struct ck_log {
    int fd;
    FILE *fp;
    enum ck_log_mode mode;
} ck_log_t;

int ck_log_init(struct ck_log *log, FILE *fp);
int ck_log_open(struct ck_log *log, const char *path, enum ck_log_mode mode);
void ck_log_close(struct ck_log *log);
int ck_log_print(struct ck_log *log, const char *format, ...);
int ck_log_vprint(struct ck_log *log, const char *format, va_list ap);

#endif /* CKIT_LOG_H */
