/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ckit/rbtree.h>
#include <ckit/debug.h>

#define KEY_DATA_OFFSET(node) ((unsigned char *)(node) + sizeof(*(node)))
#define VAL_DATA_OFFSET(node) ((unsigned char *)(node) + sizeof(*(node)) + \
                               (node)->key.len)

static int default_comp(rb_key_t *k1, rb_key_t *k2)
{
	if (k1->len != k2->len)
		return k1->len < k2->len;
	
	return memcmp(k1->data, k2->data, k1->len);
}

static const char *default_key_print(rb_key_t *k)
{
	return "";
}

static const char *default_value_print(rb_value_t *v)
{
    return "";
}

void default_key_free(rb_key_t *k)
{
    if (k->data) {
        free(k->data);
        k->data = NULL;
    }
}

void default_value_free(rb_value_t *v)
{
    if (v->data) {
        free(v->data);
        v->data = NULL;
    }
}

const rb_node_ops_t default_tree_ops = {
	.key_compare = default_comp,
	.key_print = default_key_print,
	.value_print = default_value_print,
    .key_free = default_key_free,
    .value_free = default_value_free,
};

rb_node_t *rb_node_alloc(void *key, size_t key_len,
						 void *val, size_t val_len,
						 rb_node_t *parent)
{
    rb_node_t *n;
    size_t tot_len = sizeof(*n) + key_len + val_len;
    
    n = malloc(tot_len);
    
    if (!n)
        return n;
    
    memset(n, 0, tot_len);
    n->key.len = key_len;
    n->val.len = val_len;
    /*
      n->key.data = KEY_DATA_OFFSET(n);
      n->val.data = VAL_DATA_OFFSET(n);
      memcpy(n->key.data, key, key_len);
      memcpy(n->val.data, val, val_len);
    */
    n->key.data = key;
    n->val.data = val;
    
    n->left = NULL;
    n->right = NULL;
    n->color = RB_COLOR_RED;
    n->parent = parent;
    
    if (val_len == sizeof(int))
		memcpy(&n->debug, val, val_len);
    
    return n;
}

/**
   Insert value.
 
   Returns 1 if value was inserted, 0 if the value is already stored,
   and -1 in case of error (e.g., memory allocation failure).
*/
int bst_insert(rb_tree_t *tree, void *key, size_t key_len,
               void *val, size_t val_len, rb_node_t **new_node)
{
	rb_node_t *parent = NULL, *curr = tree->root;
	rb_key_t insert_key = { .data = (void *)key, .len = key_len };
	
	if (!curr) {
		/* Insert root */
		tree->root = rb_node_alloc(key, key_len, val, val_len, parent);
		
		if (!tree->root)
			return -1;
        
		if (new_node)
			*new_node = tree->root;
		
		LOG_DBG("Added new root\n");
		
		return 1;
	}
    
	while (1) {
	    int comp;
	    
	    parent = curr;
	    
	    comp = tree->ops->key_compare(&insert_key, &curr->key);
	    
	    /*
          if (comp == 0) {
          //LOG_DBG("element exists\n");
          return 0;
          } else
        */
	    if (comp < 0) {
            curr = curr->left;
            //LOG_DBG("\tgoing left\n");
            
            if (!curr) {
                /* Insert new node at found location */
                curr = rb_node_alloc(key, key_len, val, val_len, parent);
                parent->left = curr;
                break;
            }
	    } else {
            curr = curr->right;
            //LOG_DBG("\tgoing right\n");
            
            if (!curr) {
                /* Insert new node at found location */
                curr = rb_node_alloc(key, key_len, val, val_len, parent);
                parent->right = curr;
                break;
            }
	    }
	}
    
	LOG_DBG("\tnew node: %s\n", tree->ops->value_print(&curr->val));
	LOG_DBG("\tparent: %s\n", tree->ops->value_print(&parent->val));
	
	if (new_node)
		*new_node = curr;
	
	return curr ? 1 : -1;
}

static inline rb_node_t *grandparent(rb_node_t *node)
{
    if (node && node->parent)
        return node->parent->parent;
    return NULL;
}

static inline rb_node_t *uncle(rb_node_t *node)
{
	rb_node_t *gp = grandparent(node);
	
	if (gp == NULL)
		return NULL;
	
	if (node->parent == gp->left)
		return gp->right;
	
	return gp->left;
}

static inline rb_node_t *sibling(rb_node_t *node)
{
    if (node && node->parent) {
        if (node->parent->right != node)
            return node->parent->right;
        else
            return node->parent->left;
    }
    return NULL;
}

static void rb_tree_rotate_right_side_right(rb_node_t *node)
{
	if (node && node->left) {
		rb_node_t *child = node->left;
		rb_node_t *parent = node->parent;
        /*
		LOG_DBG("rotate right side right node=%d child=%d parent=%d\n",
			   node->debug, child->debug, parent->debug);
        */
		parent->right = child;
		child->parent = parent;
		node->parent = child;
		node->left = child->right;
		child->right = node;
	}
}

static void rb_tree_rotate_left_side_right(rb_node_t *node)
{
	if (node && node->left) {
		rb_node_t *child = node->left;
		rb_node_t *parent = node->parent;
		LOG_DBG("rotate left side right node=%d child=%d parent=%d\n",
			   node->debug, child->debug, parent->debug);
		parent->left = child;
		child->parent = parent;
		node->parent = child;
		node->left = child->right;
		child->right = node;
	}
}

