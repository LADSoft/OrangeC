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

#pragma once

#include "CmdFiles.h"
#include <string>
#include <vector>
#include <set>
#include "ObjTypes.h"

class ObjSymbol;
class ObjFile;
class ObjSection;
class Symbol
{
  public:
    enum SymbolType
    {
        Absolute,
        BSSGlobal,
        BSSLocal,
        Common,
        DataGlobal,
        DataLocal,
        Import,
        Debug,
        ROGlobal,
        ROLocal,
        Section,
        TextGlobal,
        TextLocal,
        Undefined,
        ObjFileSpecific
    };
    Symbol() : fileName(""), symbolName(""), offset(0), type(Undefined) {}
    Symbol(std::string file, std::string name, ObjInt Offset, SymbolType Type) :
        fileName(file), symbolName(name), offset(Offset), type(Type)
    {
    }

    std::string fileName;
    std::string symbolName;
    std::string displayName;
    SymbolType type;
    ObjInt offset;

    bool operator()(Symbol* left, Symbol* right) const { return left->symbolName < right->symbolName; }
    char SymbolTypeChar() const { return SymbolTypeChars[type]; }
    static const char SymbolTypeChars[];
};
class SymbolTable
{
  public:
    SymbolTable(bool AllSymbols, bool ExternalSymbols, bool UndefinedSymbols) :
        allSymbols(AllSymbols), externalSymbols(ExternalSymbols), undefinedSymbols(UndefinedSymbols)
    {
    }
    void Load(CmdFiles& files);

    std::vector<Symbol*> symbols;

  protected:
    void ProcessObjectFile(ObjFile* file);
    void ProcessSymbol(ObjSymbol* sym);
    Symbol::SymbolType GlobalSymbolType(ObjSymbol* sym, bool local);
    ObjInt GlobalSymbolOffset(ObjSymbol* sym);
    void ResolveExternalTypes();
    void CullExterns();
    void LoadExterns();

  private:
    bool allSymbols;
    bool externalSymbols;
    bool undefinedSymbols;
    std::vector<ObjSection*> sections;
    std::set<Symbol*, Symbol> externals;
};
