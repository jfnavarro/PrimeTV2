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

/* This class uses Cairo, the trees and the information given by the class LayoutTrees
 * to draw in a Cairo Object the Trees */

#ifndef DRAWTREE2_H
#define DRAWTREE2_H

#include <stdio.h>
#include <stdlib.h>
#include "libraries/gammamapex.h"
#include "libraries/BeepVector.hh"
#include <vector>   
#include <utility>  
#include <iostream> 
#include <fstream>  
#include "libraries/Node.hh"
#include "libraries/treeextended.h"
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-svg.h>
#include "phyltr.h"
#include <parameters.h>
#include "edge.h"


using namespace std;

//helper function to free memory of vectors
template <class C> void FreeClear( C & cntr ) {
    for ( typename C::iterator it = cntr.begin(); 
              it != cntr.end(); ++it ) {
        delete * it;
    }
    cntr.clear();
}

  
class DrawTree_time {
    
  public:
    
    /* constructor, parameters in constant, the trees are going to be modified
     * the gamma object is constant and the cairo object is optional */
    DrawTree_time(const Parameters&,TreeExtended &gene, TreeExtended &species, 
		  const GammaMapEx<Node> &gamma, cairo_t *cr_ = 0);
    
    //destructor
    ~DrawTree_time();
    
    /* this function create a file and render the cairo surface into 
     * that file */
    int RenderImage();
    
    /*this function make the affine matrix tranformation over the surface
     * according to the parameters given */
    void calculateTransformation();
    
    /* this function include the logo PrimeTV on the  top right part of the
     * screen */
    void createHeader();
    
    /* this function draws a legend with information of all the colors 
     * and what part of the tree they belong to */
    void createLegend();
    
    /* this function draws the text given a parameter from the user on the 
     * top of the screen*/    
    void createTitle();

    /* Add info about the cost of performed reconciliations (dupl + lgt cost) */
    void writeEventCosts();
    
    /* this function draws the markers on the gene tree*/
    void GeneTreeMarkers();
    
    /* this function draws the time axis and edges */
    void DrawTimeEdges();
    
    /* this function draws the species edges */
    void DrawSpeciesEdgesWithContour();
    void DrawSpeciesEdges();
    
    /* this function draws the time labels on the time edges */
    void DrawTimeLabels();
    
    /* this function draws the species nodes */
    void DrawSpeciesNodes();
    
    /* this function draws the species node label names and ids */
    void DrawSpeciesNodeLabels();
    
    /* this function draws the gene node labels and ids*/
    void DrawGeneLabels();
    
    /* this function draws the gene edges */
    void DrawGeneEdges();
    
    /* this function draws the gene nodes */
    void DrawGeneNodes();
    
    /* this function draws the lateral transfer edges */
    void DrawLGT();
    
    /* this function draws the time labels on the edges */
    void TimeLabelsOnEdges();
    
  private:			

    /* this function converts double to string */
    string double2charp(double x);
    
    /* this function is a helper function to draw the gene edges
     * it draws all the edges that are not Laterl Transfer
     */
    void newDrawPath(Node *x);
    
    /* this functions returns the node that is highest up mapped to the node given and it 
     * is also lateral transfer */
    Node* getHighestMappedLGT(Node *n);
    
    /* the same as before be the lowest mapped */
    
    Node* getLowestMappedLGT(Node *n);
    
    /* this functions returns the lowest noded mapped to the node given as input
     *  but not being lateral transfer */
    Node* getLowestMappedNOLGT(Node *n);
    
    /* this function checks wheter the node give is a destiny of a lateral
     * transfer or not */
    bool destinyLGT(Node *n);
    
    /* this function is a helper function to draw lateral the lateral 
     * transfer paths */
    void newLGTPath(Node *n);
    
    /* this function get the edge that includes the nodes given as inputs
     */
    Edge *getEdge(Node *sp,Node *gn);
    
    /* this function checks wheter there exist or not an edge
     * with the origin x */
    bool existLGTEdge(double x);
    
    /* this function returns a pair of x cordinates, the origin x 
     * and destiny x of the Lateral Transfer */
    pair<Node*,pair<double,double> > getOriginLGT(Node *n);
    
    /* this function checks if the nodes given as inputs overlap at x
     */
    bool overlapSpeciesNode(double x,Node *origin, Node *destiny);
    
    /* this function check if there is a collision between the two spaces given 
     * as inputs */
    bool checkCollision(double x00,double y00, double x01, 
		        double y01, double x10, double y10, double x11,double y11);
    
    /* this function add a new edge to the vector of edges */
    void addEdge(Node *spO,Node *spE,Node *gO,Node *gE,
		double xo,double yo,double xe,double ye,Edge::category m);
    unsigned NumberLT(Node *son);

  /* Computer the intersection of two lines, given four points. */
    void intersection(double x1, double y1,
		      double x2, double y2,
		      double x3, double y3,
		      double x4, double y4,
		      double &x5, double &y5);
      
    
    std::map<Node*,unsigned> LGT; //map of lateral transfer
    std::vector<Edge*> geneEdges; //edges
    LambdaMapEx<Node> lambda;
    const Parameters *parameters;
    TreeExtended *gene;
    TreeExtended *species;
    const GammaMapEx<Node> *gamma;

    //Cairo objects
    cairo_surface_t *surface;
    cairo_surface_t *surfaceBackground;
    cairo_t *cr;
    cairo_text_extents_t extents;
    cairo_matrix_t matrix;
    
    //standard drawing parameters
    double pagewidth;
    double pageheight;
    double fontsize;
    double linewidth;
    double s_contour_width;
    Colours *config;
    bool image;
    double leafWidth;
    
  /* During layout, the guest tree is traversed and the number 
   * of duplications/transfers are counted and stored here:
   */
  int nDupl;
  int nTrans;
};
#endif
