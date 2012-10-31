/*
 * Layout.hh
 *
 *  Created on: Sep 14, 2012
 *      Author: marco
 */

#ifndef LAYOUT_HH_
#define LAYOUT_HH_

#include "libraries/gammamapex.h"
#include "libraries/SetOfNodesEx.hh"
#include "libraries/StrStrMap.hh"

class Layout {

  public:
    
	class NodeWithIndex {
	  public:
	    Node* node;
	    int layoutIndex;
	    NodeWithIndex(){};
	    virtual ~NodeWithIndex(){};
	};  
  
	Layout(Node*, Node &, GammaMapEx<Node> &);
	void solveMCNRT(Node *, Node &, GammaMapEx<Node> &);
	void exploreNode(Node *, NodeWithIndex**, NodeWithIndex**, NodeWithIndex**, int &, int , Node *);
	int evaluateDirect(NodeWithIndex **, NodeWithIndex **, NodeWithIndex **, int, bool , Node*);
	NodeWithIndex **rotateLayout(NodeWithIndex **, int, int);
	NodeWithIndex **reverseLayout(NodeWithIndex **, int);
	int findOrderIndex(NodeWithIndex **, Node*, int);
	Node* getRightMostLeftSon(Node*, SetOfNodesEx<Node>);
	int retrieveNodePos(SetOfNodesEx<Node>, Node*);
	int computeCrossing(int, int, NodeWithIndex **, int);
	void rotateBinary(Node*, NodeWithIndex **, SetOfNodesEx<Node> *, Node*);
	virtual ~Layout();

private:
	bool print;
	bool minimize;
	bool readOnly;
	int optCount;
	int regCount;
	StrStrMap gs;
	SetOfNodesEx<Node> fatherSet, leftChildSet, rightChildSet;
};


#endif /* LAYOUT_HH_ */
