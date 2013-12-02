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
             Lars Arvestad, © the MCMC-club, SBC, all rights reserved
             Bengt Sennblad, © the MCMC-club, SBC, all rights reserved
 */

#include "TreeIO.h"

#include <cassert>		// For early bug detection
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

TreeIO::TreeIO()
    : source(readFromStdin),
      stringThatWasPreviouslyNamedS("")
{}

TreeIO::TreeIO(enum TreeSource source, const std::string &s)
    : source(source),
      stringThatWasPreviouslyNamedS(s)
{}

TreeIO::~TreeIO()
{}

TreeIO
TreeIO::fromFile(const std::string &f)
{
    if (f == "")
    {
        return TreeIO();
    }
    else
    {
        return TreeIO(readFromFile, f);
    }
}


TreeIO
TreeIO::fromString(const std::string &s)
{
    return TreeIO(readFromString, s);
}

// Change source using these utilities:
void
TreeIO::setSourceFile(const std::string &filename)
{
    source = readFromFile;
    stringThatWasPreviouslyNamedS = filename;
}

void
TreeIO::setSourceString(const std::string &str)
{
    source = readFromString;
    stringThatWasPreviouslyNamedS = str;
}

// Map leaves in the gene tree to leaves in the species tree
// This is a bit incongruent with the rest of the code and should 
// probably get its own class! /arve
// Expected line format: 
// <whitespace>? <gene name> <whitespace> <species name> <whitespace>?
// Line length is at most 1024 chars.
StrStrMap
TreeIO::readGeneSpeciesInfo(const std::string &filename)
{
    ifstream is(filename.c_str());
    unsigned lineno = 1;

    StrStrMap gene2species;
    if(is.peek() == '#') // gs may start with a '#'
    {
        char dummy[LINELENGTH];
        is.getline(dummy, LINELENGTH);// >> dummy;
    }

    while (is.good())
    {
        std::string gene = std::string();
        std::string species = std::string();
        if (is >> gene)
        {
            if (is >> species)
            {
                gene2species.insert(gene, species);
            }
            else
            {
                std::ostringstream line_str;
                line_str << "Line " << lineno;
                is.close();
                throw AnError("The gene-to-species mapping seems to be "
                              "badly formatted. ", line_str.str());
            }
        }
        lineno++;
    }
    is.close();
    return gene2species;
}

std::vector<StrStrMap>
TreeIO::readGeneSpeciesInfoVector(const std::string &filename)
{
    ifstream is(filename.c_str());
    unsigned lineno = 1;
    std::vector<StrStrMap> gene2speciesVec;
    StrStrMap gene2species;
    std::string gsmark;
    is >> gsmark;
    if(gsmark != "#")
    {
        throw AnError("error in gs vector, every gs must be preceeded by '#' line\n");
    }

    while (is.good())
    {
        char dummy[LINELENGTH];
        is.getline(dummy, LINELENGTH);
        std::string gene;
        std::string species;
        if (is >> gene)
        {
            if(gene == "#")
            {
                gene2speciesVec.push_back(gene2species);
                gene2species.clearMap();
            }
            else
            {
                if (is >> species)
                {
                    gene2species.insert(gene, species);
                }
                else
                {
                    std::ostringstream line_str;
                    line_str << "(Line " << lineno << ")";
                    throw AnError("The gene-to-species mapping seems to be "
                                  "badly formatted. ", line_str.str());
                }
            }
        }
        
        lineno++;
    }
    gene2speciesVec.push_back(gene2species);
    return gene2speciesVec;
}

TreeExtended TreeIO::readHostTree()
{
    TreeIOTraits traits;
    checkTagsForTree(traits);
    NHXtree *t = readTree();
    if(traits.containsTimeInformation() == false)
    {
        throw AnError("Host tree lacks time information for some of it nodes", 1);
    }
    traits.enforceHostTree();
    std::vector<SetOfNodesEx<Node> > *AC = 0;
    StrStrMap *gs = 0;
    TreeExtended tree = readBeepTree(t, traits, AC, gs);
    delete_trees(t);
    return tree;
}

