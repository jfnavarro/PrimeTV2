#ifndef NODE_HH
#define NODE_HH
#include <iostream>
#include <string>
#include "Color.h"
#include <map>
#include <libraries/SetOfNodesEx.hh>

    using namespace std;
    class Tree;
    
    enum  Type{Leaf=0, Speciation=1, Duplication=2, LateralTransfer=3, Undefined=4};
    
    typedef float Real;

  class Node
  {

  public:
    
    Node(unsigned id);
    Node(unsigned id, const std::string& nodeName);
    virtual ~Node();
    Node(const Node &);
    
    /* Extra methods added by Marco P. **/
    enum structureType {
	NOTINIT = 0, XTRA = 1, MP = 2
    };
	
    struct valueInfo 
    {
      Node* swapNode;
      int type;
      double value;
      int XtraIndex;

      valueInfo(Node* sNode) {
	swapNode = sNode;
	type = 0;
	value = 0;
      }

      valueInfo(Node* sNode, int ty, double val) : type(ty), value(val) {
	  swapNode = sNode;
      }
    };
    
    /* Extra methods added by Marco P. **/	
	
    //Extra Methods aded by Jose Carlos Fernandez
    void setColor(Color c);
    void setSize(double s);
    void setX(double x);
    void setY(double y);
    void setHostParent( Node *hostparent);
    void setHostChild( Node *hostchild);
    void setReconcilation(Type t);
    void setVisited(unsigned inc);
    void incVisited();
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
    int getXtraIndex();
    int getXtraIndex() const;
    //Extra Methods aded by Jose Carlos Fernandez
    
    /* Extra methods added by Marco P. **/
    void setXtraIndex(int xtraNIndex);
    void setBinary(bool);
    bool isBinary();
    bool isBinary() const;
    void addNodeSwap(Node* speciesNode, Node* swappedNode);
    Node* getSwappedNode(Node*);
    bool hasBeenSwapped();
    int set(Node*, int, double, int);
    std::map<int,Node::valueInfo>::iterator find(Node*);
    Node* getBinaryParent();
    void setBinaryParent(Node*);
    Node* getBinarySibling();	
    void setBinaryLeftChild(Node*);
    void rotateBinaryChild();
    void setBinaryRightChild(Node*);
    Node* getBinaryLeftChild();
    Node* getBinaryRightChild();
    void setLayoutIndex(int layoutIndex);
    int getLayoutIndex();
    void setBinarySibling(Node*);
    void setDuplication(bool);
    bool isDuplication();
    void init();
    bool subTrComplExpl();
    void setSubTrComplExpl(bool);
    
    void rotate();
    Node* getLeftChild() const;
    Node* getRightChild() const;
    Node* getParent() const;
    Node* getSibling() const;
    Node* getDominatingChild(Node* y);
    const std::string& getName() const;
    Tree* getTree();
    unsigned getNumber() const;
    unsigned getPorder() const;
    unsigned getNumberOfLeaves() const;
    Real getBranchLength() const;
    Node& operator=(const Node &);
    unsigned getMaxPathToLeaf();
    void setName(const std::string& nodeName);
    void setTree(Tree& T);
    void setChildren(Node *left, Node *right);
    void setParent(Node *parent);
    void changeID(unsigned newID);
    void deleteSubtree();
    SetOfNodesEx<Node> getLeaves();
    bool isLeaf() const;		
    bool isRoot() const;		
    bool operator<=(const Node& b) const;
    bool operator<(const Node& b) const;
    bool operator<(const Node* b) const;
    bool operator>(const Node& b) const;
    bool dominates(const Node &b) const;
    bool strictlyDominates(const Node &b) const;
    
    friend std::ostream& operator<< (std::ostream& o, const Node &v);
    friend std::ostream& operator<< (std::ostream& o, const Node *v);

  private:

    std::string stringify(std::string tag, Real val) const;
    std::string stringify(std::string tag, std::string val) const;
    std::string stringify(std::string tag, Node *v) const;

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
    
    /* extra features Jose Carlos Fernandez */
    Color color;
    double size;
    double x;
    double y;
    Node *hostParent;
    Node *hostChild;
    Type reconcilation;
    unsigned visited;
    
    /* extra features Marco P. */
    typedef std::map<int, valueInfo> mapType;
    mapType NMap;
    int xtraNIndex;
    std::map<int, Node*> nodeMap;
    int layoutIndex;
    Node *binaryParent;
    Node *binarySibling;
    Node *binaryLeftChild;
    Node *binaryRightChild;
    bool binary;
    bool swapped;
    bool subTrExpl;
    /* extra features Marco P. */
    
  public:
    
    static unsigned int maxID;

    Real getNodeTime() const;
    Real getTime() const;
    Real getLength() const;
    bool changeNodeTime(const Real &t);
    bool changeTime(const Real &et);
    void setNodeTime(const Real &t);
    void setTime(const Real &t); 
    void setLength(const Real &newLength);

  };

#endif