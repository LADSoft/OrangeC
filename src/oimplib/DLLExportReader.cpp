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

#include "DLLExportReader.h"
#include "PEHeader.h"
#include "MZHeader.h"

DLLExportReader::~DLLExportReader() {}

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
                if (in.eof())
                    in.clear(std::ios::failbit);  // the preceding read may have read PAST the eof.
                buf[255] = 0;                     // just in case
                byOrd = false;
            }
            else
            {
                byOrd = true;
            }
            in.seekg(eh.ordinal_rva - rva + phys + i * 2);
            in.read((char*)&ord, sizeof(ord));
            exports.push_back(std::make_unique<DLLExport>(buf, ord, byOrd));
        }
    }
    return !in.fail();
}
int DLLExportReader::Read()
{
    int rv = 2;
    std::fstream in(name, std::ios::in | std::ios::binary);
    if (!in.is_open())
        return 1;  // can't find file
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
                        rv = 3;  // no export table
                        for (int i = 0; i < peh.num_objects; i++)
                        {
                            PEObject obj;
                            in.read((char*)&obj, sizeof(obj));
                            if (!in.fail())
                            {
                                if (peh.export_rva >= obj.virtual_addr && peh.export_rva < obj.virtual_addr + obj.virtual_size)
                                {
                                    if (doExports(in, obj.raw_ptr + peh.export_rva - obj.virtual_addr, peh.export_rva))
                                        rv = 0;
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