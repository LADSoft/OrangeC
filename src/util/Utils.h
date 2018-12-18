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

#ifndef UTIL_H
#define UTIL_H

#include <string>

class Utils
{
  public:
    static void banner(const char* progName);
    static void usage(const char* progName, const char* text);
    static void fatal(const char* format)
    {
        fprintf(stderr, "Fatal error: ");
        fputs(format, stderr);
        exit(1);
    }
    template <typename... Args> // templates are MUCH more portable than varargs
    static void fatal(const char* format, Args... arg)
    {
        fprintf(stderr, "Fatal error: ");
        fprintf(stderr, format, arg...);
        fputc('\n', stderr);
        exit(1);
    }
    static void fatal(const std::string& format)
    {
        fatal(format.c_str());
    }
    template <typename... Args>
    static void fatal(const std::string& format, Args... arg)
    {
        fatal(format.c_str(), arg...);
    }
    static char* GetModuleName();
    static void SetEnvironmentToPathParent(const char* name);
    static std::string FullPath(const std::string& path, const std::string& name);
    static std::string QualifiedFile(const char* path, const char* ext);
    static std::string SearchForFile(const std::string& path, const std::string& name);
    static std::string NumberToString(int num);
    static std::string NumberToStringHex(int num);
    static int StringToNumber(std::string str);
    static int StringToNumberHex(std::string str);
    static char* ShortName(const char* v);

    static unsigned PartialCRC32(unsigned crc, unsigned char* data, size_t len);
    static unsigned CRC32(unsigned char* data, size_t len) { PartialCRC32(0, data, len); };
    template <size_t len>
    static unsigned CRC32(unsigned char (&data)[len])
    {
        CRC32(data, len);
    }
    template <size_t len>
    static unsigned PartialCRC32(unsigned crc, unsigned char* data)
    {
        PartialCRC32(crc, data, len);
    }
    static unsigned crctab[256];
};
#endif