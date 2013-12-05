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

#ifndef LAMBDAMAPEX_CPP
#define LAMBDAMAPEX_CPP

#include "LambdaMapEx.h"

LambdaMapEx::LambdaMapEx(unsigned Nnodes)
    : NodeVector(Nnodes),
      description()
{
}

LambdaMapEx::LambdaMapEx(const TreeExtended& G,const TreeExtended& S, const StrStrMap &gs)
    : NodeVector(G.getNumberOfNodes()),
      description()
{
    try
    {
        recursiveLambda(G.getRootNode(),S,gs);
    }
    catch (AnError& err)
    {
        throw err;
    }
    ostringstream oss;
    oss << "LambdaMap between guest tree" << G.getName()
        << " and host tree " << S.getName();
    description = oss.str();
}

LambdaMapEx::LambdaMapEx(const LambdaMapEx& l)
    : NodeVector(l),
      description(l.description)
{

}

LambdaMapEx::LambdaMapEx()
    : NodeVector(0),
      description()
{

}

LambdaMapEx&
LambdaMapEx::operator=(const LambdaMapEx &l)
{
    if (&l != this)
    {
        NodeVector::operator=(l);
        description = l.description;
    }
    return *this;
}

LambdaMapEx::~LambdaMapEx()
{

}

void LambdaMapEx::update(const TreeExtended& G, const TreeExtended& S, StrStrMap* gs)
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

void LambdaMapEx::update(const TreeExtended& G, const TreeExtended& S,
                         const std::vector< unsigned > &sigma,
                         const dynamic_bitset<> &transfer_edges)
{

    clearValues();

    for (Node *u = G.postorder_begin();
         u != 0;
         u = G.postorder_next(u))
    {
        // Take care of gene tree leaves and continue.
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

void LambdaMapEx::update(const TreeExtended &G, const TreeExtended& S,
                         const std::vector<unsigned> &lambda)
{
    for (unsigned i = 0; i < lambda.size(); i++)
    {
        pv[i] = S.getNode(lambda[i]);
    }

}

std::ostream&
operator<<(std::ostream &o, const LambdaMapEx& l)
{
    return o << l.print();
}

std::string
LambdaMapEx::print() const
{
    std::ostringstream oss;
    oss << description << ":\n";
    for(unsigned i = 0; i < pv.size(); i++)
    {
        oss << "\tLambda[" << i << "] = " << pv[i]->getNumber() << ";\n";
    }
    return oss.str();
}


Node* LambdaMapEx::recursiveLambda(Node *g,const TreeExtended& S, const StrStrMap& gs)
{
    if(g->isLeaf())
    {
        return compLeafLambda(g, S, gs);
    }
    else
    {

        Node *ls = recursiveLambda(g->getLeftChild(), S, gs);
        Node *rs = recursiveLambda(g->getRightChild(), S, gs);
        Node *s = S.mostRecentCommonAncestor(ls,rs);
        pv[g->getNumber()] = s;
        return s;
    }
}

Node* LambdaMapEx::recursiveLambda(Node *g,const TreeExtended& S)
{
    if(g->isLeaf())
    {
        return pv[g->getNumber()];
    }
    else
    {
        Node *ls = recursiveLambda(g->getLeftChild(), S);
        Node *rs = recursiveLambda(g->getRightChild(), S);
        Node *s = S.mostRecentCommonAncestor(ls,rs);
        pv[g->getNumber()] = s;
        return s;

    }
}

Node*
LambdaMapEx::compLeafLambda(Node *g, const TreeExtended& S, const StrStrMap& gs)
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
        Node *s = S.findLeaf(sp_name);
        pv[g->getNumber()] = s;
        return s;
    }
    catch (AnError& e)
    {
        cerr << "An error occured when trying to map genes to species.\n"
             << "Please verify that gene and species names are correct\n"
             << "and complete!\n\n";
        e.action();
        return 0;
    }
}

const bool LambdaMapEx::valid() const
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

#endif // LAMBDAMAPEX_CPP
