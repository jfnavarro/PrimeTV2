#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

#include "AnError.hh"
#include "BeepVector.hh"
#include "Tree.hh"
#include "Node.hh"
#include "TreeIO.hh"


//----------------------------------------------------------------------
//
// class Tree:
// Tree is a directed binary graph of Nodes, rooted at rootNode. 
// Author: LArs Arvestad, Bengt Sennblad, copyright the MCMC-club, SBC, 
// all rights reserved
//
//----------------------------------------------------------------------

namespace beep
{
  using namespace std;
  //Public

  //----------------------------------------------------------------------
  //
  // Constructors and Destructors
  //
  //----------------------------------------------------------------------
  Tree::Tree() :
    PerturbationObservable(),
    noOfNodes(0),
    noOfLeaves(0),
    rootNode(NULL),
    name2node(),
    all_nodes(DEF_NODE_VEC_SIZE, NULL), 
    name("Tree"),
    perturbed_node(0),    // Used in MCMC, the 'final start value' of this
    perturbed_tree(true), // parameter is really set in setRootNode(Node v)
    times(0),
    rates(0),
    lengths(0),
    topTime(0),
    deleteTimes(false),
    deleteRates(false),
    deleteLengths(false)
  {
  }

  Tree::~Tree()
  {
    if(rootNode)
      {
	rootNode->deleteSubtree();    
	delete rootNode;
	rootNode = 0;
      }
    if(	deleteTimes == true && times)
      {
	delete times;
	times = 0;
      }
    if(deleteLengths == true && lengths)
      {
	delete lengths;
	lengths = 0;
      }
    if(deleteRates == true && rates)
      {
	delete rates;
	rates = 0;
      }

  }

  // Named constructor that creates a single node tree. This is used for
  // those cases when there is no host tree (e.g., when the guest tree
  // is a species tree)
  //----------------------------------------------------------------------
  Tree
  Tree::EmptyTree(const Real& rootTime, string leafname)
  {
    Tree T;
    string name = leafname;
    T.setRootNode(T.addNode(0, 0, 0, name));
    T.times = new RealVector(T,0);
    T.topTime = rootTime;
    T.setName("Tree");
    return T;
  }

  Tree::Tree(const Tree &T)
    : noOfNodes(T.noOfNodes),
      noOfLeaves(T.noOfLeaves),
      rootNode(NULL),
      name2node(),                                       // Initialization
      all_nodes(max(noOfNodes,DEF_NODE_VEC_SIZE), NULL), // Allocate vector
      name(T.name),
      perturbed_node(0),
      perturbed_tree(true), // I'm not sure on what the value really should be
      times(0),
      rates(0),
      lengths(0),
      topTime(T.topTime),
      deleteTimes(false),
      deleteRates(false),
      deleteLengths(false)
  {
    if(T.getRootNode())
      {
	setRootNode(copyAllNodes(T.getRootNode()));
      }
    perturbedTree(true);
    if(T.times)
      {
	times = new RealVector(*T.times);
	deleteTimes = true;
      }
    if(T.rates)
      {
	rates = new RealVector(*T.rates);
	deleteRates = true;
      }
    if(T.lengths)
      {
	lengths = new RealVector(*T.lengths);
	deleteLengths = true;
      }
  }

  // Assignment
  //----------------------------------------------------------------------
  Tree&
  Tree::operator=(const Tree& T)
  {
    if(this != &T)
      {
	clear();
	noOfNodes = T.noOfNodes;
	noOfLeaves = T.noOfLeaves;
	if(noOfNodes > all_nodes.size())
	  {
	    all_nodes.resize(noOfNodes, NULL);
	  }
	name = T.name;
	if(T.getRootNode())
	  {
	    setRootNode(copyAllNodes(T.getRootNode()));
	    perturbed_node = rootNode;
	  }
	if(T.times)
	  {
	    times = new RealVector(*T.times);
	    deleteTimes = true;
	  }
	if(T.rates)
	  {
	    rates = new RealVector(*T.rates);
	    deleteRates = true;
	  }
	if(T.lengths)
	  {
	    lengths = new RealVector(*T.lengths);
	    deleteLengths = true;
	  }
	topTime = T.topTime;

      }
    return *this;
  }

