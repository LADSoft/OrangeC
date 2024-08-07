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
        int c;
        char buf[260], *p = buf;
        int n = 0;
        do
        {
            if (++n > 259)
                return false;
            c = fgetc(stream);
            if (c < 0)
                return false;
            *p++ = (char)c;
        } while (c != 0);
        ObjString name = buf;
        files.push_back(std::make_unique<FileDescriptor>(name));
    }
    return true;
}
bool LibFiles::ReadOffsets(FILE* stream, int count)
{
    assert(count == files.size());
    for (auto&& file : *this)
    {
        unsigned ofs;
        if (fread(&ofs, 4, 1, stream) != 1)
            return false;
        file->offset = ofs;
    }
    return true;
}
ObjFile* LibFiles::LoadModule(FILE* stream, ObjInt FileIndex, ObjFactory* factory)
{
    if (FileIndex >= files.size())
        return nullptr;
    auto& a = files[FileIndex];
    if (!a->offset)
        return nullptr;
    if (fseek(stream, a->offset, SEEK_SET))
        return nullptr;
    return ReadData(stream, a->name, factory);
}
