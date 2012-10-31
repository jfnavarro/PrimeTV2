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


Layout::Layout(Node *fatherNode, Node &geneRoot, GammaMapEx<Node> &gamma){
	print = false;
	minimize = true;
	readOnly = true;
	regCount = 0;
	optCount = 0;
	solveMCNRT(fatherNode, geneRoot, gamma);
	if(regCount<=optCount){
		cout << "regular " << regCount << std::endl;
		cout << "optimized " << optCount << std::endl;
	}
	else{
		cout << "optimized " << optCount << std::endl;
		cout << "regular " << regCount << std::endl;
	}
}

void Layout::solveMCNRT(Node *fatherNode, Node &geneRoot, GammaMapEx<Node> &gamma) {


		Node *leftChild, *rightChild;
		//keep going down layer after layer
		if ((fatherNode->getRightChild() == NULL) or (fatherNode->getLeftChild() == NULL)) {
			return;
		}
		bool leaf = false;
		rightChild = fatherNode->getRightChild();
		leftChild = fatherNode->getLeftChild();

		solveMCNRT(leftChild, geneRoot, gamma);
		solveMCNRT(rightChild, geneRoot, gamma);
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
		//pointers to the index where to insert a node in tau and sigma
		int s = 0;
		unsigned int i;
		//scan all the genes nodes embedded inside the species father
		Node *control = NULL;
		for (i = 0; i < fatherSet.size(); i++) {
			Node *actualNode = fatherSet.operator [](i);
			exploreNode(actualNode, sigma, tau, reversedTau, s, layoutSize, control);
		}

		//	get the leftmost internal node, intNode = y
		if ((leftChild->getRightChild() == NULL)
				and (leftChild->getLeftChild() == NULL)
				and (rightChild->getLeftChild() == NULL)
				and (rightChild->getRightChild() == NULL))
			leaf = true;

		int direct = evaluateDirect(sigma, tau, reversedTau, layoutSize, readOnly,fatherNode);
		rotatedTau = rotateLayout(tau, layoutSize, leftChildSet.size());
		revRotTau = reverseLayout(rotatedTau, layoutSize);
		int rotated = evaluateDirect(sigma, rotatedTau, revRotTau, layoutSize,readOnly, fatherNode);
		
		regCount += direct;
		print = false;
		readOnly = false;
		
		if ((direct >= rotated)) {
			fatherNode->rotate();
			direct = evaluateDirect(sigma, rotatedTau, revRotTau, layoutSize, readOnly, fatherNode);
			optCount += direct;

		} else {

			rotated = evaluateDirect(sigma, tau, reversedTau, layoutSize, readOnly,fatherNode);
			optCount += rotated;
		}


		//cleaning up
		delete[] sigma;
		delete[] tau;
		delete[] reversedTau;
		delete[] revRotTau;
		delete[] rotatedTau;
}

Layout::NodeWithIndex** Layout::reverseLayout(NodeWithIndex **delta, int layoutSize) {
	NodeWithIndex **reversedDelta = new NodeWithIndex *[layoutSize];
	int i;
	for (i = 0; i < layoutSize; i++) {
		reversedDelta[i] = new NodeWithIndex;
		reversedDelta[i] = delta[layoutSize - 1 - i];
	}

	return reversedDelta;

}



Layout::NodeWithIndex** Layout::rotateLayout(NodeWithIndex **delta, int layoutSize,
		int leftSize) {
	NodeWithIndex **rotatedDelta = new NodeWithIndex *[layoutSize];
	int i;
	for (i = 0; i < layoutSize - leftSize; i++) {
		rotatedDelta[i] = new NodeWithIndex;
		rotatedDelta[i]->node	 = delta[leftSize + i]->node;
		rotatedDelta[i]->layoutIndex = delta[leftSize + i]->layoutIndex;
	}
	for (i = 0; i < leftSize; i++) {
		rotatedDelta[layoutSize - leftSize + i] = new NodeWithIndex;
		rotatedDelta[layoutSize - leftSize + i]->node = delta[i]->node;
		rotatedDelta[layoutSize - leftSize + i]->layoutIndex = delta[i]->layoutIndex;
	}

	return rotatedDelta;

}

