/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

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

#include <QX11Info>
#include <QDebug>
#include <QPainter>
#include <QtGui>
#include <qfileinfo.h>
#include <QColorDialog>
#include <cairo-xlib-xrender.h>
#include "windows.h"


#define QUALITY 75  //0-100

MainWindow::MainWindow(Parameters *p, Mainops *m, QWidget *parent)
  :QMainWindow(parent),lastVisitedDir(),ops(m),parameters(p),guestTree(false),
        hostTree(false),menuparameters(false),isPainted(false),mapfileStatus(false),config(0)
{

    Ui_MainWindow::setupUi(this);
    params = new QWidget();
    Ui_Parameters::setupUi(params);
    params->setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint
                           | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint ); //| Qt::WindowStaysOnTopHint
    params->show();
    params->hide();

    verticalLayout = new QVBoxLayout(centralwidget);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    scrollArea = new QScrollArea();
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    verticalLayout->addWidget(scrollArea);

    widget = new Canvas(scrollArea);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(0, 25, parameters->width,parameters->height));

    scrollArea->setWidget(widget);

    loadParameters(parameters);
    createActions();

    actionLoad_Map_File->setEnabled(false);
    actionSave->setEnabled(false);
    actionShow_LGT_Scenarios->setEnabled(false);
    actionHelp->setEnabled(false);
    actionVersion->setEnabled(false);

    QString message = tr("PrimeTV GUI");
    statusBar()->showMessage(message);

    mapfile = "";
    QRect screen = QApplication::desktop()->screenGeometry();
    resize((unsigned)((screen.width())/2),(unsigned)((screen.height())/2));

    widget->paintCanvas();
    widget->update();
}

MainWindow::MainWindow(const MainWindow& other)
{

}

MainWindow::~MainWindow()
{
    if(verticalLayout)
    {
      delete(verticalLayout);
      verticalLayout = 0;
    }
    if(scrollArea)
    {
      delete(scrollArea);
      scrollArea = 0;
    }
    if(params)
    {
      delete(params);
      params = 0;
    }
    if(widget)
    {
      delete(widget);
      widget = 0;
    }
    if(config)
    {
      delete(config);
      config = 0;
    }
}

void MainWindow::loadGuest()
{
    QString temp = openFile(tr("Open Gene Tree"));

    char* text = new char[temp.length() + 1];
    strcpy(text, temp.toLatin1().constData());

    if (!checkBoxReconcile->checkState())
    {
        reconciledtree = text;
    }
    else
    {
        genetree = text;
    }

    if (strcmp(text,"") != 0)
    {
        guestTree = true;
        std::stringstream ss;
        ss << "Guest Tree : LOADED " << text;
        statusBar()->showMessage(tr(ss.str().c_str()));
    }
    else
    {
        guestTree = false;
        statusBar()->showMessage(tr("File not loaded"));
    }
}

void MainWindow::loadHost()
{
    QString temp = openFile(tr("Open Species Tree"));

    char* text = new char[temp.length() + 1];
    strcpy(text, temp.toLatin1().constData());

    speciestree = text;

    if (strcmp(text,"") != 0)
    {
        hostTree = true;
        std::stringstream ss;
        ss << "Host Tree : LOADED " << text;
        statusBar()->showMessage(tr(ss.str().c_str()));
    }
    else
    {	
        hostTree = false;
        statusBar()->showMessage(tr("File not loaded"));
    }
}

