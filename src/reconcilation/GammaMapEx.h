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

#include "SetOfNodesEx.h"
#include "LambdaMapEx.h"

#include "../tree/Treeextended.h"

#include "../Parameters.h"

#include <deque>
#include <boost/dynamic_bitset.hpp>

using namespace std;

/* Implement the map gamma : species node --> gene nodes */

class GammaMapEx 
{
public:
    
    //constructors for different scenarios
    explicit GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap &gs,
               std::vector<SetOfNodesEx<Node> > &AC_info);
    explicit GammaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap &gs);
    explicit GammaMapEx(const TreeExtended &G,const TreeExtended &S, const LambdaMapEx &L);

    GammaMapEx(const GammaMapEx& original);
    
    virtual ~GammaMapEx() {};

    //overload of operator =
    GammaMapEx& operator=(const GammaMapEx &gm);
    
    // Recursive function for reading a user-set gamma in tabular format
    // If chainsOnNode is changed to a set,
    void readGamma(Node *sn, std::vector<SetOfNodesEx<Node> >& AC_info);
    
    //generates the most parimonious gamma object
    static GammaMapEx MostParsimonious(const TreeExtended &G,const TreeExtended &S,const LambdaMapEx &L);
    
    // these functions are going to compute and load the chainOnNode vector which is a map
    // of species nodes mapped for each gene node
    void computeGammaBound(Node *g);
    void computeGammaBoundBelow(Node *g);
    void assignGammaBound(Node *u, Node *x);
    
    //Check that current gamma is valid, throws AnError if not!
    Node* checkGamma(Node *gn);
    Node* checkGammaForDuplication(Node *gn, Node *sn, Node *sl, Node *sr);
    Node* checkGammaForSpeciation(Node *gn, Node *sn, Node *sl, Node *sr);
    Node* checkGammaMembership(Node *gn, Node *sn);
    
    // check if the node given as a parameter is a LGT or not
    const bool isLateralTransfer(Node &u) const;
    
    // returns the lambdamap
    const LambdaMapEx& getLambda();

    // print the gamma on a string file
    string print(bool full) const;
    
    // Puts u in gamma(x), and registers x as having an anti-chain for x.
    // Would like to make x argument const below, but then I am not allowed
    // to put x in a dequeue!
    void addToSet(Node *x, Node *u);
    void addToSet(Node *x, Node &u);
    
    // The following methods retrieves the max-element, according
    // to porder, of the gamma-path on u, i.e., \f$ \{x: u\in\gamma(x)\} \f$,
    // Null is returned if there is no anti-chain on u.
    Node* getHighestGammaPath(Node &u) const;
    Node* getLowestGammaPath(Node &u) const;
    
    //Get the size of gamma(x), i.e., the number of GeneNodes mapped to x.
    const unsigned getSize(Node &x) const;
    const unsigned getSize(Node *x) const;
    
    //remove node from the set
    void removeFromSet(Node *x, Node *v);
    
    // check validity of the gamma
    const bool valid() const;
    const bool valid(Node *x) const;
    
    //check validity of the gamma for a scenario loaded
    const bool validLGT() const;
    
    // to obtaing the widest species leaf
    const unsigned sizeOfWidestSpeciesLeaf() const;
    const unsigned sizeOfWidestSpeciesLeaf(Node *x, unsigned cur_max) const;
    
    // alternate left and right (cordinates) for each node in the tree (if needed)
    void twistAndTurn();
    void twistAndTurn(Node *v, Node *x);
    void twistAndTurn(TreeExtended *G, TreeExtended *S);

    // returns true if the node given is speciation
    const bool isSpeciation(Node &u) const;
    
    // return the gamma map
    SetOfNodesEx<Node> getFullGamma(const Node& x) const;
    
    // true is that combination is in the gamma map
    const bool isInGamma(Node *u, Node *x) const;
    
    // update the gamma to include Lateral Gene transfer
    void update(const TreeExtended &G, const TreeExtended &S,
                      const std::vector<unsigned> &sigma,
                      const dynamic_bitset<> &transfer_edges);
    
    //Count the gamma-paths on a gene
    const unsigned numberOfGammaPaths(Node &u) const;
    
    SetOfNodesEx<Node> getGamma(Node *x) const;
    
private:

    // returns the LGT vector
    dynamic_bitset<> getLGT();
    // set the LGT vector
    void setLGT(dynamic_bitset<> lgt);
    
    const TreeExtended *Gtree;
    const TreeExtended *Stree;
    LambdaMapEx lambdaex;
    std::vector<SetOfNodesEx<Node> > gamma;
    std::vector<std::deque<Node*> > chainsOnNode;
    dynamic_bitset<> transfer_edges;
};


#endif // GAMMAMAPEX_H
