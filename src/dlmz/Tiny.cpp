/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

bool Tiny::ReadSections(ObjFile* file, ObjExpression* start)
{
    startOffs = start->Eval(0);
    if (startOffs != 0x100)
        Utils::Fatal("Start address for tiny program must be 0100h");
    int count = 0;
    ObjSection* sect;
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        sect = *it;
        size = (*it)->GetOffset()->Eval(0) + (*it)->GetSize()->Eval(0);
        count++;
    }
    if (count != 1 || size < 0x100)
        return false;
    data = std::make_unique<unsigned char[]>(size);
    unsigned char* pdata = data.get();
    ObjMemoryManager& m = sect->GetMemoryManager();
    int ofs = 0;
    for (auto&& mem : m)
    {
        int msize = mem->GetSize();
        ObjByte* mdata = mem->GetData();
        if (msize)
        {
            ObjExpression* fixup = mem->GetFixup();
            if (fixup)
            {
                if (fixup->GetOperator() == ObjExpression::eDiv)
                    Utils::Fatal("Tiny program cannot have fixups");
                int sbase = sect->GetOffset()->Eval(0);
                int n = fixup->Eval(sbase + ofs);
                int bigEndian = file->GetBigEndian();
                if (msize == 1)
                {
                    pdata[ofs] = n & 0xff;
                }
                else if (msize == 2)
                {
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
                if (mem->IsEnumerated())
                    memset(pdata + ofs, mem->GetFill(), msize);
                else
                    memcpy(pdata + ofs, mdata, msize);
            }
            ofs += msize;
        }
    }
    return true;
}
bool Tiny::Write(std::fstream& stream)
{
    stream.write((char*)data.get() + 0x100, size - 0x100);
    return true;
}
