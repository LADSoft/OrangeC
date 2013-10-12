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
#include "LibDictionary.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "Utils.h"
#include <ctype.h>
#include <iostream>
int LibDictionary::primes[] = 
{
    1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67,
        71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139,
        149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223,
        227, 229, 233, 239, 241, 251
};
void LibDictionary::CreateDictionary(LibFiles &files)
{
    int total = 0;
    int symbols = 0;
    if (data)
        Clear();
    std::map<ObjString, ObjString> publics;
    for (LibFiles::FileIterator it = files.FileBegin(); it != files.FileEnd(); ++it)
    {
        const LibFiles::FileDescriptor *fd = (*it);
        if (fd->data)
        {
            for (ObjFile::SymbolIterator pi = fd->data->PublicBegin(); pi != fd->data->PublicEnd(); ++pi)
            {
                int n = strlen((*pi)->GetName().c_str()) + 1;
                if (n & 1)
                    n++;
                total += n;
                symbols++;
                std::map<ObjString, ObjString>::iterator it = publics.find((*pi)->GetName());
                if (it != publics.end())
                {
                    std::cout << "Warning: public '" << (*pi)->GetDisplayName() << "' defined in both module '"
                        << it->second <<"' and '"<<fd->name << "'" << std::endl;
                }
                else
                {
                    publics[(*pi)->GetName()] = fd->name;
                }
            }
            // support for imports
            for (ObjFile::SymbolIterator pi = fd->data->ImportBegin(); pi != fd->data->ImportEnd(); ++pi)
            {
                int n = strlen((*pi)->GetName().c_str()) + 1;
                if (n & 1)
                    n++;
                total += n;
                symbols++;
                publics[(*pi)->GetName()] = fd->name;
            }
        }
    }
    int dictpages2, dictpages1;
    dictpages1 = (symbols + LIB_BUCKETS) / LIB_BUCKETS;
    dictpages2 = (total + (LIB_PAGE_SIZE-(LIB_BUCKETS + 1))) / (LIB_PAGE_SIZE-(LIB_BUCKETS + 1)) + 1;
    blockCount = dictpages1 > dictpages2 ? dictpages1 : dictpages2;
    blockCount--;
    bool running = true;
    while (running)
    {
        blockCount++;
        running = false;
        if (blockCount > primes[sizeof(primes)/sizeof(int)-1])
            Utils::fatal("Library dictionary too large");
        for (int i = 0; i < sizeof(primes)/sizeof(int); i++)
            if (primes[i] >= blockCount)
            {
                blockCount = primes[i];
                break;
            }
        if (data)
            delete [] data;
        data = new DICTPAGE[blockCount];
        memset(data, 0, blockCount * sizeof(DICTPAGE));
        int i = 0;
        for (LibFiles::FileIterator it = files.FileBegin(); it != files.FileEnd(); ++it)
        {
            const LibFiles::FileDescriptor *fd = (*it);
            if (fd->data)
            {	
                for (ObjFile::SymbolIterator pi = fd->data->PublicBegin(); pi != fd->data->PublicEnd(); ++pi)
                {
                    if (!InsertInDictionary((*pi)->GetName().c_str(), i))
                    {
                        running = true;
                        break;
                    }
                }
                for (ObjFile::SymbolIterator pi = fd->data->ImportBegin(); pi != fd->data->ImportEnd(); ++pi)
                {
                    if (!InsertInDictionary((*pi)->GetName().c_str(), i))
                    {
                        running = true;
                        break;
                    }
                }
                i++;
            }
        }
    }
}
bool LibDictionary::InsertInDictionary(const char *name, int index)
{
    bool put = false;
    ComputeHash(name);
    int l = strlen(name);
    int n = l + 1;
    if (n & 1)
        n++;
    do
    {
        DICTPAGE *dptr = &data[block_x];
        do
        {
            if (!dptr->f.htab[bucket_x])
            {
                int start;
                if (!dptr->f.fflag)
                    start = dptr->f.names - dptr->bytes;
                else
                    start = dptr->f.fflag *2;
                if (LIB_PAGE_SIZE-start >= n + 2)
                {
                    dptr->f.htab[bucket_x] = start/2;					
                    dptr->bytes[start] = l;
                    strncpy((char *)&dptr->bytes[start+1], name, l);
                    if (!caseSensitive)
                        for (int i=start+1; i <= start +l; i++)
                            dptr->bytes[i] = toupper(dptr->bytes[i]);
                                                            
                    start += n;
                    *(unsigned short *)(&dptr->bytes[start]) = index; // endian
                    start += 2;
                    if (start == 512)
                        start = 511;
                    dptr->f.fflag = start / 2;
                    put = true;
                    break;
                }
                else
                {
                    dptr->f.fflag = 0xff;
                    break;
                }
            }
            bucket_x += bucket_d;
            if (bucket_x >= LIB_BUCKETS)
                bucket_x -= LIB_BUCKETS;
        }
        while (obucket_x != bucket_x) ;
        if (put)
            break;
        dptr->f.fflag = 0xff;
        block_x += block_d;
        if (block_x >= blockCount)
            block_x -= blockCount;
    }
    while (oblock_x != block_x);
    return put;
}
void LibDictionary::Write(FILE *stream)
{
    if (blockCount)
        fwrite(data, blockCount * LIB_PAGE_SIZE, 1 ,stream);
}
