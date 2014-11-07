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
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"
#include "operands.h"
#include "opcodes.h"

typedef struct
{
    TCData ptrs;
    BYTE *olddataptr;
    BYTE *currentdataptr;
    HTREEITEM hTreeItem;
    enum
    {
        floating = 1, editable = 2
    } flags;
} REGDATA;

//-------------------------------------------------------------------------


extern HINSTANCE hInstance;
extern HWND hwndClient, hwndStatus, hwndFrame;
extern enum DebugState uState;
extern HWND hwndTab;
extern THREAD *activeThread;

HWND hwndRegister;
static CONTEXT regContext;
static HWND hwndTree;
static char szRegisterClassName[] = "xccRegisterClass";
static char szRegisterTitle[] = "";
static HANDLE registerHandle;
static CONTEXT oldContext;
static HBITMAP valueBitmap, itemBitmap;
static char *szRegTitle = "Register Window";

static char xeax[] = "EAX";
static char xebx[] = "EBX";
static char xecx[] = "ECX";
static char xedx[] = "EDX";
static char xesi[] = "ESI";
static char xedi[] = "EDI";
static char xesp[] = "ESP";
static char xebp[] = "EBP";
static char xeip[] = "EIP";
static char xst0[] = "ST0";
static char xst1[] = "ST1";
static char xst2[] = "ST2";
static char xst3[] = "ST3";
static char xst4[] = "ST4";
static char xst5[] = "ST5";
static char xst6[] = "ST6";
static char xst7[] = "ST7";
static char xeflags[] = "EFLAGS";
static char eaxbuf[20];
static char ebxbuf[20];
static char ecxbuf[20];
static char edxbuf[20];
static char esibuf[20];
static char edibuf[20];
static char espbuf[20];
static char ebpbuf[20];
static char eipbuf[20];
static char eflagsbuf[20];
static char st0buf[40];
static char st1buf[40];
static char st2buf[40];
static char st3buf[40];
static char st4buf[40];
static char st5buf[40];
static char st6buf[40];
static char st7buf[40];

static char regst[] = "Arithmetic";
static char control[] = "Control";
static char floatingname[] = "Floating";
static REGDATA regs[] = 
{
    {
        {
             &regst, 0
        }
        , 0, 0, 0, 
    }
    , 
    {
        {
             &xeax, &eaxbuf
        }
        , &oldContext.Eax, &regContext.Eax, 0, editable
    }
    , 
    {
        {
             &xebx, &ebxbuf
        }
        , &oldContext.Ebx, &regContext.Ebx, 0, editable
    }
    , 
    {
        {
             &xecx, &ecxbuf
        }
        , &oldContext.Ecx, &regContext.Ecx, 0, editable
    }
    , 
    {
        {
             &xedx, &edxbuf
        }
        , &oldContext.Edx, &regContext.Edx, 0, editable
    }
    , 
    {
        {
             &xesi, &esibuf
        }
        , &oldContext.Esi, &regContext.Esi, 0, editable
    }
    , 
    {
        {
             &xedi, &edibuf
        }
        , &oldContext.Edi, &regContext.Edi, 0, editable
    }
    , 
    {
        {
             &control, 0
        }
        , 0, 0, 0, 
    }
    , 
    {
        {
             &xesp, &espbuf
        }
        , &oldContext.Esp, &regContext.Esp, 0, editable
    }
    , 
    {
        {
             &xebp, &ebpbuf
        }
        , &oldContext.Ebp, &regContext.Ebp, 0, editable
    }
    , 
    {
        {
             &xeip, &eipbuf
        }
        , &oldContext.Eip, &regContext.Eip, 0, editable
    }
    , 
    {
        {
             &xeflags, &eflagsbuf
        }
        , &oldContext.EFlags, &regContext.EFlags, 0, editable
    }
    , 
    {
        {
             &floatingname, 0
        }
        , 0, 0, 0, 
    }
    , 
    {
        {
             &xst0, &st0buf
        }
        , &oldContext.FloatSave.RegisterArea[0],
            &regContext.FloatSave.RegisterArea[0], 0, editable | floating
    }
    , 
    {
        {
             &xst1, &st1buf
        }
        , &oldContext.FloatSave.RegisterArea[10],
            &regContext.FloatSave.RegisterArea[10], 0, editable | floating
    }
    , 
    {
        {
             &xst2, &st2buf
        }
        , &oldContext.FloatSave.RegisterArea[20],
            &regContext.FloatSave.RegisterArea[20], 0, editable | floating
    }
    , 
    {
        {
             &xst3, &st3buf
        }
        , &oldContext.FloatSave.RegisterArea[30],
            &regContext.FloatSave.RegisterArea[30], 0, editable | floating
    }
    , 
    {
        {
             &xst4, &st4buf
        }
        , &oldContext.FloatSave.RegisterArea[40],
            &regContext.FloatSave.RegisterArea[40], 0, editable | floating
    }
    , 
    {
        {
             &xst5, &st5buf
        }
        , &oldContext.FloatSave.RegisterArea[50],
            &regContext.FloatSave.RegisterArea[50], 0, editable | floating
    }
    , 
    {
        {
             &xst6, &st6buf
        }
        , &oldContext.FloatSave.RegisterArea[60],
            &regContext.FloatSave.RegisterArea[60], 0, editable | floating
    }
    , 
    {
        {
             &xst7, &st7buf
        }
        , &oldContext.FloatSave.RegisterArea[70],
            &regContext.FloatSave.RegisterArea[70], 0, editable | floating
    }
    , 
};