//return the layout index of rightmostleftson
int Layout::findOrderIndex(NodeWithIndex **sigma, Node* rightMostLeftSon,
		int layoutSize) {
	int i;
	for (i = 0; i < layoutSize; i++) {;
		if (sigma[i]->node == rightMostLeftSon)
			return i;
	}
	return -1;
}

//find the node regarded as z in OT-GTL
//namely the rightmost node son of the left child of the internal node y
Node* Layout::getRightMostLeftSon(Node* internalNode, SetOfNodesEx<Node> levelGenesNodes) {
	if (internalNode->getBinaryLeftChild() == NULL) {
		return NULL;
	}
	Node *nextSon = internalNode->getBinaryLeftChild();
	if (nextSon == NULL) {
		return NULL;
	}
	if (levelGenesNodes.member(nextSon)) {
		// Left son already belongs to the species level";
		return NULL;
	}
	while (!levelGenesNodes.member(nextSon)) {
		if (nextSon->getBinaryRightChild() == NULL) {
			return NULL;
		} else
			nextSon = nextSon->getBinaryRightChild();

		if (nextSon == NULL) {
			return NULL;
		}
	}
	return nextSon;
}



void Layout::rotateBinary(Node* intNode, NodeWithIndex **sigma, SetOfNodesEx<Node> *nodeset,
		Node* speciesNode) {


	if (intNode->getBinaryLeftChild() == NULL
			or intNode->getBinaryLeftChild() == NULL) {
		cout << "\n\nswapping create NULL pointers\n\n";
		return;
	}
	Node* leftMostNode = intNode->getBinaryLeftChild();

	Node* rightMostNode = intNode->getBinaryRightChild();

	Node* leftNodeSibling = intNode->getBinaryLeftChild();

	Node* rightNodeSibling = intNode->getBinaryRightChild();

	while (!nodeset->member(leftMostNode)) {
		if (leftMostNode->getBinaryLeftChild() != NULL)
			leftMostNode = leftMostNode->getBinaryLeftChild();
		else {
			return;
		}
	}

	while (!nodeset->member(leftNodeSibling)) {
		if (leftNodeSibling->getBinaryRightChild() != NULL)
			leftNodeSibling = leftNodeSibling->getBinaryRightChild();
		else {
			return;
		}
	}

	while (!nodeset->member(rightMostNode)) {
		if (rightMostNode->getBinaryRightChild() != NULL)
			rightMostNode = rightMostNode->getBinaryRightChild();
		else {
			return;
		}
	}

	while (!nodeset->member(rightNodeSibling)) {
		if (rightNodeSibling->getBinaryLeftChild() != NULL)
			rightNodeSibling = rightNodeSibling->getBinaryLeftChild();
		else {
			return;
		}
	}
	int sxSxIndex = leftMostNode->getLayoutIndex();
	int sxDxIndex = leftNodeSibling->getLayoutIndex();
	int dxDxIndex = rightMostNode->getLayoutIndex();
	int dxSxIndex = sxDxIndex + 1;

	int sxLen = sxDxIndex - sxSxIndex;
	int dxLen = dxDxIndex - dxSxIndex;
	int length = min(sxLen, dxLen);

	for (int i = 0; i <= length; i++) {

		sigma[sxSxIndex + i]->node->addNodeSwap(speciesNode,
				sigma[dxSxIndex + i]->node);
		sigma[dxSxIndex + i]->node->addNodeSwap(speciesNode,
				sigma[sxSxIndex + i]->node);
		Node* sxPointer = sigma[sxSxIndex + i]->node;

		sigma[sxSxIndex + i]->node = sigma[dxSxIndex + i]->node;
		sigma[dxSxIndex + i]->node = sxPointer;

	}
		for (int i = length; i < max(sxLen,dxLen); i++) {
			sigma[sxSxIndex + i]->node->addNodeSwap(speciesNode,
					sigma[dxDxIndex - i]->node);
			sigma[dxDxIndex - i]->node->addNodeSwap(speciesNode,
					sigma[sxSxIndex + i]->node);

			Node* sxPointer = sigma[sxSxIndex + i]->node;
			sigma[sxSxIndex + i]->node = sigma[dxDxIndex - i]->node;
			sigma[dxDxIndex - i]->node = sxPointer;
		}


	intNode->rotateBinaryChild();

}


