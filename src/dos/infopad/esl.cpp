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
            delete [] items;
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
