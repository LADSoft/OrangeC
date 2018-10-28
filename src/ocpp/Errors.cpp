/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "Errors.h"
#include "ppInclude.h"
#include "Utils.h"

#include <fstream>
#include <iostream>

int Errors::maxErrors = 100;
int Errors::errorCount;
int Errors::warningCount;
ppInclude* Errors::include;
bool Errors::showWarnings = true;
bool Errors::showTrivialWarnings;

void Errors::ErrorWithLine(const std::string& msg, std::string& filname, int lineno)
{
    std::cout << "Error ";
    std::cout << filname.c_str() << "(" << lineno << "): ";
    std::cout << msg.c_str() << std::endl;
    if (errorCount++ > maxErrors)
    {
        Utils::fatal("Too Many Errors");
    }
}
void Errors::Error(const std::string& msg)
{
    std::cout << "Error ";
    FileName();
    std::cout << msg.c_str() << std::endl;
    if (errorCount++ > maxErrors)
    {
        Utils::fatal("Too Many Errors");
    }
}
void Errors::WarningWithLine(const std::string& msg, std::string& filname, int lineno)
{
    if (showWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        std::cout << filname.c_str() << "(" << lineno << "): ";
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::Warning(const std::string& msg)
{
    if (showWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        FileName();
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::TrivialWarningWithLine(const std::string& msg, std::string& filname, int lineno)
{
    if (showTrivialWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        std::cout << filname.c_str() << "(" << lineno << "): ";
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::TrivialWarning(const std::string& msg)
{
    if (showTrivialWarnings)
    {
        warningCount++;
        std::cout << "Warning ";
        FileName();
        std::cout << msg.c_str() << std::endl;
    }
}
void Errors::Previous(const std::string& name, int lineNo, const std::string& file)
{
    std::cout << "Warning " << file.c_str() << "(" << lineNo << "): Previous definition here" << std::endl;
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
        std::cout << filname.c_str() << "(" << include->GetLineNo() << "): ";
    }
}
std::string Errors::ToNum(long long num) { return Utils::NumberToString(num); }
int Errors::GetErrorLine() { return include->GetLineNo(); }
std::string Errors::GetFileName() { return include->GetFile(); }
