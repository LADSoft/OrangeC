/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "SymbolTable.h"
#include "ObjFile.h"
#include "ObjSourceFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "ObjSection.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"
#include "LibManager.h"
#include "LibFiles.h"
#include <iostream>
const char Symbol::SymbolTypeChars[] = "ABbCDdiNRrSTtU?";

Symbol::SymbolType SymbolTable::GlobalSymbolType(ObjSymbol* sym, bool local)
{
    ObjExpression* e = sym->GetOffset();
    if (e->GetOp() == ObjExpression::eAdd)
    {
        e = e->GetLeft();
        if (e->GetOp() == ObjExpression::eSection)
        {
            ObjSection* sect = e->GetSection();
            if (sect->GetName() == "code")
            {
                if (local)
                    return Symbol::TextLocal;
                return Symbol::TextGlobal;
            }
            else if (sect->GetName() == "data")
            {
                if (local)
                    return Symbol::DataLocal;
                return Symbol::DataGlobal;
            }
            else if (sect->GetName() == "bss")
            {
                if (local)
                    return Symbol::BSSLocal;
                return Symbol::BSSGlobal;
            }
            else if (sect->GetQuals() & ObjSection::rom)
            {
                if (local)
                    return Symbol::ROLocal;
                return Symbol::ROGlobal;
            }
            else if (sect->GetQuals() & ObjSection::common)
                return Symbol::Common;
        }
    }
    return Symbol::Absolute;
}
ObjInt SymbolTable::GlobalSymbolOffset(ObjSymbol* sym) { return sym->GetOffset()->Eval(0); }
void SymbolTable::ProcessObjectFile(ObjFile* file)
{
    std::string name = file->GetName();
    size_t npos = name.find_last_of('\\');
    if (npos == std::string::npos)
        npos = name.find_last_of('/');
    if (npos != std::string::npos && npos != name.size() - 1)
        name = name.substr(npos + 1);
    int max = 1;
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        if ((*it)->GetIndex() > max)
            max = (*it)->GetIndex();
        (*it)->SetOffset(new ObjExpression(0));
    }
    sections.clear();
    sections.resize(max + 1);
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        sections[(*it)->GetIndex()] = *it;
        Symbol* s = new Symbol(name, (*it)->GetName(), 0, Symbol::Section);
        s->displayName = (*it)->GetDisplayName();
        symbols.push_back(s);
    }
    for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        Symbol::SymbolType type = GlobalSymbolType(*it, false);
        ObjInt offset = GlobalSymbolOffset(*it);
        Symbol* s = new Symbol(name, (*it)->GetName(), offset, type);
        s->displayName = (*it)->GetDisplayName();
        symbols.push_back(s);
    }
    for (auto it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        Symbol::SymbolType type = GlobalSymbolType(*it, true);
        ObjInt offset = GlobalSymbolOffset(*it);
        Symbol* s = new Symbol(name, (*it)->GetName(), offset, type);
        s->displayName = (*it)->GetDisplayName();
        symbols.push_back(s);
    }
    for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        Symbol::SymbolType type = Symbol::Undefined;
        ObjInt offset = 0;
        Symbol* s = new Symbol(name, (*it)->GetName(), offset, type);
        s->displayName = (*it)->GetDisplayName();
        externals.insert(s);
    }
    for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        Symbol::SymbolType type = Symbol::Import;
        ObjInt offset = 0;
        Symbol* s = new Symbol(name, (*it)->GetName(), offset, type);
        s->displayName = (*it)->GetDisplayName();
        symbols.push_back(s);
    }
    for (auto it = file->AutoBegin(); it != file->AutoEnd(); ++it)
    {
        Symbol::SymbolType type = Symbol::Debug;
        ObjInt offset = 0;
        Symbol* s = new Symbol(name, (*it)->GetName(), offset, type);
        s->displayName = (*it)->GetDisplayName();
        symbols.push_back(s);
    }
}
void SymbolTable::Load(CmdFiles& files)
{
    bool first = true;
    for (auto name : files)
    {
        if (!first)
        {
            LibManager librarian(name, false, true);
            if (librarian.IsOpen())
            {
                if (!librarian.LoadLibrary())
                {
                    FILE* stream = fopen(name.c_str(), "rb");
                    ObjIeeeIndexManager im1;
                    ObjFactory factory(&im1);
                    ObjIeee ieee(name.c_str(), true);
                    ObjFile* f = ieee.Read(stream, ObjIeee::eAll, &factory);
                    if (!f)
                    {
                        std::cout << "'" << name << "' is not an object file or library, not loading" << std::endl;
                    }
                    else
                    {
                        ProcessObjectFile(f);
                    }
                }
                else
                {
                    for (int i = 0; i < librarian.Files().size(); ++i)
                    {
                        ObjIeeeIndexManager im1;
                        ObjFactory factory(&im1);
                        ProcessObjectFile(librarian.LoadModule(i, &factory));
                    }
                }
            }
            else
            {
                std::cout << "'" << name << "' is not an object file or library, not loading" << std::endl;
            }
        }
        first = false;
    }
    ResolveExternalTypes();
    if (allSymbols)
    {
        LoadExterns();
    }
    else if (externalSymbols)
    {
        symbols.clear();
        LoadExterns();
    }
    else if (undefinedSymbols)
    {
        symbols.clear();
        CullExterns();
        LoadExterns();
    }
}
void SymbolTable::ResolveExternalTypes()
{
    for (auto e : symbols)
    {
        auto it = externals.find(e);
        if (it != externals.end())
        {
            (*it)->type = e->type;
        }
    }
}
void SymbolTable::CullExterns()
{
    std::set<Symbol*, Symbol> cached;
    for (auto e : externals)
        if (e->type == Symbol::Undefined)
            cached.insert(e);
    externals = cached;
}
void SymbolTable::LoadExterns()
{
    for (auto e : externals)
        symbols.push_back(e);
}
