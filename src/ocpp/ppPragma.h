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

#ifndef ppPragma_h
#define ppPragma_h

#include <list>
#include <string>
#include <unordered_map>
#include <map>
#include <map>
#include <set>
#include <ctime>
#include <stack>
#include <vector>
#include "Token.h"
#include <functional>
#define STD_PRAGMA_FENV 1
#define STD_PRAGMA_FCONTRACT 2
#define STD_PRAGMA_CXLIMITED 4

class ppInclude;
class ppDefine;

class Packing
{
  public:
    static Packing* Instance()
    {
        if (!instance)
            instance = new Packing;
        return instance;
    }

    int Get() { return list.top(); }
    void Add(int item) { list.push(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop();
    }

    void Clear()
    {
        while (list.size() > 1)
            list.pop();
    }

  protected:
#ifdef MSDOS
    Packing() { list.push(1); }
#else
    Packing() { list.push(8); }
#endif

  private:
    static Packing* instance;
    std::stack<int> list;
};
class AdditionalLinkerCommands
{
  public:
    static std::shared_ptr<AdditionalLinkerCommands> Instance()
    {
        if (!instance)
            instance = std::shared_ptr<AdditionalLinkerCommands>(new AdditionalLinkerCommands);
        return instance;
    }
    void Add(std::string linkerCommand) { comments.push_back(linkerCommand); }
    std::string Get() { return comments.back(); }
    std::vector<std::string> GetAll() { return comments; }
    void Clear() { comments.clear(); }
    void Remove() { comments.pop_back(); }

  private:
    AdditionalLinkerCommands() {}
    std::vector<std::string> comments;
    static std::shared_ptr<AdditionalLinkerCommands> instance;
};
class ObjectComment
{
  public:
    static std::shared_ptr<ObjectComment> Instance()
    {
        if (!instance)
            instance = std::shared_ptr<ObjectComment>(new ObjectComment);
        return instance;
    }
    void Add(std::string objComment) { comments.push_back(objComment); }
    std::string Get() { return comments.back(); }
    std::vector<std::string> GetAll() { return comments; }
    void Clear() { comments.clear(); }
    void Remove() { comments.pop_back(); }

  private:
    ObjectComment() {}
    std::vector<std::string> comments;
    static std::shared_ptr<ObjectComment> instance;
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

    bool Get() { return list.top(); }
    void Add(bool item) { list.push(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop();
    }
    void Mark() { marks.push(list.size()); }
    void Release()
    {
        if (marks.size())
        {
            while (marks.top() < list.size())
                list.pop();
            marks.pop();
        }
    }

    void Clear()
    {
        while (list.size() > 1)
            list.pop();
    }

  protected:
    FenvAccess() { list.push(false); }

  private:
    std::stack<int> marks;
    static FenvAccess* instance;
    std::stack<bool> list;
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

    bool Get() { return list.top(); }
    void Add(bool item) { list.push(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop();
    }
    void Mark() { marks.push(list.size()); }
    void Release()
    {
        if (marks.size())
        {
            while (marks.top() < list.size())
                list.pop();
            marks.pop();
        }
    }

    void Clear()
    {
        while (list.size() > 1)
            list.pop();
    }

  protected:
    CXLimitedRange() { list.push(false); }

  private:
    std::stack<int> marks;
    static CXLimitedRange* instance;
    std::stack<bool> list;
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

    bool Get() { return list.top(); }
    void Add(bool item) { list.push(item); }
    void Remove()
    {
        if (list.size() > 1)
            list.pop();
    }
    void Mark() { marks.push(list.size()); }
    void Release()
    {
        if (marks.size())
        {
            while (marks.top() < list.size())
                list.pop();
            marks.pop();
        }
    }

    void Clear()
    {
        while (list.size() > 1)
            list.pop();
    }

  protected:
    FPContract() { list.push(true); }

  private:
    std::stack<int> marks;
    static FPContract* instance;
    std::stack<bool> list;
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
    std::list<std::string>& Get() { return list; }
    void Clear() { list.clear(); }

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
    typedef std::unordered_map<std::string, std::string>::iterator AliasIterator;
    AliasIterator begin() { return list.begin(); }
    AliasIterator end() { return list.end(); }
    std::string GetName(AliasIterator it) { return it->second; }
    std::string GetAlias(AliasIterator it) { return it->first; }
    void Clear() { list.clear(); }
    const char* Lookup(const char* name)
    {
        std::string finder = name;
        auto it = list.find(name);
        if (it != list.end())
            return it->second.c_str();
        return nullptr;
    }

  protected:
    Aliases() {}

  private:
    static Aliases* instance;
    std::unordered_map<std::string, std::string> list;
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

    void Add(std::string& item, int Priority, bool startup) { list[item] = std::make_unique<Properties>(Priority, startup); }
    typedef std::map<std::string, std::unique_ptr<Properties>>::iterator StartupIterator;
    StartupIterator begin() { return list.begin(); }
    StartupIterator end() { return list.end(); }
    std::string GetName(StartupIterator it) { return it->first; }
    int GetPriority(StartupIterator it) { return it->second->prio; }
    bool IsStartup(StartupIterator it) { return it->second->startup; }
    std::map<std::string, std::unique_ptr<Properties>>& GetStartups() { return list; }
    void Clear() { list.clear(); }

  protected:
    Startups() {}

  private:
    static Startups* instance;
    std::map<std::string, std::unique_ptr<Properties>> list;
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

    void SetInclude(ppInclude* Include)
    {
        include = Include;
        items.clear();
    }
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
        bool operator<(const OnceItem& right) const;
        void SetParams(const std::string& fileName);

        long filesize;
        time_t filetime;
        unsigned crc;
    };
    void Clear() { items.clear(); }

    static Once* instance;
    std::set<OnceItem> items;
    ppInclude* include;
};
class Warning
{
  public:
    static Warning* Instance()
    {
        if (!instance)
            instance = new Warning;
        return instance;
    }
    enum
    {
        Disable = 1,
        AsError = 2,
        OnlyOnce = 4,
        Emitted = 8
    };

