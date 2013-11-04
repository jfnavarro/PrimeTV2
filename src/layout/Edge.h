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

/* this class store all the information of an edge, node origin and destiny (gene and species) 
 * as well as cordinates origin and destiny. It implements the basics getters and setters
 that don't need to be explained */

#ifndef EDGE_H
#define EDGE_H

class Node;

class Edge
{


public:

    enum category{Normal=0,LGT=1};

    Edge();
    Edge(const Edge& other);
    virtual ~Edge();
    virtual Edge& operator=(const Edge& other);
    virtual bool operator==(const Edge& other) const;
    
    const Node *getSpeOrigin() const;
    const Node *getSpeEnd() const;
    const Node *getGeneOrigin() const;
    const Node *getGeneEnd() const;
    double getXorigin();
    double getYorigin();
    double getXend();
    double getYend();
    category getMode();
    void setSpeOrigin(const Node &);
    void setSpeEnd(const Node &);
    void setGeneOrigin(const Node &);
    void setGeneEnd(const Node &);
    void setXorigin(double x);
    void setYorigin(double y);
    void setXend(double x);
    void setYend(double y);
    void setMode(category c);
    
private:
    
    const Node *speOrigin;
    const Node *speEnd;
    const Node *geneOrigin;
    const Node *geneEnd;
    double xOrigin;
    double xEnd;
    double yOrigin;
    double yEnd;
    category mode;
    
};

#endif // EDGE_H
