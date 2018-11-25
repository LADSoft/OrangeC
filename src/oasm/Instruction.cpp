/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#include "Instruction.h"
#include "Label.h"
#include "Fixup.h"
#include "AsmFile.h"
#include <iostream>

bool Instruction::bigEndian;

Instruction::~Instruction()
{
    if (data)
        delete data;
    for (int i = 0; i < fixups.size(); i++)
    {
        Fixup* f = fixups[i];
        delete f;
    }
}
unsigned char * Instruction::LoadData(bool isCode, unsigned char *data, size_t size)
{
#ifdef x64
    bool found = !isCode;
#else
    bool found = true;
#endif
    unsigned char *d = new unsigned char[size], *rv = d;
    for (unsigned char *s=data; size; size--)
    {
        if (found || (*s & 0xf0) != 0x40)// null REX prefix
            *d++ = *s++;
        else
        {
            found = true;
            if (*s == 0x40)
            {
                lost = true;
                s++;
            }
            else
            {
                *d++ = *s++;
            }
        }
    }
    if (!found)
        std::cerr << "Diag: missing REX prefix" << std::endl;
    if (lost)
    {
        this->size--;
    }
    return rv;
}

int Instruction::GetNext(Fixup& fixup, unsigned char* buf)
{
    if (type == RESERVE)
    {
        if (pos == 0)
            xrepeat = repeat;
        if (xrepeat == 0)
            return 0;
        int sz = size;
        memcpy(buf, data, sz);
        pos += sz;
        xrepeat--;
        return sz;
    }
    else if (type == ALIGN)
    {
        int sz = GetSize();
        if (sz == 0 || pos != 0)
            return 0;
        memset(buf, 0, sz);
        pos += sz;
        return sz;
    }
    else
    {
        if (pos >= size)
            return 0;
        int top = size;
        if (fpos < fixups.size())
        {
            top = fixups[fpos]->GetInsOffs();
            if (top == pos)
            {
                if (!fixups[fpos]->IsResolved())
                {
                    pos += fixups[fpos]->GetSize();
                    fixup = *(fixups[fpos++]);
                    return -1;
                }
                else
                {
                    fpos++;
                    if (fpos < fixups.size())
                        top = fixups[fpos]->GetInsOffs();
                    else
                        top = size;
                }
            }
        }
        int rv = top - pos;
        for (int i = pos; i < top; i++)
        {
            *buf++ = data[i];
        }
        pos = top;
        return rv;
    }
}
