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
#include "dlPmMain.h"
#include "CmdSwitch.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjIeee.h"
#include "ObjExpression.h"
#include "MZHeader.h"

CmdSwitchParser dlPmMain::SwitchParser;

CmdSwitchString dlPmMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlPmMain::DebugFile(SwitchParser, 'v');

char *dlPmMain::usageText = "[options] relfile\n"
            "\n"
            "/oxxx  Set ouput file name\n"
            "/V     Show version and date\n"
            "/!     No logo\n"
            "\n"
            "\nTime: " __TIME__ "  Date: " __DATE__;
            

int main(int argc, char **argv)
{
    dlPmMain downloader;
    return downloader.Run(argc, argv);
}
dlPmMain::~dlPmMain()
{
    for (int i=0; i < sections.size(); i++)
        delete sections[i];
    delete [] stubData;
}
void dlPmMain::GetSectionNames(std::vector<std::string> &names, ObjFile *file)
{
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        names.push_back((*it)->GetName());
    }
}
void dlPmMain::GetInputSections(const std::vector<std::string> &names, ObjFile *file, ObjFactory *factory)
{

    for (auto name : names)
    {
        ObjSection *s = file->FindSection(name);
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
bool dlPmMain::ReadSections(const std::string &path)
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
    startAddress = ieee.GetStartAddress()->Eval(0);
    if (file != nullptr)
    {
        LoadVars(file);
        std::vector<std::string> names;
        GetSectionNames(names, file);
        GetInputSections(names, file, &factory);
        return true;
    }
    return false;
    
}
std::string dlPmMain::GetOutputName(char *infile) const
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
    name = Utils::QualifiedFile(name.c_str(), ".exe");
    return name;
}			
void dlPmMain::LoadVars(ObjFile *file)
{
    for (ObjFile::SymbolIterator it = file->DefinitionBegin(); it != file->DefinitionEnd(); it++)
    {
        ObjDefinitionSymbol *p = (ObjDefinitionSymbol *)*it;
        if (p->GetName() == "STACKTOP")
        {
            memSize = p->GetValue();
        }
        else if (p->GetName() == "INITSIZE")
        {
            initSize = p->GetValue();
        }
    }
}
bool dlPmMain::LoadStub(const std::string &exeName)
{
    std::string val = "pmstb.exe";
    // look in current directory
    std::fstream *file = new std::fstream(val.c_str(), std::ios::in | std::ios::binary);
    if (file == nullptr || !file->is_open())
    {
        if (file)
        {
            delete file;
            file = nullptr;
        }
        // look in exe directory if not there
        int npos = exeName.find_last_of(CmdFiles::DIR_SEP);
        if (npos != std::string::npos)
        {
            val = exeName.substr(0, npos + 1) + "..\\lib\\" + val;
            file = new std::fstream(val.c_str(), std::ios::in | std::ios::binary);
        }
    }
    if (file == nullptr || !file->is_open())
    {
        if (file)
        {
            delete file;
            file = nullptr;
        }
        return false;
    }
    else
    {
        MZHeader mzHead;
        file->read((char *)&mzHead, sizeof(mzHead));
        int bodySize = mzHead.image_length_MOD_512 + mzHead.image_length_DIV_512 * 512;
        int oldReloc = mzHead.offset_to_relocation_table;
        int oldHeader = mzHead.n_header_paragraphs * 16;
        if (bodySize & 511)
            bodySize -= 512;
        bodySize -= oldHeader;
        int relocSize = mzHead.n_relocation_items * 4;
        int preHeader = 0x40;
        int totalHeader = (preHeader + relocSize + 15) & ~15;
        stubSize = (totalHeader + bodySize + 15) & ~15;
        stubData = new char [stubSize];
        memset(stubData, 0, stubSize);
        int newSize = bodySize + totalHeader;
        if (newSize & 511)
            newSize += 512;
        mzHead.image_length_MOD_512 = newSize % 512;
        mzHead.image_length_DIV_512 = newSize / 512;
        mzHead.offset_to_relocation_table = 0x40;
        mzHead.n_header_paragraphs = totalHeader/ 16;
        memcpy(stubData, &mzHead, sizeof(mzHead));
        *(unsigned *)(stubData + 0x3c) = stubSize;
        if (relocSize)
        {
            file->seekg(oldReloc, std::ios::beg);
            file->read(stubData + 0x40, relocSize);
        }
        file->seekg(oldHeader, std::ios::beg);
        file->read(stubData + totalHeader, bodySize);
        if (!file->eof() && file->fail())
        {
            delete file;
            return false;
        }
        delete file;
    }
    return true;
}
int dlPmMain::Run(int argc, char **argv)
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
    if (!LoadStub(argv[0]))
        Utils::fatal("Missing or invalid stub file");
    if (!ReadSections(std::string(argv[1])))
        Utils::fatal("Invalid .rel file");
    if (sections.size() != 1)
        Utils::fatal("Invalid .rel file");
    std::string outputName = GetOutputName(argv[1]);
    std::fstream out(outputName.c_str(), std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        Section *s = sections[0];
        out.write(stubData, stubSize);
        char *sig = "LSPM";
        unsigned len = 20; // size of header
        // write header
        out.write(sig, strlen(sig));
        out.write((char *)&len, sizeof(len));
        out.write((char *)&memSize, sizeof(memSize));
        out.write((char *)&s->size, sizeof(s->size));
        out.write((char *)&startAddress, sizeof(startAddress));
        // end of header
        out.write((char *)s->data, s->size);
        out.close();
        return !!out.fail();
    }
    else
    {
        return 1;
    }
}
