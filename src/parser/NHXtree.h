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


struct NHXtree *read_tree(const char *filename);
struct NHXtree *read_tree_string(const char *str);
struct NHXtree *read_tree_from_file_stream(FILE * f);
void delete_trees(struct NHXtree *T);

unsigned treeSize(const struct NHXtree *T);

struct NHXtree *new_tree(struct NHXnode *root, struct NHXtree *next_tree);
#ifdef __cplusplus
}
#endif

#endif
