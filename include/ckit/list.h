/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Simple double linked list implementation.
 *
 * Author: Erik Nordström <erik.nordstrom@gmail.com>
 */
#ifndef __LIST_H__
#define __LIST_H__

#include <ckit/ckit.h>

typedef struct list {
    struct list *prev, *next;
} list_t;

/**
 * Initialize the list anchor (head). Must be called once on all list
 * objects acting as anchors.
 */
#define LIST_INIT(list)                             \
    { (list)->prev = list; (list)->next = list; }

/**
 * Add element to front of list.
 */
static inline void list_add_front(struct list *anchor, struct list *to_add)
{
    to_add->next = anchor->next;
    to_add->prev = anchor;
    anchor->next->prev = to_add;
    anchor->next = to_add;
}

/**
 * Add element to back of list.
 */
static inline void list_add_back(struct list *anchor, struct list *to_add)
{
    to_add->next = anchor;
    to_add->prev = anchor->prev;
    anchor->prev->next = to_add;
    anchor->prev = to_add;
}

/**
 * Delete an element from the list it belongs to.
 */
static inline void list_del(struct list *to_del)
{
    to_del->prev->next = to_del->next;
    to_del->next->prev = to_del->prev;
    to_del->next = to_del->prev = NULL;
}

/**
 * Move element from one list to another.
 */
static inline void list_move(struct list *to_move, struct list *anchor)
{
    to_move->prev->next = to_move->next;
    to_move->next->prev = to_move->prev;
    list_add_front(anchor, to_move);
}

/**
 * Check if list is empty (0 = empty , 1 otherwise).
 */
#define list_empty(list) ((list)->next == (list))

/**
 * Get the object that this list element is embedded in.
 */
#define list_entry(list, type, member)          \
    get_enclosing(list, type, member)

/**
 * Get the first element in the list.
 */
#define list_front(list, type, member)          \
    get_enclosing((list)->next, type, member)

/**
 * Traverse all elments in list from front to back.
 */
#define list_foreach(ptr, anchor, member)                                 \
    for (ptr = list_front(anchor, typeof(*(ptr)), member);                \
         &(ptr)->member != anchor;                                        \
         ptr = get_enclosing((ptr)->member.next, typeof(*(ptr)), member))

#endif /* __LIST_H__ */
