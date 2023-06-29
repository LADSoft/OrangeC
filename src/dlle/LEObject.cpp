/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 */

#include <fstream>
#include "LEObject.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjExpression.h"
#include "LEHeader.h"
#include <cstring>
ObjFile* LEObject::file;

bool LEObject::IsRel(ObjExpression* e)
{
    if (!e)
        return false;
    if (e->GetOperator() == ObjExpression::ePC)
        return true;
    return IsRel(e->GetLeft()) || IsRel(e->GetRight());
}
void LEObject::Setup(unsigned& offs)
{
    pageOffs = offs + 1;
    offs += ObjectAlign(4096, initSize) / 4096;
    data = std::make_unique<unsigned char[]>(initSize);
    ObjMemoryManager& m = sect->GetMemoryManager();
    int ofs = 0;
    unsigned char* pdata = data.get();
    for (auto it = m.MemoryBegin(); it != m.MemoryEnd() && ofs < initSize; ++it)
    {
        int msize = (*it)->GetSize();
        ObjByte* mdata = (*it)->GetData();
        if (msize)
        {
            ObjExpression* fixup = (*it)->GetFixup();
            if (fixup)
            {
                int sbase = sect->GetOffset()->Eval(0);
                int n = fixup->Eval(sbase + ofs);  // need this to apply relative offsets in code seg
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
                if ((*it)->IsEnumerated())
                    memset(pdata + ofs, (*it)->GetFill(), msize);
                else
                    memcpy(pdata + ofs, mdata, msize);
            }
            ofs += msize;
        }
    }
}
void LEObject::InitFlags()
{
    name = sect->GetName();
    initSize = size = sect->GetSize()->Eval(0);
    base_addr = sect->GetOffset()->Eval(0);
    if (name == ".text")
    {
        SetFlags(LX_OF_READABLE | LX_OF_EXECUTABLE | LX_OF_BIGDEFAULT /*| LX_OF_PRELOAD*/);
    }
    else if (name == ".data")
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT);
    }
    else if (name == ".bss")
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT | LX_OF_ZEROFILL);
        initSize = 0;
    }
    else if (name == ".stack")
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT | LX_OF_ZEROFILL);
        initSize = 0;
    }
    else
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT);
    }
}
void LEObject::WriteHeader(std::fstream& stream)
{
    unsigned temp = size;  // ObjectAlign(4096, size);
    stream.write((char*)&temp, sizeof(temp));
    stream.write((char*)&base_addr, sizeof(base_addr));
    stream.write((char*)&flags, sizeof(flags));
    //	if (initSize == 0)
    //		pageOffs = 1;
    stream.write((char*)&pageOffs, sizeof(pageOffs));
    temp = ObjectAlign(4096, initSize) / 4096;
    stream.write((char*)&temp, sizeof(temp));
    temp = 0;
    stream.write((char*)&temp, sizeof(temp));
}
void LEObject::Write(std::fstream& stream)
{
    stream.write((char*)data.get(), initSize);
    int n = ObjectAlign(4096, initSize) - initSize;
    char buf[512];
    memset(buf, 0, sizeof(buf));
    while (n > 0)
    {
        int s = sizeof(buf);
        if (n < sizeof(buf))
            s = n;
        stream.write(buf, s);
        n -= s;
    }
}
void LEObject::SetFile(ObjFile* File) { file = File; }
