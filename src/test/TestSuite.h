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

#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <QObject>

namespace unit
{

    class TestSuite;
    class TestHandle
    {
    public:
        TestHandle();
        TestHandle(TestSuite *suite, const QString &name);

        TestHandle dependsOn(const QString &name);

    private:
        const QString m_name;
        TestSuite *m_suite;
    };

    class TestSuite
    {
        
    public:
        TestSuite();

        TestHandle addTest(QObject *test, const QString &name);

        int exec();

    private:
        friend class TestHandle;

        void setDependencie(const QString &parent, const QString &child);

        QObject m_root;
    };

} // namespace unit //

#endif // TESTSUITE_H
