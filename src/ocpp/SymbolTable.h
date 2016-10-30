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
    Symbol(const std::string &Name) : name(Name), fileName(""), lineNo(0) {}
    Symbol(const Symbol &old)
    { 
        name = old.name;
        fileName = old.fileName;
        lineNo = old.lineNo;
    }
    virtual ~Symbol() { }
    std::string &GetName() { return name; }
    void SetLocation(std::string &name, int line)
    {
        fileName = name;
        lineNo = line;
    }
    std::string &GetFileName() { return fileName; }
    int GetLineNo() { return lineNo; }
    void DefinedError(const std::string &msg)
    {
        Errors::Error(name + " " + msg);
        if (lineNo && fileName.size())
            Errors::Previous(name, lineNo, fileName);
    }
private:
    std::string name;
    std::string fileName;
    int lineNo;
} ;

class SymbolTable
{
public:
    SymbolTable() { }
    virtual ~SymbolTable();
    Symbol *Lookup(const std::string &name)
    {
        auto it = hashTable.find(name);
        if (it != hashTable.end())
            return it->second;
        else
            return nullptr;
    }
    void Add(Symbol *symbol)
    {
        hashTable[symbol->GetName()] = symbol;
        symList.push_back(symbol);
    }
    void Remove(Symbol *symbol)
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
    typedef std::deque<Symbol *>::iterator iterator;
    iterator begin() { return symList.begin(); }
    iterator end() { return symList.end(); }

    void clear() { hashTable.clear(); symList.clear(); }	
private:
    std::map<std::string, Symbol *> hashTable;
    std::deque<Symbol *> symList;
} ;
#endif