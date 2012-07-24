#ifndef TREE_HH
#define TREE_HH

#include <map>
#include <string>
#include <vector>
// #include "Beep.hh"
#include "PerturbationObservable.hh"


//! This is how much space we allocate for nodes in the vector all_nodes.
//! Don't worry, if you add more nodes, the vector is resized.
//!
//! Do we need to initialize the size of all_nodes at all? (not that it
//! matters much, I guess) /bens
//! \todo{Check if we can move this definition inside namespace beep}
//! Author: Bengt Sennblad, Lars Arvestad, � the MCMC-club, SBC,
//! all rights reserved
//----------------------------------------------------------------------
const unsigned DEF_NODE_VEC_SIZE = 100;

namespace beep 
{

  // Forward declarations.
  class Node;
  class RealVector;
  using namespace std;
  typedef float Real;
  
  //------------------------------------------------------------------
  //
  // class Tree:
  //! Tree is a directed binary graph of Nodes, rooted at rootNode.
  //! Inherits from PerturbationObservable so that listeners may be
  //! notified if there has been changes to topology, lengths, etc.
  //
  //------------------------------------------------------------------
  class Tree : public PerturbationObservable
  {
  public:
    //----------------------------------------------------------------
    //
    //! @name Constructors and Destructors
    //@{
    //----------------------------------------------------------------
    Tree();
    //! Copy constructor 
    //! Notice! Does copy external attributes times, rates and lengths
    Tree(const Tree &T);
    virtual ~Tree();

    //! Named constructor that creates a single node tree. This is used for
    //! those cases when there is no host tree (e.g., when the guest tree
    //! is a species tree)
    static Tree EmptyTree(const Real& RootTime = 1.0, 
			  std::string leafname = "Leaf");

    //! Assignment 
    virtual Tree& operator=(const Tree& T);

    //! Spezial Assignment for backing up a tree for later read-back!
    //! Notice! Does not copy external attributes times, rates and lengths
    //! These must be copied separately. Notice that operator= makes copies
    //! of the external attributes and invalidates pointers in external classes
    virtual Tree& partialCopy(const Tree& T);
    //@}


    //----------------------------------------------------------------------
    //
    // Interface
    //
    //----------------------------------------------------------------------
	
    //----------------------------------------------------------------------
    //! @name access manipulations affecting the whole tree
    //@{
    //----------------------------------------------------------------------
    virtual std::string getName() const; //!< Accessing name of tree
    virtual void setName(std::string s); //!< Set the name of the tree
    //! Total number of nodes in tree
    virtual unsigned getNumberOfNodes() const; 
    //! Total number of leaves in tree
    virtual unsigned getNumberOfLeaves() const; 

    //! Height. i.e., length of longest path from root to a leaf.
    //! Empty tree has height 0, and the root-only tree has height 1.
    virtual unsigned getHeight() const;

    //! Check that all nodes in T_n has sane identity numbers
    //! This is used when reading user-defined trees
    virtual bool IDnumbersAreSane(Node& n);

    virtual void clear();
    //@}

    //----------------------------------------------------------------------
    // @name Access or manipulate nodes
    // 
    //----------------------------------------------------------------------
    virtual Node * getRootNode() const; //!< Access rootNode
    virtual void setRootNode(Node *r); //!< Set the rootNode

    //! @name
    //! Access Node from number
    //@{
    virtual Node* getNode(unsigned nodeNumber);
    virtual Node* getNode(unsigned nodeNumber) const;
    //@}

    //! @name
    //! Accessing nodes from a name
    //@{
    virtual Node* findLeaf(const std::string& name) const;
    virtual Node* findNode(const std::string& name) const;
    //@}

    virtual void removeNode(Node* u);
    //! @name Adding nodes to the tree. 
    //! . Nodes get their numbers from the order they were added!
    //! . Set children to NULL if necessary.
    //! . The name is used for leaves 
    //! . <time> is the time associated with the edge above, i.e. time from
    //!   parent to child node. addNode will also update the node time (time 
    //!   from root) and warn when there is an inconsistency.
    //! . id is a unique identifer for the node.
    //! \todo{It is a little inconsistent that just a few attributes, id,
    //!  time and name, can be set by addNode. All or none? hust a thought 
    //! /bens}
    //@{
    virtual Node* addNode(Node *leftChild, 
			  Node *rightChild,
			  unsigned id,
			  std::string name = ""); 