  Tree&
  Tree::partialCopy(const Tree& T)
  {
    clearTree();
    noOfNodes = T.noOfNodes;
    noOfLeaves = T.noOfLeaves;
    if(noOfNodes > all_nodes.size())
      {
	all_nodes.resize(noOfNodes, NULL);
      }
    name = T.name;
    if(T.getRootNode()) 
      {
	setRootNode(copyAllNodes(T.getRootNode()));
	perturbed_node = rootNode;
      }

    // times rates and lengths are not copied
    deleteTimes = false;
    deleteLengths = false;
    deleteRates = false;
    topTime = T.topTime;

    return *this;
  }


//   bool
//   Tree::operator==(const Tree& T_in) const
//   {
//     TreeIO io;
//     TreeIOTraits traits;
// 
//     if(io.writeBeepTree(T_in, traits, 0) != io.writeBeepTree(*this, traits, 0))
// 	return false;
//     if((T_in.hasTimes() && hasTimes() && T_in.getTimes() == getTimes()) == false)
// 	return false;
//     if((T_in.hasRates() && hasRates() && T_in.getRates() == getRates()) == false)
// 	return false;
//     if((T_in.hasLengths() && hasLengths() && T_in.getLengths() == getLengths()) == false)
// 	return false;
//     else
//       return true;
//   }



  //----------------------------------------------------------------------
  //
  // Interface
  //
  //----------------------------------------------------------------------
  // returns the Name of the tree
  string
  Tree::getName() const
  {
    return name;
  }

  // Sets the name of the tree
  void
  Tree::setName(string s)
  {
    name = s;
  }

  // Total number of nodes in tree
  //----------------------------------------------------------------------
  unsigned 
  Tree::getNumberOfNodes() const
  {
    return noOfNodes;
  }

  // Total number of leaves in tree
  //----------------------------------------------------------------------
  unsigned 
  Tree::getNumberOfLeaves() const
  {
    return noOfLeaves;
  }

  // Height. Empty tree has height 0, and the root-only tree has height 1.
  //----------------------------------------------------------------------
  unsigned
  Tree::getHeight() const
  {
    return getHeight(rootNode);
  }

  // Check that all nodes has sane identity numbers
  // This is used when reading user-defined trees
  //----------------------------------------------------------------------
  bool 
  Tree::IDnumbersAreSane(Node& n)
  {
    bool ret = n.getNumber() < getNumberOfNodes();
    if(n.isLeaf() == false)
      {
	ret = ret && IDnumbersAreSane(*n.getLeftChild()) &&
	  IDnumbersAreSane(*n.getRightChild());
      }
    return ret;
  }

  // delete and remove all nodes from tree
  //----------------------------------------------------------------------
  void
  Tree::clear()
  {
    clearTree();
    clearNodeAttributes();
  }

  // Access rootNode
  //----------------------------------------------------------------------
  Node* 
  Tree::getRootNode() const
  {
    return rootNode;
  } 

  // set rootNode
  //----------------------------------------------------------------------
  void
  Tree::setRootNode(Node *v)
  {
    assert(v!=NULL);
    assert(v->getNumber()<all_nodes.size());

    // perturbedNode has to be initialized with an exisiting somewhere 
    // early so that like is initialized,but no nodes exist at time of 
    // construction.
    // trying initialization here for time being, alternative could be 
    // in addNode. (This is ugly - does anyone has suggestions) / bens
    perturbed_node = v; 
    rootNode = v;
  }

  // Access Node from number
  //----------------------------------------------------------------------
  Node* 
  Tree::getNode(unsigned no)
  {
    if (no >= all_nodes.size()) 
      {
	return NULL;
      }
    else
      {
	return all_nodes[no];
      }
  }

