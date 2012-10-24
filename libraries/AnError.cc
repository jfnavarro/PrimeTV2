#include <iostream>
#include <cstdlib>
#include <csignal>
#include "AnError.hh"

// Author: Lars Arvestad, © the MCMC-club, SBC, all rights reserved

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
	s = s + " ("
	  + arg_str
	  + ")";
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