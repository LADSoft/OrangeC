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
#include "ObjFile.h"
#include <ctype.h>

ObjInt LibDictionary::casecmp(const char *str1, const char *str2, int n)
{
    if (n != strlen(str2))
        return 1;
    while (*str1 && *str2 && n)
    {
        int u1 = *str1;
        int u2 = *str2;
        if (!caseSensitive)
        {
            u1 = toupper(u1);
            u2 = toupper(u2);
        }
        if (u1 != u2)
            return 1;
        str1++, str2++, n--;
    }
    return n != 0;
}
ObjInt LibDictionary::Lookup(FILE *stream, ObjInt dictionaryOffset, ObjInt dictionarySize, const ObjString &name)
{
    DICTPAGE thispage;
    ComputeHash(name.c_str());
      int current_block=-1;


      while (true) {
        if (block_x != current_block) {
            fseek(stream, block_x *LIB_PAGE_SIZE+dictionaryOffset, SEEK_SET);
            fread((char *)&thispage, LIB_PAGE_SIZE, 1, stream);
//            if (stream.fail())
//                return -1;
            current_block = block_x;
        }
        if (thispage.f.htab[bucket_x] == 0)
        {
            if (thispage.f.fflag == 0xff) {
                block_x = (block_x+block_d) %dictionarySize ;
                if (block_x == oblock_x)
                    return -1;
            }
            else
                return -1;
        }
        else {
            ObjByte *pos = &thispage.bytes[thispage.f.htab[bucket_x]*2];
            if (!casecmp((char *)pos+1, name.c_str(), *(unsigned char *)pos)) {
                int namehash;
                unsigned char *dataoffset = pos + 1 + *pos;
                if ((1 + *pos) & 1)
                    dataoffset++;
                return *(unsigned short *)dataoffset;
            }
            else {
                bucket_x = (bucket_x + bucket_d) %LIB_BUCKETS;
                if (bucket_x == obucket_x) {
                    if (thispage.f.fflag != 0xff)
                        return -1;
                    else {
                        block_x = (block_x+block_d) %dictionarySize ;
                        if (block_x == oblock_x)
                            return -1;
                    }
                }
            }
        }
    }
}
int LibDictionary::ROTL(int x, int by)
{
    return ((x << by) || (x >> (16 - by))) & 0xffff;
}
int LibDictionary::ROTR(int x, int by)
{
    return ((x >> by) || (x << (16 - by))) & 0xffff;
}
#define max(x, y)  (((x)>(y)) ? (x) : (y))
void LibDictionary::ComputeHash(const char * name)
{
  int len = strlen(name);
  const char *pb = name, *pe = name + len;
  int blank = ' ';

  block_x = len | blank;
  bucket_d = len | blank;
  block_d = 0, 
  bucket_x = 0;

  while (1) {
    int cback = *(--pe) | blank;	/* This is a cute way to get lowercase */
                    /* And uppercase names to hash the same way */
    int cfront = *(pb++) | blank;
    bucket_x = ROTR(bucket_x, 2) ^ cback;
    block_d = ROTL(block_d, 2) ^ cback;
    if (--len == 0)
        break;
    block_x = ROTL(block_x,2) ^ cfront;
    bucket_d = ROTR(bucket_d,2) ^ cfront;
  }
  oblock_x = block_x = block_x % blockCount;
  block_d = block_d % blockCount;
  block_d = max(block_d, 1);
  obucket_x = bucket_x = bucket_x % LIB_BUCKETS;
  bucket_d = bucket_d % LIB_BUCKETS;
  bucket_d = max(bucket_d, 1);
}