TreeExtended TreeIO::readGuestTree()
{
    std::vector<SetOfNodesEx<Node> > *AC = 0;
    StrStrMap *gs = 0;
    return readGuestTree(AC, gs);
}

TreeExtended TreeIO::readNewickTree()
{
    TreeIOTraits traits;
    checkTagsForTree(traits);
    NHXtree *t = readTree();
    traits.setET(false);
    traits.setNT(false);
    traits.setBL(traits.hasNW());
    traits.setNWisET(false);
    std::vector<SetOfNodesEx<Node> > *AC = 0;
    StrStrMap *gs = 0;
    TreeExtended tree = readBeepTree(t, traits, AC, gs);
    delete_trees(t);
    return tree;
}

std::string TreeIO::writeBeepTree(const TreeExtended &G,const TreeIOTraits& traits,const GammaMapEx* gamma)
{
    assert((traits.hasET() && traits.hasNT()) == false);
    std::string least = "";
    std::ostringstream name;

    if (traits.hasName())
    {
        name << "[&&PRIME NAME=" << G.getName();

        if(G.getRootNode() == 0)
        {
            name << "] [empty tree!]";
            return name.str();
        }
        else
        {
            if(traits.hasNT())
            {
                name << " TT=" << G.getTopTime();
            }
            name << "]";
        }
    }

    return recursivelyWriteBeepTree(*G.getRootNode(), least, traits,
                                    gamma, 0, 0, 0) + name.str();
}

std::string TreeIO::writeBeepTree(const TreeExtended& G, const GammaMapEx *gamma)
{
    TreeIOTraits traits;
    traits.setID(true);
    if(G.hasTimes())
    {
        traits.setNT(true);
    }
    if(G.hasLengths())
    {
        traits.setBL(true);
    }
    return writeBeepTree(G, traits, gamma);
}

TreeExtended TreeIO::readBeepTree(std::vector<SetOfNodesEx<Node> > *AC, StrStrMap *gs)
{
    TreeIOTraits traits;
    checkTagsForTree(traits);
    traits.enforceStandardSanity();
    return readBeepTree(traits, AC, gs);
}

std::string TreeIO::writeGuestTree(const TreeExtended& G, const GammaMapEx* gamma)
{
    TreeIOTraits traits;
    traits.setID(true);
    if(G.hasLengths())
    {
        traits.setBL(true);
    }
    return writeBeepTree(G, traits, gamma);
}

TreeExtended TreeIO::readBeepTree(const TreeIOTraits& tr, std::vector<SetOfNodesEx<Node> > *AC,
                                  StrStrMap *gs)
{
    NHXtree* t = readTree();
    if (t == 0)
    {
        throw AnError("No tree found!");
    }

    TreeExtended tree = readBeepTree(t, tr, AC, gs);
    delete_trees(t);
    return tree;
}

TreeExtended TreeIO::readGuestTree(std::vector<SetOfNodesEx<Node> >* AC, StrStrMap* gs)
{
    TreeIOTraits traits;
    checkTagsForTree(traits);
    NHXtree *t = readTree();
    if(traits.hasGS() == false)
    {
        gs = 0;
    }
    if(traits.hasAC() == false)
    {
        AC = 0;
    }
    traits.enforceGuestTree();
    TreeExtended tree = readBeepTree(t, traits, AC, gs);
    delete_trees(t);
    return tree;
}

