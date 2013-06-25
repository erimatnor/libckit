/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- 
 *
 * A hash table implementation with reference-counted elements.
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
int hashtable_init(struct hashtable *table, 
                   unsigned int size)
{
    int i;

    memset(table, 0, sizeof(*table));
    table->hash = malloc(sizeof(struct hashslot) * size);
    
    if (!table->hash)
        return -1;
    
    table->mask = size - 1;
    atomic_set(&table->count, 0);

    /* LOG_DBG("Initializing hash table\n"); */

	for (i = 0; i <= table->mask; i++) {
		LIST_INIT(&table->hash[i].head);
		table->hash[i].count = 0;
		pthread_mutex_init(&table->hash[i].lock, NULL);
	}

    return 0;
}

void hashtable_fini(struct hashtable *table)
{
    int i;

    for (i = 0; i <= table->mask; i++) {
        struct hashelm *he;
        
        pthread_mutex_lock(&table->hash[i].lock);

        while (!list_empty(&table->hash[i].head)) {
            he = list_front(&table->hash[i].head, struct hashelm, list);
            list_del(&he->list);
            hashelm_put(he);
        }

		LIST_INIT(&table->hash[i].head);
		table->hash[i].count = 0;
        pthread_mutex_unlock(&table->hash[i].lock);
		pthread_mutex_destroy(&table->hash[i].lock);
	}
    atomic_set(&table->count, 0);
    free(table->hash);
}

int hashtable_foreach(struct hashtable *table, 
                       void (*action)(struct hashelm *, void *), 
                       void *data)
{
    int i, n = 0;

    if (!action)
        return -1;

    for (i = 0; i <= table->mask; i++) {
        struct hashelm *he;
        
        pthread_mutex_lock(&table->hash[i].lock);
        
        list_foreach(he, &table->hash[i].head, list) {
            action(he, data);
            n++;
        }

        pthread_mutex_unlock(&table->hash[i].lock);
	}
    return n;
}

unsigned int hashtable_count(struct hashtable *table)
{
    return atomic_read(&table->count);
}

static struct hashslot *get_slot(struct hashtable *tbl,
                                    unsigned int hash)
{
    return &tbl->hash[hash & tbl->mask];
}

int hashelm_hashed(struct hashelm *he)
{
    return !list_empty(&he->list);
}

int hashelm_hash(struct hashtable *table, struct hashelm *he, 
                 const void *key)
{
    struct hashslot *slot;

    if (!hlist_unhashed(&he->list)) {
        LOG_ERR("Hash element already hashed\n");
        return -1;
    }

    he->hash = he->hashfn(key);
    slot = get_slot(table, he->hash);
    
    pthread_mutex_lock(&slot->lock);
    slot->count++;
    atomic_inc(&table->count);
    hlist_add_head(&he->list, &slot->head);
    hashelm_hold(he);
    pthread_mutex_unlock(&slot->lock);

    return 0;
}

void hashelm_unhash(struct hashtable *table, struct hashelm *he)
{
    struct hashslot *slot;    

    slot = get_slot(table, he->hash);
    pthread_mutex_lock(&slot->lock);
    hlist_del_init(&he->list);
    slot->count--;
    atomic_dec(&table->count);
    hashelm_put(he);
    pthread_mutex_unlock(&slot->lock);
}

void __hashelm_unhash(struct hashtable *table, struct hashelm *he)
{
    struct hashslot *slot;    

    slot = get_slot(table, he->hash);
    hlist_del_init(&he->list);
    slot->count--;
    atomic_dec(&table->count);
    hashelm_put(he);
}

void hashelm_hold(struct hashelm *he)
{
    atomic_inc(&he->refcount);
}

void hashelm_put(struct hashelm *he)
{
    if (atomic_dec_and_test(&he->refcount)) 
        if (he->freefn) {
            he->freefn(he);
        }
}

int hashelm_init(struct hashelm *he,
                 hashfn_t hashfn, 
                 equalfn_t equalfn, 
                 freefn_t freefn)
{
    INIT_HLIST_LIST(&he->list);
    atomic_set(&he->refcount, 1);
    he->hashfn = hashfn;
    he->equalfn = equalfn;
    he->freefn = freefn;

    return 0;
}

struct hashelm *hashtable_lookup(struct hashtable *table, 
                                 const void *key, hashfn_t hashfn)
{
    struct hashelm *he;
    struct hashslot *slot;

    slot = get_slot(table, hashfn(key));
    
    pthread_mutex_lock(&slot->lock);
    
    list_foreach(he, &slot->head, list) {
        if (he->equalfn(he, key)) {
            hashelm_hold(he);
            goto found;
        }
    }
    he = NULL;
found:
    pthread_mutex_unlock(&slot->lock);
    
    return he;
}
