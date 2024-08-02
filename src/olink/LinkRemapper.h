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

#ifndef LINKREMAPPER_H
#define LINKREMAPPER_H

#include "ObjTypes.h"
#include "LinkRegion.h"
#include <vector>

class LinkManager;
class LinkSymbolData;
class LinkRegion;
class LinkExpression;
class LinkExpressionSymbol;
class ObjFactory;
class ObjIndexManager;
class ObjFile;
class ObjSourceFile;
class ObjSection;
class ObjExpression;
class ObjSymbol;
class ObjType;

class LinkRemapper
{
  public:
      LinkRemapper(LinkManager& Manager, ObjFactory& Factory, ObjIndexManager& IndexManager, bool CompleteLink) :
          manager(&Manager), factory(&Factory), indexManager(&IndexManager), completeLink(CompleteLink)
    {
    }
    ~LinkRemapper() {}
    ObjFile* Remap();

  private:
    ObjInt RenumberSection(LinkRegion* region, ObjSection* dest, LinkRegion::OneSection* source, int group, int base);
    bool OptimizeRight(ObjExpression* exp, ObjExpression* right);
    ObjExpression* Optimize(ObjExpression* exp, ObjExpression* right);
    ObjExpression* RewriteExpression(LinkExpression* exp, LinkExpressionSymbol* sym);
    ObjExpression* ScanExpression(ObjExpression* offset, LinkSymbolData* d);
    void RenumberSymbol(ObjFile* file, ObjSymbol* sym, int index);
    ObjInt LookupFileType(ObjInt n);
    ObjInt GetTypeIndex(ObjType* type);
    void SetTypeIndex(ObjType* type);
    ObjInt RegisterType(ObjFile* file, ObjType* type, char* name);
    ObjInt MapType(ObjFile* file, ObjType* type);
    void RenumberType(ObjFile* file, ObjType* type);
    void RenumberSourceFile(ObjFile* file, ObjSourceFile* sourceFile);

    LinkManager* manager;
    ObjFactory* factory;
    ObjIndexManager* indexManager;
    bool completeLink;
    std::vector<ObjSection*> sections;
    std::map<ObjString, ObjInt> newTypes;
    std::map<ObjInt, ObjInt> fileTypes;
    static unsigned crc_table[256];
    unsigned crc32(unsigned char* buf, int len, unsigned crc);
};
#endif
