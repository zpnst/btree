#include "../btree.h"

#include <stddef.h> // for NULL macros

btree_node *btree_node_search(btree_node *ctx, int key, int *idx) {
    for (*idx = 0; *idx < ctx->keys_number && key >= ctx->keys[*idx]; *idx += 1) {
        if (key == ctx->keys[*idx]) { // found
            return ctx;
        } 
    }
    // not found (if internal node, traverse to children)
    return ctx->children_number ? btree_node_search(ctx->children[*idx], key, idx) : NULL;
}