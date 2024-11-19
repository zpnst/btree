#include "../btree.h"

#include <stdio.h>

#define DATA_DIR "data"
#define IMAGE_DIR "images"

#define FILENAME_SIZE 512
#define COMMAND_BUFFER_SIZE 512

// .png section
void write_btree_png_image(const char *filename);

// .dot section for graphviz .dot files
void write_btree_dot_file(btree_node *root, const char *filename);
void construct_btree_dot_file(btree_node *node, FILE *output_file);

// .txt section
void write_btree_txt_file(btree_node *root, const char *filename);
void construct_btree_txt_file(btree_node *node, int indent, FILE *output_file);