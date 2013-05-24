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
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#include "tv.h"
#include <commctrl.h>
#include <richedit.h>
#include "InfoPad.h"
#include "editview.h"
#include <stdlib.h>
#include <sys/stat.h>

void EnhancedStatusLine::draw()
{
    TStatusLine::draw();
    ushort cNormal = getColor(0x0301);
    for (int i=0; i < count; i++)
    {
        TDrawBuffer b;
        if (i != 0)
            b.moveChar(0, '\263', cNormal, 1);
        b.moveCStr(i ? 1 : 0, items[i].text, cNormal);
        writeBuf(items[i].offset - (i ? 1 : 0), 0, (i ? 1 : 0) + strlen(items[i].text), 1, b);
    }
}
DWORD EnhancedStatusLine::WinMessage(DWORD iMessage, DWORD wParam ,DWORD lParam)
{
    switch (iMessage)
    {
        case SB_SETPARTS:
            for (int i=0; i < count; i++)
                delete items[i].text;
            delete items;
            count = wParam;
            items = new statusItem[count];
            for (int i=0; i < wParam; i++)
            {
                items[i].offset = ((int *)lParam)[i];
                items[i].text = newStr("");
            }
            break;
        case SB_SETTEXT:
            wParam &= 0x1f;
            wParam--;
            if (wParam < count)
            {
                delete items[wParam].text;
                items[wParam].text = newStr((char *)lParam);
                drawView();
            }
            break;
    }
}
