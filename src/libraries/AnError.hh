#include <string>
#include <exception>
#ifndef ANERROR_HH
#define ANERROR_HH

using namespace std;

#define __BUG_IMPL_2__(str,l) AnError(str " (" __FILE__ ":" #l ", 1)")
#define __BUG_IMPL_1__(str,l) __BUG_IMPL_2__(str,l)
#define PROGRAMMING_ERROR(str) __BUG_IMPL_1__(str, __LINE__)
#define WARNING1(arg) {std::cerr << "Warning: " << arg << std::endl;}
#define WARNING2(arg1,arg2) {std::cerr << "Warning: " << arg1 << arg2 << std::endl;}
#define WARNING3(arg1,arg2,arg3) {std::cerr << "Warning: " << arg1 << arg2 << arg3 << std::endl;}

  class AnError : public std::exception 
  {
  
  public:

    AnError(const std::string& message, int err_code = 0) throw();
    AnError(const std::string &message, const std::string &arg, int err_code = 0) throw();
    ~AnError() throw();

    virtual void action();	
    std::string message() const;
    int code() const;	
    const char* what() const throw();

  protected:
    const std::string msg_str;
    std::string arg_str;
    int error_code;
  };

#endif