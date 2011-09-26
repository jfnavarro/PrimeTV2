#include "Colours.h"
#include <string.h>
#include <strings.h>
#include <sstream>

using namespace std;
 

 //Constructor, color combination 1 by default
 Colours::Colours()
 {
    this->default_foreground_color = Color(0.0,0.0,0.0,"Black"); // Draw the sequence names
    this->genetree_color = Color(1,0.39,0.30,"DarkPink");  //circles and rectangules of species
    this->species_edge_color = Color(0.74,0.945,0.93,"LightBlue"); //color edge
    this->snode_contour_color = Color(1,0.93,0.82,"LightPink"); //color node

    this->sedge_contour_color = Color(.60,.80,.100,"Blue"); //marker??
    this->species_node_color = Color(0.91,0.31,0.66,"DarkPink"); //for marker
    this->markerColor = Color(.77,.94,.1,"Blue2"); //marker color
    this->umColor = Color(.77,.94,.1,"Blue2");
 }

 //set the color combination
 void Colours::setColors(const char *set)
 {
    this->set = set;
    
    if(strncasecmp(set,"1",1) == 0)
    {
      this->default_foreground_color = Color(0.0,0.0,0.0,"Black"); // Draw the sequence names
      this->genetree_color = Color(1,0.39,0.30,"DarkPink");  //circles and rectangules of species
      this->species_edge_color = Color(0.74,0.945,0.93,"LightBlue"); //color edge
      this->snode_contour_color = Color(1,0.93,0.82,"LightPink"); //color node
      
      this->sedge_contour_color = Color(.60,.80,.100,"Blue"); //marker??
      this->species_node_color = Color(0.91,0.31,0.66,"DarkPink"); //for marker
      this->markerColor = Color(.77,.94,.1,"Blue2"); //marker color
      this->umColor = Color(.29,.75,.95,"Blue2");
      
    }
    else if(strncasecmp(set,"2",1) == 0)
    {
      this->default_foreground_color = Color(0.0,0.0,0.0,"Black"); // Draw the sequence names
      this->genetree_color = Color(1,0.36,0.0,"DarkOrange");  //circles and rectangules of species
      this->species_edge_color = Color(0.95,0.96,0.93,"LightGrey"); //color edge
      this->snode_contour_color = Color(1,0.76,0.0,"LightOrange"); //color node
      
      this->sedge_contour_color = Color(.72,.0,.16,"DarkRed"); //marker??
      this->species_node_color = Color(0.52,0.0,0.18,"DarkPurple"); //for marker
      this->markerColor = Color(.29,.75,.95,"Blue2"); //marker color
      this->umColor = Color(.29,.75,.95,"Blue2");
      
    }
    else if(strncasecmp(set,"3",1) == 0)
    {
      this->default_foreground_color = Color(0.0,0.0,0.0,"Black"); // Draw the sequence names
      this->genetree_color = Color(1,0.8,0.0,"DarkOrange");  //circles and rectangules of species
      this->species_edge_color = Color(0.15,0.56,0.93,"LightGrey"); //color edge
      this->snode_contour_color = Color(1,0.36,0.0,"LightOrange"); //color node
      
      this->sedge_contour_color = Color(.72,.0,.66,"DarkRed"); //marker??
      this->species_node_color = Color(0.52,0.0,0.68,"DarkPurple"); //for marker
      this->markerColor = Color(.69,.75,.95,"Blue2"); //marker color
      this->umColor = Color(0.0,0.0,0.0,"Black");
      
      
    }
    else if(strncasecmp(set,"4",1) == 0)
    {
      this->default_foreground_color = Color(0.0,0.0,0.0,"Black"); // Draw the sequence names
      this->genetree_color = Color(0.2,0.8,0.2,"DarkOrange");  //circles and rectangules of species
      this->species_edge_color = Color(0.35,0.76,0.93,"LightGrey"); //color edge
      this->snode_contour_color = Color(1,0.26,0.2,"LightOrange"); //color node
      
      this->sedge_contour_color = Color(.4,0.4,.66,"DarkRed"); //marker??
      this->species_node_color = Color(0.52,0.4,0.68,"DarkPurple"); //for marker
      this->markerColor = Color(.69,.75,.95,"Blue2"); //marker color
      this->umColor = Color(0.0,0.0,0.0,"Black");
      
      
    }
 }
 
 //return the color combination
 const char* Colours::getSet()
 {
   return this->set; 
 }

