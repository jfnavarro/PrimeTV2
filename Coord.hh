#ifndef COORD_H
#define COORD_H
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include "libraries/BeepVector.hh"

// Class the abstracts the pair<double,double> as a Coord object

//namespace beep 
//{

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


//}


#endif
