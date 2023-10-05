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
#include <cassert>
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
#include "ForwardDecls.h"
#define STD_PRAGMA_FENV 1
#define STD_PRAGMA_FCONTRACT 2
#define STD_PRAGMA_CXLIMITED 4
template <typename T, bool(isSymbolChar)(const char*, bool)>
static std::tuple<std::wstring, const Token*> MunchStrings(Tokenizer<T, isSymbolChar>& tk)
{
    std::wstring finalStr;
    const Token* tok;
    for (tok = tk.Next(); tok->IsString(); tok = tk.Next())
    {
        finalStr.append(tok->GetString());
    }
    return {finalStr, tok};
}

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
template <typename T, bool(isSymbolChar)(const char*, bool)>
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

    void SetInclude(ppInclude<T, isSymbolChar>* Include)
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
    bool AddToList()
    {
        OnceItem item(include->GetErrFile());
        if (items.find(item) != items.end())
            return false;
        items.insert(item);
        return true;
    }
    void TriggerEOF() { include->ForceEOF(); }

  private:
    struct OnceItem
    {
        OnceItem(const std::string& fileName) { SetParams(fileName); }
        OnceItem(const OnceItem& right) = default;
        bool operator<(const OnceItem& right) const
        {
            if (filesize < right.filesize)
                return true;
            else if (filesize == right.filesize)
            {
                if (filetime < right.filetime)
                    return true;
                else if (filetime == right.filetime)
                    if (crc < right.crc)
                        return true;
            }
            return false;
        }
        void SetParams(const std::string& fileName)
        {
            FILE* fil = fopen(fileName.c_str(), "rb");
            if (fil)
            {
                filesize = 0;
                crc = 0;
                int n;
                unsigned char buf[8192];
                while ((n = fread(buf, 1, sizeof(buf), fil)) > 0)
                {
                    filesize += n;
                    crc = Utils::PartialCRC32(crc, buf, n);
                }
                fclose(fil);
                struct stat statbuf;
                stat(fileName.c_str(), &statbuf);
                filetime = statbuf.st_mtime;
            }
            else
            {
                filetime = 0;
                filesize = 0;
                crc = 0;
            }
        }

        long filesize;
        time_t filetime;
        unsigned crc;
    };
    void Clear() { items.clear(); }

    static Once* instance;
    std::set<OnceItem> items;
    ppInclude<T, isSymbolChar>* include;
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
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppPragma
{
  public:
    ppPragma(ppInclude<T, isSymbolChar>* Include, ppDefine<T, isSymbolChar>* Define) : cppprio(0), ignoreGlobalInit(false)
    {
        ObjectComment::Instance()->Clear();
        Packing::Instance()->Clear();
        FenvAccess::Instance()->Clear();
        CXLimitedRange::Instance()->Clear();
        FPContract::Instance()->Clear();
        Libraries::Instance()->Clear();
        Aliases::Instance()->Clear();
        Startups::Instance()->Clear();
        Once<T, isSymbolChar>::Instance()->SetInclude(Include);
        define = Define;
    }
    bool Check(kw token, const std::string& args)
    {
        if (token == kw::PRAGMA)
        {
            ParsePragma(args);
            return true;
        }
        return false;
    }
    void ParsePragma(const std::string& args)
    {
        Tokenizer<kw> tk(args, &hash);
        const Token* id = tk.Next();
        if (id->IsIdentifier())
        {
            std::string str = id->GetId();
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            if (str == "STDC")
                HandleSTDC(tk);
            else if (str == "AUX")
                HandleAlias(tk);
            else if (str == "PACK")
                HandlePack(tk);
            else if (str == "LIBRARY")
                HandleLibrary(tk);
            else if (str == "STARTUP")
                HandleStartup(tk);
            else if (str == "RUNDOWN" || str == "EXIT")
                HandleRundown(tk);
            else if (str == "WARNING")
                HandleWarning(tk);
            else if (str == "ERROR")
                HandleError(tk);
            else if (str == "FARKEYWORD")
                HandleFar(tk);
            else if (str == "ONCE")
                HandleOnce(tk);
            else if (str == "PRIORITYCPP")
                cppprio--;
            else if (str == "IGNORE_GLOBAL_INIT")
                HandleIgnoreGlobalInit(tk);
            else if (str == "PUSH_MACRO")
                HandlePushPopMacro(tk, true);
            else if (str == "POP_MACRO")
                HandlePushPopMacro(tk, false);
            else if (str == "COMMENT")
                HandleComment(tk);
            else if (catchAll)
                catchAll(str, tk.GetString());
            // unmatched is not an error
        }
    }
    int Pack() { return Packing::Instance()->Get(); }
    std::vector<std::string> ObjComments() { return ObjectComment::Instance()->GetAll(); }
    int StdPragmas()
    {
        int rv = 0;
        if (FenvAccess::Instance()->Get())
            rv |= STD_PRAGMA_FENV;
        if (CXLimitedRange::Instance()->Get())
            rv |= STD_PRAGMA_CXLIMITED;
        if (FPContract::Instance()->Get())
            rv |= STD_PRAGMA_FCONTRACT;
        return rv;
    }

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
    void HandleSTDC(Tokenizer<T, isSymbolChar>& tk)
    {
        const Token* token = tk.Next();
        if (token && token->IsIdentifier())
        {
            std::string name = token->GetId();
            const Token* tokenCmd = tk.Next();
            if (tokenCmd && tokenCmd->IsIdentifier())
            {
                const char* val = tokenCmd->GetId().c_str();
                bool on;
                bool valid = false;
                if (!strcmp(val, "ON"))
                {
                    valid = true;
                    on = true;
                }
                else if (!strcmp(val, "OFF"))
                {
                    valid = true;
                    on = false;
                }
                if (valid)
                {
                    if (name == "FENV_ACCESS")
                        FenvAccess::Instance()->Add(on);
                    else if (name == "CX_LIMITED_RANGE")
                        CXLimitedRange::Instance()->Add(on);
                    else if (name == "FP_CONTRACT")
                        FPContract::Instance()->Add(on);
                }
            }
        }
    }
    void HandlePack(Tokenizer<T, isSymbolChar>& tk)
    {
        const Token* tok = tk.Next();
        if (tok && tok->GetKeyword() == kw::openpa)
        {
            tok = tk.Next();
            int val = -1;
            if (tok)
            {
                if (tok->GetId() == "push" || tok->GetId() == "pop")
                {
                    tok = tk.Next();
                    if (tok->GetKeyword() == kw::comma)
                        tok = tk.Next();
                }
                if (tok->IsNumeric())
                {
                    val = tok->GetInteger();
                    tok = tk.Next();
                }
            }
            if (tok)
                if (tok->GetKeyword() == kw::closepa)
                {
                    if (val <= 0)
                    {
                        Packing::Instance()->Remove();
                    }
                    else
                    {
                        Packing::Instance()->Add(val);
                    }
                }
        }
    }
    void HandleError(Tokenizer<T, isSymbolChar>& tk) { Errors::Error(tk.GetString()); }
    void HandleWarning(Tokenizer<T, isSymbolChar>& tk)
    {
        // check for microsoft warning pragma
        std::string warn = tk.GetString();
        const char* p = warn.c_str();
        while (isspace(*p))
            p++;
        if (*p != '(')
        {
            Errors::Warning(p);
        }
        else
        {
            p++;
            while (*p)
            {
                while (isspace(*p))
                    p++;
                if (!_strnicmp(p, "push", 4))
                {
                    Warning::Instance()->Push();
                    p += 4;
                }
                else if (!_strnicmp(p, "pop", 3))
                {
                    Warning::Instance()->Pop();
                    p += 3;
                }
                else
                {
                    bool enable = true;
                    if (!_strnicmp(p, "enable", 6))
                    {
                        enable = true;
                        p += 6;
                    }
                    else if (!_strnicmp(p, "disable", 7))
                    {
                        enable = false;
                        p += 7;
                    }
                    else
                    {
                        break;
                    }
                    while (isspace(*p))
                        p++;
                    if (*p++ == ':')
                    {
                        while (isspace(*p))
                            p++;
                        while (isdigit(*p))
                        {
                            int w = 0;
                            while (isdigit(*p))
                                w = w * 10 + ((*p++) - '0');
                            if (enable)
                                Warning::Instance()->ClearFlag(w, Warning::Disable);
                            else
                                Warning::Instance()->SetFlag(w, Warning::Disable);
                            while (isspace(*p))
                                p++;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                while (isspace(*p))
                    p++;
                if (*p++ != ',')
                    break;
            }
        }
    }
    void HandleSR(Tokenizer<T, isSymbolChar>& tk, bool startup)
    {
        const Token* name = tk.Next();
        if (name && name->IsIdentifier())
        {
            std::string id = name->GetId();
            const Token* prio = tk.Next();
            if (prio && prio->IsNumeric() && !prio->IsFloating())
            {
                Startups::Instance()->Add(id, prio->GetInteger(), startup);
            }
        }
    }
    void HandleStartup(Tokenizer<T, isSymbolChar>& tk) { HandleSR(tk, true); }
    void HandleRundown(Tokenizer<T, isSymbolChar>& tk) { HandleSR(tk, false); }
    void HandleLibrary(Tokenizer<T, isSymbolChar>& tk)
    {
        char buf[260 + 10];
        char* p = buf;
        char* q = p;
        Utils::StrCpy(buf, tk.GetString().c_str());
        while (isspace(*p))
            p++;
        if (*p == '(')
        {
            do
            {
                p++;
            } while (isspace(*p));
            q = (char*)strchr(p, ')');
            if (q)
            {
                while (q != p && isspace(q[-1]))
                    q--;
                *q = 0;
                if (*p)
                    Libraries::Instance()->Add(p);
            }
        }
    }
    void HandleAlias(Tokenizer<T, isSymbolChar>& tk)
    {
        const Token* name = tk.Next();
        if (name && name->IsIdentifier())
        {
            std::string id = name->GetId();
            const Token* alias = tk.Next();
            if (alias && alias->GetKeyword() == kw::eq)
            {
                alias = tk.Next();
                if (alias && alias->IsIdentifier())
                {
                    Aliases::Instance()->Add(id, alias->GetId());
                }
            }
        }
    }
    void HandleFar(Tokenizer<T, isSymbolChar>& tk) {}
    void HandleOnce(Tokenizer<T, isSymbolChar>& tk) { Once<T, isSymbolChar>::Instance()->CheckForMultiple(); }
    void HandleIgnoreGlobalInit(Tokenizer<T, isSymbolChar>& tk)
    {
        const Token* name = tk.Next();
        if (name && name->IsNumeric())
        {
            ignoreGlobalInit = !!name->GetInteger();
        }
    }
    void HandlePushPopMacro(Tokenizer<T, isSymbolChar>& tk, bool push)
    {
        int n = 0;
        std::string cache = tk.GetString();
        const char* p = cache.c_str();
        while (isspace(*p))
            p++;
        if (*p++ == '(')
        {
            while (isspace(*p))
                p++;
            if (*p++ == '"')
            {
                const char* start = p;
                while (isalnum(*p) || *p == '_')
                    p++;
                const char* end = p;
                if (*p++ == '"')
                {
                    while (isspace(*p))
                        p++;
                    if (*p == ')')
                    {
                        define->PushPopMacro(std::string(start, end - start), push);
                    }
                }
            }
        }
    }
    void HandleComment(Tokenizer<T, isSymbolChar>& tk)
    {
        const Token* checkCode = tk.Next();
        if (!checkCode->IsKeyword() && checkCode->GetKeyword() != kw::openpa)
        {
            Errors::WarningWithLine("pragma comment invalid, no starting parenthesis", Errors::GetFileName(),
                                    Errors::GetErrorLine());
        }
        const Token* tok = tk.Next();
        const Token* afterTok = nullptr;
        bool needEndParenErrors = true;
        if (tok->IsIdentifier())
        {
            enum switchVals
            {
                COMPILER,
                LIB,
                LINKER,
                USER
            };
            const std::unordered_map<std::string, switchVals> switchMap = {
                {"compiler", COMPILER}, {"lib", LIB}, {"linker", LINKER}, {"user", USER}};
            std::string idVal = tok->GetId();
            std::transform(idVal.begin(), idVal.end(), idVal.begin(), ::tolower);
            auto switchVal = switchMap.find(idVal);
            if (switchVal != switchMap.end())
            {
                switch (switchVal->second)
                {
                    case COMPILER: {
                        // We need to insert into the object library a comment about the compile date & time, currently unsupported
                        const Token* parenTok = tk.Next();
                        if (parenTok->IsKeyword() && parenTok->GetKeyword() != kw::closepa)
                        {
                            Errors::WarningWithLine(
                                "Did not place a ending parenthesis for the compiler comment pragma in the correct spot",
                                Errors::GetFileName(), Errors::GetErrorLine());
                        }
                        else
                        {
                            Errors::WarningWithLine(
                                "Pragma compiler is currently unsupported, other warnings are generated for MSVC compat",
                                Errors::GetFileName(), Errors::GetErrorLine());
                        }
                    }

                    break;
                    case LIB: {
                        // Send info to the linker like we do with pragma library
                        const Token* commaTok = tk.Next();
                        if (commaTok->IsKeyword() && commaTok->GetKeyword() == kw::comma)
                        {
                            std::tuple<std::wstring, const Token*> tup = MunchStrings(tk);
                            std::wstring myString = std::get<0>(tup);
                            afterTok = std::get<1>(tup);
                            if (myString.empty())
                            {
                                Errors::WarningWithLine(
                                    "Specified adding a library with pragma comment but did not name the library",
                                    Errors::GetFileName(), Errors::GetErrorLine());
                                needEndParenErrors = false;
                            }
                            else
                            {
                                Libraries::Instance()->Add(Utils::ConvertWStringToString(myString));
                            }
                        }
                        else
                        {
                            Errors::WarningWithLine("Specified a comment pragma for a library, but missed the comma",
                                                    Errors::GetFileName(), Errors::GetErrorLine());
                            needEndParenErrors = false;
                        }
                        break;
                    }
                    case LINKER: {
                        // Send specific commands to the linker
                        const Token* commaTok = tk.Next();
                        if (commaTok->IsKeyword() && commaTok->GetKeyword() == kw::comma)
                        {
                            std::tuple<std::wstring, const Token*> tup = MunchStrings(tk);
                            std::wstring myString = std::get<0>(tup);
                            afterTok = std::get<1>(tup);
                            if (myString.empty())
                            {
                                Errors::WarningWithLine(
                                    "Specified adding a library with pragma comment but did not name the library",
                                    Errors::GetFileName(), Errors::GetErrorLine());
                                needEndParenErrors = false;
                            }
                            else
                            {
                                Errors::WarningWithLine("Currently pragma comment(linker) is unsupported.", Errors::GetFileName(),
                                                        Errors::GetErrorLine());
                                // AdditionalLinkerCommands::Instance()->Add(Utils::ConvertWStringToString(myString));
                            }
                        }
                        else
                        {
                            Errors::WarningWithLine("Specified a comment pragma for a library, but missed the comma",
                                                    Errors::GetFileName(), Errors::GetErrorLine());
                            needEndParenErrors = false;
                        }
                        break;
                    }
                    break;
                    case USER: {
                        const Token* commaTok = tk.Next();
                        // Send specific info into the object comments
                        if (commaTok->IsKeyword() && commaTok->GetKeyword() == kw::comma)
                        {
                            std::tuple<std::wstring, const Token*> tup = MunchStrings(tk);
                            std::wstring myString = std::get<0>(tup);
                            afterTok = std::get<1>(tup);
                            if (myString.empty())
                            {
                                Errors::WarningWithLine(
                                    "Specified adding a comment to the generated object file, but did not specify what",
                                    Errors::GetFileName(), Errors::GetErrorLine());
                                needEndParenErrors = false;
                            }
                            else
                            {
                                Errors::WarningWithLine(
                                    "Pragma user is currently unsupported, other warnings are generated for MSVC compat",
                                    Errors::GetFileName(), Errors::GetErrorLine());
                                ObjectComment::Instance()->Add(Utils::ConvertWStringToString(myString));
                            }
                        }
                        else
                        {
                            Errors::WarningWithLine(
                                "Specified a comment pragma for a user generated object comment but missed the comma",
                                Errors::GetFileName(), Errors::GetErrorLine());
                        }
                    }
                    break;
                    default:
                        assert(false);
                        break;
                }
            }
            else
            {
                Errors::WarningWithLine("Pragma comment invalid, no valid comment type listed", Errors::GetFileName(),
                                        Errors::GetErrorLine());
            }
        }
        const Token* endCode = afterTok ? afterTok : tk.Next();
        if (!endCode->IsKeyword() && endCode->GetKeyword() != kw::closepa)
        {
            if (needEndParenErrors)
            {
                Errors::WarningWithLine("pragma comment invalid, no ending parenthesis where expected", Errors::GetFileName(),
                                        Errors::GetErrorLine());
            }
        }
    }

  private:
    ppDefine<T, isSymbolChar>* define;
    int cppprio;
    bool ignoreGlobalInit;
    std::function<void(const std::string&, const std::string&)> catchAll;
    KeywordHash hash = {
        {"(", kw::openpa},
        {")", kw::closepa},
        {",", kw::comma},
        {"=", kw::eq},
    };
};
#endif