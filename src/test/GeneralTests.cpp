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

#include "GeneralTests.h"

#include "../Parameters.h"
#include "../Mainops.h"
#include "../utils/AnError.hh"

#include <QTemporaryFile>
#include <QFile>
#include <QtTest/QTest>
#include <QDebug>

#include "unistd.h"

// these must not go out of scope
static Parameters *parameters = 0;
static Mainops *mainops = 0;

static string test_species_text = "(((((((ANAVT:0.0200000[&&PRIME ID=8],ANASP:0.0200000[&&PRIME ID=7])100:0.260000[&&PRIME ID=9],"
        "TRIEI:0.280000[&&PRIME ID=6])99:0.110000[&&PRIME ID=10],SYNY3:0.390000[&&PRIME ID=5])98:0.110000[&&PRIME ID=11],"
        "SYNE7:0.500000[&&PRIME ID=4])84:0.160000[&&PRIME ID=12],(PROM9:0.380000[&&PRIME ID=13],(SYNS3:0.220000[&&PRIME ID=14],"
        "((SYNSC:0.100000[&&PRIME ID=17],SYNPX:0.100000[&&PRIME ID=16])50:0.0300000[&&PRIME ID=18],SYNS9:0.130000[&&PRIME ID=15])"
        "101:0.0900000[&&PRIME ID=19])105:0.160000[&&PRIME ID=20])102:0.280000[&&PRIME ID=21])103:0.130000[&&PRIME ID=22],"
        "(SYNJB:0.0700000[&&PRIME ID=1],SYNJA:0.0700000[&&PRIME ID=2])104:0.720000[&&PRIME ID=3]):0.210000[&&PRIME ID=23],"
        "GLVIO1:1.00000[&&PRIME ID=0]):1.00000[&&PRIME ID=24][&&PRIME NAME=cyano];";

static string test_gene_text = "((((((((ANASP:1.000000[&&PRIME S=ANASP ],ANAVT:1.000000[&&PRIME S=ANAVT ])"
        "NoName:1.000000,TRIEI:1.000000[&&PRIME S=TRIEI ])NoName:1.000000,SYNY3:1.000000[&&PRIME S=SYNY3 ])"
        "NoName:1.000000,GLVIO1:1.000000[&&PRIME S=GLVIO1 ])NoName:1.000000,(SYNJA:1.000000[&&PRIME S=SYNJA ],"
        "SYNJB:1.000000[&&PRIME S=SYNJB ])NoName:1.000000)NoName:1.000000,SYNE7:1.000000[&&PRIME S=SYNE7 ])"
        "NoName:1.000000,PROM9:1.000000[&&PRIME S=PROM9 ])NoName:1.000000,(((SYNPX:1.000000[&&PRIME S=SYNPX ],"
        "SYNSC:1.000000[&&PRIME S=SYNSC ])NoName:1.000000,SYNS9:1.000000[&&PRIME S=SYNS9 ])"
        "NoName:1.000000,SYNS3:1.000000[&&PRIME S=SYNS3 ])NoName:1.000000);";

static string mapfile_text = "ANAVT ANAVT\n"
        "ANASP ANASP\n"
        "TRIEI TRIEI\n"
        "SYNY3 SYNY3\n"
        "SYNE7 SYNE7\n"
        "PROM9 PROM9\n"
        "SYNS3 SYNS3\n"
        "SYNSC SYNSC\n"
        "SYNPX SYNPX\n"
        "SYNS9 SYNS9\n"
        "SYNJB SYNJB\n"
        "SYNJB SYNJB\n"
        "SYNJA SYNJA\n"
        "GLVIO1 GLVIO1";

static string test_reconciled_text = "((SYNS3[&&PRIME S=SYNS3 AC=(14) ID=22],(SYNS9[&&PRIME S=SYNS9 AC=(15) ID=20],"
        "(SYNSC[&&PRIME S=SYNSC AC=(17) ID=18],SYNPX[&&PRIME S=SYNPX AC=(16) ID=17])"
        "[&&PRIME D=0 AC=(18) ID=19])[&&PRIME D=0 AC=(19) ID=21])[&&PRIME D=0 AC=(20 21 22 23 24) ID=23],"
        "(PROM9[&&PRIME S=PROM9 AC=(13 21 22 23 24) ID=15],(SYNE7[&&PRIME S=SYNE7 AC=(4 12 22 23 24) ID=13],"
        "((SYNJB[&&PRIME S=SYNJB AC=(1) ID=10],SYNJA[&&PRIME S=SYNJA AC=(2) ID=9])[&&PRIME D=0 AC=(3 23 24) ID=11],"
        "(GLVIO1[&&PRIME S=GLVIO1 AC=(0) ID=7],(SYNY3[&&PRIME S=SYNY3 AC=(5) ID=5],(TRIEI[&&PRIME S=TRIEI AC=(6) ID=3],"
        "(ANAVT[&&PRIME S=ANAVT AC=(8) ID=1],ANASP[&&PRIME S=ANASP AC=(7) ID=0])[&&PRIME D=0 AC=(9) ID=2])[&&PRIME D=0 AC=(10) ID=4])"
        "[&&PRIME D=0 AC=(11 12 22 23) ID=6])[&&PRIME D=0 AC=(24) ID=8])[&&PRIME D=1 ID=12])[&&PRIME D=1 ID=14])[&&PRIME D=1 ID=16])[&&PRIME D=1 ID=24];";

