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