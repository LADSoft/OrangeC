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
#ifndef OBJINDEXMANAGER_H
#define OBJINDEXMANAGER_H

#include "ObjTypes.h"

class ObjFile;

class ObjIndexManager : public ObjWrapper
{
public:
    ObjIndexManager() { ResetIndexes(); }
    virtual ~ObjIndexManager() {}
    virtual void ResetIndexes();
    virtual ObjInt NextSection() { return Section++; }
    virtual ObjInt GetSection() { return Section; }
    virtual void SetSection(ObjInt section) { Section = section; }
    virtual ObjInt NextPublic() { return Public++; }
    virtual ObjInt GetPublic() { return Public; }
    virtual void SetPublic(ObjInt ipublic) { Public = ipublic; }
    virtual ObjInt NextLocal() { return Local++; }
    virtual ObjInt GetLocal() { return Local; }
    virtual void SetLocal(ObjInt local) { Local = local; }
    virtual ObjInt NextAuto() { return Auto++; }
    virtual ObjInt GetAuto() { return Auto; }
    virtual void SetAuto(ObjInt iauto) { Auto = iauto; }
    virtual ObjInt NextReg()  { return Reg++; }
    virtual ObjInt GetReg()  { return Reg; }
    virtual void SetReg(ObjInt reg) { Reg = reg; }
    virtual ObjInt NextExternal() { return External++; }
    virtual ObjInt GetExternal() { return External; }
    virtual void SetExternal(ObjInt external) { External = external; }
    virtual ObjInt NextType() { return Type++; }
    virtual ObjInt GetType() { return Type; }
    virtual void SetType(ObjInt type) { Type = type; }
    virtual ObjInt NextFile() { return File++; }
    virtual ObjInt GetFile() { return File; }
    virtual void SetFile(ObjInt file) { File = file; }
    virtual void LoadIndexes(ObjFile *file) ;
protected:
    ObjInt Section;
    ObjInt Public;
    ObjInt Local;
    ObjInt External;
    ObjInt Auto;
    ObjInt Reg;
    ObjInt Type;
    ObjInt File;
    
};
#endif