static TCHeader tch = 
{
    "Register", "Value", 0, 0
};
static int drawn;
void SaveRegisterContext(void)
{
    oldContext = regContext;
    activeThread->regs = regContext;
}

//-------------------------------------------------------------------------

LRESULT CALLBACK RegisterProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static int charwidth;
    HDC dc;
    TEXTMETRIC metric;
    DEBUG_EVENT *xc;
    LOGBRUSH brushstr;
    RECT r;
    int i;
    NM_TREEVIEW *nmt;
    switch (iMessage)
    {
        case WM_NOTIFY:
            nmt = (NM_TREEVIEW*)lParam;
            switch (nmt->hdr.code)
            {
            case TCN_EDITQUERY:
                if (uState != atBreakpoint)
                    return 0;
                for (i = 0; i < sizeof(regs) / sizeof(REGDATA); i++)
                    if (regs[i].hTreeItem == nmt->itemNew.hItem)
                        if (regs[i].flags &editable)
                            return (LRESULT)regs[i].ptrs.col2Text;
                return 0;
            case TCN_EDITDONE:
                for (i = 0; i < sizeof(regs) / sizeof(REGDATA); i++)
                if (regs[i].hTreeItem == nmt->itemNew.hItem)
                {
                    char *buf = (char*)nmt->itemNew.pszText;
                    if (regs[i].flags &floating)
                    {
                        float val;
                        BYTE *pos = regs[i].currentdataptr;
                        sscanf(buf, "%f", &val);
                        *(long double*)pos = val;
                        sprintf(regs[i].ptrs.col2Text, "%f", (double)val);
                    }
                    else
                    {
                        if (buf[0] == '0' && buf[1] == 'x')
                            sscanf(buf + 2, "%x", (int *)regs[i].currentdataptr);
                        else if ((buf[strlen(buf) - 1] &0xDF) == 'H')
                            sscanf(buf, "%x", (int *)regs[i].currentdataptr);
                        else
                            sscanf(buf, "%d", (int *)regs[i].currentdataptr);
                        sprintf(regs[i].ptrs.col2Text, "0x%08x", 
                            *(int *)regs[i].currentdataptr);
                    }
                    regs[i].ptrs.col2Color = 0xff;
                }
                return 0;
            }
            break;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case WM_LBUTTONDOWN:
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case ID_SETCONTEXT:
                SaveRegisterContext();
                break;
            case ID_SETADDRESS:
                regContext = activeThread->regs;
                registerHandle = (HANDLE)lParam;
                if (lParam)
                {
                    int i;
                    for (i = 0; i < sizeof(regs) / sizeof(REGDATA); i++)
                    if (regs[i].currentdataptr)
                    {
                        if (regs[i].flags &floating)
                        {
                            double val;
                            BYTE *pos = regs[i].currentdataptr;
                            val = *(long double*)pos;

                            sprintf(regs[i].ptrs.col2Text, "%f", val);
                            if (!memcmp(regs[i].currentdataptr,
                                regs[i].olddataptr, sizeof(long double)))
                                regs[i].ptrs.col2Color = 0;
                            else
                                regs[i].ptrs.col2Color = 0xff;
                        }
                        else
                        {
                            sprintf(regs[i].ptrs.col2Text, "0x%08x", 
                                *(int *)regs[i].currentdataptr);
                            if (*(int *)regs[i].currentdataptr ==  *(int *)regs[i].olddataptr)
                                regs[i].ptrs.col2Color = 0;
                            else
                                regs[i].ptrs.col2Color = 0xff;
                        }
                    }
                    if (!drawn)
                    {
                        HTREEITEM after1 = 0, after2 = 0;
                        for (i = 0; i < sizeof(regs) / sizeof(REGDATA); i++)
                        {
                            TV_INSERTSTRUCT t;
                            memset(&t, 0, sizeof(t));
                            t.hParent = regs[i].currentdataptr ? after1 : 0;
                            t.hInsertAfter = regs[i].currentdataptr ? after2 :
                                after1;
                            t.UNNAMED_UNION item.mask = 0;
                            t.UNNAMED_UNION item.hItem = 0;
                            t.UNNAMED_UNION item.lParam = (int) &regs[i].ptrs;
                            regs[i].hTreeItem = TreeView_InsertItem(hwndTree,
                                &t);
                            if (!regs[i].currentdataptr)
                            {
                                TreeView_Expand(hwndTree, after1, TVE_EXPAND);
                                after1 = regs[i].hTreeItem;
                                after2 = 0;
                            }
                            else
                                after2 = regs[i].hTreeItem;
                            
                        }
                        TreeView_Expand(hwndTree, after1, TVE_EXPAND);
                        drawn = TRUE;
                    }
                }
                else if (!lParam && drawn)
                {
                    TreeView_DeleteAllItems(hwndTree);
                    drawn = FALSE;
                }
                InvalidateRect(hwndTree, 0, 0);
                break;
            default:
                break;
            }
            break;
        case WM_ACTIVATEME:
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            break;
        case WM_KILLFOCUS:
            break;
        case WM_CREATE:
            GetClientRect(hwnd, &r);
            //         valueBitmap = LoadBitmap(hInstance, "ID_VALUEBMP") ;
            //         itemBitmap = LoadBitmap(hInstance, "ID_ITEMBMP") ;
            tch.colBmp1 = itemBitmap;
            tch.colBmp2 = valueBitmap;
            hwndTree = CreateextTreeWindow(hwnd, TCS_LINE | WS_VISIBLE, &r, &tch);
            ApplyDialogFont(hwndTree);
            break;

        case WM_CLOSE:
            break;
        case WM_DESTROY:
            DestroyWindow(hwndTree);
            //         DeleteObject(itemBitmap) ;
            //         DeleteObject(valueBitmap) ;
            hwndRegister = 0;
            registerHandle = 0;
            break;
        case WM_SIZE:
            r.left = r.top = 0;
            r.right = LOWORD(lParam);
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndTree, r.left, r.top, r.right - r.left, r.bottom -
                r.top, 0);
            break;
        case WM_INITMENUPOPUP:
            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterRegisterWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &RegisterProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szRegisterClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateRegisterWindow(void)
{
    if (hwndRegister)
    {
        SendMessage(hwndRegister, WM_SETFOCUS, 0, 0);
    }
    else
    {
        hwndRegister = CreateDockableWindow(DID_REGWND, szRegisterClassName, szRegTitle, hInstance, 200, 400);
    }
    return hwndRegister;
}
