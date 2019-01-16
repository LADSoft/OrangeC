/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include <string>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "exefmt\MZHeader.h"
#include "exefmt\peHeader.h"

size_t stubSize;
size_t oldOffset;
unsigned char *stubData;
size_t exeSize;
size_t oldStubSize;
unsigned char *exeData;
bool Load(const std::string &exeName)
{
    char buf[512];
    std::fstream file(exeName.c_str(), std::ios::in | std::ios::binary | std::ios::out);
    if (!file.is_open())
    {
        return false;
    }
    MZHeader mzHead;
    file.read((char *)&mzHead, sizeof(mzHead));
    if (mzHead.signature == MZ_SIGNATURE)
    {
        file.seekg(0x3c, std::ios::beg);
        int datapos;
        file.read((char *)&datapos, 4);
        file.seekg(datapos, std::ios::beg);
        file.read((char *)buf, 512);
        if (!memcmp(buf + 0xf8, "UPX0",4) && !memcmp(buf + 0x120, "UPX1", 4) && !memcmp(buf + 0x1e0, "UPX!", 4))
        {
            memcpy(buf +0xf8,".code", 5);
            memcpy(buf+0x120,".data", 5);
            memcpy(buf+0x1e0,"LSC*",4);
            memset(buf+0x1db,0,4);
            if (!memcmp(buf + 0x148,"UPX2", 4))
                memcpy(buf + 0x148, ".bss", 4);
            file.seekp(datapos);
            file.write((char *)buf, 512);
            return true;
        }
    }
    return false;
        
}
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: renseg filename");
        return 1;
    }
    if (!Load(argv[1]))
    {
        printf("Invalid exe");
        return 1;
    }
    return 0;
}