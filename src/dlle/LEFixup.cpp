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
#include "LEFixup.h"
#include "LEObject.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjExpression.h"
#include "Utils.h"
#include "LEHeader.h"
int LEFixup::SectionOf(ObjExpression *e)
{
    if (!e)
        return -1;
    if (e->GetOperator() == ObjExpression::eSection)
    {
        return e->GetSection()->GetIndex();
    }
    int n1 = SectionOf(e->GetLeft());
    if (n1 >= 0)
        return n1;
    n1 = SectionOf(e->GetRight());
    if (n1 >= 0)
        return n1;
    return -1;
}
bool LEFixup::IsRel(ObjExpression *e)
{
    if (!e)
        return false;
    if (e->GetOperator() == ObjExpression::ePC)
        return true;
    return IsRel(e->GetLeft()) || IsRel(e->GetRight());
}
void LEFixup::LoadFixups(ObjFile &file)
{
    pages = 0;
    for (std::deque<LEObject *>::iterator it = objects.begin(); it != objects.end(); ++it)
    {
        if (lx)
            pages += ObjectAlign(4096, (*it)->GetSize())/4096;
        else
            pages += ObjectAlign(4096, (*it)->GetInitSize())/4096;
    }
    int i = 0;
    for (ObjFile::SectionIterator it = file.SectionBegin(); it != file.SectionEnd(); ++it)
    {
        ObjInt base = (*it)->GetOffset()->Eval(0);
        ObjMemoryManager &m = (*it)->GetMemoryManager();
        int ofs = 0;
        for (ObjMemoryManager::MemoryIterator it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
        {
            int msize = (*it)->GetSize();
            ObjByte *mdata = (*it)->GetData();
            if (msize)
            {
                ObjExpression *fixup = (*it)->GetFixup();
                if (fixup && !IsRel(fixup))
                {
                    if (msize != 4)
                        Utils::fatal("Invalid fixup type");
                    int so = SectionOf(fixup);
                    if (so >= 0)
                    {
                        int n = fixup->Eval(0) - objects[so]->GetAddr();
                        fixups[base + ofs] = Target(n, so, i);
                        // in the section we put a segment relative offset
                        // DOS32A doesn't need it, but UPX depends on it...
                        (*it)->SetFixup(new ObjExpression(n));
                    }
                }
                ofs += msize;
            }
        }		
        i++;
    }
    CreateSections();
}
void LEFixup::Setup()
{
        
    int page = 0;
    int foffs = 0;
    int size = 0;
    int sect = -1;
    int base = 0;
    std::map<ObjInt, Target>::iterator it = fixups.begin();
    for (page = 0; page < pages; page++)
    {
        ((unsigned *)indexTable)[page] = foffs;
        if (it != fixups.end())
        {
            if (sect != it->second.origSection)
            {
                sect = it->second.origSection;
                base = objects[sect]->GetAddr();
                size = 0;
            }
            ObjInt addr = it->first - base;
            while (addr < size + 4096 && sect == it->second.origSection)
            {
                unsigned char bf[8];
                int bfs = 0;
                bf[bfs++] = LX_FM_OFFSET32;
                ObjInt n = it->second.target;
                if (n < 65536)
                    bf[bfs++] = LX_FT_INTERNAL;
                else
                    bf[bfs++] = LX_FT_INTERNAL | LX_FF_TARGET32;
                *(unsigned short *)(bf + bfs) = addr - size;
                bfs+= sizeof(unsigned short);
                bf[bfs++] = it->second.section + 1;
                if (n < 65536)
                {
                    *(unsigned short *)(bf + bfs) = n;
                    bfs+= sizeof(unsigned short);
                }
                else
                {
                    *(unsigned *)(bf + bfs) = n;
                    bfs+= sizeof(unsigned);
                }
                memcpy(fixupTable + foffs, bf, bfs);
                foffs += bfs;
                // if the address straddles a page boundary, it has to show up in both pages
                if (addr + 3 >= size + 4096)
                    break;
                ++it;
                if (it == fixups.end())
                    break;
                addr = it->first - base;
            }
            // pad for end size != initsize in LX module
            if (it == fixups.end() || sect != it->second.section)
            {
                if (lx)
                {
                    int is = ObjectAlign(4096, objects[sect]->GetInitSize())/ 4096;
                    int s = ObjectAlign(4096, objects[sect]->GetSize())/ 4096;
                    while (s != is)
                    {
                        ((unsigned *)indexTable)[++page] = foffs;
                        is++;					
                    }
                }
            }
            size += 4096;
        }
    }
    ((unsigned *)indexTable)[page] = foffs;	
}
unsigned LEFixup::CreateSections()
{
    fixupSize = 0;
    indexTableSize = (pages + 1) * sizeof(unsigned);
    indexTable = new unsigned char[indexTableSize];
    for (std::map<ObjInt, Target>::iterator it = fixups.begin(); it != fixups.end(); ++it)
    {
        int size;
        if (it->second.target >= 65536)
            size = 9;
        else
            size = 7;
        fixupSize += size;
        // if a fixup crosses a page boundary, it must reside in both pages
        if ((it->first & ~4095) != ((it->first + 3) & ~4095))
            fixupSize += size;
    }
    fixupTable = new unsigned char[fixupSize];
    return 0;
}
void LEFixup::Write(std::fstream &stream)
{
    stream.write((char *)indexTable, indexTableSize);
    stream.write((char *)fixupTable, fixupSize);
}
