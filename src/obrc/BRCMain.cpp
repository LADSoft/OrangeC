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

#include "ObjTypes.h"
#include "ObjFactory.h"
#include "ObjIeee.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ToolChain.h"
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
    catch (std::domain_error e)
    {
        std::cout << e.what() << std::endl;
    }
}

CmdSwitchParser BRCMain::SwitchParser;
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
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText);

    // setup
    ObjString outputFile = Utils::QualifiedFile(argv[1], ".obr");

    BRCLoader loader(files);
    bool ok = loader.load();
    if (ok)
    {
        BRCWriter writer(outputFile, loader);
        ok = writer.write();
    }
    return !ok;
}
