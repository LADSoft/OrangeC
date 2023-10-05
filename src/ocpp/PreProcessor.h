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

#ifndef PreProcessor_h
#define PreProcessor_h

#include "ppDefine.h"
#include "ppError.h"
#include "ppFile.h"
#include "ppInclude.h"
#include "ppPragma.h"
#include "ppMacro.h"
#include "ppCtx.h"
#include "ppExpr.h"
#include "ppEmbed.h"
#include "ppCommon.h"

template <typename T = kw, bool(isSymbolChar)(const char*, bool) = IsSymbolCharOrDefault, void(embed_elements)(std::vector<embeder_size>) = nullptr>
class PreProcessor
{
  public:
    PreProcessor(const std::string& FileName, const std::string& SrchPth, const std::string& SysSrchPth, bool fullName,
                 bool Trigraph, char PPStart, bool isunsignedchar, Dialect dialect, bool extensions, bool NoErr,
                 const std::string& pipeName) :
        ppStart(PPStart),
        lineno(0),
        include(fullName, Trigraph, extensions, isunsignedchar, dialect, SrchPth, SysSrchPth, PPStart == '%', NoErr, pipeName),
        define(extensions, &include, dialect, PPStart == '%'),
        macro(include, define, dialect),
        ctx(define),
        trigraphs(Trigraph),
        pragma(&include, &define),
        ppEmbed(include)
    {
        InitHash();
        Errors::SetInclude(&include);
        macro.SetPreProcessor(this);
        include.SetParams(FileName, &define, &ctx);
        define.SetParams(&ctx, &macro);
    }

