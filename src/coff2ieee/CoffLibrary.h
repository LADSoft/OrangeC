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

#ifndef COFFLIBRARY_h
#define COFFLIBRARY_H

#include "CoffHeader.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "LibFromCoffDictionary.h"
#include "LibManager.h"
#include "LibFiles.h"


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>

struct Module
{
    Module() : factory(&indexManager) { }
    std::set<std::string> aliases;
    ObjInt fileOffset;
    CoffLinkerMemberHeader header;
    bool import;
    bool ignore;
    ObjIeeeIndexManager indexManager;
    ObjFactory factory;
    ObjFile *file;
    std::string importName, importDLL;
};

class CoffLibrary
{
public:
    CoffLibrary(std::string Name) : name(Name), inputFile(nullptr), importFile(nullptr), importFactory(&importIndexManager) { }
    virtual ~CoffLibrary();

    bool Load();
    bool Convert();
    bool Write(std::string fileName);

protected:
    bool LoadNames();
    bool LoadHeaders();
    bool ScanIntegrity();
    bool ConvertNormalMods();
    bool ConvertImportMods();
    bool SaveLibrary(std::string fileName);
    void Align(FILE *ostr, ObjInt align = LibManager::ALIGN);
    
private:
    std::fstream *inputFile;        
    std::map<int, Module *> modules;
    std::string name;
    LibManager::LibHeader header;
    LibDictionary dictionary;
    LibFiles files;
    ObjIndexManager importIndexManager;
    ObjFactory importFactory;
    ObjFile *importFile;
};
#endif