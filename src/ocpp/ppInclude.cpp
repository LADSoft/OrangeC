/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#define _CRT_SECURE_NO_WARNINGS

#include "ppInclude.h"
#include "PreProcessor.h"
#include "ppkw.h"
#include "Errors.h"
#include "CmdFiles.h"
#include <climits>
#include <fstream>
#include <iostream>

bool ppInclude::system;
std::string ppInclude::srchPath, ppInclude::sysSrchPath;

char ppInclude::commentChar = ';';

ppInclude::~ppInclude() {}

bool ppInclude::Check(kw token, const std::string& args)
{
    if (!current || !current->Check(token, args, current->GetErrorLine()))
        if (!CheckInclude(token, args))
            if (!CheckLine(token, args))
                return false;
    return true;
}

bool ppInclude::CheckInclude(kw token, const std::string& args)
{
    if (token == kw::INCLUDE)
    {
        std::string line1 = args;
        int npos = line1.find_first_not_of(" \t\v");
        if (npos != std::string::npos && line1[npos] != '"' && line1[npos] != '<')
            define->Process(line1);
        bool specifiedAsSystem = false;
        std::string name = ParseName(line1, specifiedAsSystem);
        name = FindFile(specifiedAsSystem, name);
        pushFile(name, line1);
        return true;
    }
    return false;
}
bool ppInclude::has_include(const std::string& args)
{
    std::string line1 = args;
    bool specifiedAsSystem = false;
    std::string name = ParseName(line1, specifiedAsSystem);
    name = FindFile(specifiedAsSystem, name);
    return !name.empty();
}
bool ppInclude::CheckLine(kw token, const std::string& args)
{
    if (token == kw::LINE)
    {
        std::string line1 = args;
        define->Process(line1);
        int n = expr.Eval(line1);
        std::string file;
        int npos = line1.find_first_not_of(" \r\v\t\n");
        if (npos < line1.size())
        {
            bool specifiedAsSystem = false;
            file = ParseName(line1.substr(npos), specifiedAsSystem);
        }
        current->SetErrlineInfo(file, n - 1);
        return true;
    }
    return false;
}
void ppInclude::pushFile(const std::string& name, const std::string& errname)
{
    // gotta do the test first to get the error correct if it isn't there
    std::fstream in(name, std::ios::in);
    if (!piper.HasPipe() && name[0] != '-' && !in.is_open())
    {
        Errors::Error(std::string("Could not open ") + errname + " for input");
    }
    else
    {
        in.close();
        if (current)
        {
            files.push_front(std::move(current));
            current = nullptr;
        }
        // this next line and the support code have been carefully crafted so that GetRealFile() should return a reference to the
        // cached object.
        current =
            std::make_unique<ppFile>(fullname, trigraphs, extendedComment, name, define, *ctx, unsignedchar, c89, asmpp, piper);
        // if (current)
        if (!current->Open())
        {
            Errors::Error(std::string("Could not open ") + errname + " for input");
            popFile();
        }
    }
    if (current)
    {
        auto it = fileMap.find(name);
        int currentIndex;
        if (it != fileMap.end())
        {
            currentIndex = it->second;
        }
        else
        {
            fileMap[name] = currentIndex = nextIndex++;
        }
        current->SetIndex(currentIndex);
    }
}
bool ppInclude::popFile()
{
    if (!files.empty())
    {
        current = std::move(files.front());
        files.pop_front();
    }
    else
    {
        current = nullptr;
    }
    forcedEOF = false;
    return true;
}
std::string ppInclude::ParseName(const std::string& args, bool& specifiedAsSystem)
{
    std::string name = "";
    const char* p = args.c_str();
    while (isspace(*p))
        p++;
    int stchr = *p++;
    if (stchr == '"' || stchr == '<')
    {
        specifiedAsSystem = stchr == '<';
        const char* q = p;
        system = stchr == '<';
        int enchr = '"';
        if (system)
            enchr = '>';
        while (*p && *p != enchr)
        {
            p++;
        }
        if (*p)
        {
            char buf[260];
            strncpy(buf, q, p - q);
            buf[p - q] = 0;
            name = buf;
        }
        else
        {
            Errors::Error("File name expected");
        }
    }
    else
        Errors::Error("File name expected");
    return name;
}
// system include: search on system search path first
// search where the source file is from next
// search in local directory next
// search the user include path next
// if didn't already search system path do it now
std::string ppInclude::FindFile(bool specifiedAsSystem, const std::string& name)
{
    std::string rv;
    // search in system search path first, if they specified it with <>
    if (specifiedAsSystem)
        rv = SrchPath(true, name, sysSrchPath);
    // if not there get the file path of the last included file, and search there
    if (rv.empty())
    {
        rv = current->GetRealFile();
        size_t npos = rv.find_last_of('\\');
        if (npos == std::string::npos)
            npos = rv.find_last_of('/');
        if (npos != std::string::npos)
        {
            rv = rv.substr(0, npos);
            rv = SrchPath(false, name, rv);
        }
        else
        {
            rv = "";
        }
    }
    // if not there search in local directory
    if (rv.empty())
    {
        rv = SrchPath(false, name, ".");
    }
    // if not there search on user search path
    if (rv.empty())
        rv = SrchPath(false, name, srchPath);
    // if not there and we haven't searched the system search path, do it now
    if (rv.empty() && !specifiedAsSystem)
        rv = SrchPath(true, name, sysSrchPath);

    return rv;
}

