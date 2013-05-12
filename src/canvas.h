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

#ifndef CANVAS_H
#define CANVAS_H
#include <QPixmap>
#include <QByteArray>
#include <cairo/cairo.h>


class Canvas: public QObject, public QPixmap
{
  Q_OBJECT

public:
  
    //constructor, parent object passed as parameter
    Canvas(QWidget* parent = 0);
    
    //generate the Cairo surface and object
    void paintCanvas();
    
    //save the canvas in a file
    bool saveCanvas(const QString & fileName, const char * format = 0, int quality = -1);
    
    //save the canvas in a pdf
    bool saveCanvasPDF(const QString& fileName);
    
    //prin the canvas
    bool print();
    
    //destructor
    ~Canvas();

protected:
  
    //methods inherited from QWidget
    //they are called everytime we draw or resize the Canvas
    virtual void paintEvent(QPaintEvent* );
    virtual void resizeEvent(QResizeEvent* );

};

#endif // CANVAS_H
