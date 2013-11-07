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
             Bengt Sennblad, © the MCMC-club, SBC, all rights reserved
 */

/* LambdaMap implements the map between gene node and species node,
   called \sigma in our JACM paper. For historical reasons, however,
   the class is unfortunately named LambdaMap.

   lambda is defined as follows
   1. If g \in leaves(G) then \lambda(g) = s \in leaves(S), in the
      natural way.
   2. Otherwise, \lambda(g) = MRCA(lambda(left(g)), lambda(right(g))).

   In case of the Lateral Transfer Lambda(g) will be either lambda(left(g)
   or lambda(right(g)
*/


#ifndef LAMBDAMAPEX_H
#define LAMBDAMAPEX_H

#include "../tree/Treeextended.h"
#include "../utils/AnError.hh"
#include "../Parameters.h"
#include "../reconcilation/BeepVector.hh"
#include "../reconcilation/StrStrMap.hh"

#include "string.h"

#include <boost/dynamic_bitset.hpp>


using namespace std;
using boost::dynamic_bitset;


template <class T>
class LambdaMapEx : public NodeVector
{

public:

    //constructors
    LambdaMapEx(unsigned Nnodes);
    LambdaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap &gs);
    LambdaMapEx(const LambdaMapEx& l);
    LambdaMapEx();
    
    //destructor
    ~LambdaMapEx();
    
    //overload of operator =
    LambdaMapEx& operator=(const LambdaMapEx& l);
    
    //update lambda for a normal reconcilation
    void update(const TreeExtended &G,const TreeExtended &S, StrStrMap* gs = 0);
    
    //update lambda with the Lateral Transfers
    void update(const TreeExtended &G, const TreeExtended &S, const std::vector<unsigned> &sigma,
                const dynamic_bitset<> &transfer_edges);
    
    //update lambda for a normal reconcilation for a lambda given
    void update(const TreeExtended &G, const TreeExtended& S,const std::vector<unsigned> &lambda);

    //overload << for printing
    std::ostream& operator<<(const LambdaMapEx<T>& l);
    
    //print lambda
    std::string print() const;
    
    //check whether is valid of not
    bool valid() const;

private:

    //recursive function to calculate the gamma as described
    T* recursiveLambda(T *g,const TreeExtended &S, const StrStrMap& gs);
    T* recursiveLambda(T *g,const TreeExtended &S);
    
    // return the speies node assigned to that gene node on the map
    T* compLeafLambda(T *g, const TreeExtended &S, const StrStrMap &gs);
    std::string description;
};

/******************************************************************************************************************************************/

template <class T>
LambdaMapEx<T>::LambdaMapEx(unsigned Nnodes)
    :NodeVector(Nnodes),
      description()
{
}

template <class T>
LambdaMapEx<T>::LambdaMapEx(const TreeExtended& G,const TreeExtended& S, const StrStrMap &gs)
    : NodeVector(G.getNumberOfNodes()),
      description()
{
    try
    {
        recursiveLambda(G.getRootNode(),S,gs);
    }
    catch (AnError& err)
    {
        err.action();
    }
    ostringstream oss;
    oss << "LambdaMap between guest tree" << G.getName()
        << " and host tree " << S.getName();
    description = oss.str();
}

template <class T>   
LambdaMapEx<T>::LambdaMapEx(const LambdaMapEx& l)
    : NodeVector(l),
      description(l.description)
{

}

template <class T>
LambdaMapEx<T>::LambdaMapEx()
    :NodeVector(0),
      description()
{

}


template <class T>
LambdaMapEx<T>&
LambdaMapEx<T>::operator=(const LambdaMapEx<T>& l)
{
    if(&l != this)
    {
        NodeVector::operator=(l);
        description = l.description;
    }
    return *this;
}

template <class T>
LambdaMapEx<T>::~LambdaMapEx()
{

}

