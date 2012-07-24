#ifndef TREEIOTRAITS_HH
#define TREEIOTRAITS_HH
 
#include <string>

//namespace beep
//{
  //
  // TreeIOTraits
  //
  //! Store and manipulate tree attributes.
  //!
  //! There are two main uses for this class:
  //!
  //! + Keep track of what attributes are known and used for a tree.
  //! + Keep track of what is wanted for a tree.
  //! 
  //! In the first use-case, we may look what is available for a tree when
  //! reading it from a file. In the second use-case, we decide what will
  //! be printed.
  //! Author: LArs Arvestad, © the MCMC-club, SBC, all rights reserved
  //

  class TreeIOTraits 
  {
  private:
    enum 
      { 
	nw,                     // node weights
	et,			// edge times
	nwiset,	                // flag for when node weights should be 
	                        // interpreted as edge times
	nt,			// node times
	bl,			// "implementation specific" 
 	tt,                      // top time
	ac,			// "anti-chains", reconciliation information
	gs,			// gene-species map
	hy,			// hybrid tree
	id,			// node identifiers
	name,	         	// tree name
	
	last_item_dont_use	// This element determines how many items 
	                        // we have added.
      };
    
  public:
    //! All traits are initiated as false!
    TreeIOTraits();
    ~TreeIOTraits();
        
    TreeIOTraits(const TreeIOTraits &tr);
    
    //! This set of accessors are here to make the transition from the old
    //! set of bools to using this class easier and safer to do. There
    //! are probably prettier and less cluttered interfaces, but this will
    //! have to do for now.
    bool hasNW() const;
    bool hasET() const;
    bool hasNWisET() const;
    bool hasNT() const;
    bool hasBL() const;
    bool hasAC() const;
    bool hasGS() const;
    bool hasHY() const;
    bool hasID() const;
    bool hasName() const;
    bool hasTT() const;
    
    void setNW(bool b);
    void setET(bool b);
    void setNWisET(bool b);
    void setNT(bool b);
    void setBL(bool b);
    void setAC(bool b);
    void setGS(bool b);
    void setHY(bool b);
    void setID(bool b);
    void setName(bool b);
    void setTT(bool b);
    
    //! Given what traits are available and what is wanted, determine the
    //! smallest common set of traits.
    void logicAnd(const TreeIOTraits t);
    
    //! All combinations of traits are either not sane or not wanted. This
    //! function enforces such constraints.
    void enforceStandardSanity();
    
    //! Set characteristics for a host/guest/Newick tree.
    //@{
    void enforceHostTree();
    void enforceGuestTree();
    void enforceNewickTree();
    //@}

    //! Check if there is any type of time information available, for edges,
    //!  nodes, or by branchlengths.
    bool containsTimeInformation();  

    //! \name IO
    //!@{
    std::string print();
    //!@}
  private:
    bool traits[last_item_dont_use];
    
  };
//} //end namespace beep

#endif
