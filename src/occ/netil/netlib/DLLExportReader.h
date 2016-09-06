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
#ifndef DLLExportReader_H
#define DLLExportReader_H

#include <string>
#include <fstream>
#include <deque>

struct DLLExport
{
    DLLExport(const std::string &Name, int ord, bool ByOrd) : name(Name), ordinal(ord), byOrd(ByOrd) { }
    std::string name;
    int ordinal;
    bool byOrd;
};

class DLLExportReader
{
public:
    DLLExportReader(const std::string &fname) : name(fname) { }
    ~DLLExportReader() ;
    bool Read() { bool rv = FindDLL(); if (rv) rv = LoadExports(); return rv; }

    std::string GetName() { return name; }	
    typedef std::deque<DLLExport *>::iterator iterator;
    iterator begin() { return exports.begin(); }
    iterator end() { return exports.end(); }
protected:
    bool doExports(std::fstream &in, int phys, int rva);
    bool FindDLL();
    bool LoadExports();
private:
    std::string name;
    std::deque<DLLExport *> exports;
};
#endif
