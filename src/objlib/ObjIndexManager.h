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
    virtual ObjInt NextReg() { return Reg++; }
    virtual ObjInt GetReg() { return Reg; }
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
    virtual void LoadIndexes(ObjFile* file);

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
