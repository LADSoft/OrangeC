/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#ifndef SymbolTable_h
#define SymbolTable_h

#include <map>
#include <deque>
#include <string>
#include <ctype.h>
#include "Errors.h"
#include "Token.h"
class Symbol
{
  public:
    Symbol(const std::string& Name) : name(Name), fileName(""), lineNo(0) {}
    Symbol(const Symbol& old)
    {
        name = old.name;
        fileName = old.fileName;
        lineNo = old.lineNo;
    }
    virtual ~Symbol() {}
    std::string& GetName() { return name; }
    void SetLocation(std::string& name, int line)
    {
        fileName = name;
        lineNo = line;
    }
    std::string& GetFileName() { return fileName; }
    int GetLineNo() { return lineNo; }
    void DefinedError(const std::string& msg)
    {
        Errors::Error(name + " " + msg);
        if (lineNo && fileName.size())
            Errors::Previous(name, lineNo, fileName);
    }

  private:
    std::string name;
    std::string fileName;
    int lineNo;
};

class SymbolTable
{
  public:
    SymbolTable() {}
    virtual ~SymbolTable();
    Symbol* Lookup(const std::string& name)
    {
        auto it = hashTable.find(name);
        if (it != hashTable.end())
            return it->second;
        else
            return nullptr;
    }
    void Add(Symbol* symbol)
    {
        hashTable[symbol->GetName()] = symbol;
        symList.push_back(symbol);
    }
    void Remove(Symbol* symbol)
    {
        auto it = hashTable.find(symbol->GetName());
        if (it != hashTable.end())
        {
            hashTable.erase(it);
            for (auto it = symList.begin(); it != symList.end(); ++it)
            {
                if (*it == symbol)
                {
                    symList.erase(it);
                    break;
                }
            }
            delete symbol;
        }
    }
    typedef std::deque<Symbol*>::iterator iterator;
    iterator begin() { return symList.begin(); }
    iterator end() { return symList.end(); }

    void clear()
    {
        hashTable.clear();
        symList.clear();
    }

  private:
    std::map<std::string, Symbol*> hashTable;
    std::deque<Symbol*> symList;
};
#endif