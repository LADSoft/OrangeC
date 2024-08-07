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

#include "LEFixup.h"
#include "LEObject.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjExpression.h"
#include "Utils.h"
#include "LEHeader.h"
int LEFixup::SectionOf(ObjExpression* e)
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
bool LEFixup::IsRel(ObjExpression* e)
{
    if (!e)
        return false;
    if (e->GetOperator() == ObjExpression::ePC)
        return true;
    return IsRel(e->GetLeft()) || IsRel(e->GetRight());
}
void LEFixup::LoadFixups(ObjFile& file)
{
    pages = 0;
    for (auto obj : objects)
    {
        if (lx)
            pages += ObjectAlign(4096, obj->GetSize()) / 4096;
        else
            pages += ObjectAlign(4096, obj->GetInitSize()) / 4096;
    }
    int i = 0;
    for (auto it = file.SectionBegin(); it != file.SectionEnd(); ++it)
    {
        ObjInt base = (*it)->GetOffset()->Eval(0);
        ObjMemoryManager& m = (*it)->GetMemoryManager();
        int ofs = 0;
        for (auto&& mem : m)
        {
            int msize = mem->GetSize();
            ObjByte* mdata = mem->GetData();
            if (msize)
            {
                ObjExpression* fixup = mem->GetFixup();
                if (fixup && !IsRel(fixup))
                {
                    if (msize != 4)
                        Utils::Fatal("Invalid fixup type");
                    int so = SectionOf(fixup);
                    if (so >= 0)
                    {
                        int n = fixup->Eval(0) - objects[so]->GetAddr();
                        fixups[base + ofs] = Target(n, so, i);
                        // in the section we put a segment relative offset
                        // DOS32A doesn't need it, but UPX depends on it...
                        mem->SetFixup(new ObjExpression(n));
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
    auto it = fixups.begin();
    for (page = 0; page < pages; page++)
    {
        ((unsigned*)indexTable.get())[page] = foffs;
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
                unsigned char bf[16];
                int bfs = 0;
                bf[bfs++] = LX_FM_OFFSET32;
                ObjInt n = it->second.target;
                if (n < 65536)
                    bf[bfs++] = LX_FT_INTERNAL;
                else
                    bf[bfs++] = LX_FT_INTERNAL | LX_FF_TARGET32;
                *(unsigned short*)(bf + bfs) = addr - size;
                bfs += sizeof(unsigned short);
                bf[bfs++] = it->second.section + 1;
                if (n < 65536)
                {
                    *(unsigned short*)(bf + bfs) = n;
                    bfs += sizeof(unsigned short);
                }
                else
                {
                    *(unsigned*)(bf + bfs) = n;
                    bfs += sizeof(unsigned);
                }
                memcpy(fixupTable.get() + foffs, bf, bfs);
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
                    int is = ObjectAlign(4096, objects[sect]->GetInitSize()) / 4096;
                    int s = ObjectAlign(4096, objects[sect]->GetSize()) / 4096;
                    while (s != is)
                    {
                        ((unsigned*)indexTable.get())[++page] = foffs;
                        is++;
                    }
                }
            }
            size += 4096;
        }
    }
    ((unsigned*)indexTable.get())[page] = foffs;
}
unsigned LEFixup::CreateSections()
{
    fixupSize = 0;
    indexTableSize = (pages + 1) * sizeof(unsigned);
    indexTable = std::make_unique<unsigned char[]>(indexTableSize);
    for (auto fixup : fixups)
    {
        int size;
        if (fixup.second.target >= 65536)
            size = 9;
        else
            size = 7;
        fixupSize += size;
        // if a fixup crosses a page boundary, it must reside in both pages
        if ((fixup.first & ~4095) != ((fixup.first + 3) & ~4095))
            fixupSize += size;
    }
    fixupTable = std::make_unique<unsigned char[]>(fixupSize);
    return 0;
}
void LEFixup::Write(std::fstream& stream)
{
    stream.write((char*)indexTable.get(), indexTableSize);
    stream.write((char*)fixupTable.get(), fixupSize);
}
