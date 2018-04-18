/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "Coff2ieeeMain.h"
#include "CoffFile.h"
#include "CoffLibrary.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjFactory.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

CmdSwitchParser Coff2ieeeMain::SwitchParser;
CmdSwitchCombineString Coff2ieeeMain::outputFileSwitch(SwitchParser, 'o');

char *Coff2ieeeMain::usageText = "[options] <coff file>\n"
            "\n"
            "/oxxx  Set output file name\n"
            "/V     Show version and date\n"
            "\n"
            "\nTime: " __TIME__ "  Date: " __DATE__;
            

int main(int argc, char **argv)
{
    Coff2ieeeMain translator;
    return translator.Run(argc, argv);
}
Coff2ieeeMain::~Coff2ieeeMain()
{
}
std::string Coff2ieeeMain::GetOutputName(char *infile) const
{
    std::string name;
    if (outputFileSwitch.GetValue().size() != 0)
    {
        name = outputFileSwitch.GetValue();
        const char *p = strrchr(name.c_str(), '.');
        if (p  && p[-1] != '.' && p[1] != '\\')
            return name;
    }
    else
    { 
        name = infile;
    }
    if (mode == lib)
        name = Utils::QualifiedFile(name.c_str(), ".l");
    else
        name = Utils::QualifiedFile(name.c_str(), ".o");
    return name;
}			
bool Coff2ieeeMain::GetMode(char *infile) 
{
    std::fstream fil(infile, std::ios::in | std::ios::binary);
    char buf[256];
    fil.read(buf,32);
    if (*(unsigned short *)buf == IMAGE_FILE_MACHINE_I386)
    {
        mode = obj;
        return true;
    }
    if (!strncmp(buf, "!<arch>\n",8))
    {
        mode = lib;
        return true;
    }
    return false;
}
int Coff2ieeeMain::Run(int argc, char **argv)
{
    Utils::banner(argv[0]);
    char *modName = Utils::GetModuleName();
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || argc < 2)
    {
        Utils::usage(argv[0], usageText);
    }
    if (argc != 2)		
    {
        Utils::usage(argv[0], usageText);
    }
    if (!GetMode(argv[1]))
    {
        std::cout << "input file must be a COFF object or library file" << std::endl;
        exit(1);
    }
    if (mode == obj)
    {
        std::cout << "converting object file" << std::endl;
        CoffFile object(argv[1]);
        outputName = GetOutputName(argv[1]);
        if (object.Load())
        {
            ObjIeeeIndexManager im1;
            ObjFactory factory(&im1);
            ObjFile *file = object.ConvertToObject(outputName, factory);
            if (file)
            {
                ObjIeee il = outputName;
    			il.SetTranslatorName("Coff2ieee");
    			il.SetDebugInfoFlag(false);
                FILE *c = fopen(outputName.c_str(), "w");
                il.Write(c, file, &factory);    
                fclose(c);
                return 0;
            }
        }
    }
    else
    {
        std::cout << "converting library file" << std::endl;
        CoffLibrary library(argv[1]);
        outputName = GetOutputName(argv[1]);
        if (library.Load())
        {
            if (library.Convert())
            {
                if (library.Write(outputName))
                    return 0;
            }
        }
    }
    printf("failed");
    return 1;
}
