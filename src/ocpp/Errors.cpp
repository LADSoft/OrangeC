/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "Errors.h"
#include "ppInclude.h"
#include "Utils.h"

#include <fstream>
#include <iostream>

int Errors::maxErrors = 100;
int Errors::errorCount;
int Errors::warningCount;
ppInclude *Errors::include;
bool Errors::showWarnings = true;
bool Errors::showTrivialWarnings;

void Errors::ErrorWithLine(const std::string &msg, std::string &filname, int lineno)
{
    std::cout << "Error ";
    std::cout << filname.c_str() << "(" << lineno << "): " ;
    std::cout << msg.c_str() << std::endl;
    if (errorCount++ > maxErrors)
    {
        Utils::fatal("Too Many Errors");
    }
}
void Errors::Error(const std::string &msg)
{
    std::cout << "Error ";
    FileName();
    std::cout << msg.c_str() << std::endl;
    if (errorCount++ > maxErrors)
    {
        Utils::fatal("Too Many Errors");
    }
}
void Errors::WarningWithLine(const std::string &msg, std::string &filname, int lineno)
{
    if (showWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        std::cout << filname.c_str() << "(" << lineno << "): " ;
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::Warning(const std::string &msg)
{
    if (showWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        FileName();
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::TrivialWarningWithLine(const std::string &msg, std::string &filname, int lineno)
{
    if (showTrivialWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        std::cout << filname.c_str() << "(" << lineno << "): " ;
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::TrivialWarning(const std::string &msg)
{
    if (showTrivialWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        FileName();
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::Previous(const std::string &name, int lineNo, const std::string &file)
{
    std::cout << "Warning "<< file.c_str() << "(" << lineNo << "): Previous definition here" << std::endl;	
}
bool Errors::ErrorCount()
{
    bool rv = errorCount != 0;
    if (errorCount || warningCount)
    {
        std::cout << errorCount << " Errors, " << warningCount << " Warnings" << std::endl;
    }
    return rv;
}
void Errors::FileName()
{
    if (include)
    {
        std::string filname = include->GetFile();
        std::cout << filname.c_str() << "(" << include->GetLineNo() << "): " ;
    }
}
std::string Errors::ToNum(L_INT num)
{
    return Utils::NumberToString(num);
}
int Errors::GetErrorLine() { return include->GetLineNo(); }
std::string Errors::GetFileName() { return include->GetFile(); }
