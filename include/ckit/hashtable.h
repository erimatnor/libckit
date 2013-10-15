/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * A thread-safe hash table implementation with reference-counted
 * elements.
 *
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 */
#ifndef CKIT_HASHTABLE_H
#define CKIT_HASHTABLE_H

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

/**
 * Initialize hash table of given size, hash function, equal function
 * and free functions to be applied to hashed elments. Must be called
 * once for every hash table before use.
 */
int hashtable_init(struct hashtable *ht, unsigned int size, 
                   hashfn_t hashfn, equalfn_t equalfn, freefn_t freefn);

/**
 * Cleanup and free hash table.
 */
void hashtable_fini(struct hashtable *ht);

/**
 * Lookup an element in the hash table based on given key. If a
 * corresponding element is returned, it will have its reference count
 * incremented by one, and must thus be followed by a hashelm_put()
 * once the element is no longer used. In case the element is not
 * found, this function returns NULL.
 */
hashelm_t *hashtable_lookup(struct hashtable *ht, const void *key);

/**
 * Returns the number of elementes in the hash table.
 */
unsigned int hashtable_count(struct hashtable *ht);

/**
 * Apply a function to every element in the hash table.
 * A write lock is acquired for each hash slot.
 */
int hashtable_foreach(struct hashtable *ht, 
                      void (*action)(struct hashelm *, void *), 
                      void *data);

/**
 * Apply a function to every element in the hash table.
 * A read lock is acquired for each hash slot.
 */
int hashtable_foreach_read(struct hashtable *ht, 
                           void (*action)(struct hashelm *, void *), 
                           void *data);

/**
 * Insert element into hash table based on given key.
 */
int hashtable_hash(struct hashtable *ht, struct hashelm *he, const void *key);

/**
 * Remove an element from the hash table.
 */
void hashtable_unhash(struct hashtable *ht, struct hashelm *he);

/**
 * Remove an element from the hash table. NOTE: this function will not
 * lock the hash table and is not thread safe.
 */
void __hashtable_unhash(struct hashtable *ht, struct hashelm *he);

/**
 * Check whether an element is hashed or not.
 */
int hashelm_hashed(struct hashelm *he);

/**
 * Increment reference count on hash element.
 */
void hashelm_hold(struct hashelm *he);

/**
 * Decrement reference count on hash element, potentially freeing it.
 */
void hashelm_put(struct hashelm *he);

/**
 * Initialize hash element. Must be called on every element before
 * insertion in hash table.
 */
int hashelm_init(struct hashelm *he);

/**
 * Get the enclosing object that this element is embedded in. 
 */
#define hashtable_entry(he, type, member) \
    get_enclosing(he, type, member)

/**
 * Do a hash table lookup and return the enclosing object that the
 * returned element is embedded in.
 */
#define hashtable_lookup_entry(tbl, key, hashfn, type, member)      \
    get_enclosing(hashtable_lookup(tbl, key, hashfn), type, member)

#endif /* CKIT_HASHTABLE_H */
