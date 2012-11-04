/*
 * Layout.cc
 *
 *  Created on: Jul 14, 2012
 *      Author: marco
 */
#include <iostream>
#include <vector>
#include <exception>
using namespace std;
#include "Layout.h"
#include <math.h>

#include "libraries/Node.hh"
#include "libraries/treeextended.h"


Layout::Layout(TreeExtended *S, TreeExtended *G)
{
  maxUns = std::numeric_limits<unsigned int>::max();
  print = false;
  minimize = true;
  readOnly = true;
  species = S;
  gene = G;
}

void Layout::run(std::map< int, int >& nodeMoveMap, const GammaMapEx<Node> &gamma)
{
  copyTree = new Tree(*gene);
  if(copyTree)
    solveMCNRT(species->getRootNode(), *gene->getRootNode(), gamma, nodeMoveMap);
  else
    std::cerr << "There was an error clonning the gene tree in the class Layout.." << std::endl;
}

void Layout::solveMCNRT(Node *fatherNode, Node &geneRoot,const GammaMapEx<Node> &gamma, std::map<int,int> &nodeMoveMap) 
{
  Node *leftChild, *rightChild;
  //keep going down layer after layer
  if ( (fatherNode->getRightChild() == NULL)	|| (fatherNode->getLeftChild() == NULL)) 
  {
    return;
  }
  
  rightChild = fatherNode->getRightChild();
  leftChild = fatherNode->getLeftChild();

  solveMCNRT(leftChild, geneRoot, gamma, nodeMoveMap);
  solveMCNRT(rightChild, geneRoot, gamma, nodeMoveMap);
  //when the leaves layer is hit, return
  //now start solving the problem from the last but one layer
  //get the genes nodes embedded in species nodes: father, leftChild and rightChild
  fatherSet = gamma.getGamma(fatherNode);
  leftChildSet = gamma.getGamma(leftChild);
  rightChildSet = gamma.getGamma(rightChild);
  //number of nodes in the tau and sigma layout
  int layoutSize = leftChildSet.size() + rightChildSet.size();

  NodeWithIndex **sigma;
  NodeWithIndex **tau;
  NodeWithIndex **reversedTau;
  NodeWithIndex **rotatedTau;
  NodeWithIndex **revRotTau;

  sigma = new NodeWithIndex *[layoutSize];
  tau = new NodeWithIndex *[layoutSize];
  reversedTau = new NodeWithIndex *[layoutSize];
  //pointers to the index where to insert next node in tau and sigma
  int s = 0;
  unsigned int i;
  //scan all the genes nodes embedded inside the species father
  Node *control = NULL;
  SetOfNodesEx<Node> *nodeset = new SetOfNodesEx<Node>;
  //originalNode points to the current node in the original tree
  //actualNode points to the current node in the copied tree (used for perform internal rotations)
  for (i = 0; i < fatherSet.size(); i++) 
  {
    Node *originalNode = fatherSet.operator [](i);
    Node *actualNode = copyTree->getNode(fatherSet.operator [](i)->getNumber());
    actualNode->setParent(NULL);
    exploreNode(actualNode, originalNode, sigma, tau, reversedTau, s, layoutSize, control, nodeset);
  }
  //	get the leftmost internal node, intNode = y
  
  /*
   * bool leaf = false;
  //NOTE easier to just check the number of leaves?
  if ((leftChild->getRightChild() == NULL) && (leftChild->getLeftChild() == NULL)
    && (rightChild->getLeftChild() == NULL) && (rightChild->getRightChild() == NULL))
      leaf = true;
  */
  
  int direct = evaluateDirect(sigma, tau, reversedTau, layoutSize, readOnly,fatherNode, nodeset, nodeMoveMap);
  rotatedTau = rotateLayout(tau, layoutSize, leftChildSet.size());
  revRotTau = reverseLayout(rotatedTau, layoutSize);
  int rotated = evaluateDirect(sigma, rotatedTau, revRotTau, layoutSize,readOnly, fatherNode, nodeset, nodeMoveMap);
  regCount += direct;
  readOnly = false;
  if ((direct >= rotated)) 
  {
    fatherNode->rotate();
    direct = evaluateDirect(sigma, rotatedTau, revRotTau, layoutSize, readOnly, fatherNode, nodeset, nodeMoveMap);
    optCount += direct;
  } 
  else 
  {
    rotated = evaluateDirect(sigma, tau, reversedTau, layoutSize, readOnly,fatherNode, nodeset, nodeMoveMap);
    optCount += rotated;
  }
  //cleaning up
  delete[] sigma;
  delete[] tau;
  delete[] reversedTau;
  delete[] revRotTau;
  delete[] rotatedTau;
}

