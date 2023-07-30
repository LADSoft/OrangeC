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

#include "ObjSection.h"
#include "ObjFile.h"
#include "ObjIO.h"
#include "ObjFactory.h"
#include "ObjUtil.h"
#include "ObjType.h"
#include "LinkManager.h"
#include "LinkPartition.h"
#include "LinkTokenizer.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "LinkRemapper.h"
#include "LinkLibrary.h"
#include "LinkDebugFile.h"
#include "LinkDll.h"
#include "Utils.h"
#include <memory>
#include <fstream>
#include <cstdio>
#include <climits>
#include <algorithm>

int LinkManager::errors;
int LinkManager::warnings;

void HookError(int aa) {}

LinkManager::LinkManager(ObjString Specification, bool CaseSensitive, const ObjString OutputFile, bool CompleteLink,
                         bool DebugPassThrough, ObjString DebugFile) :
    specification(Specification),
    outputFile(OutputFile),
    specName(Specification),
    factory(nullptr),
    indexManager(nullptr),
    completeLink(CompleteLink),
    ioBase(nullptr),
    caseSensitive(CaseSensitive),
    debugPassThrough(DebugPassThrough),
    debugFile(DebugFile)
{
}

LinkManager::~LinkManager()
{
    for (auto s : publics)
        delete s;
    for (auto s : externals)
        delete s;
    for (auto s : imports)
        delete s;
    for (auto s : exports)
        delete s;
}
void LinkManager::SetDelayLoad(const ObjString& list)
{
    auto names = Utils::split(list);
    for (auto&& n : names)
    {
        std::transform(n.begin(), n.end(), n.begin(), ::toupper);
        delayLoadNames.insert(n);
    }
}
void LinkManager::AddObject(const ObjString& name)
{
    if (!objectFiles.Add(name))
    {
        if (name.find_first_of('*') == std::string::npos && name.find_first_of('?') == std::string::npos)
            LinkError("Object file " + name + " does not exist");
    }
}
void LinkManager::AddLibrary(const ObjString& name) { libFiles.Add(name); }
void LinkManager::LoadExterns(ObjFile* file, ObjExpression* exp)
{
    if (exp->GetOperator() == ObjExpression::eSymbol)
    {
        if (exp->GetSymbol()->GetType() == ObjSymbol::eExternal)
        {
            LinkSymbolData test(file, exp->GetSymbol());
            auto it = publics.find(&test);
            if (it != publics.end())
            {
                (*it)->SetUsed(true);
            }
            else
            {
                it = virtsections.find(&test);
                if (it == virtsections.end() || !(*it)->GetUsed())
                {
                    if (externals.find(&test) == externals.end())
                    {
                        LinkSymbolData* newSymbol = new LinkSymbolData(file, exp->GetSymbol());
                        externals.insert(newSymbol);
                    }
                }
            }
        }
    }
    else if (exp->GetOperator() == ObjExpression::eSection)
    {
        ObjSection* sect = exp->GetSection();
        if (sect->GetQuals() & ObjSection::virt)
        {
            int n = sect->GetName().find('@');
            if (n != std::string::npos)
            {
                std::string name = sect->GetName().substr(n);
                ObjSymbol sym(name, ObjSymbol::ePublic, -1);
                LinkSymbolData test(file, &sym);
                auto it = virtsections.find(&test);
                if (it == virtsections.end() || !(*it)->GetUsed())
                {
                    LinkSymbolData* newSymbol = new LinkSymbolData(file, new ObjSymbol(sym));
                    externals.insert(newSymbol);
                }
            }
        }
    }
    else
    {
        if (exp->GetLeft())
        {
            LoadExterns(file, exp->GetLeft());
        }
        if (exp->GetRight())
        {
            LoadExterns(file, exp->GetRight());
        }
    }
}
void LinkManager::LoadSectionExternals(ObjFile* file, ObjSection* section)
{
    if (section)
    {
        section->SetUsed(true);
        ObjMemoryManager& memManager = section->GetMemoryManager();
        for (auto mem : memManager)
        {
            if (mem->GetFixup())
            {
                LoadExterns(file, mem->GetFixup());
            }
        }
    }
}
void LinkManager::MarkExternals(ObjFile* file)
{
    for (ObjFile::SymbolIterator it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        LinkSymbolData sym(*it);
        SymbolIterator it1 = publics.find(&sym);
        if (it1 != publics.end())
        {
            (*it1)->SetUsed(true);
        }
        SymbolIterator its = imports.find(&sym);
        if (its != imports.end())
        {
            (*its)->SetUsed(true);
        }
        SymbolIterator itv = virtsections.find(&sym);
        if (itv != virtsections.end())
        {
            (*itv)->SetUsed(true);
            (*itv)->SetRemapped(true);
        }
    }
}
void LinkManager::MergePublics(ObjFile* file, bool toerr)
{
    for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (publics.find(&test) != publics.end() || virtsections.find(&test) != virtsections.end())
        {
            if (toerr)
                LinkError("Duplicate public " + (*it)->GetDisplayName() + " in module " + file->GetName());
        }
        else
        {
            if ((*it)->GetName() == "___delayLoadHelper2")
            {
                delayLoadLoaded = true;
            }
            LinkSymbolData* newSymbol = new LinkSymbolData(file, *it);
            publics.insert(newSymbol);
            auto it = exports.find(newSymbol);
            if (it != exports.end())
            {
                (*it)->SetUsed(true);
                newSymbol->SetUsed(true);
            }
            it = externals.find(newSymbol);
            if (it != externals.end())
            {
                (*it)->SetUsed(true);
                delete (*it);
                externals.erase(it);
            }
        }
    }
    for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        importNames.insert((*it)->GetName());
        if (static_cast<ObjImportSymbol*>(*it)->GetDllName().size())
        {
            LinkSymbolData test(file, *it);
            if (imports.find(&test) == imports.end())
            {
                LinkSymbolData* newSymbol = new LinkSymbolData(file, *it);
                imports.insert(newSymbol);
            }
        }
    }
    for (auto it = file->ExportBegin(); it != file->ExportEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (exports.find(&test) == exports.end())
        {
            LinkSymbolData* newSymbol = new LinkSymbolData(file, *it);
            exports.insert(newSymbol);
            SymbolIterator it1 = publics.find(newSymbol);
            if (it1 != publics.end())
            {
                (*it1)->SetUsed(true);
                newSymbol->SetUsed(true);
            }
            else
            {
                int n = (*it)->GetName().find('@');
                if (n != std::string::npos)
                {
                    std::string name = (*it)->GetName().substr(n);
                    ObjSymbol sym(name, ObjSymbol::ePublic, -1);
                    LinkSymbolData test(file, &sym);
                    if (publics.find(&test) != publics.end())
                    {
                        if (toerr)
                            LinkError("Duplicate public " + (*it)->GetName() + " in module " + file->GetName());
                    }
                    else
                    {
                        auto it1 = virtsections.find(&test);
                        if (it1 == virtsections.end())
                        {
                            LinkSymbolData* newSymbol = new LinkSymbolData(file, new ObjSymbol(sym));
                            virtsections.insert(newSymbol);
                            // don't set used here as we want to scan the section's externals later
                        }
                    }
                }
            }
        }
    }
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        if (!((*it)->GetQuals() & ObjSection::virt))
        {
            LoadSectionExternals(file, *it);
        }
        else
        {
            int n = (*it)->GetName().find('@');
            if (n != std::string::npos)
            {
                std::string name = (*it)->GetName().substr(n);
                ObjSymbol sym(name, ObjSymbol::ePublic, -1);
                LinkSymbolData test(file, &sym);
                if (publics.find(&test) != publics.end())
                {
                    if (toerr)
                        LinkError("Duplicate public " + (*it)->GetName() + " in module " + file->GetName());
                }
                else
                {
                    auto it1 = virtsections.find(&test);
                    if (it1 == virtsections.end())
                    {
                        LinkSymbolData* newSymbol = new LinkSymbolData(file, new ObjSymbol(sym));
                        newSymbol->SetAuxData(*it);
                        virtsections.insert(newSymbol);
                    }
                    else if (!(*it1)->GetAuxData())
                    {
                        (*it1)->SetAuxData(*it);
                    }
                }
            }
        }
    }
    for (auto it = externals.begin(); it != externals.end();)
    {
        auto it1 = publics.find(*it);
        if (it1 != publics.end())
        {
            (*it)->SetUsed(true);
            (*it1)->SetUsed(true);
            delete (*it);
            externals.erase(it++);
        }
        else
        {
            auto its = imports.find(*it);
            if (its != imports.end())
            {
                (*it)->SetUsed(true);
                (*its)->SetUsed(true);
                ++it;
            }
            else
            {
                ++it;
            }
        }
    }
}
bool LinkManager::HasVirtual(std::string name)
{
    int n = name.find('@');
    if (n != std::string::npos)
    {
        name = name.substr(n);
        ObjSymbol sym(name, ObjSymbol::ePublic, -1);
        ObjFile file(name);
        LinkSymbolData test(&file, &sym);
        auto it = virtsections.find(&test);
        if (it != virtsections.end())
        {
            return (*it)->GetUsed();
        }
    }
    return false;
}
bool LinkManager::ScanVirtuals()
{
    bool rv = false;
    for (auto sym : virtsections)
    {
        if (sym->GetRemapped())
        {
            sym->SetRemapped(false);
            LoadSectionExternals(sym->GetFile(), (ObjSection*)sym->GetAuxData());
            rv = true;
        }
    }
    for (auto it = externals.begin(); it != externals.end();)
    {
        auto it1 = virtsections.find(*it);
        if (it1 != virtsections.end())
        {
            if (!(*it1)->GetUsed())
            {
                (*it1)->SetUsed(true);
                LoadSectionExternals((*it1)->GetFile(), (ObjSection*)(*it1)->GetAuxData());
            }
            (*it)->SetUsed(true);
            delete (*it);
            externals.erase(it++);
            rv = true;
        }
        else
        {
            ++it;
        }
    }
    for (auto it = exports.begin(); it != exports.end(); ++it)
    {
        auto it1 = virtsections.find(*it);
        if (it1 != virtsections.end())
        {
            if (!(*it1)->GetUsed())
            {
                (*it1)->SetUsed(true);
                LoadSectionExternals((*it1)->GetFile(), (ObjSection*)(*it1)->GetAuxData());
            }
        }
    }
    return rv;
}
FILE* LinkManager::GetLibraryPath(const std::string& stem, std::string& name)
{
    FILE* infile = fopen(name.c_str(), "rb");
    if (!infile)
    {
        std::string hold = libPath;
        while (!hold.empty())
        {
            std::string next;
            size_t npos = hold.find(";");
            if (npos == std::string::npos)
            {
                next = hold;
                hold = "";
            }
            else
            {
                next = hold.substr(0, npos);
                if (npos + 1 < hold.size())
                    hold = hold.substr(npos + 1);
                else
                    hold = "";
            }
            name = Utils::FullPath(next, stem);
            infile = fopen(name.c_str(), "rb");
            if (infile)
                hold = "";
        }
    }
    return infile;
}
void LinkManager::LoadFiles()
{
    if (!factory || !indexManager || !ioBase)
    {
        return;
    }
    int bpmau = INT_MAX;
    int mau = 1;
    for (auto name : objectFiles)
    {
        std::string path;
        FILE* infile = GetLibraryPath(name, path);
        if (infile)
        {
            ObjFile* file = ioBase->Read(infile, ObjIOBase::eAll, factory);
            if (!file)
            {
                LinkError("Invalid object file " + ioBase->GetErrorQualifier() + " in " + name);
            }
            else
            {
                file->SetInputName(name);
                if (ioBase->GetBitsPerMAU() < bpmau)
                    bpmau = ioBase->GetBitsPerMAU();
                if (ioBase->GetMAUS() > mau)
                    mau = ioBase->GetMAUS();
                fileData.push_back(file);
                MergePublics(file, true);
            }
            fclose(infile);
        }
        else
        {
            LinkError("Input file '" + name + "' does not exist.");
        }
    }
    ioBase->SetBitsPerMAU(bpmau);
    ioBase->SetMAUS(mau);
}
std::unique_ptr<LinkLibrary> LinkManager::OpenLibrary(const ObjString& name)
{
    std::unique_ptr<LinkLibrary> rv = std::make_unique<LinkLibrary>(name, caseSensitive);
    if (!rv || !rv->IsOpen())
    {
        rv.release();
        rv = std::make_unique<LinkLibrary>(Utils::FindOnPath(name, libPath), caseSensitive);
    }
    if (rv)
    {
        if (rv->IsOpen())
        {
            if (!rv->Load())
            {
                rv.release();
            }
        }
        else
        {
            rv.release();
        }
    }
    return rv;
}
void LinkManager::LoadLibraries()
{
    for (auto name : libFiles)
    {
        LinkDll checker(name, libPath, true);
        if (checker.IsDll())
        {
            if (checker.MatchesArchitecture())
            {
                // stdcall version preferred
                auto temp = std::move(checker.LoadLibrary(true));
                if (!temp)
                    LinkError("Internal error while processing '" + name + "'");
                else
                {
                    dictionaries.push_back(std::move(temp));
                    // will fall back to C version
                    temp = std::move(checker.LoadLibrary(false));
                    if (!temp)
                        LinkError("Internal error while processing '" + name + "'");
                    else
                        dictionaries.push_back(std::move(temp));
                }
            }
            else
            {
                LinkError("Dll Library '" + name + "' doesn't match architecture");
            }
        }
        else
        {
            std::unique_ptr<LinkLibrary> newLibrary = std::move(OpenLibrary(name));
            if (newLibrary)
            {
                dictionaries.push_back(std::move(newLibrary));
            }
            else
            {
                LinkError("Library '" + name + "' does not exist or is not a library");
            }
        }
    }
}
bool LinkManager::LoadLibrarySymbol(LinkLibrary* lib, const std::string& name)
{
    bool found = false;
    ObjInt objNum = lib->GetSymbol(name);
    if (objNum >= 0 && !lib->HasModule(objNum))
    {
        ObjFile* file = lib->LoadSymbol(objNum, factory);
        if (!file)
        {
            LinkError("Invalid object file " + ioBase->GetErrorQualifier() + " in library " + lib->GetName());
        }
        else
        {
            fileData.push_back(file);
            MergePublics(file, false);
        }
        found = true;
    }
    return found;
}
void LinkManager::ScanLibraries()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto&& d : dictionaries)
        {
            bool changed1 = true;
            while (changed1)
            {
                changed1 = false;
                for (auto&& extit = externals.begin(); extit != externals.end(); ++extit)
                {
                    if (!(*extit)->GetUsed() && virtsections.find(*extit) == virtsections.end())
                    {
                        bool found = LoadLibrarySymbol(d.get(), (*extit)->GetSymbol()->GetName());
                        // not resolved?
                        if (found)
                        {
                            changed = true;
                            changed1 = true;
                            break;
                        }
                    }
                }
                if (!changed1 && !delayLoadLoaded && delayLoadNames.size())
                {
                    bool found = LoadLibrarySymbol(d.get(), "___delayLoadHelper2");
                    // not resolved?
                    if (found)
                    {
                        delayLoadLoaded = true;
                        changed = true;
                        changed1 = true;
                        break;
                    }
                }
            }
        }
    }
}
void LinkManager::CloseLibraries() { dictionaries.clear(); }
bool LinkManager::ParseAssignment(LinkTokenizer& spec)
{
    ObjString symName = spec.GetSymbol();
    LinkExpression* value;
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(&value, true))
        return false;
    std::unique_ptr<LinkExpressionSymbol> esym = std::make_unique<LinkExpressionSymbol>(symName, value);
    if (!LinkExpression::EnterSymbol(esym.get()))
    {
        LinkManager::LinkError("Symbol " + symName + " redefined");
    }
    else
    {
        partitions.push_back(std::make_unique<LinkPartitionSpecifier>(esym.release()));
    }
    return spec.MustMatch(LinkTokenizer::eSemi);
}
bool LinkManager::CreateSeparateRegions(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec)
{
    std::unique_ptr<LinkPartition> newPartition = std::make_unique<LinkPartition>(this);
    std::unique_ptr<LinkOverlay> newOverlay = std::make_unique<LinkOverlay>(newPartition.get());
    std::unique_ptr<LinkRegion> newRegion = std::make_unique<LinkRegion>(newOverlay.get());
    if (!newRegion->ParseRegionSpec(manager, files, spec))
        return false;
    if (!spec.MustMatch(LinkTokenizer::eSemi))
        return false;
    for (auto itr = newRegion->NowDataBegin(); itr != newRegion->NowDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkPartition* partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(std::make_unique<LinkPartitionSpecifier>(partition));
            LinkOverlay* overlay = new LinkOverlay(partition);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion* region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    for (auto itr = newRegion->NormalDataBegin(); itr != newRegion->NormalDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkPartition* partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(std::make_unique<LinkPartitionSpecifier>(partition));
            LinkOverlay* overlay = new LinkOverlay(partition);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion* region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    for (auto itr = newRegion->PostponeDataBegin(); itr != newRegion->PostponeDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkPartition* partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(std::make_unique<LinkPartitionSpecifier>(partition));
            LinkOverlay* overlay = new LinkOverlay(partition);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion* region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    return true;
}
bool LinkManager::ParsePartitions()
{
    bool done = false;

    LinkExpression* value = new LinkExpression(externals.size());
    LinkExpressionSymbol* esym = new LinkExpressionSymbol("IMPORTCOUNT", value);
    (void)LinkExpression::EnterSymbol(esym);
    while (!done)
    {
        if (specification.Matches(LinkTokenizer::eSymbol))
        {
            if (!ParseAssignment(specification))
                return false;
        }
        else if (specification.Matches(LinkTokenizer::eRegion))
        {
            specification.NextToken();
            if (!CreateSeparateRegions(this, objectFiles, specification))
                return false;
        }
        else if (!specification.MustMatch(LinkTokenizer::ePartition))
        {
            done = true;
        }
        else
        {
            LinkPartition* newPartition = new LinkPartition(this);
            partitions.push_back(std::make_unique<LinkPartitionSpecifier>(newPartition));
            if (!newPartition->ParsePartitionSpec(this, objectFiles, specification))
                return false;
            if (!specification.MustMatch(LinkTokenizer::eSemi))
                return false;
        }
    }
    return specification.MustMatch(LinkTokenizer::eEOF);
}
void LinkManager::CreatePartitions()
{
    std::map<ObjString, LinkRegion*> createdRegions;
    for (auto file : fileData)
    {
        for (auto its = file->SectionBegin(); its != file->SectionEnd(); ++its)
        {
            ObjString name = (*its)->GetName();
            auto itr = createdRegions.find(name);
            if (itr == createdRegions.end())
            {
                LinkPartition* partition = new LinkPartition(this);
                partitions.push_back(std::make_unique<LinkPartitionSpecifier>(partition));
                LinkOverlay* overlay = new LinkOverlay(partition);
                LinkOverlaySpecifier* ospec = new LinkOverlaySpecifier(overlay);
                partition->Add(ospec);
                partition->SetName(name);
                overlay->SetName(name);
                LinkRegion* region = new LinkRegion(overlay);
                LinkRegionSpecifier* rspec = new LinkRegionSpecifier(region);
                overlay->Add(rspec);
                region->SetName(name);
                region->AddNormalData(file, (*its));
                createdRegions[name] = region;
            }
            else
            {
                itr->second->AddNormalData(file, (*its));
            }
        }
    }
}
void LinkManager::PlaceSections()
{
    try
    {
        int bottom = 0;
        int overlayNum = 0;
        for (auto& partition : partitions)
        {
            if (partition->GetSymbol())
            {
                if (completeLink)
                    partition->GetSymbol()->GetValue()->Eval(bottom);
            }
            else
            {
                bottom = partition->GetPartition()->PlacePartition(this, bottom, completeLink, overlayNum);
            }
        }
    }
    catch (std::bad_exception v)
    {
        LinkError("Cannot Evaluate items in SPC file");
        return;
    }
}
void LinkManager::UnplacedWarnings()
{
    for (auto file : fileData)
        for (auto its = file->SectionBegin(); its != file->SectionEnd(); ++its)
            if (!(*its)->GetUtilityFlag())
                LinkError("Section " + (*its)->GetName() + " unused in module " + file->GetName());
}
bool LinkManager::ExternalErrors()
{
    bool rv = false;
    bool done = false;
    while (!done)
    {
        done = true;
        for (auto it = externals.begin(); it != externals.end(); ++it)
        {
            if (LinkExpression::FindSymbol((*it)->GetSymbol()->GetName()))
            {
                externals.erase(it);
                done = false;
                break;
            }
        }
    }
    int n = 0;
    for (auto ext : externals)
    {
        bool found = imports.find(ext) != imports.end();
        if (!found)
        {
            LinkError("Undefined External '" + ext->GetSymbol()->GetDisplayName() + "' in module " + ext->GetFile()->GetName());
            rv = true;
        }
    }
    for (auto pub : publics)
    {
        delayLoadLoaded |= pub->GetSymbol()->GetName() == "___delayLoadHelper2";
        bool found = importNames.find(pub->GetSymbol()->GetName()) != importNames.end();
        if (found)
        {
            LinkWarning("Public '" + pub->GetSymbol()->GetDisplayName() + "' was also declared as an imported function");
        }
    }
    if (delayLoadNames.size() && !delayLoadLoaded)
    {
        LinkError("Undefined External '____delayLoadHelper2'");
        rv = true;
    }
    return rv;
}
void LinkManager::AddGlobalsForVirtuals(ObjFile* file)
{
    int index = file->PublicSize();
    for (auto v : virtsections)
    {
        if (v->GetUsed())
        {
            ObjSymbol* s = v->GetSymbol();
            LinkExpressionSymbol* sym = LinkExpression::FindSymbol(s->GetName());
            if (sym)
            {
                ObjExpression* exp =
                    new ObjExpression(ObjExpression::eAdd, new ObjExpression(sym->GetValue()->GetUnresolvedSection()),
                                      new ObjExpression(sym->GetValue()->GetUnresolvedSection()->GetBase()));
                s->SetIndex(index++);
                s->SetOffset(exp);
                file->Add(s);
            }
        }
    }
}
void LinkManager::CreateOutputFile()
{
    LinkRemapper remapper(*this, *factory, *indexManager, completeLink);
    ObjFile* file = remapper.Remap();
    if (!file)
    {
        LinkError("Internal error");
    }
    else
    {
        FILE* ofile = fopen(outputFile.c_str(), "wb");
        if (ofile != nullptr)
        {
            // copy the definitions into the rel file
            for (auto it = LinkExpression::begin(); it != LinkExpression::end(); ++it)
            {
                ObjDefinitionSymbol* d = factory->MakeDefinitionSymbol((*it)->GetName());
                d->SetValue((*it)->GetValue()->Eval(0));
                file->Add(d);
            }
            if (completeLink)
            {
                AddGlobalsForVirtuals(file);
                ioBase->SetAbsolute(true);
                if (!debugPassThrough)
                {
                    ioBase->SetDebugInfoFlag(false);
                    if (!debugFile.empty())
                    {
                        LinkDebugFile df(debugFile, file, this->virtualSections, this->parentSections);
                        if (!df.CreateOutput())
                        {
                            Utils::Fatal("Cannot open database '%s' for write", debugFile.c_str());
                        }
                    }
                }
            }
            else
            {
                ioBase->SetAbsolute(false);
            }
            ioBase->Write(ofile, file, factory);
            fclose(ofile);
        }
        else
        {
            Utils::Fatal("Cannot open '%s' for write", outputFile.c_str());
        }
        delete file;
    }
}
void LinkManager::SetDelayParams() 
{
    LinkExpression* value = new LinkExpression(4 * delayLoadNames.size() );
    LinkExpressionSymbol* esym = new LinkExpressionSymbol("DELAYLOADHANDLESIZE", value);
    (void)LinkExpression::EnterSymbol(esym);
    int rv = 0;
    if (delayLoadNames.size())
    {
        std::set<std::string> externalList;
        for (auto&& e : externals)
            externalList.insert(e->GetSymbol()->GetName());
        rv += delayLoadNames.size() * DelayLoadModuleThunkSize;
        for (auto&& i : imports)
        {
            ObjImportSymbol* s = reinterpret_cast<ObjImportSymbol*>(i->GetSymbol());
            // uppercase the module name for NT... 98 doesn't need it but can accept it
            std::string name = s->GetDllName();
            std::transform(name.begin(), name.end(), name.begin(), ::toupper);
            if (delayLoadNames.find(name) != delayLoadNames.end() && externalList.find(s->GetName()) != externalList.end())
            {
                rv += DelayLoadThunkSize;
            }
        }
    }

    value = new LinkExpression(rv);
    esym = new LinkExpressionSymbol("DELAYLOADTHUNKSIZE", value);
    (void)LinkExpression::EnterSymbol(esym);
}
void LinkManager::Link()
    {
    if (!objectFiles.size())
    {
        LinkError("No input files specified");
        return;
    }
    LoadFiles();
    if (completeLink)
    {
        if (!externals.empty())
        {
            for (auto file : fileData)
                MarkExternals(file);
            LoadLibraries();
            do
            {
                ScanLibraries();
            } while (ScanVirtuals());
        }
    }
    SetDelayParams();
    if (specName.empty())
    {
        CreatePartitions();
    }
    else
    {
        if (!ParsePartitions())
        {
            std::string err =
                "Specification file line " + Utils::NumberToString(specification.GetLineNo()) + specification.GetError();
            LinkError(err);
        }
    }
    PlaceSections();
    if (completeLink)
    {
        if (!ExternalErrors())
            UnplacedWarnings();
    }
    if (errors || warnings)
        std::cout << "\t" << errors << " Errors, " << warnings << " Warnings" << std::endl;
    if (errors)
    {
        std::cout << "\tErrors encountered, not creating output file" << std::endl;
    }
    else
    {
        CreateOutputFile();
    }
}
