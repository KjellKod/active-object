/* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it. The code is highly platform dependent!
*
* Code & platform dependent issues with it was originally
* published at http://www.kjellkod.cc/threadsafecircularqueue
* 2009-11-02
* @author Kjell Hedstr�m, hedstrom@kjellkod.cc */

#ifndef SW_CHECK_H_
#define SW_CHECK_H_

#include <sstream>
#include <iostream>
#include <string>

// simple namespace for unit testing
namespace SwErr
{
   void print(const char* a_file, int a_line, std::string a_expr);
   inline void print(const char* a_file, int a_line, std::string a_expr)
   {
      std::ostringstream oss;
      oss << a_expr.c_str() << "### At file: " << a_file << ", line: " << a_line <<  std::endl;
      std::cout << oss.str().c_str();
   }
}

// simple macro for unit testing
#define SW_CHECK(EXPRESSION)                                        \
   if(false == (EXPRESSION))                                        \
   {                                                                \
     std::ostringstream oss_err;                                    \
     oss_err << "### SW_CHECK(" << #EXPRESSION << ")" << std::endl; \
     SwErr::print(__FILE__, __LINE__, oss_err.str());               \
     return(0);                                                     \
   }

#endif // SW_CHECK_H_
