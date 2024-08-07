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

#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjExpression.h"
#include "OutputFormats.h"
#include "Utils.h"
#include <iostream>

bool Real::ReadSections(ObjFile* file, ObjExpression* start)
{
    size = 0;
    int base = GetFirstSeg(start);
    startOffs = start->Eval(0);
    startSeg = base >> 16;
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        if ((*it)->GetName() == ".stack")
        {
            stackSeg = (*it)->GetOffset()->Eval(0) / 16;
            stackOffs = (*it)->GetSize()->Eval(0);
        }
        else
        {
            int n = (*it)->GetOffset()->Eval(0) + (*it)->GetSize()->Eval(0);
            if (n > size)
                size = n;
        }
    }
    data = std::make_unique<unsigned char[]>(size);
    unsigned char* pdata = data.get();
    int ofs = 0;
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        if ((*it)->GetName() != ".stack")
        {
            ObjSection* sect = *it;
            ObjMemoryManager& m = sect->GetMemoryManager();
            for (auto&& memory : m)
            {
                int msize = memory->GetSize();
                ObjByte* mdata = memory->GetData();
                if (msize)
                {
                    ObjExpression* fixup = memory->GetFixup();
                    if (fixup)
                    {
                        int sbase = sect->GetOffset()->Eval(0);
                        int n = GetFixupOffset(fixup, sbase, ofs);
                        int bigEndian = file->GetBigEndian();
                        if (n < 0)
                            Utils::Fatal("Fixup offset negative");
                        if (msize == 1)
                        {
                            pdata[ofs] = n & 0xff;
                        }
                        else if (msize == 2)
                        {
                            if (n > 65535)
                                Utils::Fatal("16-bit offset outside of segment");
                            if (bigEndian)
                            {
                                pdata[ofs] = n >> 8;
                                pdata[ofs + 1] = n & 0xff;
                            }
                            else
                            {
                                pdata[ofs] = n & 0xff;
                                pdata[ofs + 1] = n >> 8;
                            }
                        }
                        else  // msize == 4
                        {
                            if (bigEndian)
                            {
                                pdata[ofs + 0] = n >> 24;
                                pdata[ofs + 1] = n >> 16;
                                pdata[ofs + 2] = n >> 8;
                                pdata[ofs + 3] = n & 0xff;
                            }
                            else
                            {
                                pdata[ofs] = n & 0xff;
                                pdata[ofs + 1] = n >> 8;
                                pdata[ofs + 2] = n >> 16;
                                pdata[ofs + 3] = n >> 24;
                            }
                        }
                    }
                    else
                    {
                        if (memory->IsEnumerated())
                            memset(pdata + ofs, memory->GetFill(), msize);
                        else
                            memcpy(pdata + ofs, mdata, msize);
                    }
                    ofs += msize;
                }
            }
        }
    }
    return true;
}
bool Real::Write(std::fstream& stream)
{
    if (stackSeg == 0 && stackOffs == 0)
    {
        std::cout << "Warning: No stack segment" << std::endl;
    }
    WriteHeader(stream);
    stream.write((char*)data.get(), size);
    return true;
}
void Real::WriteHeader(std::fstream& stream)
{
    memset(&header, 0, sizeof(header));
    int hdrSize = (sizeof(header) + (fixups.size() * 4) + 15) & ~15;
    int fileSize = hdrSize + size;
    header.signature = MZ_SIGNATURE;
    header.image_length_MOD_512 = fileSize % 512;
    header.image_length_DIV_512 = fileSize / 512;
    if (header.image_length_MOD_512 != 0)
        header.image_length_DIV_512++;
    header.n_relocation_items = fixups.size();
    header.n_header_paragraphs = hdrSize / 16;
    header.min_paragraphs_above = (size + 15) / 16;
    header.max_paragraphs_above = 0xffff;
    header.initial_SS = stackSeg;
    header.initial_SP = stackOffs;
    header.checksum = 0;
    header.initial_CS = startSeg;
    header.initial_IP = startOffs;
    header.offset_to_relocation_table = sizeof(header);
    header.overlay = 0;
    header.always_one = 1;
    stream.write((char*)&header, sizeof(header));
    for (auto fixup : fixups)
    {
        unsigned short dd[2];
        dd[0] = fixup.off;
        dd[1] = fixup.seg;
        stream.write((char*)&dd, sizeof(dd));
    }
    int n = stream.tellg() & 15;
    // pad to end of header
    if (n != 0)
    {
        char buf[16];
        memset(buf, 0, sizeof(buf));
        stream.write(buf, 16 - n);
    }
}
void Real::GetSectionBalance(ObjExpression* n, int& add, int& sub, bool positive)
{
    if (n->GetOperator() == ObjExpression::eSection)
    {
        if (positive)
        {
            add++;
        }
        else
        {
            sub++;
        }
    }
    else if (n->GetOperator() == ObjExpression::eAdd)
    {
        GetSectionBalance(n->GetLeft(), add, sub, positive);
        GetSectionBalance(n->GetRight(), add, sub, positive);
    }
    else if (n->GetOperator() == ObjExpression::eSub)
    {
        GetSectionBalance(n->GetLeft(), add, sub, positive);
        GetSectionBalance(n->GetRight(), add, sub, !positive);
    }
}
bool Real::Balanced(ObjExpression* n, bool skipping)
{
    bool rv = true;
    if (!skipping)
    {
        if (n->GetOperator() == ObjExpression::eAdd || n->GetOperator() == ObjExpression::eSub)
        {
            int add = 0, sub = 0;
            GetSectionBalance(n, add, sub, true);
            rv = add == sub;
            if (rv)
            {
                if (n->GetLeft())
                    rv &= Balanced(n->GetLeft(), true);
                if (n->GetRight())
                    rv &= Balanced(n->GetRight(), true);
            }
        }
        else
        {
            if (n->GetLeft())
                rv &= Balanced(n->GetLeft(), false);
            if (n->GetRight())
                rv &= Balanced(n->GetRight(), false);
        }
    }
    else
    {
        bool next = n->GetOperator() == ObjExpression::eAdd || n->GetOperator() == ObjExpression::eSub;
        if (n->GetLeft())
            rv &= Balanced(n->GetLeft(), next);
        if (n->GetRight())
            rv &= Balanced(n->GetRight(), next);
    }
    return rv;
}
int Real::GetFixupOffset(ObjExpression* fixup, int sbase, int pc)
{
    if (fixup->GetOperator() == ObjExpression::eDiv)
    {
        int base = GetFirstSeg(fixup);
        // segment
        Fixup xx(sbase >> 4, pc - (sbase & ~15));
        fixups.push_back(xx);
        return base >> 4;
    }
    else if ((fixup->GetOperator() == ObjExpression::eSub || fixup->GetOperator() == ObjExpression::eAdd))
    {
        if (Balanced(fixup, false))
        {
            return fixup->Eval(pc);
        }
        else
        {
            int base = GetFirstSeg(fixup);
            return fixup->Eval(pc) - base;
        }
    }
    else
    {
        if (Balanced(fixup, false))
            return fixup->Eval(pc);
        Utils::Fatal("Invalid fixup");
    }
    return 0;
}
int Real::GetFirstSeg(ObjExpression* exp)
{
    ObjExpression* find = exp;
    while (find->GetOperator() == ObjExpression::eAdd || find->GetOperator() == ObjExpression::eSub ||
           find->GetOperator() == ObjExpression::eDiv)
    {
        find = find->GetLeft();
    }
    if (find->GetOperator() != ObjExpression::eSection)
        Utils::Fatal("Invalid fixup");
    return find->GetSection()->GetOffset()->Eval(0) & ~15;
}
