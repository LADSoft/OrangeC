/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "CmdSwitch.h"
#include "Utils.h"
#include "ToolChain.h"
#include "XML.h"
#include "ADLMain.h"
#include "Loader.h"
#include "GenParser.h"
#include <fstream>
#include <iostream>

const char *ADLMain::usageText = "[options] inputfile\n"
            "\n"
            "  -d    dump database\n"
            "\nTime: " __TIME__ "  Date: " __DATE__;
            
CmdSwitchParser ADLMain::SwitchParser;
CmdSwitchBool ADLMain::DumpDB(SwitchParser, 'd');

int main(int argc, char **argv)
{
    ADLMain Main;
    return Main.Run(argc, argv);
}
ADLMain::~ADLMain()
{
}
int ADLMain::Run(int argc, char **argv)
{
    ToolChain::ShowBanner();
    if (!SwitchParser.Parse(&argc, argv))
    {
        ToolChain::Usage(usageText);
    }
    if (argc !=2)
        ToolChain::Usage(usageText);
    std::string name = argv[1];
    if (name.find_first_of(".") == std::string::npos)
        name += ".adl";
    std::fstream in(name, std::ios::in);
    if (in)
    {
        xmlNode node;
        node.Reset();
        node.SetStripSpaces(false);
        if (!node.Read(in))
        {
            Utils::Fatal("Invalid file format in line %d", node.Line());
        }
        else
        {
            in.close();
            if (node.GetName() == "ADL")
            {
                Parser p;
                node.Visit(p, &p);
                if (DumpDB.GetValue())
                    p.DumpDB();
                if (p.CreateParseTree())
                {
                    GenParser gp(p);
                    gp.Generate();
                }
                else
                {
                    std::cout << "error creating parse tree" << std::endl;
                }
            }
            else
            {
                Utils::Fatal("Expected ADL file");
            }
        }
    }
    else
    {
        Utils::Fatal("Cannot open file '%s' for read", argv[1]);
    }
    return 0;
}
