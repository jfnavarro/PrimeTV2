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

/* This class implements the User Interface, it inherits from three QT objects
 *  the MainWindow class and the Ui_MainWindo and Ui_parameters canvas that
 * are object created with UI designer and dynamically linked to the application
 */


#ifndef WINDOWS_H
#define WINDOWS_H

#include <QFileDialog>
#include <QTemporaryFile>
#include <QMainWindow>

#include "ui_primetv.h"
#include "ui_parameters.h"

class ConfigFile;
class Mainops;
class Canvas;
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class Parameters;
class QWidget;
class QGraphicsScene;
class QString;

class MainWindow : public QMainWindow, public Ui_MainWindow, public Ui_Parameters
{

    Q_OBJECT

public:

    // constructor we only need the parameters object and the parent if there is any
    explicit MainWindow(Parameters *p, Mainops *m, QMainWindow *parent = 0);
    //destructor
    virtual ~MainWindow();
    
private slots:
    
    // launches an open file dialog to select and load the guest tree
    void loadGuest();
    
    // launches an open file dialog to select and load the host tree
    void loadHost();
    
    // reconciles the trees, calculate gamma and lambda, the cordinates and draw the tree on the screen
    void generateTree();
    
    // exit the application
    void exit();
    
    // save the canvas in a file
    void save();
    
    // launches an open file dialog to select and load the map file if the reconcile option is activated
    void loadMap();
    
    // overloaded function called every time a parameter is modified to update the parameters
    void update();
    
    // reset the canvas and the parameters
    void newImage();
    
    // show or hide the parameters panel
    void showParameters(bool);
    
    // to activate the reconcilation mode, therefore the open map button will be enabled, because a map file is needed
    // to reconcile
    void activateReconcilation();
    
    // to print the canvas
    void print();
    
    // a color selection dialog will show up and the color selected with will assigned the current font
    void loadFontColor();
    
    // launches and open dialog to load a configuration file where all the parameters will be stored
    void loadConfigFile();
    
    // open a dialog to save the current configuration into a config file
    void saveConfigFile();

private:
    
    // creates an open file dialog and returns the file chosen
    QString openFile(QString header);
    
    // paint the current tree on the canvas
    void paintTree();
    
    // function that create all the actions and events asigned to the buttons
    void createActions();
    
    // function that load the actual values of parameters in case the user has given parameters as input
    // in the console
    void loadParameters(Parameters *parameters);
    
    // overloaded close Event function to handle the exit
    void closeEvent(QCloseEvent *e);
    
    QWidget *params; //parameters panel
    Canvas *canvas; //pixmap where tree is loaded
    QGraphicsScene *scene; //main scene
    
    QString lastVisitedDir;
    Mainops *ops; // main operations
    Parameters *parameters;
    QString reconciledtree;
    QString speciestree;
    QString genetree;
    QString mapfile;
    bool guestTree;
    bool hostTree;
    bool isPainted;
    bool mapfileStatus;
    ConfigFile *config;
};

#endif // WINDOWS_H
