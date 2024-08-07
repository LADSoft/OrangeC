/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "LibDictionary.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ObjSection.h"
#include "Utils.h"
#include <cctype>
#include <iostream>
#include <cstring>
#include "UTF8.h"

void LibDictionary::CreateDictionary(LibFiles& files)
{
    int total = 0;
    int symbols = 0;
    Clear();
    int i = 0;
    for (auto&& fd : files)
    {
        if (fd->data)
        {
            for (auto pi = fd->data->PublicBegin(); pi != fd->data->PublicEnd(); ++pi)
            {
                InsertInDictionary((*pi)->GetName().c_str(), i);
            }
            for (auto pi = fd->data->ImportBegin(); pi != fd->data->ImportEnd(); ++pi)
            {
                if (static_cast<ObjImportSymbol*>(*pi)->GetDllName().size())
                    InsertInDictionary((*pi)->GetName().c_str(), i);
            }
            // support for virtual sections
            for (auto si = fd->data->SectionBegin(); si != fd->data->SectionEnd(); ++si)
            {
                if ((*si)->GetQuals() & ObjSection::virt)
                {
                    int j;
                    std::string name = (*si)->GetName();
                    for (j = 0; j < name.size(); j++)
                        if (name[j] == '@')
                            break;
                    if (j < name.size())
                    {
                        InsertInDictionary(name.c_str()+ j, i);
                        if (strncmp((*si)->GetName().c_str(), "vsb@", 4) == 0)
                        {
                            InsertInDictionary(name.c_str() + j + 1, i);
                        }
                    }
                }
            }
            i++;
        }
    }
}
void LibDictionary::InsertInDictionary(const char* name, int index)
{
    char buf[2048];
    int l = strlen(name);
    int n = l + 1;
    strncpy(buf, name, 2048);
    buf[2047] = 0;
    std::string id = buf;
    if (!caseSensitive)
        id = UTF8::ToUpper(id);
    dictionary[id].push_back(index);
}
bool LibDictionary::Write(FILE* stream)
{
    char sig[4] = {'1', '1', 0, 0};
    if (fwrite(&sig[0], 4, 1, stream) != 1)
        return false;
    for (auto d : dictionary)
    {
        short len = d.first.size();
        if (fwrite(&len, sizeof(len), 1, stream) != 1)
            return false;
        if (fwrite(d.first.c_str(), len, 1, stream) != 1)
            return false;
        auto&& list = d.second;
        unsigned fileNum;
        for (int i = 0; i < list.size() - 1; i++)
        {
            fileNum = list[i] | DictionaryContinuationFlag;
            if (fwrite(&fileNum, sizeof(fileNum), 1, stream) != 1)
                return false;
        }
        fileNum = list[list.size() - 1];
        if (fwrite(&fileNum, sizeof(fileNum), 1, stream) != 1)
            return false;
    }
    short eof = 0;
    if (fwrite(&eof, sizeof(eof), 1, stream) != 1)
        return false;
    return true;
}
