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

#include "OutputObjects.h"
#include <cstring>

int OutputObject::Pad(std::fstream& stream, ObjInt addr, ObjInt size, ObjInt padChar)
{
    char buf[256];
    memset(buf, padChar, sizeof(buf));
    for (ObjInt i = 0; i < size; i += sizeof(buf))
    {
        if (size - i < sizeof(buf))
            Write(stream, buf, size - i, addr + i);
        else
            Write(stream, buf, sizeof(buf), addr + i);
    }
    return 0;
}
int BinaryOutputObject::Write(std::fstream& stream, char* data, int len, int firstAddress)
{
    (void)firstAddress;
    stream.write(data, len);
    return 0;
}