  Node* 
  Tree::getNode(unsigned no) const
  {
    if (no > all_nodes.size()) 
      {
	return NULL;
      }
    else
      {
	return all_nodes[no];
      }
  }

  // Accessing nodes from a name
  //----------------------------------------------------------------------
  Node*
  Tree::findNode(const string& name) const
  {

    //  if (iter != name2node.end()) // Why does this one not work??? IT SHOULD WHAT HAPPENS WHEN YOU USE IT? /bens
    //  if (iter->second != NULL)	// Workaround. Bad form according to STL. // second is not always set to NULL when no matching key is found /fmattias
    if(name2node.count(name) > 0)
      {
        map<string, Node*>::const_iterator iter = name2node.find(name);
	return iter->second;
      }
    else
      {
	//throw AnError("Leaf name not found", name, 1); //THIS NEEDS TO BE FIXED!
      
	return NULL;
      }
  }
  // Accessing leaves from a name
  //----------------------------------------------------------------------
  Node*
  Tree::findLeaf(const string& name) const
  {
    Node* ret = findNode(name);
    if(ret->isLeaf())
      {
	return ret;
      }
    else
      {
	//!\todo{Should we really throw AnError here?}
	throw AnError("Found interior node when looking for a leaf name ", 
		      name, 1); 
      }
  }

  void 
  Tree::removeNode(Node* u)
  {
#ifdef UNDERCONSTRUCTION
    // Get hold of parent
    if (! u->isRoot()) {
      Node *parent = u->getParent();
      if (parent->isRoot()) 
	{
	} 
      else
	{
	  Node *sibling =
	    }
      

      // move last active node to id/position of u
      int id = getNumberOfNodes() - 1;
      Node* v = getNode(id);
      if(v != u)
	{
	  v->changeID(u->getNumber());
	  all_nodes[u->getNumber()] = v;
	}
      all_nodes[getNumberOfNodes() - 1] = NULL;
      if(u->getName() != "")
	{
	  map<string,Node*>::iterator i = name2node.find(u->getName());
	  assert(i != name2node.end());
	  name2node.erase(i);
	}
      noOfNodes--;
      if(u->isLeaf())
	{
	  noOfLeaves--;
	}

      if(times)
	{
	  times->rmElement(u->getNumber());
	}
      if(lengths)
	{
	  lengths->rmElement(u->getNumber());
	}
      if(rates)
	{
	  rates->rmElement(u->getNumber());
	}
      delete u;
      u = NULL;
    }
#endif
  }

  // The main tool for constructing the tree
  //-----------------------------------------------------------------
  Node *
  Tree::addNode(Node *leftChild, 
		Node *rightChild, 
		unsigned node_id,
		string name)
  {
    assert(leftChild==NULL || leftChild->getNumber()<all_nodes.size());
    assert(rightChild==NULL || rightChild->getNumber()<all_nodes.size());


    noOfNodes++;
    if (leftChild == NULL && rightChild == NULL)
      {
	noOfLeaves++;
      }

    Node *v = new Node(node_id, name);
    v->setTree(*this);
    v->setChildren(leftChild, rightChild);
    //     if (all_nodes.size() <= node_id)//only works if node_id<2*all_nodes.size
    while(all_nodes.size() <= node_id)
      {
	all_nodes.resize(2 * all_nodes.size(), NULL);
      }
    //     else if(all_nodes[node_id] != NULL)
    if(all_nodes[node_id] != NULL)
      {
	std::ostringstream id_str;
	id_str << node_id;
	throw AnError("There seems to be two nodes with the same id!", 
		      id_str.str(), 1);
      }

    all_nodes[node_id] = v;

    name2node.insert(pair<string, Node*>(name, v));

    if(times)
      {
	times->addElement(node_id, 0.0);
      }
    if(lengths)
      {
	lengths->addElement(node_id, 0.0);
      }
    if(rates)
      {
	rates->addElement(node_id, 0.0);
      }
    return v;
  }

