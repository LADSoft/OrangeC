/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include "ppInclude.h"
#include "Errors.h"
#include "sys/stat.h"
#include <algorithm>

Packing* Packing::instance;
FenvAccess* FenvAccess::instance;
CXLimitedRange* CXLimitedRange::instance;
FPContract* FPContract::instance;
Libraries* Libraries::instance;
Aliases* Aliases::instance;
Startups* Startups::instance;
Once* Once::instance;

KeywordHash ppPragma::hash = {
    { "(", openpa},
    { ")", closepa},
    { ",", comma},
    { "=", eq},
};
bool ppPragma::Check(int token, const std::string& args)
{
    if (token == PRAGMA)
    {
        ParsePragma(args);
        return true;
    }
    return false;
}
void ppPragma::ParsePragma(const std::string& args)
{
    Tokenizer tk(args, &hash);
    std::shared_ptr<Token> id = tk.Next();
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
        // unmatched is not an error
    }
}
void ppPragma::HandleSTDC(Tokenizer& tk)
{
    std::shared_ptr<Token> token = tk.Next();
    if (token && token->IsIdentifier())
    {
        std::string name = token->GetId();
        std::shared_ptr<Token> tokenCmd = tk.Next();
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
    std::shared_ptr<Token> tok = tk.Next();
    if (tok && tok->GetKeyword() == openpa)
    {
        tok = tk.Next();
        int val = -1;
        if (tok)
            if (tok->IsNumeric())
            {
                val = tok->GetInteger();
            }
        tok = tk.Next();
        if (tok)
            if (tok->GetKeyword() == closepa)
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
    const char* p = tk.GetString().c_str();
    while (isspace(*p))
        p++;
    if (*p != '(')
        Errors::Warning(p);
}
void ppPragma::HandleSR(Tokenizer& tk, bool startup)
{
    std::shared_ptr<Token> name = tk.Next();
    if (name && name->IsIdentifier())
    {
        std::string id = name->GetId();
        std::shared_ptr<Token> prio = tk.Next();
        if (prio && prio->IsNumeric() && !prio->IsFloating())
        {
            Startups::Instance()->Add(id, prio->GetInteger(), startup);
        }
    }
}
Startups::~Startups() {}

bool Once::AddToList()
{
    OnceItem item(include->GetFile());
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
    strcpy(buf, tk.GetString().c_str());
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
    std::shared_ptr<Token> name = tk.Next();
    if (name && name->IsIdentifier())
    {
        std::string id = name->GetId();
        std::shared_ptr<Token> alias = tk.Next();
        if (alias && alias->GetKeyword() == eq)
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
