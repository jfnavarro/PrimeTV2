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

#include "Treeextended.h"
#include "Node.hh"

using namespace std;
static const unsigned NONE = -1;

TreeExtended::TreeExtended():
    Tree(),lca_is_valid(false)
{
}

//copy constructor  
TreeExtended::TreeExtended(const TreeExtended &T)
    : Tree(T),
      lca_is_valid(T.lca_is_valid)
{

}

//copy constructor from derived class
TreeExtended::TreeExtended(const Tree& T)
    : Tree(T),
      lca_is_valid(false)
{

}

TreeExtended::TreeExtended(TreeExtended* T)
    : Tree(*T),
      lca_is_valid(T->lca_is_valid)
{

}

TreeExtended::TreeExtended(const TreeExtended* T)
    : Tree(*T),
      lca_is_valid(T->lca_is_valid)
{

}


TreeExtended::TreeExtended(Tree* T)
    : Tree(*T),
      lca_is_valid(false)
{

}

TreeExtended::TreeExtended(const Tree* T)
    : Tree(*T),
      lca_is_valid(false)
{

}



// Assignment
//----------------------------------------------------------------------
TreeExtended&
TreeExtended::operator=(const TreeExtended& T)
{
    Tree::operator=(T);
    lca_is_valid = T.lca_is_valid;
    return *this;
}

TreeExtended&
TreeExtended::operator=(const Tree& T)
{
    Tree::operator=(T);
    return *this;
}

TreeExtended&
TreeExtended::operator=(const TreeExtended *T)
{
    Tree::operator=(*T);
    lca_is_valid = T->lca_is_valid;
    return *this;
}

TreeExtended&
TreeExtended::operator=(const Tree* T)
{
    Tree::operator=(*T);
    return *this;
}

Node* TreeExtended::preorder_begin() const 
{
    return getRootNode();
}

Node* TreeExtended::preorder_next(Node *v) const
{

    if (!v->isLeaf())
    {
        return v->getLeftChild();
    }
    /* special case when the tree consists of one vertex only. */
    if (v == getRootNode())
    {
        return NULL;
    }
    
    while (v != getRootNode() && v->getParent()->getRightChild() == v)
    {
        v = v->getParent();
    }
    
    if (v == getRootNode())
    {
        return NULL;
    }
    
    return v->getParent()->getRightChild();
}

Node* TreeExtended::getPostOderBegin() const
{    
    Node *v = getRootNode();
    while (!v->isLeaf())
    {
        v = v->getLeftChild();
    }
    return v;
}

Node* TreeExtended::postorder_next(Node *v) const
{

    if (v == getRootNode())
    {
        return NULL;
    }

    Node *p = v->getParent();
    Node *r = p->getRightChild();

    if (v == r)
    {
        return p;
    }
    while (!r->isLeaf())
    {
        r = r->getLeftChild();
    }
    return r;
}

const char *
TreeExtended::Invalid_id::what() const throw()
{
    return "Invalid vertex id passed to Binary_tree class.";
}

Node* TreeExtended::lca(Node *v1, Node *v2) const
{
    assert(v1->getNumber() >= 0 && v1->getNumber() < getNumberOfNodes());
    assert(v2->getNumber() >= 0 && v2->getNumber() < getNumberOfNodes());

    if(!lca_is_valid)
    {
        build_lca();
    }
    /* Get the representatives (i.e, indexes into L) of v1 and v2. */
    unsigned r1 = Ref[v1->getNumber()];
    unsigned r2 = Ref[v2->getNumber()];

    /* Make sure that r2 is the bigger one, so that the range
        of indices is [r1, r2]. */
    if (r1 > r2)
    {
        std::swap(r1, r2);
    }
    unsigned k = most_significant_bit(r2 - r1 + 1);
    unsigned idx1 = M[r1][k];
    unsigned idx2 = M[r2-(1u<<k)+1][k]; /* 1u<<k == 2^k */
    if (L[idx2] < L[idx1])
    {
        idx1 = idx2;
    }
    return getNode(E[idx1]);
}

