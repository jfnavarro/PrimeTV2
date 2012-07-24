#include "TreeIO.hh"
#include "AnError.hh"
#include "NHXannotation.h"
#include "Node.hh"
#include "StrStrMap.hh"
#include "NHXtree.h"
#include <cassert>		// For early bug detection
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>

// Author: Lars Arvestad, Bengt Sennblad, © the MCMC-club, SBC, all rights reserved

 using namespace std;
  
 TreeIO::TreeIO(enum TreeSource source, const std::string s)
    : source(source),
      stringThatWasPreviouslyNamedS(s)
  {}

  // Update: Since I sometimes want to output trees without reading 
  // anything, I will now allow instantiating the empty object.
  //--------------------------------------------------------------------
  TreeIO::TreeIO()
    : source(readFromStdin),
      stringThatWasPreviouslyNamedS("")
  {}

  TreeIO::~TreeIO()
  {}

  TreeIO::TreeIO(const TreeIO &io)
    : source(io.source),
      stringThatWasPreviouslyNamedS(io.stringThatWasPreviouslyNamedS)
  {}

  TreeIO&
  TreeIO::operator=(const TreeIO &io)
  {
    if (this != &io)
      {
	stringThatWasPreviouslyNamedS = io.stringThatWasPreviouslyNamedS;
	source = io.source;
      }
    return *this;
  }

  TreeIO
  TreeIO::fromFile(const std::string &f)
  {
    if (f=="") 
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
  //--------------------------------------------------------------------
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


  //----------------------------------------------------------------------
  //
  // Interface
  //
  //----------------------------------------------------------------------
 
  //----------------------------------------------------------------------
  // Reading trees:
  //----------------------------------------------------------------------

  // Basic function for reading trees in BEEP format
  //----------------------------------------------------------------------
 
//   template <class T, class U>
//   T TreeIO::readBeepTree(std::vector<SetOfNodesEx> *AC, StrStrMap *gs)
//   {
//     TreeIOTraits traits;
//     checkTagsForTree(traits); // Also reads the tree, apparently!
//     traits.enforceStandardSanity();
//     return readBeepTree<T,U>(traits, AC, gs);
//   }
 
//   template <class T, class U>
//   T TreeIO::readBeepTree(const TreeIOTraits& tr,
// 			 std::vector<SetOfNodesEx> *AC, 
// 			 StrStrMap *gs)
//   {
//     struct NHXtree* t = readTree();
//     if (t == NULL) 
//       {
// 	throw AnError("No tree found!");
//       }
//     
//     return readBeepTree<T,U>(t, tr, AC, gs);
//   }

  // Convenience front to readBeepTree(...)
  // Reads times and what else there is
  //----------------------------------------------------------------------
//   template <class T,class U>
//   T TreeIO::readHostTree()
//   {
//     TreeIOTraits traits;
//     struct NHXtree *t = checkTagsForTree(traits);
//     if(traits.containsTimeInformation() == false)
//       {
// 	throw AnError("Host tree lacks time information for some of it nodes", 1);
//       }
// 
//     traits.enforceHostTree();
//     std::vector<SetOfNodesEx> *AC = 0;
//     StrStrMap *gs = 0;
//     return readBeepTree<T,U>(t, traits, AC, gs);
//   }


  // Convenience front to readBeepTree(...)
  // Reads 'NW tags' as edge lengths and what else there is
  // Reads antichains info and gene species maps
  //----------------------------------------------------------------------
//   template <class T,class U>
//   T TreeIO::readGuestTree(std::vector<SetOfNodesEx>* AC, StrStrMap* gs)
//   {
//     TreeIOTraits traits;
//     struct NHXtree *t = checkTagsForTree(traits); 
//     if(traits.hasGS() == false)
//       {
// 	gs = 0;
//       }
//     if(traits.hasAC() == false)
//       {
// 	AC = 0;
//       }
//     traits.enforceGuestTree();
//     return readBeepTree<T,U>(t, traits, AC, gs);
//   }

  // Convenience front to readGuestTree(...)
  // Reads 'NW tags' as edge lengths and what else there is
  // Doese not read antichains info and gene species maps
  //----------------------------------------------------------------------
//   template <class T,class U>
//   T TreeIO::readGuestTree()
//   {
//     std::vector<SetOfNodesEx> *AC = 0;
//     StrStrMap *gs = 0;
//     return readGuestTree<T,U>(AC, gs);
//   }

  // Convenience front to readBeepTree(...)
  // Reads a plain newick tree with branch lengths from NW
  //----------------------------------------------------------------------
//   template <class T,class U>
//   T TreeIO::readNewickTree()
//   {
//     TreeIOTraits traits;
//     struct NHXtree *t = checkTagsForTree(traits);
//     traits.setET(false);
//     traits.setNT(false);
//     traits.setBL(traits.hasNW());
//     traits.setNWisET(false);
//     std::vector<SetOfNodesEx> *AC = 0;
//     StrStrMap *gs = 0;
//     return readBeepTree<T,U>(t, traits, AC,gs);
//   }



  //----------------------------------------------------------------------
  // Writing trees
  //----------------------------------------------------------------------

  // Basic function for writing tree T in newick format, with the tags 
  // indicated by traits included in PRIME markup. If gamma != NULL then AC 
  // markup will also be included.
  // Precondition: (useET && useNT) == false
  //----------------------------------------------------------------------
//   template <class T,class U>
//   std::string 
//   TreeIO::writeBeepTree(const T& G, 
// 			const TreeIOTraits& traits,
// 			const U* gamma)
//   {
//     assert((traits.hasET() && traits.hasNT()) == false);
//     std::string least = "";
//     std::ostringstream name;
// 
//     if (traits.hasName()) {
//       name << "[&&PRIME NAME=" << G.getName();
// 
//       if(G.getRootNode() == NULL)
// 	{
// 	  name << "] [empty tree!]"; 
// 	  return name.str();
// 	}
//       else
// 	{
// 	  if(traits.hasNT())
// 	    {
// 	      name << " TT=" << G.getTopTime();
// 	    }
// 	  name << "]";
// 	}
//     }
//     return recursivelyWriteBeepTree(*G.getRootNode(), least, traits,
// 				    gamma, 0, 0, 0) + name.str();
//   }

  // convenience front function for writeBeeTree(...) 
  // writes tree T with all attributes there is
  //----------------------------------------------------------------------
//   template <class T,class U>
//   std::string 
//   TreeIO::writeBeepTree(const T& G, const U* gamma)
//   {
//     TreeIOTraits traits;
//     traits.setID(true);
//     if(G.hasTimes())
//       {
// 	traits.setNT(true);
//       }
//     if(G.hasLengths())
//       {
// 	traits.setBL(true);
//       }
//     return writeBeepTree(G, traits, gamma);
//   }
    
  // convenience front function for writeBeeTree(...) 
  // writes tree S with node times as NW
  //----------------------------------------------------------------------
//   template <class T>
//   std::string 
//   TreeIO::writeHostTree(const T& S)
//   {
//     TreeIOTraits traits;
//     traits.setID(true);
//     if(S.hasTimes())
//       {
// 	traits.setNT(true);
//       }
//     if(S.getName() != "")
//       {
// 	traits.setName(true);
//       }
//     return writeBeepTree(S, traits, 0);
//   }
    
  // convenience front function for writeBeepTree(...) 
  // writes tree G with lengths and with gamma/AC info
  //----------------------------------------------------------------------
//   template <class T,class U>
//   std::string 
//   TreeIO::writeGuestTree(const T& G, const U* gamma)
//   {
//     TreeIOTraits traits;
//     traits.setID(true);
//     if(G.hasLengths())
//       {
// 	traits.setBL(true);
//       }
//     return writeBeepTree(G, traits, gamma);
//   }
    
  // convenience front function for writeGeneTree(...) 
  // writes tree G with lengths without AC info
  //----------------------------------------------------------------------
//   template <class T>
//   std::string 
//   TreeIO::writeGuestTree(const T& G)
//   {
//     return writeGuestTree(G, 0);
//   }
    
  // convenience front function for writeBeepTree(...) 
  // writes plain newick tree T with branch lengths
  //----------------------------------------------------------------------
//   template <class T>
//   std::string 
//   TreeIO::writeNewickTree(const T& G)
//   {
//     TreeIOTraits traits;
//     if(G.hasLengths())
//       {
// 	traits.setBL(true);
// 	traits.setNWisET(false);
//       }
//     return writeBeepTree(G, traits, 0);
//   }
    
  //----------------------------------------------------------------------
  // Read associations
  //----------------------------------------------------------------------

  // Map leaves in the gene tree to leaves in the species tree
  // This is a bit incongruent with the rest of the code and should 
  // probably get its own class! /arve
  // Expected line format: 
  // <whitespace>? <gene name> <whitespace> <species name> <whitespace>?
  // Line length is at most 1024 chars.
  //--------------------------------------------------------------------
  StrStrMap
  TreeIO::readGeneSpeciesInfo(const std::string &filename)
  {
    ifstream is(filename.c_str());

    // This yields warning that line is unused TODO: remove /bens
    //     char line[LINELENGTH];
    int lineno = 1;

    StrStrMap gene2species;
    if(is.peek() == '#') // gs may start with a '#'
      {
	char dummy[LINELENGTH];
	is.getline(dummy, LINELENGTH);// >> dummy;
      }
    while (is.good()) 
      {
	std::string gene;
	std::string species;
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

    // This yields warning that line is unused TODO: remove /bens
    //     char line[LINELENGTH];
    int lineno = 1;

    std::vector<StrStrMap> gene2speciesVec;
    StrStrMap gene2species;
    std::string gsmark;
    is >> gsmark;
    if(gsmark != "#")
      {
	std::cerr << "error in gs vector, every gs must be preceeded by '#' line\n";
	exit(1);
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


  //----------------------------------------------------------------------
  // Check trees
  //----------------------------------------------------------------------
  
  // READ THE TREE, then
  // precheck what tags are present in the read NHX-tree. Since ID,
  // names of nodes and trees are always read - these are not checked
  //----------------------------------------------------------------------
  struct NHXtree *
  TreeIO::checkTagsForTree(TreeIOTraits& traits)
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
    struct NHXtree* t = readTree();
    struct NHXtree* ct = t;
    if (ct == 0) 
      {
	throw AnError("The input tree is NULL!",
		      "TreeIO::checkTagsForTree()",
		      1);
      }
    while(ct)
      {
	if(TreeIO::recursivelyCheckTags(ct->root, traits) == false)
	  {
	    throw AnError("The input tree was empty!",
			  "TreeIO::checkTagsForTree()",
			  1);
	  }
	ct= ct->next;
      }

    return t;
  }
    
  //----------------------------------------------------------------------
  //
  // Implementation
  //
  //----------------------------------------------------------------------
  
  // The basic recursion function for reading NHX trees
  //----------------------------------------------------------------------
  // T = class Tree, U = class node,
//   template <class T,class U>
//   T TreeIO::readBeepTree(struct NHXtree *t,
// 		       const TreeIOTraits& traits,
// 		       std::vector<SetOfNodesEx> *AC, 
// 		       StrStrMap *gs)
//   {
//     assert(t != 0);
//     T tree;
// 
//     // Create BeepVectors to hold required 'tag' info
//     if(traits.hasET() || traits.hasNT() || 
//        (traits.hasNW() && traits.hasNWisET()))
//       {
// 	tree.setTimes(*new RealVector(treeSize(t)), true);
//       }
//     if(traits.hasBL()|| (traits.hasNW() && traits.hasNWisET() == false))
//       {
// 	tree.setLengths(*new RealVector(treeSize(t)), true);
//       }
//     
//     // Convert it into our preferred C++ data structure
//     U *r = TreeIO::extendBeepTree<T,U>(tree, t->root, traits, AC, gs, 0, 0);
//     if (r == NULL) 
//       {
// 	throw AnError("The input tree was empty!");
//       }
// 
//     if(struct NHXannotation *a = find_annotation(t->root, "NAME"))
//       {
// 	std::string str = a->arg.str;
// 	tree.setName(str);
//       }
// 
//     if(traits.hasNT())
//       {
// 	if(struct NHXannotation *a = find_annotation(t->root, "TT"))
// 	  {
// 	    Real toptime = a->arg.t;
// 	    tree.setTopTime(toptime);
// 	  }
//       }
// 
//     // Loose temp structure and hand the root in a good place.
//     delete_trees(t);
//     tree.setRootNode(r);
//     
//     if(tree.IDnumbersAreSane(*r) == false)
//       {
// 	throw AnError("There are higher ID-numbers than there are nodes in tree", "TreeIO::readBeepTree");
//       }
//     return tree;
//   }
  // The basic recursion function for reading node info from NHX structs
  //----------------------------------------------------------------------
//   template <class T, class U>
//   U*
//   TreeIO::extendBeepTree(T &S, struct NHXnode *v,
// 			 const TreeIOTraits& traits,
// 			 std::vector<SetOfNodesEx> *AC, 
// 			 StrStrMap *gs,
// 			 map<const U*, U*>* otherParent,
// 			 map<const U*, unsigned>* extinct)
//   {
//     if (v == 0) 
//       {
// 	return 0;
//       }
//     else 
//       {
// 	// First find out if node already exists
// 	//--------------------------------------
// 	U* new_node;
// 	struct NHXannotation* id = find_annotation(v, "ID");
// 	if(id)
// 	  {
// 	    new_node = S.getNode(id->arg.i);
// 
// 	    // We must have ID to be able to give HY, which gives
// 	    // the other parent of a hybrid child
// 	    if(new_node)
// 	      {
// 		struct NHXannotation* h = find_annotation(v, "HY");
// 		if(h)
// 		  {
// 		    if(otherParent)
// 		      {
// 			(*otherParent)[new_node] = new_node->getParent();
// 			S.setTopTime(new_node->getTime());
// 			return new_node;
// 		      }
// 		    else
// 		      {
// 			throw AnError("This is a HybridTree. Please use "
// 				      "readHybridTree instead",
// 				      "TreeIO::extendBeepTree",
// 				      1);
// 		      }
// 		  }
// 		else
// 		  {
// 		    ostringstream oss;
// 		    oss << "TreeIO::extendBeepTree\n"
// 			<< "Found duplicate ID for non-hybrid node "
// 			<< id->arg.i << endl;
// 		    throw AnError(oss.str(),1);
// 		  }
// 	      }
// 	  }
// 
// 
// 	//Otherwise create new node
// 	//-------------------------
// 	string name = decideNodeName(v);
// 	Real leftTime = 0;
// 	Real rightTime = 0;
// 
// 	// Pass on recursion -- topTime is used to temporarily store 
// 	// the edgeTime of a Node, remember to record them in left/rightTime
// 	U* l = extendBeepTree(S, v->left, traits, AC, gs, otherParent, extinct);
// 	if(traits.hasET() && l)
// 	  {
// 	    leftTime = S.getTopTime() + S.getTime(*l);
// 	  }
// 	U* r = extendBeepTree(S, v->right, traits, AC, gs, otherParent, extinct);
// 	if(traits.hasET() && r)
// 	  {
// 	    rightTime = S.getTopTime() + S.getTime(*r);
// 	  }
// 	// Now create the new node
// 	if(id)
// 	  {
// 	    new_node = static_cast<U*>(S.addNode(l, r, id->arg.i, name));
// 	  }
// 	else 
// 	  {
// 	    new_node =  static_cast<U*>(S.addNode(l, r, name)); 
// 	  }
// 	assert(new_node != NULL);
// 	    
// 	Real edge_time = decideEdgeTime(v, traits, otherParent);       
// 	if(traits.hasET())
// 	  {
// 	    if(r && l)
// 	      {
// 		if((2 * abs(leftTime - rightTime) / (leftTime + rightTime)) >= 0.01)
// 		  {
// 		    ostringstream oss;
// 		    oss << "Tree time inconsistency at node  "
// 			<< new_node->getNumber() 
// 			<<"\nAccording to left subtree, node time is "
// 			<< leftTime
// 			<< " but right subtree says it should be "
// 			<< rightTime
// 			<< ".\n";
// 		    throw AnError("TreeIO::extendBeepTree: " +
// 				  indentString(oss.str()));
// 		  } 
// 	      }
// 	    // There is a problem when setting time for a hybrid parent
// 	    // when the other parent is no yet created -- AnError is thrown
// 	    // so we catch it here
// 	    try
// 	      {
// 		S.setTime(*new_node, leftTime);
// 	      }
// 	    catch(AnError& e)
// 	      {
// 		if(string(e.what()) != string("HybridTree::setTime():\n"
// 					      "op is NULL for hybridNode"))
// 		  {
// 		    throw e;
// 		  }
// 		assert(S.getTime(*new_node) == leftTime);
// 	      }
// 	    S.setTopTime(edge_time);
// 	  }
// 
// 	sanityCheckOnTimes(S, new_node, v, traits);
// 	
// 	// Check if any existing branchLength should be used
// 	//-------------------------------------------------------------------
// 	if(traits.hasBL() || (traits.hasNW() && traits.hasNWisET() == false))
// 	  {
// 	    handleBranchLengths(new_node, v, traits.hasNWisET());
// 	  }
// 
// 	//Associate gene and species names
// 	//-------------------------------------------------------------------
// 	if (l == 0 && r == 0 && gs != 0) // If this is a leaf and we want to read gs
// 	  {
// 	    if (speciesName(v) != 0) 
// 	      {
// 		gs->insert(name, string(speciesName(v)));
// 	      }
// 	    else 
// 	      {
// 		//! todo{ How should I handle the situation when we 
// 		//! want to test if a gs info is given in the guest tree?
// 		//! leave gs empty, as was done before, or provide a 
// 		//! special test function if a gs exists /bens}
// // 	        throw AnError("No species given for leaf!", name, 1);
// 	      }
// 	  }
// 	
// 	// get antichain (gamma) info if requested
// 	//-------------------------------------------------------------------
// 	if (AC != 0)
// 	  {
// 	    if(AC->empty()) // if elements is not allocated in AC do so!
// 	      {
// 		AC->resize(100); // Warning arbitrary default size
// 	      }
// 	    updateACInfo(v, new_node, *AC);
// 	  }
// 	if(find_annotation(v, "EX"))
// 	  {
// 	    if(extinct)
// 	      {
// 		if(new_node->isLeaf() == false)
// 		  throw AnError("TreeIO::extinct node must be a leaf",1);
// 		(*extinct)[new_node] = 1;
// 	      }
// 	    else
// 	      {
// 		throw AnError("TreeIO::extendBeepTree\n"
// 			      "Please use readHybridTree",1);
// 	      }
// 	  }
// 	return new_node;
//       }
//   }

  // Collect info for newickString
  // Compute markup for the anti-chains on node u
  //----------------------------------------------------------------------
//   template <class T,class U>
//   std::string
//   TreeIO::getAntiChainMarkup(T &u, const U &gamma)
//   {
//     std::string ac = "";
// 
//     if (gamma.numberOfGammaPaths(u) > 0)
//       {
// 	Node *lower = gamma.getLowestGammaPath(u);
// 	Node *higher = gamma.getHighestGammaPath(u);
// 
// 	do 
// 	  {
// 	    char buf[5];
// 	    if (snprintf(buf, 4, "%d", lower->getNumber()) == -1) 
// 	      {
// 		throw AnError("Too many anti-chains (more than 9999!) "
// 			    "or possibly a programming error.");
// 	      }
// 	    if (lower == higher)
// 	      {
// 		ac.append(buf);	// Last element
// 	      }
// 	    else 
// 	      {
// 		ac.append(buf);
// 		ac.append(" ");
// 	      }
// 	    lower = lower->getParent();
// 	  } 
// 	while (lower && higher->dominates(*lower)); 
// 	ac = " AC=(" + ac + ")";
//       }
//     return ac;
//   }


  // Find the right value for edge time
  Real
  TreeIO::decideEdgeTime(struct NHXnode *v, const TreeIOTraits& traits,
			 bool isHY)
  {
    Real edge_time = 0.0;

    if(traits.hasET()) // Use edge time info from file
      {
	if(traits.hasNWisET())
	  {
	    if(struct NHXannotation* a = find_annotation(v, "NW"))
	      {
		edge_time = a->arg.t;
	      }
	    else if (isRoot(v)) 
	      {
		edge_time = 0.0;
	      } 
	    else
	      {
		throw AnError("Edge without edge time found in tree.", 
			      1);
	      }
	  }
	else if(struct NHXannotation *a = find_annotation(v, "ET"))
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
	// Check for sanity
	if(edge_time <= 0)
	  {
	    if(edge_time < 0)
	      {
		throw AnError("Tree contains an edge with negative time",1);
	      }
	    else if(isHY == false && !isRoot(v)) 
	      {
		throw AnError("Tree contains an edge with zero time.", 1);
	      }
	  }
      }
    
    return edge_time;
  }

  // Always include name , if it exists
  std::string
  TreeIO::decideNodeName(struct NHXnode *v) 
  {
    std::string name = "";		// Default name is empty
    if (v->name)		// Leaf names are always inserted into 'name'
      {
	name = v->name;
      }
    else			// Inner nodes might have a name. Go check
      {
	struct NHXannotation *a = find_annotation(v, "S");
	if (a)			// Great, we found it
	  {
	    name = a->arg.str;	// Pick string from union 'arg'
	  }
      }
    
    return name;
  }

  // Sanity check for edge times
//   template <class T, class U>
//   void
//   TreeIO::sanityCheckOnTimes(T& S, U *node, struct NHXnode *v, 
// 			     const TreeIOTraits& traits)
//   {
//     // Check if any existing info about node time should be used
//     // Note that we don't allow using both ET and NT
//     if(traits.hasNT())
//       {
// 	// check for sanity - we only need one time measure!
// 	if(traits.hasET())
// 	  {
// 	    throw AnError("Superfluous time measure, use either ET or NT, "
// 			  "but not both");
// 	  }
// 	if(struct NHXannotation *a = find_annotation(v, "NT"))
// 	  {
// 	    // There is a problem when setting time for a hybrid parent
// 	    // when the other parent is no yet created -- AnError is thrown
// 	    // so we catch it here
// 	    try
// 	      {
// 		S.setTime(*node, a->arg.t);
// 	      }
// 	    catch(AnError& e)
// 	      {
// 		if(std::string(e.what()) != std::string("HybridTree::setTime():\n"
// 					      "op is NULL for hybridNode"))
// 		  {
// 		    throw e;
// 		  }
// 		assert(S.getTime(*node) == a->arg.t);
// 	      }
// 	  }
// 	else
// 	  {
// 	    throw AnError("Edge without node time found in tree.", 1);
// 	  }
//       }
//     
//   }


  void
  TreeIO::handleBranchLengths(Node *node, struct NHXnode *v, bool NWIsET)
  {
    if(struct NHXannotation *a = find_annotation(v, "BL")) 
      {
	node->setLength(a->arg.t);
      }
    else if(NWIsET)
      {
	throw AnError("TreeIO::extendBeepTree(...):\n"
		      "No branch length info found either in 'BL' and 'NW' is used for 'ET'",
		      234);
      }
    else if(struct NHXannotation *a = find_annotation(v, "NW"))  
      {           // use info in 'NW'
	node->setLength(a->arg.t);
      }
    else if (v->parent)		// If not root...
      {
	throw AnError("TreeIO::extendBeepTree(...):\n"
		      "No branch length info found either in 'BL' or 'NW'",
		      234);
      }
  }

  // Basic helper function for writing trees in BEEP format
//   template <class T,class U>
//   std::string
//   TreeIO::recursivelyWriteBeepTree(U& u, std::string& least,
// 				   const TreeIOTraits& traits,
// 				   const T* gamma,
// 				   map<const U*,U*>* otherParent,
// 				   map<const U*,unsigned>* extinct,
// 				   map<unsigned, unsigned>* id)
//   {
//     assert((traits.hasID() && id) == false);
//     string ret;
// 
//     // Determine what should be tagged in PRIME markup
//     //-------------------------------------------------------------------
//     std::ostringstream tagstr;
//     std::ostringstream NWstr;
// 
//     if(traits.hasID())
//       {
// 	tagstr << " ID=" << u.getNumber();
//       }
// 
//     if (traits.hasET())
//       {
// 	if (traits.hasNWisET())
// 	  {
// 	    NWstr << std::showpoint <<":" << u.getTime();
// 	  }
// 	else
// 	  {
// 	    tagstr << " ET=" << std::showpoint << u.getTime();
// 	  }
//       }
//     
//     if (traits.hasNT())
//       {
// 	tagstr << " NT=" << u.getNodeTime();
//       }
// 
//     if (traits.hasBL())
//       {
// 	if (traits.hasNWisET())
// 	  {
// 	    tagstr << " BL=" << u.getLength();
// 	  }
// 	else
// 	  {
// 	    NWstr << ":" << u.getLength();
// 	  }
//       }
// 
//     // Now add node in newick format and gamma/AC if requested
//     // This is done differently ifor leaves and internal nodes
//     //-------------------------------------------------------------------
//     if (u.isLeaf())  // leaves recursion stops and 'S' is set
//       {
// 	if(id)
// 	  {
// 	    if(id->find(u.getNumber()) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[u.getNumber()] = i;
// 	      }
// 	    tagstr << " ID=" << (*id)[u.getNumber()];
// 	  }
// 
// 	// add node in newick format
// 	// 	ret = least = u.getName();
// 	least = u.getName();
// 
// 	// then add gamma to PRIME markup if requested
// 	if(gamma)
// 	  {
// 	    U *species = gamma->getLowestGammaPath(u);
// 	    // check for consistency
// 	    if(species)
// 	      {
// 		// Add species info and AC
// 		tagstr << " S=" << species->getName() 
// 		       << getAntiChainMarkup(u, *gamma);
// 	      }
// 	    else
// 	      {
// 		ostringstream err;
// 		err << "Cannot write AC, since leaf " 
// 		    << u.getNumber() 
// 		    <<" lacks a species";
// 		throw AnError(err.str());
// 	      }
// 	  }
//       }
//     else   // Internal nodes needs to send recursion on and sets 'D'
//       {
// 	// First get the strings for subtrees
// 	string least_left;
// 	string least_right;
// 	string left_str = recursivelyWriteBeepTree(*u.getLeftChild(), 
// 						   least_left, traits,
// 						   gamma, otherParent, extinct, id);
// 	string right_str = recursivelyWriteBeepTree(*u.getRightChild(),
// 						    least_right, traits,
// 						    gamma, otherParent, extinct, id);
// 	if(id)
// 	  {
// 	    if(id->find(u.getNumber()) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[u.getNumber()] = i;
// 	      }
// 	    tagstr << " ID=" << (*id)[u.getNumber()];
// 	  }
// 	
// 	ret.reserve(1024); 	// Avoid too many internal resize (expensive)
// 
// 	// Add node in newick format
// 	// Always order leaves in as alphabetical order as possible
// 	if (least_left < least_right)
// 	  {
// 	    least = least_left;
// 	    ret = "(" + left_str + ", " + right_str + ")";
// 	  }
// 	else
// 	  {
// 	    least = least_right;
// 	    ret = "(" + right_str + ", " + left_str + ")";
// 	  }
// 
// 	// then add gamma/AC to BEEP markup if requested
// 	if(gamma)
// 	  {
// 	    if(gamma->isSpeciation(u))
// 	      {
// 		tagstr << " D=0";
// 	      }
// 	    else
// 	      {
// 		tagstr << " D=1";
// 	      }
// 	    tagstr << getAntiChainMarkup(u, *gamma);
// 	  }
//       }
//     if(otherParent && otherParent->find(&u) != otherParent->end())
//       {  
// 	unsigned pn = u.getParent()->getNumber();
// 	unsigned opn = (*otherParent)[&u]->getNumber();
// 	if(id)
// 	  {
// 	    if(id->find(pn) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[pn] = i;
// 	      }
// 	    if(id->find(opn) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[opn] = i;
// 	      }
// 	  }
// 	tagstr << " HY=(" << (*id)[pn] << " " << (*id)[opn] << ")";
//       }
//     if(extinct && extinct->find(&u) != extinct->end())
//       {
// 	tagstr << " EX=1";
//       }
// 
//     if(u.getName()!= "")
//       {
// 	ret.append(u.getName());
//       }
// 
//     ret.append(NWstr.str());
// 
//     if(tagstr.str() != "")
//       {
// 	ret.append("[&&PRIME" + tagstr.str() + "]");
//       }
// 
//     return ret;
//   }


//   template <class U>
//   void
//   TreeIO::decideSubtreeOrder(U& u, map<U*, string> order)
//   {
//     if(order.find(&u) != order.end())
//       return;
//     else if(u.isLeaf())
//       order[&u] = u.getName();
//     else
//       {
// 	decideSubtreeOrder(*u.getLeftChild(), order);
// 	decideSubtreeOrder(*u.getRightChild(),order);
// 	order[&u] = min(order[u.getLeftChild()], order[u.getRightChild()]);
//       }
//     return;
//   }
		      

  // Basic helper function for writing trees in BEEP format
//   template <class T,class U>
//   std::string
//   TreeIO::recursivelyWriteBeepTree(U& u, map<U*, string> least,
// 				   const TreeIOTraits& traits,
// 				   const T* gamma,
// 				   map<const U*,U*>* otherParent,
// 				   map<const U*,unsigned>* extinct,
// 				   map<unsigned, unsigned>* id)
//   {
//     assert((traits.hasID() && id) == false);
//     string ret;
// 
//     // Determine what should be tagged in PRIME markup
//     //-------------------------------------------------------------------
//     std::ostringstream tagstr;
//     std::ostringstream NWstr;
// 
//     if(traits.hasID())
//       {
// 	tagstr << " ID=" << u.getNumber();
//       }
// 
//     if (traits.hasET())
//       {
// 	if (traits.hasNWisET())
// 	  {
// 	    NWstr << std::showpoint <<":" << u.getTime();
// 	  }
// 	else
// 	  {
// 	    tagstr << " ET=" << std::showpoint << u.getTime();
// 	  }
//       }
//     
//     if (traits.hasNT())
//       {
// 	tagstr << " NT=" << u.getNodeTime();
//       }
// 
//     if (traits.hasBL())
//       {
// 	if (traits.hasNWisET())
// 	  {
// 	    tagstr << " BL=" << u.getLength();
// 	  }
// 	else
// 	  {
// 	    NWstr << ":" << u.getLength();
// 	  }
//       }
// 
//     // Now add node in newick format and gamma/AC if requested
//     // This is done differently ifor leaves and internal nodes
//     //-------------------------------------------------------------------
//     if (u.isLeaf())  // leaves recursion stops and 'S' is set
//       {
// 	if(id)
// 	  {
// 	    if(id->find(u.getNumber()) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[u.getNumber()] = i;
// 	      }
// 	    tagstr << " ID=" << (*id)[u.getNumber()];
// 	  }
// 
// 	// then add gamma to PRIME markup if requested
// 	if(gamma)
// 	  {
// 	    U *species = gamma->getLowestGammaPath(u);
// 	    // check for consistency
// 	    if(species)
// 	      {
// 		// Add species info and AC
// 		tagstr << " S=" << species->getName() 
// 		       << getAntiChainMarkup(u, *gamma);
// 	      }
// 	    else
// 	      {
// 		ostringstream err;
// 		err << "Cannot write AC, since leaf " 
// 		    << u.getNumber() 
// 		    <<" lacks a species";
// 		throw AnError(err.str());
// 	      }
// 	  }
//       }
//     else   // Internal nodes needs to send recursion on and sets 'D'
//       {
// 	// First get the strings for subtrees
// 	// Add node in newick format
// 	// Always order leaves in as alphabetical order as possible
// 	if(least[u.getLeftChild()] < least[u.getRightChild()])
// 	  {
// 	    string left_str = recursivelyWriteBeepTree(*u.getLeftChild(), 
// 						       least, traits,
// 						       gamma, otherParent, extinct, id);
// 	    string right_str = recursivelyWriteBeepTree(*u.getRightChild(),
// 							least, traits,
// 							gamma, otherParent, extinct, id);
// 	    ret = "(" + left_str + ", " + right_str + ")";
// 	  }
// 	else
// 	  {
// 	    string right_str = recursivelyWriteBeepTree(*u.getRightChild(),
// 							least, traits,
// 							gamma, otherParent, extinct, id);
// 	    string left_str = recursivelyWriteBeepTree(*u.getLeftChild(), 
// 						       least, traits,
// 						       gamma, otherParent, extinct, id);
// 	    ret = "(" + right_str + ", " + left_str + ")";
// 	  }
// 
// 	if(id)
// 	  {
// 	    if(id->find(u.getNumber()) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[u.getNumber()] = i;
// 	      }
// 	    tagstr << " ID=" << (*id)[u.getNumber()];
// 	  }
// 	
// 	ret.reserve(1024); 	// Avoid too many internal resize (expensive)
// 
// 
// 	// then add gamma/AC to BEEP markup if requested
// 	if(gamma)
// 	  {
// 	    if(gamma->isSpeciation(u))
// 	      {
// 		tagstr << " D=0";
// 	      }
// 	    else
// 	      {
// 		tagstr << " D=1";
// 	      }
// 	    tagstr << getAntiChainMarkup(u, *gamma);
// 	  }
//       }
//     if(otherParent && otherParent->find(&u) != otherParent->end())
//       {  
// 	unsigned pn = u.getParent()->getNumber();
// 	unsigned opn = (*otherParent)[&u]->getNumber();
// 	if(id)
// 	  {
// 	    if(id->find(pn) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[pn] = i;
// 	      }
// 	    if(id->find(opn) == id->end())
// 	      {
// 		unsigned i = id->size();
// 		(*id)[opn] = i;
// 	      }
// 	  }
// 	tagstr << " HY=(" << (*id)[pn] << " " << (*id)[opn] << ")";
//       }
//     if(extinct && extinct->find(&u) != extinct->end())
//       {
// 	tagstr << " EX=1";
//       }
// 
//     if(u.getName()!= "")
//       {
// 	ret.append(u.getName());
//       }
// 
//     ret.append(NWstr.str());
// 
//     if(tagstr.str() != "")
//       {
// 	ret.append("[&&PRIME" + tagstr.str() + "]");
//       }
// 
//     return ret;
//   }
// 


  // Recursively checks what tags are given for all nodes in subtree T_v
  // Precondition: All bool argument has proper values. Assume a specific
  // bool argument, 'A' has incoming value 'a', and the value for the 
  // current subtree is 'b', then on return, A = a && b, i.e., false if 
  // either a or b is false.
  // postcondition: return true if subtree is non-empty, i.e, v != 0 
  //----------------------------------------------------------------------
  bool 
  TreeIO::recursivelyCheckTags(struct NHXnode* v, TreeIOTraits& traits)
  {
    if (v == 0) // i.e. if the parent was not a leaf
    {
      return false;
    }

      recursivelyCheckTags(v->left, traits);
      recursivelyCheckTags(v->right, traits);

    checkTags(*v, traits);
    return true;
    
  }
    
    
  // Checks what tags are given for node v
  // Precondition: All bool argument has proper values. Assume a specific
  // bool argument, 'A' has incoming value 'a', and the value for the 
  // current node is 'b', then on return, A = a && b.
  //----------------------------------------------------------------------
  void
  TreeIO::checkTags(struct NHXnode& v, TreeIOTraits& traits)
  {
    // Determine if NW is given
    //-------------------------------------------------------------------
    if(!find_annotation(&v, "NW") && !isRoot(&v))
      {
	traits.setNW(false);
      }
    
    // Determine if ET is given
    //-------------------------------------------------------------------
    if(!find_annotation(&v, "ET") && !isRoot(&v))
      {
	traits.setET(false);
      }
    
    // Check if NT is given
    //-------------------------------------------------------------------
    if(!find_annotation(&v, "NT") && !isLeaf(&v))
      {
	traits.setNT(false);
      }
    
    // Check if BL is given
    //-------------------------------------------------------------------
    if(!find_annotation(&v, "BL") && !isRoot(&v)) 
      {
	traits.setBL(false);
      }
    
    // Check if AC is given. 
    //! \todo{The AC check behaves conversely from other checks. Any presence of 
    //! AC makes the hasAC argument true.}
    //-------------------------------------------------------------------
    if(find_annotation(&v, "AC"))
      {
	traits.setAC(true);
      }
    
    // Check if GS is given for leaves.
    //-------------------------------------------------------------------
    if (v.left == 0 && v.right == 0 && speciesName(&v) == 0) 
      {
	traits.setGS(false);
      }

    // Check if there are hybrid annotations
    if(find_annotation(&v, "HY") || find_annotation(&v, "EX") || 
       find_annotation(&v, "OP"))
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
	return read_tree(NULL);
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
	PROGRAMMING_ERROR("TreeIO not properly initialized!");
	return NULL;
      }
  }


  //! \todo{add comments on what this do /bens}
  //----------------------------------------------------------------------
//   template<class U>
//   void
//   TreeIO::updateACInfo(struct NHXnode *v, U *new_node, std::vector<SetOfNodesEx> &AC)
//   {
//     struct NHXannotation *a = find_annotation(v, "AC");
//     if (a != NULL) 
//       {
// 	struct int_list *il = a->arg.il;
// 	while (il) 
// 	  {
// 	    AC[il->i].insert(new_node);
// 	    il = il->next;
// 	  }
//       }
//   }  
