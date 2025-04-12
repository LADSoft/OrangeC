/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#ifndef ppInclude_h
#define ppInclude_h

#include <list>
#include <string>
#include <unordered_set>
#include "ppExpr.h"
#include "ppFile.h"
#include "PipeArbitrator.h"
#include "ppCommon.h"

class ppFile;
class ppDefine;
class ppCtx;

class ppInclude
{
  public:
    ppInclude(bool Fullname, bool Trigraph, bool extended, bool isunsignedchar, Dialect dialect_, const std::string& SrchPth,
              const std::string& SysSrchPth, bool Asmpp, bool NoErr, const std::string& pipeName) :
        unsignedchar(isunsignedchar),
        dialect(dialect_),
        trigraphs(Trigraph),
        asmpp(Asmpp),
        define(nullptr),
        ctx(nullptr),
        extendedComment(extended || dialect_ != Dialect::c89),
        fullname(Fullname),
        current(nullptr),
        expr(unsignedchar, dialect_),
        forcedEOF(false),
        nextIndex(0),
        piper(pipeName),
        noErr(NoErr),
        systemNesting(0)
    {
        ppExpr::SetInclude(this);
        srchPath = SrchPth;
        sysSrchPath = SysSrchPth;
    }

    ~ppInclude();
    void SetParams(const std::string& Name, ppDefine* Define, ppCtx* Ctx)
    {
        define = Define;
        ctx = Ctx;
        expr.SetParams(define);
        pushFile(Name, Name, false, false);
    }
    bool Check(kw token, const std::string& line);
    bool IsOpen() const
    {
        if (current)
            return current->IsOpen();
        else
            return false;
    }
    int GetErrLineNo()
    {
        if (current)
            return current->GetErrorLine();
        return 0;
    }
    const std::string& GetErrFile()
    {
        if (current)
            return current->GetErrorFile();
        return dummy;
    }
    int GetRealLineNo()
    {
        if (current)
            return current->GetRealLine();
        return 0;
    }
    const std::string& GetRealFile()
    {
        if (current)
            return current->GetRealFile();
        return dummy;
    }
    bool GetLine(std::string& line, int& lineno);
    bool Skipping()
    {
        if (current)
            return current->Skipping();
        return false;
    }
    int GetFileIndex() const
    {
        if (current)
            return current->GetIndex();
        return false;
    }
    void IncludeFile(const std::string& name) { pushFile(name, name, false, false); }
    void SetInProc(const std::string& name) { inProc = name; }
    void Mark() { current->Mark(); }
    void Drop() { current->Drop(); }
    void Release() { current->Release(); }
    bool has_include(const std::string& args);
    bool has_include_next(const std::string& args);
    void ForceEOF() { forcedEOF = true; }
    std::set<std::string>& GetUserIncludes() { return userIncludes; }
    std::set<std::string>& GetSysIncludes() { return sysIncludes; }
    void EnterGccSystemHeader();

    static void SetCommentChar(char ch) { commentChar = ch; }
    // Put a throwaway value in dirs_skipped here unless you need to use it for #include_next shenanigans with pushFile
    std::string FindFile(bool specifiedAsSystem, const std::string& name, bool skipFirst, int& dirs_skipped, bool& foundAsSystem,
                         bool& found);
    std::string SrchPath(bool system, const std::string& name, const std::string& searchPath, bool skipUntilDepth,
                         int& filesSkipped);
    std::string ParseName(const std::string& args, bool& specifiedAsSystem);

  protected:
    void StripAsmComment(std::string& line);
    bool CheckInclude(kw token, const std::string& line);
    bool CheckLine(kw token, const std::string& line);
    void pushFile(const std::string& name, const std::string& errname, bool include_next, bool foundAsSystem,
                  int dirs_traversed = 0);
    bool popFile();
    const char* RetrievePath(char* buf, const char* path);
    void AddName(char* buf, const std::string& name);

  private:
    static bool system;
    std::unordered_set<std::string> gccSystemHeaders;
    std::list<std::unique_ptr<ppFile>> files;
    std::set<std::string> userIncludes;
    std::set<std::string> sysIncludes;
    std::unique_ptr<ppFile> current;
    std::unordered_map<std::string, int> fileMap;
    ppDefine* define;
    bool unsignedchar;
    Dialect dialect;
    bool trigraphs;
    bool extendedComment;
    bool fullname;
    ppExpr expr;
    static std::string srchPath, sysSrchPath;
    ppCtx* ctx;
    std::string inProc;
    bool asmpp;
    bool forcedEOF;
    bool noErr;
    std::string dummy;
    int nextIndex;
    PipeArbitrator piper;
    static char commentChar;
    int systemNesting;
};
#endif