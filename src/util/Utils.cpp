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
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <cstring>
#ifdef _WIN32
#    include <windows.h>
#endif
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#    define _access access
#    define _isatty isatty
#    include <sys/ioctl.h>
#else
#    include <io.h>
extern "C" char* getcwd(char*, int);
#    ifdef BORLAND
#        define _isatty isatty
#    endif
#endif

#include "Utils.h"
#include "CmdFiles.h"

#include <sstream>
#include "../version.h"
#include <iostream>

void (*Utils::cleanup)();

char* Utils::ShortName(const char* v)
{
    static char prog_name[260], *short_name, *extension;
    StrCpy(prog_name, v);
    short_name = (char *)strrchr(prog_name, '\\');
    if (short_name == nullptr)
        short_name = (char *)strrchr(prog_name, '/');
    if (short_name == nullptr)
        short_name = (char *)strrchr(prog_name, ':');
    if (short_name)
        short_name++;
    else
        short_name = prog_name;

    extension = (char *)strrchr(short_name, '.');
    if (extension != nullptr)
        *extension = '\0';
    return short_name;
}
void Utils::banner(const char* progName)
{
    bool have_version = false;
    // no banner if they specify -!, this is also caught in the cmd switch module
    // so it is transparent to the proggy
#ifndef HAVE_UNISTD_H
    // handle /V switch
    for (int i = 1; i < __argc && __argv[i]; i++)
        if (__argv[i] && (__argv[i][0] == '/' || __argv[i][0] == '-'))
            if ((__argv[i][1] == 'V' && __argv[i][2] == 0) || !strcmp(__argv[i], "--version"))
            {
                have_version = true;
            }
    if (!have_version)
        for (int i = 1; i < __argc && __argv[i]; i++)
            if (__argv[i] && (__argv[i][0] == '/' || __argv[i][0] == '-'))
            {
                if (__argv[i][1] == '!' || !strcmp(__argv[i], "--nologo"))
                {
                    return;
                }
                else if (!strncmp(&__argv[i][1], "print", 5) || !strncmp(&__argv[i][1], "dump", 4))
                {
                    return;
                }
                else if (!strncmp(&__argv[i][1], "-print", 6) || !strncmp(&__argv[i][1], "-dump", 5))
                {
                    return;
                }
            }
#endif
    printf("%s (OrangeC) Version " STRING_VERSION "\n" COPYRIGHT "\n", ShortName(progName));
    if (have_version)
    {
        printf("\nCompile date: " __DATE__ " time: " __TIME__ "\n");
        exit(0);
    }
}
int Utils::ScreenHeight()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize max;
    ioctl(0, TIOCGWINSZ, &max);
    return max.ws_row;