void TreeIO::checkTagsForTree(TreeIOTraits &traits)
{
    // Reset all argmuents before starting
    traits.setNW(true);
    traits.setET(true);
    traits.setNT(true);
    traits.setBL(true);
    traits.setGS(true);
    traits.setAC(false);
    traits.setHY(false);

    // Parse tree from its source, into temporary datastructure
    NHXtree* t = readTree();
    if (t == 0)
    {
        throw AnError("The input tree is 0!",
                      "TreeIO::checkTagsForTree()",
                      1);
    }

    while(t)
    {
        if(recursivelyCheckTags(t->root, traits) == false)
        {
            throw AnError("The input tree was empty!",
                          "TreeIO::checkTagsForTree()",
                          1);
        }
        t = t->next;
    }

    delete_trees(t);
    t = 0;
}

// Find the right value for edge time
double
TreeIO::decideEdgeTime(const NHXnode *v, const TreeIOTraits& traits, bool isHY) const
{
    double edge_time = 0.0;

    //NHX_debug_print(const_cast<NHXnode*>(v));

    if(traits.hasET()) // Use edge time info from file
    {
        if(traits.hasNWisET())
        {
            NHXannotation* a = find_annotation(v, "NW");
            if(a != 0)
            {
                edge_time = a->arg.t;
            }
            else if (isRoot(v))
            {
                edge_time = 0.0;
            }
            else
            {
                throw AnError("Edge without edge time found in tree.", 1);
            }
        }
        else
        {
            NHXannotation *a = find_annotation(v, "ET");
            if(a != 0)
            {
                edge_time = a->arg.t;
            }
            else if (isRoot(v))
            {
                edge_time = 0.0;
            }
            else
            {
                throw AnError("Edge without edge time found in tree.", 1);
            }

        }

        // Check for sanity
        if(edge_time <= 0)
        {

            if(edge_time < 0)
            {
                throw AnError("Tree contains an edge with negative time",1);
            }
            else if(isHY == false && !(bool)isRoot(v))
            {
                throw AnError("Tree contains an edge with zero time.", 1);
            }
        }
    }

    return edge_time;
}

// Always include name , if it exists
std::string
TreeIO::decideNodeName(const NHXnode *v)
{
    std::string name = "";		// Default name is empty
    if (v->name)		// Leaf names are always inserted into 'name'
    {
        name = v->name;
    }
    else			// Inner nodes might have a name. Go check
    {
        NHXannotation *a = find_annotation(v, "S");
        if (a)			// Great, we found it
        {
            name = a->arg.str;	// Pick string from union 'arg'
        }
    }
    return name;
}

void
TreeIO::handleBranchLengths(Node *node, const NHXnode *v, bool NWIsET)
{
    if(NHXannotation *a = find_annotation(v, "BL"))
    {
        node->setLength(a->arg.t);
    }
    else if(NWIsET)
    {
        throw AnError("TreeIO::extendBeepTree(...):\n"
                      "No branch length info found either in 'BL' and 'NW' is used for 'ET'",
                      234);
    }
    else if(NHXannotation *a = find_annotation(v, "NW"))
    {
        node->setLength(a->arg.t);
    }
    else if (v->parent)		// If not root...
    {
        throw AnError("TreeIO::extendBeepTree(...):\n"
                      "No branch length info found either in 'BL' or 'NW'",
                      234);
    }
}

// Recursively checks what tags are given for all nodes in subtree T_v
// Precondition: All bool argument has proper values. Assume a specific
// bool argument, 'A' has incoming value 'a', and the value for the 
// current subtree is 'b', then on return, A = a && b, i.e., false if 
// either a or b is false.
// postcondition: return true if subtree is non-empty, i.e, v != 0
bool TreeIO::recursivelyCheckTags(const NHXnode* v, TreeIOTraits& traits)
{
    if (v == 0) // i.e. if the parent was not a leaf
    {
        return false;
    }

    recursivelyCheckTags(v->left, traits);
    recursivelyCheckTags(v->right, traits);
    checkTags(v, traits);
    return true;
}


