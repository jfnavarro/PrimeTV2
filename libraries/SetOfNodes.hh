// We want to store a set of Node objects in order to map species
// nodes to anti-chains.
//
// The implementation makes use of the set template in STL
//
// The main reason for this class is to mimic random access for sets.
//
#ifndef SETOFNODES_HH
#define SETOFNODES_HH

#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <sstream>
#include "Node.hh"

namespace beep
{
  // forward declartations
  class SetOfNodes;
  bool operator==(const SetOfNodes &s1, const SetOfNodes &s2);
  bool operator<(const SetOfNodes &s1, const SetOfNodes &s2);

  //! Author: Lars Arvestad, SBC, � the MCMC-club, SBC, all rights reserved
  class SetOfNodes
  {
  public:
    SetOfNodes();
    SetOfNodes(const SetOfNodes& SON);
    ~SetOfNodes();

    SetOfNodes& operator=(const SetOfNodes &son);

    void insert(beep::Node* u);	// Add u to the set
    void insertVector(std::vector<beep::Node*>& v);  //Add a vector to the set - NOTE! RISK THAT THIS FUNCTION WILL BECHANGED!
    void erase(beep::Node* u);	// Remove u from the set
    bool member(beep::Node* u) const; // Is u a member of the set?
    bool empty() const;		// Is the set empty or not?
    unsigned size() const;	// Number of elements in set
    beep::Node* operator[](unsigned i) const; // Random access
  
    //----------------------------------------------------------------------
    //
    // I/O
    //
    //----------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const SetOfNodes& S)
    {
      return os << "Class SetOfNodes:\n"  
		<< "Holds and provides access (using operator[]) to a set\n"
		<< " of nodes.\n"
		<< "Attributes:\n"
		<< "   theSet: \n"
		<< S.set4os()
	// 	      << "\n"
	;
    };
  
    std::string strRep() const
    {
        std::stringstream ss;
        for(unsigned int i = 0; i < theSet.size(); i++){
            const Node *node = operator[](i);
            ss << node->getNumber() << " ";
        }
        return ss.str();
    };

    // helper function for operator <<
    //----------------------------------------------------------------------
    std::string set4os() const
    {
      std::ostringstream os;
      os << "   ";
      for(std::set<beep::Node*>::const_iterator i = theSet.begin();
	  i != theSet.end(); i++)
	{
	  if(*i)
	    os << (*i)->getNumber();
	  else
	    os << "NULL";
	  os << "\t";
	}
      os << "\n";
      return os.str();
    };

    friend bool 
    operator==(const SetOfNodes &s1, const SetOfNodes &s2)
    {
      return (s1.theSet == s2.theSet);
    };

    friend bool 
    operator<(const SetOfNodes &s1, const SetOfNodes &s2)
    {
      return (s1.theSet < s2.theSet);
    };


  private:
    std::set<beep::Node* > theSet;
  };

}//end namespace beep

#endif