//return the number of nodes smaller than z after x
//that equal the number of crossings
int Layout::computeCrossing(int x, int z, NodeWithIndex **delta, int layoutSize) {
	//find node x in delta layout
	int i, j;
	int sum = 0;
	for (i = 0; i < layoutSize; i++) {
		if (delta[i]->layoutIndex == x) {
			if (print)
				cout << "\n  x = " << x << "   ; found at index"
						<< delta[i]->layoutIndex << "    i = " << i << "\n";
			break;
		}
	}
	//node smaller than z positioned after x in "delta" layout
	for (j = i; j < layoutSize; j++) {
		if (delta[j]->layoutIndex <= z) {
			sum++;
			if (print)
				cout << "\nfound one crossing" << delta[j]->layoutIndex
						<< "is <= " << z << "\n";
		} else if (print)
			cout << "\n No crossing since " << delta[j]->layoutIndex << "is > "
					<< z << "\n";
	}
	return sum;
}

int Layout::evaluateDirect(NodeWithIndex **sigma, NodeWithIndex **tau,
		NodeWithIndex **reversedTau, int layoutSize, bool readOnly,
		Node* speciesNode) {

	int z;
	//number of crossing with regular layout and with inverted ones
	int sum, reversedSum;
	//keep track of the total number of crossing the actual layout implies
	int totalSum = 0;
	//keep track of the maximum layout index of the node not yet worked with
	int maxToCompute = layoutSize;

	SetOfNodesEx<Node> *nodeset = new SetOfNodesEx<Node>;

	vector<Node*> newVector;
	vector<Node*> deleteVector;

	for (int i = 0; i < layoutSize; i++) {
		newVector.push_back(sigma[i]->node);
		nodeset->insert(sigma[i]->node);
		nodeset->operator [](nodeset->size()-1)->setLayoutIndex(i);
	}

	int tmpSize = layoutSize;

	Node *tempNode;
	bool binary = true;
	for (int l = 0; tmpSize >= 1; l++) {
		for (int i = 0; i < tmpSize - 1; i = i + 2) {

			Node *node1 = newVector.at(i);
			Node *node2 = newVector.at(i + 1);
			//no bin rel set for both the nodes
			if((node1->getBinaryParent()==NULL)&&(node2->getBinaryParent()==NULL)){
				tempNode = new Node(++Node::maxID);
				tempNode->setBinary(binary);
				node1->setBinaryParent(tempNode);
				tempNode->setBinaryLeftChild(node1);
				node2->setBinaryParent(tempNode);
				tempNode->setBinaryRightChild(node2);
				deleteVector.push_back(tempNode);
			}
			//the two nodes are set as sibligns
			else if((node1->getBinaryParent()!=NULL) && (node2->getBinaryParent()==node1->getBinaryParent())){
					tempNode = node1->getBinaryParent();
					deleteVector.push_back(tempNode);

			}
			else{
				//just the first has it set: update it and start from the second in the next iteration
				if(node1->getBinaryParent()!=NULL)
					deleteVector.push_back(node1->getBinaryParent());
				else
					deleteVector.push_back(node1);
				i-=1;
			}
		}
		if (tmpSize % 2 != 0) {
			deleteVector.push_back(newVector.at(tmpSize - 1));
		}
		if (tmpSize == 1)
			break;
		else
			tmpSize = deleteVector.size();
		newVector = deleteVector;
		deleteVector.clear();
	}

	Node *intNode = tempNode;
	Node* lastNode;
	if (layoutSize == 2)
		lastNode = sigma[0]->node;
	else
		lastNode = sigma[0]->node->getBinaryParent();

	while ((intNode!=NULL)&&(intNode != lastNode)) {
		//		find z

		Node *rightMostLeftSon = getRightMostLeftSon(intNode, *nodeset);
		//if NULL intNode should not be considered, and we should move one layer up

		if (rightMostLeftSon == NULL) {

			if (intNode->getBinaryLeftChild() != NULL) {
				intNode = intNode->getBinaryLeftChild();
			} else {
				intNode = NULL;
			}
			if (layoutSize == 2) {
				rightMostLeftSon = sigma[0]->node;
				intNode = rightMostLeftSon->getBinaryParent();
			} else
				continue;
		}

		z = findOrderIndex(sigma, rightMostLeftSon, layoutSize);
		z = rightMostLeftSon->getLayoutIndex();

		sum = 0, reversedSum = 0;
		for (int i = z + 1; i < maxToCompute; i++) {
			sum = sum + computeCrossing(i, z, tau, layoutSize);
			reversedSum = reversedSum
					+ computeCrossing(i, z, reversedTau, layoutSize);
		}
		print = false;

		if (sum > reversedSum) {
			totalSum += reversedSum;

		} else {
			totalSum += sum;
		}
		if ((intNode!=NULL) && (intNode->getBinaryLeftChild() != NULL)) {
			intNode = intNode->getBinaryLeftChild();
		} else
			intNode = NULL;
		maxToCompute = z + 1;
	}

	return totalSum;
}



