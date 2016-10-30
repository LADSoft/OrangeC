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
#ifndef ResFile_h
#define ResFile_h

#include <string>
#include <deque>
#include <fstream>
#include <set>
#include "Resource.h"

class ResFile
{
public:
    ResFile() : stream(nullptr) { }
    virtual ~ResFile();
    void Mark();
    void MarkHeader();
    void Release();
    void Align();
    void WriteByte(int byte);
    void WriteWord(int word);
    void WriteDWord(int dword);
    void WriteData(const unsigned char *data, int len);
    void WriteString(const std::wstring &str);
    bool Write(const std::string &name);
    void Reset();
    void Add(Resource *th);
    size_t GetPos() const { return stream->tellp(); }
    void SetPos(size_t n) { stream->seekp(n); }
private:
    std::deque<Resource *> resources;
    struct lt
    {
        // in this LT operation, the resource ids are guaranteed to be numeric.
        // we are doing this to force a sort of the string tables when we read
        // them back out
        bool operator ()(Resource *left, Resource *right)
        {
            return left->GetId().GetId() < right->GetId().GetId();
        }
    } ;
    // strings are kept in a separate list because they have to be at the end
    // of the file and sorted
    std::set<Resource *, lt> strings;
    int hdrSize;
    int size;
    int base;
    std::fstream *stream;
} ;
#endif