  //The main tool for constructing the tree, overloading for trees
  //without any branch times
  //----------------------------------------------------------------------
  Node *
  Tree::addNode(Node *leftChild, 
		Node *rightChild, 
		string name)
  {
    return addNode(leftChild, rightChild, getNumberOfNodes(), name);
  }

  // MRCA gets most recent common ancestor of two speciesNodes
  //----------------------------------------------------------------------
  Node* 
  Tree::mostRecentCommonAncestor(Node* a, Node* b) const
  {
    assert(a != NULL);
    assert(b != NULL);

    while (a != b)
      {
	if (b->dominates(*a))
	  {
	    a = a->getParent();
	  }
	else 
	  {
	    b = b->getParent();
	  }
      }
    return a;
  }

  // MRCA gets most recent common ancestor of two speciesNodes
  //----------------------------------------------------------------------
  const Node* 
  Tree::mostRecentCommonAncestor(const Node* a, const Node* b) const
  {
    assert(a != NULL);
    assert(b != NULL);

    while (a != b)
      {
	if (b->dominates(*a))
	  {
	    a = a->getParent();
	  }
	else 
	  {
	    b = b->getParent();
	  }
      }
    return a;
  }

  // Check if times/rates/lengths are available
  //----------------------------------------------------------------------
  bool 
  Tree::hasTimes() const
  {
    return times != 0;
  }
  bool 
  Tree::hasRates() const
  {
    return rates != 0;
  }
  bool 
  Tree::hasLengths() const
  {
    return lengths != 0;
  }



  // Gets the node time of node v
  //----------------------------------------------------------------------
  Real
  Tree::getTime(const Node& v) const
  {
    return (*times)[v];
  }

  // Gets the node time of node v
  //----------------------------------------------------------------------
  Real
  Tree::getEdgeTime(const Node& v) const
  {
    if(v.isRoot())
      {
	return topTime;
      }
    else
      {
	return (*times)[v.getParent()] - (*times)[v];
      }
  }

  // Gets the weight of node v
  //----------------------------------------------------------------------
  Real
  Tree::getLength(const Node& v) const
  {
    return (*lengths)[v];
  }

  // Gets the rate of node v
  //----------------------------------------------------------------------
  Real
  Tree::getRate(const Node& v) const
  {
    if(rates->size() == 1)
      {
	return (*rates)[0u];
      }
    else
      {
	return (*rates)[v];
      }
  }

  // Sets the divergence time of node v
  void
  Tree::setTimeNoAssert(const Node& v, Real time) const
  {
    (*times)[v] = time;
  }

  // Sets the divergence time of node v
  //----------------------------------------------------------------------
  void
  Tree::setTime(const Node& v, Real time) const
  {
    (*times)[v] = time;

    assert(v.isLeaf() || (*times)[v] >= (*times)[v.getLeftChild()]);
    assert(v.isLeaf() || (*times)[v] >= (*times)[v.getRightChild()]);
    assert(v.isRoot() || (*times)[v.getParent()] >= (*times)[v]);
  }

  // Sets the edge time of node v
  //----------------------------------------------------------------------
  void
  Tree::setEdgeTime(const Node& v, Real time) const
  {
    if(v.isRoot())
      {
	topTime = time;
      }
    else
      {
	(*times)[v] = (*times)[v.getParent()] - time;

	assert((*times)[v] > (*times)[v.getLeftChild()]);
	assert((*times)[v] > (*times)[v.getRightChild()]);
      }
  }

  // Sets the weight of node v
  //----------------------------------------------------------------------
  void
  Tree::setLength(const Node& v, Real weight) const
  {
    if(weight < 2 * std::numeric_limits< double >::min())
      {
	weight = 2 * std::numeric_limits< double >::min();
      }
    if(v.isRoot() == false && v.getParent()->isRoot())
      {
	Node& s = *v.getSibling();
	weight = (weight + (*lengths)[s])/2;
	(*lengths)[s] = weight;
      }
    (*lengths)[v] = weight;
  }

