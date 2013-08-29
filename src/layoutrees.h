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
*/

/* this class calculates all the information needed to draw the trees, it calculates
 * all the node cordinates and assign them to the nodes, it also calculates the most important
 * information needed to draw, it uses the tree size and the dimensions of the picture to calculate
 * all the cordinates and it does it dinamically according to the dimensions, it also increments the size
 * of the picture if the size is too small to fit the tree*/

#ifndef LAYOUTSPECIESTREE_H
#define LAYOUTSPECIESTREE_H

#include "libraries/treeextended.h"
#include "libraries/Node.hh"
#include "libraries/gammamapex.h"
#include "parameters.h"

using namespace std;

class LayoutTrees
{  

public:

    /*constructor : gene tree, species tree and parameters are going to be modified */
    LayoutTrees(TreeExtended *r,TreeExtended *g,
		 Parameters *p,const GammaMapEx<Node> *gm,
		 const LambdaMapEx<Node> *l);
    
    void start();
    
    /* destructor*/
    ~LayoutTrees();
    
    double getNodeHeight();
    
     /* this functions takes a map of node->node and replaces their cordinates in
     * the gene tree */
    void replaceNodes(const std::map<int,int> &replacements); 
    
private:
   
    /* this function ladderize the tree to the right, it swapes the nodes */
    int Ladderize_right(); 
    int Ladderize_right(Node *n);
    
    /* the same to the left */
    int Ladderize_left(); 
    int Ladderize_left(Node *n);
    
    /* this function calculates the Species node cordinates using three different time approaches */
    void CountSpeciesCoordinates(Node *n, int depth);
    
    /* this function calculates the gene cordinates according to the species node cordinates calculated
     * previously */
    void CountGeneCoordinates(Node *n);
    
    /* this function assigns a leaf gene or species node its cordinates, the positions will depend of the number
     * of nodes mapped to the species nodes where the node is being positioned on */
    void AssignLeafGene(Node *n);
    
    /* this function assigns a duplication gene its cordinates, depending on the number of duplication
     * nodes that are being located in the species edge */
    void AssignGeneDuplication(Node *n);
    
    /* this function assigns a LGT gene its information */
    void AssignGeneLGT(Node *n);
    
    /* this function returns the number of nodes of the species node that has the most nodes
     * mapped */
    int  MostGenes();
    
    /* this function maps all the times of the tree to distribute them equally in groups to 
     * be used to calculte the x cordinates of the species nodes */
    unsigned maptimes();
    
    /* this function returns the left most y cordinate of the node given */
    double getLeftMostCoordinate (Node* o);
    
    /* the same but the right most cordiante */
    double getRightMostCoordinate (Node* o);
    
    /* this function maps all the duplications of the tree in the vector Adress */
    Node* FindDuplications(Node* node); 
    
    /* this function maps the number of duplication mapped to each species node*/
    Node* MapDuplications(Node* de, unsigned line);
    
    /* this function returns the level in the tree of the duplication node given */
    unsigned Duplevel(Node* nd, int levellineage);
    
    /* this function returns the right most cordiante node for a duplication
     * node given as input */
    double RightMostCoordinate(Node* o, Node *end_of_slice, int duplevel);
    
    /* the same for the left*/
    double LeftMostCoordinate(Node* o, Node *end_of_slice, int duplevel);
    
    /* this function displaces the species node if it is colliding with another node*/
    void CalcLegIntersection(Node *left, Node *right, Node *u);
    
    /* this function calculates the time intervals and the drawing parameters */
    void calculateIntervals();
    
    /* this function calculates the sizes of the nodes and increments the dimension
     * of the picture is needed */
    void calculateSizes();
    
    /* this function returns the highest mapped LGT node of the node given */
    Node* getHighestMappedLGT(Node *n);
    
    /* this function returns the size of the biggest name label on both trees */
    double biggestLabel();
    
    //external attributes
    TreeExtended *species;
    TreeExtended *gene;
    Parameters *parameters;
    const GammaMapEx<Node> *gamma;
    const LambdaMapEx<Node> *lambda;
    BeepVector<unsigned> bv;
    BeepVector<Node*> Adress;
    
    double NodeHeight;
    double yspace;
    double xspace;
    double currentY;
    double YCanvasSize; 
    double XCanvasSize;
    double xCanvasXtra;
    int maxdeepleaf;
    int maxdeepleaftimes;
    unsigned leveltime;
    std::vector<double> numXPositions; //equally distributed no time
    std::map<double,double> numXPositionsTimes; //distributed equally by time
    std::vector<double> maptime;

    bool nodetime;
    bool equal;
    

};

#endif // LAYOUTSPECIESTREE_H
