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

/* This class holds the main operation of the program, it gives functions
 * to reconcile and open the trees and to calculate the gamma and the lambda and also
 * to obtain the LGT scenarios, it also gives functions to draw the trees and calculates
 * the cordinates */

#ifndef MAINOPS_H
#define MAINOPS_H
#include "phyltr.h"
#include <boost/dynamic_bitset.hpp>
#include "libraries/Node.hh"
#include <parameters.h>
#include "libraries/TreeIO.hh"
#include "libraries/AnError.hh"
#include "DrawTree_time.hh"
#include "libraries/gammamapex.h"
#include "Layout.h"

class Mainops
{

public:
  
  //constructor
  Mainops();  
  //destructor
  ~Mainops();
  
  /* calculate the Lateral transfer scenarios using parameters,
   * return true if there is a valid scenario. 
   */
  bool lateralTransfer(string mapname);
  
  /* calculate the lateral transfer scenarios not using parameters, therefore 
   * dynamic programming. Return true if there is a valid scenario.
   */
  bool lateralTransferDP(string mapname);
  
  /* load the reconciled gene tree and obtatins its information */
  void OpenReconciled(const char* reconciled);
  
  /* load the species tree and obtain its information */
  void OpenHost(const char* species);
  
  /* reconcile the gene tree given as input, the species tree and the map file are needed */
  void reconcileTrees(const char* gene, const char* species, const char* mapfile);
  
  /* calculate the gamma map and the lambda map of the trees loaded */
  void CalculateGamma();
  
  /* calculates the cordinates of the tree */
  void calculateCordinates();
  
  /* Draw the tree using Cairo */
  void DrawTree(cairo_t *cr = 0);
  
  /* save the tree on a file */
  int RenderImage();
  
  /* return the parameters object */
  Parameters* getParameters();
  
  /* set the parameters object */
  void setParameters(Parameters *p);
  
  /* reduce number of crossing lines */
  void reduceCrossing();
  
private:
  
  /* check if there are scenarios with LGT in the set of scenarios given */
  bool thereAreLGT(std::vector<Scenario> scenarios);
  
  /* check whether there is a scenario valid on the vector of scenarios */
  bool getValidityLGT();
  
  TreeExtended *Guest;
  TreeExtended *Host;
  GammaMapEx<Node> *gamma;
  LambdaMapEx<Node> *lambdamap;
  DrawTree_time *dt; //drawing
  Phyltr *late; //LGT
  TreeIOTraits traits;
  std::vector<Scenario> scenarios;
  dynamic_bitset<> transferedges;
  std::vector<unsigned> sigma;
  std::vector<unsigned> lambda;
  Parameters *parameters;
  TreeIO *io;
  vector<SetOfNodesEx<Node> > AC;
  StrStrMap          gs;
  Layout* layout;
};  


#endif // MAINOPS_H
