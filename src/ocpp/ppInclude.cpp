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
#define _CRT_SECURE_NO_WARNINGS

#include "ppInclude.h"
#include "PreProcessor.h"
#include "Errors.h"
#include "CmdFiles.h"
#include <limits.h>
#include <fstream>			  
ppInclude::~ppInclude()
{
    while (current)
        popFile();
}
bool ppInclude::Check(int token, const std::string &args)
{
    if (!current || !current->Check(token, args, current->GetErrorLine()))
        if (!CheckInclude(token, args))
            if (!CheckLine(token, args))
                return false;
    return true;
}

bool ppInclude::CheckInclude(int token, const std::string &args)
{
    if (token == INCLUDE)
    {
        std::string line1 = args;
        define->Process(line1);
        ParseName(line1);
        FindFile(args);
        pushFile(name);
        return true;
    }
    return false;
}
bool ppInclude::CheckLine(int token, const std::string &args)
{
    if (token == LINE)
    {
        std::string line1 = args;
        define->Process(line1);
        int npos = line1.find_first_of(',');
        if (npos == std::string::npos)
        {
            int n = expr.Eval(line1);
            ParseName(line1);
            current->SetErrlineInfo(name, n-1);
        }
        else
        {
            int n = expr.Eval(line1.substr(0, npos));
            ParseName(line1.substr(npos+1));
            current->SetErrlineInfo(name, n-1);
        }
        return true;		
    }
    return false;
}
void ppInclude::pushFile(const std::string &name)
{
    // gotta do the test first to get the error correct if it isn't there
    std::fstream in(name.c_str(), std::ios::in);
    if (in == NULL)
    {
        Errors::Error(std::string("Could not open ") + name + " for input");
    }
    else
    {
        in.close();
        if (current)
        {
            files.push_front(current);
            current = NULL;
        }
        current = new ppFile(fullname, trigraphs, extendedComment, name, define, *ctx, unsignedchar, c89, asmpp);
        //if (current)
            if (!current->Open())
            {
                Errors::Error(std::string("Could not open ") + name + " for input");
                popFile();
            }
    }
}
bool ppInclude::popFile()
{
    if (current)
    {
        delete current;
        current = NULL;
    }
    if (files.size())
    {
        current = files.front();
        files.pop_front();
    }
    return true;
}
void ppInclude::ParseName(const std::string &args)
{
    const char *p = args.c_str();
    while (isspace(*p))
        p++;
    int stchr = *p++;
    if (stchr == '"' || stchr == '<')
    {
        const char *q = p;
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
            buf[p-q] = 0;
            name = buf;
        }
        else
        {
            Errors::Error("File name expected");
        }
    }
    else
        Errors::Error("File name expected");
}
void ppInclude::FindFile(const std::string &args)
{
    if (!SrchPath(system))
        SrchPath(!system);
}
bool ppInclude::SrchPath(bool system)
{
    const char *path;
    if (system)
        path = sysSrchPath.c_str();
    else
        path = srchPath.c_str();
    char buf[260];
    do
    {
        path = RetrievePath(buf, path);
        AddName(buf);
        while (char *p = strchr(buf, '/'))
        {
            *p = CmdFiles::DIR_SEP[0];
        }
        FILE *fil = fopen(buf, "rb");
        if (fil)
        {
            fclose(fil);
            name = buf;
            return true;
        }
    } while (path);
    return false;
}
const char *ppInclude::RetrievePath(char *buf, const char *path)
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
        return NULL;
    }
}
void ppInclude::AddName(char *buf)
{
    int n = strlen(buf);
    if (n)
    {
        if (buf[n-1] != '\\')
        {
            buf[n] = '\\';
            buf[++n] = 0;
        }
    }
    strcat(buf, name.c_str());
}
void ppInclude::StripAsmComment(std::string &line)
{
    int quote = 0;
    int n = line.size();
    for (int i=0; i < n; i++)
    {
        if (line[i] == quote)
        {
            quote = 0;
        }
        else
        {
            switch(line[i])
            {
                case '"':
                case '\'':
                    quote = line[i];
                    break;
                case ';':
                    if (!quote)
                    {
                        while (i && isspace(line[i-1]))
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
        n = line.size()-1;
        while (n && isspace(line[n]))
            n--;
        if (n != line.size()-1)
            line.erase(n+1);
    }
}
bool ppInclude::GetLine(std::string &line, int &lineno)
{
    while (current)
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
        if (inProc.size())
        {
            Errors::Error(std::string("File ended with ") + inProc + " in progress");
            inProc = "";
        }
        popFile();
    }
    return false;
        
}