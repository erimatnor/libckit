/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Pipe-based IPC signals for waking/signaling between
 * threads. Supports waiting on using, e.g., select() or poll().
 *
 * Author: Erik Nordström <erik.nordstrom@gmail.com>
 *
 */
#ifndef CKIT_SIGNAL_H
#define CKIT_SIGNAL_H

#include <ckit/atomic.h>

typedef struct signal {
    int fd[2];
    atomic_t waiting; /* Incremented every time someone is waiting on
                         this signal */
} signal_t;

int signal_init(struct signal *s);
void signal_destroy(struct signal *s);
int signal_clear_val(struct signal *s, int *val);
int signal_clear(struct signal *s);
int signal_get_fd(struct signal *s);
int signal_is_raised(const struct signal *s);
int signal_wait_val(struct signal *s, int timeout, int *val);
int signal_wait(struct signal *s, int timeout);
int signal_raise_val(struct signal *s, int val);
int signal_raise(struct signal *s);
unsigned int signal_num_waiting(struct signal *s);

#endif /* CKIT_SIGNAL_H */
