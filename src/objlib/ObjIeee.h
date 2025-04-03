/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#ifndef OBJIEEE_H
#define OBJIEEE_H
#include <fstream>
#include <stdexcept>
#include <map>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include "Utils.h"
#include "ObjIO.h"
#include "ObjUtil.h"
#include "ObjIndexManager.h"
#include "ObjType.h"
#include "ObjMemory.h"
#include "ObjIeeeConstants.h"
class ObjSymbol;
class ObjSection;
class ObjSourceFile;
class ObjFunction;
class ObjDebugTag;
class ObjBrowseInfo;

void DebugThrowHook();

class ObjIeeeBinary : public ObjIOBase
{
    enum
    {
        BUFFERSIZE = 32768
    };
    enum
    {
        EBYTE = 1,
        EWORD = 2,
        EDWORD = 3,
        EINDEX = 4,
        EBUF = 5,
        ESTRING = 6,
        EEXPR = 7,
        EEMBEDDED = 0x80000000
    };

  public:
    ObjIeeeBinary(const ObjString& Name, bool CaseSensitive = true) :
        ObjIOBase(Name, CaseSensitive),
        ioBufferLen(0),
        ioBuffer(0),
        file(nullptr),
        factory(nullptr),
        sfile(nullptr),
        cs(0),
        currentDataSection(nullptr),
        ioBufferPos(0),
        lineno(0),
        first(true)
    {
    }
    virtual ~ObjIeeeBinary() {}
    virtual bool Write(FILE* fil, ObjFile* File, ObjFactory* Factory)
    {
        sfile = fil;
        factory = Factory;
        file = File;
        return HandleWrite();
    }
    virtual ObjFile* Read(FILE* fil, eParseType ParseType, ObjFactory* Factory)
    {
        sfile = fil;
        factory = Factory;
        file = nullptr;
        return HandleRead(ParseType);
    }
    virtual bool BinaryWrite(FILE* fil, ObjFile* File, ObjFactory* Factory)
    {
        sfile = fil;
        factory = Factory;
        file = File;
        return BinaryWrite();
    }
    class SyntaxError : public std::domain_error
    {
      public:
        SyntaxError(int lineno) :
            std::domain_error(std::string("Syntax Error in line ") + Utils::NumberToString(lineno)), lineNo(lineno)
        {
        }
        virtual ~SyntaxError() noexcept {};

        int GetLineNo() const { return lineNo; }

      private:
        int lineNo;
    };
    class BadFileError : public std::domain_error
    {
      public:
        BadFileError(const std::string& name) : std::domain_error(name + " is not an object file") {}
        virtual ~BadFileError() noexcept {};
    };

  protected:
    class BadCS : public std::domain_error
    {
      public:
        BadCS() : std::domain_error("Bad Checksum") {}
        virtual ~BadCS() noexcept {};
    };
    void RenderMessageInternal(ObjByte* buf, va_list arg);
    ObjByte* StartMessage(int msg, ...);
    void ContinueMessage(ObjByte* buf, ...);
    void RenderMessage(const ObjByte* buf);
    void RenderMessage(int msg, ...);
    void RenderComment(int tp, ...);
    bool HandleWrite();
    bool BinaryWrite();
    ObjFile* HandleRead(eParseType Type);

