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
#ifndef Dialog_h
#define Dialog_h

#include "Resource.h"

class RCFile;
class ResFile;
class ResourceData;

class Control
{
public:
    enum
    {
        Button= 0x80,
        Edit = 0x81,
        Static = 0x82,
        Listbox = 0x83,
        Scrollbar = 0x84,
        Combobox = 0x85
    };
    Control() : id(0), style(0), exStyle(0), helpIndex(0) { }
    ~Control();
    void WriteRes(ResFile &resFile, bool ex, bool last);
    void ReadRC(RCFile &rcFile, bool extended);
    void SetId(int Id) { id = Id; }
    int GetId() const { return id; }
    void SetStyle(int Style) { style = Style; }
    int GetStyle() const { return style; }
    void SetExStyle(int ExStyle) { exStyle = ExStyle; }
    int GetExStyle() const { return exStyle; }
    void SetHelpIndex(int HelpIndex) { helpIndex = HelpIndex; }
    int GetHelpIndex() const { return helpIndex; }
    void SetClass(const ResourceId &Cls) { cls = Cls; }
    ResourceId GetClass() const { return cls; }
    void SetText(const ResourceId &Text) { text = Text; }
    ResourceId GetText() const { return text; }
    void SetPos(const Point &Pos) { pos = Pos; }
    Point GetPos() const { return pos; }
    void SetSize(const Point &Size) { size = Size; }
    Point GetSize() const { return size; }
    void Add(ResourceData *rd) { data.push_back(rd); }
    typedef std::deque<ResourceData *>::iterator iterator;
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    static bool ValidType(RCFile &rcFile);
protected:
    void ReadStandard(RCFile &rcFile, int cls , int style, int extended, int text);
    void GetClass(RCFile &rcFile);
    void ReadGeneric(RCFile &rcFile, bool extended);
private:
    int id;
    int style;
    int exStyle;
    int helpIndex;
    ResourceId cls;
    ResourceId text;
    Point pos;
    Point size;
    std::deque<ResourceData *> data;
};

class Dialog : public Resource
{
public:
    Dialog(const ResourceId &Id, const ResourceInfo &info, bool Extended)
        : Resource(eDialog, Id, info), style(0), exStyle(0), pointSize(0),
            extended(Extended), weight(0), italics(0), charset(1), helpIndex(0) { }
    virtual ~Dialog();
    virtual void WriteRes(ResFile &resFile);
    virtual void ReadRC(RCFile &rcFile);

    void SetStyle(int Style) { style = Style; }
    int GetStyle() const { return style; }
    void SetExStyle(int ExStyle) { exStyle = ExStyle; }
    int GetExStyle() const { return exStyle; }
    void SetPointSize(int PointSize) { pointSize = PointSize; }
    int GetPointSize() const { return pointSize; }
    void SetPos(const Point &Pos) { pos = Pos; }
    Point GetPos() const { return pos; }
    void SetSize(const Point &Size) { size = Size; }
    Point GetSize() const { return size; }
    void SetClass(const ResourceId &Cls) { cls = Cls; }
    ResourceId GetClass() const { return cls; }
    void SetMenu(const ResourceId &Menu) { menu = Menu; }
    ResourceId GetMenu() const { return menu; }
    void SetCaption(const std::wstring &Caption) { caption = Caption; }
    std::wstring GetCaption() const { return caption; }
    void SetFont(const std::wstring &Font) { font = Font; }
    std::wstring GetFont() const { return font; }
    
    void Add(Control *Ctrl) { controls.push_back(Ctrl); }
    typedef std::deque<Control *>::iterator iterator;
    iterator begin() { return controls.begin(); }
    iterator end() { return controls.end(); }

protected:	
    void ReadSettings(RCFile &rcFile);
private:
    int style;
    int exStyle;
    int pointSize;
    Point pos;
    Point size;
    ResourceId cls;
    ResourceId menu;
    std::wstring caption;
    std::wstring font;
    std::deque<Control *> controls;
    // extended dialog
    bool extended;
    int weight;
    int italics;
    int charset;
    int helpIndex;
};

#endif