#include "src/btree.h"
#include "visualize/visualize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELETELEN 7
#define INSERTLEN 17

#define BTREE_DEGREE 4

void handle_searching_result(void *searching_result, int key) {
    if (searching_result) {
        printf("result for %d is: %c\n", key, *(char*)searching_result);
    } else {
        printf("result: NULL");
    }
}


void create_png_image_visualization(const char *filename_dot, const char *filename_png) {
    char command[512];

    int ret = snprintf(command, sizeof(command), "dot -Tpng %s -o %s", filename_dot, filename_png);

    if (ret < 0 || ret >= sizeof(command)) {
        fprintf(stderr, "Error in command formation\n");
        return;
    }

    int result = system("dot -Tpng data/btree_after_insertation.dot -o btree.png");
    if (result == -1) {
        perror("Error, exit code: -1");
        return;
    } else if (result != 0) {
        fprintf(stderr, "Error, exit code: %d\n", result);
        return;
    }
}

int main(void) {

    btree tree = btree_new(BTREE_DEGREE);

    char insert_values[INSERTLEN] = {};
    int insert_keys[INSERTLEN] = {25, 8, 37, 245, 564, 21, 466, 567, 67, 77, 65, 6589, 9543, 245, 23, 245, 89};
    
    for (int iter = 0; iter < INSERTLEN; iter += 1) {   
        insert_values[iter] = (char)insert_keys[iter];
        btree_insert(&tree, insert_keys[iter], &insert_values[iter]);
    }

    void *searching_result_A = btree_search(tree, 65); // A 65 in ASCII table
    void *searching_result_M = btree_search(tree, 77); // M 77 in ASCII table

    handle_searching_result(searching_result_A, 65);
    handle_searching_result(searching_result_M, 77);


    write_btree_txt_file(tree.root, "data/btree_after_insertation.txt");
    write_btree_dot_file(tree.root, "data/btree_after_insertation.dot");
    create_png_image_visualization("data/btree_after_insertation.dot", "image/btree_after_insertation.png");



    int deleteElements[DELETELEN] = {567, 21, 25, 8, 89, 23, 245};
    for (int iter = 0; iter < DELETELEN;  iter++) {
        btree_delete(&tree, deleteElements[iter]);
    }

    write_btree_txt_file(tree.root, "data/btree_after_deletion.txt");
    write_btree_dot_file(tree.root, "data/btree_after_deletion.dot");
    create_png_image_visualization("data/btree_after_deletion.dot", "image/btree_after_deletion.png");

    btree_free(&tree);

    return EXIT_SUCCESS;
}