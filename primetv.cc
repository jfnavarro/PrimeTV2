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

#include <iostream>
#include <vector>
#include <exception>
#include <parameters.h>
#include <mainops.h>
#include <fstream>
#include "libraries/AnError.hh"
#include <sys/stat.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <boost/foreach.hpp>

#include <QApplication>
#include "windows.h"
#include "ui_primetv.h"

using namespace boost;
namespace po = boost::program_options;
using namespace std;

  
  /* Helper function to sort vectors used in the parser of parameters */
  template<class T>
  ostream& operator<<(ostream& os, const vector<T>& v)
  {
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
    return os;
  }
  
  /* Helper function that checks if a filename exists in the system */
  int file_exist (const char *filename)
  {
    struct stat   buffer;   
    return (stat (filename, &buffer) == 0);
  }



int
main (int ac, char *av[]) 
{

  
/* The boost progam options object is being used to capture all the parameters from console
 * it declares three types of parameters (generic, hidden and configuration) 
 * it loads all of them and checks their basic validity, it also loads 
 * a configuration file "config.cfg" to load all the information included in the file
 * once the parameters have been read and captured they all processed, valided and
 * assigned to the object container (Parameters)
 */  
 
  
try
{
  Parameters *parameters = new Parameters();
  string config_file;
  const char* reconciledtree;
  const char* speciestree;
  const char* genetree;
  const char* mapfile = "";
  string colorconfig;
  
  // Declare a group of options that will be 
  // allowed only on command line
  po::options_description generic("Generic options");
  generic.add_options()
    ("version,v", "print version string")
    ("help,h", "produce help message") 
    ("gui", po::bool_switch(&parameters->UI), "initiates the User Interface")
    ("reconcile,R", po::bool_switch(&parameters->isreconciled)->default_value(false), 
     "wheter the Guest tree is reconciled or not")
    ("config,C", po::value<string>(&config_file)->default_value("config.cfg"),
                  "name of a file of a configuration.");
    
    // Declare a group of options that will be 
    // allowed both on command line and in
    // config file
  po::options_description config("Configuration");
  config.add_options()
    ("color,c",po::value<string>(&colorconfig)->default_value("1"),
     "Set colors according to <spec> which can be 1, 2 or 3")
    ("notimescale,t", "don't scale species edges by time")
    ("timeex,e", po::bool_switch(&parameters->timeAtEdges),
     "annotate species edges by their time extent")
    ("notime,n", po::bool_switch(&parameters->noTimeAnnotation),"no time annotation at all")
    ("format,f", po::value<string>(&parameters->format)->default_value("pdf"),
     "File format: pdf(Pdf) ps(PostScript) jpg(JPEG) svg png")
    ("nohost,y", po::bool_switch(&parameters->do_not_draw_species_tree)->default_value(false),
     "no host tree is given nor shown")
    ("noguest,g", po::bool_switch(&parameters->do_not_draw_guest_tree)->default_value(false),
     "no guest tree is shown")
    ("file,o", po::value<string>(&parameters->outfile)->default_value("image"), 
     "<string> name of the output file")
    ("inode,i", po::bool_switch(&parameters->ids_on_inner_nodes),
     "put node numbers in the inner nodes")
    ("size,p", po::value<std::vector<float> >()->multitoken(),
     "image size in pixels <width> <height>")
    ("genefont,j", po::value<string>(&parameters->gene_font)->default_value("Times-Roman"),
     "<string> use this font for gene labels (def. Times-Roman)")
    ("speciefont,q", po::value<string>(&parameters->species_font)->default_value("Times-Italic"), 
     "<string> use this font for specie labels (def. Times-Italic)")
    ("fontscale,s", po::value<float>(&parameters->fontscale)->default_value(1.0), 
     "<float> scale fonts by this number")
    ("imagescale,S", po::value<float>(&parameters->imagescale)->default_value(1.0), 
     "<float> scale image by this number")
    ("move,D", po::value<std::vector<float> >()->multitoken(), 
     "<float> <float> move the image x,y units")
    ("nomarkers,m", po::bool_switch(&parameters->markers), 
     "draw gene lines WITHOUT markers for gene nodes") 
    ("ladderize,l", po::value<char>(&parameters->ladd)->default_value('r'), 
     "<string> ladderize right (r) or left (l)")
    ("legend,L", po::bool_switch(&parameters->legend), "activate the legend")
    ("header,H", po::bool_switch(&parameters->header), "activate the header")
    ("text,T", po::value<string>(&parameters->tittleText),
     "<string> include the text on the top of the image")
    ("mark,x", po::value<std::vector<double> >(&parameters->uMarker)->multitoken(),
     "<int>....<int> Highlight the nodes indicated")
    ("marksize,z", po::value<int>(&parameters->markerscale), "<int> change the size of gene tree markers")
    ("lateralT,P", po::value<std::vector<float> >()->multitoken(),
       "<float>: [<min>] [<max>] [<dupli. cost>] [<trans. cost>] Whether to include lateral transfers or not (cost)")
    ("vertical,V", po::bool_switch(&parameters->horiz)->default_value(true), "vertical orientation, horizontal by default");

  
  // Hidden options, will be allowed both on command line and
  // in config file, but will not be shown to the user.
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("input-file", po::value< vector<string> >(), "input files: ");
    
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  po::options_description config_file_options;
  config_file_options.add(config).add(hidden);

  po::options_description visible("Allowed options");
  visible.add(generic).add(config);
        
  po::positional_options_description p;
  p.add("input-file", -1);
       
  po::variables_map vm;
  store(po::command_line_parser(ac, av).
  options(cmdline_options).positional(p).run(), vm);
  notify(vm);    
    
  ifstream ifs(config_file.c_str());
        
  if (!ifs)
  {
      cout << "can not open config file: " << config_file << "\n";
  }
  else
  {
      store(parse_config_file(ifs, config_file_options), vm);
      notify(vm);
  }
    
  if (ac < 2 || (!strcmp(av[1], "-?") || !strcmp(av[1], "--?") || !strcmp(av[1], "/?") 
  || !strcmp(av[1], "/h") || !strcmp(av[1], "-h") || !strcmp(av[1], "--h") || !strcmp(av[1], "--help") 
  || !strcmp(av[1], "/help") || !strcmp(av[1], "-help") || !strcmp(av[1], "help") ))
  {
      cout << visible << "\n" << "\n Default input <guest tree> [<host tree>] [<options>]\n Alternative input"
     "(option -R) <gene tree> <species tree> <map file> [<options>] \n";
      return 0;
  }

  if (vm.count("version")) {
      cout << "PrimeTV, version 1.0\n";
      return 0;
  }
  
  if(vm.count("color") > 1)
  {
    parameters->colorConfig->setColors(vm["color"].as<string>().c_str());
  }
  
  
  if (vm.count("input-file"))
  {
    int size = vm["input-file"].as< vector<string> >().size();
     
    if(size <= 1)
    {
	cout << "No trees given" << "\n";
	return 0;
    }
    
    BOOST_FOREACH(string s,vm["input-file"].as< vector<string> >())
    {
      if(!file_exist(s.c_str()))
      {
	cout << "One/s of the filename given don't exist" <<"\n";
	return 0;
      }
    }
   
    if(!parameters->isreconciled && size == 2)
    {
      reconciledtree = vm["input-file"].as< vector<string> >().at(0).c_str();
      
      if (vm["input-file"].as< vector<string> >().size() == 1)
      {
	parameters->do_not_draw_species_tree = true;
      }
      else if (vm["input-file"].as< vector<string> >().size() == 2)
      {
	speciestree = vm["input-file"].as< vector<string> >().at(1).c_str();
      }
    }
    else if (parameters->isreconciled && size == 3 )
    {
      genetree = vm["input-file"].as< vector<string> >().at(0).c_str();
      speciestree = vm["input-file"].as< vector<string> >().at(1).c_str();
      mapfile = vm["input-file"].as< vector<string> >().at(2).c_str();
      
    }else
    {
      cout << "Incorrect number of arguments" << "\n";
      return 0;
    }
    
    //TODO
    //function check trees, check they exist and they are okay
  }
  
  if (vm.count("notimescale"))
  {
      parameters->scaleByTime = false;
      parameters->noTimeAnnotation = true;
      parameters->equalTimes = false;
  }
  
  if (vm.count("noguest"))
  {
      if(parameters->do_not_draw_guest_tree)
	parameters->markers = false;  
  }   
  
  if (vm.count("nohost"))
  {
      if(parameters->do_not_draw_species_tree)
      {
	parameters->scaleByTime = false; 
	parameters->equalTimes = false;
	parameters->noTimeAnnotation = true;
  
      }
  }

  if (vm.count("text"))
  {
    parameters->tittle = true; 
  }
  
  if (vm.count("mark"))
  {
    parameters->isMarkerColor = true;
    parameters->markers = true;
  }
  
  if (vm.count("lateralT"))
  {
     parameters->lateralmincost = vm["lateralT"].as< vector<float> >().at(0);
     parameters->lateralmaxcost = vm["lateralT"].as< vector<float> >().at(1);
     parameters->lateralduplicost = vm["lateralT"].as< vector<float> >().at(2);
     parameters->lateraltrancost = vm["lateralT"].as< vector<float> >().at(3);
     
     if(parameters->lateralmincost < 1.0 || parameters->lateralmincost > 10.0)
	parameters->lateralmincost = 1.0;
     if(parameters->lateralmaxcost < 1.0 || parameters->lateralmaxcost > 10.0)
	parameters->lateralmaxcost = 1.0;
     if(parameters->lateralduplicost < 1.0 || parameters->lateralduplicost > 10.0)
	parameters->lateralduplicost = 1.0;
     if(parameters->lateraltrancost < 1.0 || parameters->lateraltrancost > 10.0)
	parameters->lateraltrancost = 1.0;
     
     parameters->lattransfer = true;
     
     //TODO do a better check of wheter to use DP or not
  }
  
  if (vm.count("fontscale") > 1)
  {
     if(parameters->fontscale >= 20 || parameters->fontscale < 1)
	parameters->fontscale = 10; 

  }
 
  if (vm.count("ladderize") > 1)
  {
    if (parameters->ladd != 'r' && parameters->ladd != 'l')
        parameters->ladd = 'r';
  }
  
  if (vm.count("size"))
  { 
    //TODO check more possible erroneous combinations
    if (vm["size"].as< vector<float> >().size() == 2)
    {
      parameters->width = vm["size"].as< vector<float> >().at(0);
      parameters->height = vm["size"].as< vector<float> >().at(1);
    
      if (parameters->width < 400 || parameters->width > 2000)
	parameters->width = 800;
   
      if(parameters->height < 400 || parameters->height > 2000)
	parameters->height = 1000;
    } 
  }
  
  if (vm.count("move"))
  { 
    //TODO check more possible erroneous combinations
    if (vm["move"].as< vector<float> >().size() == 2)
    {
      parameters->xoffset = vm["move"].as< vector<float> >().at(0);
      parameters->yoffset = vm["move"].as< vector<float> >().at(1);
    
      if (abs(parameters->xoffset) > parameters->width / 2)
	parameters->xoffset = 0.0;
   
      if(abs(parameters->yoffset) > parameters->height/2)
	parameters->yoffset = 0.0;
    } 
  }


//********************************************************************************************//

    
    if(parameters->UI) //We start the User Interface
    {
	
      QApplication app(ac, av);
      MainWindow *appWindow = new MainWindow(parameters);
      appWindow->show();
      return app.exec();
      delete(appWindow);
      delete(parameters);
    }
    else // We start the script version
    { 
      Mainops *main = new Mainops(); //object that cointains all the main operations
      main->setParameters(parameters);

      //if we don't need to reconcile the trees
      if(!parameters->isreconciled)
      {
	main->OpenReconciled(reconciledtree);
	main->OpenHost(speciestree);
	
      }
      else
      {
	main->reconcileTrees(genetree,speciestree,mapfile);
      }
      
      //we calculate the LGT scenarios is indicated
      if(parameters->lattransfer)
      { 
 	if(parameters->lateralmincost == 1.0 && parameters->lateralmaxcost == 1.0) //do it with parameters
	  main->lateralTransferDP(mapfile);
 	else
 	  main->lateralTransfer(mapfile);
      }
      
      main->CalculateGanmma(); //calculation of gamma and lambda
      main->calculateCordinates(); //calculation of the drawing cordinates
      main->DrawTree();  //drawing the tree
      main->RenderImage(); // save the file
      delete(parameters);
//       delete(main);
    }
  }
  catch (AnError e) {
    e.action();
  }
  catch(const boost::bad_any_cast& ex)
  {
    cerr << "The format of any of the parameters is not correct\n";
  }
  catch (const std::exception& e) {
    cerr << e.what() << endl;
    return 1;
  }

  return EXIT_SUCCESS;
}

