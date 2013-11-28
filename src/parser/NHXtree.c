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

#include "NHXnode.h"
#include "NHXtree.h"

#include <stdio.h>
#include <stdlib.h>

void
delete_trees(struct NHXtree *TreeExtended)
{
    if (TreeExtended)
    {
        delete_trees(TreeExtended->next);
        delete_tree_nodes(TreeExtended->root);
        free(TreeExtended);
    }
}

struct NHXtree *
        new_tree(struct NHXnode *root, struct NHXtree *next_tree)
{
    struct NHXtree *TreeExtended = (struct NHXtree *) malloc(sizeof(struct NHXtree));
    if (TreeExtended)
    {
        TreeExtended->root = root;
        TreeExtended->next = next_tree;
        return TreeExtended;
    }
    else
    {
        fprintf(stderr, "Out of memory! (%s:%d)\n", __FILE__, __LINE__);
        exit(1);
    }
}


unsigned 
treeSize(const struct NHXtree *TreeExtended)
{
    if (TreeExtended == NULL)
    {
        return 0;
    }

    struct NHXnode *r = TreeExtended->root;
    if (r == NULL)
    {
        return 0;
    }
    else
    {
        return subtreeSize(r);
    }
}
