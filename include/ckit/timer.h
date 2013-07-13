/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Timer queue implementation that integrates with select/poll loops.
 *
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 */
#ifndef CKIT_TIMER_H
#define CKIT_TIMER_H

#include <ckit/signal.h>
#include <ckit/time.h>
#include <ckit/heap.h>
#include <pthread.h>

struct timer {
    struct heapitem hi;
    struct timespec timeout;
    long expires; /* micro seconds */
    void (*callback)(struct timer *t);
    void (*destruct)(struct timer *t);
    void *data;        
};

struct timer_queue {
    struct heap queue;
    pthread_mutex_t lock;
    struct signal signal;
    pthread_t thr;
};

#define TIMER_CALLBACK(t, cb) struct timer t = {    \
        .lh = { &t.lh, &t.lh },                     \
        .expires = 0,                               \
        .callback = cb,                             \
        .destruct = NULL,                           \
        .data = NULL                                \
    }
#define TIMER(t) TIMER_CALLBACK(t, NULL)

enum signal_result {
    TIMER_SIGNAL_ERROR = -1,
    TIMER_SIGNAL_NONE,
    TIMER_SIGNAL_SET,
    TIMER_SIGNAL_EXIT,
};

void timer_init(struct timer *t);
struct timer *timer_new_callback(void (*callback)(struct timer *t), void *data);
void timer_free(struct timer *t);
int timer_add(struct timer_queue *tq, struct timer *t);
int timer_mod(struct timer_queue *tq, struct timer *t, unsigned long expires);
void timer_del(struct timer_queue *tq, struct timer *t);
int timer_next_timeout(struct timer_queue *tq, unsigned long *timeout);
int timer_next_timeout_timespec(struct timer_queue *tq, 
                                struct timespec *timeout);
int timer_next_timeout_timeval(struct timer_queue *tq, 
                               struct timeval *timeout);
int timer_handle_timeout(struct timer_queue *tq);
int timer_queue_get_signal(struct timer_queue *tq);
int timer_queue_signal_raise(struct timer_queue *tq);
enum signal_result timer_queue_signal_lower(struct timer_queue *tq);
void timer_queue_destroy(struct timer_queue *tq);
int timer_queue_init(struct timer_queue *tq);
void timer_queue_fini(struct timer_queue *tq);

#define timer_new() timer_new_callback(NULL, NULL
#define timer_secs(s) (s * 1000000L)
#define timer_msecs(s) (s * 1000L)
#define timer_usecs(s) (s)
#define timer_set_secs(t, s) { (t)->expires = timer_secs(s); }
#define timer_set_msecs(t, s) { (t)->expires = timer_msecs(s); }
#define timer_set_usecs(t, s) { (t)->expires = s; }
#define timer_schedule_secs(tq, t, s) ({ int ret;   \
            timer_set_secs(t, s);                   \
            ret = timer_add(tq, t);                 \
            ret; })
#define timer_schedule_msecs(tq, t, s) ({ int ret;  \
            timer_set_msecs(t, s);                  \
            ret = timer_add(tq, t);                 \
            ret; })
#define timer_scheduled(t) (t->hi.active)
#define timer_destroy(t) { if ((t)->destruct) (t)->destruct(t); }

#endif /* CKIT_TIMER_H */
