/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
 * 
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
