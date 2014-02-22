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
            "/c-   Case insensitive library\n"
            "@xxx  Read commands from file\n"
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
    if (argc < 2 && File.GetCount() < 3)
    {
        Utils::usage(argv[0], usageText);
    }
        
    // setup
    ObjString outputFile = argv[1];
    size_t n = outputFile.find_last_of('.');
    if (n == std::string::npos || n != outputFile.size()-1 && outputFile[n+1] != 'l') //FIXME && and || 
    {
        outputFile += ".l";
    }
    
    LibManager librarian(outputFile, caseSensitiveSwitch.GetValue());
    if (librarian.IsOpen())
        if (!librarian.LoadLibrary())
        {
            std::cout << outputFile << " is not a library" << std::endl;
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
