#ifndef NODE_HH
#define NODE_HH
#include <iostream>
#include <string>
#include "Color.h"

    using namespace std;
    class Tree;
    class SetOfNodes;
    
    enum  Type{Leaf=0, Speciation=1, Duplication=2, LateralTransfer=3, Undefined=4};
    
    typedef float Real;
  //-----------------------------------------------------------------------
  //! Author: Lars Arvestad
  //! copyright: mcmc-klubben, SBC
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  //
  //! Class Node
  //! This class handles nodes (vertices) in a rooted binary phylogenetic 
  //! tree (see also the class Tree). 
  //! All nodes have a unique ID, and pointers to their nearest neighboring
  //! nodes, e.g., its left and right child and its parent. Leaves are 
  //! nodes that only have parents (child pointers are NULL). Leaves also 
  //! have names associated to them, e.g., the name of the gene or organism
  //! it corresponds to. The root Node has no parent (parent pointer is NULL)
  //! Nodes also hold information about its incoming arc (e.g., the edge 
  //! between the node and its parent). This information include the 
  //! chronological time and the length  measured in expected number of 
  //! substitutions (= the time * the substitution rate of the edge). 
  //! The node time of a node is the sum of the all edge times in the path 
  //! between the node and any leaf of the subtree rooted at the node. Node
  //! times of leaves are always 0. Note, that we, thus, for practical reasons
  //! use 'backward' time! Edge times and nodeTimes are clearly closely 
  //! connected and there are functions in Node for keeping them so. However,
  //! any connection between times and lengths are considered handled outside
  //! Node.
  //! Ancestors of a node is related to the node through a chain of parents
  //! and descendant of a node is related to this node through a chain of 
  //! child relationships.
  //! The parent/child relation between a set of nodes imply a partial 
  //! order on this set, this is implemented in the attribute porder and
  //! associated functions.
  //
  //----------------------------------------------------------------------
  class Node
  {
    //-----------------------------------------------------------------------
    //
    // Constructors, destructors and assigment operators
    //
    //-----------------------------------------------------------------------
  public:
    Node(unsigned id);
    Node(unsigned id, const std::string& nodeName);
    virtual ~Node();              

    //! Copy constructor - relatives in tree are not copied!
    //-----------------------------------------------------------------------
    Node(const Node &);
  private: 	// Must not use these, before proper implementation

    //! Assignment: Also pointers are copied, which might not be the 
    //! proper thing to do.
    //----------------------------------------------------------------------
    

 
  public:
    
    
    //Extra Methods aded by Jose Carlos Fernandez
    //----------------------------------------------------------------------
    /*---------SETTERS--------------------*/
    void setColor(Color c);
    void setSize(double s);
    void setX(double x);
    void setY(double y);
    void setHostParent( Node *hostparent);
    void setHostChild( Node *hostchild);
    void setReconcilation(Type t);
    void setVisited(unsigned inc);
    void incVisited();
    
    /*---------GETTERS--------------------*/
    Color getColor();
    double getSize();
    double getX();
    double getY();
    double getX() const;
    double getY() const;
    Node *getHostParent();
    Node *getHostChild();
    Type getReconcilation();
    Node *getHostParent() const;
    Node *getHostChild() const;
    Type getReconcilation() const;
    unsigned getVisited();
    
    //----------------------------------------------------------------------
    //
    // Access methods
    //
    //----------------------------------------------------------------------
    //! @name Accessing relatives
    //! Returns the requested relative 
    //----------------------------------------------------------------------
    //@{
    Node* getLeftChild() const;
    Node* getRightChild() const;
    Node* getParent() const;
    Node* getSibling() const;
    //@}

    //! Retrieve the child of current node (x) that has y as a descendant. 
    //! y may be a child of x, but could also be a grandchild.
    //----------------------------------------------------------------------
    Node* getDominatingChild(Node* y);

    //! Rotate the node, i.e., switch left and right child.
    //----------------------------------------------------------------------
    void rotate();

    //! get the (leaf) name
    //----------------------------------------------------------------------
    const std::string& getName() const; 

    //! Get the owner tree
    //! \todo{There is no check that ownerTree!=NULL}
    //----------------------------------------------------------------------
    Tree* getTree();

    //! get the node's number (post-order)
    //! \todo{ I think this has changed so that the number don't imply 
    //! anything about the partial order /bens}
    //----------------------------------------------------------------------
    unsigned getNumber() const;
    unsigned getPorder() const;
    unsigned getNumberOfLeaves() const;
    Real getBranchLength() const;
    
    
    Node& operator=(const Node &);
    
    //! Used only in ReconciliationTimeMCMC to set up the suggestion variance
    unsigned getMaxPathToLeaf();

    //----------------------------------------------------------------------
    //
    // Manipulators
    //
    //----------------------------------------------------------------------
    
    //!Set the (leaf) name
    //----------------------------------------------------------------------
    void setName(const std::string& nodeName);       

    //! Set the owner tree
    //----------------------------------------------------------------------
    void setTree(Tree& T); 
   
    //! Sets pointers to left and right subtrees. 
    //! Note that left's and right' parent is also set!
    //! The partial order attribute (porder) is updated when necessary.
    //-----------------------------------------------------------------------
    void setChildren(Node *left, Node *right); 

    //! Sets pointer to parent node
    //-----------------------------------------------------------------------
    void setParent(Node *parent);

    //! Change ID of this, used, e.g., in HybridTree, to ascertain condition 
    //! ID < Tree.getNumberOfNodes(), when deleting hybrid or extinction nodes
    //----------------------------------------------------------------------
    void changeID(unsigned newID);

    //! Delete all nodes lower in the tree. The current node is not deleted.
    //-----------------------------------------------------------------------
    void deleteSubtree();

    /**
     * getLeaves
     *
     * Returns the leaves in the subtree with this node as root.
     *
     * @return All leaves in this subtree.
     * @author peter9 January 2010.
     */
    SetOfNodes getLeaves();

    //----------------------------------------------------------------------
    //
    // Tests
    //
    //----------------------------------------------------------------------

    //! Checks if the current node is a leaf - A leaf lacks children
    //----------------------------------------------------------------------
    bool isLeaf() const;		

    //! Checks if the current node is the root. Only the root has no parent
    //----------------------------------------------------------------------
    bool isRoot() const;		

    //! \name Comparison, order
    //! This is used when putting a node in a set. See for instance GammaMap.
    //! If the tree has been built using post-order numbering (guarenteed 
    //! through TreeIO), this is also a domination test. That means that 
    //! if a < b, then b dominates a in the tree. Beware that domination is 
    //! partial, and this test does not warn you when two  nodes are not 
    //! ordered with respect to domination.
    //! These really need to be double-checked
    //----------------------------------------------------------------------
    //@{
    bool operator<=(const Node& b) const;
    bool operator<(const Node& b) const;
    bool operator<(const Node* b) const;
    bool operator>(const Node& b) const;

    //! Return non-zero if this is higher up in the tree (dominates) than x.
    //! This will probably be deprecated(?)
    //----------------------------------------------------------------------
    bool dominates(const Node &b) const;
    bool strictlyDominates(const Node &b) const;
    //@}


    //----------------------------------------------------------------------
    //
    // Output
    //
    //----------------------------------------------------------------------

    //! \name Simple output
    //----------------------------------------------------------------------
    //@{
    friend std::ostream& operator<< (std::ostream& o, const Node &v);
    friend std::ostream& operator<< (std::ostream& o, const Node *v);
    //@}

  private:
    //! \name Output helpers
    std::string stringify(std::string tag, Real val) const;
    std::string stringify(std::string tag, std::string val) const;
    std::string stringify(std::string tag, Node *v) const;
    
    //----------------------------------------------------------------------
    //
    // Attributes
    //
    //----------------------------------------------------------------------
  protected:
    unsigned number;      //! the number, the unique id in the tree.

    Node *parent;         //! pointer to the parent Node 
    Node *leftChild;      //! pointer to the left child Node 
    Node *rightChild;     //! pointer to the right child Node
    long porder;	  //! Defining partial order of tree.

    //!\name deprecated
    //@{
    Real time;		  //! the arc time from the parent to current node
    Real nodeTime;        //! the time from the leaves.
    Real branchLength;	  //! equals time * rate, we might want to remove this
    //@}

    std::string name;     //! the (leaf) name
    Tree* ownerTree;      //! The tree to which I belong
    
    /* extra feactures*/
    
    Color color;
    double size;
    double x;
    double y;
    Node *hostParent;
    Node *hostChild;
    Type reconcilation;
    unsigned visited;

  public:
    //! \name DEPRECATED BUT KEPT FOR NOW! 
    //! @{

    //=====================================================================
    // 
    // DEPRECATED!
    //
    //=====================================================================

    //! DEPRECATED! updateTimeAndLength
    // Updates the edge time and branchLength attributes, in relation to its
    // parent!
    // We probably want to do this because we have used setNodeTime and 
    // subsequently need to get the  edge times and branchLengths correct.
    // Typically, when having set a node's nodeTime, we call 
    // updateTimeAndLength() for its CHILDREN! (see below).
    // updateTimeAndLength() only updates branchLength if both branchLength 
    // and edge time had previouosly been set.
    //

    //!get the node's chronological time from the leaves in the subtree
    //! rooted at the node
    //----------------------------------------------------------------------
    Real getNodeTime() const;                   

    //!get the arc's chronological time from parent to current node
    //----------------------------------------------------------------------
    Real getTime() const;

    //!get the branch (edge) length associated with the node
    //----------------------------------------------------------------------
    Real getLength() const;

    //----------------------------------------------------------------------
    //! \name Setting times, nodeTimes and branchLengths
    //!
    //! This is not a trivial issue! We provide two ways of setting/changing
    //! time attributes:
    //! - bool change<Node>Time(time) is the safe way. It only allows 
    //!     changes that are compatible with the nodeTimes of this node's 
    //!     parent and children, otherwise it returns false. It also 
    //!     automatically updates time/nodeTime
    //! - void setTime/NodeTime/Length(value) is the raw, unsafe way, that
    //!     usually is used when reading trees from file or in functions
    //!     with rock-hard control of times (see below).
    //!     it does not check for compatibility with other time attributes.
    //!     and it does not update other time attributes.
    //!     These other attributes can be updated with update<Node>Time().
    //!     You would probably be using this in a recursive manner to 
    //!     change times (see, e.g., ReconciliationTimeSampler::sampleTimes()
    //! If the last approach is used then we often want to update the other 
    //! time dependent attributes:
    //! - void updateTime() After using setNodeTime(value) 
    //!     updateTime() can be called to update the 
    //!     edge time of the node.
    //!     There are some restrictions, see comments on these functions
    //! - void updateNodeTime() After using setTime(value) 
    //!     updateNodeTime() can be called to update the nodeTime of the node
    //!     There are some restrictions, see comments on these functions
    //----------------------------------------------------------------------
    //@{
    //! Provided that nt does not violate the partial order of nodes and 
    //! their Nodetime, this sets the node's chronological time from the 
    //! leaves of the subtree rooted at the node AND update surrounding edge
    //! times
    //! Note that it does not change the surrounding node times. 
    //! This assumes that all times and node times have been initiated. 
    //! Note that the root's edgetime will never be changed by this function
    //! This has to be done explicitely!
    //! Please note that this function no longer changes the edge lengths!
    //----------------------------------------------------------------------
    bool changeNodeTime(const Real &t);

    //! Provided that et does not violate the partial order of nodes and 
    //! their Nodetimes, this sets the node's edge time AND update its 
    //! node time and surrounding edge times.
    //! Note that it does not change the surrounding node times. 
    //! This assumes that all times and node times have been initiated. 
    //! Does not work for leaves, since we cannot alter a leaf's nodeTime
    //! Please note that this function no longer changes the edge lengths!
    //----------------------------------------------------------------------
    bool changeTime(const Real &et);

    //! Set the node's chronological time from the leaves of the subtree
    //! rooted at the node. No check for compatibility with other nodes
    //! no update of dependent attributes!
    //! Use updateTime() to get NodeTimes and branchLengths up to date
    //----------------------------------------------------------------------
    void setNodeTime(const Real &t);   

    //! Set the arc's chronological time. No check for compatibility with 
    //! other nodes, no update of dependent attributes!
    //! Use updateNodeTime() to get NodeTimes and branchLengths up to date
    //----------------------------------------------------------------------
    void setTime(const Real &t);   

    //! Set the branch length associated with the node
    //! This does not affect time, nodeTime or substRate. It is (reasonably)
    //! assumed that when using this function, time and substRate is not 
    //! modeled.
    //! Thus, you are probably performing a standard tree-reconstruction
    //! perturbing only the topology and branchLengths of the tree.
    //----------------------------------------------------------------------
    void setLength(const Real &newLength);
    //@}


    //!@}

  };

#endif