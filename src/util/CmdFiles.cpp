/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#ifndef HAVE_UNISTD_H
#    include <io.h>
#endif

#include "CmdFiles.h"
using namespace std;  // borland puts the io stuff in the std namespace...
                      // microsoft does not seem to.

const char* CmdFiles::DIR_SEP = "\\";
const char* CmdFiles::PATH_SEP = ";";

CmdFiles::~CmdFiles() {}
bool CmdFiles::Add(char** array, bool recurseDirs)
{
    while (*array)
    {
        Add(std::string(*array), recurseDirs);
        array++;
    }
    return true;
}
bool CmdFiles::RecurseDirs(const std::string& path, const std::string& name, bool recurseDirs)
{
    bool rv = false;
#ifndef HAVE_UNISTD_H
    struct _finddata_t find;
#endif
    std::string q = path + "*.*";
    size_t handle;
    // borland does not define the char * as const...
#ifndef HAVE_UNISTD_H
    if ((handle = _findfirst(const_cast<char*>(q.c_str()), &find)) != -1)
    {
        do
        {
            if (strcmp(find.name, ".") && strcmp(find.name, ".."))
            {
                if (find.attrib & _A_SUBDIR)
                {
                    std::string newName = path + std::string(find.name) + DIR_SEP + name;
                    rv |= Add(newName, recurseDirs);
                }
            }
        } while (_findnext(handle, &find) != -1);
        _findclose(handle);
    }
#endif
    return rv;
}
bool CmdFiles::Add(const std::string& name, bool recurseDirs)
{
    bool rv = false;
#ifndef HAVE_UNISTD_H
    struct _finddata_t find;
#endif
    std::string path, lname;
    size_t n = name.find_last_of(DIR_SEP[0]);
    size_t n1 = name.find_last_of('/');
    if (n1 != std::string::npos && n != std::string::npos)
        n = n1 > n ? n1 : n;
    else if (n == std::string::npos)
        n = n1;
    if (n != std::string::npos)
    {
        path = name.substr(0, n + 1);
        lname = name.substr(n + 1);
    }
    else
    {
        n = name.find_last_of(':');
        if (n != std::string::npos)
        {
            path = name.substr(0, n + 1);
            lname = name.substr(n + 1);
        }
        else
        {
            lname = name;
        }
    }
    size_t handle;
    // borland does not define the char * as const...
#ifndef HAVE_UNISTD_H
    if ((handle = _findfirst(const_cast<char*>(name.c_str()), &find)) != -1)
    {
        do
        {
            if (!(find.attrib & _A_SUBDIR) && /*!(find.attrib & _A_VOLID) && */
                !(find.attrib & _A_HIDDEN))
            {
                std::string file(path + std::string(find.name));
                names.push_back(file);
                rv = true;
            }
        } while (_findnext(handle, &find) != -1);
        _findclose(handle);
    }
#endif
    if (recurseDirs)
    {
        rv |= RecurseDirs(path, lname, recurseDirs);
    }
    if (!rv)
    {
        if (name.find_first_of('*') == std::string::npos && name.find_first_of('?') == std::string::npos)
        {
            names.push_back(name);
            rv = true;
        }
    }
    return rv;
}
bool CmdFiles::AddFromPath(const std::string& name, const std::string& path)
{
    bool rv = false;

    rv = Add(name, false);
    if (!rv)
    {
        size_t n = name.find_last_of(DIR_SEP[0]);
        size_t n1 = name.find_last_of('/');
        if (n1 != std::string::npos && n != std::string::npos)
            n = n1 > n ? n1 : n;
        if (n != std::string::npos)
        {
            n++;
        }
        else
        {
            n = name.find_first_of(":");
            if (n != std::string::npos)
                n++;
            else
                n = 0;
        }
        std::string internalName = name.substr(n, name.size());
        n = 0;
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
            if (curpath.size() != 0 && curpath.substr(curpath.size() - 1, curpath.size()) != DIR_SEP)
            {
                curpath += DIR_SEP;
            }
            curpath += internalName;
            rv = Add(curpath, false);
        }
    }
    return rv;
}