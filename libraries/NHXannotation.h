#ifndef NHXANNOTATIONS_H
#define NHXANNOTATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "NHXnode.h"

/*
  We need a list structure to put annotations in.
*/  

/* 
   This is how long tag strings (such as 'AC' or 'D') in 
   NHX trees are allowed to be, *including* '\0'!
*/
#define MAX_ANNOTATION_ID_LENGTH 5


struct NHXannotation {
  char anno_type[MAX_ANNOTATION_ID_LENGTH];
  union {
    float t;			/* For node times */
    unsigned i;			/* Anti-chain (i.e., number of species node) */
    char *str;			/* For species name, et.c. */
    struct int_list *il;	/* List of integers */

    /* Preparing for other annotation here! */
  } arg;

  struct NHXannotation *next;
};

/* Structure for integer lists */
struct int_list {
  int i;
  struct int_list *next;
};

/*
  'Public' functions
*/

void annotate_node(struct NHXnode *n, struct NHXannotation *l); /* Put the necessary node time info from l into n, and
							  * put l into n.
							  */
void delete_annotation_list(struct NHXannotation *l); /* Free up all memory */


/*
  Check annotation type. Essentially doing a strcmp with 'tag'.
*/
int annotation_isa(struct NHXannotation* l, const char *tag);

/*
  Prepend a new annotation to the annotation list l:
*/
struct NHXannotation *new_newick_weight(float t, struct NHXannotation *l);
struct NHXannotation *new_annotation(char *tag, struct NHXannotation *l);

struct NHXannotation *new_duplication(struct NHXannotation *l);
struct NHXannotation *new_anti_chain(unsigned i, struct NHXannotation *l);
struct NHXannotation *new_species_name(char *str, struct NHXannotation *l);
struct NHXannotation *new_node_id(unsigned id, struct NHXannotation *l);

struct NHXannotation *append_annotations(struct NHXannotation* l1, struct NHXannotation *l2);

/*
  Handle integer lists.
*/
struct int_list* new_int_list(int i, struct int_list *next);
void free_int_list(struct int_list *il);
  struct int_list* int_list_reverse(struct int_list *il);
#ifdef __cplusplus
}
#endif

#endif
