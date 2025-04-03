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

#include "Instruction.h"
#include "Label.h"
#include "Fixup.h"
#include "AsmFile.h"
#include <iostream>

bool Instruction::bigEndian;

Instruction::Instruction(std::shared_ptr<Label>& lbl) :
    label(lbl), type(LABEL), altdata(nullptr), pos(0), fpos(0), size(0), offs(0), repeat(1), xrepeat(1), lost(false)
{
}
Instruction::Instruction(void* dataIn, int Size, bool isData) :
    type(isData ? DATA : CODE),
    label(nullptr),
    pos(0),
    fpos(0),
    repeat(1),
    xrepeat(1),
    size(Size),
    offs(0),
    lost(false),
    data(LoadData(!isData, (unsigned char*)dataIn, Size)),
    altdata(nullptr)
{
}
Instruction::Instruction(int aln) :
    type(ALIGN), label(nullptr), altdata(nullptr), pos(0), fpos(0), size(aln), offs(0), repeat(1), xrepeat(1), lost(false)
{
}
Instruction::Instruction(int Repeat, int Size) :
    type(RESERVE),
    label(nullptr),
    altdata(nullptr),
    pos(0),
    fpos(0),
    size(Size),
    repeat(Repeat),
    xrepeat(Repeat),
    offs(0),
    data(std::make_unique<unsigned char[]>(Size)),
    lost(false)
{
    memset(data.get(), 0, size);
}
Instruction::Instruction(void* data) :
    type(ALT), label(nullptr), altdata(data), pos(0), fpos(0), size(0), offs(0), repeat(1), xrepeat(1), lost(false)
{
}

Instruction::~Instruction() {}
std::unique_ptr<unsigned char[]> Instruction::LoadData(bool isCode, unsigned char* data, size_t size)
{
#ifdef x64
    bool found = !isCode;
#else
    bool found = true;
#endif
    std::unique_ptr<unsigned char[]> rv = std::make_unique<unsigned char[]>(size);
    unsigned char* d = rv.get();
    for (unsigned char* s = data; size; size--)
    {
        if (found || (*s & 0xf0) != 0x40)  // null REX prefix
            *d++ = *s++;
        else
        {
            found = true;
            if (*s == 0x40)
            {
                this->size--;
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
    return rv;
}
void Instruction::Add(std::shared_ptr<Fixup> fixup)
{
    // taking ownership now...
    fixups.push_back(fixup);
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
        memcpy(buf, data.get(), sz);
        pos += sz;
        xrepeat--;
        return sz;
    }
    else if (type == ALIGN)
    {
        int sz = GetSize();
        if (sz == 0 || pos != 0)
            return 0;
        switch (fillWidth)
        {
            case 1:
            default:
                memset(buf, fill, sz);
                break;
            case 2:
                for (int i = 0; i < sz; i += 2)
                    *(unsigned short*)(buf + i) = fill;
                if (sz & 1)
                {
                    buf[sz - 1] = fill;
                }
                break;
            case 4:
                for (int i = 0; i < sz; i += 4)
                    *(unsigned*)(buf + i) = fill;
                switch (sz & 3)
                {
                    case 1:
                        *(unsigned short*)(sz - 3) = fill;
                        *(unsigned char*)(sz - 1) = fill >> 16;
                        break;
                    case 2:
                        *(unsigned short*)(sz - 2) = fill;
                        break;
                    case 3:
                        *(unsigned char*)(sz - 1) = fill;
                        break;
                }
                break;
        }
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
            *buf++ = data.get()[i];
        }
        pos = top;
        return rv;
    }
}
FixupContainer* Instruction::GetFixups() { return &fixups; }