void TreeExtended::build_lca() const
{
    /* First, we build the E and L arrays, i.e., the Euler tour and
    the level array. */

    /* We define a recursive function to compute E and L. */
    struct Create_EL
    {
        void operator()(const Tree &tree,
                        unsigned cur_vertex, unsigned cur_level,
                        std::vector<unsigned> &E, std::vector<unsigned> &L)
        {
            E.push_back(cur_vertex);
            L.push_back(cur_level);
            
            if (tree.getNode(cur_vertex)->isLeaf())
            {
                return;
            }
            this->operator()(tree, tree.getNode(cur_vertex)->getLeftChild()->getNumber(), cur_level + 1, E, L);
            E.push_back(cur_vertex);
            L.push_back(cur_level);
            this->operator()(tree,tree.getNode(cur_vertex)->getRightChild()->getNumber(), cur_level + 1, E, L);
            E.push_back(cur_vertex);
            L.push_back(cur_level);
        }
    };

    E.clear(); E.reserve(this->getNumberOfNodes());
    L.clear(); L.reserve(this->getNumberOfNodes());

    Create_EL create_EL;
    create_EL(*this, getRootNode()->getNumber(), 0, E, L);

    /* Create the R-vector. */
    Ref.clear();
    Ref.resize(this->getNumberOfNodes(), NONE);
    for (unsigned i = 0; i < E.size(); ++i)
    {
        if (Ref[E[i]] == NONE)
            Ref[E[i]] = i;
    }


    /* Initialize and build the M-matrix. */
    unsigned rows = L.size();
    unsigned cols = most_significant_bit(L.size()) + 1;
    M.resize(boost::extents[rows][cols]);

    for (unsigned i = 0; i < rows; ++i)
    {
        M[i][0] = i;
    }
    for (unsigned j = 1; j < cols; ++j)
    {
        unsigned stride = 1u << j;
        for (unsigned i = 0; i < rows; ++i)
        {
            if (i + stride/2 >= rows)
            {
                M[i][j] = M[i][j-1];
                continue;
            }
            unsigned idx1 = M[i][j-1];
            unsigned idx2 = M[i + stride/2][j-1];
            M[i][j] = L[idx2] < L[idx1] ? idx2 : idx1;
        }
    }

    lca_is_valid = true;
}


unsigned
TreeExtended::most_significant_bit(unsigned v)
{
    static const char LogTable256[] =
    {
        0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
    };

    unsigned bits = std::numeric_limits<unsigned>::digits;
    unsigned c = 0; // c will be lg(v) truncated
    while (bits > 8)
    {
        bits /= 2;
        if (v >> bits)
        {
            c += bits;
            v >>= bits;
        }
    }
    c += LogTable256[v];
    return c;
}

bool
TreeExtended::descendant(Node *v1, Node *v2) const
{
    /* Is v1 descendant of v2? */
    return lca(v1, v2) == v2;
}


double TreeExtended::findMaximumDistanceToLeaf(Node *n) const
{
    if (n->isLeaf())
    {
        return n->getBranchLength();
    }
    else
    {
        Node *left = n->getLeftChild();
        Node *right = n->getRightChild();

        double maxleft = findMaximumDistanceToLeaf(left);
        double maxright = findMaximumDistanceToLeaf(right);
        if(maxleft > maxright)
        {
            return n->getLeftChild()->getBranchLength() + 1;
        }
        else
        {
            return n->getRightChild()->getBranchLength() + 1;
        }
    }
}

unsigned TreeExtended::getNumberOfChildren(Node *n) const
{
    if(n == NULL)
    {
        return 0;
    }
    else
    {
        return (getNumberOfChildren(n->getLeftChild()) + 1)
                + (getNumberOfChildren(n->getRightChild()) + 1);
    }
}


void TreeExtended::reset()
{
    for(unsigned i = 0; i < getNumberOfNodes(); i++)
    {
        Node *n = getNode(i);
        n->setHostChild(NULL);
        n->setHostParent(NULL);
        n->setVisited(0);
        n->setX(0);
        n->setY(0);
        n->setReconcilation(Undefined);
    }
}
