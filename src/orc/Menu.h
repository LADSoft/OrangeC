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

#ifndef Menu_h
#define Menu_h

#include "Resource.h"
#include <deque>
#include <memory>

class RCFile;
class ResFile;

class MenuItem;
typedef std::deque<std::unique_ptr<MenuItem>> MenuItemList;

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
    MenuItem() : type(0), state(0), id(0), helpIndex(0) {}
    virtual ~MenuItem();
    void WriteRes(ResFile& resFile, bool ex, bool last);
    static void ReadRC(RCFile& rcFile, MenuItemList& list, bool ex);
    void SetType(unsigned Type) { type = Type; }
    unsigned GetType() const { return type; }
    void SetState(unsigned State) { state = State; }
    unsigned GetState() const { return state; }
    void SetId(unsigned Id) { id = Id; }
    unsigned GetId() const { return id; }
    void SetText(const std::wstring& Text) { text = Text; }
    std::wstring GetText() const { return text; }
    void SetHelpIndex(int HelpIndex) { helpIndex = HelpIndex; }
    int GetHelpIndex() const { return helpIndex; }

    void Add(MenuItem* item);
    typedef MenuItemList::iterator iterator;
    iterator begin() { return popup.begin(); }
    iterator end() { return popup.end(); }

  protected:
    void GetFlags(RCFile& rcFile);
    bool ReadRCInternal(RCFile& rcFile, bool ex);

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
    Menu(const ResourceId& Id, const ResourceInfo& info, bool Ext) : Resource(eMenu, Id, info), helpIndex(0), extended(Ext) {}
    virtual ~Menu();
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);
    void SetHelpIndex(int HelpIndex) { helpIndex = HelpIndex; }
    int GetHelpIndex() const { return helpIndex; }
    void SetExtended(bool flag) { extended = flag; }
    bool GetExtended() const { return extended; }
    void Add(MenuItem* item);
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