static void rb_tree_rotate_right_side_left(rb_node_t *node)
{
	if (node && node->right) {
		rb_node_t *child = node->right;
		rb_node_t *parent = node->parent;
		LOG_DBG("rotate right side left node=%d child=%d parent=%d\n",
			   node->debug, child->debug, parent->debug);
		parent->right = child;
		child->parent = parent;
		node->parent = child;
		node->right = child->left;
		child->left = node;
	}
}

static void rb_tree_rotate_left_side_left(rb_node_t *node)
{
	if (node && node->right) {
		rb_node_t *child = node->right;
		rb_node_t *parent = node->parent;
		LOG_DBG("rotate left side left node=%d child=%d parent=%d\n",
			   node->debug, child->debug, parent->debug);
		parent->left = child;
		child->parent = parent;
		node->parent = child;
		node->right = child->left;
		child->left = node;
	}
}

static void rb_tree_insert_case1(rb_node_t *node);

static void rb_tree_insert_case5(rb_node_t *node)
{
	rb_node_t *g = grandparent(node);
	
	//LOG_DBG("inserting case 5\n");
	
	node->parent->color = RB_COLOR_BLACK;
	g->color = RB_COLOR_RED;
	
	if (node->parent->left == node) {
		rb_tree_rotate_left_side_right(g);
	} else {
		rb_tree_rotate_right_side_left(g);
	}
}

static void rb_tree_insert_case4(rb_node_t *node)
{
	rb_node_t *g = grandparent(node);
	
	/*
      LOG_DBG("inserting case 4 parent_c=%d uncle_c=%d\n",
      node->parent->color, uncle(node) ? uncle(node)->color : 0);
    */
	if (node->parent->right == node && node->parent == g->left) {
		rb_tree_rotate_left_side_left(node->parent);
		node = node->left;
	} else if (node->parent->left == node && node->parent == g->right) {
		rb_tree_rotate_right_side_right(node->parent);
		node = node->right;
	}
	if (node)
		rb_tree_insert_case5(node);
}

static void rb_tree_insert_case3(rb_node_t *node)
{
	rb_node_t *u = uncle(node);
	
	//LOG_DBG("inserting case 3\n");
	
	if (u && u->color == RB_COLOR_RED) {
		rb_node_t *g;
		node->parent->color = RB_COLOR_BLACK;
		u->color = RB_COLOR_BLACK;
		g = grandparent(node);
		g->color = RB_COLOR_RED;
		rb_tree_insert_case1(g);
	} else {
		rb_tree_insert_case4(node);
	}
}

static void rb_tree_insert_case2(rb_node_t *node)
{
	//LOG_DBG("inserting case 2\n");
	
	if (node->parent->color == RB_COLOR_BLACK)
		return;
	rb_tree_insert_case3(node);
}

static void rb_tree_insert_case1(rb_node_t *node)
{
	//LOG_DBG("inserting case 1\n");
	
	if (node->parent == NULL) {
		/* Root is always black */
		node->color = RB_COLOR_BLACK;
	} else {
		rb_tree_insert_case2(node);
	}
}

int _rb_tree_insert(rb_tree_t *tree, void *key, size_t key_len,
                    void *val, size_t val_len)
{
	rb_node_t *new_node = NULL;
	int ret;
	
	ret = bst_insert(tree, key, key_len, val, val_len, &new_node);
	
	if (ret <= 0) {
		fLOG_DBG(stderr, "inserting error\n");
		return ret;
	}
	
	rb_tree_insert_case1(new_node);
	/*
      LOG_DBG("\tinserted %s color=%s\n",
      tree->ops->value_print(&new_node->val),
      new_node->color ? "red" : "black");
    */
	return ret;
}

int rb_tree_insert(rb_tree_t *tree, const void *key, size_t key_len,
                   const void *val, size_t val_len)
{
    void *key_copy, *val_copy;
    
    key_copy = malloc(key_len);
    
    if (!key_copy)
        return -1;
    
    val_copy = malloc(val_len);
    
    if (!val_copy)
        return -1;
    
    memcpy(key_copy, key, key_len);
    memcpy(val_copy, val, val_len);
    
    return rb_tree_insert(tree, key_copy, key_len, val_copy, val_len);
}

int rb_tree_delete(rb_tree_t *tree, const void *key, size_t key_len)
{
    
}

static void rb_tree_node_destroy(rb_tree_t *tree, rb_node_t *node)
{
    if (!node)
        return;
    
    rb_tree_node_destroy(tree, node->left);
    rb_tree_node_destroy(tree, node->right);
    
    if (tree->ops->key_free)
        tree->ops->key_free(&node->key);
    
    if (tree->ops->value_free)
        tree->ops->value_free(&node->val);
    
    free(node);
}

void rb_tree_destroy(rb_tree_t *tree)
{
    rb_tree_node_destroy(tree, tree->root);
}

void rb_tree_print_in_order(rb_tree_t *tree)
{
	rb_node_t *curr, *prev;
	
	if (tree == NULL || tree->root == NULL)
		return;
	
	curr = tree->root;
	
	while (curr != NULL) {
		if (curr->left == NULL) {
			LOG_DBG("%s color=%s\n", tree->ops->value_print(&curr->val),
				   curr->color ? "red" : "black");
			curr = curr->right;
		} else {
			prev = curr->left;
			
			while (prev->right != NULL && prev->right != curr)
				prev = prev->right;
			
			if (prev->right == NULL) {
				prev->right = curr;
				curr = curr->left;
			} else {
				prev->right = NULL;
				LOG_DBG("%s color=%s\n", tree->ops->value_print(&curr->val),
					   curr->color ? "red" : "black" );
				curr = curr->right;
			}
		}
	}
}

