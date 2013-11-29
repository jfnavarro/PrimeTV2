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
             Lars Arvestad, the MCMC-club, SBC, all rights reserved
             Bengt Sennblad the MCMC-club, SBC, all rights reserved
 */

#ifndef TREEIO_HH
#define TREEIO_HH

#include "../parser/NHXtree.h"
#include "../parser/NHXnode.h"
#include "../parser/NHXannotation.h"
#include "../reconcilation/StrStrMap.h"
#include "../reconcilation/SetOfNodesEx.h"
#include "../reconcilation/GammaMapEx.h"
#include "../utils/AnError.h"
#include "TreeIOTraits.h"
#include "Node.h"

#include <map>
#include <string>
#include <vector>

// The size of a char buffer when reading gene-species info:
#define LINELENGTH 10000   //1024

class Tree;

using namespace std;

// Provides methods for reading and writing phylo trees.
//
// + When reading gene trees, any edge weight (time) annotations are
//   totally disregarded.
// + For species trees, edge weights are honoured, and if they would
//   be missing, a negative time is inserted.

class TreeIO 
{
    enum TreeSource {
        notInitialized = 0x01,
        readFromStdin = 0x02,
        readFromFile = 0x04,
        readFromString = 0x08
    };

public:

    TreeIO();
    TreeIO(enum TreeSource src, const std::string &s);
    virtual ~TreeIO();

    // static functions to read trees
    static TreeIO fromFile(const std::string &filename);
    static TreeIO fromString(const std::string &treeString);
    //static functions to read map files
    static StrStrMap readGeneSpeciesInfo(const std::string& filename);
    static std::vector<StrStrMap> readGeneSpeciesInfoVector(const std::string& filename);

    // Change source using these utilities:
    void setSourceFile(const std::string &filename);
    void setSourceString(const std::string &str);

    // Precheck what tags are present in the read NHX-tree. Since ID,
    // Names of nodes and trees are always read - these are not checked
    void checkTagsForTree(TreeIOTraits &traits);

    // Convenience front to readBeepTree(...)
    // Reads times from NT, ET or NW and nothing more
    TreeExtended readHostTree();
    
    // Convenience front to readBeepTree(...)
    // Reads edge lengths from BL or NW and what else there is
    // Reads antichains info and gene species maps
    TreeExtended readGuestTree(std::vector<SetOfNodesEx<Node> >* AC, StrStrMap* gs);
    
    // Convenience front to readGuestTree(...)
    // Reads edge lengths from BL or NW and what else there is
    // Doese not read antichains info and gene species maps
    TreeExtended readGuestTree();
    TreeExtended readBeepTree(std::vector<SetOfNodesEx<Node> > *AC, StrStrMap *gs);
    TreeExtended readBeepTree(const TreeIOTraits& tr,
                              std::vector<SetOfNodesEx<Node> > *AC,
                              StrStrMap *gs);

    // Convenience front to readBeepTree(...)
    // Reads a plain newick tree with branch lengths from NW only
    TreeExtended readNewickTree();

    // Basic function for writing tree TreeExtended in newick format, with the tags
    // indicated by traits included in PRIME markup. If gamma != 0 then AC
    // markup will also be included.
    // Precondition: (useET && useNT) == false
    std::string writeBeepTree(const TreeExtended &G, const TreeIOTraits& traits, const GammaMapEx *gamma);

    // convenience front function for writeBeepTree(...)
    // writes tree S with all attributes
    std::string writeBeepTree(const TreeExtended& G, const GammaMapEx* gamma=0);

    // convenience front function for writeBeepTree(...)
    // writes tree G with lengths and with gamma/AC info
    std::string writeGuestTree(const TreeExtended& G, const GammaMapEx* gamma);

    // convenience front function for writeGeneTree(...)
    // writes tree G with lkengths but without AC info
    inline std::string writeGuestTree(const TreeExtended& G) { return writeGuestTree(G, 0); }

protected:
    
    // Handle the various rules for how to set the time over an edge
    double decideEdgeTime(const NHXnode *v, const TreeIOTraits& traits, bool isHY) const;

    std::string decideNodeName(const NHXnode *v);

    void handleBranchLengths(Node *node, const NHXnode *v, bool NWIsET);

    // Recursively checks what tags are given for all nodes in subtree T_v
    // Precondition: All bool argument has proper values. Assume a specific
    // bool argument, 'A' has incoming value 'a', and the value for the
    // current subtree is 'b', then on return, A = a && b, i.e., false if
    // either a or b is false.
    // postcondition: return statement is true if v != 0
    bool recursivelyCheckTags(const NHXnode *v, TreeIOTraits& traits);

    // Checks what tags are given for node v
    void checkTags(const NHXnode *v, TreeIOTraits& traits) const;

    // Helper function that reads NHXtrees:
    NHXtree* readTree();

    TreeExtended readBeepTree(NHXtree *t, const TreeIOTraits& traits,
                   std::vector<SetOfNodesEx<Node> > *AC, StrStrMap *gs);

    Node* extendBeepTree(TreeExtended &S, const NHXnode *v,
                      const TreeIOTraits& traits,
                      std::vector<SetOfNodesEx<Node> > *AC, StrStrMap *gs,
                      std::map<const Node*, Node*>* otherParent,
                      std::map<const Node*, unsigned>* extinct);

    void sanityCheckOnTimes(TreeExtended &S, Node *node, const NHXnode *v,
                            const TreeIOTraits& traits);

    std::string
    recursivelyWriteBeepTree(Node &u,
                             std::map<Node*, std::string> least,
                             const TreeIOTraits& traits,
                             const GammaMapEx* gamma,
                             std::map<const Node*,Node*>* otherParent,
                             std::map<const Node*,unsigned>* extinct,
                             std::map<unsigned, unsigned>* id);

    std::string
    recursivelyWriteBeepTree(Node &u, std::string& least,
                             const TreeIOTraits& traits,
                             const GammaMapEx *gamma,
                             std::map<const Node*,Node*>* otherParent,
                             std::map<const Node*,unsigned>* extinct,
                             std::map<unsigned, unsigned>* id);

    void decideSubtreeOrder(Node &u, std::map<Node*, std::string> order);

    std::string getAntiChainMarkup(Node &u, const GammaMapEx &gamma);

    void updateACInfo(const NHXnode *v, Node *new_node,
                      std::vector<SetOfNodesEx<Node> > &AC);

private:

    enum TreeSource source; // Where do we read trees from?
    std::string stringThatWasPreviouslyNamedS;  //filename of current file to read from

};

#endif
