/*
    PrimeTV2 : a visualizer for phylogenetic reconciled trees.
    Copyright (C) 2011  <Jose Fernandez Navarro> <jc.fernandez.navarro@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    
    Author : Jose Fernandez Navarro  -  jc.fernandez.navarro@gmail.com
             Lars Arvestad, © the MCMC-club, SBC, all rights reserved
 */
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


void annotate_node(struct NHXnode *n, struct NHXannotation *l); /* Put the necessary node time info from l into n, and*/
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
