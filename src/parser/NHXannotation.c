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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "NHXannotation.h"

void
annotate_node(struct NHXnode *n, struct NHXannotation *l)
{
    assert (n != NULL);
    n->l = l;			/* Associate annotations with the node. */
}

/*
  Check what annotation we have. Does it match our string?
  Returns 0 on identity, non-zero on difference.
  Note that only the MAX_ANNOTATION_ID_LENGTH - 1 first chars of tag
  are compared.
*/
int
annotation_isa(struct NHXannotation* l, const char *tag)
{
    assert(l != NULL);
    assert(tag != NULL);
    return strncmp(l->anno_type, tag, MAX_ANNOTATION_ID_LENGTH);
}

void
delete_annotation_list(struct NHXannotation *l)
{
    if (l != NULL) {
        if (annotation_isa(l, "S") == 0) {
            free(l->arg.str);
        }
        delete_annotation_list(l->next);
        free(l);
    }
}

struct NHXannotation*
        new_newick_weight(float t, struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, "NW", MAX_ANNOTATION_ID_LENGTH);
    a->arg.t = t;
    a->next = l;
    return a;
}

struct NHXannotation*
        new_node_time(float t, struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, "NT", MAX_ANNOTATION_ID_LENGTH);
    a->arg.t = t;
    a->next = l;
    return a;
}

/*
 * New modern style annotation-creaters
 */
struct NHXannotation*
        new_annotation(char *tag, struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, tag, MAX_ANNOTATION_ID_LENGTH);
    a->next = l;
    return a;
}

/*
 * Old style annotation-creaters
 */
struct NHXannotation*
        new_duplication(struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, "D", MAX_ANNOTATION_ID_LENGTH);
    a->next = l;
    return a;
}

struct NHXannotation*
        new_anti_chain(unsigned i, struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, "AC", MAX_ANNOTATION_ID_LENGTH);
    a->arg.i = i;
    a->next = l;
    return a;
}

struct NHXannotation*
        new_species_name(char *name, struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, "S", MAX_ANNOTATION_ID_LENGTH);
    a->arg.str = name;
    a->next = l;
    return a;
}

struct NHXannotation*
        new_node_id(unsigned id, struct NHXannotation *l)
{
    struct NHXannotation *a = (struct NHXannotation*) malloc(sizeof(struct NHXannotation));
    strncpy(a->anno_type, "ID", MAX_ANNOTATION_ID_LENGTH);
    a->arg.i = id;
    a->next = l;
    return a;
}

/*
 * I messed up in my data modelling, so sometimes I return a list
 * of annotations, when the parser sort-of expects a single item.
 * Therefore, I cannot simply change a next-pointer, but have to trace
 * down to the end of the list. It helps if l1 is short however!
 */
struct NHXannotation*
        append_annotations(struct NHXannotation* l1, struct NHXannotation* l2)
{
    struct NHXannotation *a = l1;
    if (a == NULL)
    {
        return l2;
    }
    if (l2 == NULL)
    {
        return l1;
    }
    while (a->next != NULL)
    {	/* Find last element */
        a = a->next;
    }
    a->next = l2;
    return l1;
}

/*
  Integer lists
*/
struct int_list* 
        new_int_list(int i, struct int_list *next)
{
    struct int_list *il = (struct int_list*) malloc(sizeof(struct int_list));
    il->next = next;
    il->i = i;
    return il;
}

void 
free_int_list(struct int_list *il) 
{
    if (il != NULL)
    {
        free_int_list(il->next);
        free(il);
    }
}

/*
  Reverse algorithm:
  Grab first element.
  Reverse the rest of the list.
  The first elements next points to the last elem in the reversed list.
  Point the that former second element to the first and set the
  first elements next pointer to NULL.
*/
struct int_list*
        int_list_reverse(struct int_list *il)
{
    struct int_list *reversed;
    if (il == NULL)
    {
        return NULL;
    }
    else if (il->next == NULL)
    {
        return il;
    }
    else
    {
        reversed = int_list_reverse(il->next);
        il->next->next = il;
        il->next = NULL;
        return reversed;
    }
}

