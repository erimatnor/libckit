/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ckit/log.h>

int ck_log_init(struct ck_log *log, FILE *fp, enum ck_log_mode mode)
{
    memset(log, 0, sizeof(struct ck_log));
    log->fp = fp;
    log->mode = mode;
    return 0;
}

int ck_log_open(struct ck_log *log, const char *path, enum ck_log_mode mode)
{
    char *omode = "";
    FILE *fp;

    switch (mode) {
    case CK_LOG_APPEND:
        omode = "a";
        break;
    case CK_LOG_TRUNC:
        omode = "w";
        break;
    }

    /* Open a stream handle because android doesn't support vdprintf()
     * but has vfprintf(). */
    fp = fopen(path, omode);
    
    if (!fp) {
        fprintf(stderr, "fdopen failed for %s\n", path);
        return -1;
    }

    return ck_log_init(log, fp, mode);
}

void ck_log_close(struct ck_log *log)
{
    fclose(log->fp);
}

int ck_log_print(struct ck_log *log, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = vfprintf(log->fp, format, ap);
    va_end(ap);

    if (ck_log_check_flag(log, CK_LOG_F_SYNC))
        fflush(log->fp);

    return ret;
}

int ck_log_vprint(struct ck_log *log, const char *format, va_list ap)
{
    return vfprintf(log->fp, format, ap);
}

void ck_log_set_flag(struct ck_log *log, unsigned char flags)
{
    log->flags |= flags;
}

void ck_log_unset_flag(struct ck_log *log, unsigned char flags)
{
    log->flags &= ~flags;
}

int ck_log_check_flag(struct ck_log *log, unsigned char flags)
{
    return (log->flags & flags) > 0;
}
