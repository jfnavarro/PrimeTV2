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
    Color gene_edge_color;
    Color gene_lgt_color;	/* Not in use! */
    Color gene_dupl_color;
    Color gene_spec_color;

    Color species_edge_color;
    Color species_edge_contour_color;
    Color species_node_color; 
    Color species_node_contour_color;

    Color markerColor; //marker color
    Color umColor;

    Colours();
    void setColors(const char *set);
    const char* getSet();
    
private :
   
   const char *set;
};



#endif
