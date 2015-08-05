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
#ifndef LIBDICTIONARY_H
#define LIBDICTIONARY_H

#include "ObjTypes.h"
#include <stdio.h>
#include <map>

class ObjFile;
class LibFiles;

struct DictCompare
{
    ObjInt casecmp(const char *str1, const char *str2, int n) const;

    bool operator ()(ObjString left, ObjString right) const
    {
        return casecmp(left.c_str(), right.c_str(), left.size()) < 0;
    }
    static bool caseSensitive;
};
class LibDictionary
{
public:
    typedef std::map<ObjString, ObjInt, DictCompare> Dictionary;
    LibDictionary(bool CaseSensitive = true) :caseSensitive(CaseSensitive) { DictCompare::caseSensitive = CaseSensitive; }
    ~LibDictionary() { }
    ObjInt Lookup(FILE *stream, ObjInt dictOffset, ObjInt dictPages, const ObjString &str);
    void Write(FILE *stream);
    void CreateDictionary(LibFiles &files);
    void Clear() { dictionary.clear(); }
protected:
    void InsertInDictionary(const char *name, int index);
private:
    Dictionary dictionary;
    bool caseSensitive;
} ;
#endif //LIBDICTIONARY_H