    void GatherCS(const ObjByte* cstr);
    ObjFactory* GetFactory() { return factory; }
    ObjFile* GetFile() { return file; }
    ObjString ParseString(const ObjByte* buffer, int* pos);
    void ParseTime(const ObjByte* buffer, struct tm& time, int* pos);
    ObjSymbol* FindSymbol(char ch, int index);
    ObjExpression* GetExpression(const ObjByte* buffer, int* pos);
    void CheckTerm(const ObjByte* buffer, int pos)
    {
        int n = (buffer[1] << 8) + buffer[2];
        if (n != pos)
            ThrowSyntax(buffer, eAll);
    }
    static unsigned GetByte(const ObjByte* buffer, int* pos) { return buffer[(*pos)++]; }
    static unsigned GetWord(const ObjByte* buffer, int* pos)
    {
        unsigned rv = buffer[(*pos)++] << 8;
        rv += buffer[(*pos)++];
        return rv;
    }
    static unsigned GetDWord(const ObjByte* buffer, int* pos)
    {
        unsigned rv = buffer[(*pos)++] << 24;
        rv += buffer[(*pos)++] << 16;
        rv += buffer[(*pos)++] << 8;
        rv += buffer[(*pos)++] << 0;
        return rv;
    }
    static unsigned GetIndex(const ObjByte* buffer, int* pos)
    {
        unsigned rv;
        if (buffer[*pos] & 0x80)
        {
            rv = (buffer[(*pos)++] & 0x7f) << 8;
            rv += buffer[(*pos)++] << 0;
        }
        else
        {
            rv = buffer[(*pos)++] << 16;
            rv += buffer[(*pos)++] << 8;
            rv += buffer[(*pos)++] << 0;
        }
        return rv;
    }
    unsigned embed(int x) { return x | EEMBEDDED; }
    int GetSymbolName(ObjSymbol* Symbol);
    ObjString GetSymbolName(int* index);
    void DefinePointer(ObjType::eType, int index, const ObjByte* buffer, int* pos);
    void DefineBitField(int index, const ObjByte* buffer, int* pos);
    void DefineTypeDef(int index, const ObjByte* buffer, int* pos);
    void DefineArray(ObjType::eType definer, int index, const ObjByte* buffer, int* pos);
    void DefineFunction(int index, const ObjByte* buffer, int* pos);
    void DefineStruct(ObjType::eType, int index, const ObjByte* buffer, int* pos);
    void DefineType(int index, const ObjByte* buffer, int* pos);
    bool GetOffset(const ObjByte* buffer, eParseType ParseType);
    bool CheckSum(const ObjByte* buffer, eParseType ParseType);
    bool TypeSpec(const ObjByte* buffer, eParseType ParseType);
    bool Comment(const ObjByte* buffer, eParseType ParseType);
    bool PublicSymbol(const ObjByte* buffer, eParseType ParseType);
    bool LocalSymbol(const ObjByte* buffer, eParseType ParseType);
    bool AutoSymbol(const ObjByte* buffer, eParseType ParseType);
    bool RegSymbol(const ObjByte* buffer, eParseType ParseType);
    bool TypeName(const ObjByte* buffer, eParseType ParseType);
    bool ExternalSymbol(const ObjByte* buffer, eParseType ParseType);
    bool SectionAttributes(const ObjByte* buffer, eParseType ParseType);
    bool SectionAlignment(const ObjByte* buffer, eParseType ParseType);
    bool SectionDataHeader(const ObjByte* buffer, eParseType ParseType);
    bool Data(const ObjByte* buffer, eParseType ParseType);
    bool EnumeratedData(const ObjByte* buffer, eParseType ParseType);
    bool Fixup(const ObjByte* buffer, eParseType ParseType);
    bool ModuleStart(const ObjByte* buffer, eParseType ParseType);
    bool ModuleEnd(const ObjByte* buffer, eParseType ParseType);
    bool ModuleAttributes(const ObjByte* buffer, eParseType ParseType);
    bool ModuleDate(const ObjByte* buffer, eParseType ParseType);
    [[noreturn]] bool ThrowSyntax(const ObjByte* buffer, eParseType ParseType)
    {
        (void)buffer;
        (void)ParseType;
        if (!first)
        {
            SyntaxError e(lineno);
            throw e;
        }
        else
        {
            BadFileError e(GetName());
            throw e;
        }
    }

    bool Parse(const ObjByte* buffer, eParseType ParseType);

    void ResetCS() { cs = 0; }
    typedef std::vector<ObjSymbol*> SymbolMap;
    typedef std::vector<ObjType*> TypeMap;
    typedef std::vector<ObjSection*> SectionMap;
    typedef std::vector<ObjSourceFile*> FileMap;

