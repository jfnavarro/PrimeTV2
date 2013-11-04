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

#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../utils/AnError.hh"
#include "../reconcilation/BeepVector.hh"
#include "Tree.hh"
#include "Node.hh"
#include "TreeIO.hh"

using namespace std;

Tree::Tree() :
    noOfNodes(0),
    noOfLeaves(0),
    rootNode(NULL),
    name2node(),
    all_nodes(DEF_NODE_VEC_SIZE, NULL),
    name("Tree"),
    times(0),
    lengths(0),
    rates(0),
    topTime(0)
{
}

Tree::~Tree()
{
    if(rootNode)
    {
        rootNode->deleteSubtree();
        delete rootNode;
        rootNode = 0;
    }
    //NOTE what should I do with the times,lenghts and rates?
    //clearNodeAttributes();
}

Tree::Tree(const Tree &T)
    : noOfNodes(T.noOfNodes),
      noOfLeaves(T.noOfLeaves),
      rootNode(NULL),
      name2node(),                                       // Initialization
      all_nodes(max(noOfNodes,DEF_NODE_VEC_SIZE), NULL), // Allocate vector
      name(T.name),
      //NOTE what should I do with the times,lenghts and rates?
      //times(T.times),
      //lengths(T.lengths),
      //rates(T.rates),
      topTime(T.topTime)
{
    if(T.getRootNode())
    {
        setRootNode(copyAllNodes(T.getRootNode()));
    }
}

Tree
Tree::EmptyTree(const Real& rootTime, string leafname)
{
    Tree T;
    string name = leafname;
    T.setRootNode(T.addNode(0, 0, 0, name));
    T.topTime = rootTime;
    T.setName("Tree");
    return T;
}

Tree&
Tree::operator=(const Tree& T)
{
    if(this != &T)
    {
        clear();
        noOfNodes = T.noOfNodes;
        noOfLeaves = T.noOfLeaves;
        if(noOfNodes > all_nodes.size())
        {
            all_nodes.resize(noOfNodes, NULL);
        }
        name = T.name;
        if(T.getRootNode())
        {
            setRootNode(copyAllNodes(T.getRootNode()));
        }
        //NOTE what should I do with the times,lenghts and rates?
        //times = T.times;
        //lengths = T.lengths;
        //rates = T.rates;
    }
    return *this;
}


string
Tree::getName() const
{
    return name;
}

// Sets the name of the tree
void
Tree::setName(string s)
{
    name = s;
}

// Total number of nodes in tree
//----------------------------------------------------------------------
unsigned 
Tree::getNumberOfNodes() const
{
    return noOfNodes;
}

// Total number of leaves in tree
//----------------------------------------------------------------------
unsigned 
Tree::getNumberOfLeaves() const
{
    return noOfLeaves;
}

// Height. Empty tree has height 0, and the root-only tree has height 1.
//----------------------------------------------------------------------
unsigned
Tree::getHeight() const
{
    return getHeight(rootNode);
}

// Check that all nodes has sane identity numbers
// This is used when reading user-defined trees
//----------------------------------------------------------------------
bool 
Tree::IDnumbersAreSane(Node& n)
{
    bool ret = n.getNumber() < getNumberOfNodes();
    if(n.isLeaf() == false)
    {
        ret = ret && IDnumbersAreSane(*n.getLeftChild()) &&
                IDnumbersAreSane(*n.getRightChild());
    }
    return ret;
}

// delete and remove all nodes from tree
//----------------------------------------------------------------------
void
Tree::clear()
{
    clearTree();
    clearNodeAttributes();
}

// Access rootNode
//----------------------------------------------------------------------
Node* 
Tree::getRootNode() const
{
    return rootNode;
} 

// set rootNode
//----------------------------------------------------------------------
void
Tree::setRootNode(Node *v)
{
    assert(v!=NULL);
    assert(v->getNumber()<all_nodes.size());
    rootNode = v;
}

// Access Node from number
//----------------------------------------------------------------------
Node* 
Tree::getNode(unsigned no)
{
    if (no >= all_nodes.size())
    {
        return NULL;
    }
    else
    {
        return all_nodes[no];
    }
}

Node* 
Tree::getNode(unsigned no) const
{
    if (no >= all_nodes.size())
    {
        return NULL;
    }
    else
    {
        return all_nodes[no];
    }
}

// Accessing nodes from a name
//----------------------------------------------------------------------
Node*
Tree::findNode(const string& name) const
{
    if(name2node.count(name) > 0)
    {
        map<string, Node*>::const_iterator iter = name2node.find(name);
        return iter->second;
    }
    else
    {
        return NULL;
    }
}
// Accessing leaves from a name
//----------------------------------------------------------------------
Node*
Tree::findLeaf(const string& name) const
{
    Node* ret = findNode(name);
    if(ret->isLeaf())
    {
        return ret;
    }
    else
    {
        throw AnError("Found interior node when looking for a leaf name ",
                      name, 1);
    }
}

