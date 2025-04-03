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

#include "OutputObjects.h"
#include "ObjUtil.h"

void IntelOutputObject::putrecord(std::fstream& stream, unsigned char* data, int datalen, int offset)
{
    int cs = 0, i;
    cs += offset & 255;
    cs += (offset >> 8) & 255;

    cs += datalen;
    stream << ":" << ObjUtil::ToHex(datalen, 2) << ObjUtil::ToHex(offset & 0xffff, 4) << "00";  // data record
    for (i = 0; i < datalen; i++)
    {
        stream << ObjUtil::ToHex(data[i], 2);
        cs += data[i];
    }
    cs = (256 - (cs & 255)) & 0xff;
    stream << ObjUtil::ToHex(cs, 2) << std::endl;
}
void IntelOutputObject::putulba(std::fstream& stream, int address)
{
    int cs = 0, i;
    int len;
    int offset;
    int type;
    if (stype == 4)
    {
        // linear
        type = 4;
        offset = (address >> 16) & 0xffff;
        len = 2;
    }
    else
    {
        // segmented
        type = 2;
        offset = (address >> 4) & 0xf000;
        len = 2;
    }
    cs += type;
    cs += len;
    cs += offset & 255;
    cs += (offset >> 8) & 255;
    cs = (256 - (cs & 255)) & 0xff;

    stream << ":" << ObjUtil::ToHex(len, 2) << "0000" << ObjUtil::ToHex(type, 2) << ObjUtil::ToHex(offset, 4)
           << ObjUtil::ToHex(cs, 2) << std::endl;
}
int IntelOutputObject::Write(std::fstream& stream, char* data, int len, int firstAddress)
{
    if ((firstAddress & 0xffff0000) || stype == 2)
        putulba(stream, firstAddress);
    while (len)
    {
        int m = len > 16 ? 16 : len;
        int n = ((firstAddress + 0x10000) & 0xffff0000) - firstAddress;
        m = n < m ? n : m;
        putrecord(stream, (unsigned char*)data, m, firstAddress);
        firstAddress += m;
        data += m;
        len -= m;
        if (len && (firstAddress & 0xffff) == 0)
            putulba(stream, firstAddress);
    }
    return 0;
}
int IntelOutputObject::WriteHeader(std::fstream& stream) { return 0; }
int IntelOutputObject::WriteTrailer(std::fstream& stream)
{
    stream << ":00000001FF" << std::endl;  // end record
    return 0;
}