NodeWithIndex** Layout::reverseLayout(NodeWithIndex **delta, int layoutSize) 
{
  NodeWithIndex **reversedDelta = new NodeWithIndex *[layoutSize];
  for (int i = 0; i < layoutSize; i++) 
  {
    reversedDelta[i] = new NodeWithIndex;
    reversedDelta[i] = delta[layoutSize - 1 - i];
  }
  return reversedDelta;
}



NodeWithIndex** Layout::rotateLayout(NodeWithIndex **delta, int layoutSize,int leftSize) 
{
  NodeWithIndex **rotatedDelta = new NodeWithIndex *[layoutSize];
  for (int i = 0; i < layoutSize - leftSize; i++) 
  {
    rotatedDelta[i] = new NodeWithIndex;
    rotatedDelta[i]->node	 = delta[leftSize + i]->node;
    rotatedDelta[i]->layoutIndex = delta[leftSize + i]->layoutIndex;
  }
  for (int i = 0; i < leftSize; i++) 
  {
    rotatedDelta[layoutSize - leftSize + i] = new NodeWithIndex;
    rotatedDelta[layoutSize - leftSize + i]->node = delta[i]->node;
    rotatedDelta[layoutSize - leftSize + i]->layoutIndex = delta[i]->layoutIndex;
  }
  return rotatedDelta;
}

//return the layout index of rightmostleftson
int Layout::findOrderIndex(NodeWithIndex **sigma, Node* rightMostLeftSon,int layoutSize) 
{
  for (int i = 0; i < layoutSize; i++) 
  {
    if (sigma[i]->node == rightMostLeftSon)
      return i;
  }
  return -1;
}

//find the node regarded as z in OT-GTL
//namely the rightmost node son of the left child of the internal node y
Node* Layout::getRightMostLeftSon(Node* internalNode, SetOfNodesEx<Node> levelGenesNodes) 
{
  if(internalNode==NULL)
    return NULL;
  if (internalNode->getLeftChild() == NULL) 
  {
    return NULL;
  }
  Node *nextSon = internalNode->getLeftChild();
  if (nextSon == NULL) 
  {
    return NULL;
  }
  if (levelGenesNodes.member(nextSon)) 
  {
    // Left son already belongs to the species level";
    return NULL;
  }
  while (!levelGenesNodes.member(nextSon)) 
  {
    if (nextSon->getRightChild() == NULL) 
    {
      return NULL;
    }
    else
      nextSon = nextSon->getRightChild();
    
    if (nextSon == NULL) 
    {
      return NULL;
    }
  }
  return nextSon;
}

int Layout::getLayoutIndex(Node* searchedNode, NodeWithIndex** layout, int layoutSize)
{
  int i;
  for( i = 0; i < layoutSize ; i++)
  {
    if(layout[i]->node==searchedNode)
    {
      break;
    }
  }
  return i;
}



