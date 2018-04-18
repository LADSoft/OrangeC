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
#include "dlMzMain.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include "ObjFactory.h"
#include "ObjFile.h"
#include "MZHeader.h"
#include "OutputFormats.h"

CmdSwitchParser dlMzMain::SwitchParser;

CmdSwitchString dlMzMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlMzMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlMzMain::DebugFile(SwitchParser, 'v');

char *dlMzMain::usageText = "[options] relfile\n"
            "\n"
            "/oxxx  Set output file name\n"
            "/mxxx  Set output file type\n"
            "/V     Show version and date\n"
            "\n"
            "Available output file types:\n"
            "    TINY\n"
            "    REAL (segmented, default)\n"
            "\nTime: " __TIME__ "  Date: " __DATE__;
            

int main(int argc, char **argv)
{
    dlMzMain downloader;
    return downloader.Run(argc, argv);
}
dlMzMain::~dlMzMain()
{
    delete data;
}
bool dlMzMain::GetMode()
{
    mode = UNKNOWN;
    const std::string &val = modeSwitch.GetValue();
    if (val.size() == 0)
    {
        mode = REAL;
    }
    else
    {
        if (val == "TINY")
            mode = TINY;
        else if (val == "REAL")
            mode = REAL;
    }
    return mode != UNKNOWN;
}
bool dlMzMain::ReadSections(const std::string &path)
{
    ObjIeeeIndexManager iml;
    ObjFactory factory(&iml);
    ObjIeee ieee("");
    FILE *in = fopen(path.c_str(), "rb");
    if (!in)
       Utils::fatal("Cannot open input file");
    file = ieee.Read(in, ObjIeee::eAll, &factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        delete file;
        Utils::fatal("Input file is in relative format");
    }
    if (ieee.GetStartAddress() == nullptr)
    {
        delete file;
        Utils::fatal("No start address specified");
    }
    if (file != nullptr)
    {
        if (mode == TINY)
            data = new Tiny();
        else
            data = new Real();
        return data->ReadSections(file, ieee.GetStartAddress());
    }
    return false;
    
}
std::string dlMzMain::GetOutputName(char *infile) const
{
    std::string name;
    if (outputFileSwitch.GetValue().size() != 0)
    {
        name = outputFileSwitch.GetValue();
        const char *p = strrchr(name.c_str(), '.');
        if (p  && p[-1] != '.')
            return name;
    }
    else
    { 
        name = infile;
    }
    if (mode == TINY)
        name = Utils::QualifiedFile(name.c_str(), ".com");
    else
        name = Utils::QualifiedFile(name.c_str(), ".exe");
    return name;
}			
int dlMzMain::Run(int argc, char **argv)
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
    if (!SwitchParser.Parse(&argc, argv) || argc != 2)
    {
        Utils::usage(argv[0], usageText);
    }
    if (!GetMode())
    {
        Utils::usage(argv[0], usageText);
    }
    if (!ReadSections(std::string(argv[1])))
        Utils::fatal("Invalid .rel file");
    std::string outputName = GetOutputName(argv[1]);
    std::fstream out(outputName.c_str(), std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        data->Write(out);
        return !!out.fail();
    }
    else
    {
        return 1;
    }
}
