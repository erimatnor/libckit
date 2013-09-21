/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ckit/log.h>

struct ck_log *ck_log_fopen(FILE *fp)
{
    struct ck_log *ckl;

    ckl = malloc(sizeof(struct ck_log));

    if (!ckl) {
        errno = ENOMEM;
        return NULL;
    }
    
    memset(ckl, 0, sizeof(struct ck_log));
    ckl->fp = fp;

    return NULL;
}

struct ck_log *ck_log_open(const char *path, enum ck_log_mode mode)
{
    struct ck_log *ckl;
    FILE *fp;
    char *mo = "";

    switch (mode) {
    case CK_LOG_APPEND:
        mo = "a";
        break;
    case CK_LOG_TRUNC:
        mo = "w";
        break;
    }

    fp = fopen(path, mo);

    if (!fp)
        return NULL;

    ckl = ck_log_fopen(fp);

    if (!ckl) {
        fclose(fp);
    } else {
        ckl->mode = mode;
    }

    return ckl;
}

void ck_log_close(struct ck_log *ckl)
{
    fclose(ckl->fp);
    free(ckl);
}

int ck_log_write(struct ck_log *log, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);    
    ret = vfprintf(log->fp, format, ap);
    va_end(ap);
    
    return ret;
}
