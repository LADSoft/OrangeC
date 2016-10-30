/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "LibFromCoffDictionary.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ObjSection.h"
#include "Utils.h"
#include <ctype.h>
#include <iostream>
#include <string.h>
#include "CoffLibrary.h"

void LibDictionary::CreateDictionary(std::map<int, Module *> &Modules)
{
    int files = 0;
    int total = 0;
    int symbols = 0;
    Clear();
    for (auto m : Modules)
    {
        if (!m.second->ignore)
        {
            for (auto alias : m.second->aliases)
            {
                int n = strlen(alias.c_str()) + 1;
                if (n & 1)
                    n++;
                total += n;
                symbols++;                
            }
            if (!m.second->import)
                files++;
        }
    }
    int dictpages2, dictpages1;
    int i = 0;
    for (auto m : Modules)
    {
        if (!m.second->ignore)
        {
            for (auto alias : m.second->aliases)
            {
                InsertInDictionary(alias.c_str(), m.second->import ? files : i);
            }
            i++;
        }
    }
}
void LibDictionary::InsertInDictionary(const char *name, int index)
{
    bool put = false;
    if (!strcmp(name, "_WinMain@16"))
        name = "WinMain";
    char buf[2048];
    int l = strlen(name);
    strncpy(buf, name, 2048);
    buf[2047] = 0;
    if (!caseSensitive)
        for (int i=0; i <= l; i++)
            buf[i] = toupper(buf[i]);
    dictionary[buf] = index;
}
void LibDictionary::Write(FILE *stream)
{
    char sig[4] = { '1','0',0,0 };
    fwrite(&sig[0], 4, 1, stream);
    for (auto d : dictionary)
    {
        short len = d.first.size();
        fwrite(&len, sizeof(len), 1, stream);
        fwrite(d.first.c_str(), len , 1, stream);
        ObjInt fileNum = d.second;
        fwrite(&fileNum, sizeof(fileNum), 1, stream);
    }
    short eof = 0;
    fwrite(&eof, sizeof(eof), 1, stream);
}