#endif
}
bool Utils::GetLine(const char** text, std::string& buf)
{
    if (!**text)
        return false;
    char const* start = *text;
    auto temp = (char *)strchr(*text, '\n');
    if (!temp)
    {
        *text += strlen(*text);
    }
    else
    {
        *text = temp + 1;
    }
    buf = std::string(start, *text);
    return true;
}
void Utils::usage(const char* prog_name, const char* text)
{

    int rows = 10000;
#ifdef _WIN32
    if (_isatty(fileno(stderr)))
        rows = ScreenHeight();
#else
    if (_isatty(STDERR_FILENO))
        rows = ScreenHeight();
#endif
    fprintf(stderr, "\nUsage: %s ", ShortName(prog_name));
    int left = rows - 4;
    std::string buf;
    while (GetLine(&text, buf))
    {
        std::cerr << buf;
        if (--left == 0)
        {
            std::cerr << "Press <ENTER> to continue...";
            char temp[512];
            fgets(temp, sizeof(temp), stdin);
            left = rows - 1;
        }
    }
    exit(1);
}
char* Utils::GetModuleName()
{
    static char buf[256];
#if defined(_WIN32)
    GetModuleFileNameA(nullptr, buf, sizeof(buf));
#else
#    ifdef HAVE_UNISTD_H
    StrCpy(buf, "unknown");
#    else
    StrCpy(buf, __argv[0]);
#    endif
#endif
    return buf;
}
void Utils::SetEnvironmentToPathParent(const char* name)
{
    if (!getenv(name))
    {
        char buf[512];
        StrCpy(buf, GetModuleName());
        char* p = (char *)strrchr(buf, '\\');
        if (p)
        {
            *p = 0;
            p = (char *)strrchr(buf, '\\');
            if (p)
            {
                *p = 0;
                int len = strlen(name) + strlen(buf) + 2;
                char* buf1 = (char*)calloc(1, len);
                StrCpy(buf1, len, name);
                StrCat(buf1, len, "=");
                StrCat(buf1, len, buf);
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
    if (!path.empty())
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
std::string Utils::AbsolutePath(const std::string& name)
{
    std::string rv = name;
#if defined(WIN32)
    if (name.size() >= 3 && (name[1] != ':' || name[2] != CmdFiles::DIR_SEP[0]))
    {
        char buf[MAX_PATH];
        GetCurrentDirectoryA(sizeof(buf), buf);
        if (name[1] == ':')
            rv = name.substr(0, 2) + (buf + 2) + CmdFiles::DIR_SEP + name.substr(2);
        else if (name[0] == CmdFiles::DIR_SEP[0])
            rv = std::string(buf).substr(0, 2) + name;
        else
            rv = std::string(buf) + CmdFiles::DIR_SEP + name;
    }
#endif
    return rv;
}
std::string Utils::QualifiedFile(const char* path, const char* ext)
{
    char buf[260];
    Utils::StrCpy(buf, path);
    char* p = (char *)strrchr(buf, '.');
    if (!p || (p != buf && p[-1] == '.') || p[1] == '\\')
        p = buf + strlen(buf);
    Utils::StrCpy(p, sizeof(buf) - (p - buf), ext);
    return std::string(buf);
}
char* Utils::FullQualify(char* string)
{
    static char buf[265];
    if (string[0] == '\\')
    {
        getcwd(buf, 265);
        Utils::StrCpy(buf + 2, sizeof(buf) - 2, string);
        return buf;
    }
    else if (string[1] != ':')
    {
        char *p, *q = string;
        getcwd(buf, 265);
        p = buf + strlen(buf);
        if (!strncmp(q, ".\\", 2))
            q += 2;
        p--;
        while (!strncmp(q, "..\\", 3))
        {
            q += 3;
            while (p > buf && *p != '\\')
                p--;
            if (p > buf)
                p--;
        }
        p++;
        *p++ = '\\';
        Utils::StrCpy(p, sizeof(buf) - (p - buf), q);
        return buf;
    }
    return string;
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
    while (!fpath.empty())
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
        if (current[current.size()-1] != CmdFiles::DIR_SEP[0])
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
bool Utils::HasLocalExe(const std::string& exeName)
{
    char buf[10000];
    strcpy(buf, GetModuleName());
    char* p = (char *)strrchr(buf, '/');
    char* p1 = (char *)strrchr(buf, '\\');
    if (p1 > p)
        p = p1;
    else if (!p)
        p = p1;
    if (p)
    {
        p++;
    }
    else
        p = buf;
    *p = 0;
    strcat(p, exeName.c_str());
#ifdef WIN32
    strcat(p, ".exe");
#endif
    return _access(buf, 0) == 0;
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

bool Utils::iequal(const std::string& a, const std::string& b, int sz)
{
    if (sz >= 0)
    {
        if (a.size() < sz || b.size() < sz)
            return false;
    }
    else
    {
        sz = a.size();
        if (sz != b.size())
            return false;
    }
    for (size_t i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

FILE* Utils::TempName(std::string& name)
{
    char tempFile[260];
    tempFile[0] = 0;
    tmpnam(tempFile);
    if (tempFile[0] == '\\')
    {
        // fix for buggy mingw on windows
        const char* p = getenv("TMP");
        if (!p)
            p = "";
        StrCpy(tempFile, p);
        tmpnam(tempFile + strlen(tempFile));
    }
    FILE* fil = fopen(tempFile, "w");
    if (!fil)
    {
        StrCpy(tempFile, ".\\");
        tmpnam(tempFile + strlen(tempFile));
        // this next because it apparently isn't standard how to do the return value of tmpnam
        const char *p = strrchr(tempFile, '\\');
        strcpy(tempFile + 2, p);

        fil = fopen(tempFile, "w");
        if (!fil)
        {
            Utils::fatal("TMP environment variable not set or invalid");
        }
    }
    name = tempFile;
    return fil;
}

/*
 * If no extension, add the one specified
 */
void Utils::AddExt(char* buffer, const char* ext)
{
    char* pos = (char *)strrchr(buffer, '.');
    if (!pos || (*(pos - 1) == '.') || (*(pos + 1) == '\\'))
        strcat(buffer, ext);
}

/*
 * Strip extension, if it has one
 */
void Utils::StripExt(char* buffer)
{
    char* pos = (char *)strrchr(buffer, '.');
    if (pos && (*(pos - 1) != '.'))
        *pos = 0;
}

bool Utils::HasExt(const char* buffer, const char* ext)
{
    int l = strlen(buffer), l1 = strlen(ext);
    if (l1 < l)
    {
        return Utils::iequal(buffer + l - l1, ext);
    }
    return 0;
}

bool Utils::FileExists(const char* buffer)
{
#ifdef _WIN32
    return !(GetFileAttributesA(buffer) & FILE_ATTRIBUTE_DIRECTORY);
#else
    return access(buffer, 4) == 0;
#endif
}
std::vector<std::string> Utils::split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}
void Utils::ReplaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t pos(0);
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::string Utils::FindOnPath(const std::string& name, const std::string& path)
{
    std::string hold = path;
    std::string next;
    while (!hold.empty())
    {
        size_t npos = hold.find(";");
        if (npos == std::string::npos)
        {
            next = hold;
            hold = "";
        }
        else
        {
            next = hold.substr(0, npos);
            if (npos + 1 < hold.size())
                hold = hold.substr(npos + 1);
            else
                hold = "";
        }
        std::string name1 = Utils::FullPath(next, name);
        FILE* infile = fopen(name1.c_str(), "rb");
        if (infile)
        {
            fclose(infile);
            return name1;
        }
    }
    return "";
}
