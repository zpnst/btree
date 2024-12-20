#include "../btree.h"

#include <stdlib.h>
#include <stddef.h> // for NULL macros

void btree_move_kv(btree_node *in, int in_idx, btree_node *out, int out_idx) { 
    out->keys[out_idx] = in->keys[in_idx]; // simple swap of nodes key, value pairs
    out->values[out_idx] = in->values[in_idx];
}

void btree_node_free_shallow(btree_node *ctx) {
    if (ctx) { // free single node if node not NULL
        free(ctx->children);
        free(ctx->keys);
        free(ctx->values);

        free(ctx);
    }
}
btree_node *btree_node_get_inorder_predecessor(btree_node *ctx, int idx) {
    if (ctx->children_number) {
        btree_node *current = ctx->children[idx];

        // traverse to leaves
        while (current->children_number) {
            // traverse through last child
            current = current->children[current->children_number - 1];
        }
        // current contains inorder predecessor as the last key
        return current;
    } else {
        return NULL;
    }
}