// The main tool for constructing the tree
//-----------------------------------------------------------------
Node *
Tree::addNode(Node *leftChild, 
              Node *rightChild,
              unsigned node_id,
              string name)
{
    assert(leftChild==NULL || leftChild->getNumber()<all_nodes.size());
    assert(rightChild==NULL || rightChild->getNumber()<all_nodes.size());
    noOfNodes++;
    if (leftChild == NULL && rightChild == NULL)
    {
        noOfLeaves++;
    }

    Node *v = new Node(node_id, name);
    v->setTree(*this);
    v->setChildren(leftChild, rightChild);
    while(all_nodes.size() <= node_id)
    {
        all_nodes.resize(2 * all_nodes.size(), NULL);
    }
    if(all_nodes[node_id] != NULL)
    {
        std::ostringstream id_str;
        id_str << node_id;
        throw AnError("There seems to be two nodes with the same id!",
                      id_str.str(), 1);
    }
    all_nodes[node_id] = v;
    name2node.insert(pair<string, Node*>(name, v));
    return v;
}

void
Tree::clearNodeAttributes()
{
    if(times)
    {
        delete times;
        times = 0;
    }
    if(rates)  // rates is owned by EdgeRateModel
    {
        delete rates;
        rates = 0;
    }
    if(lengths)
    {
        delete lengths;
        lengths = 0;
    }
    topTime = 0;
}

//The main tool for constructing the tree, overloading for trees
//without any branch times
//----------------------------------------------------------------------
Node *
Tree::addNode(Node *leftChild, 
              Node *rightChild,
              string name)
{
    return addNode(leftChild, rightChild, getNumberOfNodes(), name);
}

// MRCA gets most recent common ancestor of two speciesNodes
//----------------------------------------------------------------------
Node* 
Tree::mostRecentCommonAncestor(Node* a, Node* b) const
{
    assert(a != NULL);
    assert(b != NULL);

    while (a != b)
    {
        if (b->dominates(*a))
        {
            a = a->getParent();
        }
        else
        {
            b = b->getParent();
        }
    }
    return a;
}

// MRCA gets most recent common ancestor of two speciesNodes
//----------------------------------------------------------------------
const Node* 
Tree::mostRecentCommonAncestor(const Node* a, const Node* b) const
{
    assert(a != NULL);
    assert(b != NULL);

    while (a != b)
    {
        if (b->dominates(*a))
        {
            a = a->getParent();
        }
        else
        {
            b = b->getParent();
        }
    }
    return a;
}


// Check if times/rates/lengths are available
//----------------------------------------------------------------------
bool 
Tree::hasTimes() const
{
    return times != 0;
}

bool 
Tree::hasRates() const
{
    return rates != 0;
}

bool 
Tree::hasLengths() const
{
    return lengths != 0;
}

// Gets the node time of node v
//----------------------------------------------------------------------
Real
Tree::getTime(const Node& v) const
{
    return (*times)[v];
}

// Gets the node time of node v
//----------------------------------------------------------------------
Real
Tree::getEdgeTime(const Node& v) const
{
    if(v.isRoot())
    {
        return topTime;
    }
    else
    {
        return (*times)[v.getParent()] - (*times)[v];
    }
}

Real
Tree::rootToLeafTime() const
{
    Node *v = getRootNode();
    if (!v)   //assert?
    {
        throw AnError("rootToLeafTime: No root node! Not good...", 1);
    }
    return v->getNodeTime();

}

Real
Tree::getTopToLeafTime() const
{
    return (getTime(*getRootNode()) + topTime);
}

// Sanity check of time
bool 
Tree::checkTimeSanity(Node& root) const
{
    Node& left = *root.getLeftChild();
    Node& right = *root.getRightChild();

    if(getTime(left) > getTime(right) || getTime(right) > getTime(left))
    {
        return false;
    }
    else
    {
        return checkTimeSanity(left) && checkTimeSanity(right);
    }
}

// access and manipulate TopTime
//----------------------------------------------------------------------
const Real& 
Tree::getTopTime() const
{
    return topTime;
}

void 
Tree::setTopTime(Real newTime)
{
    topTime = newTime;
}

// Gets the weight of node v
//----------------------------------------------------------------------
Real
Tree::getLength(const Node& v) const
{
    return (*lengths)[v];
}

// Gets the rate of node v
//----------------------------------------------------------------------
Real
Tree::getRate(const Node& v) const
{
    if(rates->size() == 1)
    {
        return (*rates)[0u];
    }
    else
    {
        return (*rates)[v];
    }
}

// Sets the divergence time of node v
void
Tree::setTimeNoAssert(const Node& v, Real time) const
{
    (*times)[v] = time;
}

// Sets the divergence time of node v
//----------------------------------------------------------------------
void
Tree::setTime(const Node& v, Real time) const
{
    (*times)[v] = time;
    assert(v.isLeaf() || (*times)[v] >= (*times)[v.getLeftChild()]);
    assert(v.isLeaf() || (*times)[v] >= (*times)[v.getRightChild()]);
    assert(v.isRoot() || (*times)[v.getParent()] >= (*times)[v]);
}

