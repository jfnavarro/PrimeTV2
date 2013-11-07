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
 */

#ifndef GAMMAMAPEX_H
#define GAMMAMAPEX_H

#include "SetOfNodesEx.hh"
#include "LambdaMapEx.h"

#include "../tree/Treeextended.h"

#include "../Parameters.h"

#include <deque>
#include <boost/dynamic_bitset.hpp>

using namespace std;

/* Implement the map gamma : species node --> gene nodes */

template <class T>
class GammaMapEx 
{
public:
    
    //constructors for different scenarios
    GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap &gs, std::vector<SetOfNodesEx<T> > &AC_info);
    GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap &gs);
    GammaMapEx(const TreeExtended &G,const TreeExtended &S, const LambdaMapEx<T> &L);
    GammaMapEx(const GammaMapEx& original);
    
    //overload of operator =
    GammaMapEx& operator=(const GammaMapEx &gm);
    
    /* Recursive function for reading a user-set gamma in tabular format
    If chainsOnNode is changed to a set,*/
    void readGamma(T* sn, std::vector<SetOfNodesEx<T> >& AC_info);
    
    //generates the most parimonious gamma object
    static GammaMapEx MostParsimonious(const TreeExtended &G,const TreeExtended &S,const LambdaMapEx<T> &L);
    
    /* these functions are going to compute and load the chainOnNode vector which is a map
     * of species nodes mapped for each gene node*/
    void computeGammaBound(T *g);
    void computeGammaBoundBelow(T *g);
    void assignGammaBound(T *u, T *x);
    
    //Check that current gamma is valid, throws AnError if not!
    T* checkGamma(T *gn);
    T *checkGammaForDuplication(T *gn, T *sn, T *sl, T *sr);
    T *checkGammaForSpeciation(T *gn, T *sn, T *sl, T *sr);
    T *checkGammaMembership(T *gn, T *sn);
    
    // check if the node given as a parameter is a LGT or not
    bool isLateralTransfer(T &u) const;
    
    // returns the lambdamap
    LambdaMapEx<T> getLambda() const;
    LambdaMapEx<T> *getLambda();
    
    /* print the gamma on a string file */
    string print(const bool& full) const;
    
    /*Puts u in gamma(x), and registers x as having an anti-chain for x.
    Would like to make x argument const below, but then I am not allowed
    to put x in a dequeue!*/
    void addToSet(T *x, T *u);
    void addToSet(T *x, T &u);
    
    
    /*The following methods retrieves the max-element, according
    to porder, of the gamma-path on u, i.e., \f$ \{x: u\in\gamma(x)\} \f$,
     Null is returned if there is no anti-chain on u.*/
    T* getHighestGammaPath(T &u) const;
    T* getLowestGammaPath(T &u) const;
    
    //Get the size of gamma(x), i.e., the number of GeneNodes mapped to x.
    unsigned getSize(T& x) const;
    unsigned getSize(T* x) const;
    
    //remove node from the set
    void removeFromSet(T *x, T *v);
    
    // check validity of the gamma
    bool valid() const;
    bool valid(T *x) const;
    
    //check validity of the gamma for a scenario loaded
    bool validLGT() const;
    
    // to obtaing the widest species leaf
    unsigned sizeOfWidestSpeciesLeaf() const;
    unsigned sizeOfWidestSpeciesLeaf(T *x, unsigned cur_max) const;
    
    // alternate left and right in the tree
    void twistAndTurn();
    void twistAndTurn(T *v, T *x);
    
    /* returns true if the node given is speciation */
    bool isSpeciation(T& u) const;
    
    /* return the gamma map */
    SetOfNodesEx<T> getFullGamma(const T& x) const;
    
    /* true is that combination is in the gamma map */
    bool isInGamma(T *u, T *x) const;
    
    /* update the gamma to include Lateral Gene transfer */
    GammaMapEx update(const TreeExtended &G, const TreeExtended &S, const std::vector<unsigned> &sigma,
                      const dynamic_bitset<> &transfer_edges);
    
    //Count the gamma-paths on a gene
    unsigned numberOfGammaPaths(Node &u) const;
    
    
    SetOfNodesEx<T> getGamma(T *x) const;
    
private:

    /* returns the LGT vector*/
    dynamic_bitset<> getLGT();
    /* set the LGT vector*/
    void setLGT(dynamic_bitset<> lgt);
    
    const TreeExtended *Gtree;
    const TreeExtended *Stree;
    LambdaMapEx<T> lambdaex;
    std::vector<SetOfNodesEx<T> > gamma;
    std::vector<std::deque<T*> > chainsOnNode;
    dynamic_bitset<> transfer_edges;
};

