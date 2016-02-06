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
#define STRICT 
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "header.h"

extern HINSTANCE hInstance;
extern HWND hwndTbFind;
extern int modifiedFind;


static int wndoffs;
static WNDPROC oldComboProc;
static WNDPROC oldEditProc;

static char *propname = "HISTBUF";

LRESULT CALLBACK historyEditComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_RETURN:
                case VK_ESCAPE:
                    SendMessage(GetParent(hwnd), WM_COMMAND, (wParam << 16) + 1698, (LPARAM)hwnd);
                    return 0;
            }
            break;
        case WM_KEYUP:
            if (hwnd == hwndTbFind)
                modifiedFind = TRUE;
            switch(wParam)
            {
                case VK_RETURN:
                case VK_ESCAPE:
                    return 0;
            }
            break;
        default:
            break;
    }
    return CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
}

LRESULT CALLBACK historyComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    char **cbptr;
    char buf[MAX_PATH];
    int i;
    POINT pt;
    HWND hwndedit;
    pt.x = pt.y = 1;
    hwndedit = ChildWindowFromPoint(hwnd, pt);
    switch (iMessage)
    {
        case EM_GETMODIFY:
            return SendMessage(hwndedit, EM_GETMODIFY, wParam, lParam);
        case EM_SETMODIFY:
            return SendMessage(hwndedit, EM_SETMODIFY, wParam, lParam);
            
        case WM_SETMODIFY:
            return SendMessage(hwndedit, WM_SETTEXT, 0, (LPARAM)" "); 
                // hack to fix a timing varagary that kept the find button disabled even though text was changing
        case WM_COMMAND:
            if (LOWORD(wParam) == 1698)
            {
                // return or escape key pressed in edit box...
                PostMessage((HWND)GetWindowLong(hwnd, GWL_HWNDPARENT), WM_COMMAND, (4000 + HIWORD(wParam)) , (LPARAM)hwnd);
            }
            //         if ((wParam >> 16) == EN_CHANGE || (wParam >> 16) == EN_UPDATE)
            //            return SendMessage(GetParent(hwnd),WM_COMMAND,(wParam & 0xffff0000) + GetWindowLong(hwnd,GWL_ID), (LPARAM)hwnd) ;
            break;
        case WM_SETHISTORY:
            cbptr = (char **)lParam;
            SetProp(hwnd, propname, cbptr);
            for (i = 0; i < MAX_COMBO_HISTORY; i++)
            if (cbptr[i])
            {
                SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)cbptr[i]);
            }
            SendMessage(hwnd, CB_SETCURSEL, 0, 0);

            return (0);
        case WM_SAVEHISTORY:
            cbptr = GetProp(hwnd, propname);
            if (cbptr)
            {
                int sel; // = SendMessage(hwnd, CB_GETCURSEL,0,0) ;
                buf[SendMessage(hwndedit, WM_GETTEXT, MAX_PATH, (LPARAM)buf)] = 0;
                sel = SendMessage(hwnd, CB_FINDSTRINGEXACT, 0, (LPARAM)buf);
                if (sel == CB_ERR)
                {
                    if (buf[0])
                    {
                        if (cbptr[MAX_COMBO_HISTORY - 1])
                        {
                            free(cbptr[MAX_COMBO_HISTORY - 1]);
                        }
                        memmove(cbptr + 1, cbptr, (MAX_COMBO_HISTORY - 1)
                            *sizeof(char*));
                        cbptr[0] = strdup(buf);
                        SendMessage(hwnd, CB_INSERTSTRING, 0, (LPARAM)buf);
                        //                  SendMessage(hwnd,CB_SETCURSEL,-1,0 );
                        //                  SendMessage(hwndedit,WM_SETTEXT,0,(LPARAM)buf) ;
                    }
                }
                else
                {
                    char *s = cbptr[sel];
                    memmove(cbptr + 1, cbptr, sel *sizeof(char*));
                    cbptr[0] = s;
                }
            }
            return 0;
        case WM_DESTROY:
            RemoveProp(hwnd, propname);
            break;
            
    }
    return CallWindowProc(oldComboProc, hwnd, iMessage, wParam, lParam);

}

//-------------------------------------------------------------------------

void SubClassHistoryCombo(HWND combo)
{
    POINT pt;
    HWND editWnd;
    pt.x = 5;
    pt.y = 5;
    editWnd = ChildWindowFromPoint(combo, pt);
    SetWindowLong(combo, GWL_WNDPROC, (int)historyComboProc);
    SetWindowLong(editWnd, GWL_WNDPROC, (int)historyEditComboProc);
}


//-------------------------------------------------------------------------

void RegisterHistoryComboWindow(void)
{
    WNDCLASS wc;
    GetClassInfo(0, "combobox", &wc);
    oldComboProc = wc.lpfnWndProc;
    GetClassInfo(0, "edit", &wc);
    oldEditProc = wc.lpfnWndProc;
    return ;
    wc.lpfnWndProc = &historyComboProc;
    wc.lpszClassName = "historycombo";
    wc.hInstance = hInstance;
    wc.cbWndExtra += (4-wc.cbWndExtra % 4);
    wndoffs = wc.cbWndExtra;
    wc.cbWndExtra += 4;
    RegisterClass(&wc);
}
