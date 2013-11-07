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

// this class is an extension of the Tree class created by Lars Arsvestad,
// it addes functionalities and variables


#ifndef TREEEXTENDED_H
#define TREEEXTENDED_H

#include "../utils/AnError.hh"
#include "Tree.hh"

#include <boost/multi_array.hpp>
#include <boost/static_assert.hpp>

using namespace std;

class TreeExtended : public Tree
{

public:  
    
    //constructors
    TreeExtended();
    TreeExtended(const TreeExtended &T);
    TreeExtended(const Tree& T);
    TreeExtended(TreeExtended *T);
    TreeExtended(const TreeExtended *T);
    TreeExtended(Tree *T);
    TreeExtended(const Tree *T);
    
    //overload of =
    TreeExtended& operator=(const TreeExtended& T);
    TreeExtended& operator=(const Tree& T);
    TreeExtended& operator=(const TreeExtended *T);
    TreeExtended& operator=(const Tree *T);

    /* returns the first node in a postorder configuration */
    Node* getPostOderBegin() const;
    
    /* returns the first node in a preorder configuration */
    Node* preorder_begin() const;
    
    /* returns the next preorder node of the node given */
    Node* preorder_next(Node *v) const;
    
    /* returns the next postorder node of the node given*/
    Node* postorder_next(Node *v) const;
    
    /* returns the least common ancestor the two nodes given*/
    Node* lca(Node *v1, Node *v2) const;
    
    /* true is v1 is descendant of v2 */
    bool descendant(Node *v1, Node *v2) const;
    
    /* returns the maximum distance from leaf to node*/
    double findMaximumDistanceToLeaf(Node *n) const;
    
    /* returns the number of children of node n*/
    unsigned getNumberOfChildren(Node *n) const;
    
    /* reset the tree information*/
    void reset();
    
private:

    mutable bool lca_is_valid;
    mutable std::vector<unsigned> E; /* Euler-path for lca-compuatation. */
    mutable std::vector<unsigned> L; /* Level array corresponding to E. */
    mutable std::vector<std::vector<unsigned>::size_type> Ref;/* Representative array for lca-computation */
    mutable boost::multi_array<std::vector<unsigned>::size_type, 2> M; /* M-matrix for the RMQ-algorithm. */
    struct Invalid_id : public std::exception {const char *what() const throw();};
    static unsigned most_significant_bit(unsigned v);
    void build_lca() const;
};

#endif // TREEEXTENDED_H
