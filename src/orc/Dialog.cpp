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

#include "Dialog.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#ifndef HAVE_UNISTD_H
#    include <windows.h>
#endif
#include <stdexcept>
Control::~Control() {}
void Control::Add(ResourceData* rd)
{
    std::unique_ptr<ResourceData> temp(rd);
    data.push_back(std::move(temp));
}

void Control::WriteRes(ResFile& resFile, bool ex, bool last)
{
    (void)last;
    resFile.Align();
    if (!ex)
    {
        resFile.WriteDWord(style);
        resFile.WriteDWord(exStyle);
    }
    else
    {
        resFile.WriteDWord(helpIndex);
        resFile.WriteDWord(exStyle);
        resFile.WriteDWord(style);
    }
    pos.WriteRes(resFile);
    size.WriteRes(resFile);
    if (ex)
        resFile.WriteDWord(id);
    else
        resFile.WriteWord(id);
    cls.WriteRes(resFile);
    text.WriteRes(resFile);
    int len = 0;
    for (auto& res : *this)
    {
        len += res->GetLen();
    }
    resFile.WriteWord(len);
    if (len)
    {
        resFile.Align();
        for (auto& res : *this)
        {
            if (res->GetLen())
            {
                resFile.WriteData(res->GetData(), res->GetLen());
            }
        }
    }
}
bool Control::ValidType(RCFile& rcFile)
{
    switch (rcFile.GetToken()->GetKeyword())
    {
        case kw::AUTO3STATE:
        case kw::AUTOCHECKBOX:
        case kw::AUTORADIOBUTTON:
        case kw::CHECKBOX:
        case kw::COMBOBOX:
        case kw::CTEXT:
        case kw::DEFPUSHBUTTON:
        case kw::EDITTEXT:
        case kw::GROUPBOX:
        case kw::ICON:
        case kw::LISTBOX:
        case kw::LTEXT:
        case kw::PUSHBUTTON:
        case kw::RADIOBUTTON:
        case kw::RTEXT:
        case kw::SCROLLBAR:
        case kw::STATE3:
        case kw::CONTROL:
            return true;
        default:
            return false;
    }
}
void Control::ReadStandard(RCFile& rcFile, int clss, int addStyle, int extended, int hasText)
{
#ifndef HAVE_UNISTD_H
    addStyle |= WS_CHILD | WS_VISIBLE;
#endif
    cls.SetId(clss);
    if (hasText)
    {
        text.ReadRC(rcFile, true);
        rcFile.SkipComma();
    }
    id = rcFile.GetNumber();
    rcFile.SkipComma();
    pos.x = rcFile.GetNumber();
    rcFile.SkipComma();
    pos.y = rcFile.GetNumber();
    rcFile.SkipComma();
    size.x = rcFile.GetNumber();
    rcFile.SkipComma();
    size.y = rcFile.GetNumber();
    rcFile.SkipComma();
    if (!rcFile.AtEol())
    {
        style = addStyle | rcFile.GetNumber();
        rcFile.SkipComma();
    }
    else
        style = addStyle;
    if (!rcFile.AtEol())
    {
        exStyle = rcFile.GetNumber();
        rcFile.SkipComma();
    }
    if (!rcFile.AtEol())
    {
        helpIndex = rcFile.GetNumber();
        rcFile.SkipComma();
    }
#ifndef HAVE_UNISTD_H
    if (cls == Scrollbar)
    {
        if (style & SBS_VERT)
            style &= ~SBS_HORZ;
    }
#endif
}
void Control::GetClass(RCFile& rcFile)
{
    if (rcFile.IsString())
    {
        std::wstring str = rcFile.GetString();
        std::wstring str1 = str;
        for (int i = 0; i < str1.size(); i++)
            str1[i] = tolower(str1[i]);
        if (str1 == L"button")
            cls.SetId(Button);
        else if (str1 == L"edit")
            cls.SetId(Edit);
        else if (str1 == L"static")
            cls.SetId(Static);
        else if (str1 == L"listbox")
            cls.SetId(Listbox);
        else if (str1 == L"scrollbar")
            cls.SetId(Scrollbar);
        else if (str1 == L"combobox")
            cls.SetId(Combobox);
        else
        {
            cls.SetName(str);
        }
    }
    else
        switch (rcFile.GetTokenId())
        {
            case kw::AUTO3STATE:
            case kw::AUTOCHECKBOX:
            case kw::AUTORADIOBUTTON:
            case kw::CHECKBOX:
            case kw::PUSHBUTTON:
            case kw::RADIOBUTTON:
            case kw::DEFPUSHBUTTON:
            case kw::STATE3:
            case kw::GROUPBOX:
                cls.SetId(Button);
                break;
            case kw::COMBOBOX:
                cls.SetId(Combobox);
                break;
            case kw::CTEXT:
            case kw::LTEXT:
            case kw::RTEXT:
            case kw::ICON:
                cls.SetId(Static);
                break;
            case kw::EDITTEXT:
                cls.SetId(Edit);
                break;
            case kw::LISTBOX:
                cls.SetId(Listbox);
                break;
            case kw::SCROLLBAR:
                cls.SetId(Scrollbar);
                break;
            default:
                throw std::runtime_error("Unknown dialog control class");
        }
}
void Control::ReadGeneric(RCFile& rcFile, bool extended)
{
    text.ReadRC(rcFile, true);
    rcFile.SkipComma();
    id = rcFile.GetNumber();
    rcFile.SkipComma();
    GetClass(rcFile);
    rcFile.SkipComma();
#ifndef HAVE_UNISTD_H
    style = rcFile.GetNumber() | WS_CHILD | WS_VISIBLE;
#else
    style = rcFile.GetNumber();
#endif
    rcFile.SkipComma();
    pos.x = rcFile.GetNumber();
    rcFile.SkipComma();
    pos.y = rcFile.GetNumber();
    rcFile.SkipComma();
    size.x = rcFile.GetNumber();
    rcFile.SkipComma();
    size.y = rcFile.GetNumber();
    if (!rcFile.AtEol())
    {
        rcFile.SkipComma();
        exStyle = rcFile.GetNumber();
    }
    if (!rcFile.AtEol())
    {
        helpIndex = rcFile.GetNumber();
        rcFile.SkipComma();
    }
}

