/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <sys/socket.h>
#include <unistd.h>
#include <ckit/event.h>
#include <ckit/debug.h>
#include <sys/epoll.h>
#include <string.h>

int event_open(void)
{
    return epoll_create(10);
}

void event_close(int efd)
{
    close(efd);
}

unsigned epoll_cmd(enum event_ctl ctl)
{
    unsigned cmd = 0;

    switch (ctl) {
    case EVENT_CTL_ADD:
        cmd = EPOLL_CTL_ADD;
        break;
    case EVENT_CTL_MOD:
        cmd = EPOLL_CTL_MOD;
        break;
    case EVENT_CTL_DEL:
        cmd = EPOLL_CTL_DEL;
        break;
    }
    return cmd;
}

static void event_to_epoll_event(const struct event *ev, 
                                 struct epoll_event *eev)
{
    memset(eev, 0, sizeof(*eev));

    if (ev->events & EVENT_IN)
        eev->events = EPOLLIN;

    if (ev->events & EVENT_OUT)
        eev->events = EPOLLOUT;

    if (ev->events & EVENT_HUP)
        eev->events = EPOLLHUP;
    
    if (ev->events & EVENT_RDHUP)
        eev->events = EPOLLRDHUP;

    if (ev->events & EVENT_ET)
        eev->events = EPOLLET;

    if (ev->events & EVENT_ONESHOT)
        eev->events = EPOLLONESHOT;

    memcpy(&eev->data, &ev->data, sizeof(ev->data));
}

static void epoll_event_to_event(const struct epoll_event *eev, 
                                 struct event *ev)
{
    memset(ev, 0, sizeof(*ev));

    if (eev->events & EPOLLIN)
        ev->events = EVENT_IN;

    if (eev->events & EPOLLOUT)
        ev->events = EVENT_OUT;

    if (eev->events & EPOLLHUP)
        ev->events = EVENT_HUP;
    
    if (eev->events & EPOLLRDHUP)
        ev->events = EVENT_RDHUP;

    if (eev->events & EPOLLET)
        ev->events = EVENT_ET;

    if (eev->events & EPOLLONESHOT)
        ev->events = EVENT_ONESHOT;

    memcpy(&ev->data, &eev->data, sizeof(eev->data));
}

int event_ctl(int efd, enum event_ctl ctl, int fd, struct event *ev)
{
    struct epoll_event eev;
    int ret;

    event_to_epoll_event(ev, &eev);

    ret = epoll_ctl(efd, epoll_cmd(ctl), fd, &eev);

    if (ret == -1) {
        LOG_ERR("epoll_ctl: %s\n",
                strerror(errno));
    }
    return ret;
}

int event_wait(int efd, struct event *events, int maxevents, int timeout)
{
    struct epoll_event epoll_events[maxevents];
    int ret;

    //LOG_DBG("epoll_wait\n");
    ret = epoll_wait(efd, epoll_events, maxevents, timeout);
    
    //LOG_DBG("epoll_wait returned %d\n", ret);

    if (ret == -1) {
        LOG_ERR("epoll_wait: %s\n",
                strerror(errno));
    } else if (ret > 0) {
        unsigned i;

        for (i = 0; i < ret; i++)
            epoll_event_to_event(&epoll_events[i], &events[i]);
    } 

    return ret;
}
