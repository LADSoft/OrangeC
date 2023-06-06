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

class PreProcessor
{
  public:
    PreProcessor(const std::string& FileName, const std::string& SrchPth, const std::string& SysSrchPth, bool fullName,
                 bool Trigraph, char PPStart, bool isunsignedchar, bool C89, bool extensions, bool NoErr,
                 const std::string& pipeName) :
        ppStart(PPStart),
        lineno(0),
        include(fullName, Trigraph, extensions, isunsignedchar, C89, SrchPth, SysSrchPth, PPStart == '%', NoErr, pipeName),
        define(extensions, &include, C89, PPStart == '%'),
        macro(include, define),
        ctx(define),
        trigraphs(Trigraph),
        pragma(&include, &define)
    {
        InitHash();
        Errors::SetInclude(&include);
        macro.SetPreProcessor(this);
        include.SetParams(FileName, &define, &ctx);
        define.SetParams(&ctx, &macro);
    }

    void InitHash();
    bool GetLine(std::string& line);
    const std::string& GetOrigLine() { return origLine; }
    const std::string& GetErrFile() { return include.GetErrFile(); }
    const std::deque<ppDefine::TokenPos>& TokenPositions() { return define.TokenPositions(); }
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
    void SetExpressionHandler(ppExpr::CompilerExpression* handler) { ppExpr::SetExpressionHandler(handler); }
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
    bool GetPreLine(std::string& line);
    int GetAnonymousIndex() { return include.AnonymousIndex(); }
    std::string StripDigraphs(std::string line);
    std::string StripTrigraphs(std::string line);
    // this returns the state of the current input file, only means main file when no include files are opened.
    bool IsOpen() const { return include.IsOpen(); }

  private:
    bool trigraphs;
    char ppStart;
    ppCtx ctx;
    ppMacro macro;
    ppInclude include;
    ppDefine define;
    ppError ppErr;
    ppPragma pragma;
    KeywordHash hash;
    int lineno;
    std::string preData;
    std::string origLine;
};

#endif