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

#include <iostream>
#include <vector>
#include <exception>
#include <cstddef>
#include <fstream>
#include <sys/stat.h>

#include "Parameters.h"
#include "Mainops.h"
#include "utils/AnError.h"
#include "assert.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <QApplication>

#include "gui/Windows.h"

#include "options_cmake.h"

using namespace boost;
namespace po = boost::program_options;
using namespace std;

static const std::string MAJOR = boost::lexical_cast<std::string>(VERSION_MAJOR);
static const std::string MINOR = boost::lexical_cast<std::string>(VERSION_MINOR);
static const std::string PATCH = boost::lexical_cast<std::string>(VERSION_REVISION);
static const std::string VERSION =  ((MAJOR + "." + MINOR) + ".") + PATCH;

//member objects //TODO make them singleton
Parameters *parameters = 0;
Mainops *mainops = 0;

// Helper function to sort vectors used in the parser of parameters
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}

// Helper function that checks if a filename exists in the system
int file_exist(const char *filename)
{
    struct stat buffer;
    return(stat(filename, &buffer) == 0);
}

void cleanUp()
{
    if (parameters)
    {
        delete parameters;
    }
    parameters = 0;

    if (mainops)
    {
        delete mainops;
    }
    mainops = 0;
}

int
main (int ac, char *av[])
{

// The boost progam options object is being used to capture all the parameters from console
// it declares three types of parameters (generic, hidden and configuration)
// it loads all of them and checks their basic validity, it also loads
// a configuration file "~/.primetv2.cfg" to load all the information included in the file
// once the parameters have been read and captured they all processed, valided and
// assigned to the object container (Parameters)

    try
    {
        parameters = new Parameters();
        std::string config_file;
        std::string default_config_file;
        std::string reconciledtree;
        std::string speciestree;
        std::string genetree;
        std::string mapfile;
        std::string precomputed_scenario_file;
        std::string colorconfig;
        bool show_lgt_scenarios = false;
        bool load_precomputed_lgt_scenario = false;
        // Create path to default config file

        char *homepath = getenv("HOME");
        if (homepath)
        {
            // Use this if there is home directory we can use
            default_config_file = string(homepath) + "/.primetv2.cfg";
        }
        else
        {
            // If we don't find home, then create/look at a configuration in current dir.
            default_config_file = "primetv2.cfg";
        }


        // Declare a group of options that will be
        // allowed only on command line
        po::options_description generic("Generic options");
        generic.add_options()
                ("version,v", "Print version string")
                ("help,h", "Produce help message")
                ("gui", po::bool_switch(&parameters->UI), "Initiates a Graphical User Interface")
                ("reconciled,r", po::bool_switch(&parameters->isreconciled)->default_value(false),
                 "Indicates that the Guest tree is not reconciled. By default,"
                 "it is assumed that the Guest Tree is already reconciled in PRIME format."
                 "This option requires a third input file which maps guest tree leaves to host tree leaves")
                ("config,C", po::value<string>(&config_file)->default_value(default_config_file),
                 "Name of a file of a configuration.");

        // Declare a group of options that will be
        // allowed both on command line and in
        // config file
        po::options_description config("Configuration");
        config.add_options()
                ("color,c",po::value<string>(&colorconfig)->default_value("1"),
                 "Set colors according to <spec> which can be 'blue', 'kth', 'su', 'grey', 'mono', or 'yellow'."
                 "You can also use integers 1, 2,... for shortcut.")
                ("notimescale,t", "Don't scale host edges by time")
                ("timeex,e", po::bool_switch(&parameters->timeAtEdges),
                 "Annotate host edges by their time extent")
                ("notime,n", po::bool_switch(&parameters->noTimeAnnotation),"No time annotation at all")
                ("format,f", po::value<string>(&parameters->format)->default_value("pdf"),
                 "File format: pdf(Pdf) ps(PostScript) png(PNG) svg")
                ("nohost,y", po::bool_switch(&parameters->do_not_draw_species_tree)->default_value(false),
                 "No host tree is given nor shown")
                ("noguest,g", po::bool_switch(&parameters->do_not_draw_guest_tree)->default_value(false),
                 "No guest tree is shown")
                ("file,o", po::value<string>(&parameters->outfile)->default_value("image"),
                 "<string> name of the output file")
                ("inode,i", po::bool_switch(&parameters->ids_on_inner_nodes),
                 "Put node numbers in the inner nodes")
                ("size,p", po::value<std::vector<float> >()->multitoken(),
                 "Image size in pixels <width> <height>")
                ("guest-font,j", po::value<string>(&parameters->gene_font)->default_value("Times"),
                 "<string> use this font for guest tree labels (def. Times) possible options = \"serif\","
                 "\"sans-serif\", \"Purisa\", \"Sans\", \"monospace\" ..etc")
                ("host-font,q", po::value<string>(&parameters->species_font)->default_value("Times"),
                 "<string> use this font for host node labels (def. Times) possible options = \"serif\","
                 "\"sans-serif\", \"Purisa\", \"Sans\", \"monospace\" ..etc")
                ("all-font,A", po::value<string>(&parameters->all_font)->default_value("Times"),
                 "<string> use this font for the rest of text labels (def. Times) possible options = \"serif\","
                 "\"sans-serif\", \"Purisa\", \"Sans\", \"monospace\" ..etc")
                ("fontscale,s", po::value<float>(&parameters->fontscale)->default_value(1.0),
                 "<float> scale fonts by this number")
                ("genefontsize,G", po::value<float>(&parameters->gene_font_size)->default_value(10.0),
                 "<float> Size of font for gene nodes (def. 10.0)")
                ("speciesfontsize,Z", po::value<float>(&parameters->species_font_size)->default_value(10.0),
                 "<float> Size of font for species nodes (def. 10.0)")
                ("allfontsize,W", po::value<float>(&parameters->fontsize)->default_value(10.0),
                 "<float> Size of font for the rest of the labels (def. 10.0)")
                ("imagescale,S", po::value<float>(&parameters->imagescale)->default_value(1.0),
                 "<float> scale image by this number")
                ("move,D", po::value<std::vector<float> >()->multitoken(),
                 "<float> <float> move the image x,y units")
                ("nomarkers,m", po::bool_switch(&parameters->markers),
                 "Draw guest tree lines WITHOUT markers for guest tree nodes")
                ("ladderize,a", po::value<char>(&parameters->ladd)->default_value('r'),
                 "<string> ladderize right (r) or left (l)")
                ("legend,L", po::bool_switch(&parameters->legend), "Activate the legend")
                ("header,H", po::bool_switch(&parameters->header), "Activate the header")
                ("text,T", po::value<string>(&parameters->titleText),
                 "<string> include the text on the top of the image.")
                ("mark,x", po::value<std::vector<double> >(&parameters->uMarker)->multitoken(),
                 "<unsigned>....<unsigned> Highlight the nodes indicated.")
                ("marksize,z", po::value<unsigned>(&parameters->markerscale), "<unsigned> change the size of guest tree markers.")
                ("lgt,l", po::bool_switch(&parameters->lattransfer),
                 "Allow lateral transfer (LGT) when reconciling.")
                ("event-costs,P", po::value<std::vector<float> >()->multitoken(),
                 "<float>: [<min>] [<max>] [<dupli. cost>] [<trans. cost>] Parameters that give (1) minimum reconciliation cost,"
                 "(2) maximum reconciliation, (3) duplication cost, and (4) LGT cost.")
                ("show-event-count", po::bool_switch(&parameters->show_event_count),
                 "Show the number of duplications and transfers used in the computed reconciliation.")
                ("vertical,V", po::bool_switch(&parameters->horiz)->default_value(false),
                 "Vertical orientation, horizontal by default.")
                ("reduce,R", po::bool_switch(&parameters->reduce)->default_value(false),
                 "Reduce number of crossing lines, false by default.")
                ("show-scenarios,b", po::bool_switch(&show_lgt_scenarios)->default_value(false),
                 "Output on the screen all pre-computed LGT scenarios(only valid if option -l is activated).")
                ("draw-all-lgt,Y", po::bool_switch(&parameters->drawAll)->default_value(false),
                 "Draw a file for each pre-computed LGT scenario.")
                ("precomputed-lgt-scenario,X", po::value<string>(&precomputed_scenario_file),
                 "<string> name of the file containing the scenario as: \nTransfer edges Numbers: (9,2,0.12)\n"
                 "Where 9 is origin, 2 destiny and 0.12 time.");


        // Hidden options, will be allowed both on command line and
        // in config file, but will not be shown to the user.
        po::options_description hidden("Hidden options");
        hidden.add_options()("input-file", po::value< vector<string> >(), "input files: ");

        po::options_description cmdline_options;
        cmdline_options.add(generic).add(config).add(hidden);

        po::options_description config_file_options;
        config_file_options.add(config).add(hidden);

        po::options_description visible("Allowed options");
        visible.add(generic).add(config);
        
        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;
        store(po::command_line_parser(ac, av).options(cmdline_options).positional(p).run(), vm);
        notify(vm);

        ifstream ifs(config_file.c_str());
        
        if (!ifs && config_file != default_config_file)
        {
            std::cerr << "can not open config file: " << config_file << std::endl;
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
                    "(option -r) <gene tree> <species tree> <map file> [<options>] \n";
            return EXIT_SUCCESS;
        }

        if (vm.count("version"))
        {
            cout << "PrimeTV, version " << VERSION << std::endl;
            return EXIT_SUCCESS;
        }

        if(vm.count("color") == 1)
        {
            parameters->colorConfig->setColors(vm["color"].as<string>().c_str());
        }

        if (vm.count("input-file"))
        {
            int size = vm["input-file"].as< vector<string> >().size();

            if(size <= 1)
            {
                std::cerr << "No trees given" << std::endl;
                return EXIT_FAILURE;
            }

            BOOST_FOREACH(string s,vm["input-file"].as< vector<string> >())
            {
                if (!file_exist(s.c_str()))
                {
                    std::cerr << "The file name given : " << s.c_str() << " does not exist " << std::endl;
                    return EXIT_FAILURE;
                }
            }

            if (!(bool)(parameters->isreconciled) && (size == 1 || size == 2))
            {
                reconciledtree = vm["input-file"].as< vector<string> >().at(0);

                if (size == 1)
                {
                    parameters->do_not_draw_species_tree = true;
                }
                else
                {
                    speciestree = vm["input-file"].as< vector<string> >().at(1);
                }
            }
            else if (parameters->isreconciled && size == 3 )
            {
                genetree = vm["input-file"].as< vector<string> >().at(0);
                speciestree = vm["input-file"].as< vector<string> >().at(1);
                mapfile = vm["input-file"].as< vector<string> >().at(2);

            }
            else
            {
                std::cerr << "Incorrect number of arguments" << std::endl;
                return EXIT_FAILURE;
            }

            //TODO
            //function check trees, check they exist and they are well formed and proper format
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
            {
                parameters->markers = false;
            }
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
            parameters->title = true;
        }

        if (vm.count("mark"))
        {
            parameters->isMarkerColor = true;
            parameters->markers = true;
        }

        if (vm.count("event-costs"))
        {
            parameters->lateralmincost = vm["event-costs"].as< vector<float> >().at(0);
            parameters->lateralmaxcost = vm["event-costs"].as< vector<float> >().at(1);
            parameters->lateralduplicost = vm["event-costs"].as< vector<float> >().at(2);
            parameters->lateraltrancost = vm["event-costs"].as< vector<float> >().at(3);

            if(parameters->lateralmincost < 1.0 || parameters->lateralmincost > 10.0)
            {
                parameters->lateralmincost = 1.0;
            }
            if(parameters->lateralmaxcost < 1.0 || parameters->lateralmaxcost > 10.0)
            {
                parameters->lateralmaxcost = 1.0;
            }
            if(parameters->lateralduplicost < 1.0 || parameters->lateralduplicost > 10.0)
            {
                parameters->lateralduplicost = 1.0;
            }
            if(parameters->lateraltrancost < 1.0 || parameters->lateraltrancost > 10.0)
            {
                parameters->lateraltrancost = 1.0;
            }
            parameters->lattransfer = true;

            //TODO do a better check of wheter to use DP or not
        }

        if (vm.count("fontscale") > 1)
        {
            if(parameters->fontscale >= 20 || parameters->fontscale < 1)
            {
                parameters->fontscale = 10;
            }
        }

        if (vm.count("ladderize") > 1)
        {
            if (parameters->ladd != 'r' && parameters->ladd != 'l')
            {
                parameters->ladd = 'r';
            }
        }

        if (vm.count("size"))
        {
            //TODO check more possible erroneous combinations
            if (vm["size"].as< vector<float> >().size() == 2)
            {
                parameters->width = vm["size"].as< vector<float> >().at(0);
                parameters->height = vm["size"].as< vector<float> >().at(1);

                if (parameters->width < 400 || parameters->width > 6000)
                {
                    parameters->width = 1200;
                }

                if(parameters->height < 400 || parameters->height > 6000)
                {
                    parameters->height = 1400;
                }
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
                {
                    parameters->xoffset = 0.0;
                }
                if(abs(parameters->yoffset) > parameters->height / 2)
                {
                    parameters->yoffset = 0.0;
                }
            }
        }

        if (vm.count("precomputed-lgt-scenario"))
        {
            if ((bool)(parameters->lattransfer))
            {
                std::cerr << "The option -X(precomputed-lgt-scenario) cannot be used together"
                             "with the option -l(lgt).." << std::endl;
                return EXIT_FAILURE;
            }
            if(show_lgt_scenarios)
            {
                std::cerr << "The option -X(precomputed-lgt-scenario) cannot be used together"
                             "with the option -b(show-scenarios).." << std::endl;
                return EXIT_FAILURE;
            }
            if ((bool)(parameters->drawAll))
            {
                std::cerr << "The option -X(precomputed-lgt-scenario) cannot be used together"
                             "with the option -Y(draw-all-lgt).." << std::endl;
                return EXIT_FAILURE;
            }
            load_precomputed_lgt_scenario = true;
        }


        if(show_lgt_scenarios && !(bool)(parameters->lattransfer))
        {
            std::cerr << "The option -b(show-scenarios) has to be used together with"
                         "the option -l(lgt).." << std::endl;
            return EXIT_FAILURE;
        }

        if ((bool)(parameters->drawAll) && !(bool)(parameters->lattransfer))
        {
            std::cerr << "The option -Y(draw-all-lgt) has to be used together with"
                         "the option -l(lgt).." << std::endl;
            return EXIT_FAILURE;
        }

        //********************************************************************************************//

        mainops = new Mainops(); //object that cointains all the main operations
        mainops->start();
        mainops->setParameters(parameters);

        if ((bool)(parameters->UI)) //We start the User Interface
        {
            QApplication app(ac, av);
            MainWindow *appWindow = new MainWindow(parameters,mainops);
            appWindow->show();
            return app.exec();
            delete appWindow;
        }
        else // We start the script version
        {
            //if we don't need to reconcile the trees
            if (!(bool)(parameters->isreconciled))
            {
                mainops->OpenReconciled(reconciledtree);
                mainops->OpenHost(speciestree);
            }
            else
            {
                mainops->reconcileTrees(genetree,speciestree,mapfile);
            }
            //we calculate the LGT scenarios if indicated
            if ((bool)(parameters->lattransfer))
            {
                mainops->lateralTransfer(mapfile,(parameters->lateralmincost == 1.0
                                                  && parameters->lateralmaxcost == 1.0));
            }
            else if(load_precomputed_lgt_scenario)
            {
                mainops->loadPreComputedScenario(precomputed_scenario_file,mapfile);
            }

            if(parameters->drawAll)
            {
                mainops->drawAllLGT();
            }
            else
            {
                mainops->drawBest();
            }

            if(show_lgt_scenarios)
            {
                mainops->printLGT();
            }

            std::cout << "The tree/s were generated succesfully" << std::endl;

        }
    }
    catch (const AnError &e)
    {
        cleanUp();
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(const boost::bad_any_cast& ex)
    {
        cleanUp();
        cerr << "The format of any of the parameters is not correct\n";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        cleanUp();
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        cleanUp();
        cerr << "Unknown exception, contact the developer.." << std::endl;
        return EXIT_FAILURE;
    }

    cleanUp();
    return EXIT_SUCCESS;
}
