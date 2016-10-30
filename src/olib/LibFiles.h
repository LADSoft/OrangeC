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
#ifndef LIBFILES_H
#define LIBFILES_H
#include "ObjTypes.h"
#include <deque>
#include <stdio.h>
class ObjFile;
class ObjFactory;
class LibFiles
{
public:
    struct FileDescriptor
    {
        FileDescriptor(const ObjString &Name) : offset(0), name(Name), data(nullptr) { }
        FileDescriptor(const FileDescriptor &old) : name(old.name), offset(old.offset), data(nullptr) { }
        ~FileDescriptor() { /* if (data) delete data; */ } // data will go away when the factory goes awsy
        ObjString name;
        ObjInt offset;
        ObjFile *data;
    };
    LibFiles(bool CaseSensitive = true) { caseSensitive = CaseSensitive; }
    virtual ~LibFiles() { }
    
    void Add(ObjFile &obj);
    void Add(const ObjString &Name);
    void Remove(const ObjString &Name);
    void Extract(FILE *stream, const ObjString &Name);
    void Replace(const ObjString &Name);
    
    size_t size() { return files.size(); }
    bool ReadNames(FILE *stream, int count);
    void WriteNames(FILE *stream);
    void ReadOffsets(FILE *stream, int count);
    void WriteOffsets(FILE *stream);
    bool ReadFiles(FILE *stream, ObjFactory *factory);
    void WriteFiles(FILE *stream, ObjInt align);
    
    ObjFile *LoadModule(FILE *stream, ObjInt FileIndex, ObjFactory *factory);	
        
    typedef std::deque<FileDescriptor *>::iterator FileIterator;
    FileIterator FileBegin () { return files.begin(); }
    FileIterator FileEnd () { return files.end(); }
protected:
    ObjFile *ReadData(FILE *stream, const ObjString &name, ObjFactory *factory);
    void WriteData(FILE *stream, ObjFile *file, const ObjString &name);
    void Align(FILE *stream, ObjInt align);
private:
    std::deque<FileDescriptor *> files;
    std::deque<ObjFile *> objectFiles;
    bool caseSensitive;
} ;
#endif