  // Sets the rate of node v
  //----------------------------------------------------------------------
  void
  Tree::setRate(const Node& v, Real rate) const
  {
    if(rates->size() == 1)
      {
	(*rates)[0u] = rate;
      }
    else
      {
	(*rates)[v] = rate;
      }
  }

  // Handle to time, lengths and rates
  RealVector& 
  Tree::getTimes() const
  {
    return *times;
  }
  RealVector& 
  Tree::getRates() const
  {
    return *rates;
  }
  RealVector& 
  Tree::getLengths() const
  {
    return *lengths;
  }

  // Handle to set times lengths and rates
  // if internalDelete is true then Tree will handle destruction
  void
  Tree::setTimes(RealVector& v, bool internalDelete) const
  {
    if(times && deleteTimes)
      {
	delete times;
      }
    times = &v;
    deleteTimes = internalDelete;
  }

  void
  Tree::setRates(RealVector& v, bool internalDelete) const
  {
    if(rates && deleteRates)
      {
	delete rates;
      }
    rates = &v;
    deleteRates = internalDelete;
  }

  void
  Tree::setLengths(RealVector& v, bool internalDelete) const
  {
    if(lengths && deleteLengths)
      {
	delete lengths;
	lengths = 0;
      }

    lengths = &v;
    deleteLengths = internalDelete;
  }

void Tree::doDeleteTimes()
{
	if (deleteTimes && times != NULL) { delete times; }
	times = NULL;
}

void Tree::doDeleteLengths()
{
	if (deleteLengths && lengths != NULL) { delete lengths; }
	lengths = NULL;
}


void Tree::doDeleteRates()
{
	if (deleteRates && rates != NULL) { delete rates; }
	rates = NULL;
}


  // Sanity check of time
  bool 
  Tree::checkTimeSanity(Node& root) const
  {
    Node& left = *root.getLeftChild();
    Node& right = *root.getRightChild();

    if(getTime(left) > getTime(right) ||
       getTime(left) > getTime(right))
      {
	return false;
      }
    else
      {
	return checkTimeSanity(left) && checkTimeSanity(right);
      }
  }


  // access and manipulate TopTime
  //----------------------------------------------------------------------
  const Real& 
  Tree::getTopTime() const
  {
    return topTime;
  }

  void 
  Tree::setTopTime(Real newTime)
  {
    topTime = newTime;
  }


  // Total time of tree
  //----------------------------------------------------------------------
  Real
  Tree::rootToLeafTime() const
  {
    Node *v = getRootNode();
    if (!v)   //assert?
      {
	throw AnError("rootToLeafTime: No root node! Not good...", 1);
      }

    return v->getNodeTime();

  }

  Real
  Tree::getTopToLeafTime() const
  {
    return (getTime(*getRootNode()) + topTime);
  }


  // Used by primetv 
  // Essentially checks if tree is ultrametric
  //!\todo{Document better?}
  //----------------------------------------------------------------------
  Real 
  Tree::imbalance()
  {
    Node *r = getRootNode();
    assert (r != NULL);
    return imbalance(r);
  }


  // checking which node, if any, is perturbed
  //----------------------------------------------------------------------
  Node*
  Tree::perturbedNode() const
  {
    return perturbed_node;
  }

  // checking which node, if any, is perturbed
  //----------------------------------------------------------------------
  bool
  Tree::perturbedTree() const
  {
    return perturbed_tree;
  }

  // Indicating which, if any, node has been perturbed
  //---------------------------------------------------------------------
  void 
  Tree::perturbedNode(Node* n) const
  {
    perturbed_node = n;
  }

  // Indicating if the whole tree has been altered
  // Note that this also sets perturbed node
  //---------------------------------------------------------------------
  void 
  Tree::perturbedTree(bool value) const
  {
    perturbed_tree = value;
    if(value)
      {
	perturbed_node = rootNode;
      }
    else
      {
	perturbed_node = 0;
      }
  }

