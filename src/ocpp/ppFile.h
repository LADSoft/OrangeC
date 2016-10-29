/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef ppFile_h
#define ppFile_h

#include <string>
#include <stdio.h>

#include "InputFile.h"
#include "ppCond.h"

class ppDefine;
class ppCtx;
class ppFile : public InputFile
{
    enum { LINE_WIDTH = 4096 };
public:
    ppFile(bool fullname, bool Trigraph, bool extended, const std::string &Name, ppDefine *define, ppCtx &Ctx, bool isunsignedchar, bool c89, bool asmpp) : 
        InputFile(fullname, Name), trigraphs(Trigraph), extendedComment(extended),
        cond(isunsignedchar, c89, extended, asmpp), ctx(Ctx) { cond.SetParams(define, &ctx); }
    virtual ~ppFile() { }
    virtual bool GetLine(std::string &line);
    bool Check(int token, const std::string &line, int lineno)
    { return cond.Check(token, line, lineno); }
    bool Skipping() { return cond.Skipping(); }
    void Mark() { cond.Mark(); }
    void Drop() { cond.Drop(); }
    void Release() { cond.Release(); }
protected:
    virtual int StripComment(char *line);
    void StripTrigraphs(char *line);
private:
    bool trigraphs;
    bool extendedComment;
    ppCond cond;
    ppCtx &ctx;
} ;
#endif