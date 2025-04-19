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

#ifndef TOOLCHAIN_H
#define TOOLCHAIN_H
#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "Utils.h"
#include "CmdFiles.h"
#include <functional>

#ifdef TARGET_OS_WINDOWS
#    define mysystem(x) winsystem(x)
extern "C" int winsystem(const char*);
#else
#    define mysystem(x) system(x)
#endif

class CmdSwitchParser;

class ToolChain
{
  public:
    static CmdFiles StandardToolStartup(CmdSwitchParser& SwitchParser, int argc, char** argv, const char* usageText,
                                        const char* helpText, std::function<bool()> noBanner = nullptr);
    static void ShowBanner();
    static void ShowVersion();
    [[noreturn]] static void Usage(const char* text, int exitVal = 1);
    template <typename... Args>
    static int ToolInvoke(const std::string& exeName, const char* with, const char* fmt, const Args... arg)
    {
        int size = snprintf(NULL, 0, fmt, arg...) + 260;
        char* buf = (char*)calloc(sizeof(char), size);
        buf[0] = '"';
        strcpy(buf + 1, Utils::GetModuleName());
        char* p = (char*)strrchr(buf, '/');
        char* p1 = (char*)strrchr(buf, '\\');
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
        auto rv = mysystem(buf);
        free(buf);
        return rv;
    }

  private:
    static int ScreenHeight();
    static bool GetLine(const char** text, std::string& buf);
};
#endif