/************************************************************************************************************************************/


template <class T>
GammaMapEx<T>::GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap& gs,
                          std::vector<SetOfNodesEx<T> > &AC_info)
    : Gtree(&G),
      Stree(&S),
      lambdaex(G,S,gs),
      gamma(S.getNumberOfNodes()),
      chainsOnNode(G.getNumberOfNodes()),
      transfer_edges(G.getNumberOfNodes())
{
    readGamma(S.getRootNode(), AC_info);
    checkGamma(G.getRootNode());
}

template <class T>
GammaMapEx<T>::GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap& gs):
    Gtree(&G),
    Stree(&S),
    lambdaex(G, S, gs),
    gamma(S.getNumberOfNodes()),
    chainsOnNode(G.getNumberOfNodes()),
    transfer_edges(G.getNumberOfNodes())
{
}


template <class T>
GammaMapEx<T>::GammaMapEx(const TreeExtended& G,const TreeExtended& S, const LambdaMapEx<T>& L)
    : Gtree(&G),
      Stree(&S),
      lambdaex(L),
      gamma(S.getNumberOfNodes()),
      chainsOnNode(G.getNumberOfNodes()),
      transfer_edges(G.getNumberOfNodes())
{

}

template <class T>
GammaMapEx<T>::GammaMapEx(const GammaMapEx<T>& original)
    : Gtree(original.Gtree),
      Stree(original.Stree),
      lambdaex(original.lambdaex),
      gamma(original.gamma),
      chainsOnNode(original.chainsOnNode),
      transfer_edges(original.transfer_edges)
{

}

template <class T>
void
GammaMapEx<T>::readGamma(T* sn, vector<SetOfNodesEx<T> >& AC_info)
{
    if(sn->isLeaf() == false)
    {
        readGamma(sn->getLeftChild(), AC_info);
        readGamma(sn->getRightChild(), AC_info);
    }
    SetOfNodesEx<T> son = AC_info[sn->getNumber()];
    for (unsigned j = 0; j < son.size(); j++)
    {
        addToSet(sn, *son[j]);
    }
    return;
}

template <class T>
GammaMapEx<T>& GammaMapEx<T>::operator=(const GammaMapEx<T>& gm)
{
    if (this != &gm)
    {
        Gtree = gm.Gtree;
        Stree = gm.Stree;
        lambdaex = gm.lambdaex;
        gamma = gm.gamma;
        chainsOnNode = gm.chainsOnNode;
        transfer_edges = gm.transfer_edges;
    }
    return *this;
}

template <class T>
T* GammaMapEx<T>::checkGamma(T *gn)
{
    T* sn = getLowestGammaPath(*gn);

    if(gn->isLeaf())
    {
        // Lowest antichain of a leaf in G should always be a leaf in S
        if(sn == 0)
        {
            ostringstream oss;
            oss << "Reconciliation error:\nGuest tree leaf '"
                << gn->getNumber()
                << "' with label '"
                << gn->getName()
                << "' is not mapped to a species node.";
            throw AnError(oss.str(), 1);
        }
        if(!sn->isLeaf())
        {
            ostringstream oss;
            oss << "Reconciliation error:\nGuest tree leaf '"
                << gn->getNumber()
                << "' with label '"
                << gn->getName()
                << "' is not mapped to a species tree leaf.\n"
                << "The current mapping is to '"
                << sn
                << "', curiously!\n";
            throw AnError(oss.str(), 1);
        }
    }
    else
    {
        // pass recursion on
        T* gl = gn->getLeftChild();
        T* gr = gn->getRightChild();
        T* sl = checkGamma(gl);
        T* sr = checkGamma(gr);

        if(sl == sr) // i.e., gn is a duplication
        {
            T *temp = checkGammaForDuplication(gn, sn, sl, sr);
            sn = temp;
        }

        else
        {
            T *temp = checkGammaForSpeciation(gn, sn, sl, sr);
            sn = temp;
        }
    }
    
    T *temp = checkGammaMembership(gn, sn);
    sn = temp;
    return sn;
}


// To determine whether a node is lateral transfer or not, we just need to 
// look a the set of lateral transfer nodes

template <class T>
bool GammaMapEx<T>::isLateralTransfer(T& u) const
{
    if(u.isLeaf())
    {
        return false;
    }
    else
    {
        bool right = transfer_edges[u.getRightChild()->getNumber()];
        bool left = transfer_edges[u.getLeftChild()->getNumber()];
        return (bool)(right || left);
    }
}


