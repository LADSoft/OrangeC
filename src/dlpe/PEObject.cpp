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

#include "PEObject.h"
#include "ObjSection.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ResourceContainer.h"
#include <cstring>
#include "Utils.h"
#include <algorithm>

unsigned PEObject::objectAlign = 0x1000;
unsigned PEObject::fileAlign = 0x200;
unsigned PEObject::imageBase = 0x400000;
unsigned PEObject::dataInitSize;
DWORD PEObject::importThunkVA;
unsigned PEObject::importCount;
DWORD PEObject::imageBaseVA;
std::shared_ptr<unsigned char> PEObject::codeData;
DWORD PEObject::codeRVA;
DWORD PEObject::delayLoadHandleRVA;
DWORD PEObject::delayLoadThunkRVA;
std::vector<DWORD> PEObject::thunkFixups;

const char* PEObject::importThunk = "\xff\x25\x00\x00\x00\x00";  // jmp near [through an iat entry]

const char* PEObject::delayLoadThunk =
    "\xb8\x00\x00\x00\x00"   // mov eax, iatoffset
    "\xe9\x00\x00\x00\x00";  // jmp rel to another thunk

const char* PEObject::delayLoadModuleThunk =
    "\x52"                  // push edx
    "\x51"                  // push ecx
    "\x50"                  // push eax (from the preceding thunk)
    "\x68\x00\x00\x00\x00"  // push dword constant
    "\xE8\x00\x00\x00\x00"  // call relative
    "\x59"                  // pop ecx
    "\x5a"                  // pop edx
    "\xFF\xE0";             // jmp eax

std::vector<std::string> PEObject::delayLoadNames;

ObjFile* PEObject::file;

void PEObject::WriteHeader(std::fstream& stream)
{
    unsigned zero = 0;
    for (int i = 0; i < name.size() && i < 8; i++)
        stream.write(&name[i], 1);
    for (int i = name.size(); i < 8; i++)
        stream.write((char*)&zero, 1);
    unsigned msize = ObjectAlign(objectAlign, size);
    stream.write((char*)&msize, 4);
    stream.write((char*)&virtual_addr, 4);
    msize = ObjectAlign(fileAlign, initSize);
    stream.write((char*)&msize, 4);
    stream.write((char*)&raw_addr, 4);
    stream.write((char*)&zero, 4);
    stream.write((char*)&zero, 4);
    stream.write((char*)&zero, 4);
    int flg = (flags ^ WINF_NEG_FLAGS) & WINF_IMAGE_FLAGS; /* get characteristice for section */
    stream.write((char*)&flg, 4);
}
void PEObject::Write(std::fstream& stream)
{
    stream.write((char*)data.get(), initSize);
    int n = ObjectAlign(fileAlign, initSize) - initSize;
    char buf[512] = {};
    while (n > 0)
    {
        int s = sizeof(buf);
        if (n < s)
            s = n;
        stream.write(buf, s);
        n -= s;
    }
}
void PEObject::SetDelayLoadNames(std::string names)
{
    delayLoadNames = Utils::split(names);
    for (auto&& n : delayLoadNames)
        std::transform(n.begin(), n.end(), n.begin(), ::toupper);
}
void PEObject::SetFile(ObjFile* File)
{
    file = File;
    for (auto it = file->DefinitionBegin(); it != file->DefinitionEnd(); it++)
    {
        ObjDefinitionSymbol* p = (ObjDefinitionSymbol*)(*it);
        if (p->GetName() == "FILEALIGN")
        {
            fileAlign = p->GetValue();
        }
        else if (p->GetName() == "IMAGEBASE")
        {
            imageBase = p->GetValue();
        }
        else if (p->GetName() == "IMPORTTHUNKS")
        {
            importThunkVA = p->GetValue();
        }
        else if (p->GetName() == "IMPORTCOUNT")
        {
            importCount = p->GetValue();
        }
        else if (p->GetName() == "____imageBase")
        {
            imageBaseVA = p->GetValue();
        }
        else if (p->GetName() == "INITSIZE")
        {
            dataInitSize = p->GetValue();
        }
        else if (p->GetName() == "OBJECTALIGN")
        {
            objectAlign = p->GetValue();
        }
    }
}