//FIXME: need to set a pointer for moving the right node when
//there's a fix from multiple connections(duplication/speciation)
//to one

//build sigma, tau and reversed tau for current layer
void Layout::exploreNode(Node *actualNode, NodeWithIndex **sigma, NodeWithIndex **tau,
		NodeWithIndex **reversedTau, int &s, int layoutSize, Node *control) {
	//if the left gene son belongs to one of the species level descense is over.
	//add the actualNode to the sigma layout and the left child to the tau one

	Node *geneLeft = actualNode->getLeftChild();
			if (geneLeft != NULL) {
				exploreNode(geneLeft, sigma, tau, reversedTau, s, layoutSize, actualNode);
			}
			Node *geneRight = actualNode->getRightChild();
			if (geneRight != NULL) {
				exploreNode(geneRight, sigma, tau, reversedTau, s, layoutSize, actualNode);
			}

	//actual node belongs to one of the species child (straight line)
	if (leftChildSet.member(actualNode)) {
		int index = retrieveNodePos(leftChildSet, actualNode);
		if (index == -1) {
			cout << "error: Gene node not found\n";
			return;
		} else {
			tau[index] = new NodeWithIndex;
			reversedTau[layoutSize - index - 1] = new NodeWithIndex;
			sigma[s] = new NodeWithIndex;

			if(control == NULL){
				sigma[s]->node = actualNode;
				tau[index]->node = actualNode;
				reversedTau[layoutSize - index - 1]->node = actualNode;
			}
			else{
				sigma[s]->node = control;
				tau[index]->node = control;
				reversedTau[layoutSize - index - 1]->node = control;
				actualNode->setBinaryParent(control);
				if(control->getBinaryLeftChild()==NULL){
					control->setBinaryLeftChild(actualNode);
				}
				else
					control->setBinaryRightChild(actualNode);
			}
			sigma[s]->layoutIndex = s;

			tau[index]->layoutIndex = s;

			reversedTau[layoutSize - index - 1]->layoutIndex = s;
			s++;
			return;
		}
	}
	if ((rightChildSet.member(actualNode))) {
		int index = retrieveNodePos(rightChildSet, actualNode);
		int leftSize = leftChildSet.size();
		int summedIndex = index + leftSize;
		if (index == -1) {
			return;
		} else {
			sigma[s] = new NodeWithIndex;
			tau[summedIndex] = new NodeWithIndex;
			reversedTau[layoutSize - summedIndex - 1] = new NodeWithIndex;

			if(control == NULL){
				sigma[s]->node = actualNode;
				tau[summedIndex]->node = actualNode;
				reversedTau[layoutSize - summedIndex - 1]->node = actualNode;
			}
			else{
				sigma[s]->node = control;
				tau[summedIndex]->node = control;
				reversedTau[layoutSize - summedIndex - 1]->node = control;
				actualNode->setBinaryParent(control);
				if(control->getBinaryLeftChild()==NULL){
					control->setBinaryLeftChild(actualNode);
				}
				else
					control->setBinaryRightChild(actualNode);
			}
			sigma[s]->layoutIndex = s;
			tau[summedIndex]->layoutIndex = s;
			reversedTau[layoutSize - summedIndex - 1]->layoutIndex = s;
			s++;
			return;
		}
	}



	return;
}


int Layout::retrieveNodePos(SetOfNodesEx<Node> nodeSet, Node* node) {
	unsigned int i;
	for (i = 0; i < nodeSet.size(); i++) {
		if (nodeSet.operator [](i) == node)
			return i;
	}
	return -1;
}

Layout::~Layout() {
	// TODO Auto-generated destructor stub
}
