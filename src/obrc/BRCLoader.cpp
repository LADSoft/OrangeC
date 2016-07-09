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


#include "BRCDictionary.h"
#include "ObjBrowseInfo.h"
#include "Utils.h"
#include "ObjLineNo.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "ObjSourceFile.h"
#include <stdio.h>
#include <iostream>

SymData::~SymData()
{
    for (BrowseDataset::iterator it = data.begin(); it != data.end(); it++)
    {
        BrowseData *q = *it;
        delete q;
    }
}
BlockData::~BlockData()
{
}
BRCLoader::~BRCLoader()
{
    for (Symbols::iterator it = syms.begin(); it != syms.end(); ++it)
    {
        SymData *s = it->second;
        delete s;
    }
}
void BRCLoader::InsertSymData(std::string s, BrowseData *ldata, bool func)
{
    SymData *sym;
    Symbols::iterator it = syms.find(s);
    if (it != syms.end())
    {
        sym = it->second;
    }
    else
    {
        sym = new SymData(s);
        syms[s] = sym;
    }
    sym->insert(ldata);
    if (func)
        sym->func = ldata;
}

//-------------------------------------------------------------------------

void BRCLoader::InsertBlockData(std::string s, BrowseData *ldata)
{
    blocks[blocks.size()-1]->syms[s] = ldata;
}

//-------------------------------------------------------------------------

void BRCLoader::InsertFile(ObjBrowseInfo &p)
{
    int index = p.GetQual();
    index = indexMap[index];
    currentFile = index;
}

//-------------------------------------------------------------------------

ObjInt BRCLoader::InsertVariable(ObjBrowseInfo &p, bool func)
{
    BrowseData *ldata = new BrowseData;
    std::string hint;
    std::string name = p.GetData();
    ldata->type =  p.GetType();
    ldata->startLine = p.GetLine()->GetLineNumber();
    ldata->qual = p.GetQual();
    ldata->charPos = p.GetCharPos();
    ldata->hint = hint;
    ldata->blockLevel = blocks.size()- blockHead;
    ldata->fileIndex = indexMap[p.GetLineNo()->GetFile()->GetIndex()];
    // functions can be nested in C++ browse info..
    if (blocks.size()-blockHead == 0)
    {
        InsertSymData(name, ldata, func);
    } 
    else
    {
        InsertBlockData(name, ldata);
    }
    return ldata->line;
}

//-------------------------------------------------------------------------

void BRCLoader::InsertDefine(ObjBrowseInfo &p)
{
    InsertVariable(p);
}
void BRCLoader::Usages(ObjBrowseInfo &p)
{
    BrowseData *ldata = new BrowseData;
    std::string hint;
    std::string name = p.GetData();
    ldata->type =  p.GetType();
    ldata->startLine = p.GetLine()->GetLineNumber();
    ldata->qual = p.GetQual();
    ldata->charPos = p.GetCharPos();
    ldata->hint = hint;
    ldata->blockLevel = blocks.size()- blockHead;
    ldata->fileIndex = indexMap[p.GetLineNo()->GetFile()->GetIndex()];
    SymData *sym;
    Symbols::iterator it = syms.find(name);
    if (it != syms.end())
    {
        sym = it->second;
    }
    else
    {
        sym = new SymData(name);
        syms[name] = sym;
    }
    sym->usages.push_back(ldata);
}

//-------------------------------------------------------------------------
void BRCLoader::StartFunc(ObjBrowseInfo &p)
{
    functionNesting.push_front(blockHead);
    blockHead = blocks.size();
    int line = InsertVariable(p, true);
    StartBlock(line);
}

//-------------------------------------------------------------------------

