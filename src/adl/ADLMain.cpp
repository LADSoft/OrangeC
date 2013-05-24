/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "CmdSwitch.h"
#include "Utils.h"
#include "XML.h"
#include "ADLMain.h"
#include "Parser.h"
#include "GenParser.h"
#include <fstream>

char *ADLMain::usageText = "[options] inputfile\n"
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
    Utils::banner(argv[0]);
    if (!SwitchParser.Parse(&argc, argv))
    {
        Utils::usage(argv[0], usageText);
    }
    if (argc !=2)
        Utils::usage(argv[0], usageText);
    std::string name = argv[1];
    if (name.find_first_of(".") == std::string::npos)
        name += ".adl";
    std::fstream in(name.c_str(), std::ios::in);
    if (in)
    {
        xmlNode node;
        node.SetStripSpaces(false);
        if (!node.Read(in))
        {
            Utils::fatal("Invalid file format");
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
                Utils::fatal("Expected ADL file");
            }
        }
    }
    else
    {
        Utils::fatal("Cannot open file '%s' for read", argv[1]);
    }
    return 0;
}
