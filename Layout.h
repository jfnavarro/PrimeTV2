/*
 * Layout.hh
 *
 *  Created on: Sep 14, 2012
 *      Author: marco
 */

#ifndef LAYOUT_H_
#define LAYOUT_H_

//#include "NodeWithIndex.hh"
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