/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
    Author : Jose Fernandez Navarro  -  jc.fernandez.navarro@gmail.com

 */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include "Coord.hh"
#include "libraries/BeepVector.hh"

//-----------------------------------------
//This datstructure is used for the storage
//of the species tree coordinates
//For a detailed description of this storage,
//see the HTML-documentation
//------------------------------------------

  using namespace std;

  Coord::Coord()
  {
    for (int i = 0; i < 3; i++)
      {
	data.push_back(make_pair(0.0,0.0));
      }
  }

  Coord::Coord(const Coord& c)
    :data(c.data)
  {}

  pair<double,double>&
  Coord::operator[](int pos)
  {
    return data[pos];
  }
  
  double&
  Coord::operator()(int pos,int coord)
  {
    if(coord == 0)
      {
	return data[pos].first;
      }
    else
      {
	return data[pos].second;
      }
  }

  typedef BeepVector<Coord> CoordVector;