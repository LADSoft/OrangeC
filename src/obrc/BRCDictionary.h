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
#ifndef BRCDICTIONARY_H
#define BRCDICTIONARY_H

#include "ObjTypes.h"
#include "BRCLoader.h"
#include <fstream>
#include <map>

class ObjFile;
class LibFiles;
class SymData;

class BRCDictionary
{
public:
    typedef std::map<ObjString, ObjInt> Dictionary;
    enum { LIB_PAGE_SIZE = 512 } ;
    enum { LIB_BUCKETS = 37 } ;
    union DICTPAGE {
      ObjByte bytes[LIB_PAGE_SIZE];		/* Either 512 ObjBytes */
      struct {
          ObjByte htab[LIB_BUCKETS];		/* Or the 37 buckets */
        ObjByte fflag;					/* followed by the next free position */
        ObjByte names[LIB_PAGE_SIZE-LIB_BUCKETS-1];	/* followed by names section */
      } f;
    };
    BRCDictionary(Symbols &p) : symbols(p), blockCount(0), data(NULL) { }
    ~BRCDictionary() { if (data) delete [] data; }
    ObjInt Lookup(std::fstream &stream, ObjInt dictOffset, ObjInt dictPages, const ObjString &str);
    void Write(std::fstream &stream);
    void CreateDictionary(void);
    void Clear() { dictionary.clear(); if (data) { delete [] data; data = NULL; } blockCount = 0; }
    ObjInt GetBlockCount() const { return blockCount; }
    void SetBlockCount(ObjInt BlockCount) { blockCount = BlockCount; }
    ObjInt casecmp(const char *str1, const char *str2, int n);

protected:	
    int ROTL(int x, int by);
    int ROTR(int x, int by);
    void ComputeHash(const char * name);
    bool InsertInDictionary(SymData *sym);
private:
    Symbols &symbols;
    Dictionary dictionary;
    int blockCount;
    DICTPAGE *data;
    int block_d, bucket_d, block_x, bucket_x;
    int oblock_x;
    int obucket_x;
    
    static int primes[] ;
} ;
#endif BRCDICTIONARY_H