// Checks what tags are given for node v
// Precondition: All bool argument has proper values. Assume a specific
// bool argument, 'A' has incoming value 'a', and the value for the 
// current node is 'b', then on return, A = a && b.
//----------------------------------------------------------------------
void
TreeIO::checkTags(const NHXnode *v, TreeIOTraits& traits) const
{
    // Determine if NW is given
    if(find_annotation(v, "NW") == 0 && !isRoot(v))
    {
        traits.setNW(false);
    }

    // Determine if ET is given
    if(find_annotation(v, "ET") == 0 && !isRoot(v))
    {
        traits.setET(false);
    }

    // Check if NT is given
    if(find_annotation(v, "NT") == 0 && !isLeaf(v))
    {
        traits.setNT(false);
    }

    // Check if BL is given
    if(find_annotation(v, "BL") == 0 && !isRoot(v))
    {
        traits.setBL(false);
    }

    // Check if AC is given.
    if(find_annotation(v, "AC"))
    {
        traits.setAC(true);
    }

    // Check if GS is given for leaves.
    if (v->left == 0 && v->right == 0 && speciesName(v) == 0)
    {
        traits.setGS(false);
    }

    // Check if there are hybrid annotations
    if(find_annotation(v, "HY") || find_annotation(v, "EX") ||
            find_annotation(v, "OP"))
    {
        traits.setHY(true);
    }
}


// Generic reading function, interfacing NHX* code
//----------------------------------------------------------------------
NHXtree*
TreeIO::readTree()
{
    if (source == readFromStdin)
    {
        return read_tree(0);
    }
    else if (source == readFromFile)
    {
        return read_tree(stringThatWasPreviouslyNamedS.c_str());
    }
    else if (source == readFromString)
    {
        return read_tree_string(stringThatWasPreviouslyNamedS.c_str());
    }
    else
    {
        throw AnError("TreeIO not properly initialized!");
        return 0;
    }
}

// The basic function for reading NHX trees
TreeExtended TreeIO::readBeepTree(NHXtree *t, const TreeIOTraits& traits,
                                  std::vector<SetOfNodesEx<Node> > *AC, StrStrMap *gs)
{
    assert(t != 0);
    TreeExtended tree;

    // Create BeepVectors to hold required 'tag' info
    if(traits.hasET() || traits.hasNT() ||
            (traits.hasNW() && traits.hasNWisET()))
    {
        tree.setTimes(*new RealVector(treeSize(t)));
    }
    if(traits.hasBL()|| (traits.hasNW() && traits.hasNWisET() == false))
    {
        tree.setLengths(*new RealVector(treeSize(t)));
    }

    // Convert it into our preferred C++ data structure
    Node *r = extendBeepTree(tree, t->root, traits, AC, gs, 0, 0);
    if (r == 0)
    {
        throw AnError("The input tree was empty!");
    }

    NHXannotation *a = find_annotation(t->root, "NAME");
    if(a != 0)
    {
        tree.setName(a->arg.str);
    }

    if(traits.hasNT())
    {
        NHXannotation *a = find_annotation(t->root, "TT");
        if(a != 0)
        {
            tree.setTopTime(a->arg.t);
        }
    }

    tree.setRootNode(r);

    if(tree.IDnumbersAreSane(*r) == false)
    {
        throw AnError("There are higher ID-numbers than there are nodes in tree", "TreeIO::readBeepTree");
    }
    return tree;
}

// The basic recursion function for reading node info from NHX structs

