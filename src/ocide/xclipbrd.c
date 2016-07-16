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
// assumes tabs aren't going to get reset yet
#define STRICT 
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
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
    void SelToClipboard(HWND hwnd, EDITDATA *p)
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
            return ;
        for (i = start; i < end; i++)
            if (p->cd->text[i].ch == '\n')
                lf++;
        if (OpenClipboard(hwnd))
        {
            HGLOBAL glmem = GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, end -
                start + 1+lf);
            if (glmem != NULL)
            {
                char *data = GlobalLock(glmem),  *q = data;
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
    void ClipboardToSel(HWND hwnd, EDITDATA *ptr)
    {
        if (!IsClipboardFormatAvailable(CF_TEXT))
            return ;
        if (OpenClipboard(hwnd))
        {
            HANDLE clh = GetClipboardData(CF_TEXT);
            char *data = GlobalLock(clh);
            int l = GlobalSize(clh);
            if (l)
            {
                char *mem = calloc(1,l),  *p = mem,  *q = mem;
                if (mem)
                {
                    memcpy(mem, data, l);
                    mem[l - 1] = 0;
                    while (*p)
                        if (*p == '\r')
                            p++;
                        else
                            *q++ =  *p++;
                    *q = 0;
                    if (mem[0])
                    {
                        Replace(hwnd, ptr, mem, q - mem);
                        ScrollCaretIntoView(hwnd, ptr, FALSE);
                        FormatBufferFromScratch(ptr->colorizeEntries, ptr->cd->text, ptr
                            ->selstartcharpos - 1, ptr->selendcharpos + 1,
                            ptr->cd->language, ptr->cd->defbackground);
                        SendUpdate(hwnd);
                        ptr->cd->sendchangeonpaint = TRUE;
                        ptr->selendcharpos = ptr->selstartcharpos = ptr
                            ->selstartcharpos + strlen(mem);
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
