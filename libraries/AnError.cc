#include <iostream>
#include <cstdlib>
#include <csignal>

#include "AnError.hh"

// Author: Lars Arvestad, © the MCMC-club, SBC, all rights reserved
  using namespace beep;
  using namespace std;


  //----------------------------------------------------------------
  //
  // Construct/destruct/assign
  //
  //----------------------------------------------------------------

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

  //----------------------------------------------------------------
  //
  // Interface
  //
  //----------------------------------------------------------------

  // Output the error message and exit if the error code is positive
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


  // \return The error message, without even considering to exit.
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


  // The supplied error code.
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

