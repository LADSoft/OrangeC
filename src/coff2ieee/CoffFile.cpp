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

#include "CoffFile.h"
#include "ObjFactory.h"
#include "ObjExpression.h"
#include "ObjSection.h"
#include "ObjSymbol.h"
#include "ObjIeee.h"
#include "Utils.h"

CoffFile::~CoffFile() {}
bool CoffFile::Load()
{
    if (!name.empty())
        inputFile = std::make_unique<std::fstream>(name, std::ios::binary | std::ios::in);
    if (inputFile && inputFile->is_open())
    {
        inputFile->read((char*)&header, sizeof(header));
        if (!inputFile->fail() && !inputFile->eof())
        {
            if (header.Machine == IMAGE_FILE_MACHINE_I386)
            {
                sections = std::make_unique<CoffSection[]>(header.NumberOfSections + 1);
                inputFile->read((char*)sections.get(), header.NumberOfSections * sizeof(CoffSection));
                if (!inputFile->fail() && !inputFile->eof())
                {
                    inputFile->seekg(header.PointerToSymbolTable + libOffset);
                    symbols = std::make_unique<CoffSymbol[]>(header.NumberOfSymbols + 2);
                    inputFile->read((char*)symbols.get(), header.NumberOfSymbols * sizeof(CoffSymbol));
                    if (!inputFile->fail() && !inputFile->eof())
                    {
                        unsigned size;
                        inputFile->read((char*)&size, sizeof(size));
                        strings = std::make_unique<char[]>(size);
                        memcpy(strings.get(), &size, sizeof(size));
                        inputFile->read(strings.get() + sizeof(size), size - sizeof(size));
                        if (!inputFile->fail())
                        {
                            int i;
                            inputFile->clear();
                            for (i = 0; i < header.NumberOfSections; i++)
                            {
                                if (sections[i].NumberOfRelocations)
                                {
                                    CoffReloc* thisReloc;
                                    int relocCount = sections[i].NumberOfRelocations;
                                    if (sections[i].Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL)
                                    {
                                        if (sections[i].NumberOfRelocations != (short)0xffff)
                                        {
                                            std::cout << "Warning: Reloc overflow not handled properly" << std::endl;
                                        }
                                        CoffReloc temp;
                                        inputFile->seekg(sections[i].PointerToRelocations + libOffset);
                                        inputFile->read((char*)&temp, sizeof(CoffReloc));
                                        relocCount = temp.VirtualAddress + 1;
                                    }

                                    relocs.push_back(std::make_unique<CoffReloc[]>(relocCount));
                                    thisReloc = relocs.back().get();
                                    inputFile->seekg(sections[i].PointerToRelocations + libOffset);
                                    inputFile->read((char*)thisReloc, relocCount * sizeof(CoffReloc));
                                    if (inputFile->fail())
                                        break;
                                    inputFile->clear();
                                }
                                else
                                {
                                    relocs.push_back(nullptr);
                                }
                            }
                            if (i >= header.NumberOfSections)
                            {
                                return ScanSymbols() && AddComdefs();
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
bool CoffFile::ScanSymbols()
{
    sectionSymbols.resize(header.NumberOfSections + 1);
    for (int i = 0; i < header.NumberOfSymbols; i += symbols[i].NumberOfAuxSymbols + 1)
    {
        if ((symbols[i].StorageClass == IMAGE_SYM_CLASS_STATIC || symbols[i].StorageClass == IMAGE_SYM_CLASS_SECTION) &&
            symbols[i].SectionNumber)
        {
            int n = strlen(symbols[i].Name);
            if (n)
            {
                if (n > 8)
                    n = 8;
                if (!strncmp(symbols[i].Name, sections[symbols[i].SectionNumber - 1].Name, n))
                {
                    sectionSymbols[symbols[i].SectionNumber - 1] = (symbols.get() + i);
                }
            }
        }
    }
    return true;
}
bool CoffFile::AddComdefs()
{
    int n = header.NumberOfSections;
    for (int i = 1; i < header.NumberOfSymbols; i += symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].SectionNumber == 0 && symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL && symbols[i].Value)
        {
            symbols[i].SectionNumber = ++n;
        }
    }
    return true;
}
std::string CoffFile::GetSectionName(int sect)
{
    if (sectionSymbols[sect] && ((CoffSectionAux*)&sectionSymbols[sect][1])->Selection > 1)
    {
        std::string name;
        CoffSymbol* sym = sectionSymbols[sect] + sectionSymbols[sect]->NumberOfAuxSymbols + 1;
        while (sym < symbols.get() + header.NumberOfSymbols)
        {
            if (sym->SectionNumber == sect + 1)
                break;
            sym += sym->NumberOfAuxSymbols + 1;
        }
        if (sym > symbols.get() + header.NumberOfSymbols)
        {
            Utils::Fatal("Comdat symbol name not found, exiting");
        }
        if (*(unsigned*)sym->Name == 0)
        {
            name = strings.get() + *(unsigned*)(sym->Name + 4);
        }
        else
        {
            name = sym->Name;
            if (name.size() > 8)
                name = name.substr(0, 8);
        }
        if (sections[sect].Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE))
            return "vsc" + name;
        else
            return "vsd" + name;
    }

    if (sections[sect].Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE))
    {
        return "code";
    }
    if (!strcmp(sections[sect].Name, ".string"))
    {
        return "string";
    }
    if (sections[sect].Characteristics & IMAGE_SCN_MEM_READ)
    {
        if (sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE)
        {
            if (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
                return "const";
            else if (sections[sect].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
                return "data";
        }
        else
        {
            if (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
                return "bss";
        }
    }
    return "data";
}
ObjInt CoffFile::GetSectionQualifiers(int sect)
{
    if (sectionSymbols[sect] && ((CoffSectionAux*)&sectionSymbols[sect][1])->Selection > 1)
    {
        ObjInt sel = 0;
        switch (((CoffSectionAux*)&sectionSymbols[sect][1])->Selection)
        {
            //            case 1:
            //                sel = ObjSection::unique;
            break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                sel = ObjSection::max;
                break;
            default:
                sel = ObjSection::max;
                std::cout << "warning: unknown comdat style #" << (int)((CoffSectionAux*)&sectionSymbols[sect][1])->Selection
                          << std::endl;
                break;
        }
        if (sections[sect].Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE))
            return ObjSection::rom | sel | ObjSection::virt;
        else
            return ObjSection::ram | sel | ObjSection::virt;
    }

    if (sections[sect].Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE))
    {
        return ObjSection::rom;
    }
    if (!strcmp(sections[sect].Name, ".string"))
    {
        return ObjSection::ram;
    }
    if (sections[sect].Characteristics & IMAGE_SCN_MEM_READ)
    {
        if (sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE)
        {
            if (sections[sect].Characteristics & (IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_CNT_INITIALIZED_DATA))
                return ObjSection::ram;
        }
        else
        {
            if (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
                return ObjSection::rom;
        }
    }
    return ObjSection::ram;
}
ObjFile* CoffFile::ConvertToObject(std::string outputName, ObjFactory& factory)
{
    ObjFile* fil = new ObjFile(outputName);
    std::vector<ObjSection*> objectSections;
    std::map<int, ObjSymbol*> externalMapping;
    // Create the sections;
    for (int i = 0; i < header.NumberOfSections; i++)
    {
        if (!(sections[i].Characteristics & (IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_LNK_INFO)) &&
            Utils::iequal(sections[i].Name, ".debug", 6))
        {
            std::string sectname = GetSectionName(i);
            ObjSection* sect = factory.MakeSection(sectname);
            if (sectionSymbols[i])
            {
                CoffSectionAux* aux = (CoffSectionAux*)&sectionSymbols[i][1];
                sect->SetSize(new ObjExpression(aux->Length));
            }
            sect->SetAlignment(IMAGE_SCN_ALIGN(sections[i].Characteristics));
            sect->SetQuals(GetSectionQualifiers(i));
            fil->Add(sect);
            objectSections.push_back(sect);
        }
        else
        {
            objectSections.push_back(nullptr);
        }
    }
    // dump comdefs
    for (int i = 1; i < header.NumberOfSymbols; i += symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].SectionNumber > header.NumberOfSections && symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL &&
            symbols[i].Value)
        {
            char* sname = symbols[i].Name;
            std::string symbolName;
            if (*(unsigned*)sname == 0)
            {
                symbolName = strings.get() + *(unsigned*)(sname + 4);
            }
            else
            {
                symbolName = symbols[i].Name;
                if (symbolName.size() > 8)
                    symbolName = symbolName.substr(0, 8);
            }
            symbolName = std::string("vsb") + symbolName;
            ObjSection* sect = factory.MakeSection(symbolName);
            sect->SetSize(new ObjExpression(symbols[i].Value));
            sect->SetAlignment(8);
            sect->SetQuals(ObjSection::ram | ObjSection::max | ObjSection::virt);
            fil->Add(sect);
            objectSections.push_back(sect);
        }
    }
    // create the symbols
    for (int i = 0; i < header.NumberOfSymbols; i += symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL && symbols[i].SectionNumber <= header.NumberOfSections)
        {
            if (symbols[i].SectionNumber <= 0 ||
                (sectionSymbols[i] && (((CoffSectionAux*)&sectionSymbols[symbols[i].SectionNumber - 1][1])->Selection <= 1)))
            {
                char* sname = symbols[i].Name;
                std::string symbolName;
                if (*(unsigned*)sname == 0)
                {
                    symbolName = strings.get() + *(unsigned*)(sname + 4);
                    if (symbolName == "_WinMain@16")
                        symbolName = "WinMain";
                }
                else
                {
                    symbolName = symbols[i].Name;
                    if (symbolName.size() > 8)
                        symbolName = symbolName.substr(0, 8);
                }
                if (symbols[i].SectionNumber == -1)
                {
                    ObjSymbol* symbol = factory.MakePublicSymbol(symbolName);
                    ObjExpression* exp = new ObjExpression(symbols[i].Value);
                    symbol->SetOffset(exp);
                    fil->Add(symbol);
                }
                else if (symbols[i].SectionNumber)
                {
                    ObjSymbol* symbol = factory.MakePublicSymbol(symbolName);
                    ObjExpression* exp =
                        new ObjExpression(ObjExpression::eAdd, new ObjExpression(objectSections[symbols[i].SectionNumber - 1]),
                                          new ObjExpression(symbols[i].Value));
                    symbol->SetOffset(exp);
                    fil->Add(symbol);
                }
                else
                {
                    ObjSymbol* symbol = factory.MakeExternalSymbol(symbolName);
                    externalMapping[i] = symbol;
                    fil->Add(symbol);
                }
            }
        }
    }
    // dump data to the sections
    for (int i = 0; i < header.NumberOfSections; i++)
    {
        if (objectSections[i])
        {
            int len = sections[i].SizeOfRawData;
            if (len)
            {
                inputFile->seekg(sections[i].PointerToRawData + libOffset);
                int relocCount = sections[i].NumberOfRelocations;
                CoffReloc* relocPointer = relocs[i].get();
                if (relocCount == 0xffff && (sections[i].Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL))
                {
                    relocCount = relocPointer[0].VirtualAddress;
                    relocPointer++;
                }
                unsigned offset = 0;
                inputFile->clear();
                while (offset < sections[i].SizeOfRawData)
                {
                    if (!relocCount)
                    {
                        ObjByte* data = new ObjByte[sections[i].SizeOfRawData - offset];
                        inputFile->read((char*)data, sections[i].SizeOfRawData - offset);
                        if (inputFile->fail())
                        {
                            delete[] data;
                            delete fil;
                            return nullptr;
                        }
                        ObjMemory* newMem = new ObjMemory(data, sections[i].SizeOfRawData - offset);
                        objectSections[i]->Add(newMem);
                        offset = sections[i].SizeOfRawData;
                    }
                    else
                    {
                        unsigned fixupOffset;
                        int n = relocPointer->VirtualAddress - offset;
                        if (n)
                        {
                            ObjByte* data = new ObjByte[n];
                            inputFile->read((char*)data, n);
                            if (inputFile->fail())
                            {
                                delete[] data;
                                delete fil;
                                return nullptr;
                            }
                            ObjMemory* newMem = new ObjMemory(data, n);
                            objectSections[i]->Add(newMem);
                        }
                        inputFile->read((char*)&fixupOffset, sizeof(unsigned));
                        if (inputFile->fail())
                        {
                            delete fil;
                            return nullptr;
                        }
                        switch (relocPointer->Type)
                        {
                            ObjExpression* fixupExpr;
                            case IMAGE_REL_I386_DIR32:
                            case IMAGE_REL_I386_REL32:
                                if (symbols[relocPointer->SymbolTableIndex].StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
                                {
                                    if (symbols[relocPointer->SymbolTableIndex].SectionNumber == 0)
                                    {
                                        // external
                                        fixupExpr = new ObjExpression(externalMapping[relocPointer->SymbolTableIndex]);
                                    }
                                    else if (symbols[relocPointer->SymbolTableIndex].SectionNumber > header.NumberOfSections)
                                    {
                                        fixupExpr = new ObjExpression(
                                            ObjExpression::eAdd,
                                            new ObjExpression(
                                                objectSections[symbols[relocPointer->SymbolTableIndex].SectionNumber - 1]),
                                            new ObjExpression(0));
                                    }
                                    else
                                    {

                                        // public
                                        fixupExpr = new ObjExpression(
                                            ObjExpression::eAdd,
                                            new ObjExpression(
                                                objectSections[symbols[relocPointer->SymbolTableIndex].SectionNumber - 1]),
                                            new ObjExpression(symbols[relocPointer->SymbolTableIndex].Value));
                                    }
                                }
                                else
                                {
                                    // local static
                                    fixupExpr = new ObjExpression(
                                        ObjExpression::eAdd,
                                        new ObjExpression(
                                            objectSections[symbols[relocPointer->SymbolTableIndex].SectionNumber - 1]),
                                        new ObjExpression(symbols[relocPointer->SymbolTableIndex].Value));
                                }
                                if (relocPointer->Type == IMAGE_REL_I386_REL32)
                                {
                                    fixupExpr =
                                        new ObjExpression(ObjExpression::eAdd, fixupExpr, new ObjExpression(fixupOffset - 4));
                                    fixupExpr =
                                        new ObjExpression(ObjExpression::eSub, fixupExpr, new ObjExpression(ObjExpression::ePC));
                                }
                                else if (fixupOffset)
                                {
                                    fixupExpr = new ObjExpression(ObjExpression::eAdd, fixupExpr, new ObjExpression(fixupOffset));
                                }
                                objectSections[i]->Add(new ObjMemory(fixupExpr, 4));

                                break;
                            default:
                                std::cout << "Invalid relocation" << std::endl;
                                delete fil;
                                return nullptr;
                        }

                        offset += n + 4;
                        relocPointer++;
                        relocCount--;
                    }
                }
            }
        }
    }
    for (int i = 1; i < header.NumberOfSymbols; i += symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].SectionNumber > header.NumberOfSections && symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL &&
            symbols[i].Value)
        {
            int n = symbols[i].SectionNumber - 1;
            ObjByte* data = new ObjByte[symbols[i].Value];
            memset(data, 0, symbols[i].Value);
            ObjMemory* newMem = new ObjMemory(data, symbols[i].Value);
            objectSections[n]->Add(newMem);
        }
    }
    return fil;
}
