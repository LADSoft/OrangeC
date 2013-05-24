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
#ifndef LINKMAP_H
#define LINKMAP_H

#include "ObjTypes.h"
#include "LinkRegion.h"
#include <fstream>

class LinkManager;
class LinkPartition;
class LinkOverlay;
class LinkRegion;
class LinkSymbolData;
class LinkAttribs;

class LinkMap
{
    public:
        enum eMapType
        {
            eNormal,
            ePublic,
            eDetailed
        } ;
        enum eMapMode
        {
            eLinear,
            eSeg32,
            eSeg16
        };
        LinkMap(eMapType Type, eMapMode MapMode, const ObjString &Name, LinkManager *Manager)
            : type(Type), name(Name), manager(Manager), mode(MapMode) {}
        ~LinkMap() {}
        void WriteMap();
    private:
        struct MapSymbolData
        {
            MapSymbolData(LinkSymbolData *Sym, ObjInt Abs, ObjInt Base, int Group) :
                sym(Sym), base(Base), abs(Abs), group(Group) { }
            LinkSymbolData *sym;
            ObjInt base;
            ObjInt abs;
            int group;
            bool used;
        } ;
        std::fstream &Address(std::fstream &stream, ObjInt base, ObjInt offset, ObjInt group);
        ObjInt PublicBase(ObjExpression *exp, int &group);
        void NormalSections(std::fstream &stream);
        void DetailedSections(std::fstream &stream);
        void Publics(std::fstream &stream);
        void ShowPartitionLine(std::fstream &stream, LinkPartition *partition);
        void ShowOverlayLine(std::fstream &stream, LinkOverlay *overlay);
        void ShowRegionLine(std::fstream &stream, LinkRegion *region, ObjInt offs, int group);
        void ShowFileLine(std::fstream &stream, LinkRegion::OneSection *data, ObjInt offs);
        void ShowSymbol(std::fstream &stream, const MapSymbolData &symbol);
        void ShowAttribs(std::fstream &stream, LinkAttribs &attribs, ObjInt offs, int group);
        eMapType type;
        eMapMode mode;
        ObjString name;
        LinkManager *manager;
        struct linkltcomparebyname
        {
            bool operator () (const MapSymbolData &left, const MapSymbolData &right) 
                { return left.sym->GetSymbol()->GetName() < right.sym->GetSymbol()->GetName();}
        } ;
        struct linkltcomparebyvalue
        {
            bool operator () (const MapSymbolData &left, const MapSymbolData &right) 
                { return left.abs < right.abs;}
        } ;
        std::vector<LinkOverlay *>overlays;		
};
#endif
