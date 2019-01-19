/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include "LibDictionary.h"
#include "ObjFile.h"
#include <cctype>
#include <iostream>

bool DictCompare::caseSensitive;

ObjInt DictCompare::casecmp(const char* str1, const char* str2, int n) const
{
    int v;
    if (caseSensitive)
        v = _strnicmp(str1, str2, n);
    else
        v = strncmp(str1, str2, n);
    if (v == 0)
    {
        int l = str1[n], r = str2[n];
        if (l < r)
            return -1;
        else if (l > r)
            return 1;
        else
            return 0; // n is the length of one of the strings, so, this is the nulls...
    }
    return v;
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
