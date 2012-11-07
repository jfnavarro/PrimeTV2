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

 /*TODO I should remove the functionality of the objects from the constructor and only create
  * and destroy objects one time */
 
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
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> 

using namespace std;

Mainops::Mainops()
 :Guest(0),Host(0),gamma(0),lambdamap(0),dt(0),parameters(0),io(0)
{

}

void Mainops::start()
{
  io = new TreeIO();
  dt = new DrawTree_time();
}

Mainops::~Mainops()
{
  if(Guest)
  {
    delete(Guest);
    Guest = 0;
  }
  if(Host)
  {
    delete(Host);
    Host = 0;
  }
  if(gamma)
  {
    delete(gamma);
    gamma = 0;
  }
  /*if(lambdamap)
  {
    delete(lambdamap);
    lambdamap = 0;
  }*/
  if(dt)
  {
    delete(dt);
    dt = 0;
  }
  if(io)
  {
    delete(io);
    io = 0;
  }
}

bool Mainops::lateralTransfer(const std::string &mapname, bool dp)
{
      Phyltr late =  Phyltr();
      late.g_input.duplication_cost = parameters->lateralduplicost;
      late.g_input.transfer_cost = parameters->lateraltrancost;
      late.g_input.max_cost = parameters->lateralmaxcost;
      late.g_input.min_cost = parameters->lateralmincost;
      late.g_input.gene_tree = Guest;
      late.g_input.species_tree = Host;
      if(dp)
      {
	late.g_input.print_only_minimal_loss_scenarios = false;
	late.g_input.print_only_minimal_transfer_scenarios = false;
      }
      if(parameters->isreconciled)
      {
	late.g_input.sigma_fname = mapname;
	late.read_sigma();
      }
      else
      {
	late.read_sigma(gs.getMapping());
      }
      
      if(dp)
      {      
	late.dp_algorithm();
	late.backtrack();
      }
      else
      {
	late.fpt_algorithm();
      }
      if(late.scenarios.size() > 0 && thereAreLGT(late.scenarios))
      {
	Scenario scenario = late.getMinCostScenario(); 
	lambda = scenario.cp.getLambda();
	transferedges = scenario.transfer_edges;
	parameters->transferedges = transferedges;
	sigma = late.g_input.sigma;
	scenarios = late.scenarios;
	return true;
      }
      else
      {
	parameters->lattransfer = false;
	std::cout << "Not valid LGT scenarios found.." << std::endl;
	return false;
      }
}

