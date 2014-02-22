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
#include "Linkmanager.h"
#include "LinkRemapper.h"
#include "LinkPartition.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "ObjFactory.h"
#include "ObjIeee.h"
#include "LinkRegionFileSpec.h"
#include <time.h>
#include <stdio.h>
unsigned LinkRemapper::crc_table[256] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d, 
} ;
unsigned LinkRemapper::crc32(unsigned char *buf, int len, unsigned crc) 
{
    int i;
    for (i = 0; i < len; i++) 
    {
        crc = crc_table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}
ObjInt LinkRemapper::RenumberSection(LinkRegion *region, ObjSection *dest, 
                                   LinkRegion::OneSection *source, int group, int base)
{
    ObjInt rv = region->GetAttribs().GetSize();
    ObjSection *sect = source->section;
    dest->SetQuals(dest->GetQuals() | sect->GetQuals());
    sect->SetIndex(group);
    sect->SetOffset(new ObjExpression(region->GetParent()->GetAttribs().GetAddress()));
    ObjMemoryManager &memManager = sect->GetMemoryManager();
    LinkSymbolData d;
    d.SetFile(source->file);
    // now copy all the data from the old section to the new one
    if (!(dest->GetQuals() & ObjSection::common) || dest->GetMemoryManager().MemoryBegin() == dest->GetMemoryManager().MemoryEnd())
    {
        int n = sect->GetBase() - base;
        if (n > 0)
        {
            // padding between sections, e.g. for alignment and roundsize 
            ObjMemory *newMem = new ObjMemory(n, region->GetAttribs().GetFill());
            dest->Add(newMem);
        }
        else
        {
            n = 0;
        }
        source->section->SetOffset(new ObjExpression(base + n));
        for (ObjMemoryManager::MemoryIterator it = memManager.MemoryBegin();
             it != memManager.MemoryEnd(); ++it)
        {
            n += (*it)->GetSize();
            // redo source file indexes
            dest->Add(*it);
            if ((*it)->GetFixup())
            {
                (*it)->SetFixup(ScanExpression((*it)->GetFixup(), &d));
            }
        }
        rv = n ;
    }
    return rv;
}
bool LinkRemapper::OptimizeRight(ObjExpression *exp, ObjExpression *right)
{
    bool rv = false;
    switch (right->GetOperator())
    {
        case ObjExpression::eSub:
            rv = OptimizeRight(exp, right->GetLeft());
            break;
        case ObjExpression::eAdd:
            rv = OptimizeRight(exp, right->GetLeft());
            if (!rv)
                rv = OptimizeRight(exp, right->GetRight());
            break;
        case ObjExpression::eSection:
            if (exp->GetSection() == right->GetSection())
            {
                rv = true;
                delete right;
                right = new ObjExpression(0);
            }
            break;
    }
    return rv;
}
ObjExpression *LinkRemapper::Optimize(ObjExpression *exp, ObjExpression *right)
{
    switch (exp->GetOperator())
    {
        case ObjExpression::eSection:
        {
            if (right && OptimizeRight(exp, right))
            {
                delete exp;
                exp = new ObjExpression(0);
            }
            break;
        }
        case ObjExpression::eSub:
        {
            if (right)
                exp->SetLeft(Optimize(exp->GetLeft(), right));
            exp->SetLeft(Optimize(exp->GetLeft(), exp->GetRight()));
            exp->SetRight(Optimize(exp->GetRight(), NULL));
            break;
        }
        default:
            if (exp->GetLeft())
                exp->SetLeft(Optimize(exp->GetLeft(), right));
            if (exp->GetRight())
                exp->SetRight(Optimize(exp->GetRight(), right));
            break;
    }
    return exp;
}
ObjExpression *LinkRemapper::RewriteExpression(LinkExpression *exp, LinkExpressionSymbol *sym)
{
    ObjExpression *rv = NULL;
    switch (exp->GetOperator())
    {
        case LinkExpression::eValue:
            rv = new ObjExpression(exp->GetValue());
            break;
        case LinkExpression::ePC:
            break;
        case LinkExpression::eSymbol:
        {
            LinkExpressionSymbol *sym = LinkExpression::FindSymbol(exp->GetSymbol());
            if (sym)
            {
                rv = RewriteExpression(sym->GetValue(), sym);
            }
            break;
        }
        case LinkExpression::eSection:
        {
            int n = exp->GetSection();
            if (n < sections.size())
            {
                rv = new ObjExpression(sections[n]);				
            }
            break;
        }
        case LinkExpression::eAdd:
            rv = new ObjExpression(ObjExpression::eAdd, RewriteExpression(exp->GetLeft(), sym), RewriteExpression(exp->GetRight(), sym));
            break;
        case LinkExpression::eSub:
            rv = new ObjExpression(ObjExpression::eSub, RewriteExpression(exp->GetLeft(), sym), RewriteExpression(exp->GetRight(), sym));
            break;
        case LinkExpression::eMul:
            rv = new ObjExpression(ObjExpression::eMul, RewriteExpression(exp->GetLeft(), sym), RewriteExpression(exp->GetRight(), sym));
            break;
        case LinkExpression::eDiv:
            rv = new ObjExpression(ObjExpression::eDiv, RewriteExpression(exp->GetLeft(), sym), RewriteExpression(exp->GetRight(), sym));
            break;
        case LinkExpression::eNeg:
            rv = new ObjExpression(ObjExpression::eNeg, RewriteExpression(exp->GetLeft(), sym), RewriteExpression(exp->GetRight(), sym));
            break;
        case LinkExpression::eCpl:
            rv = new ObjExpression(ObjExpression::eCmpl, RewriteExpression(exp->GetLeft(), sym), RewriteExpression(exp->GetRight(), sym));
            break;
    }
    if (!rv)
    {
        LinkManager::LinkError(std::string("Evaluation of symbol '") + sym->GetName() + "' too complex");
        rv = new ObjExpression(0);
    }
    return rv;
}
ObjExpression *LinkRemapper::ScanExpression(ObjExpression *offset, LinkSymbolData *d)
{
    switch(offset->GetOperator())
    {
        default:
        case ObjExpression::eSymbol:
            if (d && offset->GetSymbol()->GetType() == ObjSymbol::eExternal)
            {
                d->SetSymbol(offset->GetSymbol());
                LinkManager::SymbolIterator it = manager->PublicFind(d);
                if (it != manager->PublicEnd())
                {
                    return (*it)->GetSymbol()->GetOffset();
                }
                LinkExpressionSymbol *sym = LinkExpression::FindSymbol(offset->GetSymbol()->GetName());
                if (sym)
                {
                    return Optimize(RewriteExpression(sym->GetValue(), sym), NULL);
                }
            }
            return offset;
        case ObjExpression::ePC:
        case ObjExpression::eValue:
        case ObjExpression::eNop:
            return offset;
        case ObjExpression::eSection:
        {
            int n = offset->GetSection()->GetVirtualOffset();
            if (n != -1)
            {
                return new ObjExpression(n);
            }
            else
            {
                n = offset->GetSection()->GetBase();
                if (n != 0)
                {
                    return new ObjExpression(ObjExpression::eAdd, offset, new ObjExpression(n));
                }
            }
            return offset;
        }
        case ObjExpression::eExpression:
        case ObjExpression::eNonExpression:
        case ObjExpression::eAdd:
        case ObjExpression::eSub:
        case ObjExpression::eMul:
        case ObjExpression::eDiv:
            offset->SetRight(ScanExpression(offset->GetRight(),d));
            // fallthrough
        case ObjExpression::eNeg:
        case ObjExpression::eCmpl:
            offset->SetLeft(ScanExpression(offset->GetLeft(),d));
            return offset;
    }	
}
void LinkRemapper::RenumberSymbol(ObjFile *file, ObjSymbol *sym, int index)
{
    if (sym->GetOffset())
        sym->SetOffset(ScanExpression(sym->GetOffset(), NULL));
    sym->SetIndex(index);
    file->Add(sym);
}
ObjInt LinkRemapper::GetTypeIndex(ObjType *type)
{
    if (type->GetType() < ObjType::eVoid)
        return LookupFileType(type->GetIndex());
    else
        return LookupFileType((int)type->GetType());
}
void LinkRemapper::SetTypeIndex(ObjType *type)
{
    if (type->GetType() < ObjType::eVoid)
        type->SetIndex(LookupFileType(type->GetIndex()));
    else
        type->SetType((ObjType::eType)LookupFileType((int)type->GetType()));
}
ObjInt LinkRemapper::RegisterType(ObjFile *file, ObjType *type, char *name)
{
    int n;
    std::map<ObjString, int>::iterator sny = newTypes.find(name);
    if (sny != newTypes.end())
    {
        n = sny->second; // already exists in another module
    }
    else
    {
        // this is the first time we encountered it, register it as a global type
        file->Add(type);
        n = indexManager->NextType();
        newTypes[name] = n;
    }
    fileTypes[type->GetIndex()] = n;
      type->SetIndex(n);
    return n;
}
ObjInt LinkRemapper::MapType(ObjFile *file, ObjType *type)
{
    char name[256];
    // we don't already have a mapping for this type, now see if the type
    // was declared in some other module
    switch(type->GetType())
    {
        case ObjType::ePointer:
            sprintf(name, "*%d;%d", type->GetSize(), GetTypeIndex(type->GetBaseType()));
            break;
        case ObjType::eFunction:
        {
            ObjFunction *func = static_cast<ObjFunction *>(type);
            sprintf(name, "$%d;%d", func->GetLinkage(), GetTypeIndex(func->GetBaseType()));
        }
            break;
        case ObjType::eBitField:
            sprintf(name, "B%d;%d;%d;%d", type->GetSize(), GetTypeIndex(type->GetBaseType()),
                    type->GetStartBit(), type->GetBitCount());
            break;
        case ObjType::eStruct:
        case ObjType::eUnion:
        case ObjType::eEnum:
        {
            
            // we are crcing the fields to allow for the possibility they use the same
            // structure name different ways in differnet files..
            unsigned crc = 0xffffffff;
            {
                for (ObjType::FieldIterator it = type->FieldBegin(); it != type->FieldEnd(); ++it)
                {
                    sprintf(name, "%s;%d;", (*it)->GetName().c_str(), (*it)->GetConstVal());
                    crc = crc32((unsigned char *)name, strlen(name), crc);
                }
            }
            crc ^= 0xffffffff;
            sprintf(name, "S%d;%s;%X", type->GetSize(), type->GetName().c_str(), crc);
            break;
        }
        case ObjType::eArray:
        case ObjType::eVla:
            sprintf(name, "A%d;%d;%d;%d", type->GetSize(),
                    type->GetBaseType() ? GetTypeIndex(type->GetBaseType()) : 0, GetTypeIndex(type->GetIndexType()),
                    type->GetBase(), type->GetTop()); //FIXME
            break;
        case ObjType::eTypeDef:
            sprintf(name, "T%d;%d;%s", type->GetSize(),
                    GetTypeIndex(type->GetBaseType()),type->GetName().c_str());
            break;
    }
    RegisterType(file, type, name);
    return 0;
}
ObjInt LinkRemapper::LookupFileType(ObjInt type)
{
    // see if it is a builtin...
    if (type <= ObjType::eReservedTop)
        return type;
    // see if we already have a mapping for this type
    std::map<ObjInt, ObjInt>::iterator nty = fileTypes.find(type);
    if (nty != fileTypes.end())
        return nty->second;
    else
        return 0;
}
void LinkRemapper::RenumberType(ObjFile *file, ObjType *type)
{
//	type->SetIndex(indexManager->NextType());
//	file->Add(type);
//   return;
    int n = LookupFileType(type->GetIndex());
    if (!n)
    {
        MapType(file, type);
    }
    else
    {
          type->SetIndex(n);
    }
}
void LinkRemapper::RenumberSourceFile(ObjFile *file, ObjSourceFile *sourceFile)
{
    // redo source file indexes for duplicate sources
    sourceFile->SetIndex(indexManager->NextFile());
    file->Add(sourceFile);
}
ObjFile *LinkRemapper::Remap()
{
    ObjFile *file = new ObjFile(manager->GetOutputFile());
    time_t t = time(0);
    tm *tmx = localtime(&t);
    if (tmx)
        file->SetFileTime(*tmx);
    int group = 0;
    for (LinkManager::PartitionIterator it = manager->PartitionBegin();
         it != manager->PartitionEnd(); ++it)
    {
        if ((*it)->GetPartition())
        {
            for (LinkPartition::OverlayIterator ito = (*it)->GetPartition()->OverlayBegin();
                 ito != (*it)->GetPartition()->OverlayEnd(); ++ito)
            {
                if ((*ito)->GetOverlay())
                {
                    ObjString name = (*ito)->GetOverlay()->GetName();
                    ObjSection *section = new ObjSection(name, group);
                    sections.push_back(section);
                    ++group;
                }
            }
        }
    }
    std::map<std::string, int> externs;
    for (LinkManager::FileIterator it = manager->FileBegin(); it != manager->FileEnd(); ++it)
    {
        if ((*it)->GetBigEndian())
            file->SetBigEndian(true);
        for (ObjFile::SymbolIterator its = (*it)->PublicBegin(); its != (*it)->PublicEnd(); ++its)
        {
            RenumberSymbol(file, *its, indexManager->NextPublic());
        }
        for (ObjFile::SymbolIterator its = (*it)->ExternalBegin(); its != (*it)->ExternalEnd(); ++its)
        {
            std::string name = (*its)->GetName();
            if (manager->IsExternal(name))
            {
                std::map<std::string, int>::iterator itf = externs.find(name);
                
                int n;
                if (itf == externs.end())
                {
                    n = indexManager->NextExternal();
                    externs[name] = n;
                    RenumberSymbol(file, *its, n);
                }
                else
                {
                    if ((*its)->GetOffset())
                        (*its)->SetOffset(ScanExpression((*its)->GetOffset(), NULL));
                    (*its)->SetIndex(itf->second);
                }
            }
        }
        for (ObjFile::SymbolIterator its = (*it)->LocalBegin(); its != (*it)->LocalEnd(); ++its)
        {
            RenumberSymbol(file, *its, indexManager->NextLocal());
        }
        for (ObjFile::SymbolIterator its = (*it)->AutoBegin(); its != (*it)->AutoEnd(); ++its)
        {
            RenumberSymbol(file, *its, indexManager->NextAuto());
        }
        for (ObjFile::SymbolIterator its = (*it)->RegBegin(); its != (*it)->RegEnd(); ++its)
        {
            RenumberSymbol(file, *its, indexManager->NextReg());
        }
        for (ObjFile::SymbolIterator its = (*it)->ExportBegin(); its != (*it)->ExportEnd(); ++its)
        {
            RenumberSymbol(file, *its, 0);
        }
        for (ObjFile::TypeIterator its = (*it)->TypeBegin(); its != (*it)->TypeEnd(); ++its)
        {
            RenumberType(file, *its);
        }
        fileTypes.clear();
        for (ObjFile::SourceFileIterator its = (*it)->SourceFileBegin(); its != (*it)->SourceFileEnd(); ++its)
        {
            RenumberSourceFile(file, *its);
        }
    }
    for (LinkManager::SymbolIterator its = manager->ImportBegin(); its != manager->ImportEnd(); ++its)
    {
        ObjSymbol *sym = (*its)->GetSymbol();
        RenumberSymbol(file, sym, 0);
    }
    group = 0;
    for (LinkManager::PartitionIterator it = manager->PartitionBegin();
         it != manager->PartitionEnd(); ++it)
    {
        if ((*it)->GetPartition())
        {
            for (LinkPartition::OverlayIterator ito = (*it)->GetPartition()->OverlayBegin();
                 ito != (*it)->GetPartition()->OverlayEnd(); ++ito)
            {
                if ((*ito)->GetOverlay())
                {
                    ObjSection *section = sections[group];
                    ObjInt base = 0;
                    section->SetQuals(0);
                    section->SetAlignment((*ito)->GetOverlay()->GetAttribs().GetAlign());
                    if ((*ito)->GetOverlay()->GetAttribs().GetSize() > 0)
                        section->SetSize(new ObjExpression((*ito)->GetOverlay()->GetAttribs().GetSize()));
                    for (LinkOverlay::RegionIterator itr = (*ito)->GetOverlay()->RegionBegin();
                         itr != (*ito)->GetOverlay()->RegionEnd(); ++ itr)
                    {
                        if ((*itr)->GetRegion())
                        {
                            ObjInt bytes = 0;
                            for (LinkRegion::SectionDataIterator its = (*itr)->GetRegion()->NowDataBegin();
                                 its != (*itr)->GetRegion()->NowDataEnd(); ++its)
                                for (LinkRegion::OneSectionIterator itt = (*its)->sections.begin();
                                     itt != (*its)->sections.end(); ++ itt)
                                {
                                    bytes += RenumberSection((*itr)->GetRegion(), section, &(*itt), group, bytes + base);
                                }
                            for (LinkRegion::SectionDataIterator its = (*itr)->GetRegion()->NormalDataBegin();
                                 its != (*itr)->GetRegion()->NormalDataEnd(); ++its)
                                for (LinkRegion::OneSectionIterator itt = (*its)->sections.begin();
                                     itt != (*its)->sections.end(); ++ itt)
                                {
                                    bytes += RenumberSection((*itr)->GetRegion(), section, &(*itt), group, bytes + base);
                                }
                            for (LinkRegion::SectionDataIterator its = (*itr)->GetRegion()->PostponeDataBegin();
                                 its != (*itr)->GetRegion()->PostponeDataEnd(); ++its)
                                for (LinkRegion::OneSectionIterator itt = (*its)->sections.begin();
                                     itt != (*its)->sections.end(); ++ itt)
                                {
                                    bytes += RenumberSection((*itr)->GetRegion(), section, &(*itt), group, bytes + base);
                                }
                                /*
                            int n= (*itr)->GetRegion()->GetAttribs().GetSize() - bytes;
                            if (n > 0)
                            {
                                // padding between sections, e.g. for alignment and roundsize 
                                ObjMemory *newMem = new ObjMemory(n, (*itr)->GetRegion()->GetAttribs().GetFill());
                                section->Add(newMem);
                            }
                            */
                            base += bytes;
                        }
                    }
                    if (section->GetSize())
                    {
                        int n = section->GetSize()->Eval(0) - section->GetMemoryManager().GetSize();
                        if (n > 0)
                        {
                            // padding between sections, e.g. for alignment and roundsize 
                            ObjMemory *newMem = new ObjMemory(n, (*ito)->GetOverlay()->GetAttribs().GetFill());
                            section->Add(newMem);
                        }
                    }
                    if (completeLink)
                    {
                        section->SetQuals(section->GetQuals() | ObjSection::absolute);
                        section->SetBase((*ito)->GetOverlay()->GetAttribs().GetAddress());
                    }
                    section->SetOffset(new ObjExpression((*ito)->GetOverlay()->GetAttribs().GetAddress()));
                    file->Add(section);
                    ++group;
                }
            }
        }
    }
    ObjExpression *sa = manager->GetObjIo()->GetStartAddress();
    if (sa)
    {
        manager->GetObjIo()->SetStartAddress(file, ScanExpression(sa, NULL));
    }

    return file;
}