std::string ppInclude::SrchPath(bool system, const std::string& name, const std::string& searchPath)
{
    const char* path = searchPath.c_str();
    char buf[260];
    do
    {
        path = RetrievePath(buf, path);
        AddName(buf, name);

        while (char* p = strchr(buf, '/'))
        {
            *p = CmdFiles::DIR_SEP[0];
        }
        FILE* fil = fopen(buf, "rb");
        if (fil)
        {
            if (searchPath == ".")  // clean up for current directory searches
                memmove(buf, buf + 2, strlen(buf)+1);
            if (!system)
            {
                if (userIncludes.find(buf) == userIncludes.end())
                {
                    userIncludes.insert(buf);
                }
            }
            fclose(fil);
            return buf;
        }
    } while (path);
    return "";
}
const char* ppInclude::RetrievePath(char* buf, const char* path)
{
    while (*path && *path != ';')
    {
        *buf++ = *path++;
    }
    *buf = 0;
    if (*path)
    {
        return path + 1;
    }
    return nullptr;
}
void ppInclude::AddName(char* buf, const std::string& name)
{
    int n = strlen(buf);
    if (n)
    {
        if (buf[n - 1] != '\\')
        {
            buf[n] = '\\';
            buf[++n] = 0;
        }
    }
    strcat(buf, name.c_str());
}
void ppInclude::StripAsmComment(std::string& line)
{
    int quote = 0;
    if (line[0] == '/')
        line = "";
    int n = line.size();
    for (int i = 0; i < n; i++)
    {
        if (line[i] == quote)
        {
            quote = 0;
        }
        else
        {
            switch (line[i])
            {
                case '"':
                case '\'':
                    quote = line[i];
                    break;
                default:
                    if (!quote && line[i] == commentChar)
                    {
                        while (i && isspace(line[i - 1]))
                            i--;
                        line.erase(i);
                        return;
                    }
                    break;
            }
        }
    }
    if (!line.empty())
    {
        n = line.size() - 1;
        while (n && isspace(line[n]))
            n--;
        if (n != line.size() - 1)
            line.erase(n + 1);
    }
}
bool ppInclude::GetLine(std::string& line, int& lineno)
{
    while (current)
    {
        if (!forcedEOF)
        {
            if (current->GetLine(line))
            {
                if (current && !files.empty())
                    lineno = GetErrLineNo();
                else
                    lineno = INT_MIN;
                if (asmpp)
                    StripAsmComment(line);
                return true;
            }
            current->CheckErrors();
        }
        if (!inProc.empty())
        {
            Errors::Error("File ended with " + inProc + " in progress");
            inProc = "";
        }
        popFile();
    }
    return false;
}