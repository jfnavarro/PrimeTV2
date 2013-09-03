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
             Bengt Sennblad © the MCMC-club, SBC, all rights reserved
 */

#ifndef TREEIO_HH
#define TREEIO_HH

// #include "Beep.hh"
#include "NHXtree.h"
#include "NHXnode.h"
#include "TreeIOTraits.hh"
#include "StrStrMap.hh"
#include <string>
#include <vector>
#include "SetOfNodesEx.hh"
#include <map>
#include "AnError.hh"
#include "gammamapex.h"
#include "NHXannotation.h"
#include "Node.hh"

// The size of a char buffer when reading gene-species info:
#define LINELENGTH 10000   //1024


class Node;
class Tree;

using namespace std;
//--------------------------------------------------------------------
//
// TreeIO
//
//! Provides methods for reading and writing phylo trees. 
//!
//! + When reading gene trees, any edge weight (time) annotations are 
//!   totally disregarded.
//! + For species trees, edge weights are honoured, and if they would 
//!   be missing, a negative time is inserted.
//
//--------------------------------------------------------------------
class TreeIO 
{
//! convenience alias 
enum TreeSource {notInitialized, readFromStdin, readFromFile, readFromString};

//! Only create TreeIO objects through *named constructors* (see C++ FAQ 
//! lite). Actual constructor is protected, and does actually not do 
//! anything! There is one exception: If you want to read from STDIN, 
//! use the empty constructor, TreeIO(). But you get the same behaviour
//! if the filename is empty.
//--------------------------------------------------------------------
protected:
TreeIO(enum TreeSource src, const std::string s);

public:
TreeIO();	    	//! "Empty" constructor, allows reading from STDIN.
virtual ~TreeIO();
TreeIO(const TreeIO &io);
virtual TreeIO& operator=(const TreeIO &io);

//! Usage: 
//!   TreeIO io = fromFile("Nisse"); /* read from file Nisse */
//!   TreeIO io = fromString("(a,(b,c));"); /* read from this string */
//!   TreeIO io = fromFile("");      /* read from STDIN  */
//!   TreeIO io;                     /* read from STDIN  */
//--------------------------------------------------------------------
static TreeIO fromFile(const std::string &filename);
static TreeIO fromString(const std::string &treeString);

//! Change source using these utilities:
//--------------------------------------------------------------------
void setSourceFile(const std::string &filename);
void setSourceString(const std::string &str); 

public:

//! Basic function for reading trees in PRIME format
//! ID and name of nodes are always read, reads everything there is, 
//! unless told otherwise by traits.
//! precondition: (useET && useNT) != true
//----------------------------------------------------------------------

template <class T,class U>
T readBeepTree(std::vector<SetOfNodesEx<U> > *AC, StrStrMap *gs)
{
    TreeIOTraits traits;
    checkTagsForTree(traits); // Also reads the tree, apparently!
    traits.enforceStandardSanity();
    return readBeepTree<T,U>(traits, AC, gs);
}

template <class T,class U>
T readBeepTree(const TreeIOTraits& tr, std::vector<SetOfNodesEx<U> > *AC,
            StrStrMap *gs)
{
    struct NHXtree* t = readTree();
    if (t == NULL) 
    {
    throw AnError("No tree found!");
    }

    return readBeepTree<T,U>(t, tr, AC, gs);
}

//! Convenience front to readBeepTree(...)
//! Reads times from NT, ET or NW and nothing more
//----------------------------------------------------------------------
template <class T,class U>
T readHostTree()
{
    TreeIOTraits traits;
    struct NHXtree *t = checkTagsForTree(traits);
    if(traits.containsTimeInformation() == false)
    {
    throw AnError("Host tree lacks time information for some of it nodes", 1);
    }

    traits.enforceHostTree();
    std::vector<SetOfNodesEx<U> > *AC = 0;
    StrStrMap *gs = 0;
    return readBeepTree<T,U>(t, traits, AC, gs);
}


//! Convenience front to readBeepTree(...)
//! Reads edge lengths from BL or NW and what else there is
//! Reads antichains info and gene species maps
//----------------------------------------------------------------------
template <class T,class U>
T readGuestTree(std::vector<SetOfNodesEx<U> >* AC, StrStrMap* gs)
{
    TreeIOTraits traits;
    struct NHXtree *t = checkTagsForTree(traits); 
    if(traits.hasGS() == false)
    {
        gs = 0;
    }
    if(traits.hasAC() == false)
    {
        AC = 0;
    }
    traits.enforceGuestTree();
    return readBeepTree<T,U>(t, traits, AC, gs);
}

//! Convenience front to readGuestTree(...)
//! Reads edge lengths from BL or NW and what else there is
//! Doese not read antichains info and gene species maps
//----------------------------------------------------------------------
template <class T,class U>
T readGuestTree()
{
    std::vector<SetOfNodesEx<U> > *AC = 0;
    StrStrMap *gs = 0;
    return readGuestTree<T,U>(AC, gs);
}

//! Convenience front to readBeepTree(...)
//! Reads a plain newick tree with branch lengths from NW only
//----------------------------------------------------------------------
template <class T,class U>
T readNewickTree()
{
    TreeIOTraits traits;
    struct NHXtree *t = checkTagsForTree(traits);
    traits.setET(false);
    traits.setNT(false);
    traits.setBL(traits.hasNW());
    traits.setNWisET(false);
    std::vector<SetOfNodesEx<U> > *AC = 0;
    StrStrMap *gs = 0;
    return readBeepTree<T,U>(t, traits, AC,gs);
}

//! Basic function for writing tree T in newick format, with the tags 
//! indicated by traits included in PRIME markup. If gamma != NULL then AC 
//! markup will also be included.
//! Precondition: (useET && useNT) == false
//----------------------------------------------------------------------
template <class T,class U>
static std::string writeBeepTree(const T& G, 
                    const TreeIOTraits& traits,
                    const GammaMapEx<U>* gamma)
{
    assert((traits.hasET() && traits.hasNT()) == false);
    std::string least = "";
    std::ostringstream name;

    if (traits.hasName()) 
    {
        name << "[&&PRIME NAME=" << G.getName();

        if(G.getRootNode() == NULL)
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

//! convenience front function for writeBeepTree(...) 
//! writes tree S with all attributes 
//----------------------------------------------------------------------
template <class T,class U>
static std::string writeBeepTree(const T& G, const U* gamma=0)
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

//! convenience front function for writeBeepTree(...) 
//! writes tree S with edge times
//----------------------------------------------------------------------
template <class T>
static std::string writeHostTree(const T& S)
{
    TreeIOTraits traits;
    traits.setID(true);
    if(S.hasTimes())
    {
        traits.setNT(true);
    }
    if(S.getName() != "")
    {
        traits.setName(true);
    }
    return writeBeepTree(S, traits, 0);
}

//! convenience front function for writeBeepTree(...) 
//! writes tree G with lengths and with gamma/AC info
//----------------------------------------------------------------------
template <class T,class U>
static std::string writeGuestTree(const T& G, const GammaMapEx<U>* gamma)
{
    TreeIOTraits traits;
    traits.setID(true);
    if(G.hasLengths())
    {
        traits.setBL(true);
    }
    return writeBeepTree(G, traits, gamma);
}

//! convenience front function for writeGeneTree(...) 
//! writes tree G with lkengths but without AC info
//----------------------------------------------------------------------
template <class T>
static std::string writeGuestTree(const T& G)
{
    return writeGuestTree(G, 0);
}

//! convenience front function for writeBeepTree(...) 
//! writes plain newick tree T with branch lengths
//----------------------------------------------------------------------
template <class T>
static std::string writeNewickTree(const T& G)
{
    TreeIOTraits traits;
    if(G.hasLengths())
    {
        traits.setBL(true);
        traits.setNWisET(false);
    }
    return writeBeepTree(G, traits, 0);
}

//! Map leaves in the gene tree to leaves in the species tree
//! \todo{This is a bit incongruent with the rest of the code and should 
//! probably get its own class! /arve}
//! \todo{ It should definitely have its name changed /bens}
//--------------------------------------------------------------------
static StrStrMap readGeneSpeciesInfo(const std::string& filename);
static std:: vector<StrStrMap> 
readGeneSpeciesInfoVector(const std::string& filename);

//! Precheck what tags are present in the read NHX-tree. Since ID,
//! Names of nodes and trees are always read - these are not checked
struct NHXtree* checkTagsForTree(TreeIOTraits &traits);

protected:
    
//! The basic function for reading NHX trees
//----------------------------------------------------------------------
template <class T,class U>
T readBeepTree(struct NHXtree *t, const TreeIOTraits& traits, 
            std::vector<SetOfNodesEx<U> > *AC, StrStrMap *gs)
{
    assert(t != 0);
    T tree;

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
    U *r = TreeIO::extendBeepTree<T,U>(tree, t->root, traits, AC, gs, 0, 0);
    if (r == NULL) 
    {
        throw AnError("The input tree was empty!");
    }

    if(struct NHXannotation *a = find_annotation(t->root, "NAME"))
    {
        std::string str = a->arg.str;
        tree.setName(str);
    }

    if(traits.hasNT())
    {
        if(struct NHXannotation *a = find_annotation(t->root, "TT"))
        {
            Real toptime = a->arg.t;
            tree.setTopTime(toptime);
        }
    }

    //Loose temp structure and hand the root in a good place.
    delete_trees(t);
    tree.setRootNode(r);

    if(tree.IDnumbersAreSane(*r) == false)
    {
        throw AnError("There are higher ID-numbers than there are nodes in tree", "TreeIO::readBeepTree");
    }
    return tree;
}

//! The basic recursion function for reading node info from NHX structs
//----------------------------------------------------------------------
template <class T,class U>
U* extendBeepTree(T &S, struct NHXnode *v, 
            const TreeIOTraits& traits,
            std::vector<SetOfNodesEx<U> > *AC, StrStrMap *gs,
            std::map<const U*, U*>* otherParent,
            std::map<const U*, unsigned>* extinct)
{
    if (v == 0) 
    {
        return 0;
    }
    else 
    {
        // First find out if node already exists
        //--------------------------------------
        U* new_node;
        struct NHXannotation* id = find_annotation(v, "ID");
        if(id)
        {
            new_node = static_cast<U*>(S.getNode(id->arg.i));

            // We must have ID to be able to give HY, which gives
            // the other parent of a hybrid child
            if(new_node)
            {
                struct NHXannotation* h = find_annotation(v, "HY");
                if(h)
                {
                    if(otherParent)
                    {
                        (*otherParent)[new_node] = static_cast<U*>(new_node->getParent());
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
        //-------------------------
        string name = decideNodeName(v);
        Real leftTime = 0;
        Real rightTime = 0;

        // Pass on recursion -- topTime is used to temporarily store 
        // the edgeTime of a Node, remember to record them in left/rightTime
        U* l = extendBeepTree(S, v->left, traits, AC, gs, otherParent, extinct);
        if(traits.hasET() && l)
        {
            leftTime = S.getTopTime() + S.getTime(*l);
        }
        U* r = extendBeepTree(S, v->right, traits, AC, gs, otherParent, extinct);
        if(traits.hasET() && r)
        {
            rightTime = S.getTopTime() + S.getTime(*r);
        }
        // Now create the new node
        if(id)
        {
            new_node = static_cast<U*>(S.addNode(l, r, id->arg.i, name));
        }
        else 
        {
            new_node =  static_cast<U*>(S.addNode(l, r, name)); 
        }
        assert(new_node != NULL);
            
        Real edge_time = decideEdgeTime(v, traits, otherParent);       
        if(traits.hasET())
        {
            if(r && l)
            {
                if((2 * abs(leftTime - rightTime) / (leftTime + rightTime)) >= 0.01)
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
            // There is a problem when setting time for a hybrid parent
            // when the other parent is no yet created -- AnError is thrown
            // so we catch it here
            try
            {
                S.setTime(*new_node, leftTime);
            }
            catch(AnError& e)
            {
                if(string(e.what()) != string("HybridTree::setTime():\n"
                                "op is NULL for hybridNode"))
                {
                    throw e;
                }
                assert(S.getTime(*new_node) == leftTime);
            }
            S.setTopTime(edge_time);
        }

        sanityCheckOnTimes(S, new_node, v, traits);
        
        // Check if any existing branchLength should be used
        //-------------------------------------------------------------------
        if(traits.hasBL() || (traits.hasNW() && traits.hasNWisET() == false))
        {
            handleBranchLengths(new_node, v, traits.hasNWisET());
        }

        //Associate gene and species names
        //-------------------------------------------------------------------
        if (l == 0 && r == 0 && gs != 0) // If this is a leaf and we want to read gs
        {
            if (speciesName(v) != 0) 
            {
                gs->insert(name, string(speciesName(v)));
            }
            else 
            {
            //! todo{ How should I handle the situation when we 
            //! want to test if a gs info is given in the guest tree?
            //! leave gs empty, as was done before, or provide a 
            //! special test function if a gs exists /bens}
            //  throw AnError("No species given for leaf!", name, 1);
            }
        }
        
        // get antichain (gamma) info if requested
        //-------------------------------------------------------------------
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
                    throw AnError("TreeIO::extinct node must be a leaf",1);
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

//! Handle the various rules for how to set the time over an edge
Real  decideEdgeTime(struct NHXnode *v, const TreeIOTraits& traits, bool isHY);
std::string decideNodeName(struct NHXnode *v);

template <class T, class U>
void sanityCheckOnTimes(T& S, U *node, struct NHXnode *v, const TreeIOTraits& traits)
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
        if(struct NHXannotation *a = find_annotation(v, "NT"))
        {
            // There is a problem when setting time for a hybrid parent
            // when the other parent is no yet created -- AnError is thrown
            // so we catch it here
            try
            {
                S.setTime(*node, a->arg.t);
            }
                catch(AnError& e)
                {
                    if(std::string(e.what()) != std::string("HybridTree::setTime():\n" "op is NULL for hybridNode"))
                {
                    throw e;
                }
                assert(S.getTime(*node) == a->arg.t);
            }
        }
        else
        {
            throw AnError("Edge without node time found in tree.", 1);
        }
    }

}

void handleBranchLengths(Node *node, struct NHXnode *v, bool NWIsET);

// Basic helper function for writing trees in PRIME format
    
static std::string 
recursivelyWriteBeepTree(Node &u, std::string& least, 
                const TreeIOTraits& traits,
                const GammaMapEx<Node>* gamma,
                std::map<const Node*,Node*>* otherParent,
                std::map<const Node*,unsigned>* extinct,
                std::map<unsigned, unsigned>* id)
{
    assert((traits.hasID() && id) == false);
    string ret;

    // Determine what should be tagged in PRIME markup
    //-------------------------------------------------------------------
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
    //-------------------------------------------------------------------
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
    
template <class U>
static void decideSubtreeOrder(U &u, std::map<U*, std::string> order)
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

template <class T,class U>
static std::string 
recursivelyWriteBeepTree(U& u, std::map<U*, std::string> least,
                const TreeIOTraits& traits,
                const T* gamma,
                std::map<const U*,U*>* otherParent,
                std::map<const U*,unsigned>* extinct,
                std::map<unsigned, unsigned>* id)
{
    assert((traits.hasID() && id) == false);
    string ret;

    // Determine what should be tagged in PRIME markup
    //-------------------------------------------------------------------
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
    //-------------------------------------------------------------------
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

        // then add gamma to PRIME markup if requested
        if(gamma)
        {
            U *species = gamma->getLowestGammaPath(u);
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
            (   *id)[opn] = i;
            }
        }
        tagstr << " HY=(" << (*id)[pn] << " " << (*id)[opn] << ")";
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


//! Collect info for newickString
//! Compute markup for the anti-chains on node u
//----------------------------------------------------------------------
template <class T,class U>
static std::string getAntiChainMarkup(T &u, const GammaMapEx<U> &gamma)
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

//! Recursively checks what tags are given for all nodes in subtree T_v
//! Precondition: All bool argument has proper values. Assume a specific
//! bool argument, 'A' has incoming value 'a', and the value for the 
//! current subtree is 'b', then on return, A = a && b, i.e., false if 
//! either a or b is false.
//! postcondition: return statement is true if v != 0
//----------------------------------------------------------------------
bool recursivelyCheckTags(struct NHXnode* v, TreeIOTraits& traits);

//! Checks what tags are given for node v
//----------------------------------------------------------------------
void checkTags(struct NHXnode& v, TreeIOTraits& traits);

//! Helper function that reads NHXtrees:
//----------------------------------------------------------------------
NHXtree* readTree();

//! \todo{add comments on what this do /bens}
//----------------------------------------------------------------------
template<class U>
void updateACInfo(struct NHXnode *v, U *new_node, 
            std::vector<SetOfNodesEx<U> > &AC)
    {
    struct NHXannotation *a = find_annotation(v, "AC");
    if (a != NULL) 
    {
        struct int_list *il = a->arg.il;
        while (il) 
        {
            AC[il->i].insert(new_node);
            il = il->next;
        }
    }
    }  

protected:

enum TreeSource source; //!< Where do we read trees from?
std::string stringThatWasPreviouslyNamedS;          //!< filename of current file to read from
static std::string antiChainMarkupTag; //!< \todo{is this used? /bens 

};
  
#endif