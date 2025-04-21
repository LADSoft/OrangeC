/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#ifndef UTIL_H
#define UTIL_H
#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>

#ifdef TARGET_OS_WINDOWS
#    include "io.h"
#endif
#include <sys/stat.h>

#define MAINTRY try
#define MAINCATCH                                              \
catch (std::exception& e)                                      \
{                                                              \
     printf("Fatal: caught: %s\n", e.what());                  \
     return 3;                                                 \
}                                                              \
catch (...)                                                    \
{                                                              \
    printf("Fatal: caught something...");                      \
    return 3;                                                  \
}

class Utils
{
  public:
    [[noreturn]] static void Fatal(const char* format)
    {
        fprintf(stderr, "Fatal error: ");
        fputs(format, stderr);
        if (cleanup)
            cleanup();
        exit(1);
    }
    template <typename... Args>  // templates are MUCH more portable than varargs
    [[noreturn]] static void Fatal(const char* format, Args... arg)
    {
        fprintf(stderr, "Fatal error: ");
        fprintf(stderr, format, arg...);
        fputc('\n', stderr);
        if (cleanup)
            cleanup();
        exit(1);
    }
    [[noreturn]] static void Fatal(const std::string& format) { Fatal(format.c_str()); }
    template <typename... Args>
    [[noreturn]] static void Fatal(const std::string& format, Args... arg)
    {
        Fatal(format.c_str(), arg...);
    }
    static bool HasLocalExe(const std::string& exeName);
    static size_t file_size(const std::string& file)
    {
        struct stat stats;
        stat(file.c_str(), &stats);
        return stats.st_size;
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
    static bool FileExists(const std::string& buffer) { return FileExists(buffer.c_str()); }
    static std::string FindOnPath(const std::string& name, const std::string& path);
    static std::vector<std::string> split(const std::string& strToSplit, char delimeter = ';');
    static void ReplaceAll(std::string& str, const std::string& from, const std::string& to);
    static std::string ConvertWStringToString(std::wstring str)
    {
        char intStr[256];
        size_t sz = wcstombs(nullptr, str.c_str(), 256);
        sz = wcstombs(intStr, str.c_str(), sz);
        return std::string(intStr, sz);
    }
    static bool NamedPipe(int* fds, const std::string& name);
    static bool PipeWrite(int fileno, const std::string& data);
    static std::string PipeRead(int fileno);

    static unsigned PartialCRC32(unsigned crc, const unsigned char* data, size_t len);
    static unsigned CRC32(const unsigned char* data, size_t len) { return PartialCRC32(0, data, len); };

    static unsigned Random(unsigned);

    static char* StrCpy(char* data, int len, const char* source)
    {
        if (len <= 0)
            return data;
        int n = strlen(source);
        if (n > len-1)
            n = len-1;
        memcpy(data, source, n);
        data[n] = '\0';
        return data;
    }
    template <int len>
    static char* StrCpy(char (&data)[len], const char* source)
    {
        return StrCpy(data, len, source);
    }
    static char* StrCat(char* data, int len, const char* source)
    {
        if (len <= 0)
            return data;
        int num = (int)len - (int)strlen(data) - 1;
        if (num > 0)
            strncat(data, source, num);
        return data;
    }
    template <int len>
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
    static void (*cleanup)();
};
#endif