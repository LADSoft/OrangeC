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

#ifndef LINKMANAGER_H
#define LINKMANAGER_H

#include <fstream>
#include <set>
#include "ObjTypes.h"
#include "ObjSymbol.h"
#include "CmdFiles.h"
#include "LinkTokenizer.h"
#include <iostream>
#include <cstring>
#include <map>
#include <memory>
#include <deque>
#include <cstdio>

class LibManager;
class LinkPartition;
class LinkPartitionSpecifier;
class LinkLibrary;
class ObjSymbol;
class LinkLibrary;
class ObjFactory;
class ObjIOBase;
class ObjFile;
class ObjIndexManager;
class ObjExpression;
class ObjSection;

void HookError(int);
class LinkSymbolData
{
  public:
    LinkSymbolData(ObjFile* File, ObjSymbol* Symbol) :
        file(File), symbol(Symbol), used(false), visited(false), remapped(false), auxData(nullptr)
    {
    }
    LinkSymbolData(ObjSymbol* Symbol) :
        file(nullptr), symbol(Symbol), used(false), visited(false), remapped(false), auxData(nullptr)
    {
    }
    LinkSymbolData() : file(nullptr), symbol(nullptr), used(false), visited(false), remapped(false), auxData(nullptr) {}
    ~LinkSymbolData() {}

    ObjFile* GetFile() const { return file; }
    void SetFile(ObjFile* File) { file = File; }
    ObjSymbol* GetSymbol() const { return symbol; }
    void SetSymbol(ObjSymbol* sym) { symbol = sym; }
    void SetAuxData(void* data) { auxData = data; }
    void* GetAuxData() const { return auxData; }
    bool GetUsed() const { return used; }
    void SetUsed(bool Used) { used = Used; }
    bool GetVisited() const { return visited; }
    void SetVisited(bool Visited) { visited = Visited; }
    bool GetRemapped() const { return remapped; }
    void SetRemapped(bool Remapped) { remapped = Remapped; }

  private:
    bool used;
    bool remapped;
    bool visited;
    ObjFile* file;
    ObjSymbol* symbol;
    void* auxData;
};
struct linkltcompare
{
    bool operator()(LinkSymbolData* left, LinkSymbolData* right) const
    {
        return strcmp(left->GetSymbol()->GetName().c_str(), right->GetSymbol()->GetName().c_str()) < 0;
    }
    //		{ return left->GetSymbol()->GetName() < right->GetSymbol()->GetName();}
};
class LinkManager
{
    typedef std::vector<std::unique_ptr<LinkPartitionSpecifier>> PartitionData;
    typedef std::set<LinkSymbolData*, linkltcompare> SymbolData;
    typedef std::vector<ObjFile*> FileData;

  public:
    LinkManager(ObjString Specification, bool CaseSensitive, const ObjString OutputFile = "", bool CompleteLink = false,
                bool DebugPassThrough = false, ObjString DebugFile = "");

    ~LinkManager();

    void SetFactory(ObjFactory* Factory) { factory = Factory; }
    ObjIOBase* GetObjIo() { return ioBase; }
    void SetObjIo(ObjIOBase* IoBase) { ioBase = IoBase; }
    void SetIndexManager(ObjIndexManager* Manager) { indexManager = Manager; }
    void AddObject(const ObjString& name);
    void AddLibrary(const ObjString& name);
    void SetLibPath(const ObjString& path) { libPath = path; }
    void SetOutputFile(const ObjString& name) { outputFile = name; }
    ObjString GetOutputFile() const { return outputFile; }
    void Link();

    typedef PartitionData::iterator PartitionIterator;

    PartitionIterator PartitionBegin() { return partitions.begin(); }
    PartitionIterator PartitionEnd() { return partitions.end(); }

    typedef SymbolData::iterator SymbolIterator;
    void InsertSymbol(LinkSymbolData* data) { publics.insert(data); }

    void EnterVirtualSection(ObjSection* sect) { virtualSections.push_back(sect); }
    void MapSectionToParent(ObjSection* child, ObjSection* parent) { parentSections[child] = parent; }

    SymbolIterator PublicBegin() { return publics.begin(); }
    SymbolIterator PublicEnd() { return publics.end(); }
    SymbolIterator PublicFind(LinkSymbolData* d) { return publics.find(d); }
    SymbolIterator ExternalBegin() { return externals.begin(); }
    SymbolIterator ExternalEnd() { return externals.end(); }
    SymbolIterator ImportBegin() { return imports.begin(); }
    SymbolIterator ImportEnd() { return imports.end(); }
    SymbolData GetImports() { return imports; }
    typedef FileData::iterator FileIterator;

    FileIterator FileBegin() { return fileData.begin(); }
    FileIterator FileEnd() { return fileData.end(); }

    static void LinkError(const ObjString& error)
    {
        HookError(0);
        std::cout << "Error: " << error << std::endl;
        errors++;
    }
    static void LinkWarning(const ObjString& error)
    {
        HookError(1);
        std::cout << "Warning: " << error << std::endl;
        warnings++;
    }

    FILE* GetLibraryPath(const std::string& stem, std::string& name);

    bool IsExternal(ObjString& name)
    {
        ObjSymbol symbol(name, ObjSymbol::eLabel, 0);
        LinkSymbolData data(&symbol);
        SymbolIterator it = externals.find(&data);
        if (it != externals.end())
            return true;
        return false;
    }
    int ErrCount() const { return errors; }
    int WarnCount() const { return warnings; }
    bool HasVirtual(std::string name);

  private:
    void LoadExterns(ObjFile* file, ObjExpression* exp);
    void LoadSectionExternals(ObjFile* file, ObjSection* section);
    void MarkExternals(ObjFile* file);
    void MergePublics(ObjFile* file, bool toerr);
    bool ScanVirtuals();
    void LoadFiles();
    std::unique_ptr<LinkLibrary> OpenLibrary(const ObjString& name);
    void LoadLibraries();
    bool LoadLibrarySymbol(LinkLibrary* lib, const std::string& name);
    void ScanLibraries();
    void CloseLibraries();
    bool ParseAssignment(LinkTokenizer& spec);
    bool CreateSeparateRegions(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec);
    bool ParsePartitions();
    void CreatePartitions();
    void PlaceSections();
    void UnplacedWarnings();
    bool ExternalErrors();
    void AddGlobalsForVirtuals(ObjFile* file);
    void CreateOutputFile();
    ObjString outputFile;
    LinkTokenizer specification;
    PartitionData partitions;
    SymbolData virtsections;
    SymbolData publics;
    SymbolData externals;
    SymbolData imports;
    SymbolData exports;
    std::set<std::string> importNames;
    FileData fileData;
    CmdFiles objectFiles;
    CmdFiles libFiles;
    std::deque<std::unique_ptr<LinkLibrary>> dictionaries;
    std::vector<ObjSection*> virtualSections;
    std::map<ObjSection*, ObjSection*> parentSections;
    ObjIOBase* ioBase;
    ObjIndexManager* indexManager;
    ObjFactory* factory;
    ObjString libPath;
    ObjString specName;
    ObjString debugFile;
    bool completeLink;
    bool caseSensitive;
    bool debugPassThrough;
    static int errors;
    static int warnings;
};
#endif
