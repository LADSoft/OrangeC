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

#include <strstream>
#include <fstream>
#include <stdio.h>

int LinkManager::errors;
int LinkManager::warnings;

void HookError(int aa)
{
}
LinkManager::~LinkManager()
{
    for (std::set<LinkLibrary *>::iterator it = dictionaries.begin(); it != dictionaries.end(); ++it)
        delete (*it);
    for (PartitionIterator it = partitions.begin(); it != partitions.end(); ++it)
        delete(*it);
    for (SymbolIterator it = publics.begin(); it != publics.end(); ++it)
        delete(*it);
    for (SymbolIterator it = externals.begin(); it != externals.end(); ++it)
        delete(*it);
    for (SymbolIterator it = imports.begin(); it != imports.end(); ++it)
        delete(*it);
    for (SymbolIterator it = exports.begin(); it != exports.end(); ++it)
        delete(*it);
//	for (FileIterator it = fileData.begin(); it != fileData.end(); ++it)
//		delete(*it);
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
void LinkManager::MergePublics(ObjFile *file, bool toerr)
{
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (publics.find(&test) != publics.end())
        {
            if (toerr)
                LinkError("Duplicate public " + (*it)->GetDisplayName() + " in module " + file->GetName());
        }
        else
        {
            LinkSymbolData *newSymbol = new LinkSymbolData(file, *it);
            publics.insert(newSymbol);
            SymbolIterator its = externals.find(newSymbol);
            if (its != externals.end())
            {
                
                newSymbol->SetUsed(true);
                LinkSymbolData *p = *its;
                externals.erase(its);
                delete p;
            }
            its = exports.find(newSymbol);
            if (its != exports.end())
            {
                (*its)->SetUsed(true);
            }
        }
    }
    for (ObjFile::SymbolIterator it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        LinkSymbolData test(file, *it);
        if (externals.find(&test) == externals.end())
        {
            SymbolIterator it1 = publics.find(&test);
            if (it1 != publics.end())
            {
                (*it1)->SetUsed(true);
            }
            else
            {
                LinkSymbolData *newSymbol = new LinkSymbolData(file, *it);
                SymbolIterator its = imports.find(newSymbol);
                if (its != imports.end())
                {
                    newSymbol->SetUsed(true);
                    (*its)->SetUsed(true);
                }
                externals.insert(newSymbol);
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
            SymbolIterator it = externals.find(newSymbol);
            if (it != externals.end())
            {
                (*it)->SetUsed(true);
                newSymbol->SetUsed(true);
            }
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
            name = Utils::FullPath(libPath, *(*it));
            infile = fopen(name.c_str(), "rb");
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
            rv = NULL;
        }
        rv = new LinkLibrary(Utils::FullPath(libPath, name), caseSensitive);
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
            rv = NULL;
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
void LinkManager::ScanLibraries()
{
    SymbolData dt;
    while (externals.size())
    {
        bool found = false;
        SymbolIterator extit = externals.begin();
        for ( ; extit != externals.end(); ++ extit)
        {
            if (!(*extit)->GetUsed())
                break;
        }
        if (extit == externals.end())
            break;
        LinkSymbolData *current = *extit;
        for (std::set<LinkLibrary *>::iterator it = dictionaries.begin(); it != dictionaries.end(); ++it)
        {
            ObjInt objNum = (*it)->GetSymbol((*extit)->GetSymbol()->GetName());
            if (objNum >= 0)
            {
                ObjFile *file = (*it)->LoadSymbol(objNum, factory);
                if (!file)
                {
                    LinkError("Invalid object file " + ioBase->GetErrorQualifier() + " in library " + (*it)->GetName());
                }
                else
                {
                    fileData.push_back(file);
                    MergePublics(file, false);
                }
                found = true;
                break;
            }
        }
        // not resolved?
        if (!found)
        {
            dt.insert(current);
            externals.erase(extit);
        }
    }
    for (SymbolIterator it = dt.begin(); it != dt.end(); ++it)
    {
        externals.insert(*it);
    }
}
void LinkManager::CloseLibraries()
{
    for (std::set<LinkLibrary *>::iterator it = dictionaries.begin(); it != dictionaries.end(); ++it)
    {
        delete(*it);
    }
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
        for (LinkRegion::OneSectionIterator it = (*itr)->sections.begin();
             it != (*itr)->sections.end(); ++ it)
        {
            LinkPartition * partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(new LinkPartitionSpecifier(partition));
            LinkOverlay *overlay = new LinkOverlay(partition);
            overlay->SetName(std::string((*it).file->GetName()) + "_" + (*it).section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData((*it).file, (*it).section);
        }
    }
    for (LinkRegion::SectionDataIterator itr = newRegion->NormalDataBegin();
         itr != newRegion->NormalDataEnd(); ++itr)
    {
        for (LinkRegion::OneSectionIterator it = (*itr)->sections.begin();
             it != (*itr)->sections.end(); ++ it)
        {
            LinkPartition * partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(new LinkPartitionSpecifier(partition));
            LinkOverlay *overlay = new LinkOverlay(partition);
            overlay->SetName(std::string((*it).file->GetName()) + "_" + (*it).section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData((*it).file, (*it).section);
        }
    }
    for (LinkRegion::SectionDataIterator itr = newRegion->PostponeDataBegin();
         itr != newRegion->PostponeDataEnd(); ++itr)
    {
        for (LinkRegion::OneSectionIterator it = (*itr)->sections.begin();
             it != (*itr)->sections.end(); ++ it)
        {
            LinkPartition * partition = new LinkPartition(this);
            partition->SetName("replicate");
            partitions.push_back(new LinkPartitionSpecifier(partition));
            LinkOverlay *overlay = new LinkOverlay(partition);
            overlay->SetName(std::string((*it).file->GetName()) + "_" + (*it).section->GetName());
            partition->Add(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData((*it).file, (*it).section);
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
    for (SymbolIterator it = imports.begin(); it != imports.end(); ++it)
        if ((*it)->GetUsed())
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
    for (FileIterator it = fileData.begin(); it != fileData.end(); ++it)
    {
        for (ObjFile::SectionIterator its = (*it)->SectionBegin(); its != (*it)->SectionEnd(); ++its)
        {
            ObjString name = (*its)->GetName();
            std::map<ObjString, LinkRegion *>::iterator itr = createdRegions.find(name);
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
                region->AddNormalData((*it), (*its));
                createdRegions[name] = region;
            }
            else
            {
                itr->second->AddNormalData((*it), (*its));
            }
        }
    }
}
void LinkManager::PlaceSections()
{
    try {
        int bottom = 0;
        int overlayNum = 0;
        for (PartitionIterator it = partitions.begin(); it != partitions.end(); ++it)
        {
            if ((*it)->GetSymbol())
            {
                if (completeLink)
                    (*it)->GetSymbol()->GetValue()->Eval(bottom);
            }
            else
            {
                bottom = (*it)->GetPartition()->PlacePartition(bottom, completeLink, overlayNum);
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
    for (FileIterator it = fileData.begin(); it != fileData.end(); ++it)
        for (ObjFile::SectionIterator its = (*it)->SectionBegin(); its != (*it)->SectionEnd(); ++its)
            if (!(*its)->GetUtilityFlag())
                LinkError("Section " + (*its)->GetName() + " unused in module " + (*it)->GetName());
            
}
bool LinkManager::ExternalErrors()
{
    bool rv = false;
    bool done = false;
    while (!done)
    {
        done = true;
        for (SymbolIterator it = externals.begin(); it != externals.end(); it++)
        {
            if (LinkExpression::FindSymbol((*it)->GetSymbol()->GetName()))
            {
                externals.erase(it);
                done = false;
                break;
            }
        }
    }
    for (SymbolIterator pt = externals.begin(); pt != externals.end(); pt++)
    {
        bool found = false;
        for (SymbolIterator it = imports.begin(); it != imports.end(); it++)
        {
            if ((*it)->GetSymbol()->GetName() == (*pt)->GetSymbol()->GetName())
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            LinkError("Undefined External " + (*pt)->GetSymbol()->GetDisplayName() +
                  " in module " + (*pt)->GetFile()->GetName());
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
        if (ofile != NULL)
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
                        LinkDebugFile df(debugFile, file);
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
            ScanLibraries();
        }
    }
    if (!specName.size())
    {
        CreatePartitions();
    }
    else {
        if (!ParsePartitions())
        {
            std::strstream stream;
            stream << "Specification file line " << specification.GetLineNo() << " : "
                    << specification.GetError();
            std::string err(stream.str());
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