    void InitHash()
    {
        hash["define"] = kw::DEFINE;
        hash["undef"] = kw::UNDEF;
        hash["pragma"] = kw::PRAGMA;
        hash["error"] = kw::ERROR;
        hash["warning"] = kw::WARNING;
        hash["line"] = kw::LINE;
        hash["include"] = kw::INCLUDE;
        hash["include_next"] = kw::INCLUDE_NEXT;
        hash["if"] = kw::IF;
        hash["elif"] = kw::ELIF;
        hash["ifdef"] = kw::IFDEF;
        hash["ifndef"] = kw::IFNDEF;
        hash["else"] = kw::ELSE;
        hash["endif"] = kw::ENDIF;
        hash["elifdef"] = kw::ELIFDEF;
        hash["elifndef"] = kw::ELIFNDEF;
        if (ppStart == '#')
        {
            hash["embed"] = kw::EMBED;
        }
        if (ppStart == '%')
        {
            hash["idefine"] = kw::IDEFINE;
            hash["iassign"] = kw::IASSIGN;
            hash["ifctx"] = kw::IFCTX;
            hash["elifctx"] = kw::ELIFCTX;
            hash["ifnctx"] = kw::IFNCTX;
            hash["elifnctx"] = kw::ELIFNCTX;
            hash["ifidn"] = kw::IFIDN;
            hash["elifidn"] = kw::ELIFIDN;
            hash["ifidni"] = kw::IFIDNI;
            hash["elifidni"] = kw::ELIFIDNI;
            hash["ifid"] = kw::IFID;
            hash["elifid"] = kw::ELIFID;
            hash["ifnum"] = kw::IFNUM;
            hash["elifnum"] = kw::ELIFNUM;
            hash["ifstr"] = kw::IFSTR;
            hash["elifstr"] = kw::ELIFSTR;
            hash["ifnidn"] = kw::IFNIDN;
            hash["elifnidn"] = kw::ELIFNIDN;
            hash["ifnidni"] = kw::IFNIDNI;
            hash["elifnidni"] = kw::ELIFNIDNI;
            hash["ifnid"] = kw::IFNID;
            hash["elifnid"] = kw::ELIFNID;
            hash["ifnnum"] = kw::IFNNUM;
            hash["elifnnum"] = kw::ELIFNNUM;
            hash["ifnstr"] = kw::IFNSTR;
            hash["elifnstr"] = kw::ELIFNSTR;
            hash["assign"] = kw::ASSIGN;
            hash["rep"] = kw::REP;
            hash["endrep"] = kw::ENDREP;
            hash["exitrep"] = kw::EXITREP;
            hash["macro"] = kw::MACRO;
            hash["imacro"] = kw::IMACRO;
            hash["endmacro"] = kw::ENDMACRO;
            hash["rotate"] = kw::ROTATE;
            hash["push"] = kw::PUSH;
            hash["pop"] = kw::POP;
            hash["repl"] = kw::REPL;
        }
    }
    bool GetLine(std::string& line)
    {
        std::string last;
        while (1)
        {
            if (!GetPreLine(line))
                return false;
            if (!last.empty())
                line = last + " " + line;
            origLine = line;
        preprocess:
            size_t n = line.find_first_not_of(" \n\t\v\r");
            if (n != std::string::npos)
            {
                if ((line[n] == ppStart && (n + 1 < line.length() && line[n + 1] != ppStart)) ||
                    (trigraphs && n < line.size() - 1 && line[n] == '%' && line[n + 1] == ':'))
                {
                    int n1 = line.find_first_not_of(" \n\t\v\r", n + 1);
                    if (n1 != std::string::npos)
                    {
                        if (!isdigit(line[n1]))
                        {
                            if (trigraphs)
                                line = StripDigraphs(line);
                            Tokenizer<kw> tk(line.substr(n + 1), &hash);
                            const Token* t = tk.Next();
                            if (t->IsKeyword())
                            {
                                if (t->GetKeyword() == kw::ASSIGN || t->GetKeyword() == kw::IASSIGN)
                                {
                                    if (include.Skipping())
                                        line.erase(0, line.size());
                                    else
                                    {
                                        int npos = line.find("assign");
                                        if (npos != std::string::npos)
                                        {
                                            npos = line.find_first_not_of(" \t\r\n\v", npos + 6);
                                            if (npos != std::string::npos)
                                            {
                                                npos = line.find_first_of(" \t\r\n\v", npos);
                                                if (npos != std::string::npos)
                                                {
                                                    std::string left = line.substr(0, npos);
                                                    std::string right = line.substr(npos);
                                                    define.Process(right);
                                                    line = left + right;
                                                }
                                            }
                                        }
                                    }
                                    return true;
                                }
                                else
                                {
                                    line = tk.GetString();
                                    kw token = t->GetKeyword();
                                    // must come first
                                    if (!include.Check(token, line))
                                    {
                                        if (!define.Check(token, line))
                                        {
                                            if (!ppErr.Check(token, line))
                                            {
                                                if (!pragma.Check(token, line))
                                                {
                                                    if (!ppEmbed.Check(token, line))
                                                    {
                                                        if (ppStart != '%' ||
                                                            (!macro.Check(token, line) && !ctx.Check(token, line)))
                                                            Errors::Error("Unknown preprocessor directive");
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (ppStart == '%')
                                {
                                    goto join;
                                }
                                if (!include.Skipping())
                                {
                                    Errors::Error("Invalid preprocessor directive");
                                }
                            }
                            line.erase(0, line.size());
                        }
                        else
                        {
                            goto join;
                        }
                    }
                    else
                    {
                        line.erase(0, line.size());
                    }
                }
                else
                {
                join:
                    if (ppStart != '%' && include.Skipping())
                    {
                        line.erase(0, line.size());
                        break;
                    }
                    if (define.Process(line, true) != INT_MIN + 1)
                    {
                        if (ppStart == '%')
                        {
                            size_t n = line.find_first_not_of(" \n\t\v\r");
                            if (n != std::string::npos)
                            {
                                if ((line[n] == ppStart && (n + 1 < line.length() && line[n + 1] != ppStart)) ||
                                    (trigraphs && n < line.size() - 1 && line[n] == '%' && line[n + 1] == ':'))
                                    goto preprocess;
                            }
                            if (include.Skipping())
                            {
                                line.erase(0, line.size());
                            }
                        }
                        break;
                    }
                    last = line;
                }
            }
        }
        return true;
    }

    const std::string& GetOrigLine() { return origLine; }
    const std::string& GetErrFile() { return include.GetErrFile(); }
    const std::deque<typename ppDefine<T, isSymbolChar>::TokenPos>& TokenPositions() { return define.TokenPositions(); }
    int GetErrLineNo() { return include.GetErrLineNo(); }
    const std::string& GetRealFile() { return include.GetRealFile(); }
    int GetRealLineNo() { return include.GetRealLineNo(); }
    int GetMainLineNo() { return lineno; }
    void Define(const std::string name, std::string value, bool caseInsensitive = false)
    {
        define.Define(name, value, nullptr, false, false, false, caseInsensitive);
    }
    void Undefine(std::string name) { define.Undefine(name); }
    SymbolTable& GetDefines() { return define.GetDefines(); }
    int GetFileIndex() { return include.GetFileIndex(); }
    void CompilePragma(const std::string& val) { return pragma.ParsePragma(val); }

    int GetPack() { return pragma.Pack(); }
    int GetStdPragmas() { return pragma.StdPragmas(); }
    void MarkStdPragma() { pragma.Mark(); }
    void ReleaseStdPragma() { pragma.Release(); }
    void SetPragmaCatchall(std::function<void(const std::string&, const std::string&)> callback)
    {
        pragma.SetPragmaCatchall(callback);
    }
    void SetExpressionHandler(typename ppExpr<T, isSymbolChar>::CompilerExpression* handler) { ppExpr::SetExpressionHandler(handler); }
    int GetCppPrio() { return pragma.CppPrio(); }
    std::list<std::string>& GetIncludeLibs() { return pragma.IncludeLibs(); }
    std::set<std::string>& GetUserIncludes() { return include.GetUserIncludes(); }
    std::set<std::string>& GetSysIncludes() { return include.GetSysIncludes(); }
    std::map<std::string, std::unique_ptr<Startups::Properties>>& GetStartups() { return pragma.GetStartups(); }
    const char* LookupAlias(const char* name) const { return pragma.LookupAlias(name); }
    void IncludeFile(const std::string& name) { include.IncludeFile(name); }
    int GetCtxId() { return ctx.GetTopId(); }
    int GetMacroId() { return macro.GetMacroId(); }
    void Assign(std::string& name, int value, bool caseInsensitive) { define.Assign(name, value, caseInsensitive); }
    bool InMacro() { return macro.InMacro(); }
    void SetPreData(const char* data) { preData = data; }
    bool GetPreLine(std::string& line)
    {
        if (!preData.empty())
        {
            int npos = preData.find_first_of("\n");
            if (npos == std::string::npos)
            {
                line = preData;
                preData = "";
            }
            else
            {
                line = preData.substr(0, npos);
                preData.erase(0, npos + 1);
            }
            return true;
        }
        if (ppStart == '%' && macro.GetLine(line, lineno))
        {
            return true;
        }
        if (ppEmbed.GetLine(line, lineno))
        {
            return true;
        }
        if (include.GetLine(line, lineno))
        {
            return true;
        }
        return false;
    }

    std::string StripDigraphs(std::string line)
    {
        if (line.size() < 2)
            return line;
        std::string rv;
        int last = 0, cp;
        for (cp = 0; cp < line.size() - 1; cp++)
        {
            int ch = 0;
            if (line[cp] == '<' && line[cp + 1] == ':')
            {
                ch = '[';
            }
            else if (line[cp] == ':' && line[cp + 1] == '>')
            {
                ch = ']';
            }
            else if (line[cp] == '<' && line[cp + 1] == '%')
            {
                ch = '{';
            }
            else if (line[cp] == '%' && line[cp + 1] == '>')
            {
                ch = '}';
            }
            else if (line[cp] == '%' && line[cp + 1] == ':')
            {
                ch = '#';
            }
            if (ch)
            {
                static char msg[2] = {0, 0};
                msg[0] = ch;
                if (last != cp)
                {
                    rv += line.substr(last, cp - last) + msg;
                }
                else
                {
                    rv += msg;
                }
                cp++;
                last = cp + 1;
            }
        }
        if (last == 0)
            return line;
        rv += line.substr(last, line.size() - last);
        return rv;
    }
    std::string StripTrigraphs(std::string line);
    // this returns the state of the current input file, only means main file when no include files are opened.
    bool IsOpen() const { return include.IsOpen(); }
    void SetEmbedCallback(std::function<void(std::vector<embeder_size>)> embed_func) { ppEmbed.set_embed_function(embed_func); }

  private:
    bool trigraphs;
    char ppStart;
    ppCtx<T, isSymbolChar> ctx;
    ppMacro<T, isSymbolChar> macro;
    ppInclude<T, isSymbolChar> include;
    ppDefine<T, isSymbolChar> define;
    ppError ppErr;
    ppPragma<T, isSymbolChar> pragma;
    KeywordHash hash;
    embeder<T, isSymbolChar, embed_elements> ppEmbed;
    int lineno;
    std::string preData;
    std::string origLine;
};

#endif