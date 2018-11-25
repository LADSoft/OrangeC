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

#include "LibDictionary.h"
#include "ObjFile.h"
#include <cctype>
#include <iostream>

bool DictCompare::caseSensitive;

ObjInt DictCompare::casecmp(const char* str1, const char* str2, int n) const
{
    while (*str1 && *str2 && n)
    {
        int u1 = *str1;
        int u2 = *str2;
        if (!caseSensitive)
        {
            u1 = toupper(u1);
            u2 = toupper(u2);
        }
        if (u1 != u2)
            break;
        str1++, str2++, n--;
    }
    if (n == 0)
    {
        if (!*str2)
            return 0;
        return -1;
    }
    else if (!*str2)
        return 1;
    return *str1 < *str2 ? -1 : 1;
}
ObjInt LibDictionary::Lookup(FILE* stream, ObjInt dictionaryOffset, ObjInt dictionarySize, const ObjString& name)
{
    if (dictionary.empty())
    {
        fseek(stream, 0, SEEK_END);
        int end = ftell(stream);
        int size = end - dictionaryOffset;
        ObjByte* buf = new ObjByte[size];
        fseek(stream, dictionaryOffset, SEEK_SET);
        fread(buf, size, 1, stream);
        ObjByte* q = buf;
        char sig[4] = {'1', '0', 0, 0}, sig1[4];
        if (!memcmp(sig, buf, 4))
        {
            int len;
            q += 4;
            len = *(short*)q;
            while (len)
            {
                q += 2;
                int fileNum = *(int*)(q + len);
                q[len] = 0;
                dictionary[(char*)q] = fileNum;
                q += len + 4;
                len = *(short*)q;
            }
        }
        else
        {
            std::cout << "Old format library detected, please rebuild libraries" << std::endl;
        }
        delete[] buf;
    }
    auto it = dictionary.find(name);
    if (it != dictionary.end())
        return it->second;
    return -1;
}
