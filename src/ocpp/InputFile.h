/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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

#ifndef InputFile_h
#define InputFile_h

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstdio>
#include "ppDefine.h"

class InputFile
{
    enum
    {
        LINE_WIDTH = 4096
    };

  public:
    InputFile(bool fullname, const std::string& Name) :
        name(Name),
        lineno(0),
        errlineno(0),
        errname(GetErrorName(fullname, name)),
        inComment(false),
        commentLine(0),
        endedWithoutEOL(false),
        file(nullptr),
        utf8BOM(false)
    {
    }
    virtual ~InputFile()
    {
        if (file && file != stdin)
            fclose(file);
        CheckErrors();
    }
    virtual bool Open()
    {
        if (name[0] == '-')
            file = stdin;
        else
            file = fopen(name.c_str(), "r");
        CheckUTF8BOM();
        return file != nullptr;
    }
    int GetLineNo() { return lineno; }
    int GetErrorLine() { return errlineno; }
    std::string GetErrorFile() { return errname; }
    void CheckErrors();
    virtual bool GetLine(std::string& line);
    void SetErrlineInfo(std::string& name, int line)
    {
        errname = name;
        lineno = line;
    }

  protected:
    std::string GetErrorName(bool full, std::string& name);
    virtual int StripComment(char* line) { return strlen(line); }
    bool ReadLine(char* line);
    void CheckUTF8BOM();

  protected:
    bool inComment;
    int commentLine;
    int lineno;
    int errlineno;
    bool utf8BOM; // just cache whether it exists, we don't actually use it at this point though.
                  // input files are assumed to be UTF8 anyway...

  private:
    FILE* file;
    std::string name;
    std::string errname;
    bool endedWithoutEOL;
};
#endif