  //---------------------------------------------------------------------
  // I/O
  //---------------------------------------------------------------------
  std::ostream& 
  operator<<(std::ostream &o, 
	     const Tree& T)
  {
    return o << T.print();
  }

  std::string
  Tree::print(bool useET, bool useNT, bool useBL, bool useER) const
  {
    ostringstream oss;
    string namestr = getName();
    if (namestr.length() > 0) 
      {
	oss << "Tree " << getName() << ":\n";
      }
    else 
      {
	oss << "Tree:\n";
      }
    if(rootNode)
      {
	oss << subtree4os(getRootNode(), "", "", useET, useNT, useBL, useER);
      }
    else
      {
	oss << "NULL";
      }
    //     oss << subtree4os(getRootNode(), 0, useET, useNT, useBL, useER);
    return oss.str();
  }

  std::string
  Tree::print() const
  {
    return print(times, times, lengths, rates);
  }



  //----------------------------------------------------------------------
  //
  // Implementation
  //
  //----------------------------------------------------------------------

  // Recursively copy all nodes in a tree. And keep track of names etc while
  // you are at it!
  // Notice! Assumes that the new nodes will belong to the calling Tree!!
  //----------------------------------------------------------------------
  Node *
  Tree::copyAllNodes(const Node *v)
  {
    assert(v != NULL);
    Node *u = new Node(*v);
    u->setTree(*this);

    assert(u->getNumber() < all_nodes.size());
    all_nodes[u->getNumber()] = u;

    if(u->getName() != "")
      {
	name2node[u->getName()] = u;
      }
    if (v->isLeaf() == false)
      {
	Node *l = copyAllNodes(v->getLeftChild());
	Node *r = copyAllNodes(v->getRightChild());
	u->setChildren(l, r);	// Notice that setChildren changes (or corrects) porder here!
      }
    return u;
  }

  // Recursively copy all nodes in a tree. And keep track of names etc while
  // you are at it! The new nodes get new IDs
  // Notice! Assumes that the new nodes will belong to the calling Tree!!
  //----------------------------------------------------------------------
  Node *
  Tree::copySubtree(const Node *v)
  {
    assert(v != NULL);
    string name = v->getName();
    if(name != "")
      {
	while(name2node.find(name) != name2node.end())
	  {
	    name = name + "a";
	  }
      }
    Node *u = addNode(NULL, NULL, name);
    u->setTree(*this);

    assert(u->getNumber()<all_nodes.size());
    all_nodes[u->getNumber()] = u;

    if (v->isLeaf())
      {
	name2node[u->getName()] = u;
	return u;
      }
    else
      {
	Node *l = copySubtree(v->getLeftChild());
	Node *r = copySubtree(v->getRightChild());
	u->setChildren(l, r);	// Notice that setChildren changes (or corrects) porder here!
	return u;
      }
  }


  // delete and remove all nodes from tree
  //----------------------------------------------------------------------
  void
  Tree::clearTree()
  {
    if(rootNode != NULL)
      {
	rootNode->deleteSubtree();    
	delete rootNode;
	rootNode = NULL;
      }
    noOfNodes = noOfLeaves = 0;
    name2node.clear();
    all_nodes.clear();
    all_nodes = std::vector<Node*>(DEF_NODE_VEC_SIZE, NULL);
    perturbed_node = NULL;
    perturbed_tree = true;
  }

  void
  Tree::clearNodeAttributes()
  {
    if(times)
      {
	if(deleteTimes)
	  {
	    delete times;
	  }
	times = 0;
      }
    if(rates)  // rates is owned by EdgeRateModel
      {
	if(deleteRates)
	  {
	    delete rates;
	  }
	rates = 0;
      }
    if(lengths)
      {
	if(deleteLengths)
	  {
	    delete lengths;
	  }
	lengths = 0;
      }
    topTime = 0;
  }