Node* TreeIO::extendBeepTree(TreeExtended &S,
                             const NHXnode *v,
                             const TreeIOTraits& traits,
                             std::vector<SetOfNodesEx<Node> > *AC,
                             StrStrMap *gs,
                             std::map<const Node*, Node*>* otherParent,
                             std::map<const Node*, unsigned>* extinct)
{
    if (v == 0)
    {
        return 0;
    }
    else
    {
        // First find out if node already exists
        Node* new_node;
        NHXannotation* id = find_annotation(v, "ID");
        const bool hasID = (id != 0);
        if(hasID)
        {
            new_node = S.getNode(id->arg.i);

            // We must have ID to be able to give HY, which gives
            // the other parent of a hybrid child
            if(new_node)
            {
                NHXannotation* h = find_annotation(v, "HY");
                if(h != 0)
                {
                    if(otherParent)
                    {
                        (*otherParent)[new_node] = new_node->getParent();
                        S.setTopTime(new_node->getTime());
                        return new_node;
                    }
                    else
                    {
                        throw AnError("This is a HybridTree. Please use "
                                      "readHybridTree instead",
                                      "TreeIO::extendBeepTree",
                                      1);
                    }
                }
                else
                {
                    ostringstream oss;
                    oss << "TreeIO::extendBeepTree\n"
                        << "Found duplicate ID for non-hybrid node "
                        << id->arg.i << endl;
                    throw AnError(oss.str(),1);
                }
            }
        }

        //Otherwise create new node
        string name = decideNodeName(v);
        double leftTime = 0;
        double rightTime = 0;

        // Pass on recursion -- topTime is used to temporarily store
        // the edgeTime of a Node, remember to record them in left/rightTime
        Node* l = extendBeepTree(S, v->left, traits, AC, gs, otherParent, extinct);
        if(traits.hasET() && l)
        {
            leftTime = S.getTopTime() + S.getTime(*l);
        }
        Node* r = extendBeepTree(S, v->right, traits, AC, gs, otherParent, extinct);
        if(traits.hasET() && r)
        {
            rightTime = S.getTopTime() + S.getTime(*r);
        }

        // Now create the new node
        if(hasID)
        {
            new_node = S.addNode(l, r, id->arg.i, name);
        }
        else
        {
            new_node =  S.addNode(l, r, name);
        }
        assert(new_node != 0);

        double edge_time = decideEdgeTime(v, traits, otherParent);

        if(traits.hasET())
        {
            if(r && l)
            {
                if ((2 * abs(leftTime - rightTime) / (leftTime + rightTime)) >= 0.01)
                {
                    ostringstream oss;
                    oss << "Tree time inconsistency at node  "
                        << new_node->getNumber()
                        <<"\nAccording to left subtree, node time is "
                       << leftTime
                       << " but right subtree says it should be "
                       << rightTime
                       << ".\n";
                    throw AnError("TreeIO::extendBeepTree: " +
                                  oss.str());
                }
            }
            S.setTime(*new_node, leftTime);
            S.setTopTime(edge_time);
        }
        //NOTE this is doing S.seTime again
        sanityCheckOnTimes(S, new_node, v, traits);

        // Check if any existing branchLength should be used
        if(traits.hasBL() || (traits.hasNW() && traits.hasNWisET() == false))
        {
            handleBranchLengths(new_node, v, traits.hasNWisET());
        }

        //Associate gene and species names
        if (l == 0 && r == 0 && gs != 0) // If this is a leaf and we want to read gs
        {
            if (speciesName(v) != 0)
            {
                gs->insert(name, string(speciesName(v)));
            }

        }

        // get antichain (gamma) info if requested
        if (AC != 0)
        {
            if(AC->empty()) // if elements is not allocated in AC do so!
            {
                AC->resize(100); // Warning arbitrary default size
            }
            updateACInfo(v, new_node, *AC);
        }

        if(find_annotation(v, "EX"))
        {
            if(extinct)
            {
                if(new_node->isLeaf() == false)
                {
                    throw AnError("TreeIO::extinct node must be a leaf",1);
                }
                (*extinct)[new_node] = 1;
            }
            else
            {
                throw AnError("TreeIO::extendBeepTree\n"
                              "Please use readHybridTree",1);
            }
        }
        return new_node;
    }
}

