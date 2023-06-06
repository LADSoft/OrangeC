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

#ifndef ppFile_h
#define ppFile_h

#include <string>
#include <cstdio>

#include "InputFile.h"
#include "ppCond.h"

class ppDefine;
class ppCtx;
class ppFile : public InputFile
{

  public:
    ppFile(bool fullname, bool Trigraph, bool extended, const std::string& Name, ppDefine* define, ppCtx& Ctx, bool isunsignedchar,
           bool c89, bool asmpp, PipeArbitrator& piper, int directories_travelled = 0) :
        InputFile(fullname, Name, piper),
        trigraphs(Trigraph),
        extendedComment(extended),
        cond(isunsignedchar, c89, extended, asmpp),
        ctx(Ctx),
        anonymousIndex(1),
        directoriesTraversed(directories_travelled)
    {
        cond.SetParams(define, &ctx);
    }
    virtual ~ppFile() {}
    virtual bool GetLine(std::string& line);
    bool Check(kw token, const std::string& line, int lineno) { return cond.Check(token, line, lineno); }
    bool Skipping() { return cond.Skipping(); }
    void Mark() { cond.Mark(); }
    void Drop() { cond.Drop(); }
    void Release() { cond.Release(); }
    int AnonymousIndex() { return anonymousIndex++; }
    int getDirsTravelled() { return directoriesTraversed; }

  protected:
    virtual int StripComment(char* line);
    void StripTrigraphs(char* line);

  private:
    bool trigraphs;
    bool extendedComment;
    ppCond cond;
    ppCtx& ctx;
    int anonymousIndex;
    int directoriesTraversed = 0;
};
#endif