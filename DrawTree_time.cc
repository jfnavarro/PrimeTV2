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


#include <string>
#include <sstream>
#include <algorithm>
#include "libraries/AnError.hh"
#include "DrawTree_time.hh"
#include "Colours.h"
#include "math.h"


//dashes pens used to pain with dashes
static const double dashed1[] = {4.0, 1.0};
static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
static const double dashed3[] = {0.0};
static const double pi = 3.141516;

  
  using namespace std;
  
  //Constructor

  DrawTree_time::DrawTree_time(const Parameters &p,TreeExtended &gene, TreeExtended &species, 
			       const GammaMapEx<Node> &gamma, cairo_t *cr_)
    :parameters(&p),gene(&gene),species(&species),gamma(&gamma),surface(0),surfaceBackground(0),config(0),
     nDupl(0),nTrans(0)
  {
    
    image = false;
    config = parameters->colorConfig;
    lambda = gamma.getLambda();
    
    //change the dimensions according to the oriantation
    if(parameters->horiz)
    {
      pageheight = parameters->width;
      pagewidth = parameters->height;
    }
    else
    {
      pagewidth = parameters->width;
      pageheight = parameters->height;
    }
    
    leafWidth = parameters->leafwidth;
    
    char str[80];
    //file format has to be completed
    
    //create the surface according to the format given
    if(parameters->format.compare("pdf") == 0)
    {
      surface = cairo_pdf_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".pdf"), 
					  pagewidth - parameters->separation/2, pageheight - parameters->separation/2);
      surfaceBackground = cairo_pdf_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".pdf"), pagewidth, pageheight);
      
    }else if(parameters->format.compare("ps") == 0)
    {
      surface = cairo_ps_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".ps"), 
					 pagewidth - parameters->separation/2, pageheight - parameters->separation/2);
      surfaceBackground = cairo_ps_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".ps"), pagewidth, pageheight);
      
    }else if(parameters->format.compare("svg") == 0)
    {
      surface = cairo_svg_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".svg"), 
					  pagewidth - parameters->separation/2, pageheight - parameters->separation/2);
      surfaceBackground = cairo_svg_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".svg"), pagewidth, pageheight);
    }
    else if(parameters->format.compare("jpg") == 0)
    {
      image = true;
      surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, pagewidth - parameters->separation/2, pageheight - parameters->separation/2);
      surfaceBackground = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, pagewidth, pageheight);
    }
    else
    {
      surface = cairo_pdf_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".pdf"), 
					  pagewidth - parameters->maxLeafNameSize, pageheight - parameters->maxLeafNameSize);
      surfaceBackground = cairo_pdf_surface_create (strcat(strcpy(str,parameters->outfile.c_str()),".pdf"), pagewidth, pageheight);
    }
        
    //if the cairo object has been given as inputs
    if(!cr_)    
      cr = cairo_create (surface); 
    else
      this->cr = cr_;
    
    pageheight -= parameters->separation;
    pagewidth -= parameters->separation;
    //font size and colour
    fontsize = parameters->fontsize;
    fontsize = fontsize * parameters->fontscale;
    linewidth = parameters->linewidth;
    s_contour_width = parameters->s_contour_width;
    
    cairo_select_font_face (cr, "Times", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, fontsize);
    cairo_set_source_rgba (cr, 1, 1, 1,1);
    cairo_set_line_width (cr, linewidth);
    cairo_paint(cr);
 }

  //
  // Total destruction
  //
  DrawTree_time::~DrawTree_time()
  {
    if(!cr)
      cairo_destroy(cr);
    if(!surface)
      cairo_surface_destroy(surface);
    if(!surfaceBackground)
      cairo_surface_destroy(surfaceBackground);
   
    delete(config);
    FreeClear(geneEdges);
  }
  
  void DrawTree_time::createHeader()
  { 
    cairo_surface_t *image = cairo_image_surface_create_from_png("images/header.png");
    cairo_save(cr);
    cairo_set_source_surface(cr,image,pagewidth - cairo_image_surface_get_width(image),10);
    cairo_paint(cr);
    cairo_surface_destroy(image);
    cairo_restore(cr);
  
  }

  int DrawTree_time::RenderImage()
  {
      
    if(image)
    {
      char str[80];
      cairo_status_t e = cairo_surface_write_to_png (surface, strcat(strcpy(str,parameters->outfile.c_str()),".jpg"));
      if (!e == CAIRO_STATUS_SUCCESS )
	throw AnError("Could not write file!\n", 1);
    }
   
    cr = cairo_create(surfaceBackground);
    if(parameters->horiz)
      cairo_set_source_surface(cr,surface,parameters->maxLeafNameSize,parameters->maxLeafNameSize);
    else
      cairo_set_source_surface(cr,surface,parameters->maxLeafNameSize,parameters->maxLeafNameSize);
    
    cairo_paint(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    cairo_surface_destroy(surfaceBackground);
    cr = 0;
    surface = 0;
    surfaceBackground = 0;
    return EXIT_SUCCESS;
    
  }
  
  void DrawTree_time::calculateTransformation()
  {

    if(parameters->horiz)
    {
      double c = cos(pi/2);
      double s = sin(pi/2);
      double cx = pagewidth/2;
      double cy = pageheight/2;
    
      cairo_matrix_t matrix2;
      cairo_matrix_init(&matrix,c,s,-s,c,cx-c*cx+s*cy,cy-s*cx-c*cy);
      double xscale = (pageheight/pagewidth) * parameters->imagescale;
      double yscale = (pagewidth/pageheight) * parameters->imagescale;
      double yoffset = parameters->xoffset + parameters->separation - parameters->maxLeafNameSize/2;
      double xoffset = parameters->yoffset + parameters->separation - parameters->maxLeafNameSize/2;
      cairo_matrix_init(&matrix2,xscale,0,0,yscale,xoffset,-yoffset);
      cairo_matrix_multiply(&matrix,&matrix2,&matrix);
      cairo_transform(cr,&matrix);
    }
    else
    {
      cairo_matrix_init(&matrix,parameters->imagescale,0,0,parameters->imagescale,
			parameters->xoffset,parameters->yoffset);
      cairo_transform(cr,&matrix);
    }
  }
  
  
  void DrawTree_time::createTitle()
  { 
    cairo_save(cr);
    cairo_matrix_invert(&matrix);
    cairo_transform(cr,&matrix);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_select_font_face (cr, "Times", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, fontsize);

    const char *str = parameters->titleText.c_str();
    cairo_text_extents (cr, str, &extents);
    cairo_move_to (cr, std::max(0.0,(pagewidth/2) - extents.width/2), extents.height*2);
    cairo_show_text(cr, str);
    cairo_restore(cr);
  }
  
void DrawTree_time::writeEventCosts()
  { 
    cairo_save(cr);
    cairo_matrix_invert(&matrix);
    cairo_transform(cr,&matrix);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_select_font_face (cr, "Times", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, fontsize);


    ostringstream oss;
    if (parameters->lattransfer) {
      oss << "#duplications: " << nDupl
	  << ", #transfers: " << nTrans << endl;
    } else {
      oss << "#duplications: " << nDupl
	  << ", no transfers ";
    }
    const char *str = oss.str().c_str();
    cerr << str << endl;
    cairo_text_extents (cr, str, &extents);
    cairo_move_to (cr, 0, extents.height*2);
    cairo_show_text(cr,  str);
    cairo_restore(cr);
  }
  
  void DrawTree_time::createLegend()
  {
    int x = 0;
    int y = 0;
    int width = 180;
    int height = 90;

    cairo_save(cr);
    cairo_matrix_invert(&matrix);
    cairo_transform(cr,&matrix);
    cairo_set_font_size (cr, 10);
    
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_select_font_face (cr, "Times", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, fontsize);
    cairo_move_to (cr, x, y);
    cairo_rel_line_to (cr, width, 0);
    cairo_rel_line_to (cr, 0, height);
    cairo_rel_line_to (cr, -width, 0);
    cairo_close_path (cr);
    cairo_stroke(cr);
    cairo_move_to(cr,width/2-x,y+10);
    cairo_show_text(cr,"Legend");
    
    cairo_set_source_rgba(cr,config->gene_edge_color.red,
			  config->gene_edge_color.green,config->gene_edge_color.blue,1);
    cairo_move_to(cr,x+10,y+20);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+20);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+25);
    cairo_show_text(cr,"Gene Tree Color");
    
    cairo_set_source_rgba(cr,config->species_edge_color.red,
			  config->species_edge_color.green,config->species_edge_color.blue,1);
    cairo_move_to(cr,x+10,y+30);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+30);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+35);
    cairo_show_text(cr,"Species Edge Color");
    
    cairo_set_source_rgba(cr,config->species_node_color.red,
			  config->species_node_color.green,config->species_node_color.blue,1);
    cairo_move_to(cr,x+10,y+40);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+40);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+45);
    cairo_show_text(cr,"Node Contour Color");
    
    cairo_set_source_rgba(cr,config->umColor.red,config->umColor.green,config->umColor.blue,0.60);
    cairo_move_to(cr,x+10,y+50);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+50);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+55);
    cairo_show_text(cr,"Marker Color");
    
    cairo_move_to(cr,x+10,y+60);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+60);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+65);
    cairo_show_text(cr,"Time and Axes Color");
    
    cairo_set_source_rgba(cr,parameters->speciesFontColor.red,
			  parameters->speciesFontColor.green,parameters->speciesFontColor.blue,1);
    cairo_move_to(cr,x+10,y+70);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+70);
    cairo_stroke(cr);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+75);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_show_text(cr,"Species Font Color");
    cairo_stroke(cr);
    
    
    cairo_set_source_rgba(cr,parameters->geneFontColor.red,
			  parameters->geneFontColor.green,parameters->geneFontColor.blue,1);
    cairo_move_to(cr,x+10,y+80);
    cairo_set_line_width(cr, 10);
    cairo_line_to(cr,x+15,y+80);
    cairo_stroke(cr);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr,x+30,y+85);
    cairo_set_source_rgba(cr,0,0,0,1);
    cairo_show_text(cr,"Gene Font Color");
    cairo_restore(cr);
  }


  void
  DrawTree_time::GeneTreeMarkers()
  {
    cairo_set_source_rgba(cr,config->gene_edge_color.red,config->gene_edge_color.green,config->gene_edge_color.blue,1);
    
    if(parameters->isMarkerColor) 
	cairo_set_source_rgba(cr,config->umColor.red,config->umColor.green,config->umColor.blue,0.80);
	
    cairo_select_font_face (cr, parameters->gene_font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr,parameters->um_fontsize * parameters->fontscale);
    cairo_text_extents (cr, "i", &extents);
    double offset = extents.width/2;
    
    for(vector<double>::const_iterator i = parameters->uMarker.begin(); 
	    i != parameters->uMarker.end(); i++)
    {
      if(*i < gene->getNumberOfNodes())
      {
	Node *n = gene->getNode(*i);
        ostringstream os;
        os << *i;
        const string st = os.str();
        cairo_move_to(cr,n->getX() + parameters->ux_offset + offset,n->getY() + parameters->uy_offset + offset);
        cairo_show_text(cr,st.c_str()); 
      }
    }
  }

  //
  // Convert a number to a string. 
  //
  string
  DrawTree_time::double2charp(double x)
  {
    ostringstream et;
    et << x;
    return et.str();
  }

   void DrawTree_time::DrawTimeEdges()
   {
     cairo_set_line_width (cr, linewidth);
     double midnode = leafWidth;
     cairo_set_font_size (cr, fontsize);
     cairo_set_source_rgba (cr, 0, 0, 0, 1);
     cairo_move_to(cr, 0,pageheight);  
     cairo_line_to(cr, pagewidth,pageheight);
     cairo_line_to(cr,pagewidth,0);
     cairo_set_line_width(cr, 1);
     cairo_set_dash(cr, dashed1, len1, 0);

     for(unsigned int u = 0; u < species->getNumberOfNodes(); u++) {
	Node* n = species->getNode(u);
	if(!n->isLeaf()) {		    
	    cairo_move_to(cr, n->getX(), pageheight);    
	    cairo_line_to(cr, n->getX(), n->getY()+midnode);
	  }
      }
     cairo_stroke(cr);
     cairo_set_dash(cr, dashed3, 0, 0);
   }
   
   void DrawTree_time::DrawSpeciesEdgesWithContour()
   {
     Color& cfill = config->species_edge_color;
     Color& cline = config->species_edge_contour_color;

     cairo_set_line_width(cr, s_contour_width);
     cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
     cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
     double midnode = leafWidth;
     Node *root = species->getRootNode();
     cairo_move_to(cr,0,root->getY()+midnode);
     cairo_rel_line_to(cr,root->getX(),0);
     cairo_rel_line_to(cr,0,-midnode*2);
     cairo_rel_line_to(cr,- root->getX(),0);
     cairo_close_path(cr);

     cairo_set_source_rgba(cr, cline.red, cline.green, cline.blue, 1);
     cairo_stroke_preserve(cr);
     cairo_set_source_rgba(cr, cfill.red, cfill.green, cfill.blue, 1);
     cairo_fill(cr);
     
     for ( Node *n = species->preorder_begin(); n != NULL; n = species->preorder_next(n) )
     {
       double x = n->getX();
       double y = n->getY();

       if(!n->isLeaf())
       { 
	 double pmidx, pmidy;
	 intersection(x, y - midnode,
		      n->getLeftChild()->getX(), n->getLeftChild()->getY()-midnode,		      
		      x, y + midnode,
		      n->getRightChild()->getX(), n->getRightChild()->getY()+midnode,
		      pmidx, pmidy);

	 cairo_move_to(cr,x,y + midnode); 
 	 cairo_rel_line_to(cr,n->getLeftChild()->getX()-x,n->getLeftChild()->getY()-y);
 	 cairo_rel_line_to(cr,0,-midnode*2);

	 cairo_line_to(cr,pmidx, pmidy);
	 
 	 cairo_line_to(cr,n->getRightChild()->getX(),n->getRightChild()->getY()+midnode);
 	 cairo_rel_line_to(cr,0,-midnode*2);
 	 cairo_line_to(cr,x,y - midnode);
	 cairo_close_path(cr);

	 cairo_set_source_rgba(cr, cline.red, cline.green, cline.blue, 1);
	 cairo_stroke_preserve(cr);
	 cairo_set_source_rgba(cr, cfill.red, cfill.green, cfill.blue, 1);
	 cairo_fill(cr);
       }
     }
   }

