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

typedef enum ck_log_flag {
    CK_LOG_F_SYNC  = (1 << 0),
} ck_log_flag_t;

typedef struct ck_log {
    int fd;
    FILE *fp;
    enum ck_log_flag flags;
    enum ck_log_mode mode;
} ck_log_t;

int ck_log_init(struct ck_log *log, FILE *fp);
int ck_log_open(struct ck_log *log, const char *path, enum ck_log_mode mode);
void ck_log_close(struct ck_log *log);
int ck_log_print(struct ck_log *log, const char *format, ...);
int ck_log_vprint(struct ck_log *log, const char *format, va_list ap);
void ck_log_set_flag(struct ck_log *log, unsigned char flags);
void ck_log_unset_flag(struct ck_log *log, unsigned char flags);
int ck_log_check_flag(struct ck_log *log, unsigned char flags);

#endif /* CKIT_LOG_H */
