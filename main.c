#include "src/btree.h"
#include "visualize/visualize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELETELEN 7
#define INSERTLEN 17

#define BTREE_DEGREE 3

void handle_searching_result(void *searching_result, int key) {
    if (searching_result) {
        printf("result for %d is: %c\n", key, *(char*)searching_result);
    } else {
        printf("result: NULL");
    }
}

int main(void) {

    btree tree = btree_new(BTREE_DEGREE);

    char insert_values[INSERTLEN] = {};
    int insert_keys[INSERTLEN] = {56, 11, 46, 9, 12, 456, 65, 90, 16, 34, 445, 4567, 77, 674, 1245, 245, 67};
    
    for (int iter = 0; iter < INSERTLEN; iter += 1) {   
        insert_values[iter] = (char)insert_keys[iter];
        btree_insert(&tree, insert_keys[iter], &insert_values[iter]);
    }

    void *searching_result_A = btree_search(tree, 65); // A 65 in ASCII table
    void *searching_result_M = btree_search(tree, 77); // M 77 in ASCII table

    handle_searching_result(searching_result_A, 65);
    handle_searching_result(searching_result_M, 77);


    const char *btree_after_deletion = "btree_after_deletion";
    const char *btree_after_insertation = "btree_after_insertation";


    write_btree_txt_file(tree.root, btree_after_insertation);
    write_btree_dot_file(tree.root, btree_after_insertation);
    write_btree_png_image(btree_after_insertation);


    int deleteElements[DELETELEN] = {456, 46, 65, 245, 4567, 67, 11};
    for (int iter = 0; iter < DELETELEN;  iter++) {
        btree_delete(&tree, deleteElements[iter]);
    }


    write_btree_txt_file(tree.root, btree_after_deletion);
    write_btree_dot_file(tree.root, btree_after_deletion);
    write_btree_png_image(btree_after_deletion);

    btree_free(&tree);

    return EXIT_SUCCESS;
}