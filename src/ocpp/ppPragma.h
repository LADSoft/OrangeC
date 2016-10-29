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
#ifndef ppPragma_h
#define ppPragma_h

#include <list>
#include <string>
#include <map>

#include "Token.h"

class Packing
{
public:
    static Packing *Instance()
    {
        if (!instance)
            instance = new Packing;
        return instance;
    }
    
    int Get() { return list.front(); }
    void Add(int item) { list.push_front(item); }
    void Remove() { if (list.size() > 1) list.pop_front(); }
protected:
#ifdef MSDOS
    Packing() { list.push_front(1); }
#else
    Packing() { list.push_front(8); }
#endif

private:
    static Packing *instance;
    std::list<int> list;
} ;
class FenvAccess
{
public:
    static FenvAccess *Instance()
    {
        if (!instance)
            instance = new FenvAccess;
        return instance;
    }
    
    bool Get() { return list.front(); }
    void Add(bool item) { list.push_front(item); }
    void Remove() { if (list.size() > 1) list.pop_front(); }
    void Mark() { pos = list.size(); }
    void Release() { while (pos > list.size()) list.pop_front(); }
protected:
    FenvAccess() : pos(0) { list.push_front(false);  }

private:
    size_t pos;
    static FenvAccess *instance;
    std::list<bool> list;
} ;
class CXLimitedRange
{
public:
    static CXLimitedRange *Instance()
    {
        if (!instance)
            instance = new CXLimitedRange;
        return instance;
    }
    
    bool Get() { return list.front(); }
    void Add(bool item) { list.push_front(item); }
    void Remove() { if (list.size() > 1) list.pop_front(); }
    void Mark() { pos = list.size(); }
    void Release() { while (pos > list.size()) list.pop_front(); }
protected:
    CXLimitedRange() : pos(0) { list.push_front(false); }

private:
    size_t pos;
    static CXLimitedRange *instance;
    std::list<bool> list;
} ;
class FPContract
{
public:
    static FPContract *Instance()
    {
        if (!instance)
            instance = new FPContract;
        return instance;
    }
    
    bool Get() { return list.front(); }
    void Add(bool item) { list.push_front(item); }
    void Remove() { if (list.size() > 1) list.pop_front(); }
    void Mark() { pos = list.size(); }
    void Release() { while (pos > list.size()) list.pop_front(); }
protected:
    FPContract() : pos(0) { list.push_front(true); }

private:
    size_t pos;
    static FPContract *instance;
    std::list<bool> list;
} ;
class Libraries
{
public:
    static Libraries *Instance()
    {
        if (!instance)
            instance = new Libraries;
        return instance;
    }
    
    void Add(const std::string &item) { list.push_back(item); }
    typedef std::list<std::string>::iterator LibraryIterator;
    LibraryIterator begin() { return list.begin(); }
    LibraryIterator end() { return list.end(); }
    std::string &GetName(LibraryIterator it) { return (*it); }
protected:
    Libraries() { }

private:
    static Libraries *instance;
    std::list<std::string> list;
};
class Aliases
{
public:
    static Aliases *Instance()
    {
        if (!instance)
            instance = new Aliases;
        return instance;
    }
    
    void Add(const std::string &item, const std::string &alias) { list[alias] = item; }
    typedef std::map<std::string, std::string>::iterator AliasIterator;
    AliasIterator begin() { return list.begin(); }
    AliasIterator end() { return list.end(); }
    std::string GetName(AliasIterator it) { return it->second; }
    std::string GetAlias(AliasIterator it) { return it->first; }
protected:
    Aliases() { }

private:
    static Aliases *instance;
    std::map<std::string, std::string> list;
};
class Startups
{
public:
    class Properties 
    {
    public:
        Properties(int Prio, bool Startup) : prio(Prio), startup(Startup) { }
        int prio;
        bool startup;
    };
    static Startups *Instance()
    {
        if (!instance)
            instance = new Startups;
        return instance;
    }
    ~Startups() ;
    
    void Add(std::string &item, int Priority, bool startup)
    {
        list[item] = new Properties(Priority, startup);
    }
    typedef std::map<std::string, Properties *>::iterator StartupIterator;
    StartupIterator begin() { return list.begin(); }
    StartupIterator end() { return list.end(); }
    std::string GetName(StartupIterator it) { return it->first; }
    int GetPriority(StartupIterator it) { return it->second->prio; }
    bool IsStartup(StartupIterator it) { return it->second->startup; }
protected:
    Startups() { }

private:
    static Startups *instance;
    std::map<std::string, Properties *> list;
};
class ppPragma
{
public:
    ppPragma() { InitHash(); }
    void InitHash() ;
    bool Check(int token, const std::string &args);
    void ParsePragma( const std::string &args);
    void Mark()
    {
        FenvAccess::Instance()->Mark();
        CXLimitedRange::Instance()->Mark();
        FPContract::Instance()->Mark();
    }
    void Release()
    {
        FenvAccess::Instance()->Release();
        CXLimitedRange::Instance()->Release();
        FPContract::Instance()->Release();
    }	
protected:
    void HandleSTDC(Tokenizer &tk);
    void HandlePack(Tokenizer &tk);
    void HandleError(Tokenizer &tk);
    void HandleWarning(Tokenizer &tk);
    void HandleSR(Tokenizer &tk, bool startup);
    void HandleStartup(Tokenizer &tk) { HandleSR(tk, true); }
    void HandleRundown(Tokenizer &tk) { HandleSR(tk, false); }
    void HandleLibrary(Tokenizer &tk);
    void HandleAlias(Tokenizer &tk);
    void HandleFar(Tokenizer &tk);
    
private:
    KeywordHash hash;
} ;
#endif