template <class T>
GammaMapEx<T>
GammaMapEx<T>::MostParsimonious(const TreeExtended& G,const TreeExtended& S,const LambdaMapEx<T>& L)
{
    GammaMapEx gamma_star(G, S, L);
    gamma_star.computeGammaBound(G.getRootNode());
    return gamma_star;
}

template <class T>
LambdaMapEx<T> GammaMapEx<T>::getLambda() const
{
    return lambdaex;
}

template <class T>
LambdaMapEx<T> *GammaMapEx<T>::getLambda() 
{
    return &lambdaex;
}

template <class T>
void
GammaMapEx<T>::computeGammaBound(T *v)
{
    computeGammaBoundBelow(v);
    // Now make sure that the root of the species tree got mapped.
    T *sroot = Stree->getRootNode();
    if (getSize(sroot) == 0)
    {
        assignGammaBound(v, sroot);
    }
}

template <class T>
void
GammaMapEx<T>::computeGammaBoundBelow(T *v)
{
    assert(v != NULL);

    if (v->isLeaf())
    {
        addToSet(lambdaex[v], *v);
    }
    else
    {
        T *left = v->getLeftChild();
        T *right = v->getRightChild();

        computeGammaBoundBelow(left);
        computeGammaBoundBelow(right);

        T *x = lambdaex[v];
        T *xl = lambdaex[left];
        T *xr = lambdaex[right];
        if (x != xl && x != xr)
        {
            addToSet(x, *v);
            assignGammaBound(left, x->getDominatingChild(xl));
            assignGammaBound(right,  x->getDominatingChild(xr));
        }
        else if (x != xl) //lateral transfer in xl
        {
            assignGammaBound(left, x); // Include x!
        }
        else if (x != xr) //lateral transfer in xr
        {
            assignGammaBound(right, x); // Include x!
        }
    }
}

template <class T>
void
GammaMapEx<T>::addToSet(T *x,  T *v) 
{
    assert(x != NULL);
    gamma[x->getNumber()].insert(v);
    chainsOnNode[v->getNumber()].push_back(x);

}

template <class T>
void
GammaMapEx<T>::addToSet(T *x, T &v)
{
    assert(x != NULL);
    addToSet(x, &v);
}

template <class T>  T*
GammaMapEx<T>::getHighestGammaPath(T &u) const // Dominates all others on u
{
    const deque<T*>& anti_chains = chainsOnNode[u.getNumber()];
    if (anti_chains.empty())
    {
        return NULL;
    }
    else
    {
        return anti_chains.back();
    }
}

template <class T> T* 
GammaMapEx<T>::getLowestGammaPath(T &u) const // Dominated by others
{
    const deque<T*> &anti_chains = chainsOnNode[u.getNumber()];
    if (anti_chains.empty())
    {
        return NULL;
    }
    else
    {
        return anti_chains.front();
    }
}

template <class T>  unsigned
GammaMapEx<T>::getSize(T& x) const   //version with reference
{
    return gamma[x.getNumber()].size();
}

template <class T> unsigned
GammaMapEx<T>::getSize(T* x) const   //version with pointer
{
    return gamma[x->getNumber()].size();
}

template <class T> T *
GammaMapEx<T>::checkGammaForDuplication(T *gn, T *sn, T *sl, T *sr)
{
    while(sn == sl)
    {
        removeFromSet(sn, gn);
        sn = getLowestGammaPath(*gn);
    }

    if(sn != 0)
    {
        if(*sn < *sl)
        {
            ostringstream oss;
            oss << "Reconciliation error:\nThe host nodes that the "
                << "children of guest node '"
                << gn->getNumber()
                << "' are ancestral\nto the host node that guest node '"
                << gn->getNumber()
                << "' itself is mapped to\n";
            throw AnError(oss.str(), 1);
        }
        else if(sn != sl->getParent())
        {
            ostringstream oss;
            oss << "Reconcilation error:\nThe subtree rooted at guest node '"
                << gn->getNumber()
                << "' is missing from gamma("
                << sl->getParent()->getNumber()
                << ")\n";
            throw AnError(oss.str(), 1);
        }

        return sn;
    }
    else
    {
        return sl;
    }
}

