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

#include "Layoutrees.h"

#include "../tree/Treeextended.h"
#include "../Parameters.h"
#include "../tree/Node.h"

static bool sort_double(const double u, const double v)
{
    return u > v;
}

LayoutTrees::LayoutTrees(TreeExtended *r,
                         TreeExtended *g,
                         Parameters *p,
                         const GammaMapEx *gm,
                         const LambdaMapEx *l)
    :species(r),
      gene(g),
      parameters(p),
      gamma(gm),
      lambda(l),
      bv(r->getNumberOfNodes()),
      Adress(g->getNumberOfNodes())
{


}

void LayoutTrees::start()
{
    if (parameters->ladd == 'r')
    {
        Ladderize_right();
        equal = parameters->equalTimes;
    }
    else if (parameters->ladd == 'l')
    {
        Ladderize_left();
        equal = false;
    }
    
    parameters->maxLeafNameSize = biggestLabel() * parameters->fontsize;

    // we calculate the separation between the tree and the margin of the canvas
    // according to the size of the canvas and the size of the biggest leaf and the font size
    // so the leaves names can fit in the canvas
    parameters->separation = (parameters->maxLeafNameSize *= 0.25); //0.25 extra offset
    parameters->root_sep = std::abs(parameters->separation / 2); //TODO this number is a bit random

    //scale by time and do it equally distributed
    nodetime = parameters->scaleByTime;
    
    calculateSizes();
    calculateIntervals();

    // add xCanvasXtra(root node space) to the separation
    parameters->separation += xCanvasXtra;

    // starting y is on the middle of the canvas
    currentY = YCanvasSize - (yspace / 2.0);
    
    CountSpeciesCoordinates(species->getRootNode(),0);
    FindDuplications(gene->getRootNode());
    MapDuplications(gene->getRootNode(),species->getRootNode()->getNumber()+1);
    CountGeneCoordinates(gene->getRootNode());
}

LayoutTrees::~LayoutTrees()
{

}

void LayoutTrees::calculateSizes()
{
    //max numbers of gene nodes mapped to a species node
    const unsigned maxnodesmapped = MostGenes();
    //total number of species leaves
    const unsigned numberLeaves = species->getNumberOfLeaves();
    //number of nodes mapped to the species root node
    const unsigned sizeRoot = gamma->getSize(species->getRootNode());

    //extra espace from the root to the margin to draw gene nodes mapped to the root
    xCanvasXtra = parameters->root_sep * (double)sizeRoot;
    
    // yspace is the space between each specie node, we compute it according to the width of the canvas
    // and the total number of leaves
    yspace = (double)(parameters->height) / (double)numberLeaves;
    
    // nodeheight is the space between species nodes where we can place gene nodes
    NodeHeight = yspace / (double)maxnodesmapped;

    const double min_node_height = (double)parameters->min_node_height * (double)maxnodesmapped;
    const double aspect_ratio = std::abs((double)parameters->height / (double)parameters->width);

    //if I will change the size I want to store the original size as well
    parameters->adapted_height = parameters->height;
    parameters->adapted_width = parameters->width;

    //check if the tree is too big to fit in the actual canvas size (min_node_height is 3 by default)
    while( NodeHeight < min_node_height )
    {
        double yrate = std::abs(min_node_height * numberLeaves);
        //increase canvas size (keeping aspect ratio)
        parameters->adapted_height += yrate;
        parameters->adapted_width += (yrate * aspect_ratio);
        //recompute yspace
        yspace = (double)(parameters->adapted_height) / (double)numberLeaves;
        //recompute NodeHeight
        NodeHeight = yspace / (double)maxnodesmapped;
    }
    
    //Y and X CanvasSize will be the real space dedicated to place the trees
    YCanvasSize = parameters->adapted_height - parameters->separation;
    XCanvasSize = parameters->adapted_width - xCanvasXtra - parameters->separation;
}

void LayoutTrees::calculateIntervals()
{
    maxdeepleaf = species->getRootNode()->getMaxPathToLeaf() + 1;
    
    if(equal)
    {
        maxdeepleaftimes = maptimes();

        for(unsigned i = 0; i < maxdeepleaftimes; ++i)
        {
            const double ratio = (double)i / (double)(maxdeepleaftimes);
            double value = ratio * XCanvasSize;
            double time = maptime[i];
            numXPositionsTimes.insert(pair<double,double>(time,value));
        }
    }
    
    for(unsigned i = 0; i < maxdeepleaf; ++i)
    {
        double ratio = (double)i / (double)(maxdeepleaf-1);
        double value = ratio * XCanvasSize;
        numXPositions.push_back(value);
    }

}

