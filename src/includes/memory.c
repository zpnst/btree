#include "../btree.h"

#include <stdlib.h>

btree_node *btree_new_node(btree tree) {
    btree_node *new_tree = (btree_node*)malloc(sizeof(btree_node));

    new_tree->keys_number = 0;
    new_tree->childer_number = 0;

    new_tree->keys = (int*)malloc(sizeof(int) * (tree.degree - 1));
    new_tree->values = (void**)malloc(sizeof(void*) * (tree.degree - 1));
    new_tree->children = (btree_node**)malloc(sizeof(btree_node*) * tree.degree);

    return new_tree;

}

btree_node *btree_new_node_item(btree tree, int key, void *value) {
    btree_node *new_tree = btree_new_node(tree);

    new_tree->keys[0] = key;
    new_tree->values[0] = value;
    new_tree->keys_number = 1;

    return new_tree;
}

void btree_node_free(btree_node *ctx, btree *tree) {
    if (ctx) {
        for (int iter = 0; iter < ctx->childer_number; iter += 1) {
            btree_node_free(ctx->children[iter], tree);
        }

        free(ctx->keys);
        free(ctx->values);
        free(ctx->children);

        free(ctx);
    }
}