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

#include "LibManager.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include <cassert>

ObjFile* LibFiles::ReadData(FILE* stream, const ObjString& name, ObjFactory* factory)
{
    ObjIeeeIndexManager im1;
    ObjIeee ieee(name.c_str(), caseSensitive);
    return ieee.Read(stream, ObjIeee::eAll, factory);
}
bool LibFiles::ReadNames(FILE* stream, int count)
{
    for (int i = 0; i < count; i++)
    {
        char c = ' ';
        char buf[260], *p = buf;
        do
        {
            c = fgetc(stream);
            //            if (stream.fail())
            //                return false;
            *p++ = c;
        } while (c != 0);
        ObjString name = buf;
        files.push_back(std::make_unique<FileDescriptor>(name));
    }
    return true;
}
void LibFiles::ReadOffsets(FILE* stream, int count)
{
    assert(count == files.size());
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        unsigned ofs;
        fread(&ofs, 4, 1, stream);
        (*it)->offset = ofs;
    }
}
ObjFile* LibFiles::LoadModule(FILE* stream, ObjInt FileIndex, ObjFactory* factory)
{
    if (FileIndex >= files.size())
        return nullptr;
    auto& a = files[FileIndex];
    if (!a->offset)
        return nullptr;
    fseek(stream, a->offset, SEEK_SET);
    return ReadData(stream, a->name, factory);
}