void MainWindow::generateTree()
{
    try
    {
        if ( ((!hostTree || !guestTree) && !parameters->isreconciled) ||
                ( (!hostTree || !guestTree || !mapfileStatus) && parameters->isreconciled))
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage("You have to load the trees before or the trees loaded are not valid");
            errorMessage.exec();

        }
        else
        {   
            ops->setParameters(parameters);
	     ops->cleanTrees();
	     ops->start();
            
	    if (!checkBoxReconcile->checkState())
            {
                ops->OpenReconciled(reconciledtree);
                ops->OpenHost(speciestree);
            }
            else
            {
                ops->reconcileTrees(genetree,speciestree,mapfile);
            }

            if (parameters->lattransfer)
	    {
		ops->lateralTransfer(mapfile,(parameters->lateralmincost == 1.0 && parameters->lateralmaxcost == 1.0));
            }
            
            ops->CalculateGamma();

            paintTree();

            actionSave->setEnabled(true);
            pushButtonMoveDown->setEnabled(true);
            pushButtonMoveLeft->setEnabled(true);
            pushButtonMoveRight->setEnabled(true);
            pushButtonMoveUp->setEnabled(true);
            pushButtonZoomOut->setEnabled(true);
            pushButtonZoomIn->setEnabled(true);
            statusBar()->showMessage(tr("Tree Generated"));
        }

    }
    catch(AnError &e)
    {
       QErrorMessage errorMessage;
       errorMessage.showMessage(e.what());
       errorMessage.exec();
       guestTree = false;
       hostTree = false;
    }
    catch (std::exception&  e)
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage(e.what());
        errorMessage.exec();
	 guestTree = false;
	 hostTree = false;
    }
    catch (...)
    {
      qDebug() << "Unknown Exception";
    }

}

void MainWindow::paintTree()
{
    //widget->paintCanvas();
    ops->calculateCordinates();
    ops->DrawTree(widget->getCairoCanvas());
    widget->update();
    isPainted = true;
    repaint();
}

void MainWindow::exit()
{
    params->close();
    QCoreApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    params->close();
    QCoreApplication::exit();
}

void MainWindow::update()
{
    //TODO i need to restorei imagescale and offset to original prior increasing
    //     the increasing amoung should be stored
    if (QObject::sender() == pushButtonZoomIn)
    {
        parameters->imagescale += 0.10;
    }
    else if (QObject::sender() == pushButtonZoomOut)
    {
        parameters->imagescale -= 0.10;
    }
    else if (QObject::sender() == pushButtonMoveDown)
    {
        parameters->yoffset++;
    }
    else if (QObject::sender() == pushButtonMoveUp)
    {
        parameters->yoffset--;
    }
    else if (QObject::sender() == pushButtonMoveLeft)
    {
        parameters->xoffset--;
    }
    else if (QObject::sender() == pushButtonMoveRight)
    {
        parameters->xoffset++;
    }

    parameters->title = checkBoxHeader->isChecked();
    lineEditHeaderText->setEnabled(checkBoxHeader->isChecked());

    if (checkBoxHeader->isChecked())
    {
        QString text = lineEditHeaderText->text();
        QByteArray ba = text.toLocal8Bit();
        parameters->titleText = ba.data();
    }
    else
    {
        lineEditHeaderText->setText(tr(""));
    }

    parameters->horiz = checkBoxHV->isChecked();
    parameters->markers = checkBoxMarkers->isChecked();
    parameters->header = checkBoxLogo->isChecked();
    parameters->do_not_draw_guest_tree = checkBoxGuest->isChecked();
    parameters->do_not_draw_species_tree = checkBoxHost->isChecked();

    if (checkBoxLadderize->isChecked())
        parameters->ladd = 'l';
    else
        parameters->ladd = 'r';

    parameters->legend = checkBoxLegend->isChecked();
    parameters->ids_on_inner_nodes = checkBoxINodes->isChecked();

    parameters->fontscale = doubleSpinBoxTextScale->value();
    parameters->width = spinBoxHorizontalSize->value();
    parameters->height = spinBoxVerticalSize->value();
    
    parameters->fontsize = spinBoxAllFontSize->value();
    parameters->gene_font_size = spinBoxGeneFontSize->value();
    parameters->species_font_size = spinBoxSpeciesFontSize->value();
    
    if (parameters->horiz)
        widget->resize(parameters->height,parameters->width);
    else
        widget->resize(parameters->width,parameters->height);

    if (QObject::sender() == comboBoxTime)
    {
        if (comboBoxTime->currentIndex() == 0)
        {
            parameters->scaleByTime = true;
            parameters->noTimeAnnotation = false;
            parameters->equalTimes = true;
            parameters->timeAtEdges = false;

        }
        else if (comboBoxTime->currentIndex() == 1)
        {
            parameters->scaleByTime = false;
            parameters->noTimeAnnotation = true;
            parameters->equalTimes = false;
            parameters->timeAtEdges = false;
        }
        else if (comboBoxTime->currentIndex() == 2)
        {
            parameters->scaleByTime = true;
            parameters->noTimeAnnotation = false;
            parameters->equalTimes = false;
            parameters->timeAtEdges = true;
        }
    }
    parameters->species_font = QFontInfo(fontComboBoxSpecies->currentFont()).family().toUtf8().constData();
    parameters->gene_font = QFontInfo(fontComboBoxGene->currentFont()).family().toUtf8().constData();
    parameters->all_font = QFontInfo(fontComboBoxAll->currentFont()).family().toUtf8().constData();

    if (radioButtonColor1->isChecked())
        parameters->colorConfig->setColors("1");
    else if (radioButtonColor2->isChecked())
        parameters->colorConfig->setColors("2");
    else if (radioButtonColor3->isChecked())
        parameters->colorConfig->setColors("3");
    else if (radioButtonColor4->isChecked())
        parameters->colorConfig->setColors("4");

    if (isPainted && hostTree && guestTree)
    {
        paintTree();
    }
}

