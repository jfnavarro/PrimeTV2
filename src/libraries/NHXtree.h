#ifndef NHXTREE_H
#define NHXTREE_H

/*
  An NHXtree keeps the pointer to the roo node of a tree 
  and has also a pointer to the next tree in the list, since 
  we are actually talking about a tree list here!
*/
  

#include "NHXnode.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>


struct NHXtree {
  struct NHXtree *next;
  struct NHXnode *root;
};

/*
  'Public' functions
*/
struct NHXtree *read_tree(const char *filename);
struct NHXtree *read_tree_string(const char *str);
struct NHXtree *read_tree_from_file_stream(FILE * f);
void delete_trees(struct NHXtree *T);

unsigned treeSize(const struct NHXtree *T);

/*
  Internal stuff
*/
struct NHXtree *new_tree(struct NHXnode *root, struct NHXtree *next_tree);
#ifdef __cplusplus
}
#endif

#endif
