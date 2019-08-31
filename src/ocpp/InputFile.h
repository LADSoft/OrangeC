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

class PipeArbitrator;

class InputFile
{
    enum
    {
        LINE_WIDTH = 4096
    };

  public:
    InputFile(bool fullname, const std::string& Name, PipeArbitrator &Piper) :
        name(Name),
        lineno(0),
        errlineno(0),
        errname(GetErrorName(fullname, name)),
        inComment(false),
        commentLine(0),
        endedWithoutEOL(false),
        fileno(-1),
        utf8BOM(false),
        fileIndex(0),
        inputLen(0),
        bufPtr(inputBuffer),
        piper(Piper)
    {
    }
    virtual ~InputFile();
    virtual bool Open();
    bool IsOpen() const { return fileno >= 0; }
    int GetLineNo() { return lineno; }
    int GetErrorLine() { return errlineno; }
    const std::string& GetErrorFile() { return errname; }
    int GetRealLine() { return lineno; }
    const std::string& GetRealFile() { return name; }
    void CheckErrors();
    virtual bool GetLine(std::string& line);
    void SetErrlineInfo(std::string& name, int line)
    {
        errname = name;
        errlineno = line;
    }
    int GetIndex() const { return fileIndex; }
    void SetIndex(int index) { fileIndex = index; }

  protected:
    std::string GetErrorName(bool full, const std::string& name);
    virtual int StripComment(char* line) { return strlen(line); }
    bool ReadLine(char* line);
    void CheckUTF8BOM();
    bool ReadString(char *line, int width);


  protected:
    bool inComment;
    int commentLine;
    int lineno;
    int errlineno;
    bool utf8BOM;  // just cache whether it exists, we don't actually use it at this point though.
                   // input files are assumed to be UTF8 anyway...

  private:
    int inputLen;
    char inputBuffer[32000];
    char *bufPtr;
    int fileno;
    const std::string& name;
    std::string errname;
    bool endedWithoutEOL;
    int fileIndex;
    PipeArbitrator& piper;
};
#endif