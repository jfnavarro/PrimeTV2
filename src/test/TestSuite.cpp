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

#include <QtTest/QTest>

#include "Breadthfirsttopdown.h"

#include "TestSuite.h"

namespace unit
{

    //TestHandle::TestHandle() : m_suite(0), m_name() { }
    TestHandle::TestHandle(TestSuite *suite, const QString &name)
        : m_name(name), m_suite(suite)
    { }

    TestHandle TestHandle::dependsOn(const QString &name)
    {
        // <name> must execute before <m_name>
        m_suite->setDependencie(name, m_name);
        return (*this);
    }

    TestSuite::TestSuite() { }

    TestHandle TestSuite::addTest(QObject *test, const QString &name)
    {
        // assign name to object and add it without dependencies (to root)
        test->setObjectName(name);
        test->setParent(&m_root);

        return TestHandle(this, name);
    }

    unsigned TestSuite::exec()
    {
        QScopedPointer<Linearizer> linearizer(new BreadthFirstTopDown());

        QList<QObject *> list = linearizer->list(&m_root);
        list.removeFirst(); // pop dummy root object

        unsigned exitCode = 0;
        foreach (QObject *object, list)
        {
            if (QTest::qExec(object))
            {
                exitCode = -1;
            }
        }

        return exitCode;
    }

    void TestSuite::setDependencie(const QString &parent, const QString &child)
    {
        QObject *parentObject = m_root.findChild<QObject *>(parent, Qt::FindChildrenRecursively);
        QObject *childObject = m_root.findChild<QObject *>(child, Qt::FindChildrenRecursively);

        // abort if either is null
        if (!parentObject || !childObject)
        {
            return;
        }

        childObject->setParent(parentObject);
    }

} // namespace unit //
