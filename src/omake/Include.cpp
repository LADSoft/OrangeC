/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "Include.h"
#include "Parser.h"
#include "CmdFiles.h"
#include "Eval.h"
#include "Variable.h"
#include "Maker.h"
#include "Rule.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

std::shared_ptr<Include> Include::instance = nullptr;

std::shared_ptr<Include> Include::Instance()
{
    if (!instance)
        instance = std::shared_ptr<Include>(new Include);
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
        std::string inputFile;
        while (!std::cin.eof())
        {
            char buf[2048];
            buf[0] = 0;
            std::cin.getline(buf, sizeof(buf));
            if (buf[0])
            {
                inputFile += buf;
                inputFile += "\n";
            }
        }
        Parser p(inputFile, "con:", 1);
        if (MakeFiles)
            p.SetIgnoreFirstGoal();
        rv = p.Parse();
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
            while (!includeDirs.empty())
            {
                current = Eval::ExtractFirst(includeDirs, ";") + "\\" + name;
                if (access(current.c_str(), 0) != -1)
                    break;
            }
        }
        std::fstream in(current, std::ios::in | std::ios::binary);
        if (!in.fail())
        {
            in.seekg(0, std::ios::end);
            std::streamoff len = in.tellg();
            in.seekg(0);
            if (!in.fail())
            {
                std::unique_ptr<char> text(new char[len + 1]);
                in.read(text.get(), len);
                text.get()[len] = 0;
                in.close();
                char *p = text.get(), *q = p;
                while (*p)
                    if (*p != '\r')
                        *q++ = *p++;
                    else
                        p++;
                *q = 0;
                len = strlen(text.get());
                if (!in.fail())
                {
                    Parser p(std::string(text.get()), name, 1);
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
#include <direct.h>
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
    while (!iname.empty())
    {
        std::string current = Eval::ExtractFirst(iname, seps);
        if (current == "-")
        {
           cmdFiles.Add(current);
        }
        else
        {
            auto includes = includeDirs;
            if (!currentPath.empty() && current[0] == '.')
            {
                includes = currentPath.top() + CmdFiles::PATH_SEP + includes;
            }
            cmdFiles.AddFromPath(current, includes);
        }
    }
    for (auto it = cmdFiles.FileNameBegin(); rv && it != cmdFiles.FileNameEnd(); ++it)
    {
        Variable* v = VariableContainer::Instance()->Lookup("MAKEFILE_LIST");
        if (!v)
        {
            v = new Variable(std::string("MAKEFILE_LIST"), (*it), Variable::f_simple, Variable::o_file);
            *VariableContainer::Instance() += v;
        }
        else
        {
            v->SetValue(v->GetValue() + " " + (*it));
        }
        files.push_back((*it));
        auto path = (*it);
        int n = path.find_last_of("/\\");
        if (n != std::string::npos)
        {
            path = path.substr(0, n);
            currentPath.push(path);
        }
        rv &= Parse((*it), ignoreOk || MakeFile, MakeFile);
        if (n != std::string::npos)
        {
            currentPath.pop();
        }
    }
    return rv;
}
bool Include::MakeMakefiles(bool Silent, OutputType outputType, bool& didSomething)
{
    Maker maker(Silent, false, false, false, outputType);
    for (auto&& goal : *this)
    {
        if (goal != "-")
            maker.AddGoal(goal);
    }
    for (auto&& file : ignoredFiles)
    {
        maker.SetIgnoreFailed(file);
    }
    didSomething = !maker.CreateDependencyTree();
    return maker.RunCommands();
}
