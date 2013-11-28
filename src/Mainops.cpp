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

#include "Mainops.h"

#include "tree/Node.h"
#include "Parameters.h"
#include "tree/TreeIO.h"
#include "tree/Node.h"
#include "tree/TreeIO.h"
#include "utils/AnError.h"
#include "draw/DrawTreeCairo.h"
#include "layout/Layoutrees.h"

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> 

using namespace std;

Mainops::Mainops()
    :genesTree(0),speciesTree(0),gamma(0),lambdamap(0),dt(0),parameters(0),io(0)
{

}

void Mainops::start()
{
    io = new TreeIO();
    dt = new DrawTreeCairo();
}

void Mainops::cleanTrees()
{
    if(genesTree)
    {
        delete genesTree;
    }
    genesTree = 0;

    if(speciesTree)
    {
        delete speciesTree;
    }
    speciesTree = 0;

    if(io)
    {
        delete io;
    }
    io = 0;

    io = new TreeIO();
    AC.clear();
    gs.clearMap();
}

Mainops::~Mainops()
{
    if(genesTree)
    {
        delete(genesTree);
    }
    genesTree = 0;

    if(speciesTree)
    {
        delete(speciesTree);
    }
    speciesTree = 0;

    if(gamma)
    {
        delete(gamma);

    }
    gamma = 0;

    //if(lambdamap)
    //{
        //delete(lambdamap);

    //}
    //lambdamap = 0;

    if(dt)
    {
        delete(dt);

    }
    dt = 0;

    if(io)
    {
        delete(io);
    }
    io = 0;
}