void Layout::rotateBinary(Node* intNode, NodeWithIndex **sigma, SetOfNodesEx<Node> *nodeset,
			   Node* speciesNode, int layoutSize, std::map<int, int> &nodeMoveMap) 
{

  if (intNode->getLeftChild() == NULL || intNode->getLeftChild() == NULL) 
  {
    cerr << "\n\nswapping create NULL pointers\n\n";
    return;
  }

  //get the index of the four delimiters of the layout section to be rotated
  //if we rotate node x and y we need to get their leftmost and right most son in the layout
  Node* leftMostNode = intNode->getLeftChild();
  Node* rightMostNode = intNode->getRightChild();
  Node* leftNodeSibling = intNode->getLeftChild();
  Node* rightNodeSibling = intNode->getRightChild();

  while (!nodeset->member(leftMostNode)) 
  {
    if (leftMostNode->getLeftChild() != NULL)
      leftMostNode = leftMostNode->getLeftChild();
    else 
    {
      return;
    }
  }

  while (!nodeset->member(leftNodeSibling)) 
  {
    if (leftNodeSibling->getRightChild() != NULL)
      leftNodeSibling = leftNodeSibling->getRightChild();
    else 
    {
      return;
    }
  }

  while (!nodeset->member(rightMostNode)) 
  {
    if (rightMostNode->getRightChild() != NULL)
      rightMostNode = rightMostNode->getRightChild();
    else 
    {
      return;
    }
  }

  while (!nodeset->member(rightNodeSibling)) 
  {
    if (rightNodeSibling->getLeftChild() != NULL)
      rightNodeSibling = rightNodeSibling->getLeftChild();
    else 
    {
      return;
    }
  }
  
  int sxSxIndex = getLayoutIndex(leftMostNode, sigma, layoutSize);
  int sxDxIndex = getLayoutIndex(leftNodeSibling, sigma, layoutSize);
  int dxDxIndex = getLayoutIndex(rightMostNode, sigma, layoutSize);
  int dxSxIndex = sxDxIndex + 1;
  int sxLen = sxDxIndex - sxSxIndex;
  int dxLen = dxDxIndex - dxSxIndex;
  int length = min(sxLen, dxLen);

  if(length<=0) return;

  for (int i = 0; i <= min(sxLen, dxLen); i++) 
  {
    nodeMoveMap.operator [](sigma[sxSxIndex + i]->node->getNumber()) = sigma[dxSxIndex + i]->node->getNumber();
    if(print)
      cout << "\nSWAP: " << sigma[sxSxIndex + i]->node->getNumber() << "  AND  " <<  sigma[dxSxIndex + i]->node->getNumber() << std::endl;
    Node* sxPointer = sigma[sxSxIndex + i]->node;

    sigma[sxSxIndex + i]->node = sigma[dxSxIndex + i]->node;
    sigma[dxSxIndex + i]->node = sxPointer;

  }

  for (int i = length; i < max(sxLen, dxLen); i++) 
  {
    nodeMoveMap.operator [](sigma[sxSxIndex + i]->node->getNumber()) = sigma[dxDxIndex - i]->node->getNumber();
    if(print)
      cout << "\nSWAP: " << sigma[sxSxIndex + i]->node->getNumber() << "  AND  " <<  sigma[dxDxIndex - i]->node->getNumber() << std::endl;
    Node* sxPointer = sigma[sxSxIndex + i]->node;
    sigma[sxSxIndex + i]->node = sigma[dxDxIndex - i]->node;
    sigma[dxDxIndex - i]->node = sxPointer;
  }

  if(intNode->getLeftChild()!=NULL and intNode->getRightChild() != NULL)
  {
    swap(*intNode->getLeftChild(),*intNode->getRightChild());
  }

}

//return the number of nodes smaller than z after x
//that equal the number of crossings
int Layout::computeCrossing(int x, int z, NodeWithIndex **delta, int layoutSize) 
{
  //find node x in delta layout
  int i, j;
  int sum = 0;
  for (i = 0; i < layoutSize; i++) 
  {
    if (delta[i]->layoutIndex == x) 
    {
      break;
    }
  }

  //node smaller than z positioned after x in "delta" layout
  for (j = i; j < layoutSize; j++) 
  {
    if (delta[j]->layoutIndex <= z) {
      sum++;
    }
  }
  return sum;
}

