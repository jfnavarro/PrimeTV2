#ifndef STRSTRMAP_H
#define STRSTRMAP_H

#include <string>
#include <map>
#include <stdexcept>


namespace beep
{

  //---------------------------------------------------------------------
  //! Store maps of strings to other strings. (Think gene names to species
  //! names.) Strings in the interface are char pointers, but internally, 
  //! I use C++ std class string until we go string in the whole app.
  //!
  //! \todo{This class is a candidate for a rewrite such that it is a subclass
  //! of map<string,string> rather than a, eeh, container for it.}
  //
  // Please notice that this file also contain the subclass DummyStrStrMap
  //! Author: Lars Arvestad, SBC, © the MCMC-club, SBC, all rights reserved
  //---------------------------------------------------------------------
  class StrStrMap
  {
  public:
    //---------------------------------------------------------------------
    // 
    // Construct / Destruct
    //
    //---------------------------------------------------------------------
    StrStrMap();
    virtual ~StrStrMap();
    StrStrMap(const StrStrMap& sm);

    StrStrMap& operator=(const StrStrMap& sm);


    
    //---------------------------------------------------------------------
    // 
    // Interface
    //
    //---------------------------------------------------------------------

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

    //---------------------------------------------------------------------
    //! I/O
    //---------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream &o, const StrStrMap &);
    
    //---------------------------------------------------------------------
    //
    // Attributes
    //
    //---------------------------------------------------------------------
  private:
    std::map<std::string, std::string> avbildning;	// Stores the mapping
    
  };

  //---------------------------------------------------------------------
  //
  //! Dummy map from any sequence to one species when no species tree is used.
  //
  //---------------------------------------------------------------------
  class StrStrMapDummy : public StrStrMap 
  {
  public:
    //---------------------------------------------------------------------
    //
    // Construct/desctruct/assign
    // 
    //---------------------------------------------------------------------
    StrStrMapDummy( const std::string& theSpecies ) 
      : s(theSpecies) 
    {}

    virtual ~StrStrMapDummy() 
    {}
    

    virtual void insert(const std::string &u, const std::string &x) 
    {
      throw std::runtime_error(std::string("Not implemented"));
    }

    virtual std::string getNthItem(unsigned idx) const 
    {
      throw std::runtime_error(std::string("Not implemented"));
    }

    virtual std::string find(const std::string &u) const 
    {
      return s; 
    }
     
    //---------------------------------------------------------------------
    //
    // Attributes
    // 
    //---------------------------------------------------------------------
 private:
    std::string s;
        
  };

}//end namespace beep


#endif
