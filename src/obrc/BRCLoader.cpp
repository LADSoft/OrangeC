/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#include "BRCDictionary.h"
#include "ObjBrowseInfo.h"
#include "Utils.h"
#include "ObjLineNo.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "ObjSourceFile.h"
#include <cstdio>
#include <iostream>

SymData::~SymData()
{
    for (auto l : data)
        delete l;
}
BlockData::~BlockData() {}
BRCLoader::~BRCLoader()
{
    for (auto sym : syms)
        delete sym.second;
}
void BRCLoader::InsertSymData(std::string s, BrowseData* ldata, bool func)
{
    SymData* sym;
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
    for (auto compare : sym->data)
    {
        if (ldata->type == compare->type)
            if (ldata->startLine == compare->startLine)
                if (ldata->qual == compare->qual)
                    //                    if (ldata->charPos == compare->charPos)
                    //                        if (ldata->hint == compare->hint)
                    if (ldata->blockLevel == compare->blockLevel)
                        if (ldata->fileIndex == compare->fileIndex)
                        {
                            delete ldata;
                            return;
                        }
    }
    sym->insert(ldata);
    if (func)
        sym->func = ldata;
}

//-------------------------------------------------------------------------

void BRCLoader::InsertBlockData(std::string s, BrowseData* ldata) { blocks[blocks.size() - 1]->syms[s] = ldata; }

//-------------------------------------------------------------------------

void BRCLoader::InsertFile(ObjBrowseInfo& p)
{
    int index = p.GetQual();
    index = indexMap[index];
    currentFile = index;
}

//-------------------------------------------------------------------------

ObjInt BRCLoader::InsertVariable(ObjBrowseInfo& p, bool func)
{
    BrowseData* ldata = new BrowseData;
    std::string hint;
    std::string name = p.GetData();
    ldata->type = p.GetType();
    ldata->startLine = p.GetLine()->GetLineNumber();
    ldata->qual = p.GetQual();
    ldata->charPos = p.GetCharPos();
    ldata->hint = hint;
    ldata->blockLevel = blocks.size() - blockHead;
    ldata->fileIndex = indexMap[p.GetLineNo()->GetFile()->GetIndex()];
    // functions can be nested in C++ browse info..
    if (blocks.size() - blockHead == 0)
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

void BRCLoader::InsertDefine(ObjBrowseInfo& p) { InsertVariable(p); }
void BRCLoader::Usages(ObjBrowseInfo& p)
{
    BrowseData* ldata = new BrowseData;
    std::string hint;
    std::string name = p.GetData();
    ldata->type = p.GetType();
    ldata->startLine = p.GetLine()->GetLineNumber();
    ldata->qual = p.GetQual();
    ldata->charPos = p.GetCharPos();
    ldata->hint = hint;
    ldata->blockLevel = blocks.size() - blockHead;
    ldata->fileIndex = indexMap[p.GetLineNo()->GetFile()->GetIndex()];
    SymData* sym;
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
void BRCLoader::StartFunc(ObjBrowseInfo& p)
{
    functionNesting.push_front(blockHead);
    blockHead = blocks.size();
    int line = InsertVariable(p, true);
    StartBlock(line);
}

//-------------------------------------------------------------------------

void BRCLoader::EndFunc(ObjBrowseInfo& p)
{
    std::string name = p.GetData();
    int line = p.GetLine()->GetLineNumber();
    SymData* sym = syms[name];
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
    BlockData* b = new BlockData;
    b->count = 0;
    b->start = line;
    blocks.push_back(b);
}

//-------------------------------------------------------------------------

void BRCLoader::EndBlock(int line)
{
    if (!blocks.empty())
    {
        BlockData* b = blocks.back();
        blocks.pop_back();

        b->end = line;
        for (auto sym : b->syms)
        {
            InsertSymData(sym.first, sym.second);
        }
        delete b;
    }
}
void BRCLoader::ParseData(ObjFile& f)
{
    blocks.clear();
    for (ObjFile::BrowseInfoIterator it = f.BrowseInfoBegin(); it != f.BrowseInfoEnd(); ++it)
    {
        ObjBrowseInfo* p = *it;
        switch (p->GetType())
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
void BRCLoader::LoadSourceFiles(ObjFile& fil)
{
    indexMap.clear();
    for (ObjFile::SourceFileIterator it = fil.SourceFileBegin(); it != fil.SourceFileEnd(); ++it)
    {
        int index;
        auto it1 = nameMap.find((*it)->GetName());
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
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        std::string name = **it;
        ObjIeeeIndexManager im1;
        ObjFactory fact1(&im1);
        FILE* b = fopen(name.c_str(), "rb");
        //        std::fstream b(name.c_str(), std::fstream::in);
        if (b != nullptr)
        {
            ObjIeee i(name);
            ObjFile* fil = i.Read(b, ObjIeee::eAll, &fact1);
            fclose(b);
            //            b.close();
            if (fil)
            {
                LoadSourceFiles(*fil);
                ParseData(*fil);
            }
            else
            {
                std::string temp = std::string("Invalid browse information file ") + name.c_str();
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
