#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// BTREE

btree btree_new(int degree) {
    if (degree < BTREE_MIN_M) {
        degree = BTREE_MIN_M;
    }

    btree new_btree = {
        .degree = degree, 
        .min_degree = ceilf((float)degree / 2.0f), 
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


btree_node *btree_node_search(btree_node *ctx, int key, int *idx) {
    for (*idx = 0; *idx < ctx->keys_number && key >= ctx->keys[*idx]; *idx += 1) {
        if (key == ctx->keys[*idx]) { // found
            return ctx;
        } 
    }
    // not found (if internal node, traverse to children)
    return ctx->childer_number ? btree_node_search(ctx->children[*idx], key, idx) : NULL;
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
        btree_node_delete(tree->root, *tree, key);
        if (!tree->root->keys_number) { // tree will shrink if there are no keys in root
            btree_node *previous_root = tree->root;
            tree->root = NULL;
            if (previous_root->childer_number) {
                // update root pointer
                tree->root = previous_root->children[0];
            }
            btree_node_free_shallow(previous_root);
        }
    }
}

// BTREE_NODE 

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

void btree_move_kv(btree_node *in, int in_idx, btree_node *out, int out_idx) {
    out->keys[out_idx] = in->keys[in_idx];
    out->values[out_idx] = in->values[in_idx];
}

btree_node *btree_node_insert(btree_node *ctx, btree tree, int key, void *value) {
    // find position
    int iter = 0;

    // get position to insert a new unit 
    while (iter < ctx->keys_number && key > ctx->keys[iter]) {
        iter += 1;
    }

    if (iter < ctx->keys_number && key == ctx->keys[iter]) { // just replace value if key already exists
        ctx->values[iter] = value;
        return NULL;
    }
    
    // did not find a kay in this node
    btree_node *result = NULL;

    if (!ctx->childer_number) { // recursion base

        // leaf node
        if (ctx->keys_number == tree.degree - 1) { // EX: [(3, 5, 7)] <-- (10) 
            // will have an overflow (split and upshift)

            result = btree_new_node_item(tree, key, value); // returns [(10, (), ())]

            // split elements
            btree_node_slpit(ctx, tree, result, iter);
        } else {
            // simple insert
            // .. right shift elements after
            // .... EX: [(5, 7, ())] <-- (3); [(5, (), 7)] <-- (3); [((), 5, 7)] <-- (3)
            for (int jter = ctx->keys_number; jter > iter; jter -= 1) {
                btree_move_kv(ctx, jter - 1, ctx, jter);
            }

            // EX: insert [(3, 5, 7)]
            ctx->keys[iter] = key;
            ctx->values[iter] = value;

            ctx->keys_number += 1;
        }

    } else {
        // internal node (insert into child)
        // . .insert into child
        result = btree_node_insert(ctx->children[iter], tree, key, value);

        if (result) {

            // checking the condition on each call during stack promotion
            // .. The tree grows from the bottom up
            if (ctx->keys_number == tree.degree - 1) { // this could have happened in the process of splitting
                // will have overflow in internal node
                btree_node_slpit(ctx, tree, result, iter);
            } else {
                // rifht shift keys to insert unshifted element
                for (int jter = ctx->keys_number; jter > iter; jter -= 1) {
                    btree_move_kv(ctx, jter - 1, ctx, jter);
                }
                // insert upshifted element
                // .. result is (10, (), ()). 
                // .... just copy fisrt node value to current node at current position and then delete result!
                btree_move_kv(result, 0, ctx, iter);

                // right shift children
                // .. also we need to shift each child too
                for (int jter = ctx->childer_number; jter > iter + 1; jter -= 1) {
                    ctx->children[jter] = ctx->children[jter - 1];
                }

                // insert children from upshifted element 
                ctx->children[iter + 1] = result->children[1];
                ctx->children[iter] = result->children[0];

                // free result
                free(result->children);

                free(result->keys);
                free(result->values);

                free(result);

                result = NULL;

                // update counter
                ctx->keys_number += 1;
                ctx->childer_number += 1;
            }
        }
    }
    return result;
}

void btree_node_slpit(btree_node *ctx, btree tree, btree_node *new_node, int idx) {

    // ctx --> [(3, 5, 7)], new node --> [(10, (), ())], index --> 2

    // whill set new node as the upshift element

    int has_children = ctx->childer_number; // we need to determine that we have children

    btree_node *tmp_node = btree_new_node(tree);

    // copy childen
    // .. EX: tmp_node = [(10, (), ())]
    memcpy(tmp_node->children, new_node->children, 2 * sizeof(btree_node *));

    // swap new node with node to be upshifted

    // we nned to know where we insert a new value(after or before median)
    // .. because we need   know how to catch our node after insertation (we need to shift our median or pop it up)

    /*  
        EX degree --> 5 ==> ((min degree - 1) ~ median index) = ceil(5/2) - 1 = 3 - 1 = 2
        .. m - median, @, #, & - some values, ei - element to insert

        0) - ctx node --> [(@, #, m, &)]

        EX: i = 0 (index of ei)
        1) - if ei index < m index --> (ei, @, #, m, &) --> # is new median!! so, we nned to pop up # and shift m and other units after that

             [(#)]
            /     \
        [(ei, @)] [(m, &)]


        EX: i = 3 (index of ei)
        2) - if ei index > m index --> (@, #, m, ei, &) --> m is still median! so we nned to pop it up!

             [(m)]
             /   \
        [(@, #)] [(ei, &)]


        EX: i = 2 (index of ei)
        3) - if ei index == m index --> new node wiil pe poped! so, we don't swift anything!

             [(ei)]
             /   \
        [(@, #)] [(m, &)]
    */

    // In these cases, we shift all the elements in the direction 
    // we need, depending on the location of the index relative to the center of the node
    
    // first case
    if (idx < tree.min_degree - 1) {
        // insert before median
        // .. current median element will be shifted

        // .... we do this because # is new median and in ctx node index of # is min degree index - 1!
        btree_move_kv(ctx, tree.min_degree - 2, tmp_node, 0); // so... we store #(future median)

        // right shift to fill gap removing the median
        for (int jter = tree.min_degree - 2; jter > idx; jter -= 1) {
            btree_move_kv(ctx, jter - 1, ctx, jter);
        } // after loop [(@, @, m, &)]

        // insert new node
        btree_move_kv(new_node, 0, ctx, idx);

        // after move [(ei, @, m, &)] and #(our median that we should pop up in tmp_node)

    // second case
    } else if (idx > tree.min_degree - 1) {
        // insert after median
        // .. current median element will be poped up
        
        btree_move_kv(ctx, tree.min_degree - 1, tmp_node, 0);

        // left shift to fill gap removing the median
        for (int jter = tree.min_degree - 1; jter < idx && jter < ctx->keys_number - 1; jter += 1) {
            btree_move_kv(ctx, jter + 1, ctx, jter);
        } // after loop [(@, #, &, &)]

        btree_move_kv(new_node, 0, ctx, idx - 1); // idx - 1 because we pop up the medium
        // [(@, #, ei, &)]

    // third case (new node is the medium)
    } else { 
        btree_move_kv(new_node, 0, tmp_node, 0); // tmp_node is [(ei, (), (), ())]
    }

    // after this conditions we get node that we need to pop up in tmp node and other part of node in ctx in correct order
    // .. populate new node with element to be poped
    // .... for EX let's go through the first case with # to pop up and ctx --> [(ei, @, m, &)]

    btree_move_kv(tmp_node, 0, new_node, 0);

    // upd children for new node [(#, (), (), ())]
    new_node->children[0] = ctx;
    new_node->children[1] = btree_new_node(tree);
    /*
                            [(    #    ,    ()    ,    ()    ,    ()    )] <-- this is new node <(0_0)>
                            /        \
    this is ctx --> [(ei, @, m, &)]   [((), (), (), ())] <-- this is new node right child(child[1])
                    /   |  |   |  \
                        e.t.c
    */

    // dividing the values (new node is  (#, (), (), ()))
    for (int jter = tree.min_degree - 1; jter < tree.degree - 1; jter += 1) {
        btree_move_kv(ctx, jter, new_node->children[1], jter - tree.min_degree + 1);
        ctx->keys[jter] = __INT_MAX__;
        ctx->values[jter] = NULL;
    }
    // .. after loop
    /*
                            [(    #    ,    ()    ,    ()    ,    ()    )] <-- this is new node <(0_0)>
                            /        \
    this is ctx --> [(ei, @, 0, 0)]   [(m, &, (), ())] <-- this is new node right child(child[1])
                    /   |  |   |  \
                        e.t.c
    */
   // In both of cases we shift a helf of elements to the second child of new node
   // .. The question only about were new element located now! were we need to refresh children!

   // shift and devide children (if our ctx have children (e.t.c in the picture)
   if (has_children) {
        if (idx < tree.min_degree - 1) { // our EX case
            // just copy children to the right side!
            for (int jter = tree.min_degree - 1; jter < tree.degree; jter += 1) {

                // same logic as in dividing values! take a look at (jter - tree.min_degree) and at the picture and at the (jter - tree.min_degree! and at the pict....
                new_node->children[1]->children[jter - tree.min_degree + 1] = new_node->children[0]->children[jter];
            }
                /*
                        [(    #    ,    ()    ,    ()    ,    ()    )] <-- this is new node <(0_0)>
                        /         \
                [(ei, @, m, &)]   [(m, &, (), ())]
                /   |  |   |  \ /   |  \
                    e.t.c
                */
            
            // right shift children to the left side (because all units was shifted to the right....)
            for (int jter = tree.degree - 1; jter > idx + 1; jter -= 1) {
                new_node->children[0]->children[jter] = new_node->children[0]->children[jter - 1];
            }

            // insert temporary child (we just update children from ei (new unit) at the index 0 in out EX case, because before at the index 0 was @ and @'s children...)
            new_node->children[0]->children[idx + 1] = tmp_node->children[1];
        } else { // only one else condition
            
            // copy children to the right side
            for (int jter = tree.min_degree; jter < tree.degree; jter += 1) {
                new_node->children[1]->children[jter - tree.min_degree] = new_node->children[0]->children[jter];
            }

            // right shift children on the right side (because all units was shifted to the left....)
            for (int jter = tree.min_degree - 1; jter > idx - tree.min_degree + 1; jter -= 1) {
                new_node->children[1]->children[jter] = new_node->children[1]->children[jter - 1];
            }

            // insert temporary child (in this case new element in the second child of element, that we poped up before, so...)
            new_node->children[1]->children[idx - tree.min_degree + 1] = tmp_node->children[1];
        }

        // update counters
        new_node->children[0]->childer_number = tree.min_degree;
        new_node->children[1]->childer_number = tree.degree - tree.min_degree + 1; // cos we slit this node by this parameters!
   }

   // update keys counters
   new_node->children[0]->keys_number = tree.min_degree - 1;
   new_node->children[1]->keys_number = tree.degree - tree.min_degree; // ceilf function on the contrary
   new_node->keys_number = 1;
   new_node->childer_number = 2;

   // ok! we just update new node pointer

   btree_node_free_shallow(tmp_node);
}

btree_node *btree_node_get_inorder_predecessor(btree_node *ctx, btree tree, int idx) {
    if (ctx->childer_number) {
        btree_node *current = ctx->children[idx];

        // traverse to leaves
        while (current->childer_number) {
            // traverse through last child
            current = current->children[current->childer_number - 1];
        }
        // current contains inorder predecessor as the last key
        return current;
    } else {
        return NULL;
    }
}

void btree_node_free_shallow(btree_node *ctx) {
    if (ctx) {
        free(ctx->children);
        free(ctx->keys);
        free(ctx->values);

        free(ctx);
    }
}

void btree_node_rebalance(btree_node *ctx, btree tree, int key) {

    // we call it after deletion

    if (!ctx->childer_number) { // recursion base
        // rebalancing takes place in parent
        return;
    }

    // fing position
    int idx = 0;
    while(idx < ctx->keys_number && key > ctx->keys[idx]) {
        idx += 1;
    }

    // rebalance starting in child (go as deep as possible as first!)
    btree_node_rebalance(ctx->children[idx], tree, key);

    

    /* 
        at this line of code we have something like that because at leaf nodes we have only return instruction

        EX degree --> 5 ==> ((min degree - 1) ~ median index) = ceil(5/2) - 1 = 3 - 1 = 2


                             [(    a    ,    b    ,    c    ,    d    )] <-- this is ctx <(0_0)> 
                               /         \
        deficient node --> [(e, (), (), ()]   [(f, g, (), ())] <-- right sibling of a deficient node
                           /   |               /   |  \
                    
        let's consider that after deletion left child of ctx not still maintains B-tree rules, because it have only one element with min degree = 2
        

    */
    if (ctx->children[idx]->keys_number < tree.min_degree - 1) { 


        /*
            deficient node: ctx->children[idx]     --> [(e, (), (), ())] in our EX
            right sibling: ctx->children[idx + 1]  -->  [(f, g, (), ())] in our EX
            left siblibg: ctx->children[idx - 1]   --> NULL in our EX 

            left separator: ctx->keys[idx - 1]     --> NULL in our EX 
            right separator: ctx->keys[idx]        --> 'a' in our EX
        */

        // if we have a right sibling and sibling node have more than minumum number of keys
        if (idx < ctx->childer_number - 1 && ctx->children[idx + 1]->keys_number > tree.min_degree - 1) { // yes, we have [(f, g, (), ())], 2 > 2 - 1
            // case 1 --> Left Rotation
            // .. copy right separator to end of the deficient node
            btree_move_kv(ctx, idx, ctx->children[idx], ctx->children[idx]->keys_number);

            /*
                step 1:

                            [(    a    ,    b    ,    c    ,    d    )] 
                             /         \
                [(e, a, (), ())]   [(f, g, (), ())] 
               /   |               /   |  \
                    
            */

            ctx->children[idx]->keys_number += 1;

            // copy smallest key in right sibling to right separator
            btree_move_kv(ctx->children[idx + 1], 0, ctx, idx);

            /*
                step 2:

                            [(    f    ,    b    ,    c    ,    d    )] 
                             /         \
                [(e, a, (), ())]   [(f, g, (), ())] 
               /   |               /   |  \
                    
            */

            // left shift to cover up removal
            for (int jter = 0; jter < ctx->children[idx + 1]->keys_number - 1; jter += 1) {
                btree_move_kv(ctx->children[idx + 1], jter + 1,  ctx->children[idx + 1], jter);   
            }

            ctx->children[idx + 1]->keys_number -= 1;

            /*
                step 3:

                            [(    f    ,    b    ,    c    ,    d    )] 
                             /         \
                [(e, a, (), ())]   [(g, g, (), ())] 
               /   |               /   |  \
                    

                PS: yes, we still gave second g in the right sibling of deficient node in this palce in memory,
                but now it does not play a role, since we have decremented the number of keys and in the future this
                value will simply be overwritten (this is about how the memory of the processes of the operating system works in reality =))

                so for ease of understanding, let's draw it like this:

                            [(    f    ,    b    ,    c    ,    d    )] 
                             /         \
                [(e, a, (), ())]   [(g, (), (), ())] 
               /   |               /   |  \
            */

            if (ctx->children[idx]->childer_number) {
                // move leftmost child
                ctx->children[idx]->children[ctx->children[idx]->childer_number] = ctx->children[idx + 1]->children[0];
                ctx->children[idx]->childer_number += 1;


                /*
                                [(    f    ,    b    ,    c    ,    d    )] 
                                /         \
                    [(e, a, (), ())]   [(g, (), (), ())] 
                    /  |  \                 |   \
                */

                // and we need to left shift children in the right sibling
                for (int jter = 0; jter < ctx->children[idx + 1]->childer_number - 1; jter += 1) {
                    ctx->children[idx + 1]->children[jter] = ctx->children[idx + 1]->children[jter + 1];
                }  

                ctx->children[idx + 1]->childer_number -= 1;

                /*
                                [(    f    ,    b    ,    c    ,    d    )] 
                                /         \
                    [(e, a, (), ())]   [(g, (), (), ())] 
                    /  |  \            /   \
                */

               // that's it!

            }


        // if we not have right sibling and we have a left sibling and sibling node have more than minumum number of keys 

        // Symmetrically, in the second case, adjusted for the fact that when inserting into the right sibling 
        // (i.e into a node with large keys), we must insert not at the end, as in the case ofthe left sibling, but 
        // at the beginning, and therefore we must first shift all the values of the right sibling to the right

        } else if (idx > 0 && ctx->children[idx - 1]->keys_number > tree.min_degree - 1) {
            // case 2 --> Right Rotation
            // right shift keys in deficient node
            for (int jter = ctx->children[idx]->keys_number - 1; jter > 0; jter -= 1) {
                btree_move_kv(ctx->children[idx], jter - 1, ctx->children[idx], jter);
            }

            // copy left separator
            btree_move_kv(ctx, idx - 1, ctx->children[idx], 0);
            ctx->children[idx]->keys_number += 1;

            // copy largest key
            btree_move_kv(ctx->children[idx - 1], ctx->children[idx - 1]->keys_number - 1, ctx, idx - 1);

            ctx->children[idx - 1]->keys_number -= 1;

            if (ctx->children[idx]->childer_number) {
                // right shift
                for (int jter = ctx->children[idx]->childer_number; jter > 0; jter -= 1) {
                    ctx->children[idx]->children[jter] = ctx->children[idx]->children[jter - 1];
                }
                ctx->children[idx]->childer_number += 1;

                ctx->children[idx]->children[0] = ctx->children[idx - 1]->children[ctx->children[idx - 1]->childer_number - 1];
                ctx->children[idx - 1]->childer_number -= 1;
            }
            
        // merge! 
        } else {

            /*
                EX:

                                          [(    a    ,    b    ,    c    ,    d    )] <-- this is ctx node!
                                           /          \
                left sibling --> [(e, f, (), ())]   [(g, (), (), ())] <-- deficient node
                                 /  |  \            /   \
            */

           // first step: chek siblings 
           int left_idx = idx > 0 ? idx - 1: idx; // in our EX left_idx is 0

           btree_node *left = ctx->children[left_idx];
           btree_node *right = ctx->children[left_idx + 1];

           // copy separator to end of left merge node
           btree_move_kv(ctx, left_idx, left, left->keys_number);
           left->keys_number += 1;

            /*
                                [(    a    ,    b    ,    c    ,    d    )] <-- this is ctx node!
                                   /          \
                left --> [(e, f, a, ())]   [(g, (), (), ())] <-- right
                         /  |  |          /   \
            */

            // copy elements of right merge node to left
            for (int jter = 0; jter < right->keys_number; jter += 1) { 
                btree_move_kv(right, jter, left, left->keys_number);
                left->keys_number += 1;
            } 

            // copy children from right merge node to left
            for (int jter = 0; jter < right->childer_number; jter += 1) { 
                left->children[left->childer_number] = right->children[jter];
                left->childer_number += 1;
            } 

            /*
                so we moved all the keys and their children to the left sibling

                                 [(    a    ,    b    ,    c    ,    d    )] <-- this is ctx node!
                                   /          \
                left --> [(e, f, a, g)]   [((), (), (), ())] <-- right
                         /  |  |   |  \  
            */

            // left shift keys in root, because 'a' is not still in the root
            for (int jter = left_idx; jter < ctx->keys_number - 1; jter += 1) {
                btree_move_kv(ctx, jter + 1, ctx, jter);
            }
            ctx->keys_number -= 1;

            // left shift children in the ctx
            for (int jter = left_idx + 1; jter < ctx->childer_number - 1; jter += 1) {
                ctx->children[jter] = ctx->children[jter + 1];
            }
            ctx->childer_number -= 1;

            /*
                                 [(    b    ,    c    ,    d    ,    ()    )] <-- this is ctx node!
                                   /          \
                left --> [(e, f, a, g)]   [((), (), (), ())] <-- right
                         /  |  |   |  \       
            */

            // that's it!
            // .. We don't need the right one anymore! let's delete it

            btree_node_free_shallow(right);

        }
    }
}

int btree_node_delete(btree_node *ctx, btree tree, int key) {
    int idx = 0;
    char found = 0;

    // simply find the position
    while(idx < ctx->keys_number && key > ctx->keys[idx]) {
        idx += 1;
    }

    if (idx < ctx->keys_number && ctx->keys[idx] == key) {
        // we found key to delete!
        if (!ctx->childer_number) {
            // so key to deelte present in leaf
            // .. left shift to cover up
            for (int jter = idx; jter < ctx->keys_number - 1; jter += 1) {
                btree_move_kv(ctx, jter + 1, ctx, jter); // left shift!
            }
            ctx->keys_number -= 1;

            if (ctx->keys_number < tree.min_degree - 1) {
                // we need to rebalance the tree in this case to maintain B-tree rules
                // .. rebalance from the leaf to the root!
                btree_node_rebalance(tree.root, tree, key);
            }
        } else {
            // remove targer from internal node
            // .. get inorder predecessor
            btree_node *predecessor = btree_node_get_inorder_predecessor(ctx, tree, idx);

            // replace target eith predecessor
            btree_move_kv(predecessor, predecessor->keys_number - 1, ctx, idx);
            predecessor->keys_number -= 1;

            if (predecessor->keys_number < tree.min_degree - 1) {
                btree_node_rebalance(tree.root, tree, predecessor->keys[0]);
            }
        }
    } else {
        // traverse to child
        if (ctx->childer_number) {
            return btree_node_delete(ctx->children[idx], tree, key);
        } else {
            // no more children, failrd to find target
            return -1;
        }
    }
    return 0;
}