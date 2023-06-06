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

#ifndef ResFile_h
#define ResFile_h

#include <string>
#include <deque>
#include <fstream>
#include <set>
#include "Resource.h"
#include <memory>

class ResFile
{
  public:
    ResFile() {}
    virtual ~ResFile();
    void Mark();
    void MarkHeader();
    void Release();
    void Align();
    void WriteByte(int byte);
    void WriteWord(int word);
    void WriteDWord(int dword);
    void WriteData(const unsigned char* data, int len);
    void WriteString(const std::wstring& str);
    bool Write(const std::string& name);
    void Reset();
    void Add(Resource* th);
    size_t GetPos() { return stream.tellp(); }
    void SetPos(size_t n) { stream.seekp(n); }

  private:
    std::deque<std::unique_ptr<Resource>> resources;
    struct lt
    {
        // in this LT operation, the resource ids are guaranteed to be numeric.
        // we are doing this to force a sort of the string tables when we read
        // them back out
        bool operator()(const std::unique_ptr<Resource>& left, const std::unique_ptr<Resource>& right) const
        {
            return left->GetId().GetId() < right->GetId().GetId();
        }
    };
    // strings are kept in a separate list because they have to be at the end
    // of the file and sorted
    std::set<std::unique_ptr<Resource>, lt> strings;
    int hdrSize;
    int size;
    int base;
    std::fstream stream;
};
#endif
