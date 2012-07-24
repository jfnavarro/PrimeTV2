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


//namespace beep 
//{

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

//}


