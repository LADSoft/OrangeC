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
        LinkRemapper(LinkManager &Manager, ObjFactory &Factory, ObjIndexManager &IndexManager, bool CompleteLink)	:
            manager(&Manager), factory(&Factory), indexManager(&IndexManager), completeLink(CompleteLink) {}
        ~LinkRemapper() {}
        ObjFile *Remap();
        
    private:
        ObjInt RenumberSection(LinkRegion *region, ObjSection *dest, LinkRegion::OneSection *source, int group, int base);
        bool OptimizeRight(ObjExpression *exp, ObjExpression *right);
        ObjExpression *Optimize(ObjExpression *exp, ObjExpression *right);
        ObjExpression *RewriteExpression(LinkExpression *exp, LinkExpressionSymbol *sym);
        ObjExpression *ScanExpression(ObjExpression *offset, LinkSymbolData *d);
        void RenumberSymbol(ObjFile *file, ObjSymbol *sym, int index);
        ObjInt LookupFileType(ObjInt n);
        ObjInt GetTypeIndex(ObjType *type);
        void SetTypeIndex(ObjType *type);
        ObjInt RegisterType(ObjFile *file, ObjType *type, char *name);
        ObjInt MapType(ObjFile *file, ObjType *type);
        void RenumberType(ObjFile *file, ObjType *type);
        void RenumberSourceFile(ObjFile *file, ObjSourceFile *sourceFile);
        
        LinkManager *manager;
        ObjFactory *factory;
        ObjIndexManager *indexManager;
        bool completeLink;
        std::vector<ObjSection *> sections;
        std::map<ObjString, ObjInt> newTypes;
        std::map<ObjInt, ObjInt> fileTypes;
        static unsigned LinkRemapper::crc_table[256];
        unsigned crc32(unsigned char *buf, int len, unsigned crc);
} ;
#endif
