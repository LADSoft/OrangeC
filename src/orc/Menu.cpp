/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "Menu.h"
#include "RCFile.h"
#include "ResFile.h"
#include <fstream>
#include <stdexcept>

MenuItem::~MenuItem() {}
void MenuItem::Add(MenuItem* item)
{
    std::unique_ptr<MenuItem> temp(item);
    popup.push_back(std::move(temp));
}

void MenuItem::WriteRes(ResFile& resFile, bool ex, bool last)
{
    if (!ex)
    {
        int fl = type;
        if (!popup.empty())
            fl |= Popup;
        if (last)
            fl |= EndMenu;
        resFile.WriteWord(fl);
        if (!(fl & Popup))
        {
            resFile.WriteWord(id);
        }
        resFile.WriteString(text);
        if (fl & Popup)
        {
            int count = popup.size();
            for (auto& res : *this)
                res->WriteRes(resFile, ex, !--count);
        }
    }
    else
    {
        resFile.Align();
        resFile.WriteDWord(type);
        resFile.WriteDWord(state);
        resFile.WriteDWord(id);
        int fl = 0;
        if (!popup.empty())
            fl |= 1;
        if (last)
            fl |= EndMenu;
        resFile.WriteWord(fl);
        resFile.WriteString(text);
        if (fl & 1)
        {
            resFile.Align();
            resFile.WriteDWord(helpIndex);
            int count = popup.size();
            for (auto& res : *this)
                res->WriteRes(resFile, ex, !--count);
        }
    }
}
void MenuItem::GetFlags(RCFile& rcFile)
{
    bool done = false;
    while (!done)
    {
        switch (rcFile.GetToken()->GetKeyword())
        {
            case kw::GRAYED:
                type |= Grayed;
                break;
            case kw::INACTIVE:
                type |= Inactive;
                break;
            case kw::CHECKED:
                type |= Checked;
                break;
            case kw::MENUBARBREAK:
                type |= MenuBarBreak;
                break;
            case kw::MENUBREAK:
                type |= MenuBreak;
                break;
            case kw::HELP:
                type |= Help;
                break;
            case kw::SEPARATOR:
                break;
            default:
                done = true;
                break;
        }
        if (!done)
        {
            rcFile.NextToken();
            rcFile.SkipComma();
        }
    }
    if (!rcFile.AtEol())
    {
        state = rcFile.GetNumber();
    }
}
bool MenuItem::ReadRCInternal(RCFile& rcFile, bool ex)
{
    bool rv = false;
    if (rcFile.IsKeyword())
        switch (rcFile.GetToken()->GetKeyword())
        {
            case kw::MENUITEM: {
                rcFile.NextToken();
                if (rcFile.IsKeyword())
                {
                    if (rcFile.GetTokenId() != kw::SEPARATOR)
                        throw std::runtime_error("Invalid menu type");
                }
                else if (rcFile.IsString())
                {
                    text = rcFile.GetString();
                    rcFile.SkipComma();
                }
                if (rcFile.IsNumber())
                {
                    id = rcFile.GetNumber();
                    rcFile.SkipComma();
                }
                if (ex)
                {
                    if (rcFile.IsNumber())
                    {
                        type = rcFile.GetNumber();
                        rcFile.SkipComma();
                    }
                    if (rcFile.IsNumber())
                    {
                        state = rcFile.GetNumber();
                        rcFile.SkipComma();
                    }
                }
                else
                {
                    GetFlags(rcFile);
                }
                rcFile.NeedEol();
            }
            break;
            case kw::POPUP: {
                rcFile.NextToken();
                if (rcFile.IsString())
                {
                    text = rcFile.GetString();
                    rcFile.SkipComma();
                }
                if (rcFile.IsNumber())
                {
                    id = rcFile.GetNumber();
                    rcFile.SkipComma();
                }
                if (ex)
                {
                    if (rcFile.IsNumber())
                    {
                        type = rcFile.GetNumber();
                        rcFile.SkipComma();
                    }
                    if (rcFile.IsNumber())
                    {
                        state = rcFile.GetNumber();
                        rcFile.SkipComma();
                    }
                    if (rcFile.IsNumber())
                    {
                        helpIndex = rcFile.GetNumber();
                        rcFile.SkipComma();
                    }
                }
                else
                {
                    GetFlags(rcFile);
                }
                rcFile.NeedEol();
                ReadRC(rcFile, popup, ex);
            }
            break;
            default:
                rv = true;
                break;
        }

    return rv;
}
void MenuItem::ReadRC(RCFile& rcFile, MenuItemList& list, bool ex)
{
    rcFile.NeedBegin();
    bool done = false;
    while (!done)
    {

        list.push_back(std::make_unique<MenuItem>());
        if ((done = list.back()->ReadRCInternal(rcFile, ex)))
            list.pop_back();
    }
    rcFile.NeedEnd();
}
Menu::~Menu() {}
void Menu::Add(MenuItem* item)
{
    std::unique_ptr<MenuItem> temp(item);
    menuItems.push_back(std::move(temp));
}
void Menu::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    if (!extended)
    {
        resFile.WriteWord(0);
        resFile.WriteWord(0);
    }
    else
    {
        resFile.WriteWord(1);
        resFile.WriteWord(4);
        resFile.WriteDWord(helpIndex);
    }
    int count = menuItems.size();
    for (auto& res : *this)
        res->WriteRes(resFile, extended, !--count);
    resFile.Release();
}
void Menu::ReadRC(RCFile& rcFile)
{
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, true);
    MenuItem::ReadRC(rcFile, menuItems, extended);
}
