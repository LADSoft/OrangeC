#include "exception.h"

CompilerException::CompilerException():
    err(NULL)
{
}

CompilerException::CompilerException(const std::string& msg):
    err(strcpy(new char[msg.size() + 1], msg.c_str()))
{
}

const char* CompilerException::what() const throw()
{
    return err;
}

CompilerException::~CompilerException() throw()
{
    if (err != NULL) delete err;
}
