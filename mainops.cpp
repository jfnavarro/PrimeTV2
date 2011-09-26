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

#include "mainops.h"
#include "phyltr.h"
#include <boost/dynamic_bitset.hpp>
#include "libraries/Node.hh"
#include "libraries/TreeIO.hh"
#include "libraries/AnError.hh"
#include "DrawTree_time.hh"
#include "libraries/gammamapex.h"
#include "libraries/lambdamapex.h"
#include <vector>
#include "layoutrees.h"
#include <boost/foreach.hpp>

using namespace beep;
using namespace std;

Mainops::Mainops()
 :Guest(0),Host(0),gamma(0),lambdamap(0),dt(0),late(0),parameters(0),io(0)
{

}

Mainops::~Mainops()
{
  delete(Guest);
  delete(Host);
  delete(gamma);
  delete(lambdamap);
  delete(dt);
  delete(late);
  delete(parameters);
  delete(io);

}

void Mainops::lateralTransferDP(string mapname)
{  

      late = new Phyltr();
      late->g_input.duplication_cost = parameters->lateralduplicost;
      late->g_input.transfer_cost = parameters->lateraltrancost;
      late->g_input.max_cost = parameters->lateralmaxcost;
      late->g_input.min_cost = parameters->lateralmincost;
      late->g_input.print_only_minimal_loss_scenarios = false;
      late->g_input.print_only_minimal_transfer_scenarios = false;
      late->g_input.gene_tree = Guest;
      late->g_input.species_tree = Host;
      
      if(parameters->isreconciled)
      {
	late->g_input.sigma_fname = mapname;
	late->read_sigma();
      }
      else
      {
	late->read_sigma(gs.getMapping());
      }
      
      late->dp_algorithm();
      late->backtrack();
      
      if(late->scenarios.size() > 0 && thereAreLGT(late->scenarios))
      {
	Scenario maxscenario = late->getMaxCostScenario(); 
	lambda = maxscenario.cp.getLambda();
	transferedges = maxscenario.transfer_edges;
	parameters->transferedges = transferedges;
	sigma = late->g_input.sigma;
	scenarios = late->scenarios;
	late->printScenario(maxscenario);
      }
      else
      {
	parameters->lattransfer = false;
      }

}

void Mainops::lateralTransfer(string mapname)
{
      late = new Phyltr();
      late->g_input.duplication_cost = parameters->lateralduplicost;
      late->g_input.transfer_cost = parameters->lateraltrancost;
      late->g_input.max_cost = parameters->lateralmaxcost;
      late->g_input.min_cost = parameters->lateralmincost;
      late->g_input.gene_tree = Guest;
      late->g_input.species_tree = Host;
      
      if(parameters->isreconciled)
      {
	late->g_input.sigma_fname = mapname;
	late->read_sigma();
      }
      else
      {
	late->read_sigma(gs.getMapping());
      }
      
      late->fpt_algorithm();
      
      if(late->scenarios.size() > 0 && thereAreLGT(late->scenarios))
      {
	Scenario maxscenario = late->getMaxCostScenario(); 
	lambda = maxscenario.cp.getLambda();
	transferedges = maxscenario.transfer_edges;
	parameters->transferedges = transferedges;
	sigma = late->g_input.sigma;
	scenarios = late->scenarios;
      }
      else
      {
	parameters->lattransfer = false;
      }
}

bool Mainops::thereAreLGT(std::vector<Scenario> scenarios)
{
  BOOST_FOREACH(Scenario &sc, scenarios)
  {
    if(sc.transfer_edges.any())
      return true;
  }
  return false;
}


void Mainops::OpenReconciled(const char* reconciled)
{
     
    io = new TreeIO(TreeIO::fromFile(reconciled));
     
    Guest = new TreeExtended(io->readBeepTree<TreeExtended,Node>(&AC, &gs));

}

void Mainops::OpenHost(const char* species)
{
    Host = new TreeExtended(Tree::EmptyTree());
     
    io->setSourceFile(species);
    io->checkTagsForTree(traits);
 
    if(traits.containsTimeInformation() == false)
 	throw AnError("Host tree lacks time information for some of it nodes", 1);
    else
 	traits.enforceHostTree();

    delete(Host);
    Host = new TreeExtended(io->readBeepTree<TreeExtended,Node>(traits,0,0));
    Node *root = Host->getRootNode();

    if (root->getTime() == 0.0)
    {
      Real t = root->getNodeTime();
      root->setTime(0.1 * t); //the assert inside the function is failing
    }

    if (Host->imbalance() / Host->rootToLeafTime() > 0.01) {
 	parameters->scaleByTime = false;
 	cerr << "The species tree is not ultrametric (it appears unbalanced),\n"
 	  "so scaling by time is turned off. See also option '-t'.\n";

    }
    
}

