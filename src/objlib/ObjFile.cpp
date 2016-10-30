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
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjSymbol.h"

void ObjFile ::ResolveSymbols(ObjFactory *Factory)
{
    for (SectionIterator it = SectionBegin(); it != SectionEnd(); ++it)
        (*it)->ResolveSymbols(Factory);
}	
void ObjFile ::Add(ObjSymbol *Symbol)
{
    switch(Symbol->GetType())
    {
        case ObjSymbol::ePublic:
            publics.push_back(Symbol);
            break;
        case ObjSymbol::eExternal:
            externals.push_back(Symbol);
            break;
        case ObjSymbol::eLocal:
            locals.push_back(Symbol);
            break;
        case ObjSymbol::eAuto:
            autos.push_back(Symbol);
            break;
        case ObjSymbol::eReg:
            regs.push_back(Symbol);
            break;
        case ObjSymbol::eImport:
            imports.push_back(Symbol);
            break;
        case ObjSymbol::eExport:
            exports.push_back(Symbol);
            break;
        case ObjSymbol::eDefinition:
            definitions.push_back(Symbol);
            break;
        default:
            break;
    }
}
ObjSection *ObjFile ::FindSection(const ObjString Name)
{
    for (auto sect : sections)
        if (sect->GetName() == Name)
            return sect;
    return nullptr;
}
