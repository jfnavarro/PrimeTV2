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
typedef float Real;

const unsigned DEF_NODE_VEC_SIZE = 100;

class Tree 
{
public:

    Tree();
    Tree(const Tree &T);
    static Tree EmptyTree(const Real& RootTime = 1.0, std::string leafname = "Leaf");

    virtual ~Tree();
    virtual Tree& operator=(const Tree& T);
    std::string getName() const;
    void setName(std::string s);
    unsigned getNumberOfNodes() const;
    unsigned getNumberOfLeaves() const;
    unsigned getHeight() const;
    bool IDnumbersAreSane(Node& n);
    void clear();
    Node * getRootNode() const;
    void setRootNode(Node *r);
    Node* getNode(unsigned nodeNumber);
    Node* getNode(unsigned nodeNumber) const;
    Node* findLeaf(const std::string& name) const;
    Node* findNode(const std::string& name) const;
    Node* addNode(Node *leftChild, Node *rightChild, unsigned id, std::string name = "");
    Node* addNode(Node *leftChild,  Node *rightChild, std::string name = "");

    /* annoying methods I want to get rid of */
    bool hasTimes() const;
    bool hasRates() const;
    bool hasLengths() const;
    Real getTime(const Node& v) const;
    Real getEdgeTime(const Node& v) const;
    Real getLength(const Node& v) const;
    Real getRate(const Node& v) const;
    void setTimeNoAssert(const Node& v, Real time) const;
    void setTime(const Node& v, Real time) const;
    void setEdgeTime(const Node& v, Real time) const;
    void setLength(const Node& v, Real weight)const;
    void setRate(const Node& v, Real rate)const;
    RealVector& getTimes() const;
    RealVector& getRates() const;
    RealVector& getLengths() const;
    void setTimes(RealVector& v) const;
    void setRates(RealVector& v) const;
    void setLengths(RealVector& v) const;
    bool checkTimeSanity(Node& root) const;
    const Real& getTopTime() const;
    void setTopTime(Real newTime);
    Real rootToLeafTime() const;
    Real getTopToLeafTime() const;


    typedef std::vector<Node*>::iterator iterator;
    typedef std::vector<Node*>::const_iterator const_iterator;
    typedef std::vector<Node*>::reverse_iterator reverse_iterator;
    typedef std::vector<Node*>::const_reverse_iterator const_reverse_iterator;

    iterator begin()
    {
        return all_nodes.begin();
    };
    const_iterator begin() const
    {
        return all_nodes.begin();
    };
    iterator end()
    {
        return (all_nodes.begin()+noOfNodes);
    };
    const_iterator end() const
    {
        return (all_nodes.begin()+noOfNodes);
    };
    reverse_iterator rbegin()
    {
        return (all_nodes.rbegin()+(all_nodes.size()-noOfNodes));
    };
    const_reverse_iterator rbegin() const
    {
        return (all_nodes.rbegin()+(all_nodes.size()-noOfNodes));
    };
    reverse_iterator rend()
    {
        return all_nodes.rend();
    };
    const_reverse_iterator rend() const
    {
        return all_nodes.rend();
    };

    Node* mostRecentCommonAncestor(Node* a, Node* b) const;
    const Node* mostRecentCommonAncestor(const Node* a, const Node* b) const;
    Real imbalance();

    //friend std::ostream& operator<<(std::ostream &o, const Tree& T);
    //virtual std::string print(bool useET, bool useNT, bool useBL, bool useER) const;
    //virtual std::string print() const;

    Node* copyAllNodes(const Node* v);
    Node* copySubtree(const Node *v);
    void clearTree();
    void clearNodeAttributes();
    Real imbalance(Node *v);
    unsigned getHeight(Node* v) const;

protected:

    unsigned noOfNodes;
    unsigned noOfLeaves;
    Node * rootNode;
    std::map<std::string, Node*> name2node;
    std::vector<Node*> all_nodes;
    std::string name;

    /* puag, mutable, really? */
    mutable RealVector* times;
    mutable RealVector* lengths;
    mutable RealVector* rates;
    mutable Real topTime;

};

#endif
