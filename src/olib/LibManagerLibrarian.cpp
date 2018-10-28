/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
#include "ObjIeee.h"
#include "ObjFactory.h"
#include <string.h>

int LibManager::SaveLibrary()
{
    InitHeader();
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    if (!files.ReadFiles(stream, &fact1))
        return CANNOT_READ;
    dictionary.CreateDictionary(files);
    // can't do more reading
    Close();
    header.filesInModule = files.size();
    FILE* ostr = fopen(name.c_str(), "wb");
    if (!ostr)
    {
        return CANNOT_CREATE;
    }
    fwrite(&header, sizeof(header), 1, ostr);
    Align(ostr, 16);
    header.namesOffset = ftell(ostr);
    files.WriteNames(ostr);
    Align(ostr);
    header.filesOffset = ftell(ostr);
    files.WriteFiles(ostr, ALIGN);
    Align(ostr);
    header.offsetsOffset = ftell(ostr);
    files.WriteOffsets(ostr);
    Align(ostr);
    header.dictionaryOffset = ftell(ostr);
    header.dictionaryBlocks = 0;
    dictionary.Write(ostr);
    fseek(ostr, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, ostr);
    int rv = ferror(ostr) ? CANNOT_WRITE : SUCCESS;
    fclose(ostr);
    return rv;
}
void LibManager::Align(FILE* ostr, ObjInt align)
{
    char buf[ALIGN];
    memset(buf, 0, align);
    int n = ftell(ostr);
    if (n % align)
        fwrite(buf, align - (n % align), 1, ostr);
}
