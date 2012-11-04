#include <cassert>
#include <cmath>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <sstream>

#include "AnError.hh"
#include "Node.hh"
#include "Tree.hh"
#include "BeepVector.hh"

//----------------------------------------------------------------------
// Author: Lars Arvestad
// copyright: mcmc-klubben, SBC
//----------------------------------------------------------------------

  using namespace std;

  Node::Node(unsigned id)
    : number(id),        // unique identifier
      parent(NULL),      // neighbors in the tree
      leftChild(NULL),
      rightChild(NULL),
      porder(0),         // partial order of nodes on tree
      time(0.0),         // time of incoming edge
      nodeTime(0.0),     // time interval between node and leaves
      branchLength(0.0), // branchLength 
      name(),         // user-specified name (usually only for leaves)
      ownerTree(0),
      color(),
      size(0.0),
      x(0.0),
      y(0.0),
      hostParent(NULL),
      hostChild(NULL),
      reconcilation(Undefined),
      visited(0)
  {

  }

  Node::Node(unsigned id, const string& nodeName)
    : number(id),
      parent(NULL),
      leftChild(NULL),
      rightChild(NULL),
      porder(0),
      time(0),
      nodeTime(0.0),
      branchLength(0.0),
      name(nodeName),
      ownerTree(0),
      color(),
      size(0.0),
      x(0.0),
      y(0.0),
      hostParent(NULL),
      hostChild(NULL),
      reconcilation(Undefined),
      visited(0)
  {

  }

  // Copy relatives in tree are not copied!
  //----------------------------------------------------------------------
  Node::Node(const Node &v)
    : number(v.number),
      parent(NULL),          // relatives in tree are not copied!
      leftChild(NULL),
      rightChild(NULL),
      porder(v.porder),
      time(v.time),
      nodeTime(v.nodeTime),
      branchLength(v.branchLength),
      name(v.name),
      ownerTree(v.ownerTree),
      color(v.color),
      size(v.size),          // relatives in tree are not copied!
      x(v.x),
      y(v.y),
      hostParent(v.hostParent),
      hostChild(v.hostChild),
      reconcilation(v.reconcilation),
      visited(0)
  {

  }


  Node::~Node()
  {
    
  }
  
  

  // Assignment: Also pointers are copied, which might not be the 
  // proper thing to do.
  //---------------------------------------------------------------------
  Node & 
  Node::operator=(const Node &v)
  {
    if (this != &v)
      {
	number = v.number;
	parent = v.parent;
	leftChild = v.leftChild;
	rightChild = v.rightChild;
	porder = v.porder;
	time = v.time;
	nodeTime = v.nodeTime;
	branchLength = v.branchLength;
	name = v.name;
	ownerTree = v.ownerTree;
	color = v.color;
	size = v.size;
	x = v.size;
	y = v.size;
	hostParent = v.hostParent;
	hostChild = v.hostChild;
	reconcilation = v.reconcilation;
	visited = v.visited;
      }
  
    return *this;
  }
  


  // Return the requested relative
  //----------------------------------------------------------------------
  Node* 
  Node::getLeftChild() const
  {
    return leftChild;
  }

  Node* 
  Node::getRightChild() const
  { 
    return rightChild;
  }

  Node* 
  Node::getParent() const
  {
    return parent;
  }

  Node*
  Node::getSibling() const
  {
    Node *parent = getParent();
    Node *left = parent->getLeftChild();
    if (this == left) 
      {
	return parent->getRightChild();
      } 
    else 
      {
	return left;
      }
  }

  // Retrieve the child of current node (x) that has y as a descendant. 
  // y may be a child of x, but could also be a grandchild.
  //---------------------------------------------------------------------
  Node*
  Node::getDominatingChild(Node* y)
  {
    assert(y != NULL);

    if (this == y) 
      {
	return this;		
      }
    while (y != leftChild && y != rightChild && !y->isRoot())
      {
	y = y->parent; 
      }
    return y;
  }

  // Rotate the node, i.e., switch left and right child.
  //---------------------------------------------------------------------
  void
  Node::rotate()
  {
    Node *tmp;
    tmp = leftChild;
    leftChild = rightChild;
    rightChild = tmp;
  }


  // get the (leaf) name
  //---------------------------------------------------------------------
  const string&
  Node::getName() const
  {
    return name;
  }

  // Set the owner tree
  Tree*
  Node::getTree()
  {
    if(ownerTree != 0)
      return ownerTree;
    else
      return NULL;
  }

  // Get the node's number (post-order)
  //---------------------------------------------------------------------
  unsigned
  Node::getNumber() const
  {
    return number;
  }

  unsigned
  Node::getPorder() const
  {
    return porder;
  }

  unsigned 
  Node::getNumberOfLeaves() const
  {
    if (isLeaf()) 
      return 1;
    else
      {
	unsigned left = leftChild->getNumberOfLeaves();
	unsigned right = rightChild->getNumberOfLeaves();
	return left + right;
      }
  }
  
  Real Node::getBranchLength() const
  {
     
     return branchLength;
  }

  unsigned 
  Node::getMaxPathToLeaf()
    {
      if(isLeaf())
	return 0;
      else
	{
	  unsigned left = leftChild->getMaxPathToLeaf();
	  unsigned right = rightChild->getMaxPathToLeaf();
	  return 1 + ((left > right)? left:right);
	}
    };

  SetOfNodesEx<Node>
  Node::getLeaves()
    {
        SetOfNodesEx<Node> nodes;
        if( isLeaf() ){
            nodes.insert(this);
        }
        else{
            //Find leaves recursively
            nodes = leftChild->getLeaves();
            SetOfNodesEx<Node> r = rightChild->getLeaves();
            for(unsigned int i = 0; i < r.size(); i++){
                nodes.insert(r[i]);
            }
        }
        return nodes;
    }


  //Set the (leaf) name
  //---------------------------------------------------------------------
  void 
  Node::setName(const string& nodeName)
  {
    name = nodeName;
  }

  // Set the owner tree
  //----------------------------------------------------------------------
  void 
  Node::setTree(Tree& T)
  {
    ownerTree = &T;
  }


  // setChildren
  // Hook on subtrees to a node, and let the subtrees know
  // who the parent is. Also make sure that the partial order is OK by 
  // ensuring that porder is greater than for its children.
  //----------------------------------------------------------------------
  void
  Node::setChildren(Node *l, Node *r)
  {
    this->leftChild = l;
    this->rightChild = r;
    if (l)
      {
	l->parent = this;
	if (l->porder >= porder)
	  {
	    porder = l->porder + 1;
	  }
      }
    if (r)
      {
	r->parent = this;
	if (r->porder >= porder)
	  {
	    porder = r->porder + 1;
	  }
      }
    return;
  }


  // Sets the parent of the node
  //----------------------------------------------------------------------
  void
  Node::setParent(Node *v)
  {
    parent = v;
  }

  // Change ID of this, used, e.g., in HybridTree, to ascertain condition 
  // ID < Tree.getNumberOfNodes(), when deleting hybrid or extinction nodes
  //----------------------------------------------------------------------
  void 
  Node::changeID(unsigned newID)
  {
    assert(newID < getTree()->getNumberOfNodes());
    number = newID;
  }


  // Delete all nodes lower in the tree. The current node is not deleted.
  //----------------------------------------------------------------------
  void
  Node::deleteSubtree()
  {
    if(isLeaf() == false)
      {
	leftChild -> deleteSubtree();
	delete leftChild;
	leftChild = NULL;

	rightChild -> deleteSubtree();
	delete rightChild;
	rightChild = NULL;
      }
  }


  // Checks if the current node is a leaf - A leaf lacks children
  //---------------------------------------------------------------------
  bool
  Node::isLeaf() const 
  {
    if (getLeftChild() == NULL && getRightChild() == NULL) 
      {
	return true;
      }
    else
      {
	return false;
      }
  }

  // Checks if the current node is the root. Only the root has no parent
  //---------------------------------------------------------------------
  bool
  Node::isRoot() const
  {
    if (getParent() == NULL)
      {
	return true;
      }
    else 
      {
	return false;
      }
  }

  bool
  Node::operator<=(const Node& b) const
  {
    for(const Node* c = this; c != &b; c = c->getParent())
      {
	if(c->isRoot())
	  {
	    return false;
	  }
      }
    return true;
  }


  bool
  Node::operator<(const Node& b) const
  {
    if(this == &b)
      {
	return false;
      }
    else
      {
	return operator<=(b);
      }
  }

  bool
  Node::operator<(const Node* b) const
  {
    assert(b!= 0);
    return operator<(*b);
  }

  bool
  Node::operator>(const Node& b) const
  {
    if(this == &b)
      {
	return false;
      }
    else
      {
	return b <= *this;;
      }
  }


  bool
  Node::dominates(const Node& v) const
  {
    for(const Node* w = &v; w != this; w = w->getParent())
      {
	if(w->isRoot())
	  {
	    return false;
	  }
      }
    return true;
  }

  bool
  Node::strictlyDominates(const Node& v) const
  {
    if(this == &v)
      {
	return false;
      }
    else
      {
	return dominates(v);
      }
  }
  
  Real 
  Node::getTime() const
  {
    if(this->isRoot())
      {
	return ownerTree->getTopTime();
      }
    else if(ownerTree->hasTimes())
      {
	return ownerTree->getTime(*getParent()) - ownerTree->getTime(*this);
      }
    else
      {
	return 0;
      }
  }
  
  void
  Node::setTime(const Real &t)
  {

    if(getParent())
      {
	throw AnError("Currently we disallow using setTime for non-root nodes",1);
      }
    else
      {
	ownerTree->setTopTime(t);
      }
  }
  
  // Set the branch length associated with the node
  // This does not affect time, nodeTime. 
  //---------------------------------------------------------------------
  void
  Node::setLength(const Real &newLength)
  {
    assert(getTree()->hasLengths()); // assert lengths is modeled

    if(ownerTree->hasLengths())
      {
	ownerTree->setLength(*this, newLength);
      }
    else
      {
	throw AnError("Node::setLength:\n"
		      "ownerTree->lengths is NULL",1);
      }
  }
  
  Real 
  Node::getNodeTime() const
  {
    if(ownerTree->hasTimes())
      return ownerTree->getTime(*this);
    else
      return 0;
  }
  
  Real 
  Node::getLength() const
  {
    if(ownerTree->hasLengths())
      {
	return ownerTree->getLengths()[this->getNumber()];//(*this);
      }
    else
      {
	return 0;
      }
  }

  std::ostream& 
  operator<< (std::ostream& o, const Node &v)
  {
    ostringstream oss;
    oss << "ID=" << v.getNumber();
    oss << v.stringify("NAME", v.getName())
	<< v.stringify("NT", v.getNodeTime())
	<< v.stringify("ET", v.getTime())
	<< v.stringify("BL", v.getLength());
      
    oss << v.stringify("left", v.getLeftChild())
	<< v.stringify("right", v.getRightChild())
	<< v.stringify("parent", v.getParent());
    oss << endl;

    return o << oss.str();    
  }


  std::string
  Node::stringify(string tag, Real val) const
  {
    ostringstream oss;
    oss << "\t" << tag << "=" << val;
    return oss.str();
  }

  std::string
  Node::stringify(string tag, string s) const
  {
    ostringstream oss;
    oss << "\t" << tag << "=";
    if (s.empty()) 
      {
	oss << "no";
      }
    else 
      {
	oss << "'" << s << "'";
      }
    return oss.str();
  }

  std::string
  Node::stringify(string tag, Node *v) const
  {
    ostringstream oss;
    oss << "\t" << tag;
    if (v==NULL) 
      {
	oss << "=no";
      }
    else 
      {
	oss << "=" << v->getNumber();
      }
    return oss.str();
  }

  std::ostream& 
  operator<< (std::ostream& o, const Node *v)
  {
    return operator<<(o, *v);
  }

   /*** EXTRA FEATURES ***/
   
   
  void Node::setColor(Color c)
  {
    this->color = c;
  }

  void Node::setSize(double s)
  {
    this->size = s;
  }

  void Node::setX(double x)
  {
    this->x = x;
  }

  void Node::setY(double y)
  {
    this->y = y;
  }

  void Node::setHostChild(Node* hostchild)
  {
    this->hostChild = hostchild;
  }

  void Node::setHostParent(Node* hostparent)
  {
    this->hostParent = hostparent;
  }

  void Node::setReconcilation(Type t)
  {
    this->reconcilation = t;
  }
  
  void Node::setVisited(unsigned inc)
  {
    this->visited = inc;
  }
  
  void Node::incVisited()
  {
    this->visited++;
  }

  Color Node::getColor()
  {
    return color;
  }

  double Node::getSize()
  {
    return size;
  }

  double Node::getX()
  {
    return x;
  }
  
  double Node::getY()
  {
    return y;
  }
  
 double Node::getX() const
  {
    return x;
  }
  
  double Node::getY() const
  {
    return y;
  }

  Node* Node::getHostParent()
  {
    return hostParent;
  }

  Node* Node::getHostChild()
  {
    return hostChild;
  }

  Type Node::getReconcilation()
  {
    return reconcilation;
  }
  
  Node* Node::getHostParent() const
  {
    return hostParent;
  }

  Node* Node::getHostChild() const
  {
    return hostChild;
  }

  Type Node::getReconcilation() const
  {
    return reconcilation;
  }
  
  unsigned Node::getVisited()
  {
    return visited;
  }
  
  void Node::setRightChild(Node *r )
  {
    rightChild = r;
  }

  void Node::setLeftChild(Node *l )
  {
    leftChild = l;
  }
