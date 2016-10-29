/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#ifndef Resource_h
#define Resource_h

#include "ResourceId.h"
#include "ResourceInfo.h"
#include <fstream>
#include <deque>

class RCFile;
class ResFile;

struct Point
{
    Point() : x(0), y(0) { }
    Point(int X, int Y) { x = X, y = Y; }
    int x;
    int y;
    void WriteRes(ResFile &resFile);
} ;

class Resource
{
public:
    enum ResourceType
    {
        eFileHeader,
        eCursor,
        eBitmap,
        eIcon,
        eMenu,
        eDialog,
        eString,
        eFontDirectory,
        eFont,
        eAccelerators,
        eRCData,
        eMessageTable,
        eGroupCursor,
        eGroupIcon = 14,
        eVersion = 16,
        eDlgInclude,
        ePlugPlay,
        eVXD,
        eAnimatedCursor = 21,
        eAnimatedIcon
    };
    Resource(ResourceType ResType, const ResourceId &Id, const ResourceInfo &info)
        : type(ResType), id(Id), resInfo(info) { }
    Resource(const ResourceId &ResType, const ResourceId &Id, const ResourceInfo &info)
        : type(ResType), id(Id), resInfo(info) { }
    virtual ~Resource () { }

    virtual void WriteRes(ResFile &resFile);
    virtual void ReadRC(RCFile &rcFile) { }

    void SetResInfo(const ResourceInfo &info) { resInfo = info; }
    ResourceInfo GetResInfo() const { return resInfo; }	
    ResourceId GetType() const { return type; }
    void SetType(int Type) { type = ResourceId(Type); }
    ResourceId GetId() const { return id; }

protected:
    ResourceInfo resInfo;
    ResourceId type;
    ResourceId id;
    
} ;

#endif