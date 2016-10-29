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
#include "ObjIndexManager.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjSymbol.h"
#include "ObjType.h"
#include "ObjSourceFile.h"
void ObjIndexManager::ResetIndexes()
{
    Section = 0;
    Public = 0;
    Local = 0;
    External = 0;
    Type = 0;
    File = 0;
    Auto = 0;
    Reg = 0;
}
void ObjIndexManager::LoadIndexes(ObjFile *file)
{
    ResetIndexes();
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Public)
            Public = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::SymbolIterator it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        if ((*it)->GetIndex() >= External)
            External = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::SymbolIterator it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Local)
            Local = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::SymbolIterator it = file->AutoBegin(); it != file->AutoEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Auto)
            Auto = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::SymbolIterator it = file->RegBegin(); it != file->RegEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Reg)
            Reg = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Section)
            Section = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Type)
            Type = (*it)->GetIndex() + 1;
    } 
    for (ObjFile::SourceFileIterator it = file->SourceFileBegin(); it != file->SourceFileEnd(); ++it)
    {
        if ((*it)->GetIndex() >= File)
            File = (*it)->GetIndex() + 1;
    } 
}