void Mainops::printLGT()
{
  std::cout << "List of computed LGT scenarios sorted by cost.." << std::endl;
  BOOST_FOREACH(Scenario &sc, scenarios)
  {
    std::cout << sc << std::endl;
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
    io->setSourceFile(reconciled);
    //NOTE copy constructor re-writes and it seems to clean everything up
    Guest = new TreeExtended(io->readBeepTree<TreeExtended,Node>(&AC, &gs));
}

void Mainops::OpenHost(const char* species)
{    
    io->setSourceFile(species);
    io->checkTagsForTree(traits);
 
    if(traits.containsTimeInformation() == false)
 	throw AnError("Host tree lacks time information for some of it nodes", 1);
    else
 	traits.enforceHostTree();
    
    //NOTE copy constructor re-writes and it seems to clean everything up
    Host = new TreeExtended(io->readBeepTree<TreeExtended,Node>(traits,0,0));
    Node *root = Host->getRootNode();

    if ((double)root->getTime() == (double)0.0)
    {
      //NOTE this is strange, if node->time == 0 then node->time = 0*0.1 ??
      Real t = root->getNodeTime();
      root->setTime(0.1 * t); //the assert inside the function is failing
    }

    if (Host->imbalance() / Host->rootToLeafTime() > 0.01) {
 	parameters->scaleByTime = false;
 	cerr << "The species tree is not ultrametric (it appears unbalanced),\n"
 	  "so scaling by time is turned off. See also option '-t'.\n";

    }
    
}

void Mainops::CalculateGamma()
{

    if (parameters->do_not_draw_species_tree == false)
    {
	gamma = new GammaMapEx<Node>(*Guest, *Host, gs, AC);
        if (parameters->lattransfer)
        {
	  gamma = new GammaMapEx<Node>(gamma->update(*Guest,*Host,sigma,transferedges));
        }
        lambdamap = gamma->getLambda();
    }
    else
    {  
	lambdamap = new LambdaMapEx<Node>(*Guest, *Host, gs);

        if (parameters->lattransfer)
	{
            lambdamap->update(*Guest,*Host,sigma,transferedges);
	}
	
	gamma = new GammaMapEx<Node>(GammaMapEx<Node>::MostParsimonious(*Guest,*Host,*lambdamap));
    }

  
}


void Mainops::reconcileTrees(const char* gene, const char* species, const char* mapfile)
{

    io->setSourceFile(gene);
    Guest = new TreeExtended(io->readBeepTree<TreeExtended,Node>(&AC, &gs));
    io->setSourceFile(species);
    Host = new TreeExtended(io->readNewickTree<TreeExtended,Node>());

    if (strcmp(mapfile,"")!=0)
    {  
      gs = TreeIO::readGeneSpeciesInfo(mapfile);
    }
    else
    {
      throw AnError(": The mapfile is empty!\n");
    }

    LambdaMapEx<Node> lambdamap_local = LambdaMapEx<Node>(*Guest, *Host, gs);
    GammaMapEx<Node> gamma_local = GammaMapEx<Node>(GammaMapEx<Node>::MostParsimonious(*Guest, *Host, lambdamap_local));
    string textTree =  io->writeGuestTree<TreeExtended,Node>(*Guest,&gamma_local);
    io->setSourceString(textTree);
    Guest = new TreeExtended(io->readBeepTree<TreeExtended,Node>(&AC, &gs));
    OpenHost(species);
}



 void Mainops::calculateCordinates()
  {
    Host->reset();
    Guest->reset();
    LayoutTrees spcord = LayoutTrees(Host,Guest,parameters,gamma,lambdamap);  
    spcord.start();
    //reduce crossing only if not LGT 
    if(parameters->reduce && !(bool)(parameters->lattransfer))
    {
	std::cout << "NOTE : this option is still experimental.." << std::endl;
	Layout layout = Layout(Host, Guest);
	std::map<int,int> node2node;
	layout.run(node2node,*gamma);
	spcord.replaceNodes(node2node);
    }
    parameters->leafwidth = spcord.getNodeHeight();
  }

  
 int Mainops::checkValidity()
  {
     if(parameters->lattransfer)
       return getValidityLGT();
     else
       return gamma->valid();
  }

void Mainops::DrawTree(cairo_t *cr)
{

    dt->start(parameters,Guest,Host,gamma,lambdamap,cr);

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
      //dt->DrawSpeciesEdgesWithContour();
      dt->DrawSpeciesEdges();
      dt->DrawSpeciesNodes();
      dt->DrawSpeciesNodeLabels();
    }
    // gene tree
    if(!parameters->do_not_draw_guest_tree)
    {
      dt->DrawGeneEdges();
      dt->DrawGeneNodes();
      dt->DrawGeneLabels();
      if(parameters->markers)
	dt->GeneTreeMarkers();
    }
    
    if(parameters->header)
      dt->createHeader();
    if(parameters->legend)
      dt->createLegend();
    if(parameters->title)
      dt->createTitle();
    if (parameters->show_event_count)
      dt->writeEventCosts();
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
  {
    return true;
  }
  else
  {
    sort(scenarios.begin(), scenarios.end());
  
    BOOST_FOREACH (Scenario &sc, scenarios)
    {
      transferedges = sc.transfer_edges;
      parameters->transferedges = sc.transfer_edges;
      parameters->duplications = sc.duplications;
      lambda = sc.cp.getLambda();
      CalculateGamma();
      if(gamma->validLGT())
      { 
	return true;
      }
    }
    return false;
  } 
}

