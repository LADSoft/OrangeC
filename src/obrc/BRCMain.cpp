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
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "BRCMain.h"
#include "BRCDictionary.h"
#include "BRCWriter.h"

int main(int argc, char** argv)
{
    BRCMain librarian;
    try
    {
        return librarian.Run(argc, argv);
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
}

CmdSwitchParser BRCMain::SwitchParser;
CmdSwitchBool BRCMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchFile BRCMain::File(SwitchParser, '@');
const char* BRCMain::helpText =
    "[options] outputfile filelist \n"
    "\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "@xxx           Read commands from file\n"
    "\n"
    "Time: " __TIME__ "  Date: " __DATE__;
const char* BRCMain::usageText = "[options] outputfile filelist";

int BRCMain::Run(int argc, char** argv)
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
    if (argc < 2 || (argc == 2 && File.GetCount() < 2))
    {
        Utils::usage(argv[0], usageText);
    }

    // setup
    ObjString outputFile = Utils::QualifiedFile(argv[1], ".obr");

    CmdFiles files(argv + 2);
    if (File.GetCount())
        files.Add(File.GetValue() + 1);
    BRCLoader loader(files);
    bool ok = loader.load();
    if (ok)
    {
        BRCWriter writer(outputFile, loader);
        ok = writer.write();
    }
    return !ok;
}
