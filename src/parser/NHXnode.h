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

// struct NHXnode *read_tree(const char *filename);
void delete_node(struct NHXnode *n);
void delete_tree_nodes(struct NHXnode *n);

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