    void bufferup(const ObjByte* data, int len);
    void flush()
    {
        fwrite(ioBuffer.get(), ioBufferLen, 1, sfile);
        ioBufferLen = 0;
        fflush(sfile);
    }
    void getline(ObjByte* buf, size_t size);
    void WriteHeader();
    void WriteFiles();
    void WriteSectionHeaders();
    void WriteTypes();
    void WriteVirtualTypes();
    void WriteSymbols();
    void WriteStartAddress();
    void WriteSections();
    void WriteBrowseInfo();
    void WriteTrailer();
    void RenderCS();
    ObjString ToString(const ObjString& strng);
    static int GetTypeIndex(ObjType* Type);
    ObjString GetSymbolName(const ObjByte* buffer, int* index);
    ObjString ToTime(struct tm time);
    void RenderFunction(ObjFunction* Function);
    void RenderStructure(ObjType* Type);
    void RenderComment(eCommentType Type, ObjString strng);
    void RenderFile(ObjSourceFile* File);
    void RenderSymbol(ObjSymbol* Symbol);
    void RenderType(ObjType* Type);
    void RenderSection(ObjSection* Section);
    void RenderDebugTag(ObjDebugTag* Tag);
    void RenderMemory(ObjMemoryManager* Memory);
    void RenderMemoryBinary(ObjMemoryManager* Memory);
    void RenderBrowseInfo(ObjBrowseInfo* Memory);
    void RenderExpression(ObjByte* buf, ObjExpression* Expression);
    void PutSymbol(SymbolMap& map, int index, ObjSymbol* sym);
    ObjSymbol* GetSymbol(SymbolMap& map, int index)
    {
        if (index >= map.size())
            return 0;
        return map[index];
    }
    void PutType(int index, ObjType* type);
    ObjType* GetType(int index)
    {
        if (index >= types.size())
            return 0;
        return types[index];
    }
    void PutSection(int index, ObjSection* sect);
    ObjSection* GetSection(int index)
    {
        if (index >= sections.size())
            return 0;
        return sections[index];
    }
    void PutFile(int index, ObjSourceFile* file);
    ObjSourceFile* GetFile(int index)
    {
        if (index >= files.size())
            return 0;
        return files[index];
    }

    ObjFile* file;
    ObjFactory* factory;
    FILE* sfile;
    ObjInt cs;
    SymbolMap publics;
    SymbolMap locals;
    SymbolMap autos;
    SymbolMap regs;
    SymbolMap externals;
    TypeMap types;
    SectionMap sections;
    FileMap files;
    ObjSection* currentDataSection;
    std::unique_ptr<ObjMemory::DebugTagContainer> currentTags;
    std::unique_ptr<char[]> ioBuffer;
    size_t ioBufferLen;
    size_t ioBufferPos;
    int lineno;
    bool first;
};
class ObjIeeeAscii : public ObjIOBase
{
    enum
    {
        BUFFERSIZE = 32768
    };

  public:
    ObjIeeeAscii(const ObjString& Name, bool CaseSensitive = true) :
        ObjIOBase(Name, CaseSensitive),
        ioBufferLen(0),
        ioBuffer(0),
        ioBufferPos(0),
        sfile(nullptr),
        factory(nullptr),
        file(nullptr),
        cs(0),
        currentDataSection(nullptr),
        lineno(0),
        first(true)

    {
    }
    virtual ~ObjIeeeAscii() {}
    virtual bool Write(FILE* fil, ObjFile* File, ObjFactory* Factory)
    {
        sfile = fil;
        factory = Factory;
        file = File;
        return HandleWrite();
    }
    virtual ObjFile* Read(FILE* fil, eParseType ParseType, ObjFactory* Factory)
    {
        sfile = fil;
        factory = Factory;
        file = nullptr;
        return HandleRead(ParseType);
    }
    virtual bool BinaryWrite(FILE* fil, ObjFile* File, ObjFactory* Factory)
    {
        sfile = fil;
        factory = Factory;
        file = File;
        return BinaryWrite();
    }
    virtual std::string GetErrorQualifier() { return std::string("in line ") + Utils::NumberToString(lineno); }

