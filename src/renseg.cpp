/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2009, David Lindauer, (LADSoft).
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
#include <string>
#include <fstream>
#include <stdio.h>
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
    if (file == NULL || !file.is_open())
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
            file.write((unsigned char *)buf, 512);
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