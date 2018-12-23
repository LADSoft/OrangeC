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

#include "Include.h"
#include "Parser.h"
#include "CmdFiles.h"
#include "Eval.h"
#include "Variable.h"
#include "Maker.h"
#include "Rule.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <algorithm>
#ifdef GCCLINUX
#    include <unistd.h>
#else
#    include <io.h>
#endif

Include* Include::instance = nullptr;

Include* Include::Instance()
{
    if (!instance)
        instance = new Include;
    return instance;
}
void Include::Clear()
{
    files.clear();
    ignoredFiles.clear();
    Variable* v = VariableContainer::Instance()->Lookup("MAKEFILE_LIST");
    if (v)
        v->SetValue("");
}
bool Include::Parse(const std::string& name, bool ignoreOk, bool MakeFiles)
{
    bool rv = false;
    if (name == "-")
    {
        rv = true;
        while (!std::cin.eof())
        {
            char buf[2048];
            buf[0] = 0;
            std::cin.getline(buf, sizeof(buf));
            if (buf[0])
            {
                Parser p(std::string(buf), name, 1);
                if (MakeFiles)
                    p.SetIgnoreFirstGoal();
                rv = p.Parse();
            }
        }
    }
    else
    {
        std::string current = name;
        if (access(current.c_str(), 0) == -1)
        {
            std::string includeDirs;
            Variable* id = VariableContainer::Instance()->Lookup(".INCLUDE_DIRS");
            if (id)
            {
                includeDirs = id->GetValue();
                if (id->GetFlavor() == Variable::f_recursive)
                {
                    Eval r(includeDirs, false);
                    includeDirs = r.Evaluate();
                }
            }
            while (includeDirs.size())
            {
                current = Eval::ExtractFirst(includeDirs, ";") + "\\" + name;
                if (access(current.c_str(), 0) != -1)
                    break;
            }
        }
        std::fstream in(current.c_str(), std::ios::in | std::ios::binary);
        if (!in.fail())
        {
            in.seekg(0, std::ios::end);
            size_t len = in.tellg();
            in.seekg(0);
            if (!in.fail())
            {
                char* text = new char[len + 1];
                in.read(text, len);
                text[len] = 0;
                in.close();
                char *p = text, *q = p;
                while (*p)
                    if (*p != '\r')
                        *q++ = *p++;
                    else
                        p++;
                *q = 0;
                len = strlen(text);
                if (!in.fail())
                {
                    Parser p(std::string(text), name, 1);
                    if (MakeFiles)
                        p.SetIgnoreFirstGoal();
                    rv = p.Parse();
                }
            }
        }
        else
        {

            if (ignoreOk)
            {
                ignoredFiles.insert(name);
                rv = true;
            }
        }
    }
    return rv;
}
bool Include::AddFileList(const std::string& name, bool ignoreOk, bool MakeFile)
{
    Eval e(name, false);
    std::string iname = e.Evaluate();
    bool rv = true;
    CmdFiles cmdFiles;
    std::string seps(" ");
    seps += CmdFiles::PATH_SEP;
    std::string includeDirs;
    Variable* id = VariableContainer::Instance()->Lookup(".INCLUDE_DIRS");
    if (id)
    {
        includeDirs = id->GetValue();
        if (id->GetFlavor() == Variable::f_recursive)
        {
            Eval r(includeDirs, false);
            includeDirs = r.Evaluate();
        }
    }
    while (iname.size())
    {
        std::string current = Eval::ExtractFirst(iname, seps);
        cmdFiles.AddFromPath(current, includeDirs);
    }
    for (CmdFiles::FileNameIterator it = cmdFiles.FileNameBegin(); rv && it != cmdFiles.FileNameEnd(); ++it)
    {
        Variable* v = VariableContainer::Instance()->Lookup("MAKEFILE_LIST");
        if (!v)
        {
            v = new Variable(std::string("MAKEFILE_LIST"), *(*it), Variable::f_simple, Variable::o_file);
            *VariableContainer::Instance() += v;
        }
        else
        {
            v->SetValue(v->GetValue() + " " + *(*it));
        }
        files.push_back(*(*it));
        rv &= Parse(*(*it), ignoreOk | MakeFile, MakeFile);
    }
    return rv;
}
bool Include::MakeMakefiles(bool Silent, OutputType outputType)
{
    Maker maker(Silent, false, false, false, outputType);
    for (auto goal : *this)
    {
        if (goal != "-")
            maker.AddGoal(goal);
    }
    for (auto file : ignoredFiles)
    {
        maker.SetIgnoreFailed(file);
    }
    maker.CreateDependencyTree();
    return maker.RunCommands();
}
