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

#include <QLinkedList>

#include "Breadthfirsttopdown.h"

namespace unit
{

    const QList<QObject *> BreadthFirstTopDown::list(QObject *object)
    {
        QList<QObject *> list;

        // add root layer
        if (object)
        {
            list.append(object);
        }

        // go through list and append children to the back
        for (int i = 0; i < list.size(); ++i)
        {
            // append current layer
            list.append(list[i]->children());
        }
        return list;
    }

} // namespace unit //