void TreeIO::sanityCheckOnTimes(TreeExtended &S, Node *node, const NHXnode *v, const TreeIOTraits& traits)
{
    // Check if any existing info about node time should be used
    // Note that we don't allow using both ET and NT
    if(traits.hasNT())
    {
        // check for sanity - we only need one time measure!
        if(traits.hasET())
        {
            throw AnError("Superfluous time measure, use either ET or NT, "
                          "but not both");
        }
        if(NHXannotation *a = find_annotation(v, "NT"))
        {
            S.setTime(*node, a->arg.t);
        }
        else
        {
            throw AnError("Edge without node time found in tree.", 1);
        }
    }

}

// Basic helper function for writing trees in PRIME format
std::string
TreeIO::recursivelyWriteBeepTree(Node &u,
                                 std::map<Node *, string> least,
                                 const TreeIOTraits& traits,
                                 const GammaMapEx* gamma,
                                 std::map<const Node*,Node*>* otherParent,
                                 std::map<const Node*,unsigned>* extinct,
                                 std::map<unsigned, unsigned>* id)
{
    assert((traits.hasID() && id != 0) == false);
    string ret;

    // Determine what should be tagged in PRIME markup
    std::ostringstream tagstr;
    std::ostringstream NWstr;

    if(traits.hasID())
    {
        tagstr << " ID=" << u.getNumber();
    }

    if (traits.hasET())
    {
        if (traits.hasNWisET())
        {
            NWstr << std::showpoint <<":" << u.getTime();
        }
        else
        {
            tagstr << " ET=" << std::showpoint << u.getTime();
        }
    }

    if (traits.hasNT())
    {
        tagstr << " NT=" << u.getNodeTime();
    }

    if (traits.hasBL())
    {
        if (traits.hasNWisET())
        {
            tagstr << " BL=" << u.getLength();
        }
        else
        {
            NWstr << ":" << u.getLength();
        }
    }

    // Now add node in newick format and gamma/AC if requested
    // This is done differently ifor leaves and internal nodes
    if (u.isLeaf())  // leaves recursion stops and 'S' is set
    {
        if(id != 0)
        {
            if(id->find(u.getNumber()) == id->end())
            {
                unsigned i = id->size();
                (*id)[u.getNumber()] = i;
            }
            tagstr << " ID=" << (*id)[u.getNumber()];
        }

        // then add gamma to PRIME markup if requested
        if(gamma)
        {
            Node *species = gamma->getLowestGammaPath(u);
            // check for consistency
            if(species)
            {
                // Add species info and AC
                tagstr << " S=" << species->getName()
                       << getAntiChainMarkup(u, *gamma);
            }
            else
            {
                ostringstream err;
                err << "Cannot write AC, since leaf "
                    << u.getNumber()
                    <<" lacks a species";
                throw AnError(err.str());
            }
        }
    }
    else   // Internal nodes needs to send recursion on and sets 'D'
    {
        // First get the strings for subtrees
        // Add node in newick format
        // Always order leaves in as alphabetical order as possible
        if(least[u.getLeftChild()] < least[u.getRightChild()])
        {
            string left_str = recursivelyWriteBeepTree(*u.getLeftChild(),
                                                       least, traits,
                                                       gamma, otherParent, extinct, id);
            string right_str = recursivelyWriteBeepTree(*u.getRightChild(),
                                                        least, traits,
                                                        gamma, otherParent, extinct, id);
            ret = "(" + left_str + ", " + right_str + ")";
        }
        else
        {
            string right_str = recursivelyWriteBeepTree(*u.getRightChild(),
                                                        least, traits,
                                                        gamma, otherParent, extinct, id);
            string left_str = recursivelyWriteBeepTree(*u.getLeftChild(),
                                                       least, traits,
                                                       gamma, otherParent, extinct, id);
            ret = "(" + right_str + ", " + left_str + ")";
        }

        if(id != 0)
        {
            if(id->find(u.getNumber()) == id->end())
            {
                unsigned i = id->size();
                (*id)[u.getNumber()] = i;
            }
            tagstr << " ID=" << (*id)[u.getNumber()];
        }

        ret.reserve(1024); 	// Avoid too many internal resize (expensive)


        // then add gamma/AC to BEEP markup if requested
        if(gamma)
        {
            if(gamma->isSpeciation(u))
            {
                tagstr << " D=0";
            }
            else
            {
                tagstr << " D=1";
            }
            tagstr << getAntiChainMarkup(u, *gamma);
        }
    }

    if(otherParent && otherParent->find(&u) != otherParent->end())
    {
        unsigned pn = u.getParent()->getNumber();
        unsigned opn = (*otherParent)[&u]->getNumber();
        if(id != 0)
        {
            if(id->find(pn) == id->end())
            {
                unsigned i = id->size();
                (*id)[pn] = i;
            }
            if(id->find(opn) == id->end())
            {
                unsigned i = id->size();
                (*id)[opn] = i;
            }
        
            tagstr << " HY=(" << (*id)[pn] << " " << (*id)[opn] << ")";
        }
    }

    if(extinct && extinct->find(&u) != extinct->end())
    {
        tagstr << " EX=1";
    }

    if(u.getName()!= "")
    {
        ret.append(u.getName());
    }

    ret.append(NWstr.str());

    if(tagstr.str() != "")
    {
        ret.append("[&&PRIME" + tagstr.str() + "]");
    }

    return ret;
}

