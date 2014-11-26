/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#ifndef OBJBROWSEINFO_H
#define OBJBROWSEINFO_H

#include "ObjTypes.h"

class ObjLineNo;
class ObjMemory;

class ObjBrowseInfo : public ObjWrapper
{
public:
    enum eType { eDefine, eVariable, eFileStart, eFuncStart, eFuncEnd, eBlockStart, eBlockEnd };
    enum eQual { eGlobal, eStatic, eExternal, eLocal, ePrototype, eTypeval };
    ObjBrowseInfo(eType Type, eQual Qual, ObjLineNo *Line, ObjInt CharPos, ObjString &Data) :
        type(Type), qual(Qual), line(Line), charpos(CharPos), data(Data) { }
    virtual ~ObjBrowseInfo() { }
                
    ObjLineNo *GetLine() { return line; }
    void SetLine(ObjLineNo * Line) { line = Line; }
    eType GetType() { return type; }
    eQual GetQual() { return qual; }
    void SetType(eType Type) { type = Type; }
    ObjString &GetData() { return data; }
    void SetData (ObjString &Data){ data = Data; }
    ObjLineNo *GetLineNo() { return line; }
    void SetLineNo(ObjLineNo *Line) { line = Line; }				
    ObjInt GetCharPos() { return charpos; }
    void SetCharPos(ObjInt CharPos) { charpos = CharPos; }				   
private:
    ObjLineNo *line;
    ObjInt charpos;
    eType type;
    eQual qual;
    ObjString data;	
} ;
#endif OBJBROWSEINFO_H
