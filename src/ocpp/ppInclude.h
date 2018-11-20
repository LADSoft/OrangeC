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

#ifndef ppInclude_h
#define ppInclude_h

#include <list>
#include <string>

#include "ppExpr.h"
#include "ppFile.h"

class ppFile;
class ppDefine;
class ppCtx;

class ppInclude
{
  public:
    ppInclude(bool Fullname, bool Trigraph, bool extended, bool isunsignedchar, bool C89, const std::string& SrchPth,
              const std::string& SysSrchPth, bool Asmpp) :
        unsignedchar(isunsignedchar),
        c89(C89),
        trigraphs(Trigraph),
        asmpp(Asmpp),
        define(nullptr),
        ctx(nullptr),
        extendedComment(extended || !C89),
        fullname(Fullname),
        current(nullptr),
        expr(unsignedchar),
        forcedEOF(false)
    {
        srchPath = SrchPth;
        sysSrchPath = SysSrchPth;
    }

    ~ppInclude();
    void SetParams(const std::string& Name, ppDefine* Define, ppCtx* Ctx)
    {
        define = Define;
        ctx = Ctx;
        expr.SetParams(define);
        pushFile(Name, Name);
    }
    bool Check(int token, const std::string& line);
    int GetLineNo()
    {
        if (current)
            return current->GetErrorLine();
        else
            return 0;
    }
    std::string GetFile()
    {
        if (current)
            return current->GetErrorFile();
        else
            return "";
    }
    bool GetLine(std::string& line, int& lineno);
    bool Skipping()
    {
        if (current)
            return current->Skipping();
        else
            return false;
    }
    void IncludeFile(const std::string& name) { pushFile(name, name); }
    void SetInProc(const std::string& name) { inProc = name; }
    void Mark() { current->Mark(); }
    void Drop() { current->Drop(); }
    void Release() { current->Release(); }
    static bool __has_include(const std::string &args);
    void ForceEOF() { forcedEOF = true; }
  protected:
    void StripAsmComment(std::string& line);
    bool CheckInclude(int token, const std::string& line);
    bool CheckLine(int token, const std::string& line);
    void pushFile(const std::string& name, const std::string& errname);
    bool popFile();
    static std::string ParseName(const std::string& args);
    static std::string FindFile(const std::string& name);
    static std::string SrchPath(bool system, const std::string& name);
    static const char* RetrievePath(char* buf, const char* path);
    static void AddName(char* buf, const std::string& name);

  private:
    static bool system;
    std::list<ppFile*> files;
    ppFile* current;
    ppDefine* define;
    bool unsignedchar;
    bool c89;
    bool trigraphs;
    bool extendedComment;
    bool fullname;
    ppExpr expr;
    static std::string srchPath, sysSrchPath;
    ppCtx* ctx;
    std::string inProc;
    bool asmpp;
    bool forcedEOF;
};
#endif