void MainWindow::newImage()
{
    guestTree = false;
    hostTree = false;
    menuparameters = false;
    isPainted = false;
    mapfileStatus = false;
    parameters = new Parameters();
    loadParameters(parameters);
    widget->resize(parameters->width,parameters->height);
    widget->paintCanvas();
    actionLoad_Map_File->setEnabled(false);
    actionSave->setEnabled(false);
    statusBar()->showMessage(tr("New tree"));
    widget->paintCanvas();
    widget->update();
    repaint();
}

void MainWindow::loadParameters(Parameters *parameters)
{
    fontComboBoxGene->setCurrentFont(QFont(QString::fromStdString(parameters->gene_font)));
    fontComboBoxSpecies->setCurrentFont(QFont(QString::fromStdString(parameters->species_font)));
    fontComboBoxAll->setCurrentFont(QFont(QString::fromStdString(parameters->all_font)));
    doubleSpinBoxTextScale->setValue(parameters->fontscale);
    checkBoxMarkers->setChecked(parameters->markers);

    if (parameters->ladd == 'r')
        checkBoxLadderize->setChecked(false);
    else if (parameters->ladd == 'l')
        checkBoxLadderize->setChecked(true);

    checkBoxReconcile->setChecked(parameters->isreconciled);
    checkBoxGuest->setChecked(parameters->do_not_draw_guest_tree);
    checkBoxHV->setChecked(parameters->horiz);
    checkBoxLogo->setChecked(parameters->header);
    checkBoxLegend->setChecked(parameters->legend);
    checkBoxHeader->setChecked(parameters->title);
    checkBoxLGT->setChecked(parameters->lattransfer);
    checkBoxHost->setChecked(parameters->do_not_draw_species_tree);
    checkBoxINodes->setChecked(parameters->ids_on_inner_nodes);

    int set = atoi(parameters->colorConfig->getSet());
    switch (set)
    {
    case(1):
        radioButtonColor1->setChecked(true);
        break;
    case(2):
        radioButtonColor2->setChecked(true);
        break;
    case(3):
        radioButtonColor3->setChecked(true);
        break;
    case(4):
        radioButtonColor4->setChecked(true);
        break;

    default:
        radioButtonColor1->setChecked(true);
    }

    lineEditHeaderText->setText(QString::fromStdString(parameters->titleText));
    spinBoxVerticalSize->setValue(parameters->height);
    spinBoxHorizontalSize->setValue(parameters->width);
    spinBoxMaxCost->setValue(parameters->lateralmaxcost);
    spinBoxDupliCost->setValue(parameters->lateralduplicost);
    spinBoxSpeCost->setValue(parameters->lateraltrancost);
    spinBoxMinCost->setValue(parameters->lateralmincost);
    
    
    spinBoxAllFontSize->setValue(parameters->fontsize);
    spinBoxGeneFontSize->setValue(parameters->gene_font_size);
    spinBoxSpeciesFontSize->setValue(parameters->species_font_size);
    
    if (parameters->horiz)
        widget->resize(parameters->height,parameters->width);
    else
        widget->resize(parameters->width,parameters->height);

    if (parameters->noTimeAnnotation)
        comboBoxTime->setCurrentIndex(1);
    else if (parameters->timeAtEdges)
        comboBoxTime->setCurrentIndex(2);

}



