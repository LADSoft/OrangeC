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
#ifndef Cursor_h
#define Cursor_h

#include "Resource.h"
#include "Icon.h"

#include <deque>

class RCFile;
class ResFile;
class ResourceData;

class Cursor : public Icon
{
public:
    Cursor(const ResourceInfo &info)
        : Icon(info, ResourceId(++nextCursorIndex)) { SetType(eCursor); }
    virtual ~Cursor() { }
    virtual void WriteRes(ResFile &resFile);
    bool ReadBin(ResourceData *rd);
    
    void SetHotspot(Point &hs) { hotspot = hs; }
    Point GetHotspot() const { return hotspot; }
    static void Reset() { nextCursorIndex = 0; }
private:
    Point hotspot;
    static int nextCursorIndex;
};
class GroupCursor : public Resource
{
public:
    GroupCursor(const ResourceId &Id, const ResourceInfo &info)
        : Resource(eGroupCursor, Id, info) { }
    virtual ~GroupCursor() { }
    virtual void WriteRes(ResFile &resFile);
    virtual void ReadRC(RCFile &rcFile);
    
    void Add(Cursor *Cursor) { cursors.push_back(Cursor); }
    typedef std::deque<Cursor *>::iterator iterator;
    iterator begin() { return cursors.begin(); }
    iterator end() { return cursors.end(); }
private:
    std::deque<Cursor *> cursors;
};

#endif