unsigned LayoutTrees::maptimes()
{
    for (unsigned i = 0; i < species->getNumberOfNodes(); ++i)
    {
        Node *n = species->getNode(i);
        double time = n->getNodeTime();
        std::vector<double>::iterator it = std::find(maptime.begin(),maptime.end(),time);
        if(it == maptime.end())
        {
            maptime.push_back(time);
        }
    }
    std::sort(maptime.begin(),maptime.end(),sort_double);
    return maptime.size();
}

double LayoutTrees::getRightMostCoordinate (Node* o) const
{
    if (o->isLeaf())
    {
        return o->getY();
    }
    
    else
    {
        return getRightMostCoordinate(o->getRightChild());
    }
}

double LayoutTrees::getLeftMostCoordinate (Node* o) const
{
    if(o->isLeaf())
    {
        return o->getY();
    }
    
    else
    {
        return getLeftMostCoordinate(o->getRightChild());
    }
}

// yspace has been calculated according to the number of leaves and the height
// so the cordinate y of each node will be increased by y on the leaves, the x
// cordinate on the leaves is always the same, the y cordinates of intern
// nodes is calculated in the midpoint between the right most y and the
// left most y, the x position is calcuted used the time mapped vectors
void LayoutTrees::CountSpeciesCoordinates(Node *n, unsigned depth)
{
    if (n->isLeaf())
    {
        n->setY(currentY);
        currentY -= yspace;
        //x is equal to the size of the drawin canvas for x plus the root node
        //drawing size but no the separation
        n->setX(XCanvasSize + xCanvasXtra);
    }
    else
    {
        Node *left = n->getLeftChild();
        Node *right = n->getRightChild();

        CountSpeciesCoordinates(left, depth + 1);
        CountSpeciesCoordinates(right, depth + 1);

        const double sumyleft = getRightMostCoordinate(left);
        const double sumyright = getLeftMostCoordinate(right);
        const double yposition = (sumyleft + sumyright) / 2;
        const double time = n->getNodeTime();
        double xposition;

        if(nodetime && !equal)
        {
            xposition = ((1-time) * XCanvasSize) + xCanvasXtra;
        }
        else if(nodetime && equal)
        {
            xposition = numXPositionsTimes[time] + xCanvasXtra;
        }
        else
        {
            xposition = numXPositions.at(depth) + xCanvasXtra;
        }
        n->setY(yposition);
        n->setX(xposition);
        CalcLegIntersection(left,right,n);
    }
    
}

unsigned LayoutTrees::MostGenes() const
{
    unsigned currentMax = 0;
    for(Node *n = species->postorder_begin(); n != 0; n = species->postorder_next(n))
    {
        unsigned size = gamma->getSize(n);
        if ( size > currentMax )
        {
            currentMax = size;
        }
    }
    return currentMax;
}

// each species node has a visited atribute, if the size (number os nodes mapped)
// of a species node is > 1 then the nodeheight will be divided by the number
// of nodes mapped and the current node located in the according position
void LayoutTrees::AssignLeafGene(Node *n)
{
    Node *spn = gamma->getLowestGammaPath(*n);
    n->setX(spn->getX());
    double y;
    const unsigned size = gamma->getSize(spn);
    
    if(size > 1)
    {
        const unsigned yoffset = spn->getVisited();
        const unsigned delta = NodeHeight / (size - 1);
        y = (spn->getY() - (NodeHeight / 2) ) + (delta * yoffset);
    }
    else
    {
        y = spn->getY();
    }
    
    spn->incVisited();
    n->setY(y);
    n->setHostChild(spn);
    
    if (!n->isRoot() && gamma->isLateralTransfer(*n->getParent())
            && ((*lambda)[n] == (*lambda)[n->getParent()]))
    {
        Node *destiny = (*lambda)[getHighestMappedLGT(n)];
        n->setHostParent(destiny);
    }
    else
    {
        n->setHostParent(spn);
    }
}

void LayoutTrees::CountGeneCoordinates(Node* n)
{
    if(n->isLeaf())
    {
        n->setReconcilation(Node::Leaf);
        AssignLeafGene(n);
    }
    else
    {
        Node *left = n->getLeftChild();
        Node *right = n->getRightChild();
        
        CountGeneCoordinates(left);
        CountGeneCoordinates(right);

        if(gamma->isSpeciation(*n) && !gamma->isLateralTransfer(*n)) //speciation
        {
            n->setReconcilation(Node::Speciation);
            AssignLeafGene(n);
        }
        else if (gamma->isLateralTransfer(*n)) //lateral transfer
        {
            AssignGeneLGT(n);
        }
        else //duplication
        {
            AssignGeneDuplication(n);
        }
    }
}

