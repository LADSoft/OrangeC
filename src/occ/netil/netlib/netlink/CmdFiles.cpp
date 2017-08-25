/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include <windows.h>
#include <io.h>
#include "CmdFiles.h"
using namespace std; // borland puts the io stuff in the std namespace...
                     // microsoft does not seem to.

const char *CmdFiles::DIR_SEP = "\\";
const char *CmdFiles::PATH_SEP = ";";

CmdFiles::~CmdFiles()
{
    for (auto name : names)
    {
        delete name;
    }
}
bool CmdFiles::Add(char **array, bool recurseDirs)
{
    while (*array)
    {
        Add(std::string(*array), recurseDirs);
        array++;
    }
    return true;
}
bool CmdFiles::RecurseDirs(const std::string &path, const std::string &name, bool recurseDirs)
{
    bool rv = false;
    struct _finddata_t find;
    std::string q = path + "*.*";
    long handle;
    // borland does not define the char * as const...
    if ((handle = _findfirst(const_cast<char *>(q.c_str()), &find)) != -1)
    {
        do
        {
            if (strcmp(find.name, ".") && strcmp(find.name, ".."))
            {
                if (find.attrib & _A_SUBDIR)
                {
                    std::string newName = path + std::string(find.name) + DIR_SEP + name;;
                    rv |= Add(newName, recurseDirs);
                }
            }
        }
        while (_findnext(handle, &find) != -1);
        _findclose(handle);
    }
    return rv;
}
bool CmdFiles::Add(const std::string &name, bool recurseDirs)
{
    bool rv = false;
    struct _finddata_t find;
    std::string path, lname;
    size_t n = name.find_last_of(DIR_SEP[0]);
    if (n != std::string::npos)
    {
        path = name.substr(0,n+1);
        lname = name.substr(n + 1);
    }
    else
    {
        n = name.find_last_of(':');
        if (n != std::string::npos)			
        {
            path = name.substr(0, n+1);
            lname = name.substr(n + 1);
        }
        else
        {
            lname = name;
        }
    }
    long handle;
    // borland does not define the char * as const...
    if ((handle = _findfirst(const_cast<char *>(name.c_str()), &find)) != -1)
    {
        do
        {
            if (!(find.attrib & _A_SUBDIR) && /*!(find.attrib & _A_VOLID) && */
                !(find.attrib & _A_HIDDEN))
            {
                std::string *file = new std::string(path + std::string(find.name));
                names.push_back(file);		
                rv = true;
            }
        }
        while (_findnext(handle, &find) != -1);
        _findclose(handle);
    }
    if (recurseDirs)
    {
        rv |= RecurseDirs(path, lname, recurseDirs);
    }
    if (!rv)
    {
        if (name.find_first_of('*')==std::string::npos && name.find_first_of('?') == std::string::npos)
        {
            std::string *newName = new std::string(name);
            names.push_back(newName);
        }
    }
    return rv;
}
bool CmdFiles::AddFromPath(const std::string &name, const std::string &path)
{
    bool rv = false;
    
    rv = Add(name, false);
    if (!rv)
    {
        size_t x = name.find_last_of(DIR_SEP[0]);
        if (x != std::string::npos)
        {
            x++ ;
        }
        else
        {
            x = name.find_first_of(":");
            if (x != std::string::npos)
                x++;
            else
                x = 0;
        }
        std::string internalName = name.substr(x, name.size());
        size_t n = 0;
        //size_t m = 0;
        bool done = false;
        while (!done)
        {
            size_t m = path.find_first_of(PATH_SEP, n);
            if (m == std::string::npos)
            {
                m = path.size();
                done = true;
            }
            std::string curpath = path.substr(m, n);
            n = m + 1;
            if (curpath.size() != 0 && curpath.substr(curpath.size()-1, curpath.size()) != DIR_SEP)
            {
                curpath += DIR_SEP;
            }
            curpath += internalName;
            rv = Add(curpath, false);
        }
    }
    return rv;
}