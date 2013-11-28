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
#include <cmath>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <sstream>

#include "../utils/AnError.h"
#include "Node.h"
#include "Tree.h"
#include "../reconcilation/BeepVector.h"

using namespace std;

Node::Node(unsigned id)
    : number(id),        // unique identifier
      parent(NULL),      // neighbors in the tree
      leftChild(NULL),
      rightChild(NULL),
      porder(0),         // partial order of nodes on tree
      time(0.0),         // time of incoming edge
      nodeTime(0.0),     // time interval between node and leaves
      branchLength(0.0), // branchLength
      name(),         // user-specified name (usually only for leaves)
      ownerTree(0),
      color(),
      size(0.0),
      x(0.0),
      y(0.0),
      hostParent(NULL),
      hostChild(NULL),
      reconcilation(Undefined),
      visited(0)
{

}

Node::Node(unsigned id, const string& nodeName)
    : number(id),
      parent(NULL),
      leftChild(NULL),
      rightChild(NULL),
      porder(0),
      time(0),
      nodeTime(0.0),
      branchLength(0.0),
      name(nodeName),
      ownerTree(0),
      color(),
      size(0.0),
      x(0.0),
      y(0.0),
      hostParent(NULL),
      hostChild(NULL),
      reconcilation(Undefined),
      visited(0)
{

}

// Copy relatives in tree are not copied!
Node::Node(const Node &v)
    : number(v.number),
      parent(NULL),          // relatives in tree are not copied!
      leftChild(NULL),
      rightChild(NULL),
      porder(v.porder),
      time(v.time),
      nodeTime(v.nodeTime),
      branchLength(v.branchLength),
      name(v.name),
      ownerTree(v.ownerTree),
      color(v.color),
      size(v.size),          // relatives in tree are not copied!
      x(v.x),
      y(v.y),
      hostParent(v.hostParent),
      hostChild(v.hostChild),
      reconcilation(v.reconcilation),
      visited(0)
{

}


Node::~Node()
{

}

// Assignment: Also pointers are copied, which might not be the 
// proper thing to do.
Node & 
Node::operator=(const Node &v)
{
    if (this != &v)
    {
        number = v.number;
        parent = v.parent;
        leftChild = v.leftChild;
        rightChild = v.rightChild;
        porder = v.porder;
        time = v.time;
        nodeTime = v.nodeTime;
        branchLength = v.branchLength;
        name = v.name;
        ownerTree = v.ownerTree;
        color = v.color;
        size = v.size;
        x = v.size;
        y = v.size;
        hostParent = v.hostParent;
        hostChild = v.hostChild;
        reconcilation = v.reconcilation;
        visited = v.visited;
    }
    return *this;
}



// Return the requested relative
Node* 
Node::getLeftChild() const
{
    return leftChild;
}

Node* 
Node::getRightChild() const
{ 
    return rightChild;
}

Node* 
Node::getParent() const
{
    return parent;
}

Node*
Node::getSibling() const
{
    Node *parent = getParent();
    Node *left = parent->getLeftChild();
    if (this == left)
    {
        return parent->getRightChild();
    }
    else
    {
        return left;
    }
}

// Retrieve the child of current node (x) that has y as a descendant. 
// y may be a child of x, but could also be a grandchild.
Node*
Node::getDominatingChild(Node* y)
{
    assert(y != NULL);

    if (this == y)
    {
        return this;
    }
    while (y != leftChild && y != rightChild && !y->isRoot())
    {
        y = y->parent;
    }
    return y;
}

// Rotate the node, i.e., switch left and right child.
void
Node::rotate()
{
    Node *tmp;
    tmp = leftChild;
    leftChild = rightChild;
    rightChild = tmp;
}


// get the (leaf) name
const string&
Node::getName() const
{
    return name;
}

// Set the owner tree
Tree*
Node::getTree()
{
    if(ownerTree != 0)
    {
        return ownerTree;
    }
    else
    {
        return NULL;
    }
}

// Get the node's number (post-order)
const unsigned
Node::getNumber() const
{
    return number;
}

