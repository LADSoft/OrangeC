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
/*
 * keyword module
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <Windows.h>
#include "rc.h"

#define KWHASHSIZE 253

extern char lastid[];
extern enum e_sym lastst;

static HASHREC **hashtable = 0;

KEYWORDS keywords[] = 
{

    {
        0, "ACCELERATORS", kw_accelerator
    } , 
    {
        0, "ALT", kw_alt
    }
    , 
    {
        0, "ASCII", kw_ascii
    }
    , 
    {
        0, "AUTO3STATE", kw_auto3state
    }
    , 
    {
        0, "AUTOCHECKBOX", kw_autocheckbox
    }
    , 
    {
        0, "AUTORADIOBUTTON", kw_autoradiobutton
    }
    , 
    {
        0, "BEGIN", kw_begin
    }
    , 
    {
        0, "BITMAP", kw_bitmap
    }
    , 
    {
        0, "BLOCK", kw_block
    }
    , 
    {
        0, "CAPTION", kw_caption
    }
    , 
    {
        0, "CHARACTERISTICS", kw_characteristics
    }
    , 
    {
        0, "CHECKBOX", kw_checkbox
    }
    , 
    {
        0, "CHECKED", kw_checked
    }
    , 
    {
        0, "CLASS", kw_class
    }
    , 
    {
        0, "COMBOBOX", kw_combobox
    }
    , 
    {
        0, "CONTROL", kw_control
    }
    , 
    {
        0, "CTEXT", kw_ctext
    }
    , 
    {
        0, "CURSOR", kw_cursor
    }
    , 
    {
        0, "DEFPUSHBUTTON", kw_defpushbutton
    }
    , 
    {
        0, "DIALOG", kw_dialog
    }
    , 
    {
        0, "DIALOGEX", kw_dialogex
    }
    , 
    {
        0, "DISCARDABLE", kw_discardable
    }
    , 
    {
        0, "DLGINCLUDE", kw_dlginclude
    }
    , 
    {
        0, "EDITTEXT", kw_edittext
    }
    , 
    {
        0, "END", kw_end
    }
    , 
    {
        0, "EXSTYLE", kw_exstyle
    }
    , 
    {
        0, "FILEDATE", kw_filedate
    }
    , 
    {
        0, "FILEFLAGSMASK", kw_fileflagmask
    }
    , 
    {
        0, "FILEFLAGS", kw_fileflags
    }
    , 
    {
        0, "FILEOS", kw_fileos
    }
    , 
    {
        0, "FILESUBTYPE", kw_filesubtype
    }
    , 
    {
        0, "FILETYPE", kw_filetype
    }
    , 
    {
        0, "FILEVERSION", kw_fileversion
    }
    , 
    {
        0, "FIXED", kw_fixed
    }
    , 
    {
        0, "FONT", kw_font
    }
    , 
    {
        0, "GRAYED", kw_grayed
    }
    , 
    {
        0, "GROUPBOX", kw_groupbox
    }
    , 
    {
        0, "HELP", kw_help
    }
    , 
    {
        0, "ICON", kw_icon
    }
    , 
    {
        0, "IMPURE", kw_impure
    }
    , 
    {
        0, "INACTIVE", kw_inactive
    }
    , 
    {
        0, "ITALIC", kw_italic
    }
    , 
    {
        0, "LANGUAGE", kw_language
    }
    , 
    {
        0, "LISTBOX", kw_listbox
    }
    , 
    {
        0, "LOADONCALL", kw_loadoncall
    }
    , 
    {
        0, "LTEXT", kw_ltext
    }
    , 
    {
        0, "MENU", kw_menu
    }
    , 
    {
        0, "MENUEX", kw_menuex
    }
    , 
    {
        0, "MENUBARBREAK", kw_menubarbreak
    }
    , 
    {
        0, "MENUBREAK", kw_menubreak
    }
    , 
    {
        0, "MENUITEM", kw_menuitem
    }
    , 
    {
        0, "MESSAGETABLE", kw_messagetable
    }
    , 
    {
        0, "MOVEABLE", kw_moveable
    }
    , 
    {
        0, "NOINVERT", kw_noinvert
    }
    , 
    {
        0, "NONDISCARDABLE", kw_nondiscardable
    }
    , 
    {
        0, "NOT", kw_not
    }
    , 
    {
        0, "POPUP", kw_popup
    }
    , 
    {
        0, "PRELOAD", kw_preload
    }
    , 
    {
        0, "PRODUCTVERSION", kw_productversion
    }
    , 
    {
        0, "PURE", kw_pure
    }
    , 
    {
        0, "PUSHBUTTON", kw_pushbutton
    }
    , 
    {
        0, "RADIOBUTTON", kw_radiobutton
    }
    , 
    {
        0, "RCDATA", kw_rcdata
    }
    , 
    {
        0, "RCINCLUDE", kw_rcinclude
    }
    , 
    {
        0, "RTEXT", kw_rtext
    }
    , 
    {
        0, "SCROLLBAR", kw_scrollbar
    }
    , 
    {
        0, "SEPARATOR", kw_separator
    }
    , 
    {
        0, "SHIFT", kw_shift
    }
    , 
    {
        0, "STATE3", kw_state3
    }
    , 
    {
        0, "STRINGTABLE", kw_stringtable
    }
    , 
    {
        0, "STYLE", kw_style
    }
    , 
    {
        0, "VALUE", kw_value
    }
    , 
    {
        0, "VERSION", kw_version
    }
    , 
    {
        0, "VERSIONINFO", kw_versioninfo
    }
    , 
    {
        0, "VIRTKEY", kw_virtkey
    }
    , 
    {
        0, "WEIGHT", kw_weight
    }
    , 
    {
        0, 0, 0
    }
};
char *namefromkw(int st)
{
    KEYWORDS *s = keywords;
    while (s->word)
    {
        if (s->stype == st)
            return s->word;
        s++;
    }
    return 0;
}

//-------------------------------------------------------------------------

void kwini(void)
/*
 * create a keyword hash table
 */
{
    struct kwblk *q = keywords;
    if (!hashtable)
    {
        hashtable = (HASHREC*)malloc(KWHASHSIZE *sizeof(HASHREC*));
        memset(hashtable, 0, KWHASHSIZE *sizeof(HASHREC*));
        while (q->word)
        {
            AddHash(q, hashtable, KWHASHSIZE);
            q++;
        } 
    }
}

//-------------------------------------------------------------------------

int searchkw(void)
/*
 * see if the current symbol is a keyword
 */
{
    char buf[256];
    int i;
    struct kwblk **kwbp;
    for (i = 0; i < strlen(lastid) + 1; i++)
        buf[i] = toupper(lastid[i]);
    if (lastst != ident)
        return 0;
    kwbp = LookupHash(buf, hashtable, KWHASHSIZE);

    if (kwbp)
    {
        return lastst = (*kwbp)->stype;
    } return (0);
}
