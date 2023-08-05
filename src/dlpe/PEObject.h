/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.h
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

#ifdef TARGET_OS_WINDOWS
#include <windows.h>
#else
#include <stdint.h>
typedef uint32_t DWORD;
typedef DWORD FARPROC;
typedef unsigned char BYTE;
#endif


#include "ObjTypes.h"
#define PEHEADER_ONLY
#include "PEHeader.h"

#include <string>
#include <set>
#include <fstream>
#include <deque>
#include "ObjSection.h"
#include <memory>
#include <map>
#include <vector>
#define RVA(offset) (offset + virtual_addr)

class ObjFile;

class ResourceContainer;
class ObjExpression;
inline ObjInt ObjectAlign(ObjInt alignment, ObjInt value) { return (value + alignment - 1) & ~(alignment - 1); }

struct DelayLoadDirectory
{
    DWORD attributes;
    DWORD dllname;
    DWORD moduleHandleRVA;
    DWORD addressThunk;
    DWORD nameThunk;
    DWORD boundTable;
    DWORD unloadTable;
    DWORD time;
};

struct ImportDirectory
{
    DWORD addressThunk;
    DWORD time;
    DWORD version;
    DWORD dllName;
    DWORD nameThunk;
};

struct Module
{
    std::string name;
    DWORD moduleHandleRVA = 0;
    DWORD time = 0;
    std::vector<std::tuple<std::string, ObjSymbol*, DWORD, FARPROC>> externalNames;
    std::vector<std::string> publicNames;
};

template <class Directory>
struct DllImports
{
  public:
    DllImports(std::map<std::string, Module*>& Modules, const std::vector<std::string>& delayLoadModules, ObjInt Offset,
               bool bind = false, bool unload = false) :
        modules(Modules), offset(Offset), bindTable(bind), unloadTable(unload)
    {
        TrimModules(delayLoadModules);
    }
    void WriteDirectory(DWORD virtual_addr, DWORD imageBase, unsigned char* data);
    void WriteTables(std::vector<DWORD>& thunkFixups, DWORD virtual_addr, DWORD imageBase, DWORD& thunkTableRVA,
                     unsigned char* data);
    size_t ModuleSize(std::map<ObjString, ObjSymbol*> externs);

    typedef unsigned short HintType;
    auto& Modules() { return modules; }
    size_t CountOfIAT();
    size_t CountOfModules();
    DWORD IATOffset() const { return iatAddr; }
    DWORD DirectoryOffset() const { return directory; }

  protected:
    void TrimModules(const std::vector<std::string>&);
    bool IsDelayLoad(const std::vector<std::string>& delayLoadNames, std::string& name);
    size_t ModuleNameSize();
    size_t ModuleDirectoryNameSize();
    size_t sizeOfTables()
    {
        int n = CountOfIAT();
        n *= 2 + bindTable + unloadTable;  // import name table, import address table, hint table
        return n * sizeof(Entry);
    }
    size_t sizeOfDirectory() { return CountOfModules() * sizeof(Directory); }

    enum
    {
        DELAYLOAD_BY_RVA = 1
    };

    enum
    {
        IMPORT_BY_ORDINAL = 0x80000000
    };
    struct Entry
    {
        int ord_or_rva;
    };

  private:
    std::map<std::string, Module*> modules;
    ObjInt offset;
    ObjInt directory;
    ObjInt directoryNames;
    ObjInt iatAddr;
    ObjInt names;
    ObjInt nameAddr;
    ObjInt bindAddr;
    ObjInt unloadAddr;
    bool bindTable;
    bool unloadTable;
};


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
    virtual void Patch() {}
    const std::string& GetName() const { return name; }
    unsigned GetAddr() { return virtual_addr; }
    unsigned GetSize() { return size; }
    unsigned GetFlags() { return flags; }
    void SetFlags(unsigned Flags) { flags = Flags; }
    void WriteHeader(std::fstream& stream);
    void Write(std::fstream& stream);
    unsigned GetNextVirtual() { return (virtual_addr + size + objectAlign - 1) & ~(objectAlign - 1); }
    unsigned GetNextRaw() { return (virtual_addr + initSize + fileAlign - 1) & ~(fileAlign - 1); }
    static void SetFile(ObjFile* File);
    static void SetDelayLoadNames(std::string names);

    auto Regions()
    { 
         if (regions.size())
             return regions;
         return std::vector<std::pair<ObjInt, ObjInt>>{{virtual_addr, initSize}};
    }
  protected:
    std::string name;
    unsigned size;
    unsigned initSize;
    unsigned virtual_addr;
    unsigned raw_addr;
    unsigned flags;
    std::vector<std::pair<ObjInt, ObjInt>> regions;
    std::shared_ptr<unsigned char> data;
    static std::vector<std::string> delayLoadNames;
    static unsigned objectAlign;
    static unsigned fileAlign;
    static unsigned imageBase;
    static unsigned dataInitSize;
    static DWORD importThunkVA;
    static unsigned importCount;
    static DWORD imageBaseVA;
    static std::shared_ptr<unsigned char> codeData;
    static DWORD codeRVA;
    static DWORD delayLoadHandleRVA;
    static DWORD delayLoadThunkRVA;
    static ObjFile* file;
    
    static const char* importThunk;
    static const char* delayLoadThunk;
    static const char* delayLoadModuleThunk;
    static std::vector<DWORD> thunkFixups;

  private:
};

class PEDataObject : public PEObject
{
  public:
    PEDataObject(ObjFile* File, ObjSection* Sect);
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);
    virtual void Patch();
    void InitFlags();
    ObjInt SetImportThunk(int index, unsigned va);

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
    enum
    {
        ImportThunkSize = 6,
        DelayLoadThunkSize = 10,
        DelayLoadModuleThunkSize = 0x11,
    };
    PEImportObject(std::deque<std::shared_ptr<PEObject>>& Objects, bool BindTable, bool UnloadTable);
    virtual void Setup(ObjInt& endVa, ObjInt& endPhys);
    static size_t ThunkSize(std::string name);

  protected:
    void LoadHandles(DllImports<DelayLoadDirectory>& delay);
    void LoadBindingInfo(DllImports<DelayLoadDirectory>& delay, std::map<std::string, Module*> modules);
    void SymbolNotFoundError();
    void WriteThunks(DllImports<DelayLoadDirectory>& delay, std::map<std::string, Module*> modules,
                     std::map<ObjString, ObjSymbol*> publics);

  private:
    std::deque<std::shared_ptr<PEObject>>& objects;
    bool bindTable;
    bool unloadTable;
};
class PEExportObject : public PEObject
{
  public:
    PEExportObject(const std::string& name, bool Flat) : PEObject(".edata"), moduleName(name), flat(Flat), appliedFlat(false)
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