int Layout::evaluateDirect(NodeWithIndex **sigma, NodeWithIndex **tau,
		NodeWithIndex **reversedTau, int layoutSize, bool readOnly,
		Node* speciesNode, SetOfNodesEx<Node> *nodeset, std::map<int,int> &nodeMoveMap) 
{
  int z;
  //number of crossing with regular layout and with inverted ones
  int sum, reversedSum;
  //keep track of the total number of crossing the actual layout implies
  int totalSum = 0;
  //keep track of the maximum layout index of the node not yet worked with
  int maxToCompute = layoutSize;
  vector<Node*> newVector;
  vector<Node*> deleteVector;

  for (int i = 0; i < layoutSize; i++) 
  {
    if(sigma[i]!=NULL)
      newVector.push_back(sigma[i]->node);
   }

  int tmpSize = layoutSize;
  Node *tempNode;
  //bool binary = true;

  for (int l = 0; tmpSize >= 1; l++) 
  {
    for (int i = 0; i < tmpSize - 1; i = i + 2) 
    {
	Node *node1 = newVector.at(i);
	Node *node2 = newVector.at(i + 1);
	//no bin rel set for both the nodes
	if((node1->getParent()==NULL)&&(node2->getParent()==NULL))
	{
	  tempNode = new Node(maxUns - node1->getNumber());
	  node1->setParent(tempNode);
	  tempNode->setLeftChild(node1);
	  node2->setParent(tempNode);
	  tempNode->setRightChild(node2);
	  deleteVector.push_back(tempNode);
	}
	//the two nodes are set as sibligns
	else if((node1->getParent()!=NULL) && (node2->getParent()==node1->getParent()))
	{
	  tempNode = node1->getParent();
	  deleteVector.push_back(tempNode);
	}
	else
	{
	  //just the first has it set: update it and start from the second in the next iteration
	  if(node1->getParent()!=NULL)
	    deleteVector.push_back(node1->getParent());
	  else
	    deleteVector.push_back(node1);
	  i-=1;
	}
     }
      
    if (tmpSize % 2 != 0) 
    {
      deleteVector.push_back(newVector.at(tmpSize - 1));
    }
    if (tmpSize == 1)
      break;
    else
    {
     //NOTE watch out here
     tmpSize = deleteVector.size();
     newVector = deleteVector;
     deleteVector.clear();
    }
  }

  Node *intNode = tempNode;
  Node* lastNode;
  if (layoutSize == 2)
    lastNode = sigma[0]->node;
  else
    lastNode = sigma[0]->node->getParent();

  while ((intNode!=NULL)&&(intNode != lastNode)) 
  {
    Node *rightMostLeftSon = getRightMostLeftSon(intNode, *nodeset);
    //if NULL intNode should not be considered, and we should move one layer up
    if (rightMostLeftSon == NULL) 
    {
      if (intNode->getLeftChild() != NULL) 
      {
	intNode = intNode->getLeftChild();
      } 
      else 
      {
	intNode = NULL;
      }
      
      if (layoutSize == 2) 
      {
	rightMostLeftSon = sigma[0]->node;
	intNode = rightMostLeftSon->getParent();
      }
      else
	continue;
    }

    z = findOrderIndex(sigma, rightMostLeftSon, layoutSize);
    z = getLayoutIndex(rightMostLeftSon, sigma, layoutSize);
    sum = 0, reversedSum = 0;

    for (int i = z + 1; i < maxToCompute; i++) 
    {
      sum = sum + computeCrossing(i, z, tau, layoutSize);
      reversedSum = reversedSum + computeCrossing(i, z, reversedTau, layoutSize);
    }

    if (sum > reversedSum) 
    {
      totalSum += reversedSum;
      if (readOnly == false) 
      {
	  rotateBinary(intNode, sigma, nodeset, speciesNode, layoutSize, nodeMoveMap);
      }
    } 
    else 
    {
      totalSum += sum;
    }
      
    if (intNode!=NULL)
    {
      if(intNode->getLeftChild() != NULL) 
      {
	intNode = intNode->getLeftChild();
      } 
      else
      {
	intNode = NULL;
      }
    } 
    maxToCompute = z + 1;
    
   }

   return totalSum;
}

