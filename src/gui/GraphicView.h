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

#ifndef GRAPHICVIEW_H
#define GRAPHICVIEW_H

#include <QGraphicsView>


class GraphicView : public QGraphicsView
{

    Q_OBJECT

public:

    GraphicView( QWidget* parent = 0);

    virtual ~GraphicView();
    
protected:

    virtual void mousePressEvent(QMouseEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void wheelEvent(QWheelEvent* event);
    
    virtual void resizeEvent(QResizeEvent* event);
    
    //Holds the current centerpoint for the view, used for panning and zooming
    QPointF CurrentCenterPoint;

    //From panning the view
    QPoint LastPanPoint;

    //Set the current centerpoint in the
    void SetCenter(const QPointF& centerPoint);
    QPointF GetCenter() { return CurrentCenterPoint; }
    
public slots:

    void zoomIn() { scale(1.2, 1.2); }
    
    void zoomOut() { scale(1 / 1.2, 1 / 1.2); }
    
    void rotateLeft() { rotate(-10); }

    void rotateRight() { rotate(10); }
    
    void invert() { scale(1, -1); }

    void setBackground(QColor color);

};

#endif // GRAPHICVIEW_H
