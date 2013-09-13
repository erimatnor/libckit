/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef __PBUF_H__
#define __PBUF_H__

#include <ckit/atomic.h>

#define CTRL_BLOCK_SIZE 20

/* Packet buffer, with similar semantics to Linux sk_buffs. */
typedef struct pbuf {    
    atomic_t refcount;
    unsigned ifindex;
    unsigned char cb[CTRL_BLOCK_SIZE];
    size_t alloc_len;
    size_t len;
    unsigned data, tail, end;
    unsigned link_offset;
    unsigned network_offset;
    unsigned transport_offset;
    unsigned payload_offset;
    unsigned char head[0];
} pbuf_t;

int pbuf_pool_init(size_t pool_size);
void pbuf_pool_cleanup(void);

#define PBUF_MAX_SIZE 4096
#define PBUF_MTU_SIZE 1510

struct pbuf *pbuf_alloc(size_t size);
void pbuf_free(struct pbuf *pb);
struct pbuf *pbuf_copy(struct pbuf *pb);

static inline unsigned char *pbuf_data(struct pbuf *pb)
{
    return &pb->head[pb->data];
}

static inline void pbuf_hold(struct pbuf *pb)
{
    atomic_inc(&pb->refcount);
}

/**
 * Reserve space at head of buffer.
 */
unsigned char *pbuf_reserve(struct pbuf *pb, size_t len);

/**
 * Add data to start of buffer.
 */
unsigned char *pbuf_push(struct pbuf *pb, size_t len);

/**
 * Remove data to start of buffer.
 */
unsigned char *pbuf_pull(struct pbuf *pb, size_t len);

/**
 * Add data to end of buffer.
 */
unsigned char *pbuf_put(struct pbuf *pb, size_t len);

/**
 * Remove data from end of buffer.
 */
void pbuf_trim(struct pbuf *pb, size_t len);

/**
 * Return space at head.
 */
unsigned pbuf_headroom(struct pbuf *pb);

/**
 * Return space at tail.
 */
unsigned pbuf_tailroom(struct pbuf *pb);

static inline unsigned char *pbuf_link_header(struct pbuf *pb)
{
    return &pb->head[pb->link_offset];
}

static inline unsigned char *pbuf_network_header(struct pbuf *pb)
{
    return &pb->head[pb->network_offset];
}

static inline unsigned char *pbuf_transport_header(struct pbuf *pb)
{
    return &pb->head[pb->transport_offset];
}

static inline unsigned char *pbuf_payload(struct pbuf *pb)
{
    return &pb->head[pb->payload_offset];
}

static inline void pbuf_reset_link_header(struct pbuf *pb)
{
    pb->link_offset = pb->data;
}

static inline void pbuf_reset_network_header(struct pbuf *pb)
{
    pb->network_offset = pb->data;
}

static inline void pbuf_reset_transport_header(struct pbuf *pb)
{
    pb->transport_offset = pb->data;
}

static inline void pbuf_reset_payload(struct pbuf *pb)
{
    pb->payload_offset = pb->data;
}

#endif /* __PBUF_H__ */