    struct ParseDataLT
    {
        bool operator()(const char* left, const char* right) const
        {
            if (left[0] < right[0])
                return true;
            if (left[0] == right[0])
                return left[1] < right[1];
            return false;
        }
    };
    class ParseData
    {
      public:
        typedef bool (ObjIeeeAscii::*ParseFunctionType)(const char* buffer, eParseType ParseType);
        ParseData() : name(""), func(&ObjIeeeAscii::ThrowSyntax) {}
        ParseData(const char* Name, ParseFunctionType Func) : name(Name), func(Func) {}
        bool Dispatch(ObjIeeeAscii* instance, const char* inBuffer, eParseType ParseType)
        {
            return (instance->*func)(inBuffer, ParseType);
        }
        const char* GetName() { return name; }

      private:
        const char* name;
        ParseFunctionType func;
    };

  protected:
    class BadCS : public std::domain_error
    {
      public:
        BadCS() : std::domain_error("Bad Checksum") {}
        virtual ~BadCS() noexcept {};
    };
    class SyntaxError : public std::domain_error
    {
      public:
        SyntaxError(int lineno) :
            std::domain_error(std::string("Syntax Error in line ") + Utils::NumberToString(lineno)), lineNo(lineno)
        {
        }
        virtual ~SyntaxError() noexcept {};

        int GetLineNo() const { return lineNo; }

      private:
        int lineNo;
    };
    class BadFileError : public std::domain_error
    {
      public:
        BadFileError(const std::string& name) : std::domain_error(name + " is not an object file") {}
        virtual ~BadFileError() noexcept {};
    };
    friend class ObjIeeeAscii::ParseData;
    bool HandleWrite();
    bool BinaryWrite();
    ObjFile* HandleRead(eParseType Type);

    void GatherCS(const char* cstr);
    ObjFactory* GetFactory() { return factory; }
    ObjFile* GetFile() { return file; }
    ObjString ParseString(const char* buffer, int* pos);
    void ParseTime(const char* buffer, struct tm& time, int* pos);
    ObjSymbol* FindSymbol(char ch, int index);
    ObjExpression* GetExpression(const char* buffer, int* pos);
    void CheckTerm(const char* buffer)
    {
        if (buffer[0] != '.' || buffer[1])
            ThrowSyntax(buffer, eAll);
    }
    ObjString GetSymbolName(ObjSymbol* Symbol);
    ObjString GetSymbolName(int* index);
    void DefinePointer(ObjType::eType, int index, const char* buffer, int* pos);
    void DefineBitField(int index, const char* buffer, int* pos);
    void DefineTypeDef(int index, const char* buffer, int* pos);
    void DefineArray(ObjType::eType definer, int index, const char* buffer, int* pos);
    void DefineFunction(int index, const char* buffer, int* pos);
    void DefineStruct(ObjType::eType, int index, const char* buffer, int* pos);
    void DefineType(int index, const char* buffer, int* pos);
    bool GetOffset(const char* buffer, eParseType ParseType);
    bool CheckSum(const char* buffer, eParseType ParseType);
    bool TypeSpec(const char* buffer, eParseType ParseType);
    bool Comment(const char* buffer, eParseType ParseType);
    bool PublicSymbol(const char* buffer, eParseType ParseType);
    bool LocalSymbol(const char* buffer, eParseType ParseType);
    bool AutoSymbol(const char* buffer, eParseType ParseType);
    bool RegSymbol(const char* buffer, eParseType ParseType);
    bool TypeName(const char* buffer, eParseType ParseType);
    bool ExternalSymbol(const char* buffer, eParseType ParseType);
    bool SectionAttributes(const char* buffer, eParseType ParseType);
    bool SectionAlignment(const char* buffer, eParseType ParseType);
    bool SectionDataHeader(const char* buffer, eParseType ParseType);
    bool Data(const char* buffer, eParseType ParseType);
    bool EnumeratedData(const char* buffer, eParseType ParseType);
    bool Fixup(const char* buffer, eParseType ParseType);
    bool ModuleStart(const char* buffer, eParseType ParseType);
    bool ModuleEnd(const char* buffer, eParseType ParseType);
    bool ModuleAttributes(const char* buffer, eParseType ParseType);
    bool ModuleDate(const char* buffer, eParseType ParseType);
    [[noreturn]] bool ThrowSyntax(const char* buffer, eParseType ParseType)
    {
        (void)buffer;
        (void)ParseType;
        if (!first)
        {
            SyntaxError e(lineno);
            throw e;
        }
        else
        {
            BadFileError e(GetName());
            throw e;
        }
    }

