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

#ifndef GAMMAMAPEX_CPP
#define GAMMAMAPEX_CPP

#include "GammaMapEx.h"

GammaMapEx::GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap& gs,
                          std::vector<SetOfNodesEx<Node> > &AC_info)
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


GammaMapEx::GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap& gs):
    Gtree(&G),
    Stree(&S),
    lambdaex(G, S, gs),
    gamma(S.getNumberOfNodes()),
    chainsOnNode(G.getNumberOfNodes()),
    transfer_edges(G.getNumberOfNodes())
{
}

GammaMapEx::GammaMapEx(const TreeExtended& G,const TreeExtended& S, const LambdaMapEx& L)
    : Gtree(&G),
      Stree(&S),
      lambdaex(L),
      gamma(S.getNumberOfNodes()),
      chainsOnNode(G.getNumberOfNodes()),
      transfer_edges(G.getNumberOfNodes())
{

}

GammaMapEx::GammaMapEx(const GammaMapEx& original)
    : Gtree(original.Gtree),
      Stree(original.Stree),
      lambdaex(original.lambdaex),
      gamma(original.gamma),
      chainsOnNode(original.chainsOnNode),
      transfer_edges(original.transfer_edges)
{

}

void
GammaMapEx::readGamma(Node* sn, vector<SetOfNodesEx<Node> >& AC_info)
{
    if(sn->isLeaf() == false)
    {
        readGamma(sn->getLeftChild(), AC_info);
        readGamma(sn->getRightChild(), AC_info);
    }
    SetOfNodesEx<Node> son = AC_info[sn->getNumber()];
    for (unsigned j = 0; j < son.size(); j++)
    {
        addToSet(sn, *son[j]);
    }
    return;
}

GammaMapEx& GammaMapEx::operator=(const GammaMapEx& gm)
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

Node* GammaMapEx::checkGamma(Node *gn)
{
    Node* sn = getLowestGammaPath(*gn);

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
        if (!sn->isLeaf())
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
        Node* gl = gn->getLeftChild();
        Node* gr = gn->getRightChild();
        Node* sl = checkGamma(gl);
        Node* sr = checkGamma(gr);

        if(sl == sr) // i.e., gn is a duplication
        {
            Node *temp = checkGammaForDuplication(gn, sn, sl, sr);
            sn = temp;
        }

        else
        {
            Node *temp = checkGammaForSpeciation(gn, sn, sl, sr);
            sn = temp;
        }
    }

    Node *temp = checkGammaMembership(gn, sn);
    sn = temp;
    return sn;
}

// To determine whether a node is lateral transfer or not, we just need to
// look a the set of lateral transfer nodes
const bool GammaMapEx::isLateralTransfer(Node& u) const
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


GammaMapEx
GammaMapEx::MostParsimonious(const TreeExtended& G,const TreeExtended& S,const LambdaMapEx& L)
{
    GammaMapEx gamma_star(G, S, L);
    gamma_star.computeGammaBound(G.getRootNode());
    return gamma_star;
}

LambdaMapEx GammaMapEx::getLambda() const
{
    return lambdaex;
}

LambdaMapEx *GammaMapEx::getLambda()
{
    return &lambdaex;
}

void
GammaMapEx::computeGammaBound(Node *v)
{
    computeGammaBoundBelow(v);
    // Now make sure that the root of the species tree got mapped.
    Node *sroot = Stree->getRootNode();
    if (getSize(sroot) == 0)
    {
        assignGammaBound(v, sroot);
    }
}

