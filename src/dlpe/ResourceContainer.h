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

#ifndef ResourceContainer_h
#define ResourceContainer_h

#include <string>
#include <map>
#include <deque>

class ResourceContainer
{
  public:
    ResourceContainer() {}
    ~ResourceContainer();
    void AddFile(std::string name) { names.push_back(name); }
    bool LoadFiles();

    size_t size() { return names.size(); }
    bool empty() { return names.size() == 0; }
#define SIGNAT 0x494e4753  //'SG' + ('NI' << 16)
    struct ResourceData
    {
        unsigned sig;
        unsigned char* data;
        int length;
        int language;
    };
    typedef std::map<std::wstring, ResourceData> NamedIds;
    typedef std::map<int, ResourceData> NumberedIds;
    struct Ids
    {
        NamedIds namedIds;
        NumberedIds numberedIds;
    };
    typedef std::map<std::wstring, Ids> NamedTypes;
    typedef std::map<int, Ids> NumberedTypes;
    NamedTypes namedTypes;
    NumberedTypes numberedTypes;

  protected:
    int GetId(unsigned short* hdrdata, int& i, int& id, std::wstring& name);
    bool LoadFile(const std::string& name);
    void InsertResource(int typeId, std::wstring type, int nameId, std::wstring name, unsigned char* data, size_t len,
                        int language);
    std::deque<std::string> names;
    static unsigned char resourceHeader[32];
};
#endif
