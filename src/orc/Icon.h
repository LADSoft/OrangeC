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
#ifndef Icon_h
#define Icon_h

#include "Resource.h"
#include "ResourceData.h"
#include <deque>

class RCFile;
class ResFile;
class ResourceData;

class Icon : public Resource
{
public:
    Icon(const ResourceInfo &info)
        : Resource(eIcon, ResourceId(++nextIconIndex), info),
        colors(0), planes(0), bits(0), data(NULL) { }
    Icon(const ResourceInfo &info, const ResourceId &id)
        : Resource(eIcon, id, info),
        colors(0), planes(0), bits(0), data(NULL) { }
    virtual ~Icon() { delete data; }
    void ReadBin(ResourceData *rd);
    virtual void WriteRes(ResFile &resFile);
    virtual void ReadRC(RCFile &rcFile) { }
    unsigned GetIndex() const { return GetId().GetId(); }
    void SetSize(const Point &Size) { size = Size; }
    Point GetSize() const { return size; }
    void SetColors(const unsigned Colors) { colors = Colors; }
    unsigned GetColors() const { return colors; }
    void SetPlanes(const unsigned Planes) { planes = Planes; }
    unsigned GetPlanes() const { return planes; }
    unsigned GetBytes() const { if (data) return data->GetLen(); else return 0; }
    void SetBits(const unsigned Bits) { bits = Bits; }
    unsigned GetBits() const { return bits; }
    void SetData(ResourceData *rdata) {
        delete data;
        data = rdata;
    }
    ResourceData *GetData() const { return data; }
    static void Reset() { nextIconIndex = 0; }
protected:
    ResourceData *data;
private:
    Point size;
    unsigned colors;
    unsigned planes;
    unsigned bits;
    static int nextIconIndex;
};
class GroupIcon : public Resource
{
public:
    GroupIcon(const ResourceId &Id, const ResourceInfo &info)
        : Resource(eGroupIcon, Id, info) { }
    virtual ~GroupIcon() { }
    virtual void WriteRes(ResFile &resFile);
    virtual void ReadRC(RCFile &rcFile);
    
    void Add(Icon *icon) { icons.push_back(icon); }
    
    typedef std::deque<Icon *>::iterator iterator;
    iterator begin() { return icons.begin(); }
    iterator end() { return icons.end(); }
    
private:
    std::deque< ::Icon *> icons;
};

#endif