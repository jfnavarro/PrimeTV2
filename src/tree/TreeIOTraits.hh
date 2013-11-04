/*
    PrimeTV2 : a visualizer for phylogenetic reconciled trees.
    Copyright (C) 2011  <Jose Fernandez Navarro> <jc.fernandez.navarro@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    
    Author : Jose Fernandez Navarro  -  jc.fernandez.navarro@gmail.com
             Lars Arvestad, © the MCMC-club, SBC, all rights reserved
 */

#ifndef TREEIOTRAITS_HH
#define TREEIOTRAITS_HH
 
#include <string>

using namespace std;

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
//

class TreeIOTraits 
{
private:
    enum 
    { 
        nw,                 // node weights
        et,                 // edge times
        nwiset,             // flag for when node weights should be interpreted as edge times
        nt,                 // node times
        bl,                 // "implementation specific"
        tt,                 // top time
        ac,                 // "anti-chains", reconciliation information
        gs,                 // gene-species map
        hy,                 // hybrid tree
        id,                 // node identifiers
        name,	         	// tree name
        last_item_dont_use	// This element determines how many items we have added.
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
    void logicAnd(const TreeIOTraits &t);

    //! All combinations of traits are either not sane or not wanted. This
    //! function enforces such constraints.
    void enforceStandardSanity();

    //! Set characteristics for a host/guest/Newick tree.
    void enforceHostTree();
    void enforceGuestTree();
    void enforceNewickTree();

    //! Check if there is any type of time information available, for edges,
    //!  nodes, or by branchlengths.
    bool containsTimeInformation();  

    std::string print();

private:
    bool traits[last_item_dont_use];

};

#endif
