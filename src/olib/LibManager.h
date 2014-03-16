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
#ifndef LIBMANAGER_H
#define LIBMANAGER_H

#include <vector>
#include <set>
#include <stdio.h>
#include "ObjTypes.h"
#include "LibDictionary.h"
#include "LibFiles.h"

class ObjSymbol;
class ObjFile;
class ObjFactory;

class LibManager
{
    public:
        LibManager(const ObjString &Name, bool CaseSensitive = true) 
            : dictionary(CaseSensitive), name(Name), files(CaseSensitive)
        {
            stream = fopen(Name.c_str(), "rb");
            InitHeader();
        }
        ~LibManager() { Close(); }
        
        void AddFile(const ObjString &name) {files.Add(name); }
        void AddFile(ObjFile &obj) { files.Add(obj); }
        void RemoveFile(const ObjString &name) {files.Remove(name); }
        void ExtractFile(const ObjString &name) {files.Extract(stream, name); }
        void ReplaceFile(const ObjString &name) { files.Replace(name); }
        ObjInt Lookup(const ObjString &name);
        ObjFile *LoadModule(ObjInt index, ObjFactory *factory) { return files.LoadModule(stream, index, factory); }
        bool LoadLibrary();
        bool SaveLibrary();
        bool fail() const { return false; } //stream.fail(); }
        bool IsOpen() const { return stream != NULL; }
        void Close() { if (stream) fclose(stream); }
        enum { ALIGN = 512 };
    protected:
        void Align(FILE *ostr, ObjInt align = ALIGN);
        struct LibHeader
        {
            enum { LIB_SIG = 0x4442494c };
            unsigned sig;
            unsigned filesInModule;
            unsigned offsetsOffset;
            unsigned namesOffset;
            unsigned filesOffset;
            unsigned dictionaryOffset;
            unsigned dictionaryBlocks;
            unsigned reserved;
        } ;
        void InitHeader();
        void WriteHeader() { fseek(stream, 0, SEEK_SET); fwrite((char *)&header, sizeof(header), 1, stream); }
        void ReadHeader() { fseek(stream, 0, SEEK_SET); fread((char *)&header, sizeof(header), 1, stream); }
    private:
        LibHeader header;
        FILE *stream;
        LibFiles files;
        LibDictionary dictionary;
        ObjString name;
} ;
#endif
