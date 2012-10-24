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

/* Object that stores all the parameters needed in the application */

#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "Colours.h"
#include <vector>
#include "libraries/StrStrMap.hh"
#include "libraries/SetOfNodes.hh"
#include <boost/dynamic_bitset.hpp>

using namespace std;
using boost::dynamic_bitset;

class Parameters
{
public:
  
  Parameters();
  Parameters& operator=(const Parameters &ts);
  
  string species_font;
  string gene_font;
  string all_font;
  bool ids_on_inner_nodes;
  bool do_not_draw_species_tree;
  bool do_not_draw_guest_tree;
  float fontscale;		
  float imagescale;
  float xoffset;
  float yoffset;
  float ufontsize;
  float species_font_size;
  float gene_font_size;
  float all_font_size;
  int markerscale;
  bool isMarkerColor;
  bool header;
  bool legend;
  bool title;
  bool lattransfer;
  float lateralmincost;
  float lateralmaxcost;
  float lateralduplicost;
  float lateraltrancost;
  bool show_event_count;
  bool UI;
  bool scaleByTime;
  bool timeAtEdges;	
  bool noTimeAnnotation;
  bool markers;
  bool horiz;
  string format; 
  Colours *colorConfig;
  string outfile; 
  string titleText;
  std::vector<double> uMarker;
  char ladd;  
  bool isreconciled;
  float um_fontsize;
  float ux_offset;
  float uy_offset;
  float width;
  float height;
  float fontsize;
  double leafwidth;
  dynamic_bitset<> transferedges;
  dynamic_bitset<> duplications;
  bool equalTimes;
  double linewidth;
  double s_contour_width;
  double separation;
  double root_sep;
  double min_node_height;
  double maxLeafNameSize;
  Color speciesFontColor;
  Color geneFontColor;
  Color allFontColor;
  bool reduce;
};


#endif // PARAMETERS_H
