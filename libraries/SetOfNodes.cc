#include "SetOfNodes.hh"

// Author: LArs Arvestad, © the MCMC-club, SBC, all rights reserved

  using namespace std;

  SetOfNodes::SetOfNodes()
  {
    // Start with empty set!
  }

  SetOfNodes::SetOfNodes(const SetOfNodes& SON)
    : theSet(SON.theSet)
  {
  }

  SetOfNodes::~SetOfNodes()
  {
  }

  SetOfNodes& 
  SetOfNodes::operator=(const SetOfNodes &son)
  {
    if (this != &son)
      {
	theSet = son.theSet;
      }

    return *this;
  }


  void
  SetOfNodes::insert(Node *u)
  {
    theSet.insert(u);
  }

  void
  SetOfNodes::insertVector(vector<Node*>& v)
  {
    //I am simply not sure how sets work, so I hope this ugly thing works
    theSet.insert(v.begin(), v.end());
  }

  void
  SetOfNodes::erase(Node *u)
  {
    set<Node *>::iterator iter = theSet.find(u);
    theSet.erase(iter);
  }


  bool
  SetOfNodes::member(Node *u) const
  {
    if (theSet.find(u) == theSet.end())
      {
	return false;
      }
    else
      {
	return true;
      }
  }


  bool
  SetOfNodes::empty() const
  {
    return theSet.empty();
  }


  unsigned
  SetOfNodes::size() const
  {
    return theSet.size();
  }



  Node* 
  SetOfNodes::operator[](unsigned i) const
  {
    unsigned j;
    set<Node*>::iterator iter;
    for (j = 0, iter = theSet.begin(); 
	 j < i;
	 iter++, j++)
      {
	// Walk on up
      }
    return *iter;
      
  }
  
  std::set<Node*>::iterator SetOfNodes::search(Node *u){
	  return theSet.find(u);
  }
