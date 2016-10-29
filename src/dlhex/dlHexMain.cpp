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
#include "dlHexMain.h"
#include "CmdSwitch.h"
#include "Utils.h"
#include "OutputObjects.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>

CmdSwitchParser dlHexMain::SwitchParser;

CmdSwitchString dlHexMain::modeSwitch(SwitchParser, 'm');
CmdSwitchString dlHexMain::sectionsSwitch(SwitchParser, 'c');
CmdSwitchHex dlHexMain::padSwitch(SwitchParser,'p', -1, 0, 0x100);
CmdSwitchString dlHexMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlHexMain::DebugFile(SwitchParser, 'v');

char *dlHexMain::usageText = "[options] relfile\n"
            "\n"
            "/cxxx Specify sections\n"
            "/mxxx Set output file type\n"
            "/oxxx Set ouput file name\n"
            "/p:xx Set pad value\n"
            "\n"
            "Available output file types:\n"
            "   M# Motorola Hex (# = 1,2,3 sets srecord type)\n"
            "   I# Intel Hex    (# = 1,2,4)\n"
            "   B  Binary file  (default)\n"
            "\nTime: " __TIME__ "  Date: " __DATE__;
            

int main(int argc, char **argv)
{
    dlHexMain downloader;
    return downloader.Run(argc, argv);
}
dlHexMain::~dlHexMain()
{
    for (int i=0; i < sections.size(); i++)
        delete sections[i];
}
void dlHexMain::GetSectionNames(std::vector<std::string> &names, ObjFile *file)
{
    if (sectionsSwitch.GetValue().size())
    {
        const char *p = sectionsSwitch.GetValue().c_str();
        char buf[256], *q = buf;
        while (*p)
        {
            if (*p == ',')
            {
                if (q - buf != 0)
                {
                    *q = 0;
                    q = buf;
                    std::string name = buf;
                    if (file->FindSection(name))
                        names.push_back(name);
                    else
                        std::cout << "Warning: Section '" << name.c_str() << "' not in .rel file" << std::endl;
                }
                p++;
            }
            else
                *q++ = *p++;
        }
        if (q - buf != 0)
        {
            *q = 0;
            std::string name = buf;
            if (file->FindSection(name))
                names.push_back(name);
            else
                std::cout << "Warning: Section '" << name.c_str() << "' not in .rel file" << std::endl;
        }
    }
    else
    {
        for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
        {
            names.push_back((*it)->GetName());
        }
    }
}
void dlHexMain::GetInputSections(const std::vector<std::string> &names, ObjFile *file, ObjFactory *factory)
{
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
    {
        ObjSection *s = file->FindSection((*it));
        ObjInt size = s->GetSize()->Eval(0);
        ObjInt addr = s->GetOffset()->Eval(0);
        Section *p = new Section(addr, size);
        p->data = new char[size];
        sections.push_back(p);
        s->ResolveSymbols(factory);
        ObjMemoryManager &m = s->GetMemoryManager();
        int ofs = 0;
        for (ObjMemoryManager::MemoryIterator it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
        {
            int msize = (*it)->GetSize();
            ObjByte *mdata = (*it)->GetData();
            if (msize)
            {
                ObjExpression *fixup = (*it)->GetFixup();
                if (fixup)
                {
                    int sbase = s->GetOffset()->Eval(0);
                    int n = fixup->Eval(sbase + ofs);
                    int bigEndian = file->GetBigEndian();
                    if (msize == 1)
                    {
                        p->data[ofs] = n & 0xff;
                    }
                    else if (msize == 2)
                    {
                        if (bigEndian)
                        {
                            p->data[ofs] = n >> 8;
                            p->data[ofs + 1] = n & 0xff;
                        }
                        else
                        {
                            p->data[ofs] = n & 0xff;
                            p->data[ofs+1] = n >> 8;
                        }
                    }
                    else // msize == 4
                    {
                        if (bigEndian)
                        {
                            p->data[ofs + 0] = n >> 24;
                            p->data[ofs + 1] = n >> 16;
                            p->data[ofs + 2] = n >>  8;
                            p->data[ofs + 3] = n & 0xff;
                        }
                        else
                        {
                            p->data[ofs] = n & 0xff;
                            p->data[ofs+1] = n >> 8;
                            p->data[ofs+2] = n >> 16;
                            p->data[ofs+3] = n >> 24;
                        }
                    }
                }
                else
                {
                    if ((*it)->IsEnumerated())
                        memset(p->data + ofs, (*it)->GetFill(), msize);
                    else
                        memcpy(p->data + ofs, mdata, msize);
                }
                ofs += msize;
            }
        }
    }
}
bool dlHexMain::ReadSections(const std::string &path)
{
    ObjIeeeIndexManager iml;
    ObjFactory factory(&iml);
    ObjIeee ieee("");
    FILE *in = fopen(path.c_str(), "rb");
    if (!in)
       Utils::fatal("Cannot open input file");
    ObjFile *file = ieee.Read(in, ObjIeee::eAll, &factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        delete file;
        Utils::fatal("Input file is in relative format");
    }
    if (file != NULL)
    {
        std::vector<std::string> names;
        GetSectionNames(names, file);
        GetInputSections(names, file, &factory);
        return true;
    }
    return false;
    
}
bool dlHexMain::GetOutputMode()
{
    const std::string &modeStr = modeSwitch.GetValue();
    if (modeStr.size() == 0)
    {
        outputMode = eBinary;
        return true;
    }
    switch(modeStr.c_str()[0])
    {
        case 'm':
        case 'M':
            outputMode = eMotorola;
            extraMode = atoi(modeStr.c_str()+1);
            if (extraMode < 1 || extraMode > 3)
                return false;
            break;
        case 'i':
        case 'I':		
            outputMode = eIntel;
            extraMode = atoi(modeStr.c_str()+1);
            if (extraMode != 1 && extraMode != 2 && extraMode != 4)
                return false;
            break;
        case 'b':
        case 'B':
            if (modeStr.size() != 1)
                return false;
            outputMode = eBinary;
            break;
        default:
            return false;
    }
    return true;
}
std::string dlHexMain::GetOutputName(char *infile) const
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
    switch(outputMode)
    {
        case eBinary:
            name = Utils::QualifiedFile(name.c_str(), ".bin");
            break;
        case eMotorola:
            name = Utils::QualifiedFile(name.c_str(), ".s19");
            break;
        case eIntel:
            name = Utils::QualifiedFile(name.c_str(), ".hex");
            break;
        default:
            name = Utils::QualifiedFile(name.c_str(), "");
            break;
    }
    return name;
}			
int dlHexMain::Run(int argc, char **argv)
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
    if (!SwitchParser.Parse(&argc, argv) || argc != 2 || !GetOutputMode())
    {
        Utils::usage(argv[0], usageText);
    }
    if (!ReadSections(std::string(argv[1])))
        Utils::fatal("Invalid .rel file");
        
    std::string outputName = GetOutputName(argv[1]);
    OutputObject *o = NULL;
    switch(outputMode)
    {
        case eBinary:
            o = new BinaryOutputObject;
            break;
        case eIntel:
            o = new IntelOutputObject(extraMode);
            break;
        case eMotorola:
            o = new MotorolaOutputObject(outputName, extraMode);
            break;
    }
    std::fstream out(outputName.c_str(), o->GetOpenFlags());
    if (!out.fail())
    {
        int addr = 0;
        o->WriteHeader(out);
        for (int i=0; i < sections.size(); i++)
        {
            Section *s = sections[i];
            if (padSwitch.GetValue() >= 0 && s->address > addr)
                o->Pad(out, addr, s->address - addr, padSwitch.GetValue());
            o->Write(out, s->data, s->size, s->address);
            addr = s->address + s->size;
        }
        o->WriteTrailer(out);
        out.close();
        return !!out.fail();
    }
    else
    {
        return 1;
    }
}