void MainWindow::save()
{

    QString filename = QFileDialog::getSaveFileName(this, tr("Select file to save"), QDir::currentPath(),
                       tr("Document files (*.jpg *.pdf *.png *.bmp *.riff *.xbm);;All files (*)"));
    bool status = true;

    if ( !filename.isNull() )
    {

        QFileInfo fileinfo(filename);
        const char* suffix;

        static QByteArray ba = fileinfo.suffix().toLocal8Bit();
        suffix = ba.constData();

        if (fileinfo.baseName() == "")
        {
            if (parameters->isreconciled)
                fileinfo = QFileInfo(genetree);
            else
                fileinfo = QFileInfo(reconciledtree);
        }


        if (!QImageWriter::supportedImageFormats().contains(QByteArray(suffix)))
        {
            suffix = "pdf";
            parameters->format = suffix;
            string fileRelname = fileinfo.absoluteFilePath().toUtf8().constData();
            parameters->outfile = fileRelname;
            status = widget->saveCanvasPDF(fileinfo.absoluteFilePath() + "." + suffix);
        }
        else
        {
            status = widget->saveCanvas(fileinfo.absoluteFilePath() + "." + suffix,suffix,QUALITY);
        }

        if (status)
        {
            statusBar()->showMessage(tr("File Saved"));
        }
        else
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage(tr("Error saving file"));
            errorMessage.exec();
            status = false;
        }

    }
    else
    {
        statusBar()->showMessage(tr("File not loaded"));
    }

    if (status)
    {
        actionSave->setEnabled(false);
        pushButtonMoveDown->setEnabled(false);
        pushButtonMoveLeft->setEnabled(false);
        pushButtonMoveRight->setEnabled(false);
        pushButtonMoveUp->setEnabled(false);
        pushButtonZoomOut->setEnabled(false);
        pushButtonZoomIn->setEnabled(false);
    }
}

void MainWindow::loadMap()
{
    QString temp = openFile(tr("Open Map File"));

    char* text = new char[temp.length() + 1];
    strcpy(text, temp.toLatin1().constData());

    mapfile = text;

    if (strcmp(mapfile,"") != 0)
    {
        std::stringstream ss;
        ss << "Map File : LOADED " << text;
        statusBar()->showMessage(tr(ss.str().c_str()));
        mapfileStatus = true;
    }
    else
    {
        statusBar()->showMessage(tr("File not loaded"));
    }
}

QString MainWindow::openFile(QString header)
{
    if (lastVisitedDir.isEmpty()) {
        lastVisitedDir = QDir::currentPath();
    }
    QString filename = QFileDialog::getOpenFileName( this, header, lastVisitedDir,
                       tr("All files (*);;Document files (*.txt *.nhx *.tree)"), 0, QFileDialog::DontUseNativeDialog );

    if ( !filename.isNull() )
    {
        lastVisitedDir = QFileInfo(filename).dir().path();
        return filename;
    }
    else
        return "";
}

void MainWindow::showParameters()
{
    menuparameters = !menuparameters;

    if (menuparameters)
    {
        params->setVisible(true);

    }
    else
    {
        params->setVisible(false);
    }

}

void MainWindow::activateLGT()
{
    parameters->lattransfer = checkBoxLGT->isChecked();
    parameters->lateralmincost = (float)spinBoxMinCost->value();
    parameters->lateralmaxcost = (float)spinBoxMaxCost->value();
    parameters->lateralduplicost = (float)spinBoxDupliCost->value();
    parameters->lateraltrancost = (float)spinBoxSpeCost->value();

    if (checkBoxLGT->isChecked() && hostTree && guestTree && isPainted)
    {
	ops->lateralTransfer(mapfile,(parameters->lateralmincost == 1.0 && parameters->lateralmaxcost == 1.0) );
	ops->CalculateGamma();
	paintTree();
    }

}

