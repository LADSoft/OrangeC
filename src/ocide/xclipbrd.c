/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

// assumes tabs aren't going to get reset yet
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"
#include <richedit.h>
#include <limits.h>
#include "c_types.h"
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include "symtypes.h"
/**********************************************************************
 * SelToClipboard copies the current selection to the clipboard
 **********************************************************************/
void SelToClipboard(HWND hwnd, EDITDATA* p)
{
    int start, end, lf = 0, i;
    if (p->selendcharpos - p->selstartcharpos < 0)
    {
        end = p->selstartcharpos;
        start = p->selendcharpos;
    }
    else
    {
        end = p->selendcharpos;
        start = p->selstartcharpos;
    }
    if (end == start)
        return;
    for (i = start; i < end; i++)
        if (p->cd->text[i].ch == '\n')
            lf++;
    if (OpenClipboard(hwnd))
    {
        HGLOBAL glmem = GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, end - start + 1 + lf);
        if (glmem != NULL)
        {
            char *data = GlobalLock(glmem), *q = data;
            for (i = start; i < end; i++)
            {
                if (p->cd->text[i].ch == '\n')
                    *q++ = '\r';
                *q++ = p->cd->text[i].ch;
            }
            *q++ = 0;
            GlobalUnlock(data);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, glmem);
        }
        CloseClipboard();
    }
}
/**********************************************************************
 * ClipboardToSel pastes the clipboard into the text
 **********************************************************************/
void ClipboardToSel(HWND hwnd, EDITDATA* ptr)
{
    if (!IsClipboardFormatAvailable(CF_TEXT))
        return;
    if (OpenClipboard(hwnd))
    {
        HANDLE clh = GetClipboardData(CF_TEXT);
        char* data = GlobalLock(clh);
        int l = GlobalSize(clh);
        if (l)
        {
            char *mem = calloc(1, l), *p = mem, *q = mem;
            if (mem)
            {
                memcpy(mem, data, l);
                mem[l - 1] = 0;
                while (*p)
                    if (*p == '\r')
                        p++;
                    else
                        *q++ = *p++;
                *q = 0;
                if (mem[0])
                {
                    Replace(hwnd, ptr, mem, q - mem);
                    ScrollCaretIntoView(hwnd, ptr, FALSE);
                    FormatBufferFromScratch(ptr->colorizeEntries, ptr->cd->text, ptr->selstartcharpos - 1, ptr->selendcharpos + 1,
                                            ptr->cd->language, ptr->cd->defbackground);
                    SendUpdate(hwnd);
                    ptr->cd->sendchangeonpaint = TRUE;
                    ptr->selendcharpos = ptr->selstartcharpos = ptr->selstartcharpos + strlen(mem);
                    MoveCaret(hwnd, ptr);
                    InvalidateRect(hwnd, 0, 0);
                }
                free(mem);
            }
        }
        GlobalUnlock(data);
        CloseClipboard();
    }
}
