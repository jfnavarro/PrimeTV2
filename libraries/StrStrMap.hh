#ifndef STRSTRMAP_H
#define STRSTRMAP_H

#include <string>
#include <map>
#include <stdexcept>
  
  // Author: Lars Arvestad, © the MCMC-club, SBC, all rights reserved

  using namespace std;

  class StrStrMap 
  {
  public:

    StrStrMap();
    virtual ~StrStrMap();
    StrStrMap(const StrStrMap& sm);

    StrStrMap& operator=(const StrStrMap& sm);

    //! Adding relations 
    //! u is a gene name x is a species name
    void insert(const std::string &u, const std::string &x);	// Map u to x.
    void change(const std::string &u, const std::string &x);	// Map u to x.

    //! Retrieval. What does u map to? 
    //! The empty string is returned when not in map.
    virtual std::string find(const std::string &u) const ;

    //! Random access to a "left" item, use that to retrieve "right" item.
    std::string getNthItem(unsigned idx) const; 

    //! reset map
    void clearMap();

    //! Diagnostics. The number of relations
    unsigned size() const;		
    unsigned reverseSize() const;	
    
    //! returns the mapping
    std::map<std::string, std::string> getMapping() const
    {
      return avbildning;
    }
    
    friend std::ostream& operator<<(std::ostream &o, const StrStrMap &);

  private:
    
    std::map<std::string, std::string> avbildning;	// Stores the mapping
    
  };

#endif