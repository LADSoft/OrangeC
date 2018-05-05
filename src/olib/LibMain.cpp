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

int main(int argc, char **argv)
{
    LibMain librarian;
    return librarian.Run(argc, argv);
}

CmdSwitchParser LibMain::SwitchParser;

CmdSwitchBool LibMain::caseSensitiveSwitch(SwitchParser, 'c', true);
CmdSwitchOutput LibMain::OutputFile(SwitchParser, 'o', ".a");
CmdSwitchFile LibMain::File(SwitchParser, '@');
char *LibMain::usageText = "[options] libfile [+ files] [- files] [* files]\n"
            "\n"
            "/c-            Case insensitive library\n"
            "/oxxx          Set output file name\n"
            "/V, --version  Show version and date\n"
            "/!             No logo\n"
            "@xxx           Read commands from file\n"
            "\n"
            "Time: " __TIME__ "  Date: " __DATE__;

void LibMain::AddFile(LibManager &librarian, const char *arg)
{
    const char *p = arg;
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
        switch(mode)
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
int LibMain::Run(int argc, char **argv)
{
    Utils::banner(argv[0]);
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
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
    if (argc < 2 && File.GetCount() < 3)
    {
        Utils::usage(argv[0], usageText);
    }
        
    // setup
    ObjString outputFile = argv[1];
    size_t n = outputFile.find_last_of('.');
    if (n == std::string::npos || (n != outputFile.size()-1 && outputFile[n+1] != 'l'))
    {
        outputFile += ".l";
    }
    
    LibManager librarian(outputFile, caseSensitiveSwitch.GetValue());
    if (librarian.IsOpen())
        if (!librarian.LoadLibrary())
        {
            std::cout << outputFile.c_str() << " is not a library" << std::endl;
            return 1;
        }
    for (int i= 2; i < argc; i++)
        AddFile(librarian, argv[i]);
    for (int i = 1; i < File.GetCount(); i++)
        AddFile(librarian, File.GetValue()[i]);
    for (CmdFiles::FileNameIterator it = addFiles.FileNameBegin(); it != addFiles.FileNameEnd(); ++it)
    {
        librarian.AddFile(*(*it));
    }
    for (CmdFiles::FileNameIterator it = replaceFiles.FileNameBegin(); it != replaceFiles.FileNameEnd(); ++it)
    {
        librarian.ReplaceFile(*(*it));
    }
    if (modified)
        if (!librarian.SaveLibrary())
        {
            std::cout << "Error writing library file" << std::endl;
            return 1;
        }
    return 0;
}