    virtual Node* addNode(Node *leftChild, 
			  Node *rightChild, 
			  std::string name = ""); 
    //@}
	
    typedef std::vector<Node*>::iterator iterator;
    typedef std::vector<Node*>::const_iterator const_iterator;
    typedef std::vector<Node*>::reverse_iterator reverse_iterator;
    typedef std::vector<Node*>::const_reverse_iterator const_reverse_iterator;
	
    //! @name Iterator access to the nodes of the tree
    //@{
    virtual iterator begin()                      
    {
      return all_nodes.begin(); 
    };
    virtual const_iterator begin() const          
    {
      return all_nodes.begin(); 
    };
    virtual iterator end()                      
    {
      return (all_nodes.begin()+noOfNodes); 
    };
    virtual const_iterator end() const            
    {
      return (all_nodes.begin()+noOfNodes); 
    };
    virtual reverse_iterator rbegin()             
    {
      return (all_nodes.rbegin()+(all_nodes.size()-noOfNodes)); 
    };
    virtual const_reverse_iterator rbegin() const 
    {
      return (all_nodes.rbegin()+(all_nodes.size()-noOfNodes)); 
    };
    virtual reverse_iterator rend()               
    {
      return all_nodes.rend(); 
    };
    virtual const_reverse_iterator rend() const   
    {
      return all_nodes.rend(); 
    };

    //@}
	
    //! MRCA gets most recent common ancestor of two Nodes
    virtual Node* mostRecentCommonAncestor(Node* a, Node* b) const;
    virtual const Node* mostRecentCommonAncestor(const Node* a, const Node* b) const;
    //

    //----------------------------------------------------------------------
    //! @name Access manipulate times, rates and lengths 
    //@{
    //----------------------------------------------------------------------

    //! @name Check if times/rates/lengths are available
    //@{
    virtual bool hasTimes() const;
    virtual bool hasRates() const;
    virtual bool hasLengths() const;
    //@}

    //! \todo{These are not checking if times(rates/weights is NULL
    //! @name Get the time/length/rate associated with node v
    // @{
    virtual Real getTime(const Node& v) const;
    virtual Real getEdgeTime(const Node& v) const;
    virtual Real getLength(const Node& v) const;
    virtual Real getRate(const Node& v) const;
    //@}  

    //! \todo{These are not checking if times(rates/weights is NULL
    //! @name Set the time/length/rate associated with node v
    //@{
    virtual void setTimeNoAssert(const Node& v, Real time) const;
    virtual void setTime(const Node& v, Real time) const;
    virtual void setEdgeTime(const Node& v, Real time) const;
    virtual void setLength(const Node& v, Real weight)const;
    virtual void setRate(const Node& v, Real rate)const;
    //@}  

    //! @name Handle to time, lengths and rates
    // @{
    virtual RealVector& getTimes() const;
    virtual RealVector& getRates() const;
    virtual RealVector& getLengths() const;
    // @}

    //! @name Handle to set times lengths and rates
    //! if internalDelete is true then Tree will handle destruction
    // @{
    virtual void setTimes(RealVector& v, bool internalDelete = false) const;
    virtual void setRates(RealVector& v, bool internalDelete = false) const;
    virtual void setLengths(RealVector& v, bool internalDelete =false) const;
    // @}

    //! @name Removes vector from tree.
    //! If internal delete flag is true, deletes vector first.
    // @{
    virtual void doDeleteTimes();
    virtual void doDeleteLengths();
    virtual void doDeleteRates();
    // @}

    //! Sanity check of time
    virtual bool checkTimeSanity(Node& root) const;
    //! access and manipulate TopTime
    virtual const Real& getTopTime() const;
    virtual void setTopTime(Real newTime);

    //! The total time of the tree
    virtual Real rootToLeafTime() const;
    virtual Real getTopToLeafTime() const;

