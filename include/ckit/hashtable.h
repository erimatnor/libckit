/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * A hash table implementation with reference-counted elements.
 *
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 */
#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <ckit/hash.h>
#include <ckit/list.h>
#include <ckit/atomic.h>
#include <string.h>

struct hashelm;

typedef unsigned int (*hashfn_t)(const void *key);
typedef int (*equalfn_t)(const struct hashelm *elm, const void *key);
typedef void (*freefn_t)(struct hashelm *elm);

typedef struct hashelm {
	list_t list;
    atomic_t refcount;
    unsigned int hash;
	void *key;
	unsigned long keylen;
    hashfn_t hashfn;
    equalfn_t equalfn;
    freefn_t freefn;
} hashelm_t;

#define HTABLE_MIN_SIZE 32

typedef struct hashtable {
	struct hashslot *hash;
    unsigned int mask;
    atomic_t count;
} hashtable_t;

static inline unsigned int default_hashfn(const void *key)
{
    return *((unsigned int *)key);
}

static inline int default_equalfn(const hashelm_t *e, const void *key)
{
    return memcmp(e->key, key, e->keylen) == 0;
}

int hashtable_init(struct hashtable *table, unsigned int size);
void hashtable_fini(struct hashtable *table);
hashelm_t *hashtable_lookup(struct hashtable *table, const void *key,
                            hashfn_t hashfn);
unsigned int hashtable_count(struct hashtable *table);
int hashtable_foreach(struct hashtable *table, 
                      void (*action)(struct hashelm *, void *), 
                      void *data);
int hashelm_hashed(struct hashelm *he);
int hashelm_hash(struct hashtable *table, struct hashelm *he, const void *key);
void __hashelm_unhash(struct hashtable *table, struct hashelm *he);
void hashelm_unhash(struct hashtable *table, struct hashelm *he);
void hashelm_hold(struct hashelm *he);
void hashelm_put(struct hashelm *he);
int hashelm_init(struct hashelm *he, hashfn_t hashfn, 
                 equalfn_t equalfn, freefn_t freefn);

#define hashtable_lookup_entry(tbl, key, hashfn, type, member)      \
    get_enclosing(hashtable_lookup(tbl, key, hashfn), type, member)

#endif /* __HASHTABLE_H__ */
