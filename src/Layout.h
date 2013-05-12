/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
    Author : Jose Fernandez Navarro  -  jc.fernandez.navarro@gmail.com
*/


/*
 * Layout.hh
 *
 *  Created on: Sep 14, 2012
 *      Author: marco
 */

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "libraries/gammamapex.h"
#include "libraries/SetOfNodesEx.hh"
#include "libraries/StrStrMap.hh"

class NodeWithIndex 
{
  public:
    Node* node;
    int layoutIndex;
    NodeWithIndex(){};
    virtual ~NodeWithIndex(){};
};

class Layout 
{
  public:
    Layout(TreeExtended *S, TreeExtended *G);
    void solveMCNRT(Node *, Node &,const GammaMapEx<Node> &, std::map <int, int> &);
    void exploreNode(Node *, Node *, NodeWithIndex**, NodeWithIndex**, NodeWithIndex**, int &, int , Node *, SetOfNodesEx<Node>*);
    int evaluateDirect(NodeWithIndex **, NodeWithIndex **, NodeWithIndex **, int, bool , Node*, SetOfNodesEx<Node>*, std::map <int, int> &);
    NodeWithIndex **rotateLayout(NodeWithIndex **, int, int);
    NodeWithIndex **reverseLayout(NodeWithIndex **, int);
    int findOrderIndex(NodeWithIndex **, Node*, int);
    Node* getRightMostLeftSon(Node*, SetOfNodesEx<Node>);
    int retrieveNodePos(SetOfNodesEx<Node>, Node*);
    int computeCrossing(int, int, NodeWithIndex **, int);
    void rotateBinary(Node*, NodeWithIndex **, SetOfNodesEx<Node> *, Node*, int, std::map <int, int> &);
    int getLayoutIndex(Node*, NodeWithIndex**, int);
    virtual ~Layout();
    void determineSwap(NodeWithIndex**, int);
    void run(std::map< int, int >& nodeMoveMap, const GammaMapEx<Node> &gamma);
    
  private:
    unsigned maxUns;
    Tree* copyTree;
    bool print;
    bool minimize;
    bool readOnly;
    int optCount;
    int regCount;
    TreeExtended *species;
    TreeExtended *gene;
    StrStrMap gs;
    SetOfNodesEx<Node> fatherSet, leftChildSet, rightChildSet;
};


#endif /* LAYOUT_H_ */