#include "visualize.h"

#include <stdio.h>

// .dot section for graphviz .dot files
void construct_btree_dot_file(btree_node *node, FILE *output_file) {
    static int nullcount = 0;
    if (!node) return;
    fprintf(output_file, "    node%p [label=\"", (void*)node);
    for (int i = 0; i < node->keys_number; i++) {
        fprintf(output_file, "<f%d>%d", i, node->keys[i]);
        if (i < node->keys_number - 1) {
            fprintf(output_file, "|");
        }
    }
    fprintf(output_file, "\"];\n");

    if (node->childer_number) {
        for (int i = 0; i <= node->keys_number; i++) {
            if (node->children[i]) {
                fprintf(output_file, "    \"node%p\":f%d -> \"node%p\";\n", (void*)node, i, (void*)node->children[i]);
                construct_btree_dot_file(node->children[i], output_file);
            } else {
                fprintf(output_file, "    null%d [shape=point];\n", nullcount);
                fprintf(output_file, "    \"node%p\":f%d -> null%d;\n", (void*)node, i, nullcount);
                nullcount++;
            }
        }
    }
}

void write_btree_dot_file(btree_node *root, const char *filename) {
    FILE *output_file = fopen(filename, "w");
    if (!output_file) {
        perror("Failed to open file");
        return;
    }

    fprintf(output_file, "digraph BTree {\n");
    fprintf(output_file, "    node [shape=record, height=.1];\n");
    construct_btree_dot_file(root, output_file);
    fprintf(output_file, "}\n");

    fclose(output_file);
}

// .txt section
void construct_btree_txt_file(btree_node *node, int indent, FILE *output_file) {
    if (!node) return;

    for (int i = 0; i < indent; i++) { // print spaces
        fprintf(output_file, " ");
    }

    fprintf(output_file, "[");
    for (int i = 0; i < node->keys_number; i++) {
        fprintf(output_file, "%d", node->keys[i]);
        if (i < node->keys_number - 1) {
            fprintf(output_file, " ");
        }
    }
    fprintf(output_file, "]\n");

    if (node->childer_number) {
        for (int i = 0; i <= node->keys_number; i++) {
            construct_btree_txt_file(node->children[i], indent + 4, output_file);
        }
    }
}

void write_btree_txt_file(btree_node *root, const char *filename) {
    FILE *output_file = fopen(filename, "w");
    if (!output_file) {
        perror("Failed to open file");
        return;
    }
    construct_btree_txt_file(root, 0, output_file);
    fclose(output_file);
}
