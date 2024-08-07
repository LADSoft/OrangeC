/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef Fixup_h
#define Fixup_h

#include <vector>
#include <string>

class AsmExprNode;

class Fixup
{
  public:
    Fixup() : expr(nullptr), rel(0), size(0), relOffs(0), resolved(false), insOffs(0), errorLine(0), adjustable(false),
        canceled() {}
    Fixup(std::shared_ptr<AsmExprNode>& Expr, int Size, int Rel, int RelOffs = 0, bool Adjustable = false) :
        expr(Expr), rel(Rel), size(Size), relOffs(RelOffs), resolved(false), insOffs(0), errorLine(0), adjustable(Adjustable),
        canceled()
    {
    }
    void SetRel(bool Rel) { rel = Rel; }
    bool IsRel() { return rel; }
    void SetRelOffs(int ofs) { relOffs = ofs; }
    int GetRelOffs() { return relOffs; }
    void SetSize(int sz) { size = sz; }
    int GetSize() { return size; }
    void SetInsOffs(int offs) { insOffs = offs; }
    int GetInsOffs() { return insOffs; }
    std::shared_ptr<AsmExprNode> GetExpr() { return expr; }
    void SetExpr(std::shared_ptr<AsmExprNode>& newExpr) { expr = newExpr; }
    void SetResolved() { resolved = true; }
    bool IsResolved() { return resolved; }
    void SetAdjustable(bool adj) { adjustable = adj; }
    bool IsAdjustable() { return adjustable; }
    void SetErrorLine(int line) { errorLine = line; }
    int GetErrorLine() { return errorLine; }
    void SetFileName(std::string& name) { fileName = name; }
    std::string GetFileName() { return fileName; }
    void SetCanceled(bool val) { canceled = val; }
    bool IsCanceled() const { return canceled; }
  private:
    bool canceled;
    std::shared_ptr<AsmExprNode> expr;
    int errorLine;
    std::string fileName;
    int rel;
    int size;
    int relOffs;
    int insOffs;
    bool resolved;
    bool adjustable;
};
#endif
