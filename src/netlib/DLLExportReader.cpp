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

#include <stdio.h>
#include <string.h>
#include "DLLExportReader.h"
#include "DotNetPELib.h"
#include "PEHeader.h"
#include "MZHeader.h"

#include "PEFile.h"
using namespace DotNetPELib;

const char* DIR_SEP = "\\";

DLLExportReader::~DLLExportReader()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        DLLExport* p = *it;
        delete p;
    }
}

bool DLLExportReader::FindDLL()
{
    char* p = strrchr((char*)name.c_str(), '.');
    if (!p || p[1] == DIR_SEP[0])
    {
        name += ".dll";
    }
    // just try to open the file
    // this will catch absolute files and files relative to the current directory
    FILE* fil = fopen(name.c_str(), "rb");
    if (fil)
    {
        fclose(fil);
        return true;
    }
    // now look in windows/system32
    p = getenv("SystemRoot");
    if (p)
    {
        char buf[260];
        StrCpy(buf, p);
        if (buf[strlen(buf) - 1] != DIR_SEP[0])
            StrCat(buf, DIR_SEP);
        StrCat(buf, "System32");
        StrCat(buf, DIR_SEP);
        StrCat(buf, name.c_str());
        fil = fopen(buf, "rb");
        if (fil)
        {
            name = buf;
            fclose(fil);
            return true;
        }
    }
    // now search the path
    p = getenv("PATH");
    if (p)
    {
        while (*p)
        {
            char buf[260], *q = buf;
            while (*p && *p != ';')
                *q++ = *p++;
            *q = 0;
            if (*p)
                p++;
            if (buf[strlen(buf) - 1] != DIR_SEP[0])
                StrCat(buf, DIR_SEP);
            StrCat(buf, name.c_str());
            fil = fopen(buf, "rb");
            if (fil)
            {
                name = buf;
                fclose(fil);
                return true;
            }
        }
    }
    return false;
}
bool DLLExportReader::doExports(std::fstream& in, int phys, int rva)
{
    in.seekg(phys);
    if (!in.fail())
    {
        PEExportHeader eh;
        in.read((char*)&eh, sizeof(eh));
        for (int i = 0; i < eh.n_name_ptrs; i++)
        {
            int nameptr;
            short ord = 0;
            char buf[256];
            bool byOrd;
            buf[0] = 0;
            in.seekg(eh.name_rva - rva + phys + i * 4);
            in.read((char*)&nameptr, sizeof(nameptr));
            if (nameptr)
            {
                in.seekg(nameptr - rva + phys);
                in.read(buf, 256);
                buf[255] = 0;  // just in case
                byOrd = false;
            }
            else
            {
                byOrd = true;
            }
            in.seekg(eh.ordinal_rva - rva + phys + i * 2);
            in.read((char*)&ord, sizeof(ord));
            exports.push_back(new DLLExport(buf, ord, byOrd));
        }
    }
    return !in.fail();
}
bool DLLExportReader::LoadExports()
{
    bool rv = false;
    std::fstream in(name.c_str(), std::ios::in | std::ios::binary);
    if (!in.fail())
    {
        MZHeader mzh;
        in.read((char*)&mzh, sizeof(mzh));
        if (!in.fail() && mzh.signature == MZ_SIGNATURE)
        {
            // seek to the position where the offset to the PEHeader is stored
            in.seekg(mzh.n_header_paragraphs * 16 - 4);
            int pos;
            in.read((char*)&pos, sizeof(pos));
            if (!in.fail())
            {
                in.seekg(pos);
                if (!in.fail())
                {
                    PEHeader peh;
                    in.read((char*)&peh, sizeof(peh));
                    if (!in.fail() && peh.signature == PESIG)
                    {
                        for (int i = 0; i < peh.num_objects; i++)
                        {
                            PEObject obj;
                            in.read((char*)&obj, sizeof(obj));
                            if (!in.fail())
                            {
                                if (peh.export_rva >= obj.virtual_addr && peh.export_rva < obj.virtual_addr + obj.virtual_size)
                                {
                                    rv = doExports(in, obj.raw_ptr + peh.export_rva - obj.virtual_addr, peh.export_rva);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return rv;
}