const unsigned
Node::getPorder() const
{
    return porder;
}

const unsigned
Node::getNumberOfLeaves() const
{
    if (isLeaf())
    {
        return 1;
    }
    else
    {
        unsigned left = leftChild->getNumberOfLeaves();
        unsigned right = rightChild->getNumberOfLeaves();
        return left + right;
    }
}

const double Node::getBranchLength() const
{
    return branchLength;
}

const unsigned
Node::getMaxPathToLeaf() const
{
    if(isLeaf())
    {
        return 0;
    }
    else
    {
        unsigned left = leftChild->getMaxPathToLeaf();
        unsigned right = rightChild->getMaxPathToLeaf();
        return 1 + ((left > right)? left:right);
    }
}

SetOfNodesEx<Node>
Node::getLeaves()
{
    SetOfNodesEx<Node> nodes;
    if( isLeaf() )
    {
        nodes.insert(this);
    }
    else
    {
        //Find leaves recursively
        nodes = leftChild->getLeaves();
        SetOfNodesEx<Node> r = rightChild->getLeaves();
        for(unsigned i = 0; i < r.size(); i++)
        {
            nodes.insert(r[i]);
        }
    }
    return nodes;
}


//Set the (leaf) name
void 
Node::setName(const string& nodeName)
{
    name = nodeName;
}

// Set the owner tree
void 
Node::setTree(Tree& TreeExtended)
{
    ownerTree = &TreeExtended;
}

// setChildren
// Hook on subtrees to a node, and let the subtrees know
// who the parent is. Also make sure that the partial order is OK by 
// ensuring that porder is greater than for its children.
void
Node::setChildren(Node *l, Node *r)
{
    this->leftChild = l;
    this->rightChild = r;
    if (l)
    {
        l->parent = this;
        if (l->porder >= porder)
        {
            porder = l->porder + 1;
        }
    }
    if (r)
    {
        r->parent = this;
        if (r->porder >= porder)
        {
            porder = r->porder + 1;
        }
    }
    return;
}


// Sets the parent of the node
void
Node::setParent(Node *v)
{
    parent = v;
}

// Change ID of this, used, e.g., in HybridTree, to ascertain condition 
// ID < Tree.getNumberOfNodes(), when deleting hybrid or extinction nodes
void 
Node::changeID(unsigned newID)
{
    assert(newID < getTree()->getNumberOfNodes());
    number = newID;
}


// Delete all nodes lower in the tree. The current node is not deleted.
void
Node::deleteSubtree()
{
    if(isLeaf() == false)
    {
        leftChild -> deleteSubtree();
        delete leftChild;
        leftChild = NULL;
        rightChild -> deleteSubtree();
        delete rightChild;
        rightChild = NULL;
    }
}