const bool Mainops::lateralTransfer(const std::string &mapname, bool dp)
{
    Phyltr late = Phyltr();
    late.g_input.duplication_cost = parameters->lateralduplicost;
    late.g_input.transfer_cost = parameters->lateraltrancost;
    late.g_input.max_cost = parameters->lateralmaxcost;
    late.g_input.min_cost = parameters->lateralmincost;
    late.g_input.gene_tree = genesTree;
    late.g_input.species_tree = speciesTree;
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
        std::cerr << "Not valid LGT scenarios found.." << std::endl;
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

const bool Mainops::thereAreLGT(const std::vector<Scenario> &scenarios) const
{
    BOOST_FOREACH(const Scenario &sc, scenarios)
    {
        if(sc.transfer_edges.any())
        {
            return true;
        }
    }
    return false;
}

void Mainops::OpenReconciled(const string &reconciled)
{
    io->setSourceFile(reconciled);
    genesTree = new TreeExtended(io->readBeepTree(&AC, &gs));
}

void Mainops::OpenHost(const string &species)
{
    io->setSourceFile(species);
    speciesTree = new TreeExtended(io->readHostTree());
    Node *root = speciesTree->getRootNode();

    if ((double)root->getTime() != (double)0.0)
    {
        double t = root->getNodeTime();
        root->setTime(0.1 * t);
    }

    if (speciesTree->imbalance() / speciesTree->rootToLeafTime() > 0.01)
    {
        parameters->scaleByTime = false;
        std::cerr << "The species tree is not ultrametric (it appears unbalanced),\n"
                     "so scaling by time is turned off. See also option '-t'" << std::endl;

    }
}

void Mainops::CalculateGamma()
{
    if (parameters->do_not_draw_species_tree == false)
    {
        gamma = new GammaMapEx(*genesTree, *speciesTree, gs, AC);
        if (parameters->lattransfer)
        {
            gamma = new GammaMapEx(gamma->update(*genesTree,*speciesTree,sigma,transferedges));
        }
        lambdamap = gamma->getLambda();
    }
    else
    {
        lambdamap = new LambdaMapEx(*genesTree, *speciesTree, gs);
        if (parameters->lattransfer)
        {
            lambdamap->update(*genesTree,*speciesTree,sigma,transferedges);
        }
        gamma = new GammaMapEx(GammaMapEx::MostParsimonious(*genesTree,*speciesTree,*lambdamap));
    }
}

void Mainops::reconcileTrees(const string &gene, const string &species, const string &mapfile)
{
    io->setSourceFile(gene);
    genesTree = new TreeExtended(io->readBeepTree(&AC, &gs));
    io->setSourceFile(species);
    speciesTree = new TreeExtended(io->readNewickTree());

    if(mapfile != "")
    {
        gs = TreeIO::readGeneSpeciesInfo(mapfile);
    }
    else
    {
        throw AnError(": The mapfile is empty!\n");
    }

    LambdaMapEx lambdamap_local = LambdaMapEx(*genesTree, *speciesTree, gs);
    GammaMapEx gamma_local = GammaMapEx(GammaMapEx::MostParsimonious(*genesTree, *speciesTree, lambdamap_local));
    string textTree = io->writeGuestTree(*genesTree,&gamma_local);
    io->setSourceString(textTree);
    genesTree = new TreeExtended(io->readBeepTree(&AC, &gs));
    OpenHost(species);
}

void Mainops::calculateCordinates()
{
    speciesTree->reset();
    genesTree->reset();
    //reduce crossing only if not LGT
    if(parameters->reduce && !(bool)(parameters->lattransfer))
    {
        std::cerr << "NOTE : the option -R is still experimental.." << std::endl;
        gamma->twistAndTurn();
    }
    LayoutTrees spcord = LayoutTrees(speciesTree,genesTree,parameters,gamma,lambdamap);
    spcord.start();
    parameters->leafwidth = spcord.getNodeHeight();
}

const bool Mainops::checkValidity()
{
    if(parameters->lattransfer)
    {
        return getValidityLGT();
    }
    else
    {
        return gamma->valid();
    }
}

void Mainops::DrawTree(cairo_t *cr)
{

    dt->start(parameters,genesTree,speciesTree,gamma,lambdamap,cr);

    dt->calculateTransformation();
    
    if(parameters->do_not_draw_species_tree == false)
    {
        //species tree
        if (!parameters->noTimeAnnotation)
        {
            if (parameters->timeAtEdges)
            {
                dt->TimeLabelsOnEdges();
            }
            else
            {
                dt->DrawTimeEdges();
                dt->DrawTimeLabels();
            }
        }
        //dt->DrawSpeciesEdgesWithContour(); //NOTE this breaks the layout
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
        {
            dt->GeneTreeMarkers();
        }
    }
    
    if(parameters->header)
    {
        dt->createHeader();
    }
    if(parameters->legend)
    {
        dt->createLegend();
    }
    if(parameters->title)
    {
        dt->createTitle();
    }
    if(parameters->show_event_count)
    {
        dt->writeEventCosts();
    }
}

const bool Mainops::RenderImage()
{
    bool ok = dt->RenderImage();
    dt->cleanUp();
    return ok;
}

Parameters* Mainops::getParameters()
{
    return parameters;
}

void Mainops::setParameters(Parameters *p)
{
    parameters = p;
}

const bool Mainops::getValidityLGT()
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
        {
            throw AnError(": The LGT scenario was not valid. Aborts!\n");
        }
        else
        {
            throw AnError(": This is not a correctly reconciled tree. Aborts!\n");
        }
    }
    calculateCordinates(); //calculation of the drawing cordinates
    DrawTree();  //drawing the tree
    if(!RenderImage())
    {
        throw AnError(": Error rendering the tree..\n");
    } // save the file
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
            if(!RenderImage())
            {
                throw AnError(": Error rendering the tree..\n");
            } // save the file
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
            {
                continue;
            }
            else if(line[0] == '#')  // Skip any comment lines
            {
                continue;
            }
            else if ((line.find("Transfer") != std::string::npos))
            {
                const std::size_t start_pos = line.find(":");
                const std::size_t stop_pos = line.size() - 1;
                std::string temp = line.substr(start_pos + 1,stop_pos - start_pos);
                stringstream lineStream(temp);
                std::vector<std::string> transfer_nodes((istream_iterator<std::string>(lineStream)), istream_iterator<std::string>());
                transferedges.clear();
                transferedges.resize(genesTree->getNumberOfNodes());
                for(std::vector<std::string>::const_iterator it = transfer_nodes.begin(); it != transfer_nodes.end(); ++it)
                {
                    std::vector<std::string> strs;
                    std::string temp = *it;
                    temp.erase(remove(temp.begin(),temp.end(),'('),temp.end());
                    temp.erase(remove(temp.begin(),temp.end(),')'),temp.end());
                    boost::split(strs, temp, boost::is_any_of(","));
                    if(genesTree->getNode(boost::lexical_cast<unsigned>(strs.at(0))) != NULL)
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
    late.g_input.gene_tree = genesTree;
    late.g_input.species_tree = speciesTree;

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
