/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Author: Erik Nordström <erik.nordstrom@gmail.com>
 */
#ifndef __LIST_H__
#define __LIST_H__

#include <ckit/ckit.h>

typedef struct list {
    struct list *prev, *next;
} list_t;

#define LIST_INIT(list)                             \
    { (list)->prev = list; (list)->next = list; }

static inline void list_add_head(struct list *head, struct list *to_add)
{
    to_add->next = head->next;
    to_add->prev = head;
    head->next->prev = to_add;
    head->next = to_add;
}

static inline void list_add_tail(struct list *head, struct list *to_add)
{
    to_add->next = head;
    to_add->prev = head->prev;
    head->prev->next = to_add;
    head->prev = to_add;
}

static inline void list_del(struct list *to_del)
{
    to_del->prev->next = to_del->next;
    to_del->next = to_del->prev;
    to_del->next = to_del->prev = NULL;
}

#define list_empty(list) ((list)->next == (list))
#define list_entry(list, type, member)          \
    get_enclosing(list, type, member)
#define list_front(list, type, member)          \
    get_enclosing((list)->next, type, member)

#define list_foreach(ptr, head, member)                                 \
    for (ptr = list_front(head, typeof(*(ptr)), member);                \
         &(ptr)->member != head;                                        \
         ptr = get_enclosing((ptr)->member.next, typeof(*(ptr)), member))

#endif /* __LIST_H__ */
