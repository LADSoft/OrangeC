/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "ObjTypes.h"
#include "ObjFactory.h"
#include "ObjIeee.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"
#include "LibManager.h"
#include "CmdSwitch.h"
#include "Utils.h"
#include "LibMain.h"
#include <iostream>

int main(int argc, char** argv)
{
    LibMain librarian;
    try
    {
        return librarian.Run(argc, argv);
    }
    catch (std::ios_base::failure)
    {
        Utils::fatal("Fatal Error...");
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
    return 1;
}

CmdSwitchParser LibMain::SwitchParser;
CmdSwitchBool LibMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchBool LibMain::caseSensitiveSwitch(SwitchParser, 'c', true);
CmdSwitchOutput LibMain::OutputFile(SwitchParser, 'o', ".a");
CmdSwitchFile LibMain::File(SwitchParser, '@');
CmdSwitchBool LibMain::noExport(SwitchParser, 0, false, {"noexports"});
const char* LibMain::helpText =
    "[options] libfile [+ files] [- files] [* files]\n"
    "\n"
    "/c-            Case insensitive library\n"
    "/oxxx          Set output file name\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "@xxx           Read commands from file\n"
    "\n"
    "--noexports    Remove export records\n"
    "\n"
    "Time: " __TIME__ "  Date: " __DATE__;
const char* LibMain::usageText = "[options] libfile [+ files] [- files] [* files]";

void LibMain::AddFile(LibManager& librarian, const char* arg)
{
    const char* p = arg;
    if (p[0] == '+')
    {
        if (p[1] == '-')
        {
            mode = REPLACE;
            p += 2;
        }
        else
        {
            mode = ADD;
            p++;
        }
    }
    else if (p[0] == '-')
    {
        if (p[1] == '+')
        {
            mode = REPLACE;
            p += 2;
        }
        else
        {
            mode = REMOVE;
            p++;
        }
    }
    else if (p[0] == '*')
    {
        mode = EXTRACT;
        p++;
    }
    if (*p)
    {
        std::string name = p;
        switch (mode)
        {
            case ADD:
                modified = true;
                addFiles.Add(name);
                break;
            case REMOVE:
                modified = true;
                changed = true;
                librarian.RemoveFile(name);
                break;
            case REPLACE:
                modified = true;
                changed = true;
                replaceFiles.Add(name);
                break;
            case EXTRACT:
                librarian.ExtractFile(name);
                break;
        }
    }
}
int LibMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv))
    {
        Utils::usage(argv[0], usageText);
    }
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    int fileCount = argc - 1 + File.GetCount() + !!OutputFile.GetValue().size();
    if (fileCount < 2)
    {
        Utils::usage(argv[0], usageText);
    }

    // ar-like behavior for autoconf support
    if (!strcmp(argv[1], "cru"))
    {
        mode = REPLACE;
        memmove(argv + 1, argv + 2, (argc - 1) * sizeof(char*));
        --argc;
    }
    ObjString outputFile = OutputFile.GetValue();
    if (outputFile.empty())
    {
        outputFile = argv[1];
        memmove(argv + 1, argv + 2, (argc - 1) * sizeof(char*));
        --argc;
    }
    // setup
    size_t n = outputFile.find_last_of('.');
    if (n == std::string::npos)
    {
        outputFile += ".l";
    }
    else
    {
        ObjString ext = outputFile.substr(n);
        if (ext != ".l" && ext != ".a" && ext != ".lib")
            outputFile += ".l";
    }

    LibManager librarian(outputFile, noExport.GetValue(), caseSensitiveSwitch.GetValue());
    if (librarian.IsOpen())
        if (!librarian.LoadLibrary())
        {
            std::cout << outputFile << " is not a library" << std::endl;
            return 1;
        }
    for (int i = 1; i < argc; i++)
        AddFile(librarian, argv[i]);
    for (int i = 1; i < File.GetCount(); i++)
        AddFile(librarian, File.GetValue()[i]);
    for (auto it = addFiles.FileNameBegin(); it != addFiles.FileNameEnd(); ++it)
    {
        librarian.AddFile((*it));
    }
    for (auto it = replaceFiles.FileNameBegin(); it != replaceFiles.FileNameEnd(); ++it)
    {
        librarian.ReplaceFile((*it));
    }
    if (modified)
        switch (librarian.SaveLibrary())
        {
            case LibManager::CANNOT_CREATE:
                std::cout << "Cannot open library file for 'write' access" << std::endl;
                return 1;
            case LibManager::CANNOT_WRITE:
                std::cout << "Error while writing library file" << std::endl;
                return 1;
            case LibManager::CANNOT_READ:
                std::cout << "Error while reading input files" << std::endl;
                return 1;
            default:
                break;
        }
    return 0;
}
