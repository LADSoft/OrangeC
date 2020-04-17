/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include "PEObject.h"
#include "ObjSection.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ResourceContainer.h"
#include <cstring>

unsigned PEObject::objectAlign = 0x1000;
unsigned PEObject::fileAlign = 0x200;
unsigned PEObject::imageBase = 0x400000;
unsigned PEObject::dataInitSize;
unsigned PEObject::importThunkVA;
unsigned PEObject::importCount;

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
    char buf[512];
    memset(buf, 0, sizeof(buf));
    while (n > 0)
    {
        int s = sizeof(buf);
        if (n < s)
            s = n;
        stream.write(buf, s);
        n -= s;
    }
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