    bool Parse(const char* buffer, eParseType ParseType);

    void ResetCS() { cs = 0; }
    void endl() { bufferup(lineend, 1); }
    void RenderCstr(const char* cstr)
    {
        GatherCS(cstr);
        bufferup(cstr, strlen(cstr));
    }
    void RenderString(const ObjString& strng) { RenderCstr(strng.c_str()); }
    static std::map<const char*, ParseData*, ParseDataLT> parseTree;
    typedef std::vector<ObjSymbol*> SymbolMap;
    typedef std::vector<ObjType*> TypeMap;
    typedef std::vector<ObjSection*> SectionMap;
    typedef std::vector<ObjSourceFile*> FileMap;

    void bufferup(const char* data, int len);
    void flush()
    {
        fwrite(ioBuffer.get(), ioBufferLen, 1, sfile);
        ioBufferLen = 0;
        fflush(sfile);
    }
    void getline(char* buf, size_t size);
    void WriteHeader();
    void WriteFiles();
    void WriteSectionHeaders();
    void WriteTypes();
    void WriteVirtualTypes();
    void WriteSymbols();
    void WriteStartAddress();
    void WriteSections();
    void WriteBrowseInfo();
    void WriteTrailer();
    void RenderCS();
    ObjString ToString(const ObjString& strng);
    ObjString GetTypeIndex(ObjType* Type);
    ObjString GetSymbolName(const char* buffer, int* index);
    ObjString ToTime(struct tm time);
    void RenderFunction(ObjFunction* Function);
    void RenderStructure(ObjType* Type);
    void RenderComment(eCommentType Type, ObjString strng);
    void RenderFile(ObjSourceFile* File);
    void RenderSymbol(ObjSymbol* Symbol);
    void RenderType(ObjType* Type);
    void RenderSection(ObjSection* Section);
    void RenderDebugTag(ObjDebugTag* Tag);
    void RenderMemory(ObjMemoryManager* Memory);
    void RenderMemoryBinary(ObjMemoryManager* Memory);
    void RenderBrowseInfo(ObjBrowseInfo* Memory);
    void RenderExpression(ObjExpression* Expression);
    void PutSymbol(SymbolMap& map, int index, ObjSymbol* sym);
    ObjSymbol* GetSymbol(SymbolMap& map, int index)
    {
        if (index >= map.size())
            return 0;
        return map[index];
    }
    void PutType(int index, ObjType* type);
    ObjType* GetType(int index)
    {
        if (index >= types.size())
            return 0;
        return types[index];
    }
    void PutSection(int index, ObjSection* sect);
    ObjSection* GetSection(int index)
    {
        if (index >= sections.size())
            return 0;
        return sections[index];
    }
    void PutFile(int index, ObjSourceFile* file);
    ObjSourceFile* GetFile(int index)
    {
        if (index >= files.size())
            return 0;
        return files[index];
    }

    ObjFile* file;
    ObjFactory* factory;
    FILE* sfile;
    ObjInt cs;
    static ParseData parseData[];
    SymbolMap publics;
    SymbolMap locals;
    SymbolMap autos;
    SymbolMap regs;
    SymbolMap externals;
    TypeMap types;
    SectionMap sections;
    FileMap files;
    ObjSection* currentDataSection;
    std::unique_ptr<ObjMemory::DebugTagContainer> currentTags;
    std::unique_ptr<char[]> ioBuffer;
    size_t ioBufferLen;
    size_t ioBufferPos;
    int lineno;
    bool first;
    static char lineend[2];
};
class ObjIeee : public ObjIeeeBinary
{
  public:
    ObjIeee(const ObjString& Name, bool CaseSensitive = true) : ObjIeeeBinary(Name, CaseSensitive) {}
    virtual ~ObjIeee() {}
};
class ObjIeeeIndexManager : public ObjIndexManager
{
    enum
    {
        eDerivedTypeBase = 1024
    };

  public:
    ObjIeeeIndexManager() : ObjIndexManager() { ResetIndexes(); }
    virtual ~ObjIeeeIndexManager() {}
    virtual void ResetIndexes();
};
#endif
