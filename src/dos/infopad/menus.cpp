/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
#define Uses_TMenuBar
#define Uses_TMenu
#define Uses_TSubMenu
#define Uses_TKeys
#include "tv.h"
#include "InfoPad.h"
#include "IPConfig.h"
   
#define MENU(p, id)  *new TSubMenu(p, id) +
#define ENDMENU
#define SUBMENU(p, id, sub) *new TMenuItem(p, id, new TMenu(*sub))

#define ITEM(p, id, key) *new TMenuItem(p, id, key)

#define NOACCITEM(p, id) *new TMenuItem(p, id, 0, hcNoContext, NULL, NULL)
#define SEPARATOR newLine()

extern InfoPadCfg cfg;
extern MRU WindowMRU;

TMenuItem *FileMRUMenu;
TMenuItem *WinMRUMenu;

TMenuItem& operator + ( TMenuItem& s, TMenuItem& i )
{
    TMenuItem *t = &s;
    while (t->next)
        t = t->next;
    t->next = &i;
    return s;
}

TMenuBar *InfoPad::initMenuBar( TRect bounds )
{
    bounds.b.y = 1;
    FileMRUMenu = new TMenuItem("~R~eopen", 0, new TMenu(SEPARATOR));
    WinMRUMenu =  &(NOACCITEM(  "~C~lose all",  ID_CLOSEALL) +
            SEPARATOR);

    cfg.FileMRU().ToMenu(FileMRUMenu);
    WindowMRU.ToMenu(WinMRUMenu, &NOACCITEM(  "~M~ore Windows...", ID_MOREWINDOWS));
    return new TMenuBar(bounds, 
        MENU( "~F~ile",  kbAltF)
            ITEM( "~N~ew",        ID_NEW,          kbCtrlN) +
            NOACCITEM( "~O~pen...",    ID_OPEN) +
            *FileMRUMenu +
            SEPARATOR +
            ITEM( "~S~ave",       ID_SAVE,         kbCtrlS) +
            NOACCITEM( "Save ~a~s...", ID_SAVEAS) +
            NOACCITEM( "~C~lose",     ID_CLOSE) +
            SEPARATOR +
            NOACCITEM( "~P~rint",      ID_PRINT) +
            NOACCITEM( "Prin~t~er setup...", ID_PRINTSETUP) +
            SEPARATOR +
            ITEM( "Op~t~ions",        ID_OPTIONS,          kbAltO) +
            NOACCITEM( "~D~OS",        ID_DOS) +
            ITEM( "E~x~it",       cmQuit,     kbAltX)
        ENDMENU +
        MENU( "~E~dit", kbAltE)
            ITEM( "~U~ndo",      ID_UNDO,  kbCtrlZ) +
            SEPARATOR +
            ITEM( "Cu~t~",       ID_CUT,   kbCtrlX) +
            ITEM( "~C~opy",      ID_COPY,  kbCtrlC) +
            ITEM( "~P~aste",     ID_PASTE, kbCtrlV) +
            SEPARATOR +
    		NOACCITEM( "To~u~pper",   ID_TOUPPER) +
    		NOACCITEM( "To~l~ower",   ID_TOLOWER) +
    		NOACCITEM( "~I~ndent Selection", ID_INDENT) +
    		NOACCITEM( "~O~utdent Selection", ID_OUTDENT) +
    		NOACCITEM( "Co~m~ment Selection", ID_COMMENT) +
    		NOACCITEM( "U~n~comment Selection", ID_UNCOMMENT)
        ENDMENU +
        MENU( "~S~earch", kbAltS)
            ITEM( "~S~earch...", ID_SEARCH,      kbCtrlF) +
            ITEM( "~R~eplace...",ID_REPLACE,     kbCtrlH) +
            ITEM( "Find ~N~ext",      ID_SEARCHNEXT,  kbF3) +
            SEPARATOR +
            ITEM( "~G~oto", ID_GOTO, kbCtrlG)
        ENDMENU +
        MENU( "~W~indow", kbAltW)
            *WinMRUMenu
        ENDMENU +
        MENU( "~H~elp", kbAltH)
            NOACCITEM(  "~A~bout...",          ID_ABOUT)
        ENDMENU
    );
}
