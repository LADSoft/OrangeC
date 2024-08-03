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

#ifndef LIBDICTIONARY_H
#define LIBDICTIONARY_H

#include "ObjTypes.h"
#include <cstdio>
#include <unordered_map>
#include <vector>

class ObjFile;
class LibFiles;

struct DictCompare
{
    ObjInt casecmp(const std::string& str1, const std::string& str2, int n) const;

    bool operator()(const ObjString& left, const ObjString& right) const { return casecmp(left, right, left.size()) == 0; }
    static bool caseSensitive;
};
struct DictHash
{
    unsigned long operator()(const ObjString& str) const
    {
        long aa = 0;
        for (auto v : str)
        {
            aa = (aa << 8) + (aa << 2) + aa;
            aa += DictCompare::caseSensitive ? v : toupper(v);
        }
        return aa;
    }
};
class LibDictionary
{
  public:
    const unsigned DictionaryContinuationFlag = 1 << (sizeof(unsigned) * CHAR_BIT - 1);
    typedef std::unordered_map<ObjString, std::vector<unsigned>, DictHash, DictCompare> Dictionary;
    LibDictionary(bool CaseSensitive = true) : caseSensitive(CaseSensitive) { DictCompare::caseSensitive = CaseSensitive; }
    ~LibDictionary() {}
    const std::vector<unsigned>& Lookup(FILE* stream, ObjInt dictOffset, ObjInt dictPages, const ObjString& str);
    bool Write(FILE* stream);
    void CreateDictionary(LibFiles& files);
    void Clear() { dictionary.clear(); }

  protected:
    void InsertInDictionary(const char* name, int index);

  private:
    Dictionary dictionary;
    bool caseSensitive;
};
#endif  // LIBDICTIONARY_H