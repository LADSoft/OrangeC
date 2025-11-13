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

#include "LibManager.h"
#include <cstring>

void LibManager::InitHeader()
{
    header = {};
    header.sig = LibHeader::LIB_SIG;
}
bool LibManager::LoadLibrary()
{
    if (fseek(stream, 0, SEEK_END))
        return false;
    int len = ftell(stream);
    if (len < 0)
        return false;
    memset(&header, 0, sizeof(header));
    if (fseek(stream, 0, SEEK_SET))
        return false;
    if (fread((char*)&header, sizeof(header), 1, stream) != 1)
    {
        return false;
    }
    // attempt to shut up coverity
    if (feof(stream))
        return false;
    if (header.sig != LibHeader::LIB_SIG)
        return false;
    if ((int)header.namesOffset >= len || (int)header.namesOffset < 0 || fseek(stream, header.namesOffset, SEEK_SET))
        return false;
    // this next is a random value to keep coverity happy...
    if ((int)header.filesInModule > 100000 || (int)header.filesInModule < 0 || !files.ReadNames(stream, header.filesInModule))
        return false;
    if ((int)header.offsetsOffset >= len || (int)header.offsetsOffset < 0 || fseek(stream, header.offsetsOffset, SEEK_SET))
        return false;
    if (!files.ReadOffsets(stream, header.filesInModule))
        return false;
    return true;
}
const std::vector<unsigned>& LibManager::Lookup(const ObjString& name)
{
    if (header.sig == LibHeader::LIB_SIG)
        return dictionary.Lookup(stream, header.dictionaryOffset, header.dictionaryBlocks, name);
    static std::vector<unsigned> empty;
    return empty;
}