void LayoutTrees::AssignGeneDuplication(Node *n)
{
    Node *spb = Adress[n];
    double proportion = 0;
    double delta = 0;
    double edge = 0;
    if (!spb->isRoot())
    {
        Node *spbP = spb->getParent();
        edge = spb->getX() - spbP->getX();
        proportion = ((spbP->getY() - spb->getY()) / edge);
        n->setHostParent(spbP);
    }
    else
    {
        edge = spb->getX();
    }
    
    // we obtain the number of duplication and the duplications levels
    // to figure out the x position of the node, we use the left most and right
    // most cordinates of the duplication to figure out the y position
    const double ndupli = bv[spb] + 1;
    const unsigned duplilevel = Duplevel(n,spb->getNumber());
    delta = (edge/ndupli) * duplilevel;
    
    n->setX(spb->getX()-delta);
    
    const double rightMost = RightMostCoordinate(n,spb,duplilevel);
    const double leftMost = LeftMostCoordinate(n,spb,duplilevel);

    n->setY( ((rightMost + leftMost) /2) + (proportion * delta) );
    n->setReconcilation(Node::Duplication);
    n->setHostChild(spb);
}

void LayoutTrees::AssignGeneLGT(Node *n)
{
    n->setReconcilation(Node::LateralTransfer);
    
    Node *SoriginLT = (*lambda)[n];
    Node *SdestinyLT = (*lambda)[n->getLeftChild()];
    
    if(SoriginLT == SdestinyLT)
    {
        SdestinyLT = (*lambda)[n->getRightChild()];
    }
    
    n->setHostParent(SdestinyLT);
    n->setHostChild(SoriginLT);
}

Node*
LayoutTrees::FindDuplications(Node* node) 
{
    if(gamma->isSpeciation(*node) || gamma->isLateralTransfer(*node))
    {
        if (!node->isLeaf())
        {
            FindDuplications(node->getLeftChild());
            FindDuplications(node->getRightChild());
        }
        
        Node *top = gamma->getHighestGammaPath(*node);
        return top;
    }
    else
    {
        Node *top_dup_l = FindDuplications(node->getLeftChild());
        Node *top_dup_r = FindDuplications(node->getRightChild());
        Node *top_l = gamma->getHighestGammaPath(*(node->getLeftChild()));
        Node *top_r = gamma->getHighestGammaPath(*(node->getRightChild()));

        if (top_l == 0)
        {
            top_l = top_dup_l;
        }
        if (top_r == 0)
        {
            top_r = top_dup_r;
        }

        Adress[node] = species->mostRecentCommonAncestor(top_l, top_r);
        return Adress[node];
    }
}

Node*
LayoutTrees::MapDuplications(Node* de, unsigned line) 
{
    if(de->isLeaf())
    {
        return de->getParent();
    }

    else
    {
        if (!gamma->isSpeciation(*de) && !gamma->isLateralTransfer(*de))
        {
            unsigned testlineage = Adress[de]->getNumber();

            if(testlineage != line)
            {
                Node* nd = Adress[de];
                unsigned n_levels = Duplevel(de,testlineage);
                if (n_levels > bv[nd])
                {
                    bv[nd] = n_levels;
                }
            }

            line = testlineage;
        }
        
        Node* newNode = MapDuplications(de->getLeftChild(), line);
        Node* newNode2 = MapDuplications(newNode->getRightChild(), line);

        return newNode2->getParent();
    }
}

unsigned LayoutTrees::Duplevel(Node* nd, unsigned levellineage) const
{      
    if(gamma->isSpeciation(*nd) || Adress[nd]->getNumber() != unsigned(levellineage))
    {
        return 0;
    }
    else
    {
        unsigned left = Duplevel(nd->getLeftChild(), levellineage);
        unsigned right = Duplevel(nd->getRightChild(), levellineage);
        return max(left, right) + 1;
    }
}

double
LayoutTrees::LeftMostCoordinate(Node* o, Node *end_of_slice, unsigned duplevel) const
{
    if (gamma->isSpeciation(*o))
    {
        if(gamma->getLowestGammaPath(*o) != end_of_slice)
        {
            double size = gamma->getSize(end_of_slice);
            double delta = NodeHeight / (size - 1);
            double y = (end_of_slice->getY() - (NodeHeight /2) ) + (duplevel * delta);
            return y;
        }
        else
        {
            return o->getY();
        }
    }
    else
    {
        if (end_of_slice == Adress[o])
        {
            return LeftMostCoordinate(o->getLeftChild(), end_of_slice,duplevel);
        }
        else
        {
            double size = gamma->getSize(end_of_slice);
            double delta = NodeHeight / (size - 1);
            double y = (end_of_slice->getY() - (NodeHeight / 2)) + (duplevel * delta);
            return y;
        }
    }
}