void Control::ReadRC(RCFile& rcFile, bool extended)
{
#ifndef HAVE_UNISTD_H
    switch (rcFile.GetTokenId())
    {
        case kw::AUTO3STATE:
            ReadStandard(rcFile, Button, BS_AUTO3STATE | WS_TABSTOP, extended, 1);
            break;
        case kw::AUTOCHECKBOX:
            ReadStandard(rcFile, Button, BS_AUTOCHECKBOX | WS_TABSTOP, extended, 1);
            break;
        case kw::AUTORADIOBUTTON:
            ReadStandard(rcFile, Button, BS_AUTORADIOBUTTON, extended, 1);
            break;
        case kw::CHECKBOX:
            ReadStandard(rcFile, Button, BS_CHECKBOX | WS_TABSTOP, extended, 1);
            break;
        case kw::COMBOBOX:
            ReadStandard(rcFile, Combobox, 0, extended, 0);
            if (!(style & 3))
                style |= CBS_SIMPLE;
            break;
        case kw::CTEXT:
            ReadStandard(rcFile, Static, SS_CENTER | WS_GROUP, extended, 1);
            break;
        case kw::DEFPUSHBUTTON:
            ReadStandard(rcFile, Button, BS_DEFPUSHBUTTON | WS_TABSTOP, extended, 1);
            break;
        case kw::EDITTEXT:
            ReadStandard(rcFile, Edit, ES_LEFT | WS_BORDER | WS_TABSTOP, extended, 0);
            break;
        case kw::GROUPBOX:
            ReadStandard(rcFile, Button, BS_GROUPBOX, extended, 1);
            break;
        case kw::ICON:
            ReadStandard(rcFile, Static, SS_ICON, extended, 1);
            break;
        case kw::LISTBOX:
            ReadStandard(rcFile, Listbox, LBS_NOTIFY | WS_BORDER, extended, 0);
            break;
        case kw::LTEXT:
            ReadStandard(rcFile, Static, SS_LEFT | WS_GROUP, extended, 1);
            break;
        case kw::PUSHBUTTON:
            ReadStandard(rcFile, Button, BS_PUSHBUTTON | WS_TABSTOP, extended, 1);
            break;
        case kw::RADIOBUTTON:
            ReadStandard(rcFile, Button, BS_RADIOBUTTON, extended, 1);
            break;
        case kw::RTEXT:
            ReadStandard(rcFile, Static, SS_RIGHT | WS_GROUP, extended, 1);
            break;
        case kw::SCROLLBAR:
            ReadStandard(rcFile, Scrollbar, SBS_HORZ, extended, 0);
            break;
        case kw::STATE3:
            ReadStandard(rcFile, Button, BS_3STATE | WS_TABSTOP, extended, 1);
            break;
        case kw::CONTROL:
            ReadGeneric(rcFile, extended);
            break;
        default:
            // shouldn't get here
            break;
    }
#endif
    rcFile.NeedEol();
}
Dialog::~Dialog() {}

void Dialog::Add(Control* Ctrl)
{
    std::unique_ptr<Control> temp(Ctrl);
    controls.push_back(std::move(temp));
}