void Mainops::drawBest()
{
   CalculateGamma(); //calculation of gamma and lambda
   if(!checkValidity())
   {
     if(parameters->lattransfer)
      throw AnError(": The LGT scenario was not valid. Aborts!\n");
    else
      throw AnError(": This is not a correctly reconciled tree. Aborts!\n");
   }
   calculateCordinates(); //calculation of the drawing cordinates
   DrawTree();  //drawing the tree
   RenderImage(); // save the file
}

void Mainops::drawAllLGT()
{
  unsigned index = 0;
  std::string original_filename = parameters->outfile;
  sort(scenarios.begin(), scenarios.end());
  BOOST_FOREACH (Scenario &sc, scenarios)
  {
    transferedges = sc.transfer_edges;
    parameters->transferedges = sc.transfer_edges;
    parameters->duplications = sc.duplications;
    lambda = sc.cp.getLambda();
    parameters->outfile = original_filename + boost::lexical_cast<string>(++index);
    CalculateGamma(); //calculation of gamma and lambdamap
    if(gamma->validLGT())
    {
      calculateCordinates(); //calculation of the drawing cordinates
      DrawTree();  //drawing the tree
      RenderImage(); // save the file
    }
  }
}

void Mainops::loadPreComputedScenario(const std::string &filename,const std::string &mapname)
{
  
  std::ifstream scenario_file;
  std::string line;
  scenario_file.open(filename.c_str(), std::ios::in);
  if (!scenario_file) 
  {
     throw AnError("Could not open file " + filename);
  }
  
  while (getline(scenario_file, line)) 
  {
     if(scenario_file.good())    
     {
        if(line.size() == 0)  // Skip any blank lines
                continue;
        else if(line[0] == '#')  // Skip any comment lines
                continue;
        else if ((line.find("Transfer") != std::string::npos))
	  {
	    const std::size_t start_pos = line.find(":");
	    const std::size_t stop_pos = line.size() - 1;
	    std::string temp = line.substr(start_pos + 1,stop_pos - start_pos);
	    stringstream lineStream(temp);
	    std::vector<std::string> transfer_nodes((istream_iterator<std::string>(lineStream)), istream_iterator<std::string>());
	    transferedges.clear();
	    transferedges.resize(Guest->getNumberOfNodes());
	    for(std::vector<std::string>::const_iterator it = transfer_nodes.begin(); it != transfer_nodes.end(); ++it)
	    {
	      std::vector<std::string> strs;
	      std::string temp = *it;
	      temp.erase(remove(temp.begin(),temp.end(),'('),temp.end());
	      temp.erase(remove(temp.begin(),temp.end(),')'),temp.end());
	      boost::split(strs, temp, boost::is_any_of(","));
	      if(Guest->getNode(boost::lexical_cast<unsigned>(strs.at(0))) != NULL)
	      {
		transferedges.set(boost::lexical_cast<unsigned>(strs.at(0))); //origin LGT
	      }
	      else
	      {
		throw AnError("Node read in the LGT scenario file does not exist in the Gene Tree");
	      }
	      //transferedges.set(boost::lexical_cast<unsigned>(strs.at(1))); //destiny LGT
	      // strs.at(2) //this is the time NOTE not used yet (the idea is to put the time in the node and use it to compute the cordinates
	    }
	  }
     }
  }
  scenario_file.close();
  parameters->lattransfer = true;
  Phyltr late = Phyltr();
  late.g_input.gene_tree = Guest;
  late.g_input.species_tree = Host;
  if(parameters->isreconciled)
  {
    late.g_input.sigma_fname = mapname;
    late.read_sigma();
  }
  else
  {
    late.read_sigma(gs.getMapping());
  }
  parameters->transferedges = transferedges;
  sigma = late.g_input.sigma;
  return;
}