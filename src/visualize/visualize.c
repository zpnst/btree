#include "visualize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// .dot section for graphviz .dot files
void construct_btree_dot_file(btree_node *node, FILE *output_file) {
    static int nullcount = 0; 

    if (!node) return;

    fprintf(output_file, "    node%p [label=\"", (void *)node);
    for (int iter = 0; iter < node->keys_number; iter += 1) {
        fprintf(output_file, "<f%d>%d|", iter, node->keys[iter]);
    }
    fprintf(output_file, "<f%d>\"];\n", node->keys_number);

    for (int iter = 0; iter <= node->keys_number; iter += 1) { 
        if (iter >= node->childer_number) break; 

        if (node->children[iter]) {
            fprintf(output_file, "    \"node%p\":f%d -> \"node%p\";\n", (void *)node, iter, (void *)node->children[iter]);
            construct_btree_dot_file(node->children[iter], output_file);
        } else {
            fprintf(output_file, "    null%d [shape=point];\n", nullcount);
            fprintf(output_file, "    \"node%p\":f%d -> null%d;\n", (void *)node, iter, nullcount);
            nullcount += 1;
        }
    }
}

void write_btree_dot_file(btree_node *root, const char *filename) {

    char data_filename[FILENAME_SIZE];
    unsigned long error = snprintf(data_filename, sizeof(data_filename), "%s/%s.dot", DATA_DIR, filename);

    if (error < 0 || error >= sizeof(data_filename)) {
        fprintf(stderr, "Error in generating dot vizualization\n");
        return;
    }

    FILE *output_file = fopen(data_filename, "w");
    if (!output_file) {
        perror("Failed to open file");
        return;
    }

    fprintf(output_file, "digraph btree {\n");
    fprintf(output_file, "    node [shape=record, height=.1];\n");
    construct_btree_dot_file(root, output_file);
    fprintf(output_file, "}\n");

    fclose(output_file);
}

// .txt section
void construct_btree_txt_file(btree_node *node, int indent, FILE *output_file) {
    if (!node) return;

    for (int iter = 0; iter < indent; iter += 1) { // print spaces
        fprintf(output_file, " ");
    }

    fprintf(output_file, "[");
    for (int iter = 0; iter < node->keys_number; iter += 1) {
        fprintf(output_file, "%d", node->keys[iter]);
        if (iter < node->keys_number - 1) {
            fprintf(output_file, " ");
        }
    }
    fprintf(output_file, "]\n");

    if (node->childer_number) {
        for (int iter = 0; iter <= node->keys_number; iter += 1) {
            construct_btree_txt_file(node->children[iter], indent + 4, output_file);
        }
    }
}

void write_btree_txt_file(btree_node *root, const char *filename) {

    char data_filename[FILENAME_SIZE];
    unsigned long error = snprintf(data_filename, sizeof(data_filename), "%s/%s.txt", DATA_DIR, filename);

    if (error < 0 || error >= sizeof(data_filename)) {
        fprintf(stderr, "Error in generating txt vizualization\n");
        return;
    }

    FILE *output_file = fopen(data_filename, "w");
    if (!output_file) {
        perror("Failed to open file");
        return;
    }
    construct_btree_txt_file(root, 0, output_file);
    fclose(output_file);
}

// .png section
void write_btree_png_image(const char *filename) {

    char data_filename_dot[FILENAME_SIZE];
    char data_filename_png[FILENAME_SIZE];
    char generating_command[COMMAND_BUFFER_SIZE];

    unsigned long error;

    error = snprintf(data_filename_dot, sizeof(data_filename_dot), "%s/%s.dot", DATA_DIR, filename);

    if (error < 0 || error >= sizeof(data_filename_dot)) {
        fprintf(stderr, "Error while read dot file\n");
        return;
    }

    error = snprintf(data_filename_png, sizeof(data_filename_png), "%s/%s.png", IMAGE_DIR, filename);

    if (error < 0 || error >= sizeof(data_filename_png)) {
        fprintf(stderr, "Error in generating data png image\n");
        return;
    }

    error = snprintf(generating_command, sizeof(generating_command), "dot -Tpng %s -o %s", data_filename_dot, data_filename_png);

    if (error < 0 || error >= sizeof(generating_command)) {
        fprintf(stderr, "Error in generating command formation\n");
        return;
    }

    int result = system(generating_command);
    if (result == -1) {
        perror("Error, exit code: -1 while exec system()");
        return;
    } else if (result != 0) {
        fprintf(stderr, "Error while exec system(), exit code: %d\n", result);
        return;
    }
}