void Dialog::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    if (!extended)
    {
        resFile.WriteDWord(style);
        resFile.WriteDWord(exStyle);
    }
    else
    {
        resFile.WriteWord(1);
        resFile.WriteWord(0xffff);
        resFile.WriteDWord(helpIndex);
        resFile.WriteDWord(exStyle);
        resFile.WriteDWord(style);
    }
    resFile.WriteWord(controls.size());
    pos.WriteRes(resFile);
    size.WriteRes(resFile);

    menu.WriteRes(resFile);
    cls.WriteRes(resFile);

    resFile.WriteString(caption);

#ifndef HAVE_UNISTD_H
    if (style & DS_SETFONT)
    {
        resFile.WriteWord(pointSize);
        if (extended)
        {
            resFile.WriteWord(weight);
            resFile.WriteByte(italics);
            resFile.WriteByte(charset);
        }
        resFile.WriteString(font);
    }
#endif
    int count = controls.size();
    for (auto& res : *this)
    {
        resFile.Align();
        res->WriteRes(resFile, extended, !--count);
    }
    resFile.Release();
}
void Dialog::ReadSettings(RCFile& rcFile)
{
    bool hascaption = false, hasstyle = false, hasfont = false;
    resInfo.SetLanguage(rcFile.GetLanguage());
    bool done = false;
    while (!done && rcFile.IsKeyword())
    {
        switch (rcFile.GetToken()->GetKeyword())
        {
            case kw::LANGUAGE: {
                rcFile.NextToken();
                int n = rcFile.GetNumber();
                rcFile.SkipComma();
                n += rcFile.GetNumber() * 1024;
                resInfo.SetLanguage(n);
            }
            break;
            case kw::VERSION:
                rcFile.NextToken();
                resInfo.SetVersion(rcFile.GetNumber());
                break;
            case kw::CHARACTERISTICS:
                rcFile.NextToken();
                resInfo.SetCharacteristics(rcFile.GetNumber());
                break;
            case kw::STYLE:
                hasstyle = true;
                rcFile.NextToken();
                style = rcFile.GetNumber();
                break;
            case kw::EXSTYLE:
                rcFile.NextToken();
                exStyle |= rcFile.GetNumber();
                break;
            case kw::MENU:
                rcFile.NextToken();
                menu.ReadRC(rcFile);
                break;
            case kw::FONT:
                hasfont = true;
                rcFile.NextToken();
                pointSize = rcFile.GetNumber();
                rcFile.SkipComma();
                font = rcFile.GetString();
                if (extended)
                {
                    rcFile.SkipComma();
                    if (!rcFile.AtEol())
                    {
                        weight = rcFile.GetNumber();
                        rcFile.SkipComma();
                        if (!rcFile.AtEol())
                        {
                            italics = rcFile.GetNumber();
                            rcFile.SkipComma();
                            if (!rcFile.AtEol())
                            {
                                charset = rcFile.GetNumber();
                            }
                        }
                    }
                }
                break;
            case kw::CAPTION:
                hascaption = true;
                rcFile.NextToken();
                caption = rcFile.GetString();
                break;
            case kw::CLASS:
                rcFile.NextToken();
                cls.ReadRC(rcFile, true);
                break;
            case kw::HELP:
                rcFile.NextToken();
                if (!extended)
                    throw std::runtime_error("Need DialogEx");
                helpIndex = rcFile.GetNumber();
                break;
            case kw::WEIGHT:
                rcFile.NextToken();
                if (!extended)
                    throw std::runtime_error("Need DialogEx");
                weight = rcFile.GetNumber();
                break;
            case kw::ITALIC:
                rcFile.NextToken();
                if (!extended)
                    throw std::runtime_error("Need DialogEx");
                italics = rcFile.GetNumber();
                break;
            default:
                done = true;
                break;
        }
        if (!done)
        {
            rcFile.NeedEol();
        }
    }
#ifndef HAVE_UNISTD_H
    if (hascaption)
        style |= WS_CAPTION;
    if (!hasstyle)
        style |= WS_POPUPWINDOW;
    if (hasfont)
        style |= DS_SETFONT;
#endif
}
void Dialog::ReadRC(RCFile& rcFile)
{
    if (extended)
    {
        italics = 256;
    }
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, false);
    pos.x = rcFile.GetNumber();
    rcFile.SkipComma();
    pos.y = rcFile.GetNumber();
    rcFile.SkipComma();
    size.x = rcFile.GetNumber();
    rcFile.SkipComma();
    size.y = rcFile.GetNumber();
    rcFile.SkipComma();
    rcFile.NeedEol();
    ReadSettings(rcFile);
    rcFile.NeedBegin();
    while (Control::ValidType(rcFile))
    {
        controls.push_back(std::make_unique<Control>());
        controls.back()->ReadRC(rcFile, extended);
    }
    rcFile.NeedEnd();
}
