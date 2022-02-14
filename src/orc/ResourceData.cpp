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

#include "ResourceData.h"
#include "RCFile.h"
#include "ResFile.h"

#include <stdexcept>
void ResourceData::WriteRes(ResFile& resFile)
{
    if (data)
        resFile.WriteData(data.get(), len);
}
void ResourceData::ReadRC(RCFile& rcFile)
{
    if (rcFile.IsKeyword())
    {
        int maxLen = 1024;
        data = std::make_unique<unsigned char[]>(maxLen);
        rcFile.NeedBegin();
        while (rcFile.IsNumber())
        {
            if (len == maxLen)
            {
                maxLen += 1024;
                unsigned char* hold = new unsigned char[maxLen];
                memcpy(hold, data.get(), maxLen - 1024);
                data.reset(hold);
            }
            data[len++] = rcFile.GetNumber() & 0xff;
            rcFile.SkipComma();
        }
        rcFile.NeedEnd();
    }
    else
    {
        std::string name = rcFile.GetFileName();
        std::fstream in(name, std::ios::in | std::ios::binary);
        if (!in.fail())
        {
            in.seekg(0, std::ios::end);
            len = in.tellg();
            in.seekg(0, std::ios::beg);
            data = std::make_unique<unsigned char[]>(len);
            in.read((char*)data.get(), len);
        }
        else
            throw std::runtime_error(std::string("Could not open file '") + name + "'");
    }
}
