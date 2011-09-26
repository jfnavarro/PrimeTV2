// /*
//     <one line to give the library's name and an idea of what it does.>
//     Copyright (C) <year>  <name of author>
// 
//     This library is free software; you can redistribute it and/or
//     modify it under the terms of the GNU Lesser General Public
//     License as published by the Free Software Foundation; either
//     version 2.1 of the License, or (at your option) any later version.
// 
//     This library is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//     Lesser General Public License for more details.
// 
//     You should have received a copy of the GNU Lesser General Public
//     License along with this library; if not, write to the Free Software
//     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//     Author : Jose Fernandez Navarro  -  jc.fernandez.navarro@gmail.com
// */
// 
// #include "nodeextended.h"
// #include "libraries/PRNG.hh"
// 
// using namespace beep;
// 
// NodeExtended::NodeExtended(unsigned int id)
//   : Node(id),
//     color(),
//     size(0.0),
//     x(0.0),
//     y(0.0),
//     hostParent(NULL),
//     hostChild(NULL),
//     reconcilation(Undefined)
// {
// 
// }
// 
// NodeExtended::NodeExtended(const NodeExtended& n)
//   : Node(n),    
//     color(n.color),
//     size(n.size),          // relatives in tree are not copied!
//     x(n.x),
//     y(n.y),
//     hostParent(n.hostParent),
//     hostChild(n.hostChild),
//     reconcilation(n.reconcilation)
// {
// 
// }
// 
// NodeExtended::NodeExtended(unsigned int id, const std::string& nodeName)
//   : Node(id, nodeName),
//     color(),
//     size(0.0),
//     x(0.0),
//     y(0.0),
//     hostParent(NULL),
//     hostChild(NULL),
//     reconcilation(Undefined)
// {
// 
// }
// 
// NodeExtended::NodeExtended(const beep::Node& n) 
//   : Node(n),
//     color(),
//     size(0.0),
//     x(0.0),
//     y(0.0),
//     hostParent(NULL),
//     hostChild(NULL),
//     reconcilation(Undefined)
// {
// 
// }
// 
// NodeExtended::NodeExtended(NodeExtended* n)
//   : Node(*n),
//     color(n->color),
//     size(n->size),          // relatives in tree are not copied!
//     x(n->x),
//     y(n->y),
//     hostParent(n->hostParent),
//     hostChild(n->hostChild),
//     reconcilation(n->reconcilation) 
// {
// 
// }
// 
// 
// NodeExtended::NodeExtended(Node* n)
//   : Node(*n),
//     color(),
//     size(0.0),
//     x(0.0),
//     y(0.0),
//     hostParent(NULL),
//     hostChild(NULL),
//     reconcilation(Undefined)
// {
//   
// 
// }
// 
// 
// NodeExtended* NodeExtended::operator=(const NodeExtended& n)
// {
//   Node::operator=(n);
//   color = n.color;
//   size = n.size;
//   x = n.size;
//   y = n.size;
//   hostParent = n.hostParent;
//   hostChild = n.hostChild;
//   reconcilation = n.reconcilation;
//   return this;
// }
// 
// NodeExtended* NodeExtended::operator=(const beep::Node& n)
// {
//   Node::operator=(n);
//   return this;
// 
// }
// 
// NodeExtended* NodeExtended::operator=(const NodeExtended* n)
// {
//   Node::operator=(*n);
//   color = n->color;
//   size = n->size;
//   x = n->size;
//   y = n->size;
//   hostParent = n->hostParent;
//   hostChild = n->hostChild;
//   reconcilation = n->reconcilation;
//   
//   return this;
// 
// }
// 
// NodeExtended* NodeExtended::operator=(const beep::Node* n)
// {
//   Node::operator=(*n);
//   return this;
// 
// }
// 
// void NodeExtended::setColor(Color c)
// {
//   this->color = c;
// }
// 
// void NodeExtended::setSize(double s)
// {
//   this->size = s;
// }
// 
// void NodeExtended::setX(double x)
// {
//   this->x = x;
// }
// 
// void NodeExtended::setY(double y)
// {
//   this->y = y;
// }
// 
// void NodeExtended::setHostChild(NodeExtended* hostchild)
// {
//   this->hostChild = hostchild;
// }
// 
// void NodeExtended::setHostParent(NodeExtended* hostparent)
// {
//   this->hostParent = hostparent;
// }
// 
// void NodeExtended::setReconcilation(NodeExtended::Type t)
// {
//   this->reconcilation = t;
// }
// 
// Color NodeExtended::getColor()
// {
//   return color;
// }
// 
// double NodeExtended::getSize()
// {
//   return size;
// }
// 
// double NodeExtended::getX()
// {
//   return x;
// }
// 
// double NodeExtended::getY()
// {
//   return y;
// }
// 
// NodeExtended* NodeExtended::getHostParent()
// {
//   return hostParent;
// }
// 
// NodeExtended* NodeExtended::getHostChild()
// {
//   return hostChild;
// }
// 
// NodeExtended::Type NodeExtended::getReconcilation()
// {
//   return reconcilation;
// }
// 
// 
// void NodeExtended::setChildren(NodeExtended* left, NodeExtended* right)
// {
//   this->leftChild = left;
//   this->rightChild = right;
//   if(left)
//     left->parent = this;
//   if(right)
//     right->parent = this;
// }
// 
// void NodeExtended::setParent(NodeExtended* parent)
// {
//   this->parent = parent;
// }
// 
// NodeExtended* NodeExtended::getLeftChild() const
// {
//   return leftChild;
// }
// 
// NodeExtended* NodeExtended::getRightChild() const
// {
//   return rightChild;
// }
// 
// NodeExtended* NodeExtended::getParent() const
// {
//   return parent;
// }
// 
// 
// bool NodeExtended::isLeaf() const
// {
//     if (getLeftChild() == NULL && getRightChild() == NULL) 
//       {
// 	return true;
//       }
//     else
//       {
// 	return false;
//       }
// }
// 
// bool NodeExtended::isRoot() const
// {
//     if (getParent() == NULL)
//       {
// 	return true;
//       }
//     else 
//       {
// 	return false;
//       }
// }
// 
// NodeExtended::TreeExtended* NodeExtended::getTree()
// {
//   return ownerTree;
// }
// 
// void NodeExtended::setTree(TreeExtended& tree)
// {
//   this->ownerTree = &tree;
// }
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
