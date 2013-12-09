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
             Lars Arvestad, the MCMC-club, SBC, all rights reserved
 */
#ifndef STRSTRMAP_H
#define STRSTRMAP_H

#include <string>
#include <map>
#include <stdexcept>

using namespace std;

class StrStrMap 
{
public:

    StrStrMap();
    virtual ~StrStrMap();
    StrStrMap(const StrStrMap& sm);

    StrStrMap& operator=(const StrStrMap& sm);

    // Adding relations
    // u is a gene name x is a species name
    void insert(const std::string &u, const std::string &x);	// Map u to x.
    void change(const std::string &u, const std::string &x);	// Map u to x.

    // Retrieval. What does u map to?
    // The empty string is returned when not in map.
    virtual std::string find(const std::string &u) const ;

    // Random access to a "left" item, use that to retrieve "right" item.
    std::string getNthItem(unsigned idx) const;

    // reset map
    void clearMap();

    // Diagnostics. The number of relations
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