// Find the intersection of lines (p1, p2) and (p3, p4), 
// put result in p5.
// Notation nicked from http://en.wikipedia.org/wiki/Line-line_intersection.
void DrawTree_time::intersection(double x1, double y1,
				 double x2, double y2,
				 double x3, double y3,
				 double x4, double y4,
				 double &x5, double &y5)
{
  x5 = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4 - y3*x4)) 
    / ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));
  y5 = ((x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4 - y3*x4)) 
    / ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

}

   
   void DrawTree_time::DrawTimeLabels()
   {
       cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
       cairo_set_source_rgba (cr, 0, 0, 0, 1);
       cairo_text_extents (cr, " ", &extents);
       double offset = extents.width;
    
      for(unsigned int u = 0; u < species->getNumberOfNodes(); u++) {
       Node* n = species->getNode(u);
		
	if(!n->isLeaf())
	{
	 string timelabel = double2charp(n->getNodeTime());
	 double xpos = n->getX();
	 double ypos = pageheight;
	 cairo_move_to(cr,xpos+offset,ypos);
	 cairo_save(cr);
	 cairo_rotate(cr,-(pi/2));
	 cairo_show_text(cr,timelabel.c_str());
	 cairo_restore(cr);
	}
      }
      cairo_move_to(cr,pagewidth,pageheight);
      cairo_save(cr);
      cairo_rotate(cr,-(pi/2));
      cairo_show_text(cr,"0");
      cairo_restore(cr);
      cairo_text_extents (cr,"Time", &extents);
      cairo_move_to(cr,0 + extents.width,pageheight);
      cairo_save(cr);
      cairo_rotate(cr,-(pi/2));
      cairo_show_text(cr,"Time");
      cairo_restore(cr);
      cairo_stroke(cr);
     
   }
   
   void DrawTree_time::DrawSpeciesNodes()
   {
     Color& cfill = config->species_node_color;
     Color& cline = config->species_node_contour_color;
     
     for ( Node *n = species->preorder_begin(); n != NULL; n = species->preorder_next(n) )
     {
       if(!n->isLeaf())
       {
	 cairo_save(cr);
         cairo_translate (cr,n->getX(), n->getY());
         cairo_scale(cr, 0.3, 1);
         cairo_arc (cr, 0., 0.,leafWidth, 0., 2 * pi);

	 cairo_set_source_rgba(cr, cfill.red, cfill.green, cfill.blue, 1);
         cairo_fill_preserve(cr);

	 cairo_set_line_width(cr, s_contour_width);
	 cairo_set_source_rgba(cr, cline.red, cline.green, cline.blue, 1);
         cairo_stroke(cr);
	 cairo_restore(cr);
       }
     }
     
   }
   
   void DrawTree_time::DrawSpeciesNodeLabels()
   {
    cairo_select_font_face (cr, parameters->species_font.c_str(), CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_source_rgba (cr, parameters->speciesFontColor.red, 
			   parameters->speciesFontColor.green, parameters->speciesFontColor.blue, 1);
    cairo_set_font_size (cr, fontsize);
    
    for(unsigned int u = 0; u < species->getNumberOfNodes(); u++)
    {
	Node* n = species->getNode(u);
	ostringstream st;
	if(parameters->do_not_draw_species_tree == false){
	  if (parameters->ids_on_inner_nodes) {
	    st << n->getNumber();
	  }
	  st << " " + n->getName();
	}
	const string ns = st.str();
	double xpos = n->getX();
	double ypos = n->getY();
	cairo_text_extents (cr, ns.c_str(), &extents);
	if(n->isLeaf())
	  cairo_move_to(cr,xpos + extents.height/2 ,ypos + leafWidth);
	else
	  cairo_move_to(cr,xpos - 0.25 * extents.width, ypos + leafWidth);
	cairo_save(cr);
	if(parameters->horiz)
	  cairo_rotate(cr,-(pi/4));
	cairo_show_text(cr,ns.c_str());
        cairo_restore(cr);
    }

    cairo_stroke(cr);
      
   }
   
  void
  DrawTree_time::TimeLabelsOnEdges()
  {
    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    
    for(unsigned int u = 0; u < species->getNumberOfNodes(); u++) {
      Node* n = species->getNode(u);
      string timelabel = double2charp(n->getTime());
      double xpos = n->getX();
      double ypos = n->getY();
      if(!n->isLeaf())
      {
	cairo_text_extents (cr, timelabel.c_str(), &extents);
	xpos = xpos - (extents.width + extents.x_advance);
	ypos =  ypos - leafWidth;
	cairo_move_to(cr,xpos,ypos);
	cairo_save(cr);
	if(parameters->horiz)
	  cairo_rotate(cr,-(pi/4));
	cairo_show_text(cr,timelabel.c_str()); 
	cairo_restore(cr);
	
      }
    }
  }
   
  
  void DrawTree_time::DrawGeneNodes()
  {
     Color& duplCol = config->gene_dupl_color;
     Color& specCol = config->gene_spec_color;
      
    cairo_set_line_width(cr,2);
      
    for ( Node *n = gene->preorder_begin(); n != NULL; n = gene->preorder_next(n) )
    {
        double x = n->getX();
        double y = n->getY();
       
	if(n->getReconcilation() == Leaf || n->getReconcilation() == Speciation) //speciation or leaf
	{
	   cairo_set_source_rgba(cr, specCol.red, specCol.green, specCol.blue, 1);
	   cairo_arc(cr,x, y, leafWidth/10,0.0,2*pi);
	   cairo_fill(cr);
	}
	else if (n->getReconcilation() == Duplication) //duplication
	{
	  nDupl++;
	   cairo_set_source_rgba(cr, duplCol.red, duplCol.green, duplCol.blue, 1);
	   cairo_rectangle(cr,x-(leafWidth/5)/2,y-(leafWidth/5)/2,leafWidth/5,leafWidth/5);
	   cairo_fill(cr);
	     
	 }
	else if (n->getReconcilation() == LateralTransfer) //duplication
	{
	  nTrans++;
	   cairo_set_source_rgba(cr, duplCol.red, duplCol.green, duplCol.blue, 1);
	   cairo_rectangle(cr,x-(leafWidth/5)/2,y-(leafWidth/5)/2,leafWidth/5,leafWidth/5);
	   cairo_fill(cr);
	     
	 }
    }
      
    cairo_stroke(cr);
    
  }
  
  void DrawTree_time::DrawGeneEdges()
  {
      Color& regular = config->gene_edge_color;
      //      Color& lgt = config->gene_lgt_color; // No support for special LGT color at this point

      cairo_set_source_rgba(cr, regular.red, regular.green, regular.blue,1);
     
      cairo_set_line_width(cr,linewidth/2);
      
      for (unsigned i = 0; i < gene->getNumberOfNodes(); i++)
      {
	  Node *n = gene->getNode(i);
	  if(!n->isRoot())
	  {
	    if(n->getReconcilation() == LateralTransfer)
	    {
	      LGT.insert(std::make_pair(n,0));
	    }
	    else
	    {
	      newDrawPath(n);
	    }
	  }
	  else if (lambda[n]->isRoot())
	  {
	     cairo_move_to(cr,n->getX(),n->getY());
	     cairo_line_to(cr,0,n->getY());
	  }
	  else 
	  {    
	    //TODO can the root be further away than 1 node?
	    cairo_move_to(cr,n->getX(),n->getY());
	    cairo_line_to(cr,species->getRootNode()->getX(),species->getRootNode()->getY());
	    cairo_line_to(cr,0,species->getRootNode()->getY());
	  }
      }

      DrawLGT();
      cairo_stroke(cr);

  }
  
  void DrawTree_time::DrawGeneLabels()
  {
    
      cairo_select_font_face (cr, parameters->gene_font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_set_source_rgba (cr, parameters->geneFontColor.red, 
			   parameters->geneFontColor.green, parameters->geneFontColor.blue, 1);
      cairo_set_font_size (cr, fontsize);
      
      for ( Node *n = gene->preorder_begin(); n != NULL; n = gene->preorder_next(n) )
      {
	if(!gamma->isLateralTransfer(*n))
	{
	    ostringstream os;
	    if (parameters->ids_on_inner_nodes) {
	      os << n->getNumber() << "  ";
	    }
	    if(n->isLeaf())
	      os << n->getName() << " ";
	    
	    cairo_text_extents(cr, os.str().c_str(), &extents);
	    double xpos = n->getX() + extents.height;
	    double ypos = n->getY() + extents.height/2;
	
	    cairo_move_to(cr,xpos,ypos);
	    cairo_save(cr);
	    if(parameters->horiz)
	      cairo_rotate(cr,-(pi/4));
	    cairo_show_text(cr,os.str().c_str());
	    cairo_restore(cr);
	}
      }
      
      cairo_stroke(cr);
    
  }

//draw the path between gene nodes (duplications and speciations)
 void DrawTree_time::newDrawPath(Node *n)
 {
   
   Node *origin = n->getHostChild();
   Node *destiny = n->getParent()->getHostChild();
   Node *nparent = n->getParent();
   
   //when there is a LGT in between we have to draw the path
   //until the next speciation or duplication node or LT node
   if(n->getParent()->getReconcilation() == LateralTransfer 
     && (lambda[n] == lambda[n->getParent()]) && !destinyLGT(n))
   {
     destiny = n->getHostParent();
     nparent = getHighestMappedLGT(n);
   }
   else if (n->getParent()->getReconcilation() == LateralTransfer)
   {
     destiny = origin;
   }
   
   double xorigin = n->getX();
   double yorigin = n->getY();
   double xend,yend = 0.0;
   
   cairo_move_to(cr,n->getX(),n->getY());
   
   Node *o;
   //we start to draw from the lowest node and from leaves to root
   //we store every edge we draw
   for (o = origin; o != destiny; o = o->getParent() )
   {
    
       if(nparent->getReconcilation() == Speciation && o->getParent() == destiny)
       {
	 cairo_line_to(cr,nparent->getX(),nparent->getY());
	 xend = nparent->getX();
	 yend = nparent->getY();
	 addEdge(o,destiny,n,nparent,xorigin,yorigin,xend,yend,Edge::Normal);
	 xorigin = nparent->getX();
	 yorigin = nparent->getY();
       }
       else
       {
	  double x = o->getParent()->getX();
	  int size = gamma->getSize(o->getParent());
	  double y = o->getParent()->getY();
	  if(size > 1)
	  {
	    double delta = leafWidth / (size - 1);
	    y = (o->getParent()->getY() - leafWidth/2) + ((o->getParent()->getVisited()) * delta);
	  }
	  o->getParent()->incVisited();
	  cairo_line_to(cr,x,y);
	  xend = x;
	  yend = y;
	  addEdge(o,destiny,n,nparent,xorigin,yorigin,xend,yend,Edge::Normal);
	  xorigin = x;
	  yorigin = y;
       }
   }
   
   //we draw an extra edge if the destiny is a duplication
    
   if(nparent->getReconcilation() == Duplication)
   {   
     cairo_line_to(cr,nparent->getX(),nparent->getY());
     addEdge(o,destiny,n,n->getParent(),xorigin,yorigin,nparent->getX(),nparent->getY(),Edge::Normal);

   }
   
 }
 

  void DrawTree_time::DrawLGT()
  {
      typedef std::map<Node*,unsigned> map_t;
      map_t mmap;
      BOOST_FOREACH(map_t::value_type &i,LGT)
      {
	newLGTPath(i.first);
      }
  }	

 

 /* this function get the destiny x and the origin x of the LT
  * then if gets the edge when the LT lays on the origin, if 
  * there is no edge the virtual edge will be between the species nodes
  * if the destiny is in a different time frame the origin and origin will
  * be placed according to the origin edge and the LGT will be drawn from
  * there to the destiny */
 void DrawTree_time::newLGTPath(Node *n)
 {
    
    Node *destiny = n->getHostParent();
    Node *GeneOrigin = getLowestMappedLGT(n);    
    Node *GeneDestiny = getLowestMappedNOLGT(n); 

    pair<Node*,pair<double,double> > retorno;
    retorno = getOriginLGT(n);

    Node *origin = retorno.first;
    double originx = retorno.second.first;
    double destinyx = retorno.second.second;
    
    double x1 = origin->getParent()->getX();
    double x2 = origin->getX();
    double y1 = origin->getParent()->getY();
    double y2 = origin->getY();
   
    Edge *e = getEdge(origin,GeneOrigin);
    if(e)
    {
      x1 = e->getXend();
      x2 = e->getXorigin();
      y1 = e->getYend();
      y2 = e->getYorigin();
    }
    double slope = (y2 - y1) / (x2 - x1);      
    double n1 = y1 - slope*x1;
    double y = slope*originx + n1; 
    
    n->setX(originx);
    n->setY(y);
    
    if(destinyx != -1)
    {
      x2 = GeneDestiny->getX();
      x1 = destiny->getParent()->getX();
      y2 = GeneDestiny->getY();
      y1 = destiny->getParent()->getY();
      slope = (y2 - y1) / (x2 - x1);      
      n1 = y1 - slope*x1;
      y = slope*destinyx + n1; 
    
      cairo_move_to(cr,n->getX(),n->getY());
      cairo_line_to(cr,destinyx,y);
      addEdge(origin,destiny,GeneOrigin,GeneDestiny,n->getX(),n->getY(),destinyx,y,Edge::LGT);
      cairo_line_to(cr,GeneDestiny->getX(),GeneDestiny->getY());
      addEdge(origin,destiny,GeneOrigin,GeneDestiny,destinyx,y,GeneDestiny->getX(),GeneDestiny->getY(),Edge::LGT);
    }
    else
    {
      Node *newdestiny = origin->getParent()->getLeftChild() == origin ? 
	  origin->getParent()->getRightChild() : origin->getParent()->getLeftChild();  
      destinyx = originx;
              
      cairo_move_to(cr,GeneDestiny->getX(),GeneDestiny->getY());
      double xend,yend,xorigin,yorigin;
      
      for(Node *o = destiny; destiny != newdestiny; destiny = destiny->getParent())
      {
          double x = o->getParent()->getX();
	  //TODO what if we have more LGT going trough this species node??
          int size = gamma->getSize(o->getParent()) + 1;
          double y = o->getParent()->getY();
	  
          if (size > 1)
          {
              double delta = leafWidth / (size - 1);
              y = (o->getParent()->getY() - leafWidth/2) + ((o->getParent()->getVisited()) * delta);
          }
          
          o->getParent()->incVisited();
          cairo_line_to(cr,x,y);
          xend = x;
          yend = y;
          addEdge(origin,destiny,GeneOrigin,GeneDestiny,xorigin,yorigin,xend,yend,Edge::LGT);
          xorigin = x;
          yorigin = y;
      }
      
      y1 = newdestiny->getParent()->getY();
      y2 = yend;
      x2 = newdestiny->getX();
      x1 = newdestiny->getParent()->getX();
      slope = (y2 - y1) / (x2 - x1);      
      n1 = y1 - slope*x1;
      y = slope*destinyx + n1; 
      
      cairo_line_to(cr,destinyx,y);
      addEdge(origin,destiny,GeneOrigin,GeneDestiny,xend,yend,destinyx,y,Edge::LGT);
      cairo_line_to(cr,n->getX(),n->getY());
      addEdge(origin,destiny,GeneOrigin,GeneDestiny,destinyx,y,n->getX(),n->getY(),Edge::LGT);
    }
 }
 
 
 //get the highest not LGT mapped node of n
 Node* DrawTree_time::getHighestMappedLGT(Node *n)
 {
   Node *parent = n->getParent();
   
   while(parent->getReconcilation() == LateralTransfer && !parent->isRoot())
     parent = parent->getParent();
   
   while(!species->descendant(lambda[n],lambda[parent]) && !parent->isRoot())
     parent = parent->getParent();
   
   return parent;
   
 }
 

 Node* DrawTree_time::getLowestMappedLGT(Node *n)
 {
   Node *left = n->getLeftChild();
   Node *right = n->getRightChild();
   Node *child = n->getHostChild();
   Node *son;
   
    if(lambda[right] == child)
      son = right;
   else
      son = left;
   
   while(son->getReconcilation() == LateralTransfer && !son->isLeaf())
   {
      if(lambda[son->getRightChild()] == child)
	son = son->getRightChild();
      else
	son = son->getLeftChild();
     
   }
   
   return son;
   
 }
 
  Node* DrawTree_time::getLowestMappedNOLGT(Node *n)
 {
   Node *left = n->getLeftChild();
   Node *right = n->getRightChild();
   Node *child = n->getHostParent();
   Node *son;
   
    if(lambda[right] == child)
      son = right;
   else
      son = left;
   
   while(son->getReconcilation() == LateralTransfer && !son->isLeaf())
   {
      if(lambda[son->getRightChild()] == child)
	son = son->getRightChild();
      else
	son = son->getLeftChild();
     
   }
   
   return son;
   
 }
 
 
 //detect if the gene node passed as argument is destiny of a LGT
 bool DrawTree_time::destinyLGT(Node *o)
 {

   for (unsigned i = 0; i < parameters->transferedges.size(); i++)
   {
     if(parameters->transferedges[o->getNumber()] || parameters->transferedges[o->getParent()->getNumber()] )
       return true;
     
   }
   return false;
   
 }
 
 Edge *DrawTree_time::getEdge(Node *sp, Node *gn)
 {
   BOOST_FOREACH(Edge *e,geneEdges)
   {
     if(e->getSpeOrigin() == sp)
     {     
       if(sp->isLeaf() && gamma->getSize(sp) > 1)
       {
          return e;
       }
       else if(e->getGeneOrigin() == gn )
	  return e;
     }
   }
   return NULL;
 }
 
 bool DrawTree_time::existLGTEdge(double x)
 {
   BOOST_FOREACH(Edge *e,geneEdges)
   {
     if(e->getMode() == Edge::LGT && x == e->getXorigin())
       return true;
   }
   return false;
   
 }
 
 //returns the species node which the LGT will lies in between on its origin point
 pair<Node*,pair<double,double> > DrawTree_time::getOriginLGT(Node *n)
 {
   
    Node *origin = n->getHostChild();
    Node *destiny = n->getHostParent();
    
    Node *GeneOrigin = getLowestMappedLGT(n); 
    Node *GeneDestiny = getLowestMappedNOLGT(n);
    Node *nparent = getHighestMappedLGT(GeneOrigin);
    Node *originbound = lambda[nparent];
    
    double destinyx;
    double originx = (GeneDestiny->getX() + destiny->getParent()->getX()) / 2;
    
//     double size = NumberLT(n) + 1;
//     double mapped = GeneOrigin->getVisited();
//     GeneOrigin->setVisited(1);
//     double edge = GeneDestiny->getX() - destiny->getParent()->getX();
//     double originx = ((edge/size) * mapped) + destiny->getParent()->getX() + leafWidth;
//     double originx = destiny->getX() - leafWidth;
    
    while((originx > (origin->getX() - leafWidth/4) && originx > (destiny->getParent()->getX() + leafWidth/4)) 
      || (existLGTEdge(originx) || overlapSpeciesNode(originx,origin,destiny)))
    {
      originx -= parameters->linewidth * 5;
    }
    
    while((originx < (originbound->getX() + leafWidth/4) && originx < (GeneDestiny->getX() - leafWidth/4)) 
      || (existLGTEdge(originx) || overlapSpeciesNode(originx,origin,destiny)))
    {
      originx += parameters->linewidth * 5;
    }
    
    if (originx < originbound->getX() || originx > origin->getX() )
    {
      originx = (origin->getX() + origin->getParent()->getX()) / 2; 
      originx = (origin->getX() + originx) / 2;
      destinyx = -1;
      
      while(existLGTEdge(originx) || overlapSpeciesNode(originx,origin,destiny))
      {
	originx -= 5;
      }
    }
    else
    {
      while(originx < origin->getParent()->getX())
      {
	origin = origin->getParent();
      }
      destinyx = originx;
    }
    
    return std::make_pair(origin,std::make_pair(originx,destinyx));
 }


 bool DrawTree_time::overlapSpeciesNode(double x,Node *origin, Node *destiny)
 {
    double y1 = (origin->getY() + origin->getParent()->getY()) / 2;
    double y2 = (destiny->getY() + destiny->getParent()->getY()) / 2;
    
    for(Node *n = species->getPostOderBegin(); n != NULL; n = species->postorder_next(n))
    {
      
      if (n != destiny && !n->isLeaf())
      {
	if( (x >= (n->getX() - (leafWidth*0.3))) && (x <= (n->getX() + (leafWidth*0.3))) 
	    && n->getY() >= y1 && n->getY() <= y2) 
	 return true;
      }
    }
   
   return false;
 }
 
 bool DrawTree_time::checkCollision(double x00,double y00, double x01, 
				   double y01, double x10, double y10, double x11,double y11)

 {
    double m0 = (y01-y00) / (x01-x00);
    double m1 = (y11-y10) / (x11-x10);
   
    double q0 = y00 - m0 * x00;
    double q1 = y10 - m1 * x10;
    
    double collision = (q1-q0) / (m1-m0);
    
    if ( m0 == m1 && q0 == q1 )
    {
       return false;
    }
    else if (x00 <= collision && collision <= x01 && x10 <= collision && collision <= x11)
    {
       return true;
    }
    else
       return false;  
 }
 
 void DrawTree_time::addEdge(Node *spO,Node *spE,Node *gO,Node *gE,
   double xo,double yo,double xe,double ye,Edge::category m)
 {
     Edge *e = new Edge();
     e->setSpeOrigin(*spO); 
     e->setSpeEnd(*spE);
     e->setGeneOrigin(*gO);
     e->setGeneEnd(*gE);
     e->setXorigin(xo);
     e->setYorigin(yo);
     e->setXend(xe);
     e->setYend(ye);
     e->setMode(m);
     geneEdges.push_back(e); 
 }
 
 
 //count the number of times this LGT is origin of LT
 unsigned DrawTree_time::NumberLT(Node *n)
 {	
    unsigned counter = 0;
    Node *GeneOrigin = getLowestMappedLGT(n);
    
    typedef std::map<Node*,unsigned> map_t;
    map_t mmap;
    
    BOOST_FOREACH(map_t::value_type &i,LGT)
    {
        Node* temp = getLowestMappedLGT(i.first);
	if(GeneOrigin == temp)
	   counter++;
    }
    
    return counter;
 }