void
GammaMapEx::computeGammaBoundBelow(Node *v)
{
    assert(v != 0);

    if (v->isLeaf())
    {
        addToSet(lambdaex[v], *v);
    }
    else
    {
        Node *left = v->getLeftChild();
        Node *right = v->getRightChild();

        computeGammaBoundBelow(left);
        computeGammaBoundBelow(right);

        Node *x = lambdaex[v];
        Node *xl = lambdaex[left];
        Node *xr = lambdaex[right];
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

void
GammaMapEx::addToSet(Node *x,  Node *v)
{
    assert(x != 0);
    gamma[x->getNumber()].insert(v);
    chainsOnNode[v->getNumber()].push_back(x);
}

void
GammaMapEx::addToSet(Node *x, Node &v)
{
    assert(x != 0);
    addToSet(x, &v);
}

Node*
GammaMapEx::getHighestGammaPath(Node &u) const // Dominates all others on u
{
    const deque<Node*>& anti_chains = chainsOnNode[u.getNumber()];
    if (anti_chains.empty())
    {
        return 0;
    }
    else
    {
        return anti_chains.back();
    }
}

Node*
GammaMapEx::getLowestGammaPath(Node &u) const // Dominated by others
{
    const deque<Node*> &anti_chains = chainsOnNode[u.getNumber()];
    if (anti_chains.empty())
    {
        return 0;
    }
    else
    {
        return anti_chains.front();
    }
}

const unsigned GammaMapEx::getSize(Node& x) const   //version with reference
{
    return gamma[x.getNumber()].size();
}

const unsigned GammaMapEx::getSize(Node* x) const   //version with pointer
{
    return gamma[x->getNumber()].size();
}

Node *
GammaMapEx::checkGammaForDuplication(Node *gn, Node *sn, Node *sl, Node *sr)
{
    while(sn == sl)
    {
        removeFromSet(sn, gn);
        sn = getLowestGammaPath(*gn);
    }

    if(sn != 0)
    {
        if (*sn < *sl)
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

 Node*
GammaMapEx::checkGammaForSpeciation(Node *gn, Node *sn, Node *sl, Node *sr)   // gn is a speciation
{
    Node* sm = Stree->lca(sl, sr); // "lambda"
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

 Node*
GammaMapEx::checkGammaMembership(Node *gn, Node *sn)
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

 void
GammaMapEx::assignGammaBound(Node *v, Node *x)
{
    assert(x != 0);
    assert(v != 0);

    Node *y = lambdaex[v]->getParent();

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

  void
GammaMapEx::removeFromSet(Node *x, Node *v)
{
    assert(x != 0);
    if(v == 0)
    {
        return;
    }
    // Find iterator pos of x on v and remove x
    deque<Node*>& dref = chainsOnNode[v->getNumber()];
    typename deque<Node*>::iterator i = find(dref.begin(), dref.end(), x);
    if(i != dref.end())
    {
        dref.erase(i);
        gamma[x->getNumber()].erase(v);
    }
    return;
}


const bool
GammaMapEx::valid() const
{
    return valid(Stree->getRootNode());
}


const bool
GammaMapEx::valid(Node *x) const
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
                return false;
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


const bool
GammaMapEx::validLGT() const
{
    if (!transfer_edges.any())
    {
        return this->valid();
    }
    else
    {
        for( Node *n = Gtree->getPostOderBegin(); n != 0; n = Gtree->postorder_next(n))
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



const unsigned
GammaMapEx::sizeOfWidestSpeciesLeaf() const
{
    return sizeOfWidestSpeciesLeaf(Stree->getRootNode(), 0);
}


const unsigned GammaMapEx::sizeOfWidestSpeciesLeaf(Node *x, unsigned current_max) const
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


void
GammaMapEx::twistAndTurn()
{
    twistAndTurn(Gtree->getRootNode(), Stree->getRootNode());
}


void
GammaMapEx::twistAndTurn(Node *v, Node *x)
{

    if (v->isLeaf() || x->isLeaf())
    {
        // Done
    }
    else
    {
        Node *vl = v->getLeftChild();
        Node *vr = v->getRightChild();

        Node* xl = x->getLeftChild();
        Node* xr = x->getRightChild();

        Node* vll = lambdaex[vl];
        Node* vrl = lambdaex[vr];

        if (vll != lambdaex[v] && vrl != lambdaex[v])
        {
            if (vll == xr && vrl == xl)
            {
                v->setChildren(vr, vl);
            }
        }
        else if (vll != lambdaex[v])
        {
            Node *rep = x->getDominatingChild(vll);

            if (rep == xr)
            {
                v->setChildren(vr, vl);
            }
        }
        else if (vrl != lambdaex[v])
        {
            Node *rep = x->getDominatingChild(vrl);

            if (rep == xl)
            {
                v->setChildren(vr, vl);
            }
        }
        twistAndTurn(vl, vll);
        twistAndTurn(vr, vrl);
    }
}


const bool
GammaMapEx::isSpeciation(Node& u) const
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

string
GammaMapEx::print(bool full) const
{
    if(gamma.empty())
    {
        return "no gamma defined\n";
    }

    ostringstream oss;
    SetOfNodesEx<Node> gammaset;

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

SetOfNodesEx<Node>
GammaMapEx::getFullGamma(const Node& x) const
{
    const SetOfNodesEx<Node>& reduced = gamma[x.getNumber()];
    SetOfNodesEx<Node> full(reduced);
    Node* u;
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
        Node* p_x = x.getParent();

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

const bool
GammaMapEx::isInGamma(Node *u, Node *x) const
{
    const SetOfNodesEx<Node> &target_set = gamma[x->getNumber()];
    return target_set.member(u);
}

GammaMapEx
GammaMapEx::update(const TreeExtended& G, const TreeExtended& S,
                   const std::vector< unsigned > &sigma,
                   const dynamic_bitset<> &transfer_edges)
{
    lambdaex.update(G,S,sigma,transfer_edges);
    GammaMapEx gamma_star(G, S, lambdaex);
    gamma_star.setLGT(transfer_edges);
    gamma_star.computeGammaBound(G.getRootNode());
    return gamma_star;
}


const unsigned
GammaMapEx::numberOfGammaPaths(Node &u) const
{
    assert(chainsOnNode.size() > u.getNumber());
    const deque<Node*> &anti_chains = chainsOnNode[u.getNumber()];
    return anti_chains.size();
}


void GammaMapEx::setLGT(dynamic_bitset< long unsigned > lgt)
{
    this->transfer_edges = lgt;
}


dynamic_bitset<> GammaMapEx::getLGT()
{
    return this->transfer_edges;
}


SetOfNodesEx<Node>
GammaMapEx::getGamma(Node *x) const
{
    assert(x != 0);
    assert(x->getNumber() < gamma.size());
    return gamma[x->getNumber()];
}

#endif // GAMMAMAPEX_CPP
