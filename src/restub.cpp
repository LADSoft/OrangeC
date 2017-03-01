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
#include <string.h>
#include "exefmt\MZHeader.h"
#include "exefmt\peHeader.h"

size_t stubSize;
size_t oldOffset;
unsigned char *stubData;
size_t exeSize;
size_t oldStubSize;
unsigned char *exeData;
bool LoadEXE(const std::string &exeName)
{
    std::fstream file(exeName.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }
    MZHeader mzHead;
    file.read((char *)&mzHead, sizeof(mzHead));
    if (mzHead.signature == MZ_SIGNATURE)
    {
        oldStubSize = mzHead.n_header_paragraphs << 4;
        file.seekg(0x3c, std::ios::beg);
        int datapos;
        file.read((char *)&datapos, 4);
        oldOffset = datapos;
        file.seekg(datapos, std::ios::beg);
        if (!file.fail())
        {
            char sig[2];
            file.read(sig, 2);
            if (sig[0] == 'P' && sig[1] == 'E')
            {
                file.seekg(0, std::ios::end);
                int len = file.tellg();
                exeSize = len - datapos;
                exeData = new unsigned char[exeSize];
                file.seekg(datapos, std::ios::beg);
                file.read((char *)exeData, exeSize);
                return true;
            }
        }
    }
    return false;
        
}
bool LoadStub(const std::string &stubName)
{
    std::fstream *file = new std::fstream(stubName.c_str(), std::ios::in | std::ios::binary);
    if (!file || !file->is_open())
    {
        delete file;
        file = NULL;
        return false;
    }
    MZHeader mzHead;
    file->read((char *)&mzHead, sizeof(mzHead));
    int bodySize = mzHead.image_length_MOD_512 + mzHead.image_length_DIV_512 * 512;
    if (mzHead.signature != MZ_SIGNATURE)
        return false;
    int oldReloc = mzHead.offset_to_relocation_table;
    int oldHeader = mzHead.n_header_paragraphs * 16;
    if (bodySize & 511)
        bodySize -= 512;
    bodySize -= oldHeader;
    int relocSize = mzHead.n_relocation_items * 4;
    int preHeader = 0x40;
    int totalHeader = (preHeader + relocSize + 15) & ~15;
    stubSize = (totalHeader + bodySize + 15) & ~15;
    stubData = new unsigned char [stubSize];
    memset(stubData, 0, stubSize);
    int newSize = exeSize + totalHeader;
    if (newSize & 511)
        newSize += 512;
//	mzHead.image_length_MOD_512 = newSize % 512;
//	mzHead.image_length_DIV_512 = newSize / 512;
    mzHead.offset_to_relocation_table = 0x40;
    mzHead.n_header_paragraphs = totalHeader/ 16;
    memcpy(stubData, &mzHead, sizeof(mzHead));
    *(unsigned *)(stubData + 0x3c) = stubSize;
    if (relocSize)
    {
        file->seekg(oldReloc, std::ios::beg);
        file->read((char *)stubData + 0x40, relocSize);
    }
    file->seekg(oldHeader, std::ios::beg);
    file->read((char *)stubData + totalHeader, bodySize);
    if (!file->eof() && file->fail())
    {
        delete file;
        return false;
    }
    delete file;
    return true;
}
void PatchHeader()
{
    PEHeader *p = (PEHeader *)exeData;
    int relSize = stubSize - oldOffset;
    unsigned *objectTable = (unsigned *)(exeData + sizeof(PEHeader));
    int n = p->file_align - (relSize) %p->file_align;
    if (n != p->file_align)
    {
        unsigned char *f = new unsigned char[exeSize + n];
        memset(f, 0, exeSize + n);
        int hdrSize = (unsigned char *)objectTable - exeData + 10 * 4 * p->num_objects;
        memcpy(f, exeData, hdrSize);
        memcpy(f + objectTable[5] - oldOffset + n,  exeData + objectTable[5] - oldOffset, exeSize - objectTable[5]);
        exeSize += n;
        relSize += n;
        delete exeData;
        exeData = f;
        objectTable = (unsigned *)(exeData + sizeof(PEHeader));
//		MZHeader *h = (MZHeader *)stubData;
//		h->image_length_MOD_512 += n;
//		n = h->image_length_MOD_512 / 512;
//		if (h->image_length_MOD_512 % 512 == 0)
//		{
//			n--;
//			if (n > 0)
//				h->image_length_DIV_512 += n;
//		}
    }
    p = (PEHeader *)exeData;
    p->header_size += relSize;
    for (int i=0; i < p->num_objects; i++)
    {
        objectTable[5] += relSize;
        objectTable += 10;
    }
}
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("usage: restub filename stubname");
        return 1;
    }
    if (!LoadEXE(argv[1]))
    {
        printf("Missing or invalid exe file");
        return 1;
    }
    if (!LoadStub(argv[2]))
    {
        printf("Missing or invalid stub file");
        return 1;
    }
    PatchHeader();
    std::fstream out(argv[1], std::ios::out | std::ios::binary);
    if (!out)
    {
        printf("Error opening output file");
        return 1;
    }
    out.write((char *)stubData, stubSize);
    out.write((char *)exeData, exeSize);
    out.close();
    return 0;
}