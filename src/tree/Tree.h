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

#ifndef TREE_HH
#define TREE_HH

#include <map>
#include <string>
#include <vector>

// Forward declarations.
class Node;
class RealVector;
using namespace std;

const unsigned DEF_NODE_VEC_SIZE = 100;

class Tree 
{
public:

    explicit Tree();
    virtual ~Tree();

    std::string getName() const;
    void setName(const string &s);

    const unsigned getNumberOfNodes() const;
    const unsigned getNumberOfLeaves() const;
    const unsigned getHeight() const;
    const bool IDnumbersAreSane(Node& n) const;

    Node * getRootNode() const;
    Node* getNode(unsigned nodeNumber) const;
    Node* findLeaf(const std::string& name) const;
    Node* findNode(const std::string& name) const;

    Node* addNode(Node *leftChild, Node *rightChild, unsigned id, const string &name = "");
    Node* addNode(Node *leftChild,  Node *rightChild, const string &name = "");
    void setRootNode(Node *r);

    /* annoying methods I want to get rid of */
    const bool hasTimes() const;
    const bool hasRates() const;
    const bool hasLengths() const;
    const double getTime(const Node& v) const;
    const double getEdgeTime(const Node& v) const;
    const double getLength(const Node& v) const;
    const double getRate(const Node& v) const;
    void setTimeNoAssert(const Node& v, double time) const;
    void setTime(const Node& v, double time) const;
    void setEdgeTime(const Node& v, double time) const;
    void setLength(const Node& v, double weight)const;
    void setRate(const Node& v, double rate)const;
    const RealVector& getTimes() const;
    const RealVector& getRates() const;
    const RealVector& getLengths() const;
    void setTimes(RealVector& v);
    void setRates(RealVector& v);
    void setLengths(RealVector& v);
    const bool checkTimeSanity(Node& root) const;
    const double getTopTime() const;
    void setTopTime(double newTime);
    const double rootToLeafTime() const;
    const double getTopToLeafTime() const;
    /* annoying methods I want to get rid of */

    Node* mostRecentCommonAncestor(Node* a, Node* b) const;

    const double imbalance() const;
    const double imbalance(Node *v) const;
    const unsigned getHeight(Node* v) const;

    void clear();
    void clearTree();
    void clearNodeAttributes();

protected:

    unsigned noOfNodes;
    unsigned noOfLeaves;
    Node *rootNode;
    std::map<std::string, Node*> name2node;
    std::vector<Node*> all_nodes;
    std::string name;

    mutable RealVector* times;
    mutable RealVector* lengths;
    mutable RealVector* rates;
    mutable double topTime;
};

#endif