template <class T> T*
GammaMapEx<T>::checkGammaForSpeciation(T *gn, T *sn, T *sl, T *sr)   // gn is a speciation 
{
    T* sm = Stree->lca(sl, sr); // "lambda"
    while(sn == sl)
    {
        removeFromSet(sn, gn);
        sn = getLowestGammaPath(*gn);
    }

    if(sn == 0 || sn != sm)
    {
        ostringstream oss;
        oss << "Reconcilation error:\nGuest node '"
            << gn->getNumber()
            << "' should be a speciation and map to host node '"
            << sm->getNumber() << "'\n";
        throw AnError(oss.str(), 1);
    }
    else if(sl->getParent() != sm || sr->getParent() != sm)
    {
        Node* gl = gn->getLeftChild();
        Node* gr = gn->getRightChild();
        ostringstream oss;
        oss << "Reconciliation error:\nSubtrees rooted at guest nodes "
            << gl->getNumber()
            << " and/or "
            << gr->getNumber()
            << " must map to\na child of host node "
            << sm->getNumber()
            << ", but not to any of their ancestors\n";
        throw AnError(oss.str(), 1);
    }
    return sn;
}

template <class T> T*
GammaMapEx<T>::checkGammaMembership(T *gn, T *sn)
{
    for (unsigned i = 1; i < chainsOnNode[gn->getNumber()].size(); i++)
    {
        if (sn->getParent() != chainsOnNode[gn->getNumber()][i])
        {
            ostringstream oss;
            oss << "Reconciliation error:\nThe host nodes to which guest node "
                << gn->getNumber()
                << " is mapped must form a path.\nIn particular, host node "
                << chainsOnNode[gn->getNumber()][i]->getNumber()
                << " is not the parent of host node "
                << sn->getNumber()
                << "\n";
            throw AnError(oss.str(), 1);
        }
        sn = sn->getParent();
    }
    return sn;
}

template <class T> void
GammaMapEx<T>::assignGammaBound(T *v, T *x)
{
    assert(x != NULL);
    assert(v != NULL);

    T *y = lambdaex[v]->getParent();

    while (x->dominates(*y))
    {
        addToSet(y, *v);
        y = y->getParent();
        if (!y)
        {
            break;
        }
    }
}

template <class T>  void
GammaMapEx<T>::removeFromSet(T *x, T *v)
{
    assert(x != NULL);
    if(v == 0)
    {
        return;
    }
    // Find iterator pos of x on v and remove x
    deque<T*>& dref = chainsOnNode[v->getNumber()];
    typename deque<T*>::iterator i = find(dref.begin(), dref.end(), x);
    if(i != dref.end())
    {
        dref.erase(i);
        gamma[x->getNumber()].erase(v);
    }
    return;
}

template <class T>
bool
GammaMapEx<T>::valid() const
{
    try
    {
        valid(Stree->getRootNode());
    }
    catch (int i)
    {
        return false;
    }
    return true;
}

