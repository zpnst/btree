#include "../btree.h"

#include <stdlib.h>
#include <string.h> 
#include <stddef.h> // for NULL macros

btree_node *btree_node_insert(btree_node *ctx, btree tree, int key, void *value) {
    // find position
    int iter = 0;

    // get position to insert a new value 
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

        0) - EX ctx node --> [(@, #, m, &)]

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
        // aftre btree_move_kv [(@, #, ei, &)]

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
    this is ctx --> [(ei, @, m, &)]   [((), (), (), ())] <-- this is new node's right child
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
    this is ctx --> [(ei, @, 0, 0)]   [(m, &, (), ())] <-- this is new node's right child
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
                [(ei, @, 0, 0)]   [(m, &, (), ())]
                /  |  \           /   |  \
                    e.t.c
                */
            
            // right shift children to the left side (because all values was shifted to the right....)
            for (int jter = tree.degree - 1; jter > idx + 1; jter -= 1) {
                new_node->children[0]->children[jter] = new_node->children[0]->children[jter - 1];
            }

            // insert temporary child (we just update children from ei (new value) at the index 0 in our EX case, because before at the index 0 was @ and @'s children...)
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
