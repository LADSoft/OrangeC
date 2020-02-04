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
#include <memory>

class ObjFile;

class ResourceContainer;
class ObjExpression;
inline ObjInt ObjectAlign(ObjInt alignment, ObjInt value) { return (value + alignment - 1) & ~(alignment - 1); }

class PEObject
{
  public:
    enum
    {
        HeaderSize = 40
    };
    PEObject(std::string Name) : name(Name), size(0), initSize(0), virtual_addr(0), raw_addr(0), flags(0) {}
    virtual ~PEObject() {}
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys) = 0;
    virtual void Fill() {}
    const std::string& GetName() const { return name; }
    unsigned GetAddr() { return virtual_addr; }
    unsigned GetSize() { return size; }
    unsigned GetRawSize() { return initSize; }
    void SetSize(unsigned Size) { initSize = size = Size; }
    void SetInitSize(unsigned Size) { initSize = size; }
    void SetAddr(unsigned addr) { virtual_addr = addr; }
    void SetRawAddr(unsigned addr) { raw_addr = addr; }
    unsigned GetFlags() { return flags; }
    void SetFlags(unsigned Flags) { flags = Flags; }
    void WriteHeader(std::fstream& stream);
    void Write(std::fstream& stream);
    unsigned GetNextVirtual() { return (virtual_addr + size + objectAlign - 1) & ~(objectAlign - 1); }
    unsigned GetNextRaw() { return (virtual_addr + initSize + fileAlign - 1) & ~(fileAlign - 1); }
    static void SetFile(ObjFile* File);
    virtual ObjInt SetThunk(int index, unsigned val) { return -1; }

  protected:
    std::string name;
    unsigned size;
    unsigned initSize;
    unsigned virtual_addr;
    unsigned raw_addr;
    unsigned flags;
    std::unique_ptr<unsigned char[]> data;
    static unsigned objectAlign;
    static unsigned fileAlign;
    static unsigned imageBase;
    static unsigned dataInitSize;
    static unsigned importThunkVA;
    static unsigned importCount;
    static ObjFile* file;

  private:
};

class PEDataObject : public PEObject
{
  public:
    PEDataObject(ObjFile* File, ObjSection* Sect) : file(File), PEObject(Sect->GetName()), sect(Sect) { InitFlags(); }
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);
    virtual void Fill();
    void InitFlags();
    virtual ObjInt SetThunk(int index, unsigned va);
    bool hasPC(ObjExpression* exp);
    ObjExpression* getExtern(ObjExpression* exp);
    ObjInt EvalFixup(ObjExpression* fixup, ObjInt base);
    void GetImportNames();

  private:
    ObjFile* file;
    ObjSection* sect;
    std::set<std::string> importNames;
};
class PEImportObject : public PEObject
{
  public:
    PEImportObject(std::deque<std::unique_ptr<PEObject>>& Objects) : PEObject(".idata"), objects(Objects)
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_WRITEABLE | WINF_NEG_FLAGS);
    }
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);

  private:
    struct Dir
    {
        int thunkPos2;  // address thunk
        int time;
        int version;
        int dllName;
        int thunkPos;  // name thunk
    };

    enum
    {
        IMPORT_BY_ORDINAL = 0x80000000
    };
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
    std::deque<std::unique_ptr<PEObject>>& objects;
};
class PEExportObject : public PEObject
{
  public:
    PEExportObject(const std::string& name, bool Flat) : PEObject(".edata"), moduleName(name), flat(Flat)
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_NEG_FLAGS);
    }
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);
    bool ImportsNeedUnderscore() const { return flat && appliedFlat; }

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
    bool flat;
    bool appliedFlat;
protected:
    static bool skipUnderscore;
    class namelt;
};
class PEFixupObject : public PEObject
{
  public:
    PEFixupObject() : PEObject(".reloc")
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_SHARED | WINF_NEG_FLAGS);
        LoadFixups();
    }
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);
    virtual ObjInt SetThunk(int index, unsigned va)
    {
        fixups.insert(importThunkVA + 6 * index + 2);
        return -1;
    }

  private:
    // load fixups is called right off the bat because once the setup for the data objects
    // runs, the fixups will be eval'd and lose their seg-relativeness.
    bool IsRel(ObjExpression* e);
    bool IsInternal(ObjExpression* e);
    void CountSections(ObjExpression* e, bool positive, int& pos, int& neg);
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
    PEResourceObject(ResourceContainer& Resources) : PEObject(".rsrc"), resources(Resources)
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_NEG_FLAGS);
    }
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);

  private:
    struct Dir
    {
        int resource_flags;
        int time;
        int version;
        short name_entry;
        short ident_entry;
    };

    enum
    {
        SUBDIR = 0x80000000,
        RVA = 0x80000000
    };
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
    ResourceContainer& resources;
};
class PEDebugObject : public PEObject
{
  public:
    PEDebugObject(const ObjString& fname, ObjInt base) : PEObject(".debug"), fileName(fname)
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_SHARED | WINF_NEG_FLAGS);
        SetDebugInfo(fileName, base);
    }
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);

  protected:
    void SetDebugInfo(ObjString fileName, ObjInt base);
    static int NullCallback(void* NotUsed, int argc, char** argv, char** azColName);

  private:
    ObjString fileName;
};

#endif