//build sigma, tau and reversed tau for current layer
void Layout::exploreNode(Node *actualNode, Node *originalNode, NodeWithIndex **sigma, NodeWithIndex **tau,
		NodeWithIndex **reversedTau, int &s, int layoutSize, Node *control, SetOfNodesEx<Node> *nodeSet) 
{
  //if the left gene son belongs to one of the species level descense is over.
  //add the actualNode to the sigma layout and the left child to the tau one

  Node *geneLeft = originalNode->getLeftChild();
  Node *geneRight = originalNode->getRightChild();
  Node *geneLeftCopy = actualNode->getLeftChild();
  Node *geneRightCopy = actualNode->getRightChild();

  if (geneLeft != NULL && geneLeftCopy!=NULL) 
  {
    exploreNode(geneLeftCopy, geneLeft, sigma, tau, reversedTau, s, layoutSize, actualNode, nodeSet);
  }

  if (geneRight != NULL && geneRightCopy!=NULL) 
  {
    exploreNode(geneRightCopy, geneRight, sigma, tau, reversedTau, s, layoutSize, actualNode, nodeSet);
  }

  //actual node belongs to one of the species child (straight line)
  if (leftChildSet.member(originalNode)) 
  {
    int index = retrieveNodePos(leftChildSet, originalNode);
    if (index == -1) 
    {
	cerr << "error: Gene node not found\n";
	return;
    } 
    else 
    {
	tau[index] = new NodeWithIndex;
	reversedTau[layoutSize - index - 1] = new NodeWithIndex;
	sigma[s] = new NodeWithIndex;
	nodeSet->insert(actualNode);
	if(control == NULL)
	{
	  sigma[s]->node = actualNode;
	  tau[index]->node = actualNode;
	  reversedTau[layoutSize - index - 1]->node = actualNode;
	}
	else
	{
	  sigma[s]->node = control;
	  tau[index]->node = control;
	  reversedTau[layoutSize - index - 1]->node = control;
	  actualNode->setParent(control);			
	  if(control->getLeftChild()==NULL)
	  {
	    control->setLeftChild(actualNode);
	  }
	  else
	  {
	    control->setRightChild(actualNode);
	  }
	}
	sigma[s]->layoutIndex = s;
	tau[index]->layoutIndex = s;
	reversedTau[layoutSize - index - 1]->layoutIndex = s;
	s++;
	return;
    }
  }
  else if ((rightChildSet.member(originalNode))) 
  {
    int index = retrieveNodePos(rightChildSet, originalNode);		
    int leftSize = leftChildSet.size();
    int summedIndex = index + leftSize;
    if (index == -1) 
    {
	return;
    } 
    else 
    {
	sigma[s] = new NodeWithIndex;
	tau[summedIndex] = new NodeWithIndex;
	reversedTau[layoutSize - summedIndex - 1] = new NodeWithIndex;
	nodeSet->insert(actualNode);

	if(control == NULL)
	{
	  sigma[s]->node = actualNode;
	  tau[summedIndex]->node = actualNode;
	  reversedTau[layoutSize - summedIndex - 1]->node = actualNode;
	}
	else
	{
	  sigma[s]->node = control;
	  tau[summedIndex]->node = control;
	  reversedTau[layoutSize - summedIndex - 1]->node = control;
	  actualNode->setParent(control);

	  if(control->getLeftChild()==NULL)
	  {
	    control->setLeftChild(actualNode);
	  }
	  else
	  {
	    control->setRightChild(actualNode);
	  }
	}
	sigma[s]->layoutIndex = s;
	tau[summedIndex]->layoutIndex = s;
	reversedTau[layoutSize - summedIndex - 1]->layoutIndex = s;
	s++;
	return;
    }
  }
  else
  {
    actualNode = NULL;
  }

  return;
}


int Layout::retrieveNodePos(SetOfNodesEx<Node> nodeSet, Node* node) 
{
  unsigned int i;	
  for (i = 0; i < nodeSet.size(); i++) 
  {
    if (nodeSet.operator [](i) == node) return i;
  }
  return -1;
}

Layout::~Layout() 
{
  if(copyTree)
  {
    delete copyTree;
  }
  copyTree = 0;
}
