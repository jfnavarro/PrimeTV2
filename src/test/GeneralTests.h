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

#ifndef GENERALTESTS_H
#define GENERALTESTS_H

#include <QObject>
#include <iostream>
#include <stdio.h>

class QTemporaryFile;
class QString;
namespace unit
{

class GeneralTests : public QObject
{
    Q_OBJECT

public:

    explicit GeneralTests(QObject *parent = 0);

    bool run();

    inline bool exists_test (const std::string& name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }
    }

    void createTempFile(QTemporaryFile &temp_file, const std::string &input, QString &output);

private:

    QString test_species;
    QString test_map;
    QString test_gene;
    QString test_reconciled;
    QString test_precomputed;

private slots:

    void initTestCase();
    void cleanupTestCase();

};

} // namespace unit //

#endif // GENERALTESTS_H