void MainWindow::activateReconcilation()
{
    parameters->isreconciled = checkBoxReconcile->isChecked();
    guestTree = false;
    hostTree = false;
    menuparameters = false;
    mapfile = false;
    if (isPainted)
    {
        widget->resize(parameters->width,parameters->height);
        widget->paintCanvas();
    }
    isPainted = false;
    actionLoad_Map_File->setEnabled(checkBoxReconcile->isChecked());
    actionSave->setEnabled(false);
    repaint();
    statusBar()->showMessage(tr("New tree"));

}

void MainWindow::print()
{
    if (isPainted)
    {
        bool status = widget->print();
        if (!status)
        {
	    statusBar()->showMessage(tr("Error Printing"));

        }
        else
            statusBar()->showMessage(tr("Tree Printed"));
    }
    else
    {
	statusBar()->showMessage(tr("Tree Printed"));
    }

}

void MainWindow::loadFontColor()
{
    QColorDialog *colordialog = new QColorDialog();
    QColor color = colordialog->getColor();

    if (QObject::sender() == speciesColor)
    {
        parameters->speciesFontColor.blue = color.blueF();
        parameters->speciesFontColor.red = color.redF();
        parameters->speciesFontColor.green = color.greenF();
    }
    else if (QObject::sender() == allColor)
    {
        parameters->allFontColor.blue = color.blueF();
        parameters->allFontColor.red = color.redF();
        parameters->allFontColor.green = color.greenF();
    }
    else
    {
        parameters->geneFontColor.blue = color.blueF();
        parameters->geneFontColor.red = color.redF();
        parameters->geneFontColor.green = color.greenF();

    }
    update();
}

void MainWindow::loadConfigFile()
{
    QString filename = openFile(tr("Load config file"));

    if (!filename.isNull())
    {

        try
        {
            config = new ConfigFile(filename.toStdString());
	    
	    Parameters *parameters = new Parameters();
	    
            parameters->gene_font = config->read<string>((string)"genefont",(string)"Times");
            parameters->species_font = config->read<string>((string)"speciefont",(string)"Times");
	    parameters->all_font = config->read<string>((string)"allfont",(string)"Times");
	    parameters->fontsize = config->read<float>((string)"fontsize",(float)10.0);
	    parameters->gene_font_size = config->read<float>((string)"genefontsize",(float)10.0);
	    parameters->species_font_size = config->read<float>((string)"speciesfontsize",(float)10.0);
            parameters->fontscale = config->read<float>((string)"fontscale",(float)1);
            parameters->markers = config->read<bool>((string)"mark",false);
            parameters->ladd = config->read<char>((string)"ladderize",'n');
            parameters->isreconciled = config->read<bool>((string)"reconcile",false);
            parameters->do_not_draw_guest_tree = config->read<bool>((string)"noguest",false);
            parameters->horiz = config->read<bool>((string)"vertical",true);
            parameters->header = config->read<bool>((string)"header",false);
            parameters->legend = config->read<bool>((string)"legend",false);
            parameters->title = config->read<bool>((string)"text",false);
            parameters->lattransfer = config->read<bool>((string)"lgt",false);
            parameters->do_not_draw_species_tree = config->read<bool>((string)"nohost",false);
            parameters->ids_on_inner_nodes = config->read<bool>((string)"inodes",false);
            string color;
            color = config->read<string>((string)"color",(string)"1");
            parameters->colorConfig->setColors(color.c_str());
            parameters->titleText = config->read<string>((string)"titleText");
            parameters->height = config->read<float>((string)"sizeW",(float)1200);
            parameters->width = config->read<float>((string)"sizeH",(float)1400);
            parameters->lateralmaxcost = config->read<float>((string)"lateralMax",(float)1);
            parameters->lateralduplicost = config->read<float>((string)"lateralDupli",(float)1);
            parameters->lateraltrancost = config->read<float>((string)"lateralLGT",(float)2);
            parameters->lateralmincost = config->read<float>((string)"lateralMin",(float)1);
            parameters->noTimeAnnotation = config->read<bool>((string)"notime",false);
            parameters->timeAtEdges = config->read<bool>((string)"timeex",false);
            parameters->speciesFontColor.blue = config->read<double>((string)"speciesfontcolorB",0.0);
            parameters->speciesFontColor.green = config->read<double>((string)"speciesfontcolorG",0.0);
            parameters->speciesFontColor.red = config->read<double>((string)"speciesfontcolorR",0.0);
            parameters->geneFontColor.blue = config->read<double>((string)"genefontcolorB",0.0);
            parameters->geneFontColor.green =  config->read<double>((string)"genefontcolorG",0.0);
            parameters->geneFontColor.red = config->read<double>((string)"genefontcolorR",0.0);
	    parameters->allFontColor.blue = config->read<double>((string)"allfontcolorB",0.0);
            parameters->allFontColor.green =  config->read<double>((string)"allfontcolorG",0.0);
            parameters->allFontColor.red = config->read<double>((string)"allfontcolorR",0.0);

            statusBar()->showMessage(tr("Configuration Loaded"));
	    loadParameters(parameters);
	    delete(this->parameters);
	    this->parameters = parameters;
        }
        catch (std::exception& e)
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage(tr(e.what()));
            errorMessage.exec();
        }


    }
    else
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage(tr("Error loading file"));
        errorMessage.exec();
    }
}

