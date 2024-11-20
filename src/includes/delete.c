#include "../btree.h"

error_t btree_node_delete(btree_node *ctx, btree tree, int key) {
    int idx = 0;

    // simply find the position
    while(idx < ctx->keys_number && key > ctx->keys[idx]) {
        idx += 1;
    }

    if (idx < ctx->keys_number && ctx->keys[idx] == key) {
        // we found key to delete!
        if (!ctx->childer_number) {
            // so, key to delete present in leaf
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
            btree_node *predecessor = btree_node_get_inorder_predecessor(ctx, idx);

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
            // no more children, faild to find target
            return ERROR_KEY_NOT_FOUND;
        }
    }
    return ERROR_NONE;
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