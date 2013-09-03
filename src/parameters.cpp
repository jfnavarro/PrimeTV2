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
*/
#include <parameters.h>

Parameters::Parameters()
{
  species_font = "Times";
  gene_font = "Times";
  all_font = "Times";
  ids_on_inner_nodes = false;
  do_not_draw_species_tree = false;
  do_not_draw_guest_tree = false;
  fontscale = 1.0;		
  imagescale = 1.0;
  xoffset = 0.0;
  yoffset = 0.0;
  ufontsize = 1.0;
  markerscale = 1.0;
  isMarkerColor = false;
  header = false;
  legend = false;
  title = false;
  lattransfer = false;
  lateralmincost = 1.0;
  lateralmaxcost = 10.0;
  lateralduplicost = 1.0;
  lateraltrancost = 1.0;
  show_event_count = false;
  UI = false;
  scaleByTime = true;
  timeAtEdges = false;	
  noTimeAnnotation = false;
  markers = false;
  horiz = true;
  format = "pdf"; 
  colorConfig = new Colours();
  colorConfig->setColors("1");
  outfile = "image"; 
  ladd = 'n';  
  isreconciled = false;
  um_fontsize = 1.0;
  species_font_size = 10.0;
  gene_font_size = 10.0;
  ux_offset = 0.0;
  uy_offset = 0.0;
  width = 1200;
  height = 1400;
  fontsize = 10.0;
  leafwidth = 25;
  equalTimes = true;
  linewidth = 1.0;
  s_contour_width = 5.0;
  separation = 150.0;
  root_sep = 50.0;
  min_node_height = 3;
  maxLeafNameSize = 0.0;
  speciesFontColor.blue = 0.0;
  speciesFontColor.green = 0.0;
  speciesFontColor.red = 0.0;
  geneFontColor.blue = 0.0;
  geneFontColor.green = 0.0;
  geneFontColor.red = 0.0;
  allFontColor.blue = 0.0;
  allFontColor.green = 0.0;
  allFontColor.red = 0.0;
  reduce = false;
  drawAll = false;
  all_font_size = 1.0;
}

Parameters::~Parameters()
{
  if(colorConfig)
  {
    //TODO find out why this does not work
    //delete colorConfig;
    colorConfig = 0;
  }
}

Parameters& Parameters::operator=(const Parameters& p)
{
      if (this != &p)
      {
        species_font = p.species_font;
        gene_font = p.gene_font;
        all_font = p.all_font;
        ids_on_inner_nodes = p.ids_on_inner_nodes;
        do_not_draw_species_tree = p.do_not_draw_species_tree;
        do_not_draw_guest_tree = p.do_not_draw_guest_tree;
        fontscale = p.fontscale;		
        imagescale = p.imagescale;
        xoffset = p.xoffset;
        yoffset = p.yoffset;
        ufontsize = p.ufontsize;
        species_font_size = p.species_font_size;
        gene_font_size = p.gene_font_size;
        markerscale = p.markerscale;
        isMarkerColor = p.isMarkerColor;
        header = p.header;
        legend = p.legend;
        title = p.title;
        lattransfer = p.lattransfer;
        lateralmincost = p.lateralmincost;
        lateralmaxcost = p.lateralmaxcost;
        lateralduplicost = p.lateralduplicost;
        lateraltrancost = p.lateraltrancost;
        UI = p.UI;
        scaleByTime = p.scaleByTime;
        timeAtEdges = p.timeAtEdges;	
        noTimeAnnotation = p.noTimeAnnotation;
        markers = p.markers;
        horiz = p.horiz;
        format = p.format; 
        colorConfig = p.colorConfig;
        outfile = p.outfile; 
        titleText = p.titleText;
        uMarker = p.uMarker;
        ladd = p.ladd;  
        isreconciled = p.isreconciled;
        um_fontsize = p.um_fontsize;
        ux_offset = p.ux_offset;
        uy_offset = p.uy_offset;
        transferedges = p.transferedges;
        duplications = p.duplications;
        width = p.width;
        height = p.height;
        fontsize = p.fontsize;
        leafwidth = p.leafwidth;
        equalTimes = p.equalTimes;
        linewidth = p.linewidth;
        separation = p.separation;
        root_sep = p.root_sep;
        min_node_height = p.min_node_height;
        maxLeafNameSize = p.maxLeafNameSize;
        speciesFontColor = p.speciesFontColor;
        geneFontColor = p.geneFontColor;
        allFontColor = p.allFontColor;
        reduce = p.reduce;
        drawAll = p.drawAll;
        all_font_size = p.all_font_size;
        show_event_count = p.show_event_count;
        s_contour_width = p.s_contour_width;
      }
    return *this;

}



