/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef CKIT_TIME_H
#define CKIT_TIME_H

#include <sys/types.h>
#include <sys/time.h>

#define NSEC_PER_SEC   1000000000L
#define NSEC_PER_MSEC  1000000L
#define USEC_PER_SEC   1000000L
#define MSEC_PER_SEC   1000L
#define NSEC_PER_USEC  1000L
#define USEC_PER_MSEC  1000L

#define timespec_normalize(t) {                 \
        if ((t)->tv_nsec >= NSEC_PER_SEC) {     \
            (t)->tv_nsec -= NSEC_PER_SEC;       \
            (t)->tv_sec++;                      \
        } else if ((t)->tv_nsec < 0) {          \
            (t)->tv_nsec += NSEC_PER_SEC;       \
            (t)->tv_sec--;                      \
        }                                       \
    }

#define timespec_add_nsec(t1, nsec) do {        \
        (t1)->tv_sec += nsec / NSEC_PER_SEC;    \
        (t1)->tv_nsec += nsec % NSEC_PER_SEC;   \
        timespec_normalize(t1);                 \
    } while (0)

#define timespec_add(t1, t2) do {               \
        (t1)->tv_nsec += (t2)->tv_nsec;         \
        (t1)->tv_sec += (t2)->tv_sec;           \
        timespec_normalize(t1);                 \
    } while (0)

#define timespec_sub(t1, t2) do {               \
        (t1)->tv_nsec -= (t2)->tv_nsec;         \
        (t1)->tv_sec -= (t2)->tv_sec;           \
        timespec_normalize(t1);                 \
    } while (0)

#define timespec_nz(t) ((t)->tv_sec != 0 || (t)->tv_nsec != 0)
#define timespec_lt(t1, t2) ((t1)->tv_sec < (t2)->tv_sec ||     \
                             ((t1)->tv_sec == (t2)->tv_sec &&   \
                              (t1)->tv_nsec < (t2)->tv_nsec))
#define timespec_gt(t1, t2) (timespec_lt(t2, t1))
#define timespec_ge(t1, t2) (!timespec_lt(t1, t2))
#define timespec_le(t1, t2) (!timespec_gt(t1, t2))
#define timespec_eq(t1, t2) ((t1)->tv_sec == (t2)->tv_sec &&    \
                             (t1)->tv_nsec == (t2)->tv_nsec)


#define timeval_normalize(t) {                  \
        if ((t)->tv_usec >= USEC_PER_SEC) {     \
            (t)->tv_usec -= USEC_PER_SEC;       \
            (t)->tv_sec++;                      \
        } else if ((t)->tv_usec < 0) {          \
            (t)->tv_usec += USEC_PER_SEC;       \
            (t)->tv_sec--;                      \
        }                                       \
    }

#define timeval_add_usec(t1, usec) do {         \
        (t1)->tv_sec += usec / USEC_PER_SEC;    \
        (t1)->tv_usec += usec % USEC_PER_SEC;   \
        timeval_normalize(t1);                  \
    } while (0)

#define timeval_sub_usec(t1, usec) do {         \
        (t1)->tv_sec -= usec / USEC_PER_SEC;    \
        (t1)->tv_usec -= usec % USEC_PER_SEC;   \
        timeval_normalize(t1);                  \
    } while (0)

#define timeval_add(t1, t2) do {                \
        (t1)->tv_usec += (t2)->tv_usec;         \
        (t1)->tv_sec += (t2)->tv_sec;           \
        timeval_normalize(t1);                  \
    } while (0)

#define timeval_sub(t1, t2) do {                \
        (t1)->tv_usec -= (t2)->tv_usec;         \
        (t1)->tv_sec -= (t2)->tv_sec;           \
        timeval_normalize(t1);                  \
    } while (0)

#define timeval_nz(t) ((t)->tv_sec != 0 || (t)->tv_usec != 0)
#define timeval_lt(t1, t2) ((t1)->tv_sec < (t2)->tv_sec ||      \
                            ((t1)->tv_sec == (t2)->tv_sec &&    \
                             (t1)->tv_usec < (t2)->tv_usec))
#define timeval_gt(t1, t2) (timeval_lt(t2, t1))
#define timeval_ge(t1, t2) (!timeval_lt(t1, t2))
#define timeval_le(t1, t2) (!timeval_gt(t1, t2))
#define timeval_eq(t1, t2) ((t1)->tv_sec == (t2)->tv_sec && \
                            (t1)->tv_usec == (t2)->tv_usec)

#endif /* CKIT_TIME_H */
