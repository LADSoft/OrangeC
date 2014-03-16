/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
#include <commctrl.h>
#include <stdio.h>

#include "header.h"
#include "pefile.h"
static struct pe_header_struct PEHead;
static int base;
int IsPEFile(char *filename)
{
    FILE *fil = fopen(filename, "rb");
    char buf[64];
    long pos;
    if (!fil)
        return 0;
    memset(buf, 0, sizeof(buf));
    fread(buf, 64, 1, fil);
    pos = *(int*)(buf + 0x3c);
    fseek(fil, pos, SEEK_SET);
    fread(buf, 2, 1, fil);
    fclose(fil);

    if (buf[0] == 'P' && buf[1] == 'E')
        return 1;
    return  - 1;

} 
/* assumes FindExitProcessAddress has been called */
int GetEntryPoint(void)
{
    return PEHead.entry_point + base;
}

//-------------------------------------------------------------------------

int FindExitProcessAddress(HANDLE hProcess, int imagebase)
{
    struct pe_import_dir_struct PEImport;
    int dir_address;

    base = imagebase;
    ReadProcessMemory(hProcess, (LPVOID)(imagebase + 0x3c), (LPVOID)
        &dir_address, 4, 0);

    ReadProcessMemory(hProcess, (LPVOID)(imagebase + dir_address), (LPVOID)
        &PEHead, sizeof(struct pe_header_struct), 0);
    dir_address = PEHead.import_rva;
    if (dir_address == 0)
        return 0;
    do
    {
        ReadProcessMemory(hProcess, (LPVOID)(imagebase + dir_address), (LPVOID)
            &PEImport, sizeof(struct pe_import_dir_struct), 0);
        if (PEImport.dllName)
        {
            char buf[256];
            ReadProcessMemory(hProcess, (LPVOID)(imagebase + PEImport.dllName),
                (LPVOID)buf, 256, 0);
            if (!xstricmpz(buf, "KERNEL32.DLL"))
            {
                int namepos = PEImport.thunkPos2 + imagebase;
                int addrpos = PEImport.thunkPos + imagebase;
                do
                {
                    int nametext;
                    ReadProcessMemory(hProcess, (LPVOID)namepos, buf, 4, 0);
                    nametext = *(int*)buf + imagebase;
                    if (nametext == 0)
                        return 0;
                    ReadProcessMemory(hProcess, (LPVOID)nametext, buf, 256, 0);
                    if (!strcmp(buf + 2, "ExitProcess"))
                    {
                        ReadProcessMemory(hProcess, (LPVOID)addrpos, buf, 4, 0);
                        return *(int*)buf;
                    } namepos += 4;
                    addrpos += 4;
                }
                while (TRUE);

            }
            dir_address += sizeof(struct pe_import_dir_struct);
        }

    }
    while (PEImport.dllName)
        ;

    return 0;
}
