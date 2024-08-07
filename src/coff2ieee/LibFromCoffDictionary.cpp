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

#include "LibFromCoffDictionary.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ObjSection.h"
#include "Utils.h"
#include <cctype>
#include <iostream>
#include <cstring>
#include "CoffLibrary.h"

void LibDictionary::CreateDictionary(std::map<int, std::unique_ptr<Module>>& Modules)
{
    int files = 0;
    int total = 0;
    int symbols = 0;
    Clear();
    for (auto& m : Modules)
    {
        if (!m.second->ignore)
        {
            for (auto alias : m.second->aliases)
            {
                int n = strlen(alias.c_str()) + 1;
                if (n & 1)
                    n++;
                total += n;
                symbols++;
            }
            if (!m.second->import)
                files++;
        }
    }
    int dictpages2, dictpages1;
    int i = 0;
    for (auto& m : Modules)
    {
        if (!m.second->ignore)
        {
            for (auto alias : m.second->aliases)
            {
                InsertInDictionary(alias.c_str(), m.second->import ? files : i);
            }
            i++;
        }
    }
}
void LibDictionary::InsertInDictionary(const char* name, int index)
{
    bool put = false;
    if (!strcmp(name, "_WinMain@16"))
        name = "WinMain";
    char buf[2048];
    int l = strlen(name);
    strncpy(buf, name, 2048);
    buf[2047] = 0;
    if (!caseSensitive)
        for (int i = 0; i <= l; i++)
            buf[i] = toupper(buf[i]);
    dictionary[buf] = index;
}
void LibDictionary::Write(FILE* stream)
{
    char sig[4] = {'1', '0', 0, 0};
    fwrite(&sig[0], 4, 1, stream);
    for (auto d : dictionary)
    {
        short len = d.first.size();
        fwrite(&len, sizeof(len), 1, stream);
        fwrite(d.first.c_str(), len, 1, stream);
        ObjInt fileNum = d.second;
        fwrite(&fileNum, sizeof(fileNum), 1, stream);
    }
    short eof = 0;
    fwrite(&eof, sizeof(eof), 1, stream);
}