// Sets the edge time of node v
//----------------------------------------------------------------------
void
Tree::setEdgeTime(const Node& v, Real time) const
{
    if(v.isRoot())
    {
        topTime = time;
    }
    else
    {
        (*times)[v] = (*times)[v.getParent()] - time;
        assert((*times)[v] > (*times)[v.getLeftChild()]);
        assert((*times)[v] > (*times)[v.getRightChild()]);
    }
}

// Sets the weight of node v
//----------------------------------------------------------------------
void
Tree::setLength(const Node& v, Real weight) const
{
    if(weight < 2 * std::numeric_limits< double >::min())
    {
        weight = 2 * std::numeric_limits< double >::min();
    }
    if(v.isRoot() == false && v.getParent()->isRoot())
    {
        Node& s = *v.getSibling();
        weight = (weight + (*lengths)[s])/2;
        (*lengths)[s] = weight;
    }
    (*lengths)[v] = weight;
}

// Sets the rate of node v
//----------------------------------------------------------------------
void
Tree::setRate(const Node& v, Real rate) const
{
    if(rates->size() == 1)
    {
        (*rates)[0u] = rate;
    }
    else
    {
        (*rates)[v] = rate;
    }
}

// Handle to time, lengths and rates
RealVector& 
Tree::getTimes() const
{
    return *times;
}

RealVector& 
Tree::getRates() const
{
    return *rates;
}

RealVector& 
Tree::getLengths() const
{
    return *lengths;
}

void
Tree::setTimes(RealVector& v) const
{
    if(times)
    {
        delete times;
        times = 0;
    }
    times = &v;
}

void
Tree::setRates(RealVector& v) const
{
    if(rates)
    {
        delete rates;
        rates = 0;
    }
    rates = &v;
}

void
Tree::setLengths(RealVector& v) const
{
    if(lengths)
    {
        delete lengths;
        lengths = 0;
    }
    lengths = &v;
}

Real 
Tree::imbalance()
{
    Node *r = getRootNode();
    assert (r != NULL);
    return imbalance(r);
}

Node *
Tree::copyAllNodes(const Node *v)
{
    assert(v != NULL);
    Node *u = new Node(*v);
    u->setTree(*this);

    assert(u->getNumber()<all_nodes.size());
    all_nodes[u->getNumber()] = u;
    if (v->isLeaf())
    {
        name2node[u->getName()] = u;
        return u;
    }
    else
    {
        Node *l = copyAllNodes(v->getLeftChild());
        Node *r = copyAllNodes(v->getRightChild());
        u->setChildren(l, r);	// Notice that setChildren changes (or corrects) porder here!
        return u;
    }
    return NULL;
}
// Recursively copy all nodes in a tree. And keep track of names etc while
// you are at it! The new nodes get new IDs
// Notice! Assumes that the new nodes will belong to the calling Tree!!
//----------------------------------------------------------------------
Node *
Tree::copySubtree(const Node *v)
{
    assert(v != NULL);
    string name = v->getName();
    if(name != "")
    {
        while(name2node.find(name) != name2node.end())
        {
            name = name + "a";
        }
    }
    Node *u = addNode(NULL, NULL, name);
    u->setTree(*this);

    assert(u->getNumber()<all_nodes.size());
    all_nodes[u->getNumber()] = u;

    if (v->isLeaf())
    {
        name2node[u->getName()] = u;
        return u;
    }
    else
    {
        Node *l = copySubtree(v->getLeftChild());
        Node *r = copySubtree(v->getRightChild());
        u->setChildren(l, r);	// Notice that setChildren changes (or corrects) porder here!
        return u;
    }
}

// delete and remove all nodes from tree
//----------------------------------------------------------------------
void
Tree::clearTree()
{
    if(rootNode != NULL)
    {
        rootNode->deleteSubtree();
        delete rootNode;
        rootNode = NULL;
    }
    noOfNodes = noOfLeaves = 0;
    name2node.clear();
    all_nodes.clear();
    all_nodes = std::vector<Node*>(DEF_NODE_VEC_SIZE, NULL);
}

Real
Tree::imbalance(Node *v)
{
#define MAX(A, B) ((A>B) ? A : B)
    if (v->isLeaf())
    {
        return 0;
    }
    else
    {
        Node *l = v->getLeftChild();
        Node *r = v->getRightChild();
        Real my_imbalance = fabs(l->getNodeTime() + l->getTime() -
                                 r->getNodeTime()-r->getTime());
        Real l_imbalance = imbalance(l);
        Real r_imbalance = imbalance(r);
        return MAX(my_imbalance, MAX(l_imbalance, r_imbalance));
    }
}

unsigned 
Tree::getHeight(Node* v) const
{
    if (v == NULL)
        return 0;
    else
        return 1 + max(getHeight(v->getLeftChild()),
                       getHeight(v->getRightChild()));
}
