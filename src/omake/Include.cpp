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

Include *Include::instance = NULL;

Include *Include::Instance()
{
    if (!instance)
        instance = new Include;
    return instance;
}
void Include::Clear()
{
    files.clear();
    ignoredFiles.clear();
     Variable *v = VariableContainer::Instance()->Lookup("MAKEFILE_LIST");
    if (v)
        v->SetValue("");
}
bool Include::Parse(const std::string &name, bool ignoreOk, bool MakeFiles)
{
    bool rv = false;
    std::fstream in(name.c_str(), std::ios::in | std::ios::binary);
    if (in != NULL)
    {
        in.seekg(0, std::ios::end);
        size_t len = in.tellg();
        in.seekg(0);
        if (!in.fail())
        {
            char *text = new char[len + 1];
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
    return rv;
}
bool Include::AddFileList(const std::string &name, bool ignoreOk, bool MakeFile)
{
    Eval e(name, false);
     std::string iname = e.Evaluate();
     bool rv = true;
    CmdFiles cmdFiles;
    std::string seps(" ");
    seps += CmdFiles::PATH_SEP;
    std::string includeDirs;
    Variable *id = VariableContainer::Instance()->Lookup(".INCLUDE_DIRS");
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
         Variable *v = VariableContainer::Instance()->Lookup("MAKEFILE_LIST");
        if (!v)
        {
            v = new Variable(std::string("MAKEFILE_LIST"), *(*it), Variable::f_simple, Variable::o_file);
            *VariableContainer::Instance() += v;
        }
        else
        {
            v->SetValue(v->GetValue() +" " + *(*it));
        }
        files.push_back(*(*it));
        rv &= Parse(*(*it), ignoreOk | MakeFile, MakeFile);
    }
    return rv;
}
bool Include::MakeMakefiles(bool Silent)
{
    Maker maker(Silent, false, false, false);
    for (iterator it = begin(); it != end(); ++it)
    {
        maker.AddGoal(*it);
    }
    for (std::set<std::string>::iterator it = ignoredFiles.begin();
         it != ignoredFiles.end(); ++it)
    {
        maker.SetIgnoreFailed(*it);
    }
    maker.CreateDependencyTree();
    return maker.RunCommands();
}
