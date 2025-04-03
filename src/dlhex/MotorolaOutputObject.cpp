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

void MotorolaOutputObject::putdatarec(std::fstream& stream, unsigned char* data, int datalen, long offset)
{
    int cs = 0, i;
    cs += offset & 255;
    cs += (offset >> 8) & 255;
    cs += datalen;
    cs += 3;
    int len = datalen + 3;
    switch (type)
    {
        case 1:
            stream << "S1" << ObjUtil::ToHex(len, 2) << ObjUtil::ToHex(offset & 0xffff, 4);
            break;
        case 2:
            len += 1;
            cs++;
            cs += (offset >> 16) & 255;
            stream << "S2" << ObjUtil::ToHex(len, 2) << ObjUtil::ToHex(offset & 0xffffff, 6);
            break;
        case 3:
            len += 2;
            cs += 2;
            cs += (offset >> 16) & 255;
            cs += (offset >> 24) & 255;
            stream << "S3" << ObjUtil::ToHex(len, 2) << ObjUtil::ToHex(offset & 0xffffffff, 8);
            break;
    }
    for (i = 0; i < datalen; i++)
    {
        stream << ObjUtil::ToHex(data[i], 2);
        cs += data[i];
    }
    cs = 255 - (cs & 255);
    stream << ObjUtil::ToHex(cs, 2) << std::endl;
}
void MotorolaOutputObject::putendrec(std::fstream& stream)
{
    int cs = 0;
    int entryPoint = 0;
    cs += entryPoint & 255;
    cs += (entryPoint >> 8) & 255;
    cs += 3; /* record size */
    switch (type)
    {
        case 1:
            stream << "S903" << ObjUtil::ToHex(entryPoint & 0xffff, 4);
            break;
        case 2:
            cs += 1 + (entryPoint >> 16) & 255;
            stream << "S805" << ObjUtil::ToHex(entryPoint & 0xffffff, 6);
            break;
        case 3:
            cs += 1 + (entryPoint >> 16) & 255;
            cs += 1 + (entryPoint >> 24) & 255;
            stream << "S709" << ObjUtil::ToHex(entryPoint & 0xffffffff, 8);
            break;
    }
    cs = 255 - (cs & 255);
    stream << ObjUtil::ToHex(cs, 2) << std::endl;
}
void MotorolaOutputObject::putheaderrec(std::fstream& stream)
{
    int cs = 0;
    int len = name.size() + 5;
    cs += len;
    stream << "S0" << ObjUtil::ToHex(len, 2) << "0000";
    const char* p = name.c_str();
    while (*p)
    {
        stream << ObjUtil::ToHex(*p);
        cs += *p++;
    }
    cs = 255 - (cs & 255);
    stream << ObjUtil::ToHex(cs) << std::endl;
}
int MotorolaOutputObject::Write(std::fstream& stream, char* data, int len, int firstAddress)
{
    while (len)
    {
        int max = 16;
        if (type == 3)
            max = 15;
        int m = len > max ? max : len;
        putdatarec(stream, (unsigned char*)data, m, firstAddress);
        firstAddress += m;
        data += m;
        len -= m;
    }
    return 0;
}
int MotorolaOutputObject::WriteHeader(std::fstream& stream)
{
    putheaderrec(stream);
    return 0;
}
int MotorolaOutputObject::WriteTrailer(std::fstream& stream)
{
    putendrec(stream);
    return 0;
}