void TreeIO::decideSubtreeOrder(Node &u, std::map<Node*, std::string> order)
{
    if(order.find(&u) != order.end())
    {
        return;
    }
    else if(u.isLeaf())
    {
        order[&u] = u.getName();
    }
    else
    {
        decideSubtreeOrder(*u.getLeftChild(), order);
        decideSubtreeOrder(*u.getRightChild(),order);
        order[&u] = min(order[u.getLeftChild()], order[u.getRightChild()]);
    }
    return;
}

std::string
TreeIO::recursivelyWriteBeepTree(Node &u, std::string& least,
                                 const TreeIOTraits& traits,
                                 const GammaMapEx *gamma,
                                 std::map<const Node*,Node*>* otherParent,
                                 std::map<const Node*,unsigned>* extinct,
                                 std::map<unsigned, unsigned>* id)
{
    assert((traits.hasID() && id) == false);
    string ret;

    // Determine what should be tagged in PRIME markup´
    std::ostringstream tagstr;
    std::ostringstream NWstr;

    if(traits.hasID())
    {
        tagstr << " ID=" << u.getNumber();
    }

    if (traits.hasET())
    {
        if (traits.hasNWisET())
        {
            NWstr << std::showpoint <<":" << u.getTime();
        }
        else
        {
            tagstr << " ET=" << std::showpoint << u.getTime();
        }
    }

    if (traits.hasNT())
    {
        tagstr << " NT=" << u.getNodeTime();
    }

    if (traits.hasBL())
    {
        if (traits.hasNWisET())
        {
            tagstr << " BL=" << u.getLength();
        }
        else
        {
            NWstr << ":" << u.getLength();
        }
    }

    // Now add node in newick format and gamma/AC if requested
    // This is done differently ifor leaves and internal nodes
    if (u.isLeaf())  // leaves recursion stops and 'S' is set
    {
        if(id)
        {
            if(id->find(u.getNumber()) == id->end())
            {
                unsigned i = id->size();
                (*id)[u.getNumber()] = i;
            }
            tagstr << " ID=" << (*id)[u.getNumber()];
        }

        // add node in newick format
        // 	ret = least = u.getName();
        least = u.getName();

        // then add gamma to PRIME markup if requested
        if(gamma)
        {
            Node *species = gamma->getLowestGammaPath(u);
            // check for consistency
            if(species)
            {
                // Add species info and AC
                tagstr << " S=" << species->getName()
                       << getAntiChainMarkup(u, *gamma);
            }
            else
            {
                ostringstream err;
                err << "Cannot write AC, since leaf "
                    << u.getNumber()
                    <<" lacks a species";
                throw AnError(err.str());
            }
        }
    }
    else   // Internal nodes needs to send recursion on and sets 'D'
    {
        // First get the strings for subtrees
        string least_left;
        string least_right;
        string left_str = recursivelyWriteBeepTree(*u.getLeftChild(),
                                                   least_left, traits,
                                                   gamma, otherParent, extinct, id);
        string right_str = recursivelyWriteBeepTree(*u.getRightChild(),
                                                    least_right, traits,
                                                    gamma, otherParent, extinct, id);
        if(id)
        {
            if(id->find(u.getNumber()) == id->end())
            {
                unsigned i = id->size();
                (*id)[u.getNumber()] = i;
            }
            tagstr << " ID=" << (*id)[u.getNumber()];
        }

        ret.reserve(1024); 	// Avoid too many internal resize (expensive)

        // Add node in newick format
        // Always order leaves in as alphabetical order as possible
        if (least_left < least_right)
        {
            least = least_left;
            ret = "(" + left_str + ", " + right_str + ")";
        }
        else
        {
            least = least_right;
            ret = "(" + right_str + ", " + left_str + ")";
        }

        // then add gamma/AC to BEEP markup if requested
        if(gamma)
        {
            if(gamma->isSpeciation(u))
            {
                tagstr << " D=0";
            }
            else
            {
                tagstr << " D=1";
            }
            tagstr << getAntiChainMarkup(u, *gamma);
        }
    }

    if(otherParent && otherParent->find(&u) != otherParent->end())
    {
        unsigned pn = u.getParent()->getNumber();
        unsigned opn = (*otherParent)[&u]->getNumber();

        if(id)
        {
            if(id->find(pn) == id->end())
            {
                unsigned i = id->size();
                (*id)[pn] = i;
            }
            if(id->find(opn) == id->end())
            {
                unsigned i = id->size();
                (*id)[opn] = i;
            }

            tagstr << " HY=(" << (*id)[pn] << " " << (*id)[opn] << ")";
        }

    }

    if(extinct && extinct->find(&u) != extinct->end())
    {
        tagstr << " EX=1";
    }

    if(u.getName()!= "")
    {
        ret.append(u.getName());
    }

    ret.append(NWstr.str());

    if(tagstr.str() != "")
    {
        ret.append("[&&PRIME" + tagstr.str() + "]");
    }

    return ret;
}