  Real
  Tree::imbalance(Node *v)
  {
    if (v->isLeaf()) 
      {
	return 0;
      } 
    else
      {
	Node *l = v->getLeftChild();
	Node *r = v->getRightChild();
	Real my_imbalance = fabs(l->getNodeTime() + l->getTime() - 
				 r->getNodeTime()-r->getTime());
	Real l_imbalance = imbalance(l);
	Real r_imbalance = imbalance(r);
#define MAX(A, B) ((A>B) ? A : B)
	return MAX(my_imbalance, MAX(l_imbalance, r_imbalance));
      }
  }

  unsigned 
  Tree::getHeight(Node* v) const
  {
    if (v == NULL) return 0;
    else return 1 + max(getHeight(v->getLeftChild()),
			getHeight(v->getRightChild()));
  }

  string
  Tree::subtree4os(Node *v, int indent, bool useET, bool useNT, 
		   bool useBL, bool useER) const
  {
    ostringstream oss;
    if (v) 
      {
	oss << subtree4os(v->getRightChild(), indent + 5, useET, 
			  useNT, useBL, useER)
	    << string(indent, ' ');
	if (!v->getName().empty()) 
	  {
	    oss << "--- "
		<< v->getNumber()
		<< ", "
		<< v->getName();
	  }
	else 
	  {
	    oss << "--- "
		<< v->getNumber();
	  }

	if(useET)
	  {
	    assert(hasTimes());
	    oss << ",   ET: "
		<< getEdgeTime(*v);
	  } 
	if(useNT)
	  {
	    oss << ",   NT: "
		<< getTime(*v);
	  }
	if(useBL)
	  {
	    oss << ",   BL :";
	    if(lengths)
	      {
		if(lengths->size() == 1)
		  {
		    oss << (*lengths)[0u];
		  }
		else
		  {
		    oss << (*lengths)[v];
		  }
	      }
	    else
	      {
		oss << 0;
	      }
	  }
	if(useER)
	  {
	    oss << ",   ER :";
	    if(rates)
	      {
		if(rates->size() == 1)
		  {
		    oss << (*rates)[0u];
		  }
		else
		  {
		    oss << (*rates)[v];
		  }
	      }
	    else
	      {
		oss << 0;
	      }
	  }

	oss << "\n"
	    << subtree4os(v->getLeftChild(), indent + 5, useET, 
			  useNT, useBL, useER);
      }
    return oss.str();
  }
  
 

  string
  Tree::subtree4os(Node *v, string indent_left, string indent_right,
		   bool useET, bool useNT, bool useBL, bool useER) const
  {
    ostringstream oss;
    if (v != NULL) 
      {
	oss << subtree4os(v->getRightChild(), indent_right +"    |", 
			  indent_right + "     ", useET, useNT, useBL, useER)
	    << indent_right.substr(0,indent_right.size()-1) + "+";
	if(!v->getName().empty()) 
	  {
	    oss << "--- "
		<< v->getNumber()
		<< ", "
		<< v->getName();
	  }
	else 
	  {
	    oss << "--- "
		<< v->getNumber();
	  }

	if(useET)
	  {
	    assert(hasTimes());
	    oss << ",   ET: "
		<< getEdgeTime(*v);
	  } 
	if(useNT)
	  {
	    assert(hasTimes());
	    oss << ",   NT: "
		<< getTime(*v);
	  }
	if(useBL)
	  {
	    assert(hasLengths());
	    oss << ",   BL :"
		<< getLength(*v);
	  }
	if(useER)
	  {
	    assert(hasRates());
	    oss << ",   ER :";
	    if(rates)
	      {
		if(rates->size() == 1)
		  {
		    oss << (*rates)[0u];
		  }
		else
		  {
		    oss << (*rates)[v];
		  }
	      }
	  }
	oss << "\n"
	    << subtree4os(v->getLeftChild(), indent_left + "     ", 
			  indent_left + "    |", useET, useNT, useBL, useER);
      }
    return oss.str();
  }

  string
  Tree::subtree4os(Node *v, int indent) const
  {
    return subtree4os(v, indent, true, true, false, false);
  }

}// end namespace beep




