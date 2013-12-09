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

#ifndef BREADTHFIRSTTOPDOWN_H
#define BREADTHFIRSTTOPDOWN_H

#include "linearizer.h"

namespace unit
{

    // BreadthFirstTopDown implements a breadth-first, top-down linearization
    // algorithm that operate on the parent-child tree structure defined by
    // Note: QObject does not keep track of insertion order for children.
    class BreadthFirstTopDown : public Linearizer
    {
    public:
        virtual const QList<QObject *> list(QObject *object);
        BreadthFirstTopDown(){};
        virtual ~BreadthFirstTopDown(){};
    };

} // namespace unit //

#endif // BREADTHFIRSTTOPDOWN_H
