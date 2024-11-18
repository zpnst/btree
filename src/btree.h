#ifndef BTREE_H
#define BTREE_H

#define BTREE_MIN_M 3

typedef struct btree_node {
    int keys_number;              // current number of pairs (key, value)
    int childer_number;           // number of existing chidren (0 if leaf)
    
    int *keys;                    // dynamic array of keys
    void **values;                // parallel array of values

    struct btree_node **children; // children array
} btree_node;

typedef struct btree {
    int degree;
    int min_degree;

    btree_node *root;
} btree;

// BTREE 
btree btree_new(int degree);

void btree_free(btree *tree);
void btree_insert(btree *tree, int key, void *value);
void btree_delete(btree * tree, int key);

void *btree_search(btree tree, int key);


// BTREE_NODE 
btree_node *btree_new_node(btree tree);
btree_node *btree_new_node_item(btree tree, int key, void *value);
btree_node *btree_node_search(btree_node *ctx, int key, int *idx);
btree_node *btree_node_insert(btree_node *ctx, btree tree, int key, void *value);
btree_node *btree_node_get_inorder_predecessor(btree_node *ctx, int idx);

int btree_node_delete(btree_node *ctx, btree tree, int key);

void btree_node_slpit(btree_node *ctx, btree tree, btree_node *new_node, int idx);
void btree_node_free(btree_node *ctx, btree *tree);
void btree_move_kv(btree_node *in, int in_idx, btree_node *out, int out_idx);

void btree_node_free_shallow(btree_node *ctx);
void btree_node_rebalance(btree_node *ctx, btree tree, int key);

#endif