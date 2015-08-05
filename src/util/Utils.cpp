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
#include "..\version.h"

char *Utils::ShortName(char *v)
{
    static char prog_name[MAX_PATH], *short_name, *extension;
    strcpy(prog_name, v);
    short_name = strrchr(prog_name, '\\');
    if (short_name == NULL)
        short_name = strrchr(prog_name, '/');
    if (short_name == NULL)
        short_name = strrchr(prog_name, ':');
    if (short_name)
        short_name++;
    else
        short_name = prog_name;

    extension = strrchr(short_name, '.');
    if (extension != NULL)
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
       printf("%s Version "STRING_VERSION" "COPYRIGHT"\n", ShortName(progName));
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
    GetModuleFileNameA(NULL, buf, sizeof(buf));
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
