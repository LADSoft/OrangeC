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
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <fstream>
#include <stdlib.h>
#ifdef MICROSOFT
#include <windows.h>
#endif
#include "utils.h"
#include "CmdFiles.h"

#ifdef OPENWATCOM
#include <strstream>
#else
#include <sstream>
#endif
#include "version.h"

char *Utils::ShortName(char *v)
{
    static char prog_name[MAX_PATH], *short_name, *extension;
    strcpy(prog_name, v);
    short_name = strrchr(prog_name, '\\');
    if (short_name == nullptr)
        short_name = strrchr(prog_name, '/');
    if (short_name == nullptr)
        short_name = strrchr(prog_name, ':');
    if (short_name)
        short_name++;
    else
        short_name = prog_name;

    extension = strrchr(short_name, '.');
    if (extension != nullptr)
        *extension = '\0';
    return short_name;
}
void Utils::banner(char *progName)
{
    // no banner if they specify -!, this is also caught in the cmd switch module
    // so it is transparent to the proggy
    for (int i=1; i < __argc; i++)
        if (__argv[i] && (__argv[i][0] == '/' || __argv[i][0] == '-'))
            if (__argv[i][1] == '!')
                return;
       printf("%s Version " STRING_VERSION " " COPYRIGHT "\n", ShortName(progName));
}
void Utils::usage(char *prog_name, char *text)
{
    printf("\nUsage: %s %s", ShortName(prog_name), text);
    exit(1);
}
void Utils::fatal(const char *format, ...)
{
    va_list argptr;

    va_start(argptr, format);
    printf("Fatal error: ");
    vprintf(format, argptr);
    va_end(argptr);
    fputc('\n',stdout);
    exit(1);
}
char *Utils::GetModuleName()
{
    static char buf[256];
#if defined(WIN32) || defined(MICROSOFT)
    GetModuleFileNameA(nullptr, buf, sizeof(buf));
#else   
    strcpy(buf, __argv[0]);
#endif
    return buf;
}
std::string Utils::FullPath(const std::string &path, const std::string &name)
{
    std::fstream in(name.c_str(), std::ios::in);
    if (!in.fail())
    {
        return name;
    }
    if (path.size())
    {
        std::string fpath=path;
//        if (path.c_str()[strlen(path.c_str())-1] != '\\')
        if (path.c_str()[path.length()-1] != '\\')
        {
            fpath += std::string("\\");
        }
        fpath += name;
        std::fstream in(fpath.c_str(), std::ios::in);
        if (!in.fail())
        {
            return fpath;
        }
    }
    return name;
}
std::string Utils::QualifiedFile(const char *path, const char *ext)
{
    char buf[MAX_PATH];
    strcpy(buf, path);
    char *p = strrchr(buf, '.');
    if (!p || p[-1] == '.' || p[1] == '\\')
        p = buf + strlen(buf);
    strcpy(p, ext);
    return std::string(buf);
}
std::string Utils::SearchForFile(const std::string &path, const std::string &name)
{
    FILE *fil = fopen(name.c_str(), "rb");
    if (fil)
    {
        fclose(fil);
        return name;
    }
    std::string fpath = path;
    while (fpath.size())
    {
        int npos = fpath.find_first_of(CmdFiles::PATH_SEP[0]);
        std::string current;
        if (npos == std::string::npos)
        {
            current = fpath;
            fpath = "";
        }
        else
        {
            current = fpath.substr(0, npos);
            fpath.erase(0, npos + 1);
        }
        if (current[current.size()] != CmdFiles::DIR_SEP[0])
        {
            current += CmdFiles::DIR_SEP;
        }
        current += name;
        fil = fopen(current.c_str(), "rb");
        if (fil)
        {
            fclose(fil);
            return current;
        }
    }
    return name;
}
std::string Utils::NumberToString(int num)
{
#ifdef OPENWATCOM
    std::ostrstream aa;
    aa << num;
    return std::string(aa.rdbuf()->str());
#else
    std::stringstream aa;
    aa << num;
    std:: string rv;
    aa >> rv;
    return rv;
#endif
}
std::string Utils::NumberToStringHex(int num)
{
#ifdef OPENWATCOM
    std::ostrstream aa;
    aa << std::hex << num;
    return std::string(aa.rdbuf()->str());
#else
    std::stringstream aa;
    aa << std::hex << num;
    std:: string rv;
    aa >> rv;
    return rv;
#endif
}
int Utils::StringToNumber(std::string str)
{
#ifdef OPENWATCOM
    std::ostrstream aa;
    aa << str.c_str();
    std::istrstream bb(aa.rdbuf()->str());
    int rv;
    bb >> rv;
    return rv;
#else
    std::stringstream aa;
    aa << str;
    int rv;
    aa >> rv;
    return rv;
#endif
}
int Utils::StringToNumberHex(std::string str)
{
#ifdef OPENWATCOM
    std::ostrstream aa;
    aa << str.c_str();
    std::istrstream bb(aa.rdbuf()->str());
    int rv;
    bb >> std::hex >> rv;
    return rv;
#else
    std::stringstream aa;
    aa << str;
    int rv;
    aa >> std::hex >> rv;
    return rv;
#endif
}
