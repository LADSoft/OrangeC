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
#ifndef Menu_h
#define Menu_h

#include "Resource.h"
#include <deque>

class RCFile;
class ResFile;

class MenuItem;
typedef std::deque<MenuItem *> MenuItemList;

class MenuItem
{
public:
    enum
    {
        Grayed = 1,
        Inactive = 2,
        Bitmap = 4,
        Checked = 8,
        Popup = 16,
        MenuBarBreak = 32,
        MenuBreak = 64,
        EndMenu = 128,
        OwnerDraw = 256,
        Separator = 2048,
        Help = 16384
    };
    MenuItem() : type(0), state(0), id(0), popup(NULL), helpIndex(0) { }
    virtual ~MenuItem();
    void WriteRes(ResFile &resFile, bool ex, bool last);
    static bool ReadRC(RCFile &rcFile, MenuItemList &list, bool ex);
    void SetType(unsigned Type)  { type = Type; } 
    unsigned GetType() const { return type; }
    void SetState(unsigned State)  { state = State; } 
    unsigned GetState() const { return state; }
    void SetId(unsigned Id)  { id = Id; } 
    unsigned GetId() const { return id; }
    void SetText(const std::wstring &Text) { text = Text; }
    std::wstring GetText() const { return text; }
    void SetHelpIndex( int HelpIndex) { helpIndex = HelpIndex; }
    int GetHelpIndex() const { return helpIndex; }
    
    void Add(MenuItem *item) { popup.push_back(item); }
    typedef MenuItemList::iterator iterator;
    iterator begin() { return popup.begin(); }
    iterator end() { return popup.end(); }
protected:
    void GetFlags(RCFile &rcFile);
    bool ReadRCInternal(RCFile &rcFile, bool ex);
private:
    unsigned type; /* for menuex is a type, otherwise is above flags */
    unsigned state;
    unsigned id;
    std::wstring text;
    MenuItemList popup;
    // extended menuitem
    int helpIndex;
};
class Menu : public Resource
{
public:
    Menu(const ResourceId &Id, const ResourceInfo &info, bool Ext)
        : Resource(eMenu, Id, info), helpIndex(0), extended(Ext) { }
    virtual ~Menu();
    virtual void WriteRes(ResFile &resFile);
    virtual bool ReadRC(RCFile &rcFile);
    void SetHelpIndex(int HelpIndex) { helpIndex = HelpIndex; }
    int GetHelpIndex() const { return helpIndex; }
    void SetExtended(bool flag) { extended = flag; }
    bool GetExtended() const { return extended; }
    void Add(MenuItem *item) { menuItems.push_back(item); }	
    typedef MenuItemList::iterator iterator;
    iterator begin() { return menuItems.begin(); }
    iterator end() { return menuItems.end(); }
    
private:
    MenuItemList menuItems;
    // extended menu
    int helpIndex;
    bool extended;
};

#endif