// Checks if the current node is a leaf - A leaf lacks children
const bool Node::isLeaf() const
{
    if (getLeftChild() == NULL && getRightChild() == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Checks if the current node is the root. Only the root has no parent
const bool
Node::isRoot() const
{
    if (getParent() == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

const bool
Node::operator<=(const Node& b) const
{
    for(const Node* c = this; c != &b; c = c->getParent())
    {
        if(c->isRoot())
        {
            return false;
        }
    }
    return true;
}

const bool
Node::operator<(const Node& b) const
{
    if(this == &b)
    {
        return false;
    }
    else
    {
        return operator<=(b);
    }
}

const bool
Node::operator<(const Node* b) const
{
    assert(b!= 0);
    return operator<(*b);
}

const bool
Node::operator>(const Node& b) const
{
    if(this == &b)
    {
        return false;
    }
    else
    {
        return b <= *this;;
    }
}

const bool
Node::dominates(const Node& v) const
{
    for(const Node* w = &v; w != this; w = w->getParent())
    {
        if(w->isRoot())
        {
            return false;
        }
    }
    return true;
}

const bool
Node::strictlyDominates(const Node& v) const
{
    if(this == &v)
    {
        return false;
    }
    else
    {
        return dominates(v);
    }
}

const double
Node::getTime() const
{
    if(this->isRoot())
    {
        return ownerTree->getTopTime();
    }
    else if(ownerTree->hasTimes())
    {
        return ownerTree->getTime(*getParent()) - ownerTree->getTime(*this);
    }
    else
    {
        return 0;
    }
}

void
Node::setTime(const double &t)
{

    if(getParent())
    {
        throw AnError("Currently we disallow using setTime for non-root nodes",1);
    }
    else
    {
        ownerTree->setTopTime(t);
    }
}

// Set the branch length associated with the node
// This does not affect time, nodeTime.
void
Node::setLength(const double &newLength)
{
    assert(getTree()->hasLengths()); // assert lengths is modeled

    if(ownerTree->hasLengths())
    {
        ownerTree->setLength(*this, newLength);
    }
    else
    {
        throw AnError("Node::setLength:\n"
                      "ownerTree->lengths is NULL",1);
    }
}

const double
Node::getNodeTime() const
{
    if(ownerTree->hasTimes())
    {
        return ownerTree->getTime(*this);
    }
    else
    {
        return 0;
    }
}

const double
Node::getLength() const
{
    if(ownerTree->hasLengths())
    {
        return ownerTree->getLengths()[this->getNumber()];
    }
    else
    {
        return 0;
    }
}

std::ostream& 
operator<< (std::ostream& o, const Node &v)
{
    ostringstream oss;
    oss << "ID=" << v.getNumber();
    oss << v.stringify("NAME", v.getName())
        << v.stringify("NT", v.getNodeTime())
        << v.stringify("ET", v.getTime())
        << v.stringify("BL", v.getLength());
    oss << v.stringify("left", v.getLeftChild())
        << v.stringify("right", v.getRightChild())
        << v.stringify("parent", v.getParent());
    oss << endl;
    return o << oss.str();
}


std::string
Node::stringify(string tag, double val) const
{
    ostringstream oss;
    oss << "\t" << tag << "=" << val;
    return oss.str();
}

std::string
Node::stringify(string tag, string s) const
{
    ostringstream oss;
    oss << "\t" << tag << "=";
    if (s.empty())
    {
        oss << "no";
    }
    else
    {
        oss << "'" << s << "'";
    }
    return oss.str();
}

std::string
Node::stringify(string tag, Node *v) const
{
    ostringstream oss;
    oss << "\t" << tag;
    if (v==NULL)
    {
        oss << "=no";
    }
    else
    {
        oss << "=" << v->getNumber();
    }
    return oss.str();
}

std::ostream& 
operator<< (std::ostream& o, const Node *v)
{
    return operator<<(o, *v);
}

/*** EXTRA FEATURES ***/

void Node::setColor(Color c)
{
    this->color = c;
}

void Node::setSize(double s)
{
    this->size = s;
}

void Node::setX(double x)
{
    this->x = x;
}

void Node::setY(double y)
{
    this->y = y;
}

void Node::setHostChild(Node* hostchild)
{
    this->hostChild = hostchild;
}

void Node::setHostParent(Node* hostparent)
{
    this->hostParent = hostparent;
}

void Node::setReconcilation(Type t)
{
    this->reconcilation = t;
}

void Node::setVisited(unsigned inc)
{
    this->visited = inc;
}

void Node::incVisited()
{
    this->visited++;
}

Color Node::getColor()
{
    return color;
}

const double Node::getSize() const
{
    return size;
}

const double Node::getX() const
{
    return x;
}

const double Node::getY() const
{
    return y;
}

Node* Node::getHostParent()
{
    return hostParent;
}

Node* Node::getHostChild()
{
    return hostChild;
}

Node::Type Node::getReconcilation() const
{
    return reconcilation;
}

Node* Node::getHostParent() const
{
    return hostParent;
}

Node* Node::getHostChild() const
{
    return hostChild;
}

Node::Type Node::getReconcilation()
{
    return reconcilation;
}

const unsigned Node::getVisited() const
{
    return visited;
}

void Node::setRightChild(Node *r )
{
    rightChild = r;
}

void Node::setLeftChild(Node *l )
{
    leftChild = l;
}
