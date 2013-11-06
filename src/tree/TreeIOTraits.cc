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

#include "TreeIOTraits.hh"

#include "../utils/AnError.hh"
#include <sstream>

using namespace std;

TreeIOTraits::TreeIOTraits() 
{
	// We turn off everything by default. If this is changed,
	// there will be bugs all over TreeIO!
	for (int i=nw; i<last_item_dont_use; i++) 
	{
		traits[i] = false;
	}
}

TreeIOTraits::~TreeIOTraits() 
{}

TreeIOTraits::TreeIOTraits(const TreeIOTraits &tr)
{
	for (int i=nw; i<last_item_dont_use; i++) 
	{
		traits[i] = tr.traits[i];
	}
}

//! This set of accessors are here to make the transition from the old
//! set of bools to using this class easier and safer to do. There
//! are probably prettier and less cluttered interfaces, but this will
//! have to do for now.
bool 
TreeIOTraits::hasNW() const 
{
    return traits[nw];
}

bool 
TreeIOTraits::hasET() const 
{
	return traits[et];
}

bool 
TreeIOTraits::hasNWisET() const 
{
	return traits[nwiset];
}

bool 
TreeIOTraits::hasNT() const
{
	return traits[nt];
}

bool 
TreeIOTraits::hasBL() const
{
	return traits[bl];
}

bool 
TreeIOTraits::hasAC() const 
{
	return traits[ac];
}

bool 
TreeIOTraits::hasGS() const 
{
	return traits[gs];
}

bool 
TreeIOTraits::hasHY() const
{
	return traits[hy];
}

bool 
TreeIOTraits::hasID() const
{
	return traits[id];
}

bool 
TreeIOTraits::hasName() const
{
	return traits[name];
}

bool 
TreeIOTraits::hasTT() const
{
	return traits[tt];
}

void 
TreeIOTraits::setNW(bool b) 
{ 
	traits[nw] = b;
}

void 
TreeIOTraits::setET(bool b)
{
	traits[et] = b;
}

void 
TreeIOTraits::setNWisET(bool b)
{ 
	traits[nwiset] = b;
}

void 
TreeIOTraits::setNT(bool b)
{
	traits[nt] = b;
}

void 
TreeIOTraits::setBL(bool b)
{
	traits[bl] = b;
}

void 
TreeIOTraits::setAC(bool b)
{ 
	traits[ac] = b;
}

void 
TreeIOTraits::setGS(bool b) 
{
	traits[gs] = b;
}

void 
TreeIOTraits::setHY(bool b)
{
	traits[hy] = b;
}

void 
TreeIOTraits::setID(bool b)
{ 
	traits[id] = b;
}

void 
TreeIOTraits::setName(bool b)
{
	traits[name] = b;
}

void 
TreeIOTraits::setTT(bool b) 
{
	traits[tt] = b;
}

void 
TreeIOTraits::logicAnd(const TreeIOTraits &t)
{
	for (int i=nw; i<last_item_dont_use; i++) 
	{
		traits[i] = traits[i] && t.traits[i];
	}
}

void 
TreeIOTraits::enforceStandardSanity()
{
	setNT(hasNT() && !hasET());	// if hasET then discard NT
	setBL(hasBL() || (hasNW() && (hasNWisET() ==false)));
}

void 
TreeIOTraits::enforceHostTree()
{
	setBL(false);
	setAC(false);
	setGS(false);
	if(hasNT())
	{
		setNW(false);
		setET(false);
	}
	else if(hasNW()) 
	{
		setNWisET(!hasET());
		setET(true);
	}
	else if(hasET() == false)
	{
        throw AnError("TreeIOTraits::enforceHostTree:\n no time info in tree", 1);
	}
}

void 
TreeIOTraits::enforceGuestTree()
{
	if(hasBL())
	{}
	else if(hasNW())
	{
		setNWisET(false);
	}
	else
	{
		throw AnError("TreeIOTraits::enforceGuestTree:\n"
				"no branch length info in tree", 1);
	}
}

void 
TreeIOTraits::enforceNewickTree()
{
	if(hasNW())
	{
		setNWisET(false);
		setBL(false);
		setNT(false);
		setET(false);
		setAC(false);
		setGS(false);
	}
	else
	{
		throw AnError("TreeIOTraits::enforceNewickTree:\n"
				"no newick branch length info in tree", 1);
	}    
}

bool TreeIOTraits::containsTimeInformation()
{
	return hasNW() || hasET() || hasNT();
}

string TreeIOTraits::print()
{
	ostringstream oss;
	oss << "TreeIOTraits:"
	<< "\nhasID = "
	<< (hasID()?"true":"false")
	<< "\nhasET = "
	<< (hasET()?"true":"false")
	<< "\nhasNT = "
	<< (hasNT()?"true":"false")
	<< "\nhasBL = "
	<< (hasBL()?"true":"false")
	<< "\nhasNW = "
	<< (hasNW()?"true":"false")
	<< "\nhasNWisET = "
	<< (hasNWisET()?"true":"false")
	<< "\nhasAC = "
	<< (hasAC()?"true":"false")
	<< "\nhasGS = "
	<< (hasGS()?"true":"false")
	<< "\nhasHY = "
	<< (hasHY()?"true":"false")
	// 	<< "\nhasTT = "
	// 	<< (hasTT()?"true":"false")
	<< "\nhasName = "
	<< (hasName()?"true":"false");
	return oss.str();
}
