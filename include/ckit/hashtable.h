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

struct hashtable;

typedef struct hashelm {
	list_t list;
    atomic_t refcount;
    unsigned int hash;
	const void *key;
    struct hashtable *ht;
} hashelm_t;

typedef unsigned int (*hashfn_t)(const void *key);
typedef int (*equalfn_t)(const struct hashelm *elm, const void *key);
typedef void (*freefn_t)(struct hashelm *elm);

#define HTABLE_MIN_SIZE 32

typedef struct hashtable {
	struct hashslot *table;
    unsigned int mask;
    atomic_t count;
    hashfn_t hashfn;
    equalfn_t equalfn;
    freefn_t freefn;
} hashtable_t;

static inline unsigned int default_hashfn(const void *key)
{
    return *((unsigned int *)key);
}

int hashtable_init(struct hashtable *ht, unsigned int size, 
                   hashfn_t hashfn, equalfn_t equalfn, freefn_t freefn);
void hashtable_fini(struct hashtable *ht);
hashelm_t *hashtable_lookup(struct hashtable *ht, const void *key);
unsigned int hashtable_count(struct hashtable *ht);
int hashtable_foreach(struct hashtable *ht, 
                      void (*action)(struct hashelm *, void *), 
                      void *data);
int hashtable_hash(struct hashtable *ht, struct hashelm *he, const void *key);
void hashtable_unhash(struct hashtable *ht, struct hashelm *he);
int hashelm_hashed(struct hashelm *he);
void __hashelm_unhash(struct hashtable *ht, struct hashelm *he);
void hashelm_hold(struct hashelm *he);
void hashelm_put(struct hashelm *he);
int hashelm_init(struct hashelm *he);

#define hashtable_entry(he, type, member) \
    get_enclosing(he, type, member)

#define hashtable_lookup_entry(tbl, key, hashfn, type, member)      \
    get_enclosing(hashtable_lookup(tbl, key, hashfn), type, member)

#endif /* __HASHTABLE_H__ */