    //! Let the imbalance of a node be the difference in node time for the 
    //! subtrees. The imbalance of the tree is the largest imbalance found.
    //! Used in primetv to check if tree is ultrametric
    virtual Real imbalance();
    //@}

    //----------------------------------------------------------------------
    //! @name Perturbation indicators
    //! Indicating which, if any, node or if whole tree has been perturbed
    //@{
    //---------------------------------------------------------------------
    virtual Node* perturbedNode() const;
    virtual bool perturbedTree() const;

    virtual void perturbedNode(Node* n) const;
    virtual void perturbedTree(bool value) const; //!< Notice, also affects perturbedNode()
	
    //@}

    //---------------------------------------------------------------------
    // @name I/O
    //---------------------------------------------------------------------
    //@{
    friend std::ostream& operator<<(std::ostream &o, 
				    const Tree& T);
    virtual std::string print(bool useET, bool useNT, bool useBL, bool useER) const;
    virtual std::string print() const;
    //@}

    //----------------------------------------------------------------------
    //
    // Implementation
    //
    //----------------------------------------------------------------------

    //! Recursively copy all nodes in a tree. And keep track of names etc while
    //! you are at it!
    //! Postcondition: The new nodes is owned by the calling Tree, i,e., *this
    virtual Node* copyAllNodes(const Node* v);

    //! Makes a copy of a subtree with unique IDs
    virtual Node* copySubtree(const Node *v);

    //! delete and remove all nodes from tree
    virtual void clearTree();
    //! clear owned node attributes, i.e., times, rates, lengths
    virtual void clearNodeAttributes();

    //! Let the imbalance of a node be the difference in node time for the 
    //! subtrees.
    virtual Real imbalance(Node *v);

    //! Height. i.e., length of longest path from v to a leaf of T_v.
    virtual unsigned getHeight(Node* v) const;

    //! @name
    //! Helper function for I/O
    //@{
  public:
    virtual std::string subtree4os(Node *v, int indent, bool useET, 
				   bool useNT, bool useBL, bool useER) const;
    virtual std::string subtree4os(Node *v, std::string indent_left, 
				   std::string indent_right, bool useET, 
				   bool useNT, bool useBL, 
				   bool useER = false) const;
    virtual std::string subtree4os(Node* v, int indent) const;
    //@}

  protected:
    //----------------------------------------------------------------------
    //
    // @name Attributes
    //
    //----------------------------------------------------------------------
    unsigned noOfNodes;		            //!< number of Nodes
    unsigned noOfLeaves;	            //!< number of Leaves
    Node * rootNode;                        //!< the root Node
    std::map<std::string, Node*> name2node; //!< Map leaf names to leaf nodes
    std::vector<Node*> all_nodes;           //!< Random access based on Node ID
    std::string name;		            //!< Name of tree for output.

    //! @name
    //! perturbed_node and perturbed_tree is used in MCMC. 
    //! An MCMCModel perturbing a node in 
    //! the tree flags this by setting unperturbed in suggestState() and 
    //! unsetting it in discard/commitState(). Any Probabilitymodel
    //! depending on Tree can then check whether Tree is unchanged 
    //! (unpertubed=0) or which node has been perturbed.
    //! This is currently not utilized anywhere. If any ProbabilityModel
    //! is allowed to use it, then there might be a problem with resetting 
    //! perturbed_node - which ProbabilityModel should be responsible for
    //! doing this? Maybe we should reserve this for SubstitutionMCMC, which
    //! is the one that really takes time
    //@{
    mutable Node* perturbed_node;
    mutable bool perturbed_tree;
    //@}
	
    //! @name externally handled attributes
    //@{
    //! These are not guaranteed to be consistent?
    mutable RealVector* times;
    mutable RealVector* rates;
    mutable RealVector* lengths;
    mutable Real topTime;         //! The time above the root
    //@}
    //! $name Flags whether Tree has ownership over its attributes
    //@{
    mutable bool deleteTimes;
    mutable bool deleteRates;
    mutable bool deleteLengths;
    //@}
  };

}// end namespace beep

#endif