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

#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <cstring>
#ifdef TARGET_OS_WINDOWS
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

#include "CmdSwitch.h"
#include "ToolChain.h"

#include <sstream>
#include "../version.h"
#include <iostream>

void ToolChain::ShowBanner()
{
    bool have_version = false;
    // no banner if they specify -!, this is also caught in the cmd switch module
    // so it is transparent to the proggy
    const char* modName = Utils::GetModuleName();
    printf("%s (OrangeC) Version " STRING_VERSION "\n" COPYRIGHT "\n", Utils::ShortName(modName));
}
void ToolChain::ShowVersion()
{
    printf("\nCompile date: " __DATE__ " time: " __TIME__ "\n");
    exit(0);
}
void ToolChain::Usage(const char* text, int exitVal)
{

    int rows = 10000;
#ifdef TARGET_OS_WINDOWS
    if (_isatty(fileno(stderr)))
        rows = ScreenHeight();
#else
    if (_isatty(STDERR_FILENO))
        rows = ScreenHeight();
#endif
    const char* modName = Utils::GetModuleName();
    fprintf(stderr, "\nUsage: %s ", Utils::ShortName(modName));
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
CmdFiles ToolChain::StandardToolStartup(CmdSwitchParser& SwitchParser, int argc, char** argv, const char* usageText,
                                        const char* helpText, std::function<bool()> noBanner)
{
    CmdSwitchBool NoLogo(SwitchParser, '!', false, {"nologo"});
    CmdSwitchBool ShowVersion(SwitchParser, 'v', false, {"version"});
    CmdSwitchBool ShowHelp(SwitchParser, '?', false, {"help"});
    CmdSwitchFile File(SwitchParser, '@');
    CmdSwitchFile internalConfig(SwitchParser);

    Utils::SetEnvironmentToPathParent("ORANGEC");
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    if (access(configName.c_str(), 0) != 0)
    {
        configName = Utils::QualifiedFile(Utils::GetModuleName(), ".cfg");
    }
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::Fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) && !ShowHelp.GetExists())
    {
        if (!noBanner || !noBanner())
            ToolChain::ShowBanner();
        ToolChain::Usage(usageText);
    }
    if (noBanner && noBanner())
        NoLogo.SetValue(true);
    if (!NoLogo.GetValue() || ShowVersion.GetValue())
        ToolChain::ShowBanner();
    if (ShowVersion.GetValue())
        ToolChain::ShowVersion();
    if (ShowHelp.GetExists())
        ToolChain::Usage(helpText);
    auto rv = CmdFiles(argv);
    rv.Add(File);
    return rv;
}

int ToolChain::ScreenHeight()
{
#ifdef TARGET_OS_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize max;
    ioctl(0, TIOCGWINSZ, &max);
    return max.ws_row;
#endif
}
bool ToolChain::GetLine(const char** text, std::string& buf)
{
    if (!**text)
        return false;
    char const* start = *text;
    auto temp = (char*)strchr(*text, '\n');
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
