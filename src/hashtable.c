/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- 
 *
 * A thread-safe hash table implementation with reference-counted
 * elements.
 *
 * Authors: Erik Nordström <erik.nordstrom@gmail.com>
 * 
 */
#include <stdlib.h>
#include <pthread.h>
#include <ckit/hashtable.h>
#include <ckit/debug.h>

struct hashslot {
    list_t head;
	unsigned long count;
	pthread_mutex_t lock;
};

/*
  Hash table initialization.
*/
int hashtable_init(struct hashtable *ht, unsigned int size,
                   hashfn_t hashfn, equalfn_t equalfn, freefn_t freefn)
{
    int i;
    memset(ht, 0, sizeof(*ht));
    ht->table = malloc(sizeof(struct hashslot) * size);
    
    if (!ht->table)
        return -1;
    
    ht->mask = size - 1;
    atomic_set(&ht->count, 1);
    ht->hashfn = hashfn;
    ht->equalfn = equalfn;
    ht->freefn = freefn;

    /* LOG_DBG("Initializing hash table\n"); */

	for (i = 0; i <= ht->mask; i++) {
		INIT_LIST(&ht->table[i].head);
		ht->table[i].count = 0;
		pthread_mutex_init(&ht->table[i].lock, NULL);
	}

    return 0;
}

void hashtable_fini(struct hashtable *ht)
{
    int i;

    for (i = 0; i <= ht->mask; i++) {
        struct hashelm *he;
        
        pthread_mutex_lock(&ht->table[i].lock);

        while (!list_empty(&ht->table[i].head)) {
            he = list_front(&ht->table[i].head, struct hashelm, list);
            list_del(&he->list);
            hashelm_put(he);
        }

		INIT_LIST(&ht->table[i].head);
		ht->table[i].count = 0;
        pthread_mutex_unlock(&ht->table[i].lock);
		pthread_mutex_destroy(&ht->table[i].lock);
	}
    atomic_set(&ht->count, 0);
    free(ht->table);
}

int hashtable_foreach(struct hashtable *ht, 
                      void (*action)(struct hashelm *, void *), 
                      void *data)
{
    int n = 0;
    unsigned i;

    if (!action)
        return -1;

    for (i = 0; i <= ht->mask; i++) {
        struct hashelm *he;
        
        pthread_mutex_lock(&ht->table[i].lock);
        
        list_foreach(he, &ht->table[i].head, list) {
            action(he, data);
            n++;
        }

        pthread_mutex_unlock(&ht->table[i].lock);
	}
    return n;
}

unsigned int hashtable_count(struct hashtable *ht)
{
    return atomic_read(&ht->count);
}

static struct hashslot *get_slot(struct hashtable *tbl,
                                 unsigned int hash)
{
    return &tbl->table[hash & tbl->mask];
}

int hashelm_hashed(struct hashelm *he)
{
    return !list_empty(&he->list);
}

int hashtable_hash(struct hashtable *ht, struct hashelm *he, 
                   const void *key)
{
    struct hashslot *slot;
    struct hashelm *he2;

    if (!list_empty(&he->list)) {
        LOG_ERR("Hash element already hashed\n");
        return -1;
    }

    he->hash = ht->hashfn(key);
    he->ht = ht;
    he->key = key;
    slot = get_slot(ht, he->hash);
    pthread_mutex_lock(&slot->lock);

    list_foreach(he2, &slot->head, list) {
        if (ht->equalfn(he2, key)) {
            pthread_mutex_unlock(&slot->lock);
            return -1;
        }
    }
    slot->count++;
    atomic_inc(&ht->count);
    list_add_front(&slot->head, &he->list);
    hashelm_hold(he);
    pthread_mutex_unlock(&slot->lock);

    return 0;
}

static void __unhash(struct hashtable *ht, 
                     struct hashslot *slot, 
                     struct hashelm *he)
{
    list_del(&he->list);
    slot->count--;
    atomic_dec(&ht->count);
    hashelm_put(he);    
}

void hashtable_unhash(struct hashtable *ht, struct hashelm *he)
{
    struct hashslot *slot;
    slot = get_slot(ht, he->hash);
    pthread_mutex_lock(&slot->lock);
    __unhash(ht, slot, he);
    pthread_mutex_unlock(&slot->lock);
}

void __hashtable_unhash(struct hashtable *ht, struct hashelm *he)
{
    __unhash(ht, get_slot(ht, he->hash), he);
}

void hashelm_hold(struct hashelm *he)
{
    atomic_inc(&he->refcount);
}

void hashelm_put(struct hashelm *he)
{
    if (atomic_dec_and_test(&he->refcount)) {
        if (he->ht->freefn) {
            he->ht->freefn(he);
        }
    }
}

int hashelm_init(struct hashelm *he)
{
    INIT_LIST(&he->list);
    atomic_set(&he->refcount, 1);
    return 0;
}

struct hashelm *hashtable_lookup(struct hashtable *ht, const void *key)
{
    struct hashelm *he;
    struct hashslot *slot;

    slot = get_slot(ht, ht->hashfn(key));
    
    pthread_mutex_lock(&slot->lock);
    
    list_foreach(he, &slot->head, list) {
        if (ht->equalfn(he, key)) {
            hashelm_hold(he);
            goto found;
        }
    }
    he = NULL;
found:
    pthread_mutex_unlock(&slot->lock);
    
    return he;
}
