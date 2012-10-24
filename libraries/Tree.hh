#ifndef TREE_HH
#define TREE_HH

#include <map>
#include <string>
#include <vector>

  // Forward declarations.
  class Node;
  class RealVector;
  using namespace std;
  typedef float Real;

  const unsigned DEF_NODE_VEC_SIZE = 100;
  
  class Tree 
  {
  public:

    Tree();
    Tree(const Tree &T);
    static Tree EmptyTree(const Real& RootTime = 1.0, std::string leafname = "Leaf");
    
    virtual ~Tree();
    virtual Tree& operator=(const Tree& T);
    virtual std::string getName() const; 
    virtual void setName(std::string s); 
    virtual unsigned getNumberOfNodes() const; 
    virtual unsigned getNumberOfLeaves() const;
    virtual unsigned getHeight() const;
    virtual bool IDnumbersAreSane(Node& n);
    virtual void clear();
    virtual Node * getRootNode() const; 
    virtual void setRootNode(Node *r); 
    virtual Node* getNode(unsigned nodeNumber);
    virtual Node* getNode(unsigned nodeNumber) const;
    virtual Node* findLeaf(const std::string& name) const;
    virtual Node* findNode(const std::string& name) const;
    virtual Node* addNode(Node *leftChild, Node *rightChild, unsigned id, std::string name = ""); 
    virtual Node* addNode(Node *leftChild,  Node *rightChild, std::string name = ""); 

    /* annoying methods I want to get rid of */
    virtual bool hasTimes() const;
    virtual bool hasRates() const;
    virtual bool hasLengths() const;
    virtual Real getTime(const Node& v) const;
    virtual Real getEdgeTime(const Node& v) const;
    virtual Real getLength(const Node& v) const;
    virtual Real getRate(const Node& v) const;
    virtual void setTimeNoAssert(const Node& v, Real time) const;
    virtual void setTime(const Node& v, Real time) const;
    virtual void setEdgeTime(const Node& v, Real time) const;
    virtual void setLength(const Node& v, Real weight)const;
    virtual void setRate(const Node& v, Real rate)const;
    virtual RealVector& getTimes() const;
    virtual RealVector& getRates() const;
    virtual RealVector& getLengths() const;
    virtual void setTimes(RealVector& v) const;
    virtual void setRates(RealVector& v) const;
    virtual void setLengths(RealVector& v) const;
    virtual bool checkTimeSanity(Node& root) const;
    virtual const Real& getTopTime() const;
    virtual void setTopTime(Real newTime);
    virtual Real rootToLeafTime() const;
    virtual Real getTopToLeafTime() const;

    
    typedef std::vector<Node*>::iterator iterator;
    typedef std::vector<Node*>::const_iterator const_iterator;
    typedef std::vector<Node*>::reverse_iterator reverse_iterator;
    typedef std::vector<Node*>::const_reverse_iterator const_reverse_iterator;

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

    virtual Node* mostRecentCommonAncestor(Node* a, Node* b) const;
    virtual const Node* mostRecentCommonAncestor(const Node* a, const Node* b) const;
    virtual Real imbalance();
    //friend std::ostream& operator<<(std::ostream &o, const Tree& T);
    //virtual std::string print(bool useET, bool useNT, bool useBL, bool useER) const;
    //virtual std::string print() const;

    virtual Node* copyAllNodes(const Node* v);
    virtual Node* copySubtree(const Node *v);
    virtual void clearTree();
    virtual void clearNodeAttributes();
    virtual Real imbalance(Node *v);
    virtual unsigned getHeight(Node* v) const;
    
  protected:
    
    unsigned noOfNodes;		            
    unsigned noOfLeaves;	               
    Node * rootNode;                        
    std::map<std::string, Node*> name2node; 
    std::vector<Node*> all_nodes;           
    std::string name;		
    
    /* puag, mutable, really? */
    mutable RealVector* times;
    mutable RealVector* rates;
    mutable RealVector* lengths;
    mutable Real topTime;   
    
  };

#endif