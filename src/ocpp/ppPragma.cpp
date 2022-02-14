/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#define _CRT_SECURE_NO_WARNINGS

#include "ppPragma.h"
#include "PreProcessor.h"
#include "ppkw.h"
#include "ppInclude.h"
#include "Errors.h"
#include "sys/stat.h"
#include <algorithm>
#include <iostream>

Packing* Packing::instance;
FenvAccess* FenvAccess::instance;
CXLimitedRange* CXLimitedRange::instance;
FPContract* FPContract::instance;
Libraries* Libraries::instance;
Aliases* Aliases::instance;
Startups* Startups::instance;
Once* Once::instance;
Warning* Warning::instance;

KeywordHash ppPragma::hash = {
    {"(", kw::openpa},
    {")", kw::closepa},
    {",", kw::comma},
    {"=", kw::eq},
};
bool ppPragma::Check(kw token, const std::string& args)
{
    if (token == kw::PRAGMA)
    {
        ParsePragma(args);
        return true;
    }
    return false;
}
void ppPragma::ParsePragma(const std::string& args)
{
    Tokenizer tk(args, &hash);
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
        else if (str == "RUNDOWN")
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
            cppprio++;
        else if (str == "IGNORE_GLOBAL_INIT")
            HandleIgnoreGlobalInit(tk);
        else if (str == "PUSH_MACRO")
            HandlePushPopMacro(tk, true);
        else if (str == "POP_MACRO")
            HandlePushPopMacro(tk, false);
        else if (catchAll)
            catchAll(str, tk.GetString());
        // unmatched is not an error
    }
}
void ppPragma::HandleSTDC(Tokenizer& tk)
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
void ppPragma::HandlePack(Tokenizer& tk)
{
    const Token* tok = tk.Next();
    if (tok && tok->GetKeyword() == kw::openpa)
    {
        tok = tk.Next();
        int val = -1;
        if (tok)
            if (tok->IsNumeric())
            {
                val = tok->GetInteger();
                tok = tk.Next();
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
void ppPragma::HandleError(Tokenizer& tk) { Errors::Error(tk.GetString()); }
void ppPragma::HandleWarning(Tokenizer& tk)
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
            if (!strnicmp(p, "push", 4))
            {
                Warning::Instance()->Push();
                p += 4;
            }
            else if (!strnicmp(p, "pop", 3))
            {
                Warning::Instance()->Pop();
                p += 3;
            }
            else
            {
                bool enable = true;
                if (!strnicmp(p, "enable", 6))
                {
                    enable = true;
                    p += 6;
                }
                else if (!strnicmp(p, "disable", 7))
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
void ppPragma::HandleSR(Tokenizer& tk, bool startup)
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
Startups::~Startups() {}

bool Once::AddToList()
{
    OnceItem item(include->GetErrFile());
    if (items.find(item) != items.end())
        return false;
    items.insert(item);
    return true;
}
void Once::TriggerEOF() { include->ForceEOF(); }

bool Once::OnceItem::operator<(const OnceItem& right) const
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
void Once::OnceItem::SetParams(const std::string& fileName)
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
void ppPragma::HandleLibrary(Tokenizer& tk)
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
        q = strchr(p, ')');
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
void ppPragma::HandleAlias(Tokenizer& tk)
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
void ppPragma::HandleFar(Tokenizer& tk)
{
    // fixme
}
void ppPragma::HandleOnce(Tokenizer& tk) { Once::Instance()->CheckForMultiple(); }

void ppPragma::HandleIgnoreGlobalInit(Tokenizer& tk)
{
    const Token* name = tk.Next();
    if (name && name->IsNumeric())
    {
        ignoreGlobalInit = !!name->GetInteger();
    }
}

void ppPragma::HandlePushPopMacro(Tokenizer& tk, bool push)
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

int ppPragma::StdPragmas()
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
