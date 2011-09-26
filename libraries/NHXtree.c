#include "NHXnode.h"
#include "NHXtree.h"

#include <stdio.h>
//#include <malloc.h>
#include <stdlib.h>

/*
  'Public' functions
*/


void
delete_trees(struct NHXtree *T)
{
  if (T) {
    delete_trees(T->next);
    delete_tree_nodes(T->root);
    free(T);
  }
}

/*struct NHXtree *read_tree(const char *filename)
{
}
struct NHXtree *read_tree_string(const char *str)
{
}
struct NHXtree *read_tree_from_file_stream(FILE * f)
{
}*/
/*
  Internal stuff
*/
struct NHXtree *
new_tree(struct NHXnode *root, struct NHXtree *next_tree)
{
  struct NHXtree *T = (struct NHXtree *) malloc(sizeof(struct NHXtree));
  if (T) {
    T->root = root;
    T->next = next_tree;
    return T;
  } else {
    fprintf(stderr, "Out of memory! (%s:%d)\n", __FILE__, __LINE__);
    exit(1);
  }
}


unsigned 
treeSize(const struct NHXtree *T)
{
  if (T == NULL) {
    return 0;
  }

  struct NHXnode *r = T->root;
  if (r == NULL) {
    return 0;
  } else {
    return subtreeSize(r);
  }
}