    void SetFlag(int warning, int flag) { flags[warning] |= flag; }
    void ClearFlag(int warning, int flag) { flags[warning] &= ~flag; }
    bool IsSet(int warning, int flag) { return !!(flags[warning] & flag); }

    void Clear()
    {
        for (auto&& f : flags)
            f.second = 0;
    }
    void Push() { stack.push(flags); }
    void Pop()
    {
        flags = stack.top();
        stack.pop();
    }

  protected:
    Warning(){};

  private:
    static Warning* instance;
    std::map<int, int> flags;
    std::stack<std::map<int, int>> stack;
};
class ppPragma
{
  public:
    ppPragma(ppInclude* Include, ppDefine* Define) : cppprio(0), ignoreGlobalInit(false)
    {
        ObjectComment::Instance()->Clear();
        Packing::Instance()->Clear();
        FenvAccess::Instance()->Clear();
        CXLimitedRange::Instance()->Clear();
        FPContract::Instance()->Clear();
        Libraries::Instance()->Clear();
        Aliases::Instance()->Clear();
        Startups::Instance()->Clear();
        Once::Instance()->SetInclude(Include);
        define = Define;
    }
    bool Check(kw token, const std::string& args);
    void ParsePragma(const std::string& args);
    int Pack() { return Packing::Instance()->Get(); }
    std::vector<std::string> ObjComments() { return ObjectComment::Instance()->GetAll(); }
    int StdPragmas();
    int CppPrio() { return cppprio; }
    std::list<std::string>& IncludeLibs() { return Libraries::Instance()->Get(); }
    std::map<std::string, std::unique_ptr<Startups::Properties>>& GetStartups() { return Startups::Instance()->GetStartups(); }
    const char* LookupAlias(const char* name) const { return Aliases::Instance()->Lookup(name); }

    void SetPragmaCatchall(std::function<void(const std::string&, const std::string&)> callback) { catchAll = callback; }
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
    void HandleIgnoreGlobalInit(Tokenizer& tk);
    void HandlePushPopMacro(Tokenizer& tk, bool push);
    void HandleComment(Tokenizer& tk);

  private:
    ppDefine* define;
    int cppprio;
    bool ignoreGlobalInit;
    std::function<void(const std::string&, const std::string&)> catchAll;
    static KeywordHash hash;
};
#endif