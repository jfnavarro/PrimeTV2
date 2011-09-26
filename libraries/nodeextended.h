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
// #ifndef NODEEXTENDED_H
// #define NODEEXTENDED_H
// #include "libraries/Node.hh"
// #include "Color.h"
// #include <boost/concept_check.hpp>
// 
// using namespace beep;
//  
// 
// class NodeExtended : public Node
// {
//    class TreeExtended;
//   
// public:
//    
//     enum  Type{Leaf=0, Speciation=1, Duplication=2, LateralTransfer=3, Specie=4};
//     
//     NodeExtended(unsigned id);
//     NodeExtended(unsigned id, const std::string& nodeName);   
//     NodeExtended(const NodeExtended &);
//     NodeExtended(const Node &);
//     NodeExtended(NodeExtended *n);
//     NodeExtended(Node *n);
//     
//     NodeExtended* operator=(const NodeExtended& n);
//     NodeExtended* operator=(const Node& n);
//     NodeExtended* operator=(const NodeExtended* n);
//     NodeExtended* operator=(const Node* n);
//     
//     void setColor(Color c);
//     void setSize(double s);
//     void setX(double x);
//     void setY(double y);
//     void setHostParent( NodeExtended *hostparent);
//     void setHostChild( NodeExtended *hostchild);
//     void setReconcilation(Type t);
//     void setChildren(NodeExtended *left, NodeExtended *right); 
//     void setParent(NodeExtended *parent);
//     
//     
//     Color getColor();
//     double getSize();
//     double getX();
//     double getY();
//     NodeExtended *getHostParent();
//     NodeExtended *getHostChild();
//     Type getReconcilation();
//     NodeExtended* getLeftChild() const;
//     NodeExtended* getRightChild() const;
//     NodeExtended* getParent() const;
//     NodeExtended* getSibling() const;
// 
//     virtual bool isLeaf() const;
//     virtual bool isRoot() const;
//     
//     TreeExtended* getTree();
//     void setTree(TreeExtended &tree); 
//   
//     template<class T> const T& max ( const T& a, const T& b ) {
//       return (b<a)?a:b;     
//     }
// 
// 
// private:
// 
//     NodeExtended *parent;          
//     NodeExtended *leftChild;      
//     NodeExtended *rightChild;     
//     
//     TreeExtended *ownerTree;      
//   
//     Color color;
//     double size;
//     double x;
//     double y;
//     NodeExtended *hostParent;
//     NodeExtended *hostChild;
//     Type reconcilation;
//   
// };
// 
// #endif // NODEEXTENDED_H