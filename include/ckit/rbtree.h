/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef __rb_tree_h__
#define __rb_tree_h__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RB_COLOR_BLACK = 0,
    RB_COLOR_RED,
} rb_color_t;
    
typedef struct rb_key {
    size_t len;
    void *data;
} rb_key_t;

typedef struct rb_value {
    size_t len;
    void *data;
} rb_value_t;

typedef struct rb_node {
    rb_color_t color; 
    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *parent;
    int debug;
    rb_key_t key;
    rb_value_t val;
} rb_node_t;

typedef struct rb_node_ops {
    int (*key_compare)(rb_key_t *k1, rb_key_t *k2);
    const char *(*key_print)(rb_key_t *k);
    const char *(*value_print)(rb_value_t *v);
    void (*key_free)(rb_key_t *k);
    void (*value_free)(rb_value_t *k);
} rb_node_ops_t;

typedef struct rb_tree {
    struct rb_node *root;
    const struct rb_node_ops *ops;
    size_t size;
} rb_tree_t;

extern const rb_node_ops_t default_tree_ops;
    
#define DEFINE_TREE(name, setops) \
    rb_tree_t name = { .root = NULL, .ops = setops, .size = 0 }

void default_key_free(rb_key_t *k);
void default_value_free(rb_value_t *v);
    
int _rb_tree_insert(rb_tree_t *tree, void *key, size_t key_len,
                    void *val, size_t val_len);

int rb_tree_insert(rb_tree_t *tree, const void *key, size_t key_len, 
                   const void *val, size_t val_len);

void rb_tree_destroy(rb_tree_t *tree);

void rb_tree_print_in_order(rb_tree_t *tree);

#ifdef __cplusplus
}
#endif
        
#endif __rb_tree_h__
