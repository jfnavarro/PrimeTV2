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
#include "../utils/AnError.h"
#include "../Parameters.h"
#include "../reconcilation/BeepVector.h"
#include "../reconcilation/StrStrMap.h"

#include "string.h"

#include <boost/dynamic_bitset.hpp>


using namespace std;
using boost::dynamic_bitset;

class LambdaMapEx : public NodeVector
{

public:

    //constructors
    explicit LambdaMapEx(unsigned Nnodes);
    explicit LambdaMapEx(const LambdaMapEx& l);
    explicit LambdaMapEx();
    explicit LambdaMapEx(const TreeExtended &G,const TreeExtended &S, const StrStrMap &gs);

    //destructor
    virtual ~LambdaMapEx();
    
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
    std::ostream& operator<<(const LambdaMapEx &l);
    
    //print lambda
    std::string print() const;
    
    //check whether is valid of not
    const bool valid() const;

private:

    //recursive function to calculate the gamma as described
    Node* recursiveLambda(Node *g,const TreeExtended &S, const StrStrMap& gs);
    Node* recursiveLambda(Node *g,const TreeExtended &S);
    
    // return the speies node assigned to that gene node on the map
    Node* compLeafLambda(Node *g, const TreeExtended &S, const StrStrMap &gs);
    std::string description;
};

#endif // LAMBDAMAPEX_H
