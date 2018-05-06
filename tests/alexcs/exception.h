#ifndef COMPILER_EXCEPTION
#define COMPILER_EXCEPTION

#include <exception>
#include <string.h>
#include <string>

class CompilerException: public std::exception{
private:
    char *err;
public:
    CompilerException();
    CompilerException(const std::string& msg);
    ~CompilerException() throw();
    virtual const char* what() const throw();
};

#endif
