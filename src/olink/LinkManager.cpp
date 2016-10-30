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
#include "ObjSection.h"
#include "ObjFile.h"
#include "ObjIo.h"
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
#include "Utils.h"

#include <fstream>
#include <stdio.h>
#include <limits.h>

int LinkManager::errors;
int LinkManager::warnings;

void HookError(int aa)
{
}
LinkManager::~LinkManager()
{
    for (auto d : dictionaries)
        delete d;
    for (auto p : partitions)
        delete p;
    for (auto s : publics)
        delete s;
    for (auto s : externals)
        delete s;
    for (auto s : imports)
        delete s;
    for (auto s : exports)
        delete s;
}
void LinkManager::AddObject(const ObjString &name) 
{
    if (!objectFiles.Add(name))
    {
        if (!name.find_first_of('*') && !name.find_first_of('?'))
            LinkError("Object file " + name + " does not exist");
                     
    }
}
void LinkManager::AddLibrary(const ObjString &name) 
{ 
    libFiles.Add(name); 
}
void LinkManager::LoadExterns(ObjFile *file, ObjExpression *exp)
{
    if (exp->GetOperator() == ObjExpression::eSymbol)
    {
        if (exp->GetSymbol()->GetType()== ObjSymbol::eExternal)
        {
            LinkSymbolData test(file, exp->GetSymbol());
            if (externals.find(&test) == externals.end())
            {
                SymbolIterator it = publics.find(&test);
                if (it != publics.end())
                {
                    (*it)->SetUsed(true);
                }
                else
                {
                    it = virtsections.find(&test);
                    if (it == virtsections.end() || !(*it)->GetUsed())
                    {                    
                        LinkSymbolData *newSymbol = new LinkSymbolData(file, exp->GetSymbol());
                        externals.insert(newSymbol);
                    }
                }
            }
        }
    }
    else if (exp->GetOperator() == ObjExpression::eSection)
    {
        ObjSection *sect = exp->GetSection();
        if (sect->GetQuals() & ObjSection::virt)
        {
            int n = sect->GetName().find('@');
            if (n != std::string::npos)
            {
                std::string name = sect->GetName().substr(n);
                ObjSymbol sym(name, ObjSymbol::ePublic, -1);
                LinkSymbolData test(file, &sym);
                SymbolIterator it = virtsections.find(&test);
                if (it == virtsections.end() || !(*it)->GetUsed())
                {                    
                    LinkSymbolData *newSymbol = new LinkSymbolData(file, new ObjSymbol(sym));
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
void LinkManager::LoadSectionExternals(ObjFile *file, ObjSection *section)
{
    if (section)
    {
        section->SetUsed(true);
        ObjMemoryManager &memManager = section->GetMemoryManager();
        for (ObjMemoryManager::MemoryIterator it = memManager.MemoryBegin();
             it != memManager.MemoryEnd(); ++it)
        {
            if ((*it)->GetFixup())
            {
                LoadExterns(file, (*it)->GetFixup());
            }
        }
    }
}
#include <stdio.h>
void LinkManager::MergePublics(ObjFile *file, bool toerr)
{
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (publics.find(&test) != publics.end() || virtsections.find(&test) != virtsections.end())
        {
            if (toerr)
                LinkError("Duplicate public " + (*it)->GetDisplayName() + " in module " + file->GetName());
        }
        else
        {
            LinkSymbolData *newSymbol = new LinkSymbolData(file, *it);
            publics.insert(newSymbol);
            SymbolIterator it = exports.find(newSymbol);
            if (it != exports.end())
            {
                (*it)->SetUsed(true);
                newSymbol->SetUsed(true);
            }
            it = externals.find(newSymbol);
            if (it != externals.end())
            {
                (*it)->SetUsed(true);
                LinkSymbolData *p = *it;
                externals.erase(it);
                delete p;
            }
        }
    }
    for (ObjFile::SymbolIterator it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (imports.find(&test) == imports.end())
        {
            LinkSymbolData *newSymbol = new LinkSymbolData(file, *it);
            imports.insert(newSymbol);
        }
    }
    for (ObjFile::SymbolIterator it = file->ExportBegin(); it != file->ExportEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (exports.find(&test) == exports.end())
        {
            LinkSymbolData *newSymbol = new LinkSymbolData(file, *it);
            exports.insert(newSymbol);
            SymbolIterator it = publics.find(newSymbol);
            if (it != publics.end())
            {
                (*it)->SetUsed(true);
                newSymbol->SetUsed(true);
            }
        }
    }
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
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
                    SymbolIterator it1 = virtsections.find(&test);
                    if (it1 == virtsections.end())
                    {
                        LinkSymbolData *newSymbol = new LinkSymbolData(file, new ObjSymbol(sym));
                        newSymbol->SetAuxData(*it);
                        virtsections.insert(newSymbol);
                    }
                    else
                    {
                        ObjSection *last = (ObjSection *)(*it1)->GetAuxData();
                        if (last->GetAbsSize() < (*it)->GetAbsSize())
                        {
                            if ((*it1)->GetUsed())
                            {
                                (*it1)->SetRemapped(true);
                            }
                            (*it1)->SetAuxData(*it);
                        }
                    }
                }
            }
        }
    }
    for (SymbolIterator it = externals.begin(); it != externals.end();)
    {
        SymbolIterator it1 = publics.find(*it);
        if (it1 != publics.end())
        {
            (*it)->SetUsed(true);
            (*it1)->SetUsed(true);
            LinkSymbolData *p = *it;
            externals.erase(it++);
            delete p;
        }
        else 
        {
            SymbolIterator its = imports.find(*it);
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
        SymbolIterator it = virtsections.find(&test);
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
            LoadSectionExternals(sym->GetFile(), (ObjSection *)sym->GetAuxData());
            rv = true;
        }
    }
    for (SymbolIterator it = externals.begin(); it !=  externals.end();)
    {
        SymbolIterator it1 = virtsections.find(*it);
        if (it1 != virtsections.end())
        {
            if (!(*it1)->GetUsed())
            {
                (*it1)->SetUsed(true);
                LoadSectionExternals((*it1)->GetFile(), (ObjSection *)(*it1)->GetAuxData());
            }
            (*it)->SetUsed(true);
            LinkSymbolData *p = *it;
            externals.erase(it++);
            delete p;
            rv = true;
        }
        else
        {
            ++it;
        }
    }
    return rv;
}
void LinkManager::LoadFiles()
{
    if (!factory || !indexManager || !ioBase)
    {
        return;
    }
    int bpmau = INT_MAX;
    int mau = 1;
    for (CmdFiles::FileNameIterator it = objectFiles.FileNameBegin(); it != objectFiles.FileNameEnd(); ++it)
    {
        ObjString name = *(*it);
        FILE *infile = fopen(name.c_str(), "rb");
        if (!infile)
        {
            std::string hold = libPath;
            while (hold.size())
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
                    if (npos +1 < hold.size())
                        hold = hold.substr(npos+1);
                    else
                        hold = "";
                }
                name = Utils::FullPath(next, *(*it));
                infile = fopen(name.c_str(), "rb");
                if (infile)
                    hold = "";
            }
        }
        if (infile)
        {
            ObjFile *file = ioBase->Read(infile, ObjIOBase::eAll, factory);
            if (!file)
            {
                LinkError("Invalid object file " + ioBase->GetErrorQualifier() + " in " + **it);
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
            LinkError("Input file '"+name+"' does not exist.");
        }
    }	
    ioBase->SetBitsPerMAU(bpmau);
    ioBase->SetMAUS(mau);
}
LinkLibrary *LinkManager::OpenLibrary(const ObjString &name)
{
    LinkLibrary *rv = new LinkLibrary(name, caseSensitive);
    if (!rv || !rv->IsOpen())
    {
        if (rv)
        {
            delete rv;
            rv = nullptr;
        }
        std::string hold = libPath;
        std::string next;
        while (hold.size())
        {
            size_t npos = hold.find(";");
            if (npos == std::string::npos)
            {
                next = hold;
                hold = "";
            }
            else
            {
                next = hold.substr(0, npos);
                if (npos +1 < hold.size())
                    hold = hold.substr(npos+1);
                else
                    hold = "";
            }
            std::string name1 = Utils::FullPath(next, name);
            FILE *infile = fopen(name1.c_str(), "rb");
            if (infile)
            {
                hold = "";
                fclose(infile);
            }
        }
        rv = new LinkLibrary(Utils::FullPath(next, name), caseSensitive);
    }
    if (rv)
    {
        if (rv->IsOpen())
        {
            rv->Load();
        }
        else
        {
            delete rv;
            rv = nullptr;
        }
    }
    return rv;
}
void LinkManager::LoadLibraries()
{
    for (CmdFiles::FileNameIterator it = libFiles.FileNameBegin(); it != libFiles.FileNameEnd(); ++it)
    {
        LinkLibrary *newLibrary = OpenLibrary((**it));
        if (newLibrary)
        {
            dictionaries.insert(newLibrary);
        }
        else
        {
            LinkWarning("Library '" + (**it) + "' does not exist");
        }
    }
}
bool LinkManager::LoadLibrarySymbol (LinkLibrary *lib, std::string &name)
{
    bool found = false;
    ObjInt objNum = lib->GetSymbol(name);
    if (objNum >= 0 && !lib->HasModule(objNum))
    {
        ObjFile *file = lib->LoadSymbol(objNum, factory);
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
bool LinkManager::ResolveLibrary(LinkLibrary *lib, std::string &name)
{
    if (LoadLibrarySymbol(lib, name))
    {
        bool done = false;
        for (auto sym : externals)
            sym->SetVisited(false);
        while (!done)
        {
            done = true;
            for (auto sym : externals)
            {
                if (!sym->GetVisited())
                {
                    sym->SetVisited(true);
                    if (LoadLibrarySymbol(lib, sym->GetSymbol()->GetName()))
                    {
                        done = false;
                        break;
                    }
                }
            }
        }
        return true;
    }
    return false;
}
void LinkManager::ScanLibraries()
{
    SymbolData dt;
    while (externals.size())
    {
        bool found = false;
        SymbolIterator extit = externals.begin();
        for ( ; extit != externals.end(); ++ extit)
        {
            if (!(*extit)->GetUsed() && virtsections.find(*extit) == virtsections.end())
                break;
        }
        if (extit == externals.end())
            break;
        LinkSymbolData *current = *extit;
        for (auto d : dictionaries)
        {
            found = ResolveLibrary(d, (*extit)->GetSymbol()->GetName());
            if (found)
                break;
        }
        // not resolved?
        if (!found)
        {
            dt.insert(current);
            extit = externals.find(current);
            externals.erase(extit);
        }
    }
    for (auto d : dt)
    {
        externals.insert(d);
    }
}
void LinkManager::CloseLibraries()
{
    for (auto d : dictionaries)
        delete d;
    dictionaries.clear();
}
bool LinkManager::ParseAssignment(LinkTokenizer &spec)
{
    ObjString symName = spec.GetSymbol();
    LinkExpression *value;
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(&value, true))
        return false;
    LinkExpressionSymbol *esym = new LinkExpressionSymbol(symName, value) ;
    if (!LinkExpression::EnterSymbol(esym))
    {
        delete esym;
        LinkManager::LinkError("Symbol " + symName + " redefined");
    }
    else
    {
        LinkPartitionSpecifier *lps = new LinkPartitionSpecifier(esym);
        partitions.push_back(lps);
    }
    return spec.MustMatch(LinkTokenizer::eSemi);
}
bool LinkManager::CreateSeparateRegions(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec)
{
    LinkPartition *newPartition = new LinkPartition(this);
    LinkOverlay *newOverlay = new LinkOverlay(newPartition);
    LinkRegion *newRegion = new LinkRegion(newOverlay);
    if (!newRegion->ParseRegionSpec(manager, files, spec))
        return false;
    if (!spec.MustMatch(LinkTokenizer::eSemi))
        return false;	
    for (LinkRegion::SectionDataIterator itr = newRegion->NowDataBegin();
         itr != newRegion->NowDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkPartition * partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(new LinkPartitionSpecifier(partition));
            LinkOverlay *overlay = new LinkOverlay(partition);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    for (LinkRegion::SectionDataIterator itr = newRegion->NormalDataBegin();
         itr != newRegion->NormalDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkPartition * partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(new LinkPartitionSpecifier(partition));
            LinkOverlay *overlay = new LinkOverlay(partition);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    for (LinkRegion::SectionDataIterator itr = newRegion->PostponeDataBegin();
         itr != newRegion->PostponeDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkPartition * partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(new LinkPartitionSpecifier(partition));
            LinkOverlay *overlay = new LinkOverlay(partition);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    delete newRegion;
    delete newOverlay;
    delete newPartition;
    return true;
}
bool LinkManager::ParsePartitions()
{
    bool done = false;
    int numImports = 0;
    for (auto import : imports)
        if (import->GetUsed())
            numImports++;
    
    LinkExpression *value = new LinkExpression(numImports);
    LinkExpressionSymbol *esym = new LinkExpressionSymbol("IMPORTCOUNT", value) ;
    LinkExpression::EnterSymbol(esym);
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
            LinkPartition *newPartition = new LinkPartition(this);
            partitions.push_back(new LinkPartitionSpecifier(newPartition));
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
    std::map<ObjString, LinkRegion *> createdRegions;
    for (auto file : fileData)
    {
        for (ObjFile::SectionIterator its = file->SectionBegin(); its != file->SectionEnd(); ++its)
        {
            ObjString name = (*its)->GetName();
            auto itr = createdRegions.find(name);
            if (itr == createdRegions.end())
            {
                LinkPartition *partition = new LinkPartition(this);
                partitions.push_back(new LinkPartitionSpecifier(partition));
                LinkOverlay *overlay = new LinkOverlay(partition);
                LinkOverlaySpecifier *ospec = new LinkOverlaySpecifier(overlay);
                partition->Add(ospec);
                partition->SetName(name);
                overlay->SetName(name);
                LinkRegion *region = new LinkRegion(overlay);
                LinkRegionSpecifier *rspec = new LinkRegionSpecifier(region);
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
    try {
        int bottom = 0;
        int overlayNum = 0;
        for (auto partition : partitions)
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
        for (ObjFile::SectionIterator its = file->SectionBegin(); its != file->SectionEnd(); ++its)
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
        for (auto it = externals.begin() ; it != externals.end(); ++it)
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
        bool found = false;
        for (auto sym : imports)
        {
            if (sym->GetSymbol()->GetName() == ext->GetSymbol()->GetName())
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            LinkError("Undefined External " + ext->GetSymbol()->GetDisplayName() +
                  " in module " + ext->GetFile()->GetName());
            rv = true;
        }
    }
    return rv;
}
void LinkManager::CreateOutputFile()
{
    LinkRemapper remapper(*this, *factory, *indexManager, completeLink);
    ObjFile *file = remapper.Remap();
    if (!file)
    {
        LinkError("Internal error");
    }
    else
    {
        FILE *ofile = fopen(outputFile.c_str(), "wb");
        if (ofile != nullptr)
        {
            // copy the definitions into the rel file
            for (LinkExpression::iterator it = LinkExpression::begin(); it != LinkExpression::end(); ++ it)
            {
                ObjDefinitionSymbol *d = factory->MakeDefinitionSymbol((*it)->GetName());
                d->SetValue((*it)->GetValue()->Eval(0));
                file->Add(d);
            }
            if (completeLink)
            {
                ioBase->SetAbsolute(true);
                if (!debugPassThrough)
                {                    
                    ioBase->SetDebugInfoFlag(false);
                    if (debugFile.size())
                    {
                        LinkDebugFile df(debugFile, file, this->virtualSections, this->parentSections);
                        df.CreateOutput();
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
    }
    
}
void LinkManager::Link()
{
    if (!objectFiles.GetSize())
    {
        LinkError("No input files specified");
        return;
    }
    LoadFiles();
    if (completeLink)
    {
        if (externals.size())
        {
            LoadLibraries();
            do
            {
                ScanLibraries();
            } while (ScanVirtuals());
        }
    }
    if (!specName.size())
    {
        CreatePartitions();
    }
    else {
        if (!ParsePartitions())
        {
            std::string err = std::string("Specification file line ") 
                                + Utils::NumberToString(specification.GetLineNo())
                                + specification.GetError();
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
