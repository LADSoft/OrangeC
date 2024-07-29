/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#ifndef LIBMANAGER_H
#define LIBMANAGER_H

#include <vector>
#include <set>
#include <cstdio>
#include "ObjTypes.h"
#include "LibDictionary.h"
#include "LibFiles.h"

class ObjSymbol;
class ObjFile;
class ObjFactory;

class LibManager
{
  public:
    enum
    {
        CANNOT_CREATE = -1,
        CANNOT_READ = -2,
        CANNOT_WRITE = -3,
        SUCCESS = 0
    };
    LibManager(const ObjString& Name, bool noexport, bool CaseSensitive = true) :
        dictionary(CaseSensitive), name(Name), files(CaseSensitive, noexport)
    {
        stream = fopen(Name.c_str(), "rb");
        InitHeader();
    }
    ~LibManager() { Close(); }

    LibFiles& Files() { return files; }
    void AddFile(const ObjString& name) { files.Add(name); }
    void AddFile(ObjFile& obj) { files.Add(obj); }
    void RemoveFile(const ObjString& name) { files.Remove(name); }
    void ExtractFile(const ObjString& name) { files.Extract(stream, name); }
    void ReplaceFile(const ObjString& name) { files.Replace(name); }
    void ReplaceFile(ObjFile& obj) { files.Replace(obj); }
    const std::vector<unsigned>& Lookup(const ObjString& name);
    ObjFile* LoadModule(ObjInt index, ObjFactory* factory) { return files.LoadModule(stream, index, factory); }
    bool LoadLibrary();
    int SaveLibrary();
    bool fail() const { return false; }  // stream.fail(); }
    bool IsOpen() const { return stream != nullptr; }
    void Close()
    {
        if (stream)
            fclose(stream);
    }
    enum
    {
        ALIGN = 512
    };
    struct LibHeader
    {
        enum
        {
            LIB_SIG = 0x4442494c
        };
        unsigned sig;
        unsigned filesInModule;
        unsigned offsetsOffset;
        unsigned namesOffset;
        unsigned filesOffset;
        unsigned dictionaryOffset;
        unsigned dictionaryBlocks;
        unsigned reserved;
    };

  protected:
    bool Align(FILE* ostr, ObjInt align = ALIGN);
    void InitHeader();
    bool WriteHeader()
    {
        if (!fseek(stream, 0, SEEK_SET))
            return false;
        if (fwrite((char*)&header, sizeof(header), 1, stream) != 1)
            return false;
        return true;
    }
    bool ReadHeader()
    {
        if (!fseek(stream, 0, SEEK_SET))
            return false;
        if (fread((char*)&header, sizeof(header), 1, stream) != 1)
            return false;
        return true;
    }

  private:
    LibHeader header;
    FILE* stream;
    LibFiles files;
    LibDictionary dictionary;
    ObjString name;
};
#endif
