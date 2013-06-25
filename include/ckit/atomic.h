/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef __ATOMIC_H__
#define __ATOMIC_H__

typedef struct atomic {
    int value;
} atomic_t;

#define atomic_set(a, val) ({                   \
            __sync_synchronize();               \
            (a)->value = val; })
#define atomic_inc(a)                           \
    __sync_add_and_fetch(&(a)->value, 1)
#define atomic_dec(a)                           \
    __sync_sub_and_fetch(&(a)->value, 1)
#define atomic_read(a) ({                       \
            __sync_synchronize();               \
            (a)->value; })
#define atomic_dec_and_test(a)                  \
    (atomic_dec(a) == 0)

#endif /* __ATOMIC_H__ */
