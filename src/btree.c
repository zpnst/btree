#include "btree.h"

#include <math.h>
#include <stdio.h>
#include <stddef.h> // for NULL macros


btree btree_new(int degree) {
    if (degree < BTREE_MIN_M) {
        degree = BTREE_MIN_M;
    }

    btree new_btree = {
        .degree = degree, 
        .min_degree = (int)ceilf((float)degree / 2.0f), 
        .root = NULL
    };
    return new_btree;
}

void btree_free(btree *tree) {
    btree_node_free(tree->root, tree);
    tree->root = NULL;
}

void *btree_search(btree tree, int key) {
    int idx;
    btree_node *result = btree_node_search(tree.root, key, &idx);
    return result ? result->values[idx] : NULL;
}

void btree_insert(btree *tree, int key, void *value) {
    if (!tree->root) {
        tree->root = btree_new_node_item(*tree, key, value);
    } else {
        btree_node *new_node = btree_node_insert(tree->root, *tree, key, value);

        if (new_node) { // we reconstruct the three id return value is not NULL, so... we need to update root node
            tree->root = new_node;
        }
    }
}

void btree_delete(btree * tree, int key) {
    if (tree->root) {
        error_t error_code = btree_node_delete(tree->root, *tree, key);
        if (error_code != ERROR_NONE) {
            // there is no such key, just continue
            return;
        }
        if (!tree->root->keys_number) { // tree will shrink if there are no keys in root
            btree_node *previous_root = tree->root;
            tree->root = NULL;
            if (previous_root->children_number) {
                // update root pointer
                tree->root = previous_root->children[0];
            }
            btree_node_free_shallow(previous_root);
        }
    }
}