static string test_precomputed_text = "precomputed.txt";
bool show_lgt_scenarios = false;
bool load_precomputed_lgt_scenario = false;

namespace unit
{

GeneralTests::GeneralTests(QObject *parent) : QObject(parent) { }

void GeneralTests::initTestCase()
{
    parameters = new Parameters;
    mainops = new Mainops;
    QVERIFY(parameters != 0);
    QVERIFY(mainops != 0);
    mainops->start();
    mainops->setParameters(parameters);

    //create files
    QTemporaryFile temp_file_species;
    createTempFile(temp_file_species, test_species_text, test_species);
    //QFINDTESTDATA(test_species);

    QTemporaryFile temp_file_gene;
    createTempFile(temp_file_gene, test_gene_text, test_gene);
    //QFINDTESTDATA(test_gene);

    QTemporaryFile temp_file_map;
    createTempFile(temp_file_map, mapfile_text, test_map);
    //QFINDTESTDATA(test_map);

    QTemporaryFile temp_file_reconciled;
    createTempFile(temp_file_reconciled, test_reconciled_text, test_reconciled);
    //QFINDTESTDATA(test_reconciled);

    QTemporaryFile temp_file_precomputed;
    createTempFile(temp_file_precomputed, test_precomputed_text, test_precomputed);
    //QFINDTESTDATA(test_precomputed);

    //Test default parameters
    parameters->isreconciled = false;
    QVERIFY2(run() == true,"Default parameters");
    //test reconcilation
    parameters->isreconciled = true;
    QVERIFY2(run() == true, "Reconcile trees");
    //test LGT
    parameters->isreconciled = false;
    parameters->lattransfer = true;
    QVERIFY2(run() == true, "Default parameters with LGT");
    //test draw all LGT
    parameters->isreconciled = false;
    parameters->lattransfer = true;
    parameters->drawAll = true;
    QVERIFY2(run() == true,"Default parameters with LGT and drawing all");
    //test show all LGT
    parameters->isreconciled = false;
    parameters->lattransfer = true;
    parameters->drawAll = false;
    show_lgt_scenarios = true;
    QVERIFY2(run() == true,"Default parameters with LGT and printing out scenarios");
    //test show all LGT
    parameters->isreconciled = false;
    parameters->lattransfer = false;
    parameters->drawAll = false;
    show_lgt_scenarios = false;
    parameters->reduce = true;
    QVERIFY2(run() == true,"Default parameters and reducing crossing lines");
}

void GeneralTests::createTempFile(QTemporaryFile &temp_file, const std::string &input, QString &output)
{
    temp_file.setAutoRemove(false);
    QVERIFY(temp_file.open());
    QTextStream out(&temp_file);
    out << QString::fromStdString(input);
    temp_file.close();
    output = temp_file.fileName();
}

bool GeneralTests::run()
{
    bool retcode = true;
    QTemporaryFile tempfile;
    if(!tempfile.open())
    {
        return false;
    }
    parameters->outfile = tempfile.fileName().toStdString();
    tempfile.remove();
    try
    {
        mainops->cleanTrees();
        if(!(bool)(parameters->isreconciled))
        {
            mainops->OpenReconciled(test_reconciled.toLatin1().data());
            mainops->OpenHost(test_species.toLatin1().data());
        }
        else
        {
            mainops->reconcileTrees(test_gene.toLatin1().data(),test_species.toLatin1().data(),test_map.toLatin1().data());
        }

        //we calculate the LGT scenarios is indicated
        if((bool)(parameters->lattransfer))
        {
            mainops->lateralTransfer(test_map.toLatin1().data(),(parameters->lateralmincost == 1.0 && parameters->lateralmaxcost == 1.0));
        }
        else if(load_precomputed_lgt_scenario)
        {
            mainops->loadPreComputedScenario(test_precomputed.toLatin1().data(),test_map.toLatin1().data());
        }

        if(parameters->drawAll)
        {
            mainops->drawAllLGT();
        }
        else
        {
            mainops->drawBest();
        }

        if(show_lgt_scenarios)
        {
            mainops->printLGT();
        }

        //if(!exists_test(parameters->outfile + "." + parameters->format))
        //{
            //retcode = false;
        //}
    }
    catch (const AnError &e)
    {
        qDebug() << "There was an error" << e.what();
        retcode = false;
    }
    catch (const std::exception& e)
    {
        qDebug() << "There was an error" << e.what();
        retcode = false;
    }
    catch(...)
    {
        qDebug() << "There was an unknown error";
        retcode = false;
    }

    return retcode;

}

void GeneralTests::cleanupTestCase()
{
    delete parameters;
    delete mainops;
}


} // namespace unit //