// Collect info for newickString
// Compute markup for the anti-chains on node u
std::string TreeIO::getAntiChainMarkup(Node &u, const GammaMapEx &gamma)
{
    std::string ac = "";

    if (gamma.numberOfGammaPaths(u) > 0)
    {
        Node *lower = gamma.getLowestGammaPath(u);
        Node *higher = gamma.getHighestGammaPath(u);

        do
        {
            char buf[5];
            if (snprintf(buf, 4, "%d", lower->getNumber()) == -1)
            {
                throw AnError("Too many anti-chains (more than 9999!) "
                              "or possibly a programming error.");
            }
            if (lower == higher)
            {
                ac.append(buf);	// Last element
            }
            else
            {
                ac.append(buf);
                ac.append(" ");
            }
            lower = lower->getParent();
        }
        while (lower && higher->dominates(*lower));
        ac = " AC=(" + ac + ")";
    }
    return ac;
}

void TreeIO::updateACInfo(const NHXnode *v, Node *new_node,
                          std::vector<SetOfNodesEx<Node> > &AC)
{
    NHXannotation *a = find_annotation(v, "AC");
    if (a != 0)
    {
        int_list *il = a->arg.il;
        while (il)
        {
            AC[il->i].insert(new_node);
            il = il->next;
        }
    }
}
