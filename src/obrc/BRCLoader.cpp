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

SymData::SymData(std::string& Name) :
    name(Name), externalCount(0), func(nullptr), globalCount(0), argCount(0), localCount(0), fileOffs(0), index(0)
{
}

SymData::~SymData() {}
BlockData::~BlockData() {}
BRCLoader::~BRCLoader() {}
void BRCLoader::InsertSymData(std::string s, BrowseData* ldata, bool func)
{
    SymData* sym;
    auto it = syms.find(s);
    if (it != syms.end())
    {
        sym = it->second.get();
    }
    else
    {
        syms[s] = std::make_unique<SymData>(s);
        sym = syms[s].get();
    }
    for (auto& compare : sym->data)
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
    std::unique_ptr<BrowseData> temp;
    temp.reset(ldata);
    sym->insert(std::move(temp));
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
    return p.GetLine()->GetLineNumber();
}

//-------------------------------------------------------------------------

void BRCLoader::InsertDefine(ObjBrowseInfo& p) { InsertVariable(p); }
void BRCLoader::Usages(ObjBrowseInfo& p)
{
    std::unique_ptr<BrowseData> ptr = std::make_unique<BrowseData>();
    BrowseData* ldata = ptr.get();
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
    auto it = syms.find(name);
    if (it != syms.end())
    {
        sym = it->second.get();
    }
    else
    {
        syms[name] = std::make_unique<SymData>(name);
        sym = syms[name].get();
    }
    sym->usages.push_back(std::move(ptr));
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
    SymData* sym = syms[name].get();
    if (!sym)
        Utils::Fatal("EndFunc::Cannot find symbol");
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
        std::unique_ptr<BlockData> b(blocks.back());
        blocks.pop_back();

        b->end = line;
        for (auto sym : b->syms)
        {
            InsertSymData(sym.first, sym.second);
        }
    }
}
void BRCLoader::ParseData(ObjFile& f)
{
    blocks.clear();
    for (auto it = f.BrowseInfoBegin(); it != f.BrowseInfoEnd(); ++it)
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
                Utils::Fatal("Unknown browse info record");
                break;
        }
    }
}
void BRCLoader::LoadSourceFiles(ObjFile& fil)
{
    indexMap.clear();
    for (auto it = fil.SourceFileBegin(); it != fil.SourceFileEnd(); ++it)
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
    for (int i = 2; i < files.size(); ++i)
    {
        std::string name = files[i];
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
                std::cout << "Invalid browse information file " << name << std::endl;
            }
        }
        else
        {
            std::cout << "Could not open file " << name << std::endl;
            rv = false;
        }
    }
    return rv;
}
