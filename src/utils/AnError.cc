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
             Lars Arvestad, © the MCMC-club, SBC, all rights reserved
 */

#include <iostream>
#include <cstdlib>
#include <csignal>
#include "AnError.hh"

using namespace std;

AnError::AnError(const string &message, int err_code) throw()
: msg_str(message),
    arg_str(""),
    error_code(err_code)
{

}

AnError::AnError(const string &message, const string &arg, 
        int err_code) throw()
: msg_str(message),
    arg_str(arg),
    error_code(err_code)
{
}

AnError::~AnError() throw()
{
}

void
AnError::action()
{
    cerr << "Error:\n" << message();
    cerr << endl;

    if (error_code >= 1)
    {
        exit(error_code);
    }
}

std::string
AnError::message() const
{
    std::string s =  msg_str;
    if (arg_str.length() > 0) 
    {
        s = s + " (" + arg_str + ")";
    }
    return s;
}

int
AnError::code() const
{
    return error_code;
}

const char*
AnError::what() const throw()
{
    return message().c_str();
}