void Mainops::CalculateGanmma()
{


    if (parameters->do_not_draw_species_tree == false)
    {
	gamma = new  GammaMapEx<Node>(*Guest, *Host, gs, AC);
	
        if (parameters->lattransfer)
        {
	  gamma = new GammaMapEx<Node>(gamma->update(*Guest,*Host,sigma,transferedges));
        }
    }
    else
    {
	lambdamap = new LambdaMapEx<Node>(*Guest, *Host, gs);

        if (parameters->lattransfer)
            lambdamap->update(*Guest,*Host,sigma,transferedges);

	gamma = new GammaMapEx<Node>(GammaMapEx<Node>::MostParsimonious(*Guest,*Host,*lambdamap));
    }

  
}


void Mainops::reconcileTrees(const char* gene, const char* species, const char* mapfile)
{

    io = new TreeIO(TreeIO::fromFile(gene));
    Guest = new TreeExtended(io->readBeepTree<TreeExtended,Node>(&AC, &gs));
    
    io->setSourceFile(species);
    Host = new TreeExtended(io->readNewickTree<TreeExtended,Node>());

    if (strcmp(mapfile,"")!=0)
    {  
      gs = TreeIO::readGeneSpeciesInfo(mapfile);
    }

    lambdamap = new LambdaMapEx<Node>(*Guest, *Host, gs);

    gamma = new GammaMapEx<Node>(GammaMapEx<Node>::MostParsimonious(*Guest, *Host, *lambdamap));
    
    string textTree =  io->writeGuestTree<TreeExtended,Node>(*Guest,gamma);
    
    delete(io);
    io = new TreeIO(TreeIO::fromString(textTree));
   
    delete(Guest);
    Guest = new TreeExtended(io->readBeepTree<TreeExtended,Node>(&AC, &gs));

    OpenHost(species);

}



  void Mainops::calculateCordinates()
  {
     bool status = true;

     if(parameters->lattransfer)
       status = getValidityLGT();
     else
       status = gamma->valid();
     
     if (!status) {
	if(parameters->lattransfer)
	  throw AnError(": The LGT scenario was not valid. Aborts!\n");
	else
	  throw AnError(": This is not a correctly reconciled tree. Aborts!\n");
     }  
     else 
     {   
        Host->reset();
	Guest->reset();
	LayoutTrees *spcord = new LayoutTrees(*Host,*Guest,*parameters,*gamma);
	parameters->leafwidth = spcord->getNodeHeight();

     }
  }


void Mainops::DrawTree(cairo_t *cr)
{
    dt = new DrawTree_time(*parameters,*Guest,*Host,*gamma,cr);
    
    dt->calculateTransformation();
    
    if(parameters->do_not_draw_species_tree == false) {
       //species tree
      if (!parameters->noTimeAnnotation) {
	if (parameters->timeAtEdges) {
	    dt->TimeLabelsOnEdges();
	} else {
	    dt->DrawTimeEdges();
	    dt->DrawTimeLabels();
	}
      }
      dt->DrawSpeciesEdges();
      dt->DrawSpeciesNodes();
      dt->DrawSpeciesNodeLabels();
    }
    // gene tree
    if(!parameters->do_not_draw_guest_tree)
    {
      dt->DrawGeneNodes();
      dt->DrawGeneEdges();
      dt->DrawGeneLabels();
      if(parameters->markers)
	dt->GeneTreeMarkers();
    }
    
    if(parameters->header)
      dt->createHeader();
    if(parameters->legend)
      dt->createLegend();
    if(parameters->tittle)
      dt->createTittle();
    
}


int Mainops::RenderImage()
{
  return dt->RenderImage();
}

Parameters* Mainops::getParameters()
{
  return parameters;
}

void Mainops::setParameters(Parameters *p)
{
  parameters = p;
}

bool Mainops::getValidityLGT()
{
  if(gamma->validLGT())
    return true;
  else
  {
    sort(scenarios.begin(), scenarios.end());
  
    BOOST_REVERSE_FOREACH (Scenario &sc, scenarios)
    {
      transferedges = sc.transfer_edges;
      parameters->transferedges = sc.transfer_edges;
      parameters->duplications = sc.duplications;
      lambda = sc.cp.getLambda();
      CalculateGanmma();
      if(gamma->validLGT())
      { 
	return true;
      }
    }
    
    return false;
  } 
}


