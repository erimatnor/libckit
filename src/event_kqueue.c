/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <ckit/event.h>
#include <ckit/debug.h>
#include <ckit/time.h>

int event_open(void)
{
    return kqueue();
}

void event_close(int efd)
{
    close(efd);
}

static unsigned short kevent_flags(enum event_ctl ctl, struct event *ev)
{
    unsigned short flags = 0;

    switch (ctl) {
    case EVENT_CTL_ADD:
    case EVENT_CTL_MOD:
        flags |= EV_ADD;
        break;
    case EVENT_CTL_DEL:
        flags |= EV_DELETE;
        break;
    }
    
    if (ev->events & EVENT_ONESHOT)
        flags |= EV_ONESHOT;

    return flags;
}

static void kevent_to_event(const struct kevent *kev, struct event *ev)
{
    memset(ev, 0, sizeof(*ev));
    
    if (kev->filter == EVFILT_READ) {
        //LOG_DBG("EVFILT_READ is set fd=%d\n", (int)kev->ident);
        ev->events |= EVENT_IN;
    }

    if (kev->filter == EVFILT_WRITE) {
        //LOG_DBG("EVFILT_WRITE is set fd=%d\n", (int)kev->ident);
        ev->events |= EVENT_OUT;
    }

    if (kev->flags & EV_EOF) {
        //LOG_DBG("EV_EOF set on fd=%d\n", (int)kev->ident);
        ev->events |= EVENT_RDHUP;
    }
    ev->data.fd = (int)kev->ident;
    ev->data.ptr = kev->udata;
}

int event_ctl(int efd, enum event_ctl ctl, int fd, struct event *ev)
{
    struct kevent kev[2];
    unsigned i = 0;
    
    if (ev->events & EVENT_IN) {
        //LOG_ERR("EVFILT_READ fd=%d\n", fd);
        EV_SET(&kev[i++], fd, EVFILT_READ, kevent_flags(ctl, ev), 
               0, 0, ev->data.ptr);
    }

    if (ev->events & EVENT_OUT) {
        //LOG_ERR("EVFILT_WRITE fd=%d\n", fd);
        EV_SET(&kev[i++], fd, EVFILT_WRITE, kevent_flags(ctl, ev), 
               0, 0, ev->data.ptr);
    }

    return kevent(efd, kev, i, NULL, 0, NULL);
}

int event_wait(int efd, struct event *events, int maxevents, int timeout)
{
    struct kevent kevents[maxevents];
    struct timespec ts = { 0, 0 };
    struct timespec *ts_ptr = &ts;
    int ret;

    timespec_add_nsec(&ts, timeout * 1000000L);
    memset(kevents, 0, sizeof(kevents));

    if (timeout == -1) 
        ts_ptr = NULL;

    ret = kevent(efd, NULL, 0, kevents, maxevents, ts_ptr);

    if (ret == -1) {
        LOG_ERR("kevent: %s\n", strerror(errno));
    } else if (ret > 0) {
        int i;

        /* Fill in events */
        for (i = 0; i < ret; i++) {

            /* Check for error. The kevent() call may return either -1
             * on error or an error event in the eventlist in case
             * there is room */
            if (kevents[i].flags & EV_ERROR) {
                    errno = kevents[i].data;
                /* NOTE/WARNING: What happens to other events returned
                 * that aren't errors when we return -1 here? This is
                 * not entirely clear, but since they won't be
                 * processed by the caller of this function (as the
                 * return value is -1) the events should still be
                 * pending and returned in the next call to this
                 * function. This might, however, not be true for edge
                 * triggered events. */
                return -1;
            } 
            kevent_to_event(&kevents[i], &events[i]);
        }
    }

    return ret;
}
