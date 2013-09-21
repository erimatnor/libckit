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
    FILE *fp;
    enum ck_log_mode mode;
} ck_log_t;

#define DEFAULT_CK_LOG_TAG "cklog"

struct ck_log *ck_log_fopen(FILE *fp);
struct ck_log *ck_log_open(const char *path, enum ck_log_mode mode);
void ck_log_close(struct ck_log *);
int ck_log_write(struct ck_log *log, const char *format, ...);

#endif /* CKIT_LOG_H */
