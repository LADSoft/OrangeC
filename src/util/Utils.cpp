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

#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <cstdlib>
#include <cstring>
#ifdef _WIN32
#    include <windows.h>
#endif
#include "Utils.h"
#include "CmdFiles.h"

#include <sstream>
#include "../version.h"
#include <iostream>

char* Utils::ShortName(const char* v)
{
    static char prog_name[260], *short_name, *extension;
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
void Utils::banner(const char* progName)
{
    // no banner if they specify -!, this is also caught in the cmd switch module
    // so it is transparent to the proggy
#ifndef GCCLINUX
    for (int i = 1; i < __argc && __argv[i]; i++)
        if (__argv[i] && (__argv[i][0] == '/' || __argv[i][0] == '-'))
            if (__argv[i][1] == '!' || !strcmp(__argv[i], "--nologo"))
                return;
#endif
    fprintf(stderr, "%s Version " STRING_VERSION " " COPYRIGHT "\n", ShortName(progName));

#ifndef GCCLINUX
    // handle /V switch
    for (int i = 1; i < __argc && __argv[i]; i++)
        if (__argv[i] && (__argv[i][0] == '/' || __argv[i][0] == '-'))
            if (__argv[i][1] == 'V' && __argv[i][2] == 0 || !strcmp(__argv[i], "--version"))
            {
                fprintf(stderr, "\nCompile date: " __DATE__ " time: " __TIME__ "\n");
                exit(0);
            }
#endif
}
void Utils::usage(const char* prog_name, const char* text)
{
    fprintf(stderr, "\nUsage: %s %s", ShortName(prog_name), text);
    exit(1);
}
char* Utils::GetModuleName()
{
    static char buf[256];
#if defined(_WIN32)
    GetModuleFileNameA(nullptr, buf, sizeof(buf));
#else
#    ifdef GCCLINUX
    strcpy(buf, "unknown");
#    else
    strcpy(buf, __argv[0]);
#    endif
#endif
    return buf;
}
void Utils::SetEnvironmentToPathParent(const char* name)
{
    if (!getenv(name))
    {
        char buf[512];
        strcpy(buf, GetModuleName());
        char* p = strrchr(buf, '\\');
        if (p)
        {
            *p = 0;
            p = strrchr(buf, '\\');
            if (p)
            {
                *p = 0;
                char* buf1 = (char*)calloc(1, strlen(name) + strlen(buf) + 2);
                strcpy(buf1, name);
                strcat(buf1, "=");
                strcat(buf1, buf);
                putenv(buf1);
            }
        }
    }
}
std::string Utils::FullPath(const std::string& path, const std::string& name)
{
    std::fstream in(name, std::ios::in);
    if (!in.fail())
    {
        return name;
    }
    if (path.size())
    {
        std::string fpath = path;
        //        if (path.c_str()[strlen(path.c_str())-1] != '\\')
        if (path[path.length() - 1] != '\\')
        {
            fpath += "\\";
        }
        fpath += name;
        std::fstream in(fpath, std::ios::in);
        if (!in.fail())
        {
            return fpath;
        }
    }
    return name;
}
std::string Utils::QualifiedFile(const char* path, const char* ext)
{
    char buf[260];
    strcpy(buf, path);
    char* p = strrchr(buf, '.');
    if (!p || p[-1] == '.' || p[1] == '\\')
        p = buf + strlen(buf);
    strcpy(p, ext);
    return std::string(buf);
}
std::string Utils::SearchForFile(const std::string& path, const std::string& name)
{
    FILE* fil = fopen(name.c_str(), "rb");
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
std::string Utils::NumberToString(int num) { return std::to_string(num); }
std::string Utils::NumberToStringHex(int num)
{
    std::stringstream aa;
    aa << std::hex << num;
    return aa.str();
}
int Utils::StringToNumber(std::string str) { return std::stoi(str); }
int Utils::StringToNumberHex(std::string str) { return std::stoi(str, 0, 16); }
