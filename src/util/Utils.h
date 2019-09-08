/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include <string>
#include <stdlib.h>
#include <string.h>
class Utils
{
  public:
    static void banner(const char* progName);
    static void usage(const char* progName, const char* text);
    static void fatal(const char* format)
    {
        fprintf(stderr, "Fatal error: ");
        fputs(format, stderr);
        if (cleanup)
            cleanup();
        exit(1);
    }
    template <typename... Args>  // templates are MUCH more portable than varargs
    static void fatal(const char* format, Args... arg)
    {
        fprintf(stderr, "Fatal error: ");
        fprintf(stderr, format, arg...);
        fputc('\n', stderr);
        if (cleanup)
            cleanup();
        exit(1);
    }
    static void fatal(const std::string& format) { fatal(format.c_str()); }
    template <typename... Args>
    static void fatal(const std::string& format, Args... arg)
    {
        fatal(format.c_str(), arg...);
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

    static bool NamedPipe(int *fds, const std::string& name);
    static bool PipeWrite(int fileno, const std::string& data);
    static std::string PipeRead(int fileno);

    static unsigned PartialCRC32(unsigned crc, const unsigned char* data, size_t len);
    static unsigned CRC32(const unsigned char* data, size_t len) { return PartialCRC32(0, data, len); };
    static char* StrCpy(char *data, size_t len, const char* source)
    {
        strncpy(data, source, len);
        data[len - 1] = '\0';
        return data;
    }
    template<size_t len>
    static char *StrCpy(char (&data)[len], const char* source)
    {
        return StrCpy(data, len, source);
    }
    static char* StrCat(char *data, size_t len, const char* source)
    {
        int num = (int)len - (int)strlen(data) - 1;
        if (num > 0)
            strncat(data, source, num);
        return data;
    }
    template<size_t len>
    static char *StrCat(char(&data)[len], const char* source)
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
    static void(*cleanup)();
};
#endif