void MainWindow::saveConfigFile()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Select file to save"), QDir::currentPath(),
                       tr("Document files (*.cfg *.ini *.txt );;All files (*.*)"));

    if ( !filename.isNull() )
    {
        try
        {
            std::fstream out;
            out.open(filename.toStdString().c_str(),ios::out);

	    //            out << "reconcile" << " = " << parameters->isreconciled << endl;
            out << "genefont" << " = " << parameters->gene_font << endl;
            out << "speciefont" << " = " << parameters->species_font << endl;
	    out << "allfont" << " = " << parameters->all_font << endl;
	    out << "fontsize" << " = " << parameters->fontsize << endl;
	    out << "genefontsize" << " = " << parameters->gene_font_size << endl;
	    out << "speciesfontsize" << " = " << parameters->species_font_size << endl;
            out << "fontscale" << " = " << parameters->fontscale << endl;
            out << "mark" << " = " << parameters->markers << endl;
            out << "ladderize" << " = " << parameters->ladd << endl;
            out << "noguest" << " = " << parameters->do_not_draw_guest_tree << endl;
            out << "vertical" << " = " << parameters->horiz << endl;
            out << "header" << " = " << parameters->header << endl;
            out << "legend" << " = " << parameters->legend << endl;
            out << "text" << " = " << parameters->title << endl;
            out << "lgt" << " = " << parameters->lattransfer << endl;
            out << "lateralMax" << " = " << parameters->lateralmaxcost << endl;
            out << "lateralDupli" << " = " << parameters->lateralduplicost << endl;
            out << "lateralLGT" << " = " << parameters->lateraltrancost << endl;
            out << "lateralMin" << " = " << parameters->lateralmincost << endl;
            out << "nohost" << " = " << parameters->do_not_draw_species_tree << endl;
            out << "inodes" << " = " << parameters->ids_on_inner_nodes << endl;
            out << "color" << " = " << parameters->colorConfig->getSet() << endl;
            out << "titleText" << " = " << parameters->titleText << endl;
            out << "sizeW" << " = " << parameters->width << endl;
            out << "sizeH" << " = " << parameters->height << endl;
            out << "notime" << " = " << parameters->noTimeAnnotation << endl;
            out << "timeex" << " = " << parameters->timeAtEdges << endl;
            out << "speciesfontcolorB" << " = " << parameters->speciesFontColor.blue << endl;
            out << "speciesfontcolorG" << " = " << parameters->speciesFontColor.green << endl;
            out << "speciesfontcolorR " << " = " << parameters->speciesFontColor.red << endl;
            out << "genefontcolorB" << " = " << parameters->geneFontColor.blue << endl;
            out << "genefontcolorG " << " = " << parameters->geneFontColor.green << endl;
            out << "genefontcolorR " << " = " << parameters->geneFontColor.red << endl;
	    out << "allfontcolorB" << " = " << parameters->geneFontColor.blue << endl;
            out << "allfontcolorG " << " = " << parameters->geneFontColor.green << endl;
            out << "allfontcolorR " << " = " << parameters->geneFontColor.red << endl;

            out.close();
            statusBar()->showMessage(tr("Configuration saved"));
        }
        catch (std::exception& e)
        {
            QErrorMessage errorMessage;
            errorMessage.showMessage(tr(e.what()));
            errorMessage.exec();
        }
    }
    else
    {
        QErrorMessage errorMessage;
        errorMessage.showMessage(tr("Error loading file"));
        errorMessage.exec();
    }

}

