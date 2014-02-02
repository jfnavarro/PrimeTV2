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


#include "Edge.h"

#include "../tree/Node.h"

#include <iostream>
#include <boost/concept_check.hpp>


Edge::Edge()
    :speOrigin(0),
      speEnd(0),
      geneOrigin(0),
      geneEnd(0),
      xOrigin(0),
      xEnd(0),
      yOrigin(0),
      yEnd(0),
      mode(Normal)
{

}

Edge::Edge(const Edge& other)
{
    speOrigin = other.speOrigin;
    speEnd = other.speEnd;
    geneOrigin = other.geneOrigin;
    geneEnd = other.geneEnd;
    xOrigin = other.xOrigin;
    xEnd = other.xEnd;
    yOrigin = other.yOrigin;
    yEnd = other.yEnd;
    mode = other.mode;
}

Edge::~Edge()
{

}

Edge& Edge::operator=(const Edge& other)
{
    speOrigin = other.speOrigin;
    speEnd = other.speEnd;
    geneOrigin = other.geneOrigin;
    geneEnd = other.geneEnd;
    xOrigin = other.xOrigin;
    xEnd = other.xEnd;
    yOrigin = other.yOrigin;
    yEnd = other.yEnd;
    mode = other.mode;
    return *this;
}

bool Edge::operator==(const Edge& other) const
{
    if(this->speOrigin != other.speOrigin || this->speEnd != other.speEnd || this->geneOrigin != other.geneOrigin
            || this->geneEnd != other.geneEnd || this->xOrigin != other.xOrigin || this->xEnd != other.xEnd
            || this->yOrigin != other.yOrigin || this->yEnd != other.yEnd || this->mode != other.mode)
    {
        return false;
    }
    
    return true;
}

const Node *Edge::getSpeOrigin() const
{
    return speOrigin;
}

const Node *Edge::getSpeEnd() const
{
    return speEnd;
}

const Node *Edge::getGeneOrigin() const
{
    return geneOrigin;
}

const Node *Edge::getGeneEnd() const
{
    return geneEnd;
}

double Edge::getXorigin() const
{
    return xOrigin;
}

double Edge::getYorigin() const
{
    return yOrigin;
}

double Edge::getXend() const
{
    return xEnd;
}

double Edge::getYend() const
{	
    return yEnd;
}

Edge::category Edge::getMode() const
{
    return mode;
}

void Edge::setSpeOrigin(const Node &n)
{
    speOrigin = &n;
}

void Edge::setSpeEnd(const Node &n)
{
    speEnd = &n;
}

void Edge::setGeneOrigin(const Node &n)
{
    geneOrigin = &n;
}

void Edge::setGeneEnd(const Node &n)
{
    geneEnd = &n;
}

void Edge::setXorigin(double x)
{
    xOrigin = x;
}

void Edge::setYorigin(double y)
{
    yOrigin = y;
}

void Edge::setXend(double x)
{
    xEnd = x;
}

void Edge::setYend(double y)
{
    yEnd = y;
}

void Edge::setMode(category c)
{
    mode = c;
}
