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
#ifndef LINKLIBRARY_H
#define LINKLIBRARY_H

#include "ObjTypes.h"
#include <map>
#include <set>
#include "LibManager.h"
class ObjSymbol;
class ObjFactory;
class ObjFile;

class LinkLibrary
{
public:
    LinkLibrary(const ObjString &Name, bool CaseSensitive) : name(Name), manager(name, CaseSensitive) {}
    ~LinkLibrary() { Close() ;}
    ObjString GetName() { return name; }
    ObjInt GetSymbol(const ObjString &name) { return manager.Lookup(name); }
    ObjFile *LoadSymbol(ObjInt objNum, ObjFactory *factory) { loadedModules.insert(objNum); return manager.LoadModule(objNum, factory); }
    void Close() { manager.Close(); }
    bool IsOpen() { return manager.IsOpen(); }
    bool fail() { return manager.fail(); }
    bool Load() { return manager.LoadLibrary(); }
    bool HasModule(ObjInt objNum) { return loadedModules.find(objNum) != loadedModules.end(); }

private:
    ObjString name;
    LibManager manager;
    std::set<ObjInt> loadedModules;
};
#endif
