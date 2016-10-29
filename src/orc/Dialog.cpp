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
#include "Dialog.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <windows.h>
#include <stdexcept>
Control::~Control()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        ResourceData *d = *it;
        delete d;
    }
}
void Control::WriteRes(ResFile &resFile, bool ex, bool last)
{
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
    for (iterator it = begin() ; it != end(); ++it)
    {
        len += (*it)->GetLen();
    }
    resFile.WriteWord(len);
    if (len)
    {
        resFile.Align();
        for (iterator it = begin() ; it != end(); ++it)
        {
            if ((*it)->GetLen())
            {
                resFile.WriteData((*it)->GetData(), (*it)->GetLen());
            }
        }
    }
}
bool Control::ValidType(RCFile &rcFile)
{
    switch (rcFile.GetToken()->GetKeyword())
    {
        case Lexer::AUTO3STATE:
        case Lexer::AUTOCHECKBOX:
        case Lexer::AUTORADIOBUTTON:
        case Lexer::CHECKBOX:
        case Lexer::COMBOBOX:
        case Lexer::CTEXT:
        case Lexer::DEFPUSHBUTTON:
        case Lexer::EDITTEXT:
        case Lexer::GROUPBOX:
        case Lexer::ICON:
        case Lexer::LISTBOX:
        case Lexer::LTEXT:
        case Lexer::PUSHBUTTON:
        case Lexer::RADIOBUTTON:
        case Lexer::RTEXT:
        case Lexer::SCROLLBAR:
        case Lexer::STATE3:
        case Lexer::CONTROL:
            return true;
        default:
            return false;
            
    }
}
void Control::ReadStandard(RCFile &rcFile, int clss , int addStyle, int extended, int hasText)
{
    addStyle |= WS_CHILD | WS_VISIBLE;
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
    if (cls == Scrollbar)
    {
        if (style & SBS_VERT)
            style &= ~SBS_HORZ;
    }
}
void Control::GetClass(RCFile &rcFile)
{
    if (rcFile.IsString())
    {
        std::wstring str = rcFile.GetString();
        std::wstring str1 = str;
        for (int i=0; i < str1.size(); i++)
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
    else switch (rcFile.GetTokenId())
    {
        case Lexer::AUTO3STATE:
        case Lexer::AUTOCHECKBOX:
        case Lexer::AUTORADIOBUTTON:
        case Lexer::CHECKBOX:
        case Lexer::PUSHBUTTON:
        case Lexer::RADIOBUTTON:
        case Lexer::DEFPUSHBUTTON:
        case Lexer::STATE3:
        case Lexer::GROUPBOX:
            cls.SetId(Button);
            break;
        case Lexer::COMBOBOX:
            cls.SetId(Combobox);
            break;
        case Lexer::CTEXT:
        case Lexer::LTEXT:
        case Lexer::RTEXT:
        case Lexer::ICON:
            cls.SetId(Static);
            break;
        case Lexer::EDITTEXT:
            cls.SetId(Edit);
            break;
        case Lexer::LISTBOX:
            cls.SetId(Listbox);
            break;
        case Lexer::SCROLLBAR:
            cls.SetId(Scrollbar);
            break;
        default:
            throw new std::runtime_error("Unknown dialog control class");
    }
}
void Control::ReadGeneric(RCFile &rcFile, bool extended)
{
    text.ReadRC(rcFile, true);
    rcFile.SkipComma();
    id = rcFile.GetNumber();
    rcFile.SkipComma();
    GetClass(rcFile);
    rcFile.SkipComma();
    style = rcFile.GetNumber() | WS_CHILD | WS_VISIBLE;
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
                           
void Control::ReadRC(RCFile &rcFile, bool extended)
{
    switch (rcFile.GetTokenId())
    {
        case Lexer::AUTO3STATE:
            ReadStandard(rcFile, Button, BS_AUTO3STATE | WS_TABSTOP, extended, 1);
            break;
        case Lexer::AUTOCHECKBOX:
            ReadStandard(rcFile, Button, BS_AUTOCHECKBOX | WS_TABSTOP, extended, 1);
            break;
        case Lexer::AUTORADIOBUTTON:
            ReadStandard(rcFile, Button, BS_AUTORADIOBUTTON, extended, 1);
            break;
        case Lexer::CHECKBOX:
            ReadStandard(rcFile, Button, BS_CHECKBOX | WS_TABSTOP, extended, 1);
            break;
        case Lexer::COMBOBOX:
            ReadStandard(rcFile, Combobox, 0, extended, 0);
            if (!(style &3))
                style |= CBS_SIMPLE;
            break;
        case Lexer::CTEXT:
            ReadStandard(rcFile, Static, SS_CENTER | WS_GROUP, extended, 1);
            break;
        case Lexer::DEFPUSHBUTTON:
            ReadStandard(rcFile, Button, BS_DEFPUSHBUTTON | WS_TABSTOP, extended, 1);
            break;
        case Lexer::EDITTEXT:
            ReadStandard(rcFile, Edit, ES_LEFT | WS_BORDER | WS_TABSTOP, extended, 0);
            break;
        case Lexer::GROUPBOX:
            ReadStandard(rcFile, Button, BS_GROUPBOX, extended, 1);
            break;
        case Lexer::ICON:
            ReadStandard(rcFile, Static, SS_ICON, extended, 1);
            break;
        case Lexer::LISTBOX:
            ReadStandard(rcFile, Listbox, LBS_NOTIFY | WS_BORDER, extended, 0);
            break;
        case Lexer::LTEXT:
            ReadStandard(rcFile, Static, SS_LEFT | WS_GROUP, extended, 1);
            break;
        case Lexer::PUSHBUTTON:
            ReadStandard(rcFile, Button, BS_PUSHBUTTON | WS_TABSTOP, extended, 1);
            break;
        case Lexer::RADIOBUTTON:
            ReadStandard(rcFile, Button, BS_RADIOBUTTON, extended, 1);
            break;
        case Lexer::RTEXT:
            ReadStandard(rcFile, Static, SS_RIGHT | WS_GROUP, extended, 1);
            break;
        case Lexer::SCROLLBAR:
            ReadStandard(rcFile, Scrollbar, SBS_HORZ, extended, 0);
            break;
        case Lexer::STATE3:
            ReadStandard(rcFile, Button, BS_3STATE | WS_TABSTOP, extended, 1);
            break;
        case Lexer::CONTROL:
            ReadGeneric(rcFile, extended);
            break;
        default:
            // shouldn't get here
            break;
    }
    rcFile.NeedEol();
}
Dialog::~Dialog()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        Control *c = *it;
        delete c;
    }
}
void Dialog::WriteRes(ResFile &resFile)
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
    int count = controls.size();
    for (iterator it = begin(); it != end(); ++it)
    {
        resFile.Align();
        (*it)->WriteRes(resFile, extended, !--count);
    }
    resFile.Release();
}
void Dialog::ReadSettings(RCFile &rcFile)
{
    bool hascaption = false, hasstyle = false, hasfont = false;
    resInfo.SetLanguage(rcFile.GetLanguage());
    bool done = FALSE;
    while (!done && rcFile.IsKeyword())
    {
        switch (rcFile.GetToken()->GetKeyword())
        {
            case Lexer::LANGUAGE:
            {
                rcFile.NextToken();
                int n = rcFile.GetNumber() ;
                rcFile.SkipComma();
                n += rcFile.GetNumber() * 1024;
                resInfo.SetLanguage(n);
            }
                break;
            case Lexer::VERSION:
                rcFile.NextToken();
                resInfo.SetVersion(rcFile.GetNumber());
                break;
            case Lexer::CHARACTERISTICS:
                rcFile.NextToken();
                resInfo.SetCharacteristics(rcFile.GetNumber());
                break;
            case Lexer::STYLE:
                hasstyle = true;
                rcFile.NextToken();
                style = rcFile.GetNumber();
                break;
            case Lexer::EXSTYLE:
                rcFile.NextToken();
                exStyle |= rcFile.GetNumber();
                break;
            case Lexer::MENU:
                rcFile.NextToken();
                menu.ReadRC(rcFile);
                break;
            case Lexer::FONT:
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
            case Lexer::CAPTION:
                hascaption = true;
                rcFile.NextToken();
                caption = rcFile.GetString();
                break;
            case Lexer::CLASS:
                rcFile.NextToken();
                cls.ReadRC(rcFile, true);
                break;
            case Lexer::HELP:
                rcFile.NextToken();
                if (!extended)
                    throw new std::runtime_error("Need DialogEx");
                helpIndex = rcFile.GetNumber();
                break;
            case Lexer::WEIGHT:
                rcFile.NextToken();
                if (!extended)
                    throw new std::runtime_error("Need DialogEx");
                weight = rcFile.GetNumber();
                break;
            case Lexer::ITALIC:
                rcFile.NextToken();
                if (!extended)
                    throw new std::runtime_error("Need DialogEx");
                italics = rcFile.GetNumber();
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            rcFile.NeedEol();
        }
    }
    if (hascaption)
        style |= WS_CAPTION;
    if (!hasstyle)
        style |= WS_POPUPWINDOW;
    if (hasfont)
        style |= DS_SETFONT;
}
void Dialog::ReadRC(RCFile &rcFile)
{
    if (extended)
    {
        italics = 256;
    }
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile,false);
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
        Control *c = new Control;
        controls.push_back(c);
        c->ReadRC(rcFile, extended);
    }
    rcFile.NeedEnd();
}