double
LayoutTrees::RightMostCoordinate (Node* o, Node *end_of_slice, unsigned duplevel) const
{
    if (gamma->isSpeciation(*o))
    {
        if(gamma->getLowestGammaPath(*o) != end_of_slice)
        {
            double size = gamma->getSize(end_of_slice);
            double delta = NodeHeight / size - 1;
            double y = (end_of_slice->getY() - (NodeHeight / 2)) + (duplevel * delta);
            return y;
        }
        else
        {
            return o->getY();
        }
    }
    else
    {
        if (end_of_slice == Adress[o])
        {
            return RightMostCoordinate(o->getRightChild(), end_of_slice,duplevel);
        }
        else
        {
            double size = gamma->getSize(end_of_slice);
            double delta = NodeHeight / size - 1;
            double y = (end_of_slice->getY() - (NodeHeight / 2)) + (duplevel * delta);
            return y; ;
        }
    }
}

double LayoutTrees::getNodeHeight()
{
    return NodeHeight;
}

unsigned
LayoutTrees::Ladderize_left() const
{
    return Ladderize_left(species->getRootNode());
}

unsigned
LayoutTrees::Ladderize_left(Node* n) const
{
    if(n->isLeaf())
    {
        return 1;
    }
    else
    {
        unsigned leftsize = Ladderize_left(n->getLeftChild());
        unsigned rightsize = Ladderize_left(n->getRightChild());
        if(leftsize > rightsize)
        {
            n->rotate();
        }
        return leftsize + rightsize;
    }
}

unsigned
LayoutTrees::Ladderize_right() const
{
    return Ladderize_right(species->getRootNode());
}

unsigned
LayoutTrees::Ladderize_right(Node* n) const
{
    if(n->isLeaf())
    {
        return 1;
    }
    else
    {
        unsigned leftsize = Ladderize_right(n->getLeftChild());
        unsigned rightsize = Ladderize_right(n->getRightChild());
        if(rightsize > leftsize)
        {
            n->rotate();
        }
        return leftsize + rightsize;
    }
}


void
LayoutTrees::CalcLegIntersection(Node *left, Node *right, Node *u)
{
    double x0, y0, x1, y1, x2, y2, x3, y3;

    x0 = left->getX();
    y0 = left->getY() - NodeHeight;

    x1 = u->getX();
    y1 = u->getY() - NodeHeight;

    x2 = right->getX();
    y2 = right->getY() + NodeHeight;

    x3 = u->getX();
    y3 = u->getY() + NodeHeight;
    
    // The slants of the two lines
    double k_L = (y1 - y0) / (x1 - x0);
    double k_R = (y3 - y2) / (x3 - x2);

    double D_R = (y3 - y0 - k_L * (x3 - x0)) / (k_L - k_R);
    double D_L = x3 + D_R - x0;

    u->setX(x0 + D_L);
    u->setY(y0 + k_L * D_L);
}

//get the highest not LGT mapped node of n
Node* LayoutTrees::getHighestMappedLGT(Node *n)
{
    Node *parent = n->getParent();

    while(gamma->isLateralTransfer(*parent) && !parent->isRoot())
    {
        parent = parent->getParent();
    }

    while(!species->descendant((*lambda)[n],(*lambda)[parent]) && !parent->isRoot())
    {
        parent = parent->getParent();
    }

    return parent;
}

double LayoutTrees::biggestLabel() const
{
    double size = 0.0;
    
    for(unsigned i = 0; i < gene->getNumberOfNodes(); i++)
    {
        Node *n = gene->getNode(i);
        if(n->isLeaf())
        {
            size = max(size,(double)n->getName().size());
        }
    }
    
    for(unsigned i = 0; i < species->getNumberOfNodes(); i++)
    {
        Node *n = species->getNode(i);
        if(n->isLeaf())
        {
            size = max(size,(double)n->getName().size());
        }
    }
    
    return size;
}

void LayoutTrees::replaceNodes(const std::map<unsigned,unsigned>& replacements)
{
    for(std::map<unsigned,unsigned>::const_iterator it = replacements.begin();
        it != replacements.end(); ++it)
    {
        Node *first = gene->getNode(it->first);
        Node *second = gene->getNode(it->second);
        std::cout << "Replacing Node " << it->first << " with Node " << it->second << std::endl;
        if(first && second)
        {
            //HostParent and HostChild should not change
            double temp_x = first->getX();
            double temp_y = first->getY();
            first->setX(second->getX());
            first->setY(second->getY());
            second->setX(temp_x);
            second->setY(temp_y);
        }
    }
}