void MainWindow::createActions()
{
    connect(actionLoad_Guest_Tree, SIGNAL(triggered(bool)), this, SLOT(loadGuest()));
    connect(actionLoad_Host_Tree, SIGNAL(triggered(bool)), this, SLOT(loadHost()));
    connect(actionGenerate_Tree, SIGNAL(triggered(bool)), this, SLOT(generateTree()));
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(exit()));
    connect(actionNewImage, SIGNAL(triggered(bool)), this, SLOT(newImage()));
    connect(actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(actionLoad_Map_File, SIGNAL(triggered(bool)), this, SLOT(loadMap()));
    connect(actionShow_Parameters, SIGNAL(triggered(bool)), this, SLOT(showParameters()));
    connect(actionPrint, SIGNAL(triggered(bool)), this, SLOT(print()));
    connect(checkBoxHV, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxGuest, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxHeader, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxHost, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxINodes, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxLGT, SIGNAL(stateChanged(int)), this, SLOT(activateLGT()));
    connect(checkBoxLadderize, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxLegend, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxLogo, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxMarkers, SIGNAL(stateChanged(int)), this, SLOT(update()));
    connect(checkBoxReconcile, SIGNAL(stateChanged(int)), this, SLOT(activateReconcilation()));
    connect(comboBoxTime, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(doubleSpinBoxTextScale, SIGNAL(valueChanged(double)), this, SLOT(update()));
    connect(fontComboBoxGene, SIGNAL(currentFontChanged(QFont)), this, SLOT(update()));
    connect(fontComboBoxSpecies, SIGNAL(currentFontChanged(QFont)), this, SLOT(update()));
    connect(fontComboBoxAll, SIGNAL(currentFontChanged(QFont)), this, SLOT(update()));
    connect(lineEditHeaderText, SIGNAL(textEdited(QString)), this, SLOT(update()));
    connect(pushButtonMoveDown, SIGNAL(clicked()), this, SLOT(update()));
    connect(pushButtonMoveLeft, SIGNAL(clicked()), this, SLOT(update()));
    connect(pushButtonMoveRight, SIGNAL(clicked()), this, SLOT(update()));
    connect(pushButtonMoveUp, SIGNAL(clicked()), this, SLOT(update()));
    connect(pushButtonZoomIn, SIGNAL(clicked()), this, SLOT(update()));
    connect(pushButtonZoomOut, SIGNAL(clicked()), this, SLOT(update()));
    connect(radioButtonColor1, SIGNAL(toggled(bool)), this, SLOT(update()));
    connect(radioButtonColor2, SIGNAL(toggled(bool)), this, SLOT(update()));
    connect(radioButtonColor3, SIGNAL(toggled(bool)), this, SLOT(update()));
    connect(radioButtonColor4, SIGNAL(toggled(bool)), this, SLOT(update()));
    connect(spinBoxDupliCost, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxHorizontalSize, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxMaxCost, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxMinCost, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxSpeCost, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxVerticalSize, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxSpeciesFontSize, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxGeneFontSize, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(spinBoxAllFontSize, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(speciesColor, SIGNAL(clicked(bool)), this, SLOT(loadFontColor()));
    connect(geneColor, SIGNAL(clicked(bool)), this, SLOT(loadFontColor()));
    connect(allColor, SIGNAL(clicked(bool)), this, SLOT(loadFontColor()));
    connect(actionLoad_configuration, SIGNAL(triggered(bool)), this, SLOT(loadConfigFile()));
    connect(actionSave_configuration, SIGNAL(triggered(bool)), this, SLOT(saveConfigFile()));
}



void MainWindow::resizeEvent(QResizeEvent* event)
{

}

void MainWindow::paintEvent(QPaintEvent* event)
{

}
