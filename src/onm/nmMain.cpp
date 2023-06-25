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

#include "nmMain.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "SymbolTable.h"
#include "Sorter.h"
#include "PrintFormatter.h"
#include "ObjIeee.h"
#include <cstring>

CmdSwitchParser nmMain::SwitchParser;
CmdSwitchBool nmMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchBool nmMain::filePerLine(SwitchParser, 'A');
CmdSwitchBool nmMain::filePerLine2(SwitchParser, 'o');
CmdSwitchBool nmMain::allSymbols(SwitchParser, 'a');
CmdSwitchBool nmMain::demangle(SwitchParser, 'C');
CmdSwitchBool nmMain::externalSymbols(SwitchParser, 'g');
CmdSwitchBool nmMain::help(SwitchParser, 'h');
CmdSwitchBool nmMain::lineNumbers(SwitchParser, 'l');
CmdSwitchBool nmMain::sortByAddress(SwitchParser, 'n');
CmdSwitchBool nmMain::sortByAddress2(SwitchParser, 'v');
CmdSwitchBool nmMain::noSort(SwitchParser, 'p');
CmdSwitchBool nmMain::reverseSort(SwitchParser, 'r');
CmdSwitchBool nmMain::archiveIndex(SwitchParser, 's');
CmdSwitchString nmMain::radix(SwitchParser, 't');
CmdSwitchBool nmMain::undefinedSymbols(SwitchParser, 'u');
CmdSwitchBool nmMain::reservedP(SwitchParser, 'P');
CmdSwitchBool nmMain::reservedS(SwitchParser, 'S');
CmdSwitchBool nmMain::reservedX(SwitchParser, 'X');

const char* nmMain::helpText =
    "[options] inputfile\n"
    "\n"
    "/a             Show all symbols\n"
    "/g             Show all externals\n"
    "/h             Show this text\n"
    "/l             reserved\n"
    "/n,/v          Sort By Address\n"
    "/p             Don't sort\n"
    "/r             Reverse sort\n"
    "/s             Reserved\n"
    "/tx            Select radix (d,o,x)\n"
    "/u             Show undefined externals\n"
    "/A,/o          Show file on each line\n"
    "/C             Demangle symbol names\n"
    "/P             reserved\n"
    "/S             reserved\n"
    "/X             reserved\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

const char* nmMain::usageText = "[options] inputfile"; 

int main(int argc, char** argv)
{
    nmMain downloader;
    try
    {
        return downloader.Run(argc, argv);
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }

    return 1;
}
int nmMain::Run(int argc, char** argv)
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
    if (!SwitchParser.Parse(&argc, argv) || ((help.GetValue() || argc < 2) && !ShowHelp.GetExists()))
    {
        Utils::usage(argv[0], usageText);
    }
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    CmdFiles files(argv + 1);
    SymbolTable s(allSymbols.GetValue(), externalSymbols.GetValue(), undefinedSymbols.GetValue());
    s.Load(files);

    Sorter sorted(sortByAddress.GetValue() || sortByAddress2.GetValue(), noSort.GetValue(), reverseSort.GetValue());
    sorted.Sort(s);
    PrintFormatter printer(filePerLine.GetValue() || filePerLine2.GetValue(), demangle.GetValue(), lineNumbers.GetValue(),
                           archiveIndex.GetValue(), radix.GetValue());
    printer.Print(s);
    return 0;
}
