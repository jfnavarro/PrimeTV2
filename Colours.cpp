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

#include "Colours.h"
#include <string.h>
#include <strings.h>
#include <sstream>
#include <iostream>

using namespace std;
 

 //Constructor, color combination 1 by default
 Colours::Colours()
 {
   this->setColors("1");
 }

 //set the color combination
 void Colours::setColors(const char *set)
 {
    this->set = set;

    Color black = Color(0.0, 0.0, 0.0, "Black");
    Color lightgrey = Color(0.8, 0.8, 0.8, "Lightgrey");
    Color white = Color(1.0, 1.0, 1.0, "White");
    
    if(strncasecmp(set,"1",1) == 0 || strncasecmp(set, "blue", 3) == 0)
    {
      Color lightblue = Color(0.56, 0.75, 1.0, "lightblue");
      Color blue = Color(0.0, 0.375, 1.0, "blue");
      Color red = Color(1.0, 0.0, 0.0, "red");
      
      this->default_foreground_color =    black;
      this->gene_edge_color =             black; 
      this->gene_lgt_color =              black; 
      this->gene_dupl_color =             black; 
      this->gene_spec_color =             black; 

      this->species_edge_color =          lightblue;
      this->species_edge_contour_color =  lightblue;
      this->species_node_color =          blue;
      this->species_node_contour_color =  blue;

      this->markerColor =                 red; 
      this->umColor =                     red;
      
    }
    else if(strncasecmp(set,"2",1) == 0 || strncasecmp(set, "kth", 3) == 0)
    {
      Color kthBlue = Color(0.1, 0.33, 0.65,"KTHBlue");
      Color kthRed  = Color(.62, .063, 0.18,"KTHRed");
      Color kthGrey = Color(0.75,0.75, 0.75, "KTHGrey");
      
      this->default_foreground_color = 	black; 
      this->gene_edge_color =           	black; 
      this->gene_lgt_color =            	kthGrey; 
      this->gene_dupl_color =           	kthRed; 
      this->gene_spec_color =           	black; 
      this->species_edge_color =       	kthBlue; 
      this->species_edge_contour_color =     kthBlue; 
      
      this->species_node_color =       	kthGrey; 
      this->species_node_contour_color =     kthGrey; 

      this->markerColor =              	kthRed; 
      this->umColor =                 	kthRed;
      
    }
    else if(strncasecmp(set,"3",1) == 0 || strncasecmp(set, "su", 2) == 0)
    {
      Color suBlue =      Color(0.00, 0.18, 0.37, "SU Blue"); 
      Color suWaterBlue = Color(0.45, 0.70, 0.81, "SU Water Blue");
      Color suOlive =     Color(0.64, 0.66, 0.42, "SU Olive Green");
      Color suFire =      Color(0.85, 0.37, 0.0,  "SU Fire Red");

      this->default_foreground_color = 	black;
      this->gene_edge_color = 		suOlive; 
      this->gene_lgt_color =  		suOlive; 
      this->gene_dupl_color = 		white; 
      this->gene_spec_color = 		white; 

      this->species_edge_color =  		suBlue;
      this->species_edge_contour_color = 	suBlue;
      this->species_node_color =  		suWaterBlue;
      this->species_node_contour_color = 	suWaterBlue;

      this->markerColor = 			suFire;
      this->umColor =     			suFire;

    }
    else if(strncasecmp(set,"4",1) == 0 || strncasecmp(set, "grey", 4) == 0 || strncasecmp(set, "gray", 4) == 0)
    {
      Color darkgrey =  Color(0.5, 0.5, 0.5, "Darkgrey");
      
      this->default_foreground_color = 	black; 

      this->gene_edge_color = 		black; 
      this->gene_lgt_color =  		black; 
      this->gene_dupl_color = 		black; 
      this->gene_spec_color = 		black; 

      this->species_edge_color = 		lightgrey; 
      this->species_node_contour_color = 	darkgrey; 
      this->species_node_color = 		darkgrey;
      this->species_edge_contour_color = 	black;

      this->markerColor =  			black;
      this->umColor = 				black;
      
      
    }
    else if(strncasecmp(set,"5",1) == 0 || strncasecmp(set, "mono", 6) == 0)
    {
      this->default_foreground_color = 	black; 
      this->gene_edge_color =			black; 
      this->gene_lgt_color =  		black; 
      this->gene_dupl_color = 		black; 
      this->gene_spec_color = 		black; 
      
      this->species_edge_color = 		white; 
      this->species_edge_contour_color = 	black;
      this->species_node_color = 		white;
      this->species_node_contour_color = 	black;

      this->markerColor = 			black;
      this->umColor =	 			black;
    } 
    else if(strncasecmp(set,"6",1) == 0 || strncasecmp(set, "yellow", 4) == 0)
    {
      Color yellow = Color(1, 1, 0, "Yellow");
      Color red = Color(1, 0, 0, "Red");

      this->default_foreground_color = 	black; 
      this->gene_edge_color = 		black; 
      this->gene_lgt_color =  		black; 
      this->gene_dupl_color = 		black; 
      this->gene_spec_color = 		black; 

      this->species_edge_color = 		yellow; 
      this->species_edge_contour_color = 	yellow;
      this->species_node_color = 		lightgrey;
      this->species_node_contour_color = 	lightgrey;

      this->markerColor = black;
      this->umColor = red;
    }
    else if(strncasecmp(set,"7",1) == 0 || strncasecmp(set, "cyan", 4) == 0)
    {
      Color cyan = Color(0.74, 0.94, 0.92, "Cyan");
      Color ared = Color(0.98, 0.39, 0.32, "A red");
      Color beige= Color(0.99, 0.92, 0.82, "Beige");

      this->default_foreground_color = 	black; 
      this->gene_edge_color = 		ared; 
      this->gene_lgt_color =  		ared; 
      this->gene_dupl_color = 		ared; 
      this->gene_spec_color = 		ared; 
      
      this->species_edge_color = 		cyan; 
      this->species_edge_contour_color = 	cyan;
      this->species_node_color = 		beige;
      this->species_node_contour_color = 	beige;

      this->markerColor = black;
      this->umColor = black;
    }
    else if(strncasecmp(set,"8",1) == 0 || strncasecmp(set, "pink", 4) == 0)
    {
      Color lightpink = Color(0.99, 0.61, 0.91, "Light pink");
      Color pink = Color(0.99, 0.33, 0.87, "Pink");
      Color darkpink= Color(0.69, 0.08, 0.57, "Dark pink");

      this->default_foreground_color =	black; 
      this->gene_edge_color = 		black; 
      this->gene_lgt_color =  		darkpink; 
      this->gene_dupl_color = 		darkpink; 
      this->gene_spec_color = 		darkpink; 
      
      this->species_edge_color = 		lightpink; 
      this->species_edge_contour_color = 	pink;
      this->species_node_color = 		pink;
      this->species_node_contour_color = 	pink;

      this->markerColor = black;
      this->umColor = black;
    } 
    else if(strncasecmp(set,"9",1) == 0 || strncasecmp(set, "purple", 6) == 0)
    {
      Color lightpurple = Color(0.95, 0.2, 0.975, "Light purple");
      Color purple = Color(0.74, 0.03, 0.85, "purple");
      Color darkpurple= Color(0.4, 0.03, 0.55, "Dark purple");

      this->default_foreground_color = 	black; 
      this->gene_edge_color = 		black; 
      this->gene_lgt_color =  		darkpurple; 
      this->gene_dupl_color = 		darkpurple; 
      this->gene_spec_color = 		darkpurple; 
      
      this->species_edge_color = 		lightpurple; 
      this->species_edge_contour_color = 	purple;
      this->species_node_color = 		purple;
      this->species_node_contour_color = 	purple;

      this->markerColor = black;
      this->umColor = black;
    }
 }
 
 //return the color combination
 const char* Colours::getSet()
 {
   return this->set; 
 }

