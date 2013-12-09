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


#include <QDebug>
#include <QPainter>
#include <QResizeEvent>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintEngine>
#include <QtPrintSupport/QPrintDialog>
#include "Canvas.h"

Canvas::Canvas( const QPixmap& pixmap,
                QGraphicsItem* parent, QGraphicsScene* scene ) :
    QObject(), QGraphicsPixmapItem ( pixmap,parent )
{
    Q_UNUSED(scene);

    setTransformationMode(Qt::SmoothTransformation);
    setFlags(QGraphicsItem::ItemIsMovable);
}

//destroys the cairo object if it exists
Canvas::~Canvas()
{
}

bool Canvas::saveCanvas(const QString& fileName, const char* format, unsigned quality)
{
    return pixmap().save(fileName,format,quality);
}

// This function creates the printer object to capture the filename
// and then print the Cairo Canvas on the pdf page that will
// have the same size
bool Canvas::saveCanvasPDF(const QString& fileName)
{
    return pixmap().save(fileName);
}

// This function launches a printing dialog to select the printing device
// and then send the canvas to the printer to be printed out
bool Canvas::print()
{
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setResolution(150);
    printer.setFullPage(true);
    QPrintDialog print(&printer);
    if(print.exec()== QPrintDialog::Accepted)
    {
        QPainter painter;
        painter.begin(&printer);
        painter.drawPixmap(0, 0, pixmap());
        painter.end();
        return true;
    }
    else
    {
        return false;
    }
}

QRectF Canvas::boundingRect() const
{
    return QRectF ( QPointF ( 0,0 ),QSizeF(pixmap().width(),pixmap().height()));
}

void Canvas::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap ( boundingRect().toRect(), pixmap() );
    //QGraphicsPixmapItem::paint(painter, option, widget);
}

void Canvas::setPixmap(const QPixmap& pixmap)
{
    QGraphicsPixmapItem::setPixmap(pixmap);
}

void Canvas::rotateLeftCentered()
{
    QTransform temp = transform();
    resetTransform();
    //     translate ( pixmap().width() /2,pixmap().height() /2 );
    //     rotate ( -10 );
    //     translate ( - ( pixmap().width() /2 ),- ( pixmap().height() /2 ) );
    setTransform(temp,true);
}

void Canvas::rotateRightCentered()
{
    QTransform temp = transform();
    resetTransform();
    //     translate ( pixmap().width() / 2, pixmap().height() /2 );
    //     rotate ( 10 );
    //     translate ( - ( pixmap().width() / 2 ), - ( pixmap().height() / 2 ) );
    setTransform(temp,true);
}

void Canvas::invert()
{
    QTransform temp = transform();
    resetTransform();
    //     translate ( pixmap().width() /2,pixmap().height() /2 );
    //     scale ( 1, -1 );
    //     translate ( - ( pixmap().width() /2 ),- ( pixmap().height() /2 ) );
    setTransform(temp,true);
}

void Canvas::setSize ( unsigned sizeW, unsigned sizeH )
{
    prepareGeometryChange();
    QPixmap scaledP = pixmap().scaled(sizeW,sizeH,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    setPixmap(scaledP);
    update();
}

void Canvas::setVisible ( bool status)
{
    setVisible(status);
}

