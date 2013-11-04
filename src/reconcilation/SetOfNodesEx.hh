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

#ifndef SETOFNODESEX_HH
#define SETOFNODESEX_HH

#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <sstream>

#include "../utils/AnError.hh"

using namespace std;

template <class T>
class SetOfNodesEx
{

public:
    SetOfNodesEx();
    SetOfNodesEx(const SetOfNodesEx<T>& SON);
    ~SetOfNodesEx();

    SetOfNodesEx<T>& operator=(const SetOfNodesEx<T> &son);

    void insert(T* u);	// Add u to the set
    void insertVector(std::vector<T*>& v);  //Add a vector to the set - NOTE! RISK THAT THIS FUNCTION WILL BECHANGED!
    void erase(T* u);	// Remove u from the set
    bool member(T* u) const; // Is u a member of the set?
    bool empty() const;		// Is the set empty or not?
    unsigned size() const;	// Number of elements in set
    T* operator[](unsigned i) const; // Random access
    bool operator==(const SetOfNodesEx<T> &s1) const;
    bool operator<(const SetOfNodesEx<T> &s1) const;
    std::ostream& operator<<(std::ostream& os);
    std::string strRep() const;
    std::string set4os() const;
private:
    typename std::set<T*> theSet;
};



//************************************************************************************************///
// IMPLEMENTATION //

template <class T>
std::ostream& SetOfNodesEx<T>::operator<<(std::ostream& os)
{
    return os << "Class SetOfNodes:\n"
              << "Holds and provides access (using operator[]) to a set\n"
              << " of nodes.\n"
              << "Attributes:\n"
              << "   theSet: \n"
              << set4os();
}

template <class T>
std::string SetOfNodesEx<T>::strRep() const
{
    std::stringstream ss;
    for(unsigned int i = 0; i < theSet.size(); i++)
    {
        const T *node = operator[](i);
        ss << node->getNumber() << " ";
    }
    return ss.str();
}

// helper function for operator <<
//----------------------------------------------------------------------

template <class T> std::string 
SetOfNodesEx<T>::set4os() const
{
    std::ostringstream os;
    os << "   ";

    for(typename std::set<T*>::const_iterator i = theSet.begin();
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
}

template <class T> bool 
SetOfNodesEx<T>::operator==(const SetOfNodesEx<T> &s1) const
{
    return (this->theSet == s1.theSet);
};

template <class T> bool 
SetOfNodesEx<T>::operator<(const SetOfNodesEx<T> &s1) const
{
    return (this->theSet < s1.theSet);
};

template <class T>
SetOfNodesEx<T>::SetOfNodesEx()
{
    // Start with empty set!
}

template <class T>
SetOfNodesEx<T>::SetOfNodesEx(const SetOfNodesEx<T>& SON)
    : theSet(SON.theSet)
{
}

template <class T>
SetOfNodesEx<T>::~SetOfNodesEx()
{
}

template <class T>
SetOfNodesEx<T>& 
SetOfNodesEx<T>::operator=(const SetOfNodesEx<T> &son)
{
    if (this != &son)
    {
        theSet = son.theSet;
    }

    return *this;
}

template <class T>
void
SetOfNodesEx<T>::insert(T *u)
{
    theSet.insert(u);
}

template <class T>
void
SetOfNodesEx<T>::insertVector(std::vector<T*>& v)
{
    //I am simply not sure how sets work, so I hope this ugly thing works
    theSet.insert(v.begin(), v.end());
}

template <class T>
void
SetOfNodesEx<T>::erase(T *u)
{
    typename std::set<T *>::iterator iter = theSet.find(u);
    theSet.erase(iter);
}

template <class T>
bool
SetOfNodesEx<T>::member(T *u) const
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

template <class T>
bool
SetOfNodesEx<T>::empty() const
{
    return theSet.empty();
}

template <class T>
unsigned
SetOfNodesEx<T>::size() const
{
    return theSet.size();
}


template <class T>
T* 
SetOfNodesEx<T>::operator[](unsigned i) const
{
    unsigned j;
    typename std::set<T*>::iterator iter;
    for (j = 0, iter = theSet.begin(); j < i; iter++, j++)
    {
        // Walk on up
    }
    return *iter;
}


#endif
