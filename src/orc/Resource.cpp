/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include "Resource.h"
#include "ResFile.h"
#include "RCFile.h"

void Point::WriteRes(ResFile& resFile)
{
    resFile.WriteWord(x);
    resFile.WriteWord(y);
}
void Resource::WriteRes(ResFile& resFile)
{
    resFile.Align();
    resFile.Mark();
    char buf[2 * sizeof(int)];  // assumes 32 bit ints
    memset(buf, 0, sizeof(buf));
    resFile.WriteData((unsigned char*)buf, sizeof(buf));
    type.WriteRes(resFile);
    id.WriteRes(resFile);
    resFile.Align();
    resInfo.WriteRes(resFile);
    resFile.MarkHeader();
}
