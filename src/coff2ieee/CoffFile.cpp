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


#include "CoffFile.h"
#include "ObjFactory.h"
#include "ObjExpression.h"
#include "ObjSection.h"
#include "ObjSymbol.h"
#include "ObjIeee.h"
#include "Utils.h"

CoffFile::~CoffFile()
{
    if (name.size() && inputFile)
        delete inputFile;
        
    delete [] sections;
    delete [] symbols;
    delete [] strings;
    for (int i=0; i < relocs.size(); i++)
    {
        CoffReloc *reloc = relocs[i];
        delete[] reloc;
    }
    
}
bool CoffFile::Load()
{
    if (name.size())
        inputFile = new std::fstream(name.c_str(), std::ios::binary | std::ios::in);
    if (inputFile && inputFile->is_open())
    {   
        inputFile->read((char *)&header, sizeof(header));
        if (!inputFile->fail() && !inputFile->eof())
        {
            if (header.Machine == IMAGE_FILE_MACHINE_I386)
            {
                sections = new CoffSection[header.NumberOfSections + 1];
                inputFile->read((char *)sections, header.NumberOfSections * sizeof(CoffSection));
                if (!inputFile->fail() && !inputFile->eof())
                {
                    inputFile->seekg(header.PointerToSymbolTable + libOffset);
                    symbols = new CoffSymbol[header.NumberOfSymbols+2];
                    inputFile->read((char *)symbols, header.NumberOfSymbols * sizeof(CoffSymbol));
                    if (!inputFile->fail() && !inputFile->eof())
                    {
                        unsigned size;
                        inputFile->read((char *)&size, sizeof(size));
                        strings = new char[size];
                        memcpy(strings, &size, sizeof(size));
                        inputFile->read(strings + sizeof(size), size - sizeof(size));
                        if (!inputFile->fail())
                        {
                            int i;
                            inputFile->clear();
                            for (i=0; i < header.NumberOfSections; i++)
                            {
                                if (sections[i].NumberOfRelocations)
                                {
                                    CoffReloc *thisReloc;
                                    int relocCount = sections[i].NumberOfRelocations;
                                    if (sections[i].Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL)
                                    {
                                        if (sections[i].NumberOfRelocations != 0xffff)
                                        {
                                            std::cout << "Warning: Reloc overflow not handled properly" << std::endl;
                                        }
                                        CoffReloc temp;
                                        inputFile->seekg(sections[i].PointerToRelocations + libOffset);
                                        inputFile->read((char *)&temp, sizeof(CoffReloc));
                                        relocCount = temp.VirtualAddress + 1;
                                    }

                                    thisReloc = new CoffReloc[relocCount];
                                    relocs.push_back(thisReloc);
                                    inputFile->seekg(sections[i].PointerToRelocations + libOffset);
                                    inputFile->read((char *)thisReloc, relocCount * sizeof(CoffReloc)); 
                                    if (inputFile->fail())
                                        break;
                                    inputFile->clear();
                                }
                                else
                                {
                                    relocs.push_back(NULL);
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
    for (int i=0; i < header.NumberOfSymbols; i+= symbols[i].NumberOfAuxSymbols + 1)
    {
        if ((symbols[i].StorageClass == IMAGE_SYM_CLASS_STATIC || symbols[i].StorageClass == IMAGE_SYM_CLASS_SECTION) && symbols[i].SectionNumber)
        {
            int n = strlen(symbols[i].Name);
            if (n)
            {
                if (n > 8)
                    n = 8;
                if (!strncmp(symbols[i].Name, sections[symbols[i].SectionNumber-1].Name, n))
                {
                    sectionSymbols[symbols[i].SectionNumber-1] = (symbols +i);
                }
            }
        }
    }
    return true;
}
bool CoffFile::AddComdefs()
{
    int n = header.NumberOfSections;
    for (int i=1; i < header.NumberOfSymbols; i+= symbols[i].NumberOfAuxSymbols + 1)
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
    if (((CoffSectionAux *)&sectionSymbols[sect][1])->Selection > 1)
    {
        std::string name;
        CoffSymbol *sym = sectionSymbols[sect] + sectionSymbols[sect]->NumberOfAuxSymbols + 1;
        while (sym < symbols + header.NumberOfSymbols)
        {
            if (sym->SectionNumber == sect+1)
                break;
            sym += sym->NumberOfAuxSymbols + 1;
        }
        if (sym > symbols + header.NumberOfSymbols)
        {
            Utils::fatal("Comdat symbol name not found, exiting");
        }
        if (*(unsigned *)sym->Name == 0)
        {
            name = strings + *(unsigned *)(sym->Name + 4);
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
    if ((sections[sect].Characteristics & IMAGE_SCN_MEM_READ) && !(sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE) && (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA))
    {
        return "const";
    }
    if ((sections[sect].Characteristics & IMAGE_SCN_MEM_READ) && (sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE) && (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA))
    {
        return "data";
    }
    if ((sections[sect].Characteristics & IMAGE_SCN_MEM_READ) && (sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE) && (sections[sect].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
    {
        return "bss";
    }
    return "data";
}
ObjInt CoffFile::GetSectionQualifiers(int sect)
{
    if (((CoffSectionAux *)&sectionSymbols[sect][1])->Selection > 1)
    {
        ObjInt sel = 0;
        switch(((CoffSectionAux *)&sectionSymbols[sect][1])->Selection)
        {
//            case 1:
//                sel = ObjSection::unique;
                break;
            case 2:
            case 3:
            case 4:
            case 6:
                sel = ObjSection::max;
            case 5:
                sel = ObjSection::max;
                break;
            default:
                sel = ObjSection::max;
                std:: cout << "warning: unknown comdat style #" << (int)((CoffSectionAux *)&sectionSymbols[sect][1])->Selection << std::endl;
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
    if ((sections[sect].Characteristics & IMAGE_SCN_MEM_READ) && !(sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE) && (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA))
    {
        return ObjSection::rom;
    }
    if ((sections[sect].Characteristics & IMAGE_SCN_MEM_READ) && (sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE) && (sections[sect].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA))
    {
        return ObjSection::ram;
    }
    if ((sections[sect].Characteristics & IMAGE_SCN_MEM_READ) && (sections[sect].Characteristics & IMAGE_SCN_MEM_WRITE) && (sections[sect].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
    {
        return ObjSection::ram;
    }
    return ObjSection::ram;
}
ObjFile *CoffFile::ConvertToObject(std::string outputName, ObjFactory &factory)
{
    ObjFile *fil = new ObjFile(outputName);
    std::vector<ObjSection *> objectSections;
    std::map<int, ObjSymbol *> externalMapping;
    // Create the sections;
    for (int i=0; i < header.NumberOfSections; i++)
    {
        if (!(sections[i].Characteristics & (IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_LNK_INFO)) && strnicmp(sections[i].Name, ".debug", 6))
        {
            std::string sectname = GetSectionName(i);
            ObjSection * sect = factory.MakeSection(sectname);
            CoffSectionAux *aux = (CoffSectionAux *)& sectionSymbols[i][1];
            sect->SetSize(new ObjExpression(aux->Length));
            sect->SetAlignment(IMAGE_SCN_ALIGN(sections[i].Characteristics));
            sect->SetQuals(GetSectionQualifiers(i));
            fil->Add(sect);
            objectSections.push_back(sect);
        }
        else
        {
            objectSections.push_back(NULL);
        }
    }
    // dump comdefs
    for (int i=1; i < header.NumberOfSymbols; i+= symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].SectionNumber > header.NumberOfSections && symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL && symbols[i].Value)
        {
            char *sname = symbols[i].Name;
            std::string symbolName;
            if (*(unsigned *)sname == 0)
            {
                symbolName = strings + *(unsigned *)(sname + 4); 
            }            
            else
            {
                symbolName = symbols[i].Name;
                if (symbolName.size() > 8)
                    symbolName = symbolName.substr(0, 8);
            }
            symbolName = std::string("vsb") + symbolName;
            ObjSection * sect = factory.MakeSection(symbolName);
            sect->SetSize(new ObjExpression(symbols[i].Value));
            sect->SetAlignment(8);
            sect->SetQuals(ObjSection::ram | ObjSection::max | ObjSection::virt);
            fil->Add(sect);
            objectSections.push_back(sect);
        }
    }
    // create the symbols
    for (int i=0; i < header.NumberOfSymbols; i+= symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL && symbols[i].SectionNumber <= header.NumberOfSections)
        {
            if (symbols[i].SectionNumber <= 0 || (((CoffSectionAux *)&sectionSymbols[symbols[i].SectionNumber-1][1])->Selection <= 1))
            {
                char *sname = symbols[i].Name;
                std::string symbolName;
                if (*(unsigned *)sname == 0)
                {
                    symbolName = strings + *(unsigned *)(sname + 4); 
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
                    ObjSymbol *symbol = factory.MakePublicSymbol(symbolName);
                    ObjExpression *exp = new ObjExpression(symbols[i].Value);
                    symbol->SetOffset(exp);       
                    fil->Add(symbol);
                }
                else if (symbols[i].SectionNumber)
                {
                    ObjSymbol *symbol = factory.MakePublicSymbol(symbolName);
                    ObjExpression *exp = new ObjExpression(ObjExpression::eAdd, new ObjExpression(objectSections[symbols[i].SectionNumber-1]), new ObjExpression(symbols[i].Value));
                    symbol->SetOffset(exp);       
                    fil->Add(symbol);
                }
                else
                {
                    ObjSymbol *symbol = factory.MakeExternalSymbol(symbolName);
                    externalMapping[i] = symbol;
                    fil->Add(symbol);
                }
            }
        }
    }
    // dump data to the sections
    for (int i=0; i < header.NumberOfSections; i++)
    {
        if (objectSections[i])
        {
            int len = sections[i].SizeOfRawData;
            if (len)
            {
                inputFile->seekg(sections[i].PointerToRawData + libOffset);
                int relocCount = sections[i].NumberOfRelocations;
                CoffReloc * relocPointer = relocs[i];
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
                        ObjByte *data = new ObjByte[sections[i].SizeOfRawData-offset];
                        inputFile->read((char *)data, sections[i].SizeOfRawData-offset);
                        if (inputFile->fail())
                        {
                            return NULL;
                        }
                        ObjMemory *newMem = new ObjMemory(data, sections[i].SizeOfRawData-offset);
                        objectSections[i]->Add(newMem);
                        offset = sections[i].SizeOfRawData;
                    }
                    else
                    {
                        unsigned fixupOffset;
                        int n = relocPointer->VirtualAddress - offset;
                        if (n)
                        {
                            ObjByte *data = new ObjByte[n];
                            inputFile->read((char *)data, n);
                            if (inputFile->fail())
                            {
                                return NULL;
                            }
                            ObjMemory *newMem = new ObjMemory(data, n);
                            objectSections[i]->Add(newMem);
                        }
                        inputFile->read((char *)&fixupOffset, sizeof(unsigned));
                        if (inputFile->fail())
                        {
                            return NULL;
                        }
                        switch (relocPointer->Type)
                        {
                            ObjExpression *fixupExpr;
                            case IMAGE_REL_I386_DIR32:
                            case IMAGE_REL_I386_REL32:
                                if (symbols[relocPointer->SymbolTableIndex].StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
                                {
                                    if  (symbols[relocPointer->SymbolTableIndex].SectionNumber == 0)
                                    {
                                        // external
                                        fixupExpr = new ObjExpression(externalMapping[relocPointer->SymbolTableIndex]);
                                    }
                                    else if  (symbols[relocPointer->SymbolTableIndex].SectionNumber > header.NumberOfSections)
                                    {
                                        fixupExpr = new ObjExpression(ObjExpression::eAdd, new ObjExpression(objectSections[symbols[relocPointer->SymbolTableIndex].SectionNumber-1]), new ObjExpression(0));
                                    }
                                    else
                                    {
                                        
                                        // public 
                                        fixupExpr = new ObjExpression(ObjExpression::eAdd, new ObjExpression(objectSections[symbols[relocPointer->SymbolTableIndex].SectionNumber-1]), new ObjExpression(symbols[relocPointer->SymbolTableIndex].Value));
                                    }    
                                    
                                }
                                else
                                {
                                    // local static
                                    fixupExpr = new ObjExpression(ObjExpression::eAdd, new ObjExpression(objectSections[symbols[relocPointer->SymbolTableIndex].SectionNumber-1]), new ObjExpression(symbols[relocPointer->SymbolTableIndex].Value));
                                }
                                if (relocPointer->Type == IMAGE_REL_I386_REL32)
                                {
                                    fixupExpr = new ObjExpression(ObjExpression::eAdd, fixupExpr, new ObjExpression(fixupOffset - 4));
                                    fixupExpr = new ObjExpression(ObjExpression::eSub, fixupExpr, new ObjExpression(ObjExpression::ePC) );
                                }
                                else if (fixupOffset)
                                {
                                    fixupExpr = new ObjExpression(ObjExpression::eAdd, fixupExpr, new ObjExpression(fixupOffset));
                                }
                                objectSections[i]->Add(new ObjMemory(fixupExpr, 4));
   
                                break;
                            default:
                                std::cout << "Invalid relocation" << std::endl;
                                return NULL;
                        }
                        
                        offset += n + 4;
                        relocPointer++;
                        relocCount--;
                    }
                }                
            }
        }
    }
    for (int i=1; i < header.NumberOfSymbols; i+= symbols[i].NumberOfAuxSymbols + 1)
    {
        if (symbols[i].SectionNumber > header.NumberOfSections && symbols[i].StorageClass == IMAGE_SYM_CLASS_EXTERNAL && symbols[i].Value)
        {
            int n = symbols[i].SectionNumber-1;
            ObjByte *data = new ObjByte[symbols[i].Value];
            memset(data,0, symbols[i].Value);
            ObjMemory *newMem = new ObjMemory(data, symbols[i].Value);
            objectSections[n]->Add(newMem);
        }
    }
    return fil;
}
