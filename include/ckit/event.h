/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/types.h>

typedef enum event_ctl {
    EVENT_CTL_ADD,
    EVENT_CTL_DEL,
    EVENT_CTL_MOD,
} event_ctl_t;

typedef union event_data {
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} event_data_t;

struct event {
    uint32_t events;
    event_data_t data;
};

enum event_types {
    EVENT_IN  = (1 << 0),
    EVENT_OUT = (1 << 1),  
    EVENT_HUP = (1 << 2),
    EVENT_RDHUP = (1 << 3),
    EVENT_ET  = (1 << 10),
    EVENT_ONESHOT  = (1 << 11),
};

int event_open(void);
void event_close(int efd);
int event_ctl(int efd, enum event_ctl ctl, int fd, struct event *ev);
int event_wait(int efd, struct event *events, int maxevents, int timeout); 

#endif /* __EVENT_H__ */
