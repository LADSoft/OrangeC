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

#include "LibDictionary.h"
#include "ObjFile.h"
#include "Utils.h"
#include <cctype>
#include <iostream>
#include <memory>

bool DictCompare::caseSensitive;

ObjInt DictCompare::casecmp(const std::string& str1, const std::string& str2, int n) const
{
    int v;
    if (caseSensitive)
        v = !Utils::iequal(str1, str2, n);
    else
        v = strncmp(str1.c_str(), str2.c_str(), n);
    if (v == 0)
    {
        int l = str1[n], r = str2[n];
        if (l < r)
            return -1;
        else if (l > r)
            return 1;
        else
            return 0;  // n is the length of one of the strings, so, this is the nulls...
    }
    return v;
}
const std::vector<unsigned>& LibDictionary::Lookup(FILE* stream, ObjInt dictionaryOffset, ObjInt dictionarySize, const ObjString& name)
{
    const static std::vector<unsigned> dummy;
    if (dictionary.empty())
    {
        if (fseek(stream, 0, SEEK_END))
            return dummy;
        int end = ftell(stream);
        int size = end - dictionaryOffset;
        std::unique_ptr<ObjByte[]> buf = std::make_unique<ObjByte[]>(size);
        ObjByte* q = buf.get();
        if (fseek(stream, dictionaryOffset, SEEK_SET))
            return dummy;
        if (fread(q, size, 1, stream) != 1)
            return dummy;
        // attempt to shut up coverity
        if (feof(stream))
            return dummy;
        char sig[4] = {'1', '1', 0, 0};
        if (!memcmp(sig, q, 4))
        {
            int len;
            q += 4;
            len = *(short*)q;
            while (len)
            {
                q += 2;
                std::string name = std::string((char *)q, len);
                q += len;
                unsigned fileNum;
                do
                {
                    fileNum = *(unsigned*)(q);
                    q += sizeof(unsigned);
                    dictionary[name].push_back(fileNum & ~DictionaryContinuationFlag);
                } while (fileNum & DictionaryContinuationFlag);
                len = *(short*)q;
            }
        }
        else
        {
            std::cout << "Old format library detected, please rebuild libraries" << std::endl;
        }
    }
    auto it = dictionary.find(name);
    if (it != dictionary.end())
        return it->second;
    return dummy;
}
