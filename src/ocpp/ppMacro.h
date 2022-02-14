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

#ifndef ppMacro_h
#define ppMacro_h

#include "ppExpr.h"
#include "ppDefine.h"

#include <vector>
#include <string>

class PreProcessor;

struct MacroData
{
    std::vector<std::string> lines;
    int begline;
    int repsLeft;
    int offset;
    int id;  //-1 for rep, else macro invoke # 1- x
    int argmin;
    int argmax;
    bool plus;
    bool isNative;
    bool caseInsensitive;
    std::vector<std::string> defaults;
    std::vector<std::string> args;
};
class ppMacro
{
  public:
    ppMacro(ppInclude& Include, ppDefine& Define);
    ~ppMacro();
    bool Check(kw token, std::string& line);
    bool GetLine(std::string& line, int& lineno);
    bool Invoke(std::string name, std::string line);
    int GetMacroId()
    {
        MacroData* p = GetTopMacro();
        if (p)
            return p->id;
        else
            return -1;
    }
    int GetMacroMax()
    {
        MacroData* p = GetTopMacro();
        if (p)
            return p->argmax;
        else
            return 0;
    }
    std::vector<std::string>* GetMacroArgs()
    {
        MacroData* p = GetTopMacro();
        if (p)
            return &p->args;
        else
            return nullptr;
    }
    bool InMacro() { return !stack.empty(); }
    void SetPreProcessor(PreProcessor* PP) { pp = PP; }

  protected:
    void GetArgs(int max, std::string& line, std::vector<std::string>& vals);
    std::string ExtractArg(std::string& line);
    bool HandleRep(std::string& line);
    bool HandleExitRep();
    bool HandleEndRep();
    bool HandleMacro(std::string& line, bool caseInsensitive);
    bool HandleEndMacro();
    bool HandleRotate(std::string& line);
    MacroData* GetTopMacro();
    void reverse(std::vector<std::string>& x, int offs, int len);

  private:
    ppInclude& include;
    ppDefine& define;
    ppExpr expr;
    PreProcessor* pp;

    std::vector<std::unique_ptr<MacroData>> stack;
    std::unordered_map<std::string, MacroData*> macros;
    int nextMacro;
};

#endif