#ifndef NHXNODE_H
#define NHXNODE_H

#ifdef __cplusplus
extern "C" {
#endif

struct NHXnode {
  struct NHXnode *parent, *left, *right;
  char *name;			/* Name or other id of node. Mostly for leaves. */
  struct NHXannotation *l;	/* Node annotations, e.g. duplication or not, anti-chains, et.c. */
};

/*
  'Public' functions
*/
// struct NHXnode *read_tree(const char *filename);
void delete_node(struct NHXnode *n);
void delete_tree_nodes(struct NHXnode *n);

/***
  Access functions
***/

/*
    Find the annotation of a certain type
*/
struct NHXannotation *find_annotation(struct NHXnode *v, const char *tag);

/*
  isDuplication returns non-zero if the provided node is annotated
  as a duplication.
*/

int isDuplication(struct NHXnode *t); /* Returns 1 if is _annotated_ as duplication, 0 o.w.  */
int isLeaf(struct NHXnode *t);	/* Returns 1 if t is leaf, 0 otherwise */
int isRoot(struct NHXnode *t);	/* Returns 1 if t is a root (no parent), 0 otherwise */
char *speciesName(struct NHXnode *t);

unsigned subtreeSize(struct NHXnode *n); /* Count the number of nodes in tree rooted at n */

  /* Bad form, but a necessary temporary hack. /arve */
void NHX_debug_print(struct NHXnode *l);

/* 
   Functions used by Yacc/Bison. Don't touch, don't use! 

*/
struct NHXnode *new_node(char *name);

#ifdef __cplusplus
}
#endif

#endif
