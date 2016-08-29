/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "DLLExportReader.h"
#include "PEHeader.h"
#include "MZHeader.h"
#include "CmdFiles.h"

DLLExportReader::~DLLExportReader()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        DLLExport *p = *it;
        delete p;
    }
}

bool DLLExportReader::FindDLL()
{
    char *p = strrchr((char *)name.c_str(), '.');
    if (!p || p[1] == CmdFiles::DIR_SEP[0])
    {
        name += ".dll";
    }
    // just try to open the file
    // this will catch absolute files and files relative to the current directory
    FILE *fil = fopen(name.c_str(), "rb");
    if (fil)
    {
        fclose(fil);
        return true;
    }
    // now look in windows/system32
    p = getenv("SystemRoot");
    if (p)
    {
        char buf[MAX_PATH];
        strcpy(buf, p);
        if (buf[strlen(buf)-1] != CmdFiles::DIR_SEP[0])
            strcat(buf, CmdFiles::DIR_SEP);
        strcat(buf,"System32");
        strcat(buf, CmdFiles::DIR_SEP);
        strcat(buf, name.c_str());
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
    while (*p)
    {
        char buf[MAX_PATH], *q = buf;
        while (*p && *p != ';')
            *q++ = *p++;
        *q = 0;
        if (*p)
            p++;
        if (buf[strlen(buf)-1] != CmdFiles::DIR_SEP[0])
            strcat(buf, CmdFiles::DIR_SEP);
        strcat(buf, name.c_str());
        fil = fopen(buf, "rb");
        if (fil)
        {
            name = buf;
            fclose(fil);
            return true;
        }
    }
    return false;
}
bool DLLExportReader::doExports(std::fstream &in, int phys, int rva)
{
    in.seekg(phys);
    if (!in.fail())
    {
        PEExportHeader eh;
        in.read((char *)&eh, sizeof(eh));
        for (int i=0; i < eh.n_name_ptrs; i++)
        {
            int nameptr;
            short ord = 0;
            char buf[256];
            bool byOrd;
            buf[0] = 0;
            in.seekg(eh.name_rva - rva + phys + i * 4);
            in.read((char *)&nameptr, sizeof(nameptr));
            if (nameptr)
            {
                in.seekg(nameptr - rva + phys);
                in.read(buf, 256);
                buf[255] = 0; // just in case
                byOrd = false;
            }
            else
            {
                byOrd = true;
            }
            in.seekg(eh.ordinal_rva - rva + phys + i * 2);
            in.read((char *)&ord, sizeof(ord));
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
        in.read((char *)&mzh, sizeof(mzh));
        if (!in.fail() && mzh.signature == MZ_SIGNATURE)
        {
            // seek to the position where the offset to the PEHeader is stored
            in.seekg(mzh.n_header_paragraphs * 16 - 4);
            int pos;
            in.read((char *) &pos, sizeof(pos));
            if (!in.fail())
            {
                in.seekg(pos);
                if (!in.fail())
                {
                    PEHeader peh;
                    in.read((char *)&peh, sizeof(peh));
                    if (!in.fail() && peh.signature == PESIG)
                    {
                        for (int i = 0; i < peh.num_objects; i++)
                        {
                            PEObject obj;
                            in.read((char *)&obj, sizeof(obj));
                            if (!in.fail())
                            {
                                if (peh.export_rva >= obj.virtual_addr &&
                                    peh.export_rva < obj.virtual_addr + obj.virtual_size)
                                {
                                    rv = doExports(in,
                                                   obj.raw_ptr + peh.export_rva - obj.virtual_addr,
                                                   peh.export_rva);
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