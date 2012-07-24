#include <string>
#include <exception>

#ifndef ANERROR_HH
#define ANERROR_HH

//namespace beep
//{

  //! \name
  //! The following macro let you get file and lineno info in 
  //! the error object!
  //!
  //! I have no idea why it has to be so complicated. I stole the macro 
  //! structure from comp.lang.c++.moderated after several failed attempts
  //!  on my own. Actually, I am still not sure this does what I want. /arve
  //! Author: Lars Arvestad, © the MCMC-club, SBC, all rights reserved
  //-----------------------------------------------------------------------
  //@{
#define __BUG_IMPL_2__(str,l) AnError(str " (" __FILE__ ":" #l ", 1)")
#define __BUG_IMPL_1__(str,l) __BUG_IMPL_2__(str,l)
#define PROGRAMMING_ERROR(str) __BUG_IMPL_1__(str, __LINE__)
#define WARNING1(arg) {std::cerr << "Warning: " << arg << std::endl;}
#define WARNING2(arg1,arg2) {std::cerr << "Warning: " << arg1 << arg2 << std::endl;}
#define WARNING3(arg1,arg2,arg3) {std::cerr << "Warning: " << arg1 << arg2 << arg3 << std::endl;}
  //@}

  //! Handle general errors throught this interface.
  //! Go ahead and sub-class this for better error handling!
  //!
  //! The class name is chosen a little bit cryptic to avoid name 
  //! collisions since "Error" seems a little bit too generic.
  //!
  //! Notice that if you want the application to exit upon executing action(),
  //! assumed to be executed in a catch-statement, you have to set a non-zero
  //! error code! Without error code, the error is treated as a warning and the
  //! expected behaviour is that the application gracefully handles the problem.
  //!
  //! \par Example of usage:
  //! \verbatim
  //! try
  //!   {
  //!     // a lot of code
  //!     if(<something is wrong>)
  //!       {
  //! 	throw AnError("Your error message", 1); 
  //! 	// change 1 to preferred error code, default is 0
  //!       }
  //!     // a lot of code
  //!   }
  //!  catch(AnError e)
  //!    {
  //!      e.action();// This will print the error message and exit 
  //!                 // with error code 1
  //!    }
  //! \endverbatim
  class AnError : public std::exception 
  {
  
  public:
    //----------------------------------------------------------------
    //
    // Construct/destruct/assign
    //
    //----------------------------------------------------------------
    //! Construct with error code
    //! \param message    A descriptive string
    //! \param err_code   Error code, default is zero. A positive code will make
    //!                   action() call exit with that error code.
    AnError(const std::string& message, int err_code = 0) throw();

    //! Construct with default error code=0
    AnError(const std::string &message, const std::string &arg, 
	    int err_code = 0) throw();
    ~AnError() throw();

    //----------------------------------------------------------------
    //
    // Interface
    //
    //----------------------------------------------------------------

    //! \name Error handling
    //
    //! \todo{ These three functions below should probably also have exception
    //! specification throw(). Check! /bens}
    //@{

    //! Output error message and decide whether to exit or not.
    virtual void action();	


    //! Just the error message please, no action:
    std::string message() const;

  
    //! The error code
    int code() const;	
    //@}

    const char* what() const throw();

  protected:
    //----------------------------------------------------------------
    //
    // Attributes
    //
    //----------------------------------------------------------------
    const std::string msg_str;
    std::string arg_str;
    int error_code;
  };

//}//end namespace beep

#endif
