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

/* This class holds the main operation of the program, it gives functions
 * to reconcile and open the trees and to calculate the gamma and the lambda and also
 * to obtain the LGT scenarios, it also gives functions to draw the trees and calculates
 * the cordinates */

#ifndef MAINOPS_H
#define MAINOPS_H

#include "lgt/Phyltr.h"
#include "reconcilation/GammaMapEx.h"
#include "reconcilation/LambdaMapEx.h"
#include "reconcilation/SetOfNodesEx.hh"
#include "draw/DrawTreeCairo.h"

#include <boost/dynamic_bitset.hpp>

class TreeIO;
class AnError;
class Node;
class Parameters;
class StrStrMap;

class Mainops
{

public:
  
    //constructor
    Mainops();  
    //destructor
    ~Mainops();
    //instantiates objects
    void start();
    //destroy and create trees
    void cleanTrees();

    /* calculate the Lateral transfer scenarios using parameters,
    * return true if there is a valid scenario. 
    */
    bool lateralTransfer(const std::string &mapname, bool dp = false /*dynamic programming*/);

    /* load the reconciled gene tree and obtatins its information */
    void OpenReconciled(const string &gene);

    /* load the species tree and obtain its information */
    void OpenHost(const string &species);

    /* reconcile the gene tree given as input, the species tree and the map file are needed */
    void reconcileTrees(const string &gene, const string &species, const string &mapfile);

    /* calculate the gamma map and the lambda map of the trees loaded */
    void CalculateGamma();

    /* check wether the reconcilation of LGT scenario is valid */
    int checkValidity();

    /* calculates the cordinates of the tree */
    void calculateCordinates();

    /* Draw the tree using Cairo */
    void DrawTree(cairo_t *cr = 0);

    /* save the tree on a file, returns 1 if ok of 0 it if failed */
    int RenderImage();

    /* return the parameters object */
    Parameters* getParameters();

    /* set the parameters object */
    void setParameters(Parameters *p);

    /* print a selected numbef of LGT scenarios sorted by cost */
    void printLGT();

    /* draws and saves in a file the most optimal scenario */
    void drawBest();

    /* draws and saves in different files all the valid LGT scenarios*/
    void drawAllLGT();

    /* loads a precomputed LGT scenario from a text file
    * file must look like :
    * Transfer edges Numbers:	(origin,destiny,time) (1,2,0.12) 
    */
    void loadPreComputedScenario(const std::string &filename,const std::string &mapname); 

    /* check if there are scenarios with LGT in the set of scenarios given */
    bool thereAreLGT(const std::vector<Scenario> &scenarios) const;

    /* check whether there is a scenario valid on the vector of scenarios */
    bool getValidityLGT();

protected:
    
    TreeExtended *Guest;
    TreeExtended *Host;
    GammaMapEx<Node> *gamma;
    LambdaMapEx<Node> *lambdamap;
    DrawTreeCairo *dt; //drawing
    Parameters *parameters;

    std::vector<Scenario> scenarios;
    dynamic_bitset<> transferedges;
    std::vector<unsigned> sigma;
    std::vector<unsigned> lambda;

    TreeIO *io;
    std::vector<SetOfNodesEx<Node> > AC;
    StrStrMap gs;

};  

#endif // MAINOPS_H
