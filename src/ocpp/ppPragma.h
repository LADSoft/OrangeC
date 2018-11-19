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

#ifndef ppPragma_h
#define ppPragma_h

#include <list>
#include <string>
#include <map>
#include <set>
#include <time.h>
#include "Token.h"

class ppInclude;

class Packing
{
  public:
    static Packing* Instance()
    {
        if (!instance)
            instance = new Packing;
        return instance;
    }

    int Get() { return list.front(); }
    void Add(int item) { list.push_front(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop_front();
    }

  protected:
#ifdef MSDOS
    Packing() { list.push_front(1); }
#else
    Packing() { list.push_front(8); }
#endif

  private:
    static Packing* instance;
    std::list<int> list;
};
class FenvAccess
{
  public:
    static FenvAccess* Instance()
    {
        if (!instance)
            instance = new FenvAccess;
        return instance;
    }

    bool Get() { return list.front(); }
    void Add(bool item) { list.push_front(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop_front();
    }
    void Mark() { pos = list.size(); }
    void Release()
    {
        while (pos > list.size())
            list.pop_front();
    }

  protected:
    FenvAccess() : pos(0) { list.push_front(false); }

  private:
    size_t pos;
    static FenvAccess* instance;
    std::list<bool> list;
};
class CXLimitedRange
{
  public:
    static CXLimitedRange* Instance()
    {
        if (!instance)
            instance = new CXLimitedRange;
        return instance;
    }

    bool Get() { return list.front(); }
    void Add(bool item) { list.push_front(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop_front();
    }
    void Mark() { pos = list.size(); }
    void Release()
    {
        while (pos > list.size())
            list.pop_front();
    }

  protected:
    CXLimitedRange() : pos(0) { list.push_front(false); }

  private:
    size_t pos;
    static CXLimitedRange* instance;
    std::list<bool> list;
};
class FPContract
{
  public:
    static FPContract* Instance()
    {
        if (!instance)
            instance = new FPContract;
        return instance;
    }

    bool Get() { return list.front(); }
    void Add(bool item) { list.push_front(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop_front();
    }
    void Mark() { pos = list.size(); }
    void Release()
    {
        while (pos > list.size())
            list.pop_front();
    }

  protected:
    FPContract() : pos(0) { list.push_front(true); }

  private:
    size_t pos;
    static FPContract* instance;
    std::list<bool> list;
};
class Libraries
{
  public:
    static Libraries* Instance()
    {
        if (!instance)
            instance = new Libraries;
        return instance;
    }

    void Add(const std::string& item) { list.push_back(item); }
    typedef std::list<std::string>::iterator LibraryIterator;
    LibraryIterator begin() { return list.begin(); }
    LibraryIterator end() { return list.end(); }
    std::string& GetName(LibraryIterator it) { return (*it); }

  protected:
    Libraries() {}

  private:
    static Libraries* instance;
    std::list<std::string> list;
};
class Aliases
{
  public:
    static Aliases* Instance()
    {
        if (!instance)
            instance = new Aliases;
        return instance;
    }

    void Add(const std::string& item, const std::string& alias) { list[alias] = item; }
    typedef std::map<std::string, std::string>::iterator AliasIterator;
    AliasIterator begin() { return list.begin(); }
    AliasIterator end() { return list.end(); }
    std::string GetName(AliasIterator it) { return it->second; }
    std::string GetAlias(AliasIterator it) { return it->first; }

  protected:
    Aliases() {}

  private:
    static Aliases* instance;
    std::map<std::string, std::string> list;
};
class Startups
{
  public:
    class Properties
    {
      public:
        Properties(int Prio, bool Startup) : prio(Prio), startup(Startup) {}
        int prio;
        bool startup;
    };
    static Startups* Instance()
    {
        if (!instance)
            instance = new Startups;
        return instance;
    }
    ~Startups();

    void Add(std::string& item, int Priority, bool startup) { list[item] = new Properties(Priority, startup); }
    typedef std::map<std::string, Properties*>::iterator StartupIterator;
    StartupIterator begin() { return list.begin(); }
    StartupIterator end() { return list.end(); }
    std::string GetName(StartupIterator it) { return it->first; }
    int GetPriority(StartupIterator it) { return it->second->prio; }
    bool IsStartup(StartupIterator it) { return it->second->startup; }

  protected:
    Startups() {}

  private:
    static Startups* instance;
    std::map<std::string, Properties*> list;
};
class Once
{
  public:
    static Once* Instance()
    {
        if (!instance)
            instance = new Once;
        return instance;
    }
    ~Once();

    void SetInclude(ppInclude *Include) { include=Include; items.clear(); }
    void CheckForMultiple()
    {
        if (!AddToList())
            TriggerEOF();
    }
  protected:
    Once() : include(nullptr) {}
    bool AddToList();
    void TriggerEOF();
  private:
    struct OnceItem
    {
        OnceItem(const std::string& fileName) { SetParams(fileName); }
        OnceItem(const OnceItem& right) = default;
        bool operator< (const OnceItem& right) const;
        void SetParams(const std::string& fileName);

        long filesize;
        time_t filetime;
        unsigned crc;

    };

    static Once* instance;
    std::set<OnceItem> items;
    ppInclude *include;
};
class ppPragma
{
  public:
    ppPragma(ppInclude *Include) { InitHash(); Once::Instance()->SetInclude(Include); }
    void InitHash();
    bool Check(int token, const std::string& args);
    void ParsePragma(const std::string& args);
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
    void HandleSTDC(Tokenizer& tk);
    void HandlePack(Tokenizer& tk);
    void HandleError(Tokenizer& tk);
    void HandleWarning(Tokenizer& tk);
    void HandleSR(Tokenizer& tk, bool startup);
    void HandleStartup(Tokenizer& tk) { HandleSR(tk, true); }
    void HandleRundown(Tokenizer& tk) { HandleSR(tk, false); }
    void HandleLibrary(Tokenizer& tk);
    void HandleAlias(Tokenizer& tk);
    void HandleFar(Tokenizer& tk);
    void HandleOnce(Tokenizer& tk);

  private:
    KeywordHash hash;
};
#endif