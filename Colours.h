#ifndef COLOURS_H
#define COLOURS_H
#include "Color.h"
#include <string>
#include <sstream>

using namespace std;

class Colours
{

  
  public:
    
    Color default_foreground_color; // Draw the sequence names
    Color genetree_color;  //circles and rectangules of species
    Color species_edge_color; //color edge
    Color snode_contour_color; //color node

    Color sedge_contour_color; //marker??
    Color species_node_color; //for marker
    Color markerColor; //marker color
    Color umColor;

    Colours();
    void setColors(const char *set);
    const char* getSet();
    
private :
   
   const char *set;
};



#endif