template <class T>
bool
GammaMapEx<T>::valid(T *x) const
{
    if (x->isLeaf())
    {
        if (getSize(x) > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        bool l = valid(x->getLeftChild());
        bool r = valid(x->getRightChild());
        if (l || r)
        {
            if (getSize(x) == 0)
            {
                throw 1;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }
}

template <class T>
bool
GammaMapEx<T>::validLGT() const
{
    if(!transfer_edges.any())
    {
        return this->valid();
    }
    else
    {
        for( Node *n = Gtree->getPostOderBegin(); n != NULL; n = Gtree->postorder_next(n))
        {
            if(transfer_edges[n->getNumber()])
            {
                Node *destiny = lambdaex[n];
                Node *origin = lambdaex[n->getParent()->getLeftChild()];
                if(origin == destiny)
                {
                    origin = lambdaex[n->getParent()->getRightChild()];
                }
                if(destiny->getNodeTime() > origin->getParent()->getNodeTime()
                        || destiny->isRoot()
                        || origin->isRoot()
                        || n->getParent()->isRoot())
                {
                    return false;
                }
            }
            else if(n->isRoot() && lambdaex[n] != Stree->getRootNode())
            {
                return false;
            }
        }
        return true && this->valid();
    }
}


template <class T>
unsigned
GammaMapEx<T>::sizeOfWidestSpeciesLeaf() const
{
    return sizeOfWidestSpeciesLeaf(Stree->getRootNode(), 0);
}

template <class T>
unsigned 
GammaMapEx<T>::sizeOfWidestSpeciesLeaf(T *x, unsigned current_max) const
{
    if (x->isLeaf())
    {
        unsigned w = getSize(x);
        if (w > current_max)
        {
            return w;
        }
        else
        {
            return current_max;
        }
    }
    else
    {
        current_max = sizeOfWidestSpeciesLeaf(x->getLeftChild(), current_max);
        current_max = sizeOfWidestSpeciesLeaf(x->getRightChild(), current_max);
        return current_max;
    }
}

template <class T>
void
GammaMapEx<T>::twistAndTurn()
{
    twistAndTurn(Gtree->getRootNode(), Stree->getRootNode());
}

template <class T>
void
GammaMapEx<T>::twistAndTurn(T *v, T *x)
{

    if (v->isLeaf() || x->isLeaf())
    {
        // Done
    }
    else
    {
        T *vl = v->getLeftChild();
        T *vr = v->getRightChild();

        T* xl = x->getLeftChild();
        T* xr = x->getRightChild();

        T* vll = lambdaex[vl];
        T* vrl = lambdaex[vr];

        if (vll != lambdaex[v] && vrl != lambdaex[v])
        {
            if (vll == xr && vrl == xl)
            {
                v->setChildren(vr, vl);
            }
        }
        else if (vll != lambdaex[v])
        {
            T *rep = x->getDominatingChild(vll);

            if (rep == xr)
            {
                v->setChildren(vr, vl);
            }
        }
        else if (vrl != lambdaex[v])
        {
            T *rep = x->getDominatingChild(vrl);

            if (rep == xl)
            {
                v->setChildren(vr, vl);
            }
        }
        twistAndTurn(vl, vll);
        twistAndTurn(vr, vrl);
    }
}

template <class T>
bool
GammaMapEx<T>::isSpeciation(T& u) const
{
    if (lambdaex[u] == getLowestGammaPath(u))
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
string
GammaMapEx<T>::print(const bool& full) const
{
    if(gamma.empty())
    {
        return "no gamma defined\n";
    }
    
    ostringstream oss;
    SetOfNodesEx<T> gammaset;

    for (unsigned i = 0; i < gamma.size(); i++)
    {
        if(full)
        {
            gammaset = getFullGamma(*Stree->getNode(i));
        }
        else
        {
            gammaset = gamma[i];
        }
        if (!gammaset.empty())
        {
            oss << i << "\t";
            for (unsigned j = 0; j < gammaset.size(); j++)
            {
                if(j != 0)
                {
                    oss << ", " ;
                }
                oss << gammaset[j]->getNumber();
            }
            oss << "\n";
        }
        else
        {
            oss << i << "\n"; //"\tnot mapped\n";
        }
    }
    return oss.str();
}

template <class T>
SetOfNodesEx<T>
GammaMapEx<T>::getFullGamma(const T& x) const
{
    const SetOfNodesEx<T>& reduced = gamma[x.getNumber()];
    SetOfNodesEx<T> full(reduced);
    T* u;
    if(x.isRoot())  // Then include subtree induced by reduced
    {
        for(unsigned i = 0; i < reduced.size(); i++)
        {
            u = reduced[i];

            while(u->isRoot() == false)
            {
                u = u->getParent();
                full.insert(u);
            }
        }
    }
    else
    {
        T* p_x = x.getParent();

        for(unsigned i = 0; i < reduced.size(); i++)
        {
            u = reduced[i]; // get the current node

            while(isInGamma(u, p_x) == false) // else we've reached top of slice
            {
                u = u->getParent();
                if(x.dominates(*lambdaex[u])) // Don't include speciations
                {
                    full.insert(u);
                }
            }
        }
    }
    return full;
}

template <class T>
bool
GammaMapEx<T>::isInGamma(T *u, T *x) const
{
    const SetOfNodesEx<T> &target_set = gamma[x->getNumber()];
    return target_set.member(u);
}

template <class T>
GammaMapEx<T>
GammaMapEx<T>::update(const TreeExtended& G, const TreeExtended& S, const std::vector< unsigned int > &sigma, 
                      const dynamic_bitset<> &transfer_edges)
{
    lambdaex.update(G,S,sigma,transfer_edges);
    GammaMapEx gamma_star(G, S, lambdaex);
    gamma_star.setLGT(transfer_edges);
    gamma_star.computeGammaBound(G.getRootNode());
    return gamma_star;
}   

template <class T>
unsigned 
GammaMapEx<T>::numberOfGammaPaths(Node &u) const
{
    assert(chainsOnNode.size() > u.getNumber());
    const deque<T*> &anti_chains = chainsOnNode[u.getNumber()];
    return anti_chains.size();
}

template <class T>
void GammaMapEx<T>::setLGT(dynamic_bitset< long unsigned int > lgt)
{
    this->transfer_edges = lgt;
}

template <class T>
dynamic_bitset<> GammaMapEx<T>::getLGT()
{
    return this->transfer_edges;
}

template <class T>
SetOfNodesEx<T>
GammaMapEx<T>::getGamma(T *x) const
{
    assert(x != NULL);
    assert(x->getNumber() < gamma.size());
    return gamma[x->getNumber()];
}


#endif // GAMMAMAPEX_H
