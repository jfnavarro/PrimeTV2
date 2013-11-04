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

#include "NHXnode.h"
#include "NHXannotation.h"

extern struct NHXnode *root_node;
extern unsigned int lineno; /* Current line number in input file */
extern unsigned int n_left_parens; /* Number of matched left parens '(' */
extern unsigned int n_right_parens; /* Number of matched right parens ')' */

void inform_parser(const char *, const char*);

void 
delete_node(struct NHXnode *n)
{
    delete_annotation_list(n->l);
    if (n->name)
    {
        free(n->name);
    }
    free(n);
}


void
delete_tree_nodes(struct NHXnode *n)
{
    if (n != NULL)
    {
        struct NHXnode *left = n->left;
        struct NHXnode *right = n->right;
        delete_tree_nodes(left);
        delete_tree_nodes(right);
        delete_node(n);
    }
}

/*
  Given a NHX node, see if it contains an annotation with the given tag.
  Returns NULL, if not present, or if v is NULL.
*/
struct NHXannotation *
        find_annotation(struct NHXnode *v, const char *tag)
{
    struct NHXannotation *a;

    if (v == NULL)
    {
        return NULL;
    }

    a = v->l;
    while (a != NULL)
    {
        if (annotation_isa(a, tag) == 0)
        {
            return a;
        }
        a = a->next;
    }
    return NULL;
}

int 
isDuplication(struct NHXnode *v)
{
    struct NHXannotation *a = find_annotation(v, "D");

    if (a == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int 
isLeaf(struct NHXnode *t)
{
    assert(t != NULL);
    if (t->left == NULL && t->right == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int
isRoot(struct NHXnode *t)
{
    assert(t != NULL);
    if (t->parent == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


char *
speciesName(struct NHXnode *v)
{
    struct NHXannotation *a = find_annotation(v, "S");

    if (a == NULL)
    {
        return 0;
    }
    else
    {
        return a->arg.str;
    }
}

unsigned 
subtreeSize(struct NHXnode *n) /* Count the number of nodes in subtree rooted at n */
{
    if (n == NULL)
    {
        return 0;
    }
    else
    {
        return 1
                + subtreeSize(n->left)
                + subtreeSize(n->right);
    }
}



void
NHX_debug_print(struct NHXnode *v)
{
    if (v)
    {
        struct NHXannotation *l = v->l;
        if (l)
        {
            if (annotation_isa(l, "ID"))
            {
                fprintf(stderr, "ID:\t%d\n", l->arg.i);
            }
            else if (annotation_isa(l, "S"))
            {
                fprintf(stderr, "S: \t%s\n", l->arg.str);
            }
            else if (annotation_isa(l, "BW"))
            {
                fprintf(stderr, "BW:\t%f\n", l->arg.t);
            }
        }
    }
}

struct NHXnode *
        new_node(char *name)
{
    struct NHXnode *v = (struct NHXnode*) malloc (sizeof(struct NHXnode));
    if (v)
    {
        v->left = NULL;
        v->right = NULL;
        v->parent = NULL;
        v->name = name;
        v->l = NULL;
        return v;
    }
    else
    {
        fprintf(stderr, "Out of memory! (%s:%d)\n", __FILE__, __LINE__);
        exit (1);
    }
}