void BRCLoader::EndFunc(ObjBrowseInfo &p)
{
    std::string name = p.GetData();
    int line = p.GetLine()->GetLineNumber();
    SymData * sym = syms[name];
    if (!sym)
       Utils::fatal("EndFunc::Cannot find symbol");
//    (*sym->data.begin())->funcEndLine = line;
    sym->func->funcEndLine = line;
    EndBlock(line);
    blockHead = functionNesting.front();
    functionNesting.pop_front();
}

//-------------------------------------------------------------------------

void BRCLoader::StartBlock(int line)
{
    BlockData *b = new BlockData;
    b->count = 0;
    b->start = line;
    blocks.push_back(b);
}

//-------------------------------------------------------------------------

void BRCLoader::EndBlock(int line)
{
    if (blocks.size())
    {
        BlockData *b = blocks.back();
        blocks.pop_back();

        b->end = line;
        int i;
        for (std::map<std::string, BrowseData *>::iterator it = b->syms.begin();
             it != b->syms.end(); ++it)
        {
            InsertSymData(it->first, it->second);
        }
        delete b;
    }
}
void BRCLoader::ParseData(ObjFile &f)
{
    unsigned char *p;
    blocks.clear();
    for (ObjFile::BrowseInfoIterator it = f.BrowseInfoBegin(); it != f.BrowseInfoEnd(); ++it)
    {
        ObjBrowseInfo *p = *it;
        switch(p->GetType())
        {
            case ObjBrowseInfo::eFileStart:
                InsertFile(*p);
                break;
            case ObjBrowseInfo::eVariable:
                InsertVariable(*p);
                break;
            case ObjBrowseInfo::eDefine:
                InsertDefine(*p);
                break;
            case ObjBrowseInfo::eFuncStart:
                StartFunc(*p);
                break;
            case ObjBrowseInfo::eFuncEnd:
                EndFunc(*p);
                break;
            case ObjBrowseInfo::eBlockStart:
                StartBlock(p->GetLine()->GetLineNumber());
                break;
            case ObjBrowseInfo::eBlockEnd:
                EndBlock(p->GetLine()->GetLineNumber());
                break;
            case ObjBrowseInfo::eTypePrototype:
                InsertVariable(*p);
                break;
            case ObjBrowseInfo::eUsage:
                Usages(*p);
                break;
            default:
                Utils::fatal("Unknown browse info record");
                break;
        }
    }
}
void BRCLoader::LoadSourceFiles(ObjFile &fil)
{
    indexMap.clear();
    for (ObjFile::SourceFileIterator it = fil.SourceFileBegin(); it != fil.SourceFileEnd(); ++it)
    {
        int index;
        std::map<std::string, int>::iterator it1 = nameMap.find((*it)->GetName());
        if (it1 == nameMap.end())
        {
            index = nameMap.size();
            nameMap[(*it)->GetName()] = index;
        }
        else
        {
            index = it1->second;
        }
        if ((*it)->GetName() == "stdlib.h")
            printf("stdlib:%d:%d\n", (*it)->GetIndex(), index);
        indexMap[(*it)->GetIndex()] = index;
    }
}
bool BRCLoader::load()
{
    bool rv = true;
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files. FileNameEnd(); ++it)
    {
        std::string name = **it;
        ObjIeeeIndexManager im1;
        ObjFactory fact1(&im1);
        FILE *b = fopen(name.c_str(), "rb");
//        std::fstream b(name.c_str(), std::fstream::in);
        if (b != NULL)
        {
            ObjIeee i(name);
            ObjFile *fil = i.Read(b, ObjIeee::eAll, &fact1);
            fclose(b);
//            b.close();
            if (fil)
            {
                LoadSourceFiles(*fil);
                ParseData(*fil);
            }
            else
            {
                std::string temp = std::string("Invalid object file ") + name.c_str(); 
                std::cout << temp.c_str() << std::endl;
            }
        }
        else
        {
            std::string temp = std::string("Could not open file ") + name.c_str();
            std::cout << temp.c_str() << std::endl;
            rv = false;
        }
    }
    return rv;
}
