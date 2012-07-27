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
#ifndef COORD_H
#define COORD_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include "libraries/BeepVector.hh"

// Class that abstracts the pair<double,double> as a Coord object

  using namespace std;
  
  class Coord{

  public:
    Coord();
    Coord(const Coord& c);
    pair<double,double>& operator[](int pos);
    double& operator()(int pos,int coord);

  private:
    vector<pair<double,double> > data;

  };

typedef BeepVector<Coord> CoordVector;


#endif