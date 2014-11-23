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
#ifndef PEObject_H
#define PEObject_H

#include "ObjTypes.h"
#define PEHEADER_ONLY
#include "PEHeader.h"

#include <string>
#include <set>
#include <fstream>
#include <deque>
#include "ObjSection.h"
class ObjFile;

class ResourceContainer;
inline ObjInt ObjectAlign(ObjInt alignment, ObjInt value)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

class PEObject
{
public:
    enum { HeaderSize = 40 } ;
    PEObject(std::string Name) : data(NULL), name(Name), size(0), initSize(0),
        virtual_addr(0), raw_addr(0), flags(0) { }
    virtual ~PEObject() { if (data) delete[] data; }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys) = 0;
    virtual void Fill() { }
    const std::string &GetName() { return name; }
    unsigned GetAddr() { return virtual_addr; }
    unsigned GetSize() { return size; }
    unsigned GetRawSize() { return initSize; }
    void SetSize(unsigned Size) { initSize = size = Size; }
    void SetInitSize(unsigned Size) { initSize = size; }
    void SetAddr(unsigned addr) { virtual_addr = addr; }
    void SetRawAddr(unsigned addr) { raw_addr = addr; }
    unsigned GetFlags() { return flags; }
    void SetFlags(unsigned Flags) { flags = Flags; }
    void WriteHeader(std::fstream &stream);
    void Write(std::fstream &stream);
    unsigned GetNextVirtual() { return (virtual_addr + size + objectAlign - 1) & ~(objectAlign - 1); }
    unsigned GetNextRaw() { return (virtual_addr + initSize + fileAlign - 1) & ~(fileAlign - 1); }
    static void SetFile(ObjFile *File);
    virtual ObjInt SetThunk(int index, unsigned val) { return -1; }
protected:
    std::string name;
    unsigned size;
    unsigned initSize;
    unsigned virtual_addr;
    unsigned raw_addr;
    unsigned flags;
    unsigned char *data;
    static unsigned objectAlign;
    static unsigned fileAlign;
    static unsigned imageBase;
    static unsigned dataInitSize;
    static unsigned importThunkVA;
    static ObjFile *file;
private:
};

class PEDataObject : public PEObject
{
public:
    PEDataObject(ObjSection *Sect) : PEObject(Sect->GetName()), sect(Sect) { InitFlags(); }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys);
    virtual void Fill();
    void InitFlags();
    virtual ObjInt SetThunk(int index, unsigned va);
private:
    ObjSection *sect;
};
class PEImportObject : public PEObject
{
public:
    PEImportObject(std::deque<PEObject *> &Objects) : PEObject(".idata"), objects(Objects) { SetFlags(WINF_INITDATA | WINF_READABLE | WINF_WRITEABLE | WINF_NEG_FLAGS); }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys);
private:
    struct Dir
    {
        int thunkPos2; // address thunk
        int time;
        int version;
        int dllName;
        int thunkPos; // name thunk
    };

    enum { IMPORT_BY_ORDINAL = 0x80000000 } ;
    struct Entry
    {
        int ord_or_rva;
    };
    struct Module
    {
        std::string module;
        std::deque<std::string> externalNames;
        std::deque<std::string> publicNames;
        std::deque<int> ordinals;
    };
    std::deque<PEObject *> &objects;
};
class PEExportObject : public PEObject
{
public:
    PEExportObject(const std::string &name) : PEObject(".edata"), moduleName(name) { SetFlags(WINF_INITDATA | WINF_READABLE | WINF_NEG_FLAGS); }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys);
private:
    struct Header
    {
        int flags;
        int time;
        int version;
        int exe_name_rva;
        int ord_base;
        int n_eat_entries;
        int n_name_ptrs;
        int address_rva;
        int name_rva;
        int ordinal_rva;
    };
    std::string moduleName;
};
class PEFixupObject : public PEObject
{
public:
    PEFixupObject() : PEObject(".reloc") { SetFlags(WINF_INITDATA | WINF_READABLE | WINF_SHARED | WINF_NEG_FLAGS); LoadFixups(); }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys);
    virtual ObjInt SetThunk(int index, unsigned va) { fixups.insert(importThunkVA + 6 * index + 2); return -1; }
private:
    // load fixups is called right off the bat because once the setup for the data objects
    // runs, the fixups will be eval'd and lose their seg-relativeness.
    bool IsRel(ObjExpression *e);
    void LoadFixups();
    struct Block
    {
        int rva;
        int size;
        short data[2048];
    };
    std::set<ObjInt> fixups;
};
class PEResourceObject : public PEObject
{
public:
    PEResourceObject(ResourceContainer &Resources) : PEObject(".rsrc"), resources(Resources) { SetFlags(WINF_INITDATA | WINF_READABLE | WINF_NEG_FLAGS); }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys);
private:
    struct Dir
    {
        int resource_flags;
        int time;
        int version;
        short name_entry;
        short ident_entry;
    };
    
    enum {
        SUBDIR = 0x80000000,
        RVA = 0x80000000
    } ;
    struct Entry
    {
        int rva_or_id;
        int subdir_or_data;
    };
    
    struct DataEntry
    {
        int rva;
        int size;
        int codepage;
        int reserved;
    };
    ResourceContainer &resources;
};
class PEDebugObject : public PEObject
{
public:
    PEDebugObject(const ObjString &fname, ObjInt base) : PEObject(".debug"), 
        fileName(fname)
         { SetFlags(WINF_INITDATA | WINF_READABLE | WINF_SHARED | WINF_NEG_FLAGS);
           SetDebugInfo(fileName, base); }
    virtual void Setup(ObjInt &endVa, ObjInt &endPhys);
protected:
    void SetDebugInfo(ObjString fileName, ObjInt base);
    static int NullCallback(void *NotUsed, int argc, char **argv, char **azColName);
private:
    ObjString fileName;
};

#endif