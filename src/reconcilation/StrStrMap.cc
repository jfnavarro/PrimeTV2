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

#include  "StrStrMap.hh"

#include <string.h>
#include <set>
#include <map>
#include <iostream>
#include "../utils/AnError.hh"

// // Author: Lars Arvestad, © the MCMC-club, SBC, all rights reserved

using namespace std;

StrStrMap::StrStrMap() 
    : avbildning()
{
}

StrStrMap::~StrStrMap() 
{
}

StrStrMap::StrStrMap(const StrStrMap& sm)
    : avbildning(sm.avbildning)
{    
}

StrStrMap& 
StrStrMap::operator=(const StrStrMap &sm)
{
    if(&sm != this)
    {
        avbildning = sm.avbildning;
    }
    return *this;
}

void
StrStrMap::insert(const string &x, const string &y)
{
    avbildning.insert(pair<string,string>(x, y));
}

void
StrStrMap::change(const string &x, const string &y)
{
    if(avbildning.find(x) != avbildning.end())
    {
        avbildning[x] = y;
    }
    else
    {
        avbildning.insert(pair<string,string>(x, y));
    }
}

// Retrieval
std::string
StrStrMap::find(const string &s) const
{
    map<string, string>::const_iterator iter;

    iter = avbildning.find(s);
    if (iter == avbildning.end())
    {
        return "";
    }
    else
    {
        return iter->second;
    }
}

std::string
StrStrMap::getNthItem(unsigned idx) const
{
    for (map<string,string>::const_iterator i = avbildning.begin();i != avbildning.end(); i++)
    {
        if (idx == 0)
        {
            return i->first;
        }
        else
        {
            idx--;
        }
    }
    //PROGRAMMING_ERROR("Out of bounds.");
    return("");
}

// reset map
void
StrStrMap::clearMap()
{
    avbildning.clear();
}

// Diagnostics. Find how many relations are stored
unsigned
StrStrMap::size() const
{
    return avbildning.size();
}

unsigned 
StrStrMap::reverseSize() const
{
    set<string> reverse;
    for(map<string,string>::const_iterator i = avbildning.begin(); i != avbildning.end(); i++)
    {
        if(reverse.find(i->second) != reverse.end())
            reverse.insert(i->second);
    }
    return reverse.size();
}

std::ostream& 
operator<<(std::ostream &o, const StrStrMap &m)
{
    string res;

    for (map<string,string>::const_iterator i = m.avbildning.begin(); i != m.avbildning.end();i++)
    {
        res.append(i->first + "\t" + i->second + "\n");
    }
    return o << res;
}
