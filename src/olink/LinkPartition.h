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
#ifndef LINKPARTITION_H
#define LINKPARTITION_H

#include <vector>
#include <set>
#include "LinkAttribs.h"

class LinkOverlay;
class LinkManager;
class LinkTokenizer;
class LinkExpression;
class CmdFiles;
class LinkOverlaySpecifier;
class ObjFile;

class LinkPartition
{
    typedef std::vector<LinkOverlaySpecifier *> OverlayContainer;
    public:
        LinkPartition(LinkManager *Parent) : name(""), parent(Parent) { }
        ~LinkPartition();

        ObjString &GetName() { return name; }
        void SetName(const ObjString &Name) { name = Name; }

        LinkAttribs &GetAttribs() { return attribs; }

        typedef OverlayContainer::iterator OverlayIterator;
        
        OverlayIterator OverlayBegin() { return overlays.begin(); }
        OverlayIterator OverlayEnd() { return overlays.end(); }

        bool CreateSeparateRegions(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec);
        bool ParsePartitionSpec(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec);
        
        void Add(LinkOverlaySpecifier *ov) { overlays.push_back(ov); }
        ObjInt PlacePartition(LinkManager *manager, ObjInt bottom, bool completeLink, int &overlayNum);
        
    private:
        bool ParseValue(LinkTokenizer &spec, LinkExpression **rv, bool alreadyassign = false);
        bool ParseAttributes(LinkTokenizer &spec);
        bool ParseName(LinkTokenizer &spec);
        bool ParseOverlays(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec);
        bool ParseAssignment(LinkTokenizer &spec);
        ObjString name;
        OverlayContainer overlays;
        LinkAttribs attribs;
        LinkManager *parent;
};
class LinkPartitionSpecifier
{
    public:
        LinkPartitionSpecifier(LinkPartition *Partition) : partition(Partition), symbol(NULL) {}
        LinkPartitionSpecifier(LinkExpressionSymbol *Symbol) : partition(NULL), symbol(Symbol) {}
        ~LinkPartitionSpecifier()
        {
            delete partition;
            delete symbol;
        }
        LinkPartition *GetPartition() { return partition; }
        LinkExpressionSymbol *GetSymbol() { return symbol; }
        
    private:
        LinkPartition *partition;
        LinkExpressionSymbol *symbol;
};
class LinkOverlaySpecifier
{
    public:
        LinkOverlaySpecifier(LinkOverlay *Overlay) : overlay(Overlay), symbol(NULL) {}
        LinkOverlaySpecifier(LinkExpressionSymbol *Symbol) : overlay(NULL), symbol(Symbol) {}
        ~LinkOverlaySpecifier();
        LinkOverlay *GetOverlay() { return overlay; }
        LinkExpressionSymbol *GetSymbol() { return symbol; }
        
    private:
        LinkOverlay *overlay;
        LinkExpressionSymbol *symbol;
};
#endif
