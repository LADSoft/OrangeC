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

#define _CRT_SECURE_NO_WARNINGS

#include "ppInclude.h"
#include "PreProcessor.h"
#include "Errors.h"
#include "CmdFiles.h"
#include <limits.h>
#include <fstream>
#include <iostream>

bool ppInclude::system;
std::string ppInclude::srchPath, ppInclude::sysSrchPath;

ppInclude::~ppInclude()
{
    while (current)
        popFile();
}
bool ppInclude::Check(int token, const std::string& args)
{
    if (!current || !current->Check(token, args, current->GetErrorLine()))
        if (!CheckInclude(token, args))
            if (!CheckLine(token, args))
                return false;
    return true;
}

bool ppInclude::CheckInclude(int token, const std::string& args)
{
    if (token == INCLUDE)
    {
        std::string line1 = args;
        define->Process(line1);
        std::string name = ParseName(line1);
        name = FindFile(name);
        pushFile(name, line1);
        return true;
    }
    return false;
}
bool ppInclude::has_include(const std::string& args)
{
    std::string line1 = args;
    std::string name = ParseName(line1);
    name = FindFile(name);
    return name.size() != 0;
}
bool ppInclude::CheckLine(int token, const std::string& args)
{
    if (token == LINE)
    {
        std::string line1 = args;
        define->Process(line1);
        int npos = line1.find_first_of(',');
        if (npos == std::string::npos)
        {
            int n = expr.Eval(line1);
            std::string name = ParseName(line1);
            current->SetErrlineInfo(name, n - 1);
        }
        else
        {
            std::string temp = line1.substr(0, npos);
            int n = expr.Eval(temp);
            std::string name = ParseName(line1.substr(npos + 1));
            current->SetErrlineInfo(name, n - 1);
        }
        return true;
    }
    return false;
}
void ppInclude::pushFile(const std::string& name, const std::string& errname)
{
    // gotta do the test first to get the error correct if it isn't there
    std::fstream in(name.c_str(), std::ios::in);
    if (name[0] != '-' && !in.is_open())
    {
        Errors::Error(std::string("Could not open ") + errname + " for input");
    }
    else
    {
        in.close();
        if (current)
        {
            files.push_front(current);
            current = nullptr;
        }
        current = new ppFile(fullname, trigraphs, extendedComment, name, define, *ctx, unsignedchar, c89, asmpp);
        // if (current)
        if (!current->Open())
        {
            Errors::Error(std::string("Could not open ") + errname + " for input");
            popFile();
        }
    }
}
bool ppInclude::popFile()
{
    if (current)
    {
        delete current;
        current = nullptr;
    }
    if (files.size())
    {
        current = files.front();
        files.pop_front();
    }
    forcedEOF = false;
    return true;
}
std::string ppInclude::ParseName(const std::string& args)
{
    std::string name = "";
    const char* p = args.c_str();
    while (isspace(*p))
        p++;
    int stchr = *p++;
    if (stchr == '"' || stchr == '<')
    {
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
std::string ppInclude::FindFile(const std::string& name)
{
    FILE *fil = fopen(name.c_str(), "rb");
    if (fil)
    {
        fclose(fil);
        return name;
    }
    std::string rv = SrchPath(system, name);
    if (rv.size() == 0)
        rv = SrchPath(!system, name);
    return rv;
}
std::string ppInclude::SrchPath(bool system, const std::string& name)
{
    const char* path;
    if (system)
        path = sysSrchPath.c_str();
    else
        path = srchPath.c_str();
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
    else
    {
        return nullptr;
    }
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
                case ';':
                    if (!quote)
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
    if (line.size())
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
                if (current && files.size() == 0)
                    lineno = GetLineNo();
                else
                    lineno = INT_MIN;
                if (asmpp)
                    StripAsmComment(line);
                return true;
            }
            current->CheckErrors();
        }
        if (inProc.size())
        {
            Errors::Error(std::string("File ended with ") + inProc + " in progress");
            inProc = "";
        }
        popFile();
    }
    return false;
}