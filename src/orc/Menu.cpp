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
#include "Menu.h"
#include "RCFile.h"
#include "ResFile.h"
#include <fstream>
#include <exception>

MenuItem::~MenuItem()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        MenuItem *item = *it;
        delete item;
    }
}
void MenuItem::WriteRes(ResFile &resFile, bool ex, bool last)
{
    if (!ex)
    {
        int fl = type;
        if (popup.size())
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
            for (iterator it = begin(); it != end(); ++it)
                (*it)->WriteRes(resFile, ex, !--count);
        }
    }
    else
    {
        resFile.Align();
        resFile.WriteDWord(type);
        resFile.WriteDWord(state);
        resFile.WriteDWord(id);
        int fl = 0;
        if (popup.size())
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
            for (iterator it = begin(); it != end(); ++it)
                (*it)->WriteRes(resFile, ex, !--count);
        }
    }
}
void MenuItem::GetFlags(RCFile &rcFile)
{
    bool done = false;
    while (!done)
    {
        switch (rcFile.GetToken()->GetKeyword())
        {
            case Lexer::GRAYED:
                type |= Grayed;
                break;
            case Lexer::INACTIVE:
                type |= Inactive;
                break;
            case Lexer::CHECKED:
                type |= Checked;
                break;
            case Lexer::MENUBARBREAK:
                type |= MenuBarBreak;
                break;
            case Lexer::MENUBREAK:
                type |= MenuBreak;
                break;
            case Lexer::HELP:
                type |= Help;
                break;
            case Lexer::SEPARATOR:
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
bool MenuItem::ReadRCInternal(RCFile &rcFile, bool ex)
{
    bool rv = false;
    if (rcFile.IsKeyword())
        switch(rcFile.GetToken()->GetKeyword())
        {
            case Lexer::MENUITEM:
                {
                    rcFile.NextToken();
                    if (rcFile.IsKeyword())
                    {
                        if (rcFile.GetTokenId() != Lexer::SEPARATOR)
                            throw new std::runtime_error("Invalid menu type");
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
            case Lexer::POPUP:
                {
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
void MenuItem::ReadRC(RCFile &rcFile, MenuItemList &list, bool ex)
{
    rcFile.NeedBegin();
    bool done = false;
    while (!done)
    {
        
        MenuItem *item = new MenuItem;
        if (!(done = item->ReadRCInternal(rcFile, ex)))
            list.push_back(item);
        else
            delete item;
    }
    rcFile.NeedEnd();
}
Menu::~Menu()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        MenuItem *item = *it;
        delete item;
    }
}
void Menu::WriteRes(ResFile &resFile)
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
    for (iterator it = begin(); it != end(); ++it)
        (*it)->WriteRes(resFile, extended, !--count);
    resFile.Release();
}
void Menu::ReadRC(RCFile &rcFile)
{
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, true);
    MenuItem::ReadRC(rcFile, menuItems, extended);
}