template <class T>
void LambdaMapEx<T>::update(const TreeExtended& G, const TreeExtended& S, StrStrMap* gs)
{
    if(gs)
    {
        recursiveLambda(G.getRootNode(), S, *gs);
    }
    else
    {
        recursiveLambda(G.getRootNode(), S);
    }
    return;
}

template <class T>
void LambdaMapEx<T>::update(const TreeExtended& G, const TreeExtended& S, const std::vector< unsigned int > &sigma, 
                            const dynamic_bitset<> &transfer_edges)
{

    clearValues();

    for (Node *u = G.getPostOderBegin();
         u != NULL;
         u = G.postorder_next(u))
    {
        /* Take care of gene tree leaves and continue. */
        if (u->isLeaf())
        {
            pv[u->getNumber()] = S.getNode(sigma[u->getNumber()]);
            continue;
        }
        Node *v = u->getLeftChild();
        Node *w = u->getRightChild();

        if ((bool)transfer_edges[v->getNumber()])
        {
            pv[u->getNumber()] = pv[w->getNumber()];
        }
        else if ((bool)transfer_edges[w->getNumber()])
        {
            pv[u->getNumber()] = pv[v->getNumber()];
        }
        else
        {
            pv[u->getNumber()] = S.lca(pv[w->getNumber()],pv[v->getNumber()]);
        }
    }
}	

template <class T>
void LambdaMapEx<T>::update(const TreeExtended &G, const TreeExtended& S,const std::vector<unsigned> &lambda)
{
    for (unsigned i = 0; i < lambda.size(); i++)
    {
        pv[i] = S.getNode(lambda[i]);
    }

}

template <class T>
std::ostream& 
operator<<(std::ostream &o, const LambdaMapEx<T>& l)
{
    return o << l.print();
};

template <class T>
std::string 
LambdaMapEx<T>::print() const
{
    std::ostringstream oss;
    oss << description << ":\n";
    for(unsigned i = 0; i < pv.size(); i++)
    {
        oss << "\tLambda[" << i << "] = " << pv[i]->getNumber() << ";\n";
    }
    return oss.str();
}


template <class T>
T* LambdaMapEx<T>::recursiveLambda(T *g,const TreeExtended& S, const StrStrMap& gs)
{
    if(g->isLeaf())
    {
        return compLeafLambda(g, S, gs);
    }
    else
    {

        T *ls = recursiveLambda(g->getLeftChild(), S, gs);
        T *rs = recursiveLambda(g->getRightChild(), S, gs);
        T *s = S.mostRecentCommonAncestor(ls,rs);
        pv[g->getNumber()] = s;
        return s;
    }
}

template <class T>
T* LambdaMapEx<T>::recursiveLambda(T *g,const TreeExtended& S)
{
    if(g->isLeaf())
    {
        return pv[g->getNumber()];
    }
    else
    {
        T *ls = recursiveLambda(g->getLeftChild(), S);
        T *rs = recursiveLambda(g->getRightChild(), S);
        T *s = S.mostRecentCommonAncestor(ls,rs);
        pv[g->getNumber()] = s;
        return s;

    }
}

template <class T>
T*
LambdaMapEx<T>::compLeafLambda(T *g, const TreeExtended& S, const StrStrMap& gs)
{
    string genename = g->getName();
    const string sp_name = gs.find(genename);
    if (sp_name.empty())
    {
        throw AnError("Input inconsistency: "
                      "Leaf name missing in gene-to-species data.",
                      genename, 1);
    }

    try
    {
        T *s = S.findLeaf(sp_name);
        pv[g->getNumber()] = s;
        return s;
    }
    catch (AnError& e)
    {
        cerr << "An error occured when trying to map genes to species.\n"
             << "Please verify that gene and species names are correct\n"
             << "and complete!\n\n";
        e.action();
        return NULL;
    }
}

template <class T>
bool LambdaMapEx<T>::valid() const
{
    for (unsigned i = 0; i < pv.size(); i++)
    {
        if(pv[i]->isRoot())
        {
            return true;
        }
    }
    return false;
}

#endif // LAMBDAMAPEX_H
