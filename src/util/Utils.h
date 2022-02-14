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

#ifndef UTIL_H
#define UTIL_H
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>

#ifdef _WIN32
#    include "io.h"
#    define mysystem(x) winsystem(x)
extern "C" int winsystem(const char*);
#else
#    define mysystem(x) system(x)
#endif

class Utils
{
  public:
    static void banner(const char* progName);
    static void usage(const char* progName, const char* text);
    [[noreturn]] static void fatal(const char* format)
    {
        fprintf(stderr, "Fatal error: ");
        fputs(format, stderr);
        if (cleanup)
            cleanup();
        exit(1);
    }
    template <typename... Args>  // templates are MUCH more portable than varargs
    [[noreturn]] static void fatal(const char* format, Args... arg)
    {
        fprintf(stderr, "Fatal error: ");
        fprintf(stderr, format, arg...);
        fputc('\n', stderr);
        if (cleanup)
            cleanup();
        exit(1);
    }
    [[noreturn]] static void fatal(const std::string& format) { fatal(format.c_str()); }
    template <typename... Args>
    [[noreturn]] static void fatal(const std::string& format, Args... arg)
    {
        fatal(format.c_str(), arg...);
    }
    static bool HasLocalExe(const std::string& exeName);
    template <typename... Args>
    static int ToolInvoke(const std::string& exeName, const char* with, const char* fmt, const Args... arg)
    {
        char buf[10000];
        memset(buf, 0, sizeof(buf));
        buf[0] = '"';
        strcpy(buf + 1, GetModuleName());
        char* p = strrchr(buf, '/');
        char* p1 = strrchr(buf, '\\');
        if (p1 > p)
            p = p1;
        else if (!p)
            p = p1;
        if (p)
        {
            p++;
        }
        else
        {
            p = buf + 1;
        }
        *p = 0;
        strcat(p, exeName.c_str());
        strcat(p, "\" ");
        sprintf(buf + strlen(buf), fmt, arg...);
        if (with)
        {
            printf("%s\n", buf);
            if (with[0])
                printf("   with %s", with);
        }
        return mysystem(buf);
    }
    static void SetCleanup(void(Cleanup)()) { cleanup = Cleanup; }
    static char* GetModuleName();
    static void SetEnvironmentToPathParent(const char* name);
    static std::string FullPath(const std::string& path, const std::string& name);
    static std::string AbsolutePath(const std::string& name);
    static std::string QualifiedFile(const char* path, const char* ext);
    static char* FullQualify(char* string);
    static std::string SearchForFile(const std::string& path, const std::string& name);
    static std::string NumberToString(int num);
    static std::string NumberToStringHex(int num);
    static int StringToNumber(std::string str);
    static int StringToNumberHex(std::string str);
    static char* ShortName(const char* v);
    static bool iequal(const std::string& left, const std::string& right, int sz = -1);
    static FILE* TempName(std::string& name);
    static void AddExt(char* buffer, const char* ext);
    static void StripExt(char* buffer);
    static bool HasExt(const char* buffer, const char* ext);
    static bool FileExists(const char* buffer);
    static std::string FindOnPath(const std::string& name, const std::string& path);
    static std::vector<std::string> split(std::string strToSplit, char delimeter = ';');
    static void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

    static bool NamedPipe(int* fds, const std::string& name);
    static bool PipeWrite(int fileno, const std::string& data);
    static std::string PipeRead(int fileno);

    static unsigned PartialCRC32(unsigned crc, const unsigned char* data, size_t len);
    static unsigned CRC32(const unsigned char* data, size_t len) { return PartialCRC32(0, data, len); };

    static unsigned Random(unsigned);

    static char* StrCpy(char* data, size_t len, const char* source)
    {
        strncpy(data, source, len);
        data[len - 1] = '\0';
        return data;
    }
    template <size_t len>
    static char* StrCpy(char (&data)[len], const char* source)
    {
        return StrCpy(data, len, source);
    }
    static char* StrCat(char* data, size_t len, const char* source)
    {
        int num = (int)len - (int)strlen(data) - 1;
        if (num > 0)
            strncat(data, source, num);
        return data;
    }
    template <size_t len>
    static char* StrCat(char (&data)[len], const char* source)
    {
        return StrCat(data, len, source);
    }
    template <size_t len>
    static unsigned CRC32(const unsigned char (&data)[len])
    {
        return CRC32(data, len);
    }
    template <size_t len>
    static unsigned PartialCRC32(unsigned crc, const unsigned char* data)
    {
        return PartialCRC32(crc, data, len);
    }
    static unsigned crctab[256];

  private:
    static int ScreenHeight();
    static bool GetLine(const char** text, std::string& buf);
    static void (*cleanup)();
};
#endif