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
CmdSwitchBool dlPmMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchString dlPmMain::outputFileSwitch(SwitchParser, 'o');
CmdSwitchString dlPmMain::DebugFile(SwitchParser, 'v');

const char* dlPmMain::helpText =
    "[options] relfile\n"
    "\n"
    "/oxxx          Set ouput file name\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;
const char* dlPmMain::usageText = "[options] relfile";

int main(int argc, char** argv)
{
    dlPmMain downloader;
    try
    {
        return downloader.Run(argc, argv);
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
}
dlPmMain::~dlPmMain() {}
void dlPmMain::GetSectionNames(std::vector<std::string>& names, ObjFile* file)
{
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        names.push_back((*it)->GetName());
    }
}
void dlPmMain::GetInputSections(const std::vector<std::string>& names, ObjFile* file, ObjFactory* factory)
{

    for (auto name : names)
    {
        ObjSection* s = file->FindSection(name);
        ObjInt size = s->GetSize()->Eval(0);
        ObjInt addr = s->GetOffset()->Eval(0);
        sections.push_back(std::make_unique<Section>(addr, size));
        sections.back()->data = std::make_unique<char[]>(size);
        Section* p = sections.back().get();
        s->ResolveSymbols(factory);
        ObjMemoryManager& m = s->GetMemoryManager();
        int ofs = 0;
        for (auto it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
        {
            int msize = (*it)->GetSize();
            ObjByte* mdata = (*it)->GetData();
            if (msize)
            {
                ObjExpression* fixup = (*it)->GetFixup();
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
                            p->data[ofs + 1] = n >> 8;
                        }
                    }
                    else  // msize == 4
                    {
                        if (bigEndian)
                        {
                            p->data[ofs + 0] = n >> 24;
                            p->data[ofs + 1] = n >> 16;
                            p->data[ofs + 2] = n >> 8;
                            p->data[ofs + 3] = n & 0xff;
                        }
                        else
                        {
                            p->data[ofs] = n & 0xff;
                            p->data[ofs + 1] = n >> 8;
                            p->data[ofs + 2] = n >> 16;
                            p->data[ofs + 3] = n >> 24;
                        }
                    }
                }
                else
                {
                    if ((*it)->IsEnumerated())
                        memset(p->data.get() + ofs, (*it)->GetFill(), msize);
                    else
                        memcpy(p->data.get() + ofs, mdata, msize);
                }
                ofs += msize;
            }
        }
    }
}
bool dlPmMain::ReadSections(const std::string& path)
{
    ObjIeeeIndexManager iml;
    ObjFactory factory(&iml);
    ObjIeee ieee("");
    FILE* in = fopen(path.c_str(), "rb");
    if (!in)
        Utils::fatal("Cannot open input file");
    file = ieee.Read(in, ObjIeee::eAll, &factory);
    fclose(in);
    if (!ieee.GetAbsolute())
    {
        Utils::fatal("Input file is in relative format");
    }
    if (ieee.GetStartAddress() == nullptr)
    {
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
std::string dlPmMain::GetOutputName(char* infile) const
{
    std::string name;
    if (outputFileSwitch.GetValue().size() != 0)
    {
        name = outputFileSwitch.GetValue();
        const char* p = strrchr(name.c_str(), '.');
        if (p && p[-1] != '.' && p[1] != '\\')
            return name;
    }
    else
    {
        name = infile;
    }
    name = Utils::QualifiedFile(name.c_str(), ".exe");
    return name;
}
void dlPmMain::LoadVars(ObjFile* file)
{
    for (auto it = file->DefinitionBegin(); it != file->DefinitionEnd(); it++)
    {
        ObjDefinitionSymbol* p = (ObjDefinitionSymbol*)*it;
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
bool dlPmMain::LoadStub(const std::string& exeName)
{
    std::string val = "pmstb.exe";
    // look in current directory
    std::fstream* file = new std::fstream(val, std::ios::in | std::ios::binary);
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
            file = new std::fstream(val, std::ios::in | std::ios::binary);
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
        file->read((char*)&mzHead, sizeof(mzHead));
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
        stubData = std::make_unique<char[]>(stubSize);
        char* pstubData = stubData.get();
        memset(pstubData, 0, stubSize);
        int newSize = bodySize + totalHeader;
        if (newSize & 511)
            newSize += 512;
        mzHead.image_length_MOD_512 = newSize % 512;
        mzHead.image_length_DIV_512 = newSize / 512;
        mzHead.offset_to_relocation_table = 0x40;
        mzHead.n_header_paragraphs = totalHeader / 16;
        memcpy(pstubData, &mzHead, sizeof(mzHead));
        *(unsigned*)(pstubData + 0x3c) = stubSize;
        if (relocSize)
        {
            file->seekg(oldReloc, std::ios::beg);
            file->read(pstubData + 0x40, relocSize);
        }
        file->seekg(oldHeader, std::ios::beg);
        file->read(pstubData + totalHeader, bodySize);
        if (!file->eof() && file->fail())
        {
            delete file;
            return false;
        }
        delete file;
    }
    return true;
}
int dlPmMain::Run(int argc, char** argv)
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
    if (!SwitchParser.Parse(&argc, argv) || (argc != 2 && !ShowHelp.GetExists()))
    {
        Utils::usage(argv[0], usageText);
    }
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    if (!LoadStub(argv[0]))
        Utils::fatal("Missing or invalid stub file");
    if (!ReadSections(std::string(argv[1])))
        Utils::fatal("Invalid .rel file failed to read sections");
    if (sections.size() != 1)
        Utils::fatal("Invalid .rel file invalid section count");
    std::string outputName = GetOutputName(argv[1]);
    std::fstream out(outputName, std::ios::out | std::ios::binary);
    if (!out.fail())
    {
        Section* s = sections[0].get();
        out.write(stubData.get(), stubSize);
        const char* sig = "LSPM";
        unsigned len = 20;  // size of header
        // write header
        out.write(sig, strlen(sig));
        out.write((char*)&len, sizeof(len));
        out.write((char*)&memSize, sizeof(memSize));
        out.write((char*)&s->size, sizeof(s->size));
        out.write((char*)&startAddress, sizeof(startAddress));
        // end of header
        out.write((char*)s->data.get(), s->size);
        out.close();
        return !!out.fail();
    }
    else
    {
        Utils::fatal("Cannot open '%s' for write", outputName.c_str());
    }
    return 1;
}
