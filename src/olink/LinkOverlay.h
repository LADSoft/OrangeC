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
#ifndef LINKOVERLAY_H
#define LINKOVERLAY_H
#include <vector>
#include "LinkAttribs.h"

class LinkRegion;
class LinkPartition;
class LinkTokenizer;
class LinkExpression;
class LinkManager;
class CmdFiles;
class LinkRegionSpecifier;
class ObjFile;

class LinkOverlay
{
    typedef std::vector<LinkRegionSpecifier *> RegionContainer;
    public:
        LinkOverlay(LinkPartition *Parent) : parent(Parent) {}
        ~LinkOverlay();
        
        ObjString GetName() { return name; }
        void SetName(const ObjString &Name) { name = Name; }
                
        LinkAttribs &GetAttribs() { return attribs; }

        LinkPartition *GetParent() { return parent; }
        void SetParent(LinkPartition *Parent) { parent = Parent; }

        typedef RegionContainer::iterator RegionIterator;
        
        RegionIterator RegionBegin() { return regions.begin(); }
        RegionIterator RegionEnd() { return regions.end(); }
        
        void Add(LinkRegionSpecifier *region) { regions.push_back(region); }
        bool ParseOverlaySpec(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec);

        ObjInt PlaceOverlay(LinkAttribs &partitionAttribs, bool completeLink, int overlayNum);
        
    private:
        bool ParseAssignment(LinkTokenizer &spec);
        bool ParseName(LinkTokenizer &spec);
        bool ParseValue(LinkTokenizer &spec, LinkExpression **rv, bool alreadyassign = false);
        bool ParseAttributes( LinkTokenizer &spec);
        ObjString name;
        LinkPartition *parent;
        RegionContainer regions;
        LinkAttribs attribs;
};
class LinkRegionSpecifier
{
    public:
        LinkRegionSpecifier(LinkRegion *Region) : region(Region), symbol(NULL) {}
        LinkRegionSpecifier(LinkExpressionSymbol *Symbol) : region(NULL), symbol(Symbol) {}
        ~LinkRegionSpecifier();
        LinkRegion *GetRegion() { return region; }
        LinkExpressionSymbol *GetSymbol() { return symbol; }
    private:
        LinkRegion *region;
        LinkExpressionSymbol *symbol;
};
#endif
