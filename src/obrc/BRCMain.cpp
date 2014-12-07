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

int main(int argc, char **argv)
{
    BRCMain librarian;
    return librarian.Run(argc, argv);
}

CmdSwitchParser BRCMain::SwitchParser;

CmdSwitchFile BRCMain::File(SwitchParser, '@');
char *BRCMain::usageText = "[options] outputfile filelist \n"
            "\n"
            "@xxx  Read commands from file\n"
            "\n"
            "Time: " __TIME__ "  Date: " __DATE__;

int BRCMain::Run(int argc, char **argv)
{
    Utils::banner(argv[0]);
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
    if (configTest != NULL)
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv))
    {
        Utils::usage(argv[0], usageText);
    }
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

