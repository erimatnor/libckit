/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <ckit/ckit.h>
#include <ckit/atomic.h>
#include <ckit/list.h>
#include <ckit/debug.h>
#include <ckit/pbuf.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MIN_POOL_SIZE 20 /* Number of buffers available */

/* 
   We currently use a simple static pool of buffers. Should probably
   move to a slab allocator or similar in the future. */
struct pbuf_pool_entry {
    list_t lnode;
    struct pbuf pbuf;
    unsigned char data[PBUF_MAX_SIZE];
};

static pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
static list_t free_list = { &free_list, &free_list };
static list_t use_list = { &use_list, &use_list };
static struct pbuf_pool_entry *objects;

int pbuf_pool_init(size_t pool_size)
{
    size_t i;

    if (pool_size < MIN_POOL_SIZE)
        pool_size = MIN_POOL_SIZE;
    objects = calloc(pool_size, sizeof(struct pbuf_pool_entry));
    
    if (!objects)
        return -1;
    
    for (i = 0; i < pool_size; i++)
        list_add_back(&free_list, &objects[i].lnode);

    return 0;
}

void pbuf_pool_cleanup(void)
{
    free(objects);
}

static void pbuf_init(struct pbuf *pb, size_t size)
{
    memset(pb, 0, sizeof(struct pbuf));
    pb->end = size;
    atomic_set(&pb->refcount, 1);
}

struct pbuf *pbuf_alloc(size_t size)
{
    struct pbuf_pool_entry *ppe = NULL;
    struct pbuf *pb = NULL;

    assert(size <= PBUF_MAX_SIZE);

    pthread_mutex_lock(&pool_mutex);

    if (!list_empty(&free_list)) {
        ppe = list_front(&free_list, struct pbuf_pool_entry, lnode);
        list_move(&ppe->lnode, &use_list);
        pbuf_init(&ppe->pbuf, size);
        pb = &ppe->pbuf;
    }
    
    pthread_mutex_unlock(&pool_mutex);
    
    return pb;
}

void pbuf_free(struct pbuf *pb)
{
    if (atomic_dec_and_test(&pb->refcount)) {
        struct pbuf_pool_entry *ppe;
        ppe = get_enclosing(pb, struct pbuf_pool_entry, pbuf);
        /* Free, i.e., move to free list */
        pthread_mutex_lock(&pool_mutex);
        list_move(&ppe->lnode, &free_list);
        pthread_mutex_unlock(&pool_mutex);
    }
}

/**
 * Reserve space at head of buffer.
 */
unsigned char *pbuf_reserve(struct pbuf *pb, size_t len)
{
    assert(len <= (pb->end - pb->data));
    pb->data += len;
    pb->tail += len;
    return pbuf_data(pb);
}

/**
 * Add data to start of buffer.
 */
unsigned char *pbuf_push(struct pbuf *pb, size_t len)
{
    assert(len <= pb->data);
    pb->data -= len;
    pb->len += len;
    return pbuf_data(pb);
}

/**
 * Remove data to start of buffer.
 */
unsigned char *pbuf_pull(struct pbuf *pb, size_t len)
{
    assert(len <= (pb->end - pb->data));
    pb->data += len;
    pb->len -= len;
    return pbuf_data(pb);
}

/**
 * Add data to end of buffer.
 */
unsigned char *pbuf_put(struct pbuf *pb, size_t len)
{
    unsigned tail = pb->tail;
    assert(len <= (pb->end - pb->tail));
    pb->tail += len;
    pb->len += len;
    return &pb->head[tail];
}

/**
 * Remove data from end of buffer.
 */
void pbuf_trim(struct pbuf *pb, size_t len)
{
    pb->len = len;
    pb->tail = pb->data + len;
}

unsigned pbuf_headroom(struct pbuf *pb)
{
    return pb->data;
}

unsigned pbuf_tailroom(struct pbuf *pb)
{
    return pb->end - pb->tail;
}
