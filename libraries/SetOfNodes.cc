#include "SetOfNodes.hh"

// Author: LArs Arvestad, © the MCMC-club, SBC, all rights reserved
namespace beep
{
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
  SetOfNodes::insert(beep::Node *u)
  {
    theSet.insert(u);
  }

  void
  SetOfNodes::insertVector(vector<beep::Node*>& v)
  {
    //I am simply not sure how sets work, so I hope this ugly thing works
    theSet.insert(v.begin(), v.end());
    //Alternative 
    //   for(unsigned n = 0; n < v.size(); n++)
    //     {
    //       theSet.insert(v[n]);
    //     }
  }

  void
  SetOfNodes::erase(beep::Node *u)
  {
    set<beep::Node *>::iterator iter = theSet.find(u);
    theSet.erase(iter);
  }


  bool
  SetOfNodes::member(beep::Node *u) const
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



  beep::Node* 
  SetOfNodes::operator[](unsigned i) const
  {
    unsigned j;
    set<beep::Node*>::iterator iter;
    for (j = 0, iter = theSet.begin(); 
	 j < i;
	 iter++, j++)
      {
	// Walk on up
      }
    return *iter;
      
  }

}//end namespace beep
