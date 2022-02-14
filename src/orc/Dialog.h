/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#ifndef Dialog_h
#define Dialog_h

#include "Resource.h"
#include <memory>

class RCFile;
class ResFile;
class ResourceData;

class Control
{
  public:
    enum
    {
        Button = 0x80,
        Edit = 0x81,
        Static = 0x82,
        Listbox = 0x83,
        Scrollbar = 0x84,
        Combobox = 0x85
    };
    Control() : id(0), style(0), exStyle(0), helpIndex(0) {}
    ~Control();
    void WriteRes(ResFile& resFile, bool ex, bool last);
    void ReadRC(RCFile& rcFile, bool extended);
    void SetId(int Id) { id = Id; }
    int GetId() const { return id; }
    void SetStyle(int Style) { style = Style; }
    int GetStyle() const { return style; }
    void SetExStyle(int ExStyle) { exStyle = ExStyle; }
    int GetExStyle() const { return exStyle; }
    void SetHelpIndex(int HelpIndex) { helpIndex = HelpIndex; }
    int GetHelpIndex() const { return helpIndex; }
    void SetClass(const ResourceId& Cls) { cls = Cls; }
    ResourceId GetClass() const { return cls; }
    void SetText(const ResourceId& Text) { text = Text; }
    ResourceId GetText() const { return text; }
    void SetPos(const Point& Pos) { pos = Pos; }
    Point GetPos() const { return pos; }
    void SetSize(const Point& Size) { size = Size; }
    Point GetSize() const { return size; }
    void Add(ResourceData* rd);
    typedef std::deque<std::unique_ptr<ResourceData>>::iterator iterator;
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    static bool ValidType(RCFile& rcFile);

  protected:
    void ReadStandard(RCFile& rcFile, int cls, int style, int extended, int text);
    void GetClass(RCFile& rcFile);
    void ReadGeneric(RCFile& rcFile, bool extended);

  private:
    int id;
    int style;
    int exStyle;
    int helpIndex;
    ResourceId cls;
    ResourceId text;
    Point pos;
    Point size;
    std::deque<std::unique_ptr<ResourceData>> data;
};

class Dialog : public Resource
{
  public:
    Dialog(const ResourceId& Id, const ResourceInfo& info, bool Extended) :
        Resource(eDialog, Id, info),
        style(0),
        exStyle(0),
        pointSize(0),
        extended(Extended),
        weight(0),
        italics(0),
        charset(1),
        helpIndex(0)
    {
    }
    virtual ~Dialog();
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);

    void SetStyle(int Style) { style = Style; }
    int GetStyle() const { return style; }
    void SetExStyle(int ExStyle) { exStyle = ExStyle; }
    int GetExStyle() const { return exStyle; }
    void SetPointSize(int PointSize) { pointSize = PointSize; }
    int GetPointSize() const { return pointSize; }
    void SetPos(const Point& Pos) { pos = Pos; }
    Point GetPos() const { return pos; }
    void SetSize(const Point& Size) { size = Size; }
    Point GetSize() const { return size; }
    void SetClass(const ResourceId& Cls) { cls = Cls; }
    ResourceId GetClass() const { return cls; }
    void SetMenu(const ResourceId& Menu) { menu = Menu; }
    ResourceId GetMenu() const { return menu; }
    void SetCaption(const std::wstring& Caption) { caption = Caption; }
    std::wstring GetCaption() const { return caption; }
    void SetFont(const std::wstring& Font) { font = Font; }
    std::wstring GetFont() const { return font; }

    void Add(Control* Ctrl);
    typedef std::deque<std::unique_ptr<Control>>::iterator iterator;
    iterator begin() { return controls.begin(); }
    iterator end() { return controls.end(); }

  protected:
    void ReadSettings(RCFile& rcFile);

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
    std::deque<std::unique_ptr<Control>> controls;
    // extended dialog
    bool extended;
    int weight;
    int italics;
    int charset;
    int helpIndex;
};

#endif