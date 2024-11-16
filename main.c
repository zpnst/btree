#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBtree(btree_node *ctx) {
    printf("(");

    for (int iter = 0; iter < ctx->keys_number; iter += 1) {
        if (ctx->childer_number) {
            printBtree(ctx->children[iter]);
        }
        printf(" %d ", ctx->keys[iter]);
    }

    if (ctx->childer_number) { // print the last child
        printBtree(ctx->children[ctx->keys_number]);
    }

    printf(")");
}

int main(int argc, char **argv) {

    btree tree = btree_new(3);

    int keys[7] = {8, 9, 10, 11, 15, 20, 17};

    for (int iter = 0; iter < 7; iter += 1) {
        btree_insert(&tree, keys[iter], NULL);
        printf("+%04d: ", keys[iter]);
        printBtree(tree.root);
        printf("\n");
    }

    btree_free(&tree);

    return EXIT_SUCCESS;
}