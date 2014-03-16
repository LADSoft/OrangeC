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
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "winconst.h"
#define MIN_WIDTH 30
#define DEFAULT_WIDTH 230
#define DEFAULT_HEIGHT 160
#define TITLE_ALIAS 25

#define MIN_SIZE (21)

#define SRCTABHEIGHT 28

#define TRANSPARENT_COLOR 0xffffff

extern HWND hwndSrcTab;
extern HWND hwndJumpList;
extern HINSTANCE hInstance;
extern int defaultWorkArea;

static char *szFocusClassName = "xccFocusRect";
static CCW_params **cache;
static CCD_params **docks;
static CCW_params *frames[30];
static CCW_params *blanks[30];
static int frameCount, blankCount;
static int max, currentWindows;
static HWND hwndFrame, hwndClient;
static RECT moverect;
static POINT movept;
static int curstype;
static int moving = 0;
static RECT lastbound;
static int drawnbound;
static int suggestedDock, lastSuggestedDock;
static HWND sbwnd;
static int sbheight;
static int rundown;
static HWND hwndFocus;
int dock(CCW_params *p, RECT *r);
CCD_params *FindParams(CCW_params *p, int *index);
void CalculateMoveableDocks(CCW_params *p, RECT *r, POINT *pt);
void DoDocks(CCW_params *p);
void CalculateLayout(int index, int recalhidden);
void CalculateSizebarBounds(CCW_params *p, RECT *dest);
void Resize(CCW_params *p, RECT *new);
void PutWindow(HWND hwnd, RECT *r);
void CalculateHidden(CCD_params *d, int index, int state);
void InsertVertFrame(int index, int left, RECT *fullframe);
void InsertHorizFrame(int index, int top, int fullframe);

CCD_params *FindParams(CCW_params *p, int *index)
{
    int i;
    for (i = 0; i < currentWindows; i++)
    if (cache[i] == p)
    {
        if (index)
            *index = i;
        return docks[i];
    }

    return NULL;
}

#define SPI_GETFOCUSBORDERWIDTH 0x200e
#define SPI_GETFOCUSBORDERHEIGHT 0x2010
static LRESULT CALLBACK FocusWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    HDC dc;
    HBRUSH brush;
    HPEN pen;
    PAINTSTRUCT ps;
    RECT rect;
    int x,y;
    switch (iMessage)
    {
        case WM_CREATE:
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
            break;
        case WM_PAINT:
            GetClientRect(hwnd, &rect);
            SystemParametersInfo(SPI_GETFOCUSBORDERWIDTH, NULL, &x, 0);
            SystemParametersInfo(SPI_GETFOCUSBORDERHEIGHT, NULL, &y, 0);
            dc = BeginPaint(hwnd, &ps);
            brush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(dc, &rect, brush);
            DeleteObject(brush);
            DrawFocusRect(dc, &rect);
            EndPaint(hwnd, &ps);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------

void DrawBoundingRect(RECT *r1)
{
    SetWindowPos(hwndFocus, HWND_TOPMOST, r1->left, r1->top, r1->right - r1->left, r1->bottom - r1->top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOCOPYBITS);
    lastbound =  *r1;
    drawnbound = TRUE;
}
void HideBoundingRect(void)
{
    ShowWindow(hwndFocus, SW_HIDE);
    drawnbound = FALSE;
}
//-------------------------------------------------------------------------

void GetRelativeRect(HWND parent, HWND self, RECT *r)
{
    POINT pt;
    GetWindowRect(self, r);
    pt.x = r->left;
    pt.y = r->top;
    ScreenToClient(parent, &pt);
    r->bottom = r->bottom - r->top + pt.y;
    r->right = r->right - r->left + pt.x;
    r->left = pt.x;
    r->top = pt.y;
}

//-------------------------------------------------------------------------

void GetFrameWindowRect(RECT *r)
{
    GetWindowRect(hwndFrame, r);
    r->top += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) +
        GetSystemMetrics(SM_CYFRAME);
    r->bottom -= GetSystemMetrics(SM_CYFRAME);
    r->left += GetSystemMetrics(SM_CXFRAME);
    r->right -= GetSystemMetrics(SM_CXFRAME);

    r->bottom -= sbheight;
}

//-------------------------------------------------------------------------

void AllocContainer(CCW_params *p, RECT *r)
{
    char buf[256];
    RECT cr;
    CCW_params *ccw = CreateContainerWindow(hwndClient, p, r);
    FreeParent(p);
    ccw->child = p;
    p->parent = ccw; 
    SetParent(p->hwnd, ccw->hwnd);
    if (p->type != LSTOOLBAR)
    {
        SendMessage(p->hwnd, LCF_CONTAINER, 0, 1);
        GetClientRect(ccw->hwnd, &cr);
        MoveWindow(p->hwnd, 0,0, cr.right, cr.bottom, 1);
    }
    else
    {
        SendMessage(p->hwnd, LCF_SETVERTICAL, 0, 0);
        MoveWindow(ccw->hwnd, r->left, r->top, p->u.tb.hsize.cx +
                   GetSystemMetrics(SM_CXFRAME)*2, p->u.tb.hsize.cy +
                   GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION), 1);
    }
    ShowWindow(p->hwnd, SW_SHOW);
}

//-------------------------------------------------------------------------

void FreeParent(CCW_params *ps)
{
    int i;
    CCW_params *p = ps->parent;
    if (!p)
        return ;
    SetParent(ps->hwnd, hwndFrame);
    ps->parent = NULL;
    p->child = NULL;
    if (p->type == LSCONTAINER)
    {
        SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM)p->hwnd, NULL);
    }
    else
    {
        ShowWindow(p->hwnd, SW_HIDE);
        DestroyWindow(p->hwnd);
    }
}

//-------------------------------------------------------------------------

void AllocGrip(CCW_params *p, RECT *r, int vertical, int reshow, int hidden)
{
    FreeParent(p);
    if (p->type == LSTOOLBAR)
    {
        CCW_params *ccw = CreateGripWindow(hwndFrame);
        ccw->child = p;
        p->parent = ccw;
        SetParent(p->hwnd, ccw->hwnd);
        if (vertical)
            MoveWindow(ccw->hwnd, r->left, r->top, p->u.tb.vsize.cx + 2*GetSystemMetrics(SM_CXBORDER), p
                ->u.tb.vsize.cy + GRIPWIDTH + GetSystemMetrics(SM_CYFRAME), 1);
        else
            MoveWindow(ccw->hwnd, r->left, r->top, p->u.tb.hsize.cx +
                GRIPWIDTH + GetSystemMetrics(SM_CXFRAME), p->u.tb.hsize.cy+2*GetSystemMetrics(SM_CYBORDER), 1);
        if (reshow)
        {
            ShowWindow(p->hwnd, SW_SHOW);
            ShowWindow(ccw->hwnd, SW_SHOW);
            InvalidateRect(ccw->hwnd, NULL, 0);
        }
        SendMessage(p->hwnd, LCF_SETVERTICAL, GRIPWIDTH, vertical);
        SendMessage(ccw->hwnd, LCF_SETVERTICAL, 0, vertical);
    }
    else
    {
        CCW_params *ccw = CreateControlWindow(hwndFrame);
        ccw->child = p;
        p->parent = ccw;
        SetParent(p->hwnd, ccw->hwnd);
        SendMessage(p->hwnd, LCF_SETVERTICAL, 0, vertical);
        SendMessage(ccw->hwnd, LCF_SETVERTICAL, 0, vertical);
        if (vertical)
            MoveWindow(ccw->hwnd, r->left, r->top, 
                r->right-r->left + EDGEWIDTH + TITLEWIDTH, r->bottom-r->top + EDGEWIDTH * 2, 1);
        else
            MoveWindow(ccw->hwnd, r->left, r->top, r->right-r->left + 2 * EDGEWIDTH,
                r->bottom-r->top + EDGEWIDTH + TITLEWIDTH, 1);
        if (reshow)
        {
            ShowWindow(p->hwnd, SW_SHOW);
            ShowWindow(ccw->hwnd, SW_SHOW);
            InvalidateRect(ccw->hwnd, NULL, 0);
        }
    }
}

//-------------------------------------------------------------------------

void AllocBlank(RECT *r, CCD_params *d, int vertical)
{
    CCW_params *ccw = CreateBlankWindow(hwndFrame);
    blanks[blankCount++] = ccw;
    ccw->u.bw.position =  *r;
    ccw->u.bw.representative_dock = d;
}

//-------------------------------------------------------------------------

void dmgrAddStatusBar(HWND sb)
{
    RECT r;
    sbwnd = sb;

    GetWindowRect(sb, &r);
    sbheight = r.bottom - r.top;
}

//-------------------------------------------------------------------------

void GetClientWindowRect(RECT *r)
{
    GetWindowRect(hwndClient, r);
    r->top -= SRCTABHEIGHT;
    if (IsWindowVisible(hwndJumpList))
        r->top -= SendMessage(hwndJumpList, WM_GETHEIGHT, 0, 0);
}

//-------------------------------------------------------------------------

void dmgrInit(HINSTANCE hInstance, HWND frame, HWND client, int count)
{
    RECT r;
    int i;
    static BOOL registered;

    if (!registered)
    {
        WNDCLASS wc;
        registered = TRUE;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = FocusWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = 0; 
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = szFocusClassName;
        RegisterClass(&wc);
    }
    hwndFocus = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, 
                          szFocusClassName, "", WS_CLIPSIBLINGS | WS_POPUP,
                          0,0,10,10, hwndFrame, 0, hInstance, 0);
    hwndFrame = frame;
    hwndClient = client;
    cache = calloc(1, count *sizeof(CCW_params*));
    if (cache)
    {
        docks = calloc(1, count *sizeof(CCD_params*));
        if (!docks)
        {
            free(cache);
            cache = 0;
        }
    }
    if (cache)
        max = count;
        
}

//-------------------------------------------------------------------------

HWND CreateDockableWindow(int id, char *szClass, char *szTitle, HINSTANCE hInstance, int cx, int cy)
{
    HWND rv = NULL;
    if (currentWindows < max)
    {
        CCW_params *ccw = calloc(sizeof(CCW_params), 1);
        CCD_params *ccd = calloc(sizeof(CCD_params), 1);
        if (!ccw || !ccd)
        {
            free(ccw);
            free(ccd);
        }
        else
        {
            rv = CreateWindow(szClass, szTitle, WS_CHILD +
                        WS_CLIPCHILDREN + 
                        WS_CLIPSIBLINGS /*+ WS_BORDER*/, 
                        0, 0, cx, cy,
                        hwndFrame, 0, hInstance, 0);
            ccw->id = id;
            ccw->type = LSREGULAR;
            ccw->hwnd = rv;
            ccd->hidden = TRUE;
            ccd->position.left = 0;
            ccd->position.right = cx;
            ccd->position.top = 0;
            ccd->position.bottom = cy;
            ccd->lastposition = ccd->position;
            ccd->oldsize = ccd->lastposition;
            cache[currentWindows] = ccw;
            docks[currentWindows++] = ccd;
            AllocGrip(ccw, &ccd->position, FALSE, FALSE, ccd->hidden);
            if (!ccd->hidden)
                ShowWindow(ccw->hwnd, SW_SHOW);
        }
    }
    return rv;
}
void AddDockableToolbarWindow(CCW_params *p)
{
    if (currentWindows < max)
    {
        CCD_params *ccd = calloc(sizeof(CCD_params), 1);
        if (ccd)
        {
            RECT r;
            GetWindowRect(p->hwnd, &r);
            ccd->hidden = TRUE;
            ccd->position.left = 0;
            ccd->position.right = r.right - r.left;;
            ccd->position.top = 0;
            ccd->position.bottom = r.bottom - r.top;
            ccd->lastposition = ccd->position;
            ccd->oldsize = ccd->lastposition;
            cache[currentWindows] = p;
            docks[currentWindows++] = ccd;
            AllocGrip(p, &ccd->position, FALSE, FALSE, ccd->hidden);
        }
    }
}

//-------------------------------------------------------------------------

void DestroyDockableWindow(HWND hwnd)
{
    int found = -1;
    int i;
    CCD_params *d;
    CCW_params *w;
    for (i = 0; i < currentWindows; i++)
    {
        GetWindowRect(cache[i]->parent->hwnd, &docks[i]->position);
        if (cache[i]->hwnd == hwnd)
            found = i;
    }
    if (found)
    {
        free(docks[found]);
        FreeParent(cache[found]);
        memmove(&docks[found], &docks[found + 1], sizeof(CCD_params*) * currentWindows 
            - found - 1);
        memmove(&cache[found], &cache[found + 1], sizeof(CCW_params*) * currentWindows 
            - found - 1);
        currentWindows--;
        CalculateLayout( - 1, 0);
    }
}
//-------------------------------------------------------------------------

void dmgrStartMoveClient(CCW_params *p, POINT *cursor)
{
    RECT r;
    int i;
    CCD_params *d;
    p = p->child;
    d = FindParams(p, 0);
    GetFrameWindowRect(&r);
    ClipCursor(&r);

    movept = *cursor;

    for (i = 0; i < currentWindows; i++)
    {
        GetWindowRect(cache[i]->parent->hwnd, &docks[i]->position);
        if (!docks[i]->flags)
            docks[i]->oldsize = docks[i]->position;
    }

    moverect = d->oldsize;
    OffsetRect(&moverect,  - moverect.left,  - moverect.top);
    OffsetRect(&moverect, d->position.left, d->position.top);

    moving = 1;
    DrawBoundingRect(&moverect);
    lastSuggestedDock = d->flags &~DOCK_PRESENT;
}

//-------------------------------------------------------------------------

void dmgrMoveClient(CCW_params *p, POINT *cursor, int grip)
{
    RECT r = moverect;
    RECT r1;
    int newcurs = 0;
    p = p->child;
    OffsetRect(&r, cursor->x - movept.x, cursor->y - movept.y);
    CalculateMoveableDocks(p, &r, cursor);
    DrawBoundingRect(&r);

}

//-------------------------------------------------------------------------

void dmgrEndMoveClient(CCW_params *p, POINT *cursor)
{
    int i;
    if (moving == 0)
        return ;
    p = p->child;
    if (drawnbound)
    {
        HideBoundingRect();
        if (moving)
        {
            DoDocks(p);
        }
        else
            PutWindow(p->parent->hwnd, &lastbound);
    }
    moving = 0;
    ClipCursor(0);
}

//-------------------------------------------------------------------------

int dmgrStartMoveGrip(CCW_params *p, POINT *cursor)
{
    dmgrStartMoveClient(p, cursor);
    dmgrMoveClient(p, cursor, TRUE);
}

//-------------------------------------------------------------------------

int dmgrMoveGrip(CCW_params *p, POINT *cursor)
{
    dmgrMoveClient(p, cursor, TRUE);
}

//-------------------------------------------------------------------------

int dmgrEndMoveGrip(CCW_params *p, POINT *cursor)
{
    dmgrEndMoveClient(p, cursor);
}

//-------------------------------------------------------------------------

int dmgrDocked(CCW_params *p)
{
    CCD_params *d = FindParams(p, 0);
    if (d)
    {
        return !!(d->flags &DOCK_PRESENT);
    }
    return FALSE;
}

//-------------------------------------------------------------------------

int dmgrSizeBarStartMove(CCW_params *p, POINT *cursor)
{
    int i;
    RECT r;
    GetWindowRect(p->hwnd, &moverect);
    movept = *cursor;
    HideBoundingRect();
    for (i = 0; i < frameCount; i++)
    {
        GetWindowRect(frames[i]->hwnd, &frames[i]->u.fw.position);
    }
    for (i = 0; i < currentWindows; i++)
    {
        GetWindowRect(cache[i]->parent->hwnd, &docks[i]->position);
    }
    CalculateSizebarBounds(p, &r);
    ClipCursor(&r);
    DrawBoundingRect(&p->u.fw.position);
}

//-------------------------------------------------------------------------

int dmgrSizeBarMove(CCW_params *p, POINT *cursor)
{
    RECT r = moverect;

    if (!p->vertical)
        OffsetRect(&r, 0, cursor->y - movept.y);
    else
        OffsetRect(&r, cursor->x - movept.x, 0);
    DrawBoundingRect(&r);
}

//-------------------------------------------------------------------------

int dmgrSizeBarEndMove(CCW_params *p)
{
    ClipCursor(0);

    Resize(p, &lastbound);
    HideBoundingRect();
}

//-------------------------------------------------------------------------

int dmgrFlex(CCW_params *p)
{
    int index;
    CCD_params *d;
    d = FindParams(p, &index);
    d->flexparams |= p->u.cw.flexed ? FLEX_UNREQ : FLEX_REQ;
    CalculateLayout( - 1, 0);
}

//-------------------------------------------------------------------------

int dmgrSizeFrame(void)
{
#ifdef XXXXX
    for (i = 0; i < currentWindows; i++)
    {
        GetWindowRect(cache[i]->parent->hwnd, &docks[i]->position);
    }
#endif
    CalculateLayout( - 1, 0);
}

//-------------------------------------------------------------------------

int dmgrGetHiddenState(int id)
{
    int i;
    for (i = 0; i < currentWindows; i++)
        if (cache[i]->id == id)
            return docks[i]->hidden;
    return FALSE;
}

//-------------------------------------------------------------------------

int dmgrHideWindow(int id, int state)
{

    int oldstate = 0;
    CCW_params *p = 0;
    CCD_params *d = 0;
    int i, index;
    for (i = 0; i < currentWindows; i++)
    {
        if (cache[i]->id == id)
        {
            p = cache[i];
            d = docks[i];
            index = i;
            break;
        }
    }
    if (p)
    {
        oldstate = !d->hidden;
        if (state != d->hidden)
        {
            d->hidden = state;
            if (d->flags && p->type != LSTOOLBAR)
                CalculateHidden(d, index, state);
            if (d->hidden || d->flags)
            {
                AllocGrip(p, &d->position, p->vertical, !d->hidden, d->hidden);
            }
            else
            {
                AllocContainer(p, &d->position);
            }
            if (d->flags)
                CalculateLayout( - 1, 0);
        }
    }
    return oldstate;
}
//-------------------------------------------------------------------------

void PutWindow(HWND hwnd, RECT *r)
{
    RECT r1, r2 =  *r;
    if (GetParent(hwnd) == hwndClient)
    {
        GetWindowRect(hwndClient, &r1);
        OffsetRect(&r2,  - r1.left,  - r1.top);
    }
    else
    {
        GetFrameWindowRect(&r1);
        OffsetRect(&r2,  - r1.left,  - r1.top);
    }
    InvalidateRect(hwnd, 0, 0);
    MoveWindow(hwnd, r2.left, r2.top, r2.right - r2.left, r2.bottom - r2.top, 1)
        ;
}

//-------------------------------------------------------------------------

#define HYSTER(x,y) ((x)-(y) < 10 && (x)-(y) > -10)
void CalculateMoveableDocks(CCW_params *p, RECT *r, POINT *cursor)
{
    int i, fit =  - 1, fitwidth = 0, index, j;
    CCD_params *d = FindParams(p, &index);
    RECT r1, r2, r3;
    GetFrameWindowRect(&r3);
    GetClientWindowRect(&r1);
    suggestedDock = 0;

    if (HYSTER(cursor->x, r1.left))
    {
        r->top = r1.top;
        r->left = r1.left;
        if (p->type == LSTOOLBAR)
        {
            r->bottom = r->top + p->u.tb.vsize.cy + GRIPWIDTH;
            r->right = r->left + p->u.tb.vsize.cx;
        }
        else
        {
            r->bottom = r1.bottom;
            r->right = DEFAULT_WIDTH + r->left;
        }
        suggestedDock = DOCK_LEFT;
    }
    else if (HYSTER(cursor->y, r1.top))
    {
        r->left = r3.left;
        r->top = r1.top;
        if (p->type == LSTOOLBAR)
        {
            r->bottom = r->top + p->u.tb.hsize.cy;
            r->right = r->left + p->u.tb.hsize.cx;
        }
        else
        {
            r->right = r3.right;
            r->bottom = DEFAULT_HEIGHT + r->top;
        }
        suggestedDock = DOCK_TOP;
    }
    else if (HYSTER(cursor->x, r1.right))
    {
        r->top = r1.top;
        r->right = r1.right;
        if (p->type == LSTOOLBAR)
        {
            r->bottom = r->top + p->u.tb.vsize.cy;
            r->left = r->right - p->u.tb.vsize.cx;
        }
        else
        {
            r->bottom = r1.bottom;
            r->left = r->right - DEFAULT_WIDTH;
        }
        suggestedDock = DOCK_RIGHT;
    }
    else if (HYSTER(cursor->y, r1.bottom))
    {
        r->left = r3.left;
        r->bottom = r1.bottom;
        if (p->type == LSTOOLBAR)
        {
            r->top = r->bottom - p->u.tb.hsize.cy;
            r->right = r->left + p->u.tb.hsize.cx;
        }
        else
        {
            r->right = r3.right;
            r->top = r->bottom - DEFAULT_HEIGHT;
        }
        suggestedDock = DOCK_BOTTOM;
    }
    else
    {
        RECT dest;
        for (i = 0; i < currentWindows; i++)
        {
            if ((docks[i]->flags &DOCK_PRESENT) && !docks[i]->hidden)
            {
                if (PtInRect(&docks[i]->position,  *cursor))
                {
                    fit = i;
                    break;
                }
            }
        }
        if (fit !=  - 1)
        {

            int insert = 0, width;
            if (docks[fit]->flags &(DOCK_BOTTOM | DOCK_TOP))
            {
                if (p->type == LSTOOLBAR)
                {
                    if (docks[fit]->flags &DOCK_TOP)
                    {
                        r->top = docks[fit]->position.top;
                        r->bottom = r->top + p->u.tb.hsize.cy;
                    }
                    else
                    {
                        r->bottom = docks[fit]->position.bottom;
                        r->top = r->bottom - p->u.tb.hsize.cy;
                    }
                    r->right = r->left + p->u.tb.hsize.cx + GRIPWIDTH;
                }
                else
                {
                    r->top = docks[fit]->position.top;
                    r->bottom = docks[fit]->position.bottom;
                }
            }
            else
            {
                if (p->type == LSTOOLBAR)
                {
                    if (docks[fit]->flags &DOCK_LEFT)
                    {
                        r->left = docks[fit]->position.left;
                        r->right = r->left + p->u.tb.vsize.cx;
                    }
                    else
                    {
                        r->right = docks[fit]->position.right;
                        r->left = r->right - p->u.tb.vsize.cx;
                    }
                    r->bottom = r->top + p->u.tb.vsize.cy + GRIPWIDTH;
                }
                else
                {
                    r->left = docks[fit]->position.left;
                    r->right = docks[fit]->position.right;
                }
            }
            suggestedDock = docks[fit]->flags;
        }
        else
        {
            if (p->type == LSTOOLBAR)
            {
                for (i = 0; i < blankCount; i++)
                {
                    if (PtInRect(&blanks[i]->u.bw.position,  *cursor))
                    {
                        fit = i;
                        break;
                    }
                }
                if (fit !=  - 1)
                {
                    if (blanks[fit]->u.bw.representative_dock->flags &
                        (DOCK_LEFT | DOCK_RIGHT))
                    {
                        r->left = blanks[fit]->u.bw.representative_dock
                            ->position.left;
                        r->right = blanks[fit]->u.bw.representative_dock
                            ->position.right;
                        r->bottom = r->top + p->u.tb.vsize.cy + GRIPWIDTH;
                    }
                    else
                    {
                        r->top = blanks[fit]->u.bw.representative_dock
                            ->position.top;
                        r->bottom = blanks[fit]->u.bw.representative_dock
                            ->position.bottom;
                        r->right = r->left + p->u.tb.hsize.cx + GRIPWIDTH;
                    }
                    suggestedDock = blanks[fit]->u.bw.representative_dock
                        ->flags;
                }
            }
        }
    }
    lastSuggestedDock = suggestedDock;
}

//-------------------------------------------------------------------------

int InsertRow(int index, int dflag)
{
    int maxindex = 0, i;
    RECT dest;
    if (index >= 0)
    {
        docks[index]->rowindex =  - 1;
        for (i = 0; i < currentWindows; i++)
        {
            if (i == index)
                continue;
            if (docks[i]->rowindex > maxindex)
                maxindex = docks[i]->rowindex;
            if (docks[i]->flags &dflag)
            {
                if (cache[i]->type == cache[index]->type)
                {
                    if (docks[index]->position.top == docks[i]->position.top &&
                        docks[index]->position.bottom == docks[i]
                        ->position.bottom)
                        return docks[index]->rowindex = docks[i]->rowindex;
                }
                else
                {
                    if (dflag &DOCK_TOP)
                    {
                        if (docks[index]->position.top == docks[i]
                            ->position.top)
                        {
                            docks[index]->rowindex = docks[i]->rowindex;
                            break;
                        }
                    }
                    else
                    {
                        if (docks[index]->position.bottom == docks[i]
                            ->position.bottom)
                        {
                            docks[index]->rowindex = docks[i]->rowindex;
                            break;
                        }
                    }
                }
            }
        }
        if (docks[index]->rowindex ==  - 1)
        {
            docks[index]->rowindex = maxindex + 1;
        }
        else
        {
            for (i = 0; i < currentWindows; i++)
                if (i != index && (docks[i]->flags &dflag) && docks[i]
                    ->rowindex >= docks[index]->rowindex)
                    docks[i]->rowindex++;

        }
        return docks[index]->rowindex;
    }
    return 0;
}

//-------------------------------------------------------------------------

void OrderLeftRight(int *windows, int wincount, int byposition)
{
    int i, j;
    if (byposition)
        for (i = 0; i < wincount - 1; i++)
    for (j = i + 1; j < wincount; j++)
    {
        CCD_params *d1 = docks[windows[j]],  *d2 = docks[windows[i]];
        if (d1->position.left < d2->position.left || (d1->position.left == d2
            ->position.left && d2->position.right > d1->position.right))
        {
            int val = windows[j];
            windows[j] = windows[i];
            windows[i] = val;
        }
    }
    else
        for (i = 0; i < wincount - 1; i++)
    for (j = i + 1; j < wincount; j++)
    {
        CCD_params *d1 = docks[windows[j]],  *d2 = docks[windows[i]];
        if (d1->colindex < d2->colindex)
        {
            int val = windows[j];
            windows[j] = windows[i];
            windows[i] = val;
        }
    }
}

//-------------------------------------------------------------------------

int IndexLeftRight(int *windows, int wincount, int index)
{
    int j;
    RECT dest;
    int maxindex =  - 1, max = 0;
    if (docks[index]->oldflags == docks[index]->flags && docks[index]
        ->position.top == docks[index]->lastposition.top)
    if (IntersectRect(&dest, &docks[index]->position, &docks[index]
        ->lastposition))
    {
        if (dest.right - dest.left > max)
        {
            max = dest.right - dest.left;
            maxindex =  - 2;
        }
    }
    for (j = 0; j < wincount; j++)
    {
        if (IntersectRect(&dest, &docks[index]->position, &docks[windows[j]]
            ->position))
        {
            if (dest.right - dest.left > max)
            {
                max = dest.right - dest.left;
                maxindex = j;
            }
        }
    }
    if (maxindex ==  - 1)
    {
        for (maxindex = wincount; maxindex > 0; maxindex--)
        if (docks[index]->position.left > docks[windows[maxindex - 1]]
            ->position.left)
        {
            break;
        }
    }

    return maxindex;
}

//-------------------------------------------------------------------------

int InsertLeftRight(int *windows, int wincount, int index, RECT *frame, int min)
{
    int i;
    for (i = 0; i < wincount; i++)
        if (index == windows[i])
            return 0;
    //      if (wincount && cache[windows[0]]->type == LSTOOLBAR || !wincount && cache[index]->type == LSTOOLBAR) {
    //         i = wincount ;
    //      } else 
    {

        i = IndexLeftRight(windows, wincount, index);
        if (i < wincount)
        {
            if (i ==  - 2)
            {
                for (i = 0; i < wincount && docks[windows[i]]->position.left <
                    docks[index]->position.left; i++)
                    ;
            }
            else if (i !=  - 1)
            {
                int val1 = (docks[windows[i]]->position.right +
                    docks[windows[i]]->position.left) / 2;
                if (docks[index]->position.left > val1)
                {
                    i++;
                }
                else if (docks[index]->position.right > val1)
                {
                    if (val1 - docks[index]->position.left < docks[index]
                        ->position.right - val1)
                        i++;
                }
            }
            else
                i = 0;
        }
        if (docks[index]->oldflags != docks[index]->flags || docks[index]
            ->lastposition.top != min)
            docks[index]->position.right = docks[index]->position.left + (frame
                ->right - frame->left) / (wincount ? wincount : 1);
        else
            docks[index]->position.right = docks[index]->position.left +
                docks[index]->lastposition.right - docks[index]
                ->lastposition.left;
    }

    memmove(&windows[i + 1], &windows[i], (wincount - i) *sizeof(int));
    windows[i] = index;
    wincount++;

    if (cache[windows[0]]->type == LSTOOLBAR)
    {
        for (i = 1; i < wincount; i++)
        if (docks[windows[i]]->position.left < docks[windows[i - 1]]
            ->position.left + cache[windows[i - 1]]->u.tb.hsize.cx + GRIPWIDTH)
        {
            docks[windows[i]]->position.left = docks[windows[i - 1]]
                ->position.left + cache[windows[i - 1]]->u.tb.hsize.cx +
                GRIPWIDTH;
            docks[windows[i]]->position.right = docks[windows[i]]
                ->position.left + cache[windows[i]]->u.tb.hsize.cx + GRIPWIDTH;
        }
    }
    return 1;
}

//-------------------------------------------------------------------------

void HideAdjLeftRight(int *windows, int wincount, int index)
{
    if (cache[index]->type == LSTOOLBAR)
    {
        return ;
    }
    if (wincount <= 1)
        docks[index]->hiddenwidth = 1000 *(docks[index]->position.right -
            docks[index]->position.left);
    else
    {
        int j, unhiddenwidth = 0, totalwidth = 0;
        RECT r;
        int sizewidth, scale;
        OrderLeftRight(windows, wincount, TRUE);
        GetFrameWindowRect(&r);
        sizewidth = r.right - r.left;
        docks[index]->hiddenwidth = 0;
        for (j = 0; j < wincount; j++)
        {
            docks[windows[j]]->colindex = j;
            if (windows[j] != index)
            {
                totalwidth += docks[windows[j]]->hiddenwidth;
                if (!docks[windows[j]]->hidden)
                    unhiddenwidth += docks[windows[j]]->hiddenwidth;
            }
        }
        if (!unhiddenwidth)
        {
            // if there is a bug and unhiddenwith is zero, resync
            for (j = 0; j < wincount; j++)
                docks[windows[j]]->hiddenwidth = 1000 *(r.right - r.left) /
                    wincount;
        }
        else
        {
            docks[index]->hiddenwidth = totalwidth / (wincount - 1);
            if (!docks[index]->hidden)
                unhiddenwidth += docks[index]->hiddenwidth;
            scale = unhiddenwidth / sizewidth;
            for (j = 0; j < wincount; j++)
                docks[windows[j]]->hiddenwidth = docks[windows[j]]->hiddenwidth
                    *1000 / scale;
        }
    }
}

//-------------------------------------------------------------------------

void SizeLeftRight(int *windows, int wincount, int adj, RECT *r2)
{
    int i, j, multiply = 1000;
    int framesize = r2->right - r2->left;
    int actualsize = 0, currentleft = r2->left, newright;
    if (cache[windows[0]]->type != LSTOOLBAR)
        framesize -= (wincount - 1) *FRAMEWIDTH;
    for (i = 0; i < wincount; i++)
    {
        int v = windows[i];
        actualsize += docks[windows[i]]->position.right - docks[windows[i]]
            ->position.left;
        if (docks[windows[i]]->flexparams &(FLEX_LARGE | FLEX_SMALL))
            docks[windows[i]]->position = docks[windows[i]]->flexposition;
    }
    if (actualsize != framesize && actualsize)
    {
        multiply = 1000 * framesize / actualsize;
    }
    for (i = 0; i < wincount; i++)
    {
        if (docks[windows[0]]->flags & DOCK_TOP) 
        {
            docks[windows[i]]->position.bottom = adj
                    + docks[windows[0]]->position.bottom 
                    - docks[windows[0]]->position.top ;
            docks[windows[i]]->position.top = adj;
        } 
        else
        {
            docks[windows[i]]->position.top = adj
                    - docks[windows[0]]->position.bottom 
                    + docks[windows[0]]->position.top ;
            docks[windows[i]]->position.bottom = adj;
        }
        if (cache[windows[0]]->type == LSTOOLBAR)
        {
//            currentleft = docks[windows[i]]->position.left;
            if (currentleft < r2->left || currentleft >= r2->right - GRIPWIDTH)
                if (i == 0)
                    currentleft = r2->left;
                else
                    currentleft = docks[windows[i - 1]]->position.right;
            if (i > 0 && currentleft < docks[windows[i - 1]]->position.right)
                currentleft = docks[windows[i - 1]]->position.right;
            newright = currentleft + cache[windows[i]]->u.tb.hsize.cx +
                GRIPWIDTH;
            docks[windows[i]]->position.right = newright;
        }
        else
        {
            newright = docks[windows[i]]->position.right = currentleft + 
                (docks[windows[i]]->position.right - docks[windows[i]]
                ->position.left) *multiply / 1000;
        }
        docks[windows[i]]->position.left = currentleft;
        docks[windows[i]]->position.right = newright ;
        currentleft = newright;
        if (cache[windows[0]]->type != LSTOOLBAR) {
            currentleft += FRAMEWIDTH;
        }
    }
    if (cache[windows[0]]->type != LSTOOLBAR)
        docks[windows[wincount - 1]]->position.right = r2->right;
    if (wincount == 1)
    {
        SendMessage(cache[windows[0]]->hwnd, LCF_SETFLEX, 0, wincount != 1);
    }
    else
    {
        for (i = 0; i < wincount; i++)
        {
            if (docks[windows[i]]->flexparams &FLEX_UNREQ)
            {
                for (j = 0; j < wincount; j++)
                    docks[windows[j]]->flexparams = 0;
                break;
            }
            else if (docks[windows[i]]->flexparams &FLEX_REQ)
            {
                for (j = 0; j < wincount; j++)
                    docks[windows[j]]->flexparams = FLEX_SMALL;
                docks[windows[i]]->flexparams = FLEX_LARGE;
                break;
            }
            docks[windows[i]]->flexparams &= ~(FLEX_UNREQ | FLEX_REQ);
        }
        if (docks[windows[0]]->flexparams &(FLEX_SMALL | FLEX_LARGE))
        {
            framesize -= (wincount - 1) *MIN_SIZE;
            currentleft = r2->left;

            for (i = 0; i < wincount; i++)
                if (docks[windows[i]]->flexparams &FLEX_LARGE)
                    break;

            if (i < wincount)
            for (i = 0; i < wincount; i++)
            {
                docks[windows[i]]->flexposition = docks[windows[i]]->position;
                docks[windows[i]]->position.left = currentleft;
                if (docks[windows[i]]->flexparams &FLEX_LARGE)
                    currentleft = docks[windows[i]]->position.right =
                        currentleft + framesize;
                else
                    currentleft = docks[windows[i]]->position.right =
                        currentleft + MIN_SIZE;
                currentleft += FRAMEWIDTH;
            }
            else
                for (i = 0; i < wincount; i++)
                    docks[windows[i]]->flexparams = 0;
        }
        for (i = 0; i < wincount; i++)
        {
            SendMessage(cache[windows[i]]->hwnd, LCF_SETFLEX, docks[windows[i]]
                ->flexparams &FLEX_LARGE ? 1 : 0, wincount != 1);
            if (i != wincount - 1 && cache[windows[i]]->type != LSTOOLBAR)
                InsertVertFrame(windows[i], 0, 0);
        }
    }
    if (cache[windows[0]]->type == LSTOOLBAR)
    {
        int left = r2->left;
        RECT r = docks[windows[0]]->position;
        for (i = 0; i < wincount; i++)
        {
            if (left < docks[windows[i]]->position.left)
            {
                r.left = left;
                r.right = docks[windows[i]]->position.left;
                AllocBlank(&r, docks[windows[0]], FALSE);
            }
            left = docks[windows[i]]->position.right;
        }
        if (left < r2->right)
        {
            r. left = left;
            r.right = r2->right;
            AllocBlank(&r, docks[windows[0]], FALSE);
        }
    }
}

//-------------------------------------------------------------------------

int InsertColumn(int index, int dflag)
{
    int maxindex = 0, i;
    RECT dest;
    if (index >= 0)
    {
        docks[index]->rowindex =  - 1;
        for (i = 0; i < currentWindows; i++)
        {
            if (i == index)
                continue;
            if (docks[i]->rowindex > maxindex)
                maxindex = docks[i]->rowindex;
            if (docks[i]->flags &dflag)
            {
                if (cache[i]->type == cache[index]->type)
                {
                    if (docks[index]->position.left == docks[i]->position.left
                        && docks[index]->position.right == docks[i]
                        ->position.right)
                        return docks[index]->rowindex = docks[i]->rowindex;
                }
                else
                {
                    if (dflag &DOCK_LEFT)
                    {
                        if (docks[index]->position.left == docks[i]
                            ->position.left)
                        {
                            docks[index]->rowindex = docks[i]->rowindex;
                            break;
                        }
                    }
                    else
                    {
                        if (docks[index]->position.right == docks[i]
                            ->position.right)
                        {
                            docks[index]->rowindex = docks[i]->rowindex;
                            break;
                        }
                    }
                }
            }
        }
        if (docks[index]->rowindex ==  - 1)
        {
            docks[index]->rowindex = maxindex + 1;
        }
        else
        {
            for (i = 0; i < currentWindows; i++)
                if (i != index && (docks[i]->flags &dflag) && docks[i]
                    ->rowindex >= docks[index]->rowindex)
                    docks[i]->rowindex++;

        }
        return docks[index]->rowindex;
    }
    return 0;
}

//-------------------------------------------------------------------------

void OrderTopBottom(int *windows, int wincount, int byposition)
{
    int i, j;
    if (byposition)
        for (i = 0; i < wincount - 1; i++)
    for (j = i + 1; j < wincount; j++)
    {
        CCD_params *d1 = docks[windows[j]],  *d2 = docks[windows[i]];
        if (d1->position.top < d2->position.top || (d1->position.top == d2
            ->position.top && d2->position.bottom > d1->position.bottom))
        {
            int val = windows[j];
            windows[j] = windows[i];
            windows[i] = val;
        }
    }
    else
        for (i = 0; i < wincount - 1; i++)
    for (j = i + 1; j < wincount; j++)
    {
        CCD_params *d1 = docks[windows[j]],  *d2 = docks[windows[i]];
        if (d1->colindex < d2->colindex)
        {
            int val = windows[j];
            windows[j] = windows[i];
            windows[i] = val;
        }
    }
}

//-------------------------------------------------------------------------

int IndexTopBottom(int *windows, int wincount, int index)
{
    int j;
    RECT dest;
    int maxindex =  - 1, max = 0;
    if (docks[index]->oldflags == docks[index]->flags && docks[index]
        ->position.left == docks[index]->lastposition.left)
    if (IntersectRect(&dest, &docks[index]->position, &docks[index]
        ->lastposition))
    {
        if (dest.bottom - dest.top > max)
        {
            max = dest.bottom - dest.top;
            maxindex =  - 2;
        }
    }
    for (j = 0; j < wincount; j++)
    {
        if (IntersectRect(&dest, &docks[index]->position, &docks[windows[j]]
            ->position))
        {
            if (dest.bottom - dest.top > max)
            {
                max = dest.bottom - dest.top;
                maxindex = j;
            }
        }
    }
    if (maxindex ==  - 1)
    {
        for (maxindex = wincount; maxindex > 0; maxindex--)
        if (docks[index]->position.top > docks[windows[maxindex - 1]]
            ->position.top)
        {
            break;
        }
    }
    return maxindex;
}

//-------------------------------------------------------------------------

int InsertTopBottom(int *windows, int wincount, int index, RECT *frame, int min)
{
    int i;
    for (i = 0; i < wincount; i++)
        if (index == windows[i])
            return 0;
    //      if (wincount && cache[windows[0]]->type == LSTOOLBAR || !wincount && cache[index]->type == LSTOOLBAR)
    //         i = wincount ;
    //      else 
    {
        i = IndexTopBottom(windows, wincount, index);
        if (i < wincount)
        {
            if (i ==  - 2)
            {
                for (i = 0; i < wincount && docks[windows[i]]->position.top <
                    docks[index]->position.top; i++)
                    ;
            }
            else if (i !=  - 1)
            {
                int val1 = (docks[windows[i]]->position.bottom +
                    docks[windows[i]]->position.top) / 2;
                if (docks[index]->position.top > val1)
                {
                    i++;
                }
                else if (docks[index]->position.bottom > val1)
                {
                    if (val1 - docks[index]->position.top < docks[index]
                        ->position.bottom - val1)
                        i++;
                }
            }
            else
                i = 0;
        }
        if (docks[index]->oldflags != docks[index]->flags || docks[index]
            ->lastposition.left != min)
            docks[index]->position.bottom = docks[index]->position.top + (frame
                ->bottom - frame->top) / (wincount ? wincount : 1);
        else
            docks[index]->position.bottom = docks[index]->position.top +
                docks[index]->lastposition.bottom - docks[index]
                ->lastposition.top;
    }
    memmove(&windows[i + 1], &windows[i], (wincount - i) *sizeof(int));
    windows[i] = index;
    wincount++;
    if (cache[windows[0]]->type == LSTOOLBAR)
    {
        for (i = 1; i < wincount; i++)
        if (docks[windows[i]]->position.top < docks[windows[i - 1]]
            ->position.top + cache[windows[i - 1]]->u.tb.vsize.cy + GRIPWIDTH)
        {
            docks[windows[i]]->position.top = docks[windows[i - 1]]
                ->position.top + cache[windows[i - 1]]->u.tb.vsize.cy +
                GRIPWIDTH;
            docks[windows[i]]->position.bottom = docks[windows[i]]
                ->position.top + cache[windows[i]]->u.tb.vsize.cy + GRIPWIDTH;
        }
    }
    return 1;
}

//-------------------------------------------------------------------------

void HideAdjTopBottom(int *windows, int wincount, int index)
{
    if (cache[index]->type == LSTOOLBAR)
    {
        return ;
    }
    if (wincount <= 1)
        docks[index]->hiddenwidth = 1000 *(docks[index]->position.bottom -
            docks[index]->position.top);
    else
    {
        int j, unhiddenwidth = 0, totalwidth = 0;
        RECT r;
        int sizewidth, scale;
        OrderTopBottom(windows, wincount, TRUE);
        GetWindowRect(hwndClient, &r);
        sizewidth = r.bottom - r.top;
        docks[index]->hiddenwidth = 0;
        for (j = 0; j < wincount; j++)
        {
            docks[windows[j]]->colindex = j;
            if (windows[j] != index)
            {
                totalwidth += docks[windows[j]]->hiddenwidth;
                if (!docks[windows[j]]->hidden)
                    unhiddenwidth += docks[windows[j]]->hiddenwidth;
            }
        }
        if (!unhiddenwidth)
        {
            // if there is a bug and unhiddenwith is zero, resync
            for (j = 0; j < wincount; j++)
                docks[windows[j]]->hiddenwidth = 1000 *(r.bottom - r.top) /
                    wincount;
        }
        else
        {
            docks[index]->hiddenwidth = totalwidth / (wincount - 1);
            if (!docks[index]->hidden)
                unhiddenwidth += docks[index]->hiddenwidth;
            scale = unhiddenwidth / sizewidth;
            totalwidth = totalwidth * 1000 / scale;
            for (j = 0; j < wincount; j++)
                docks[windows[j]]->hiddenwidth = docks[windows[j]]->hiddenwidth
                    *1000 / scale;
        }
    }
}

//-------------------------------------------------------------------------

void SizeTopBottom(int *windows, int wincount, int adj, RECT *client, RECT
    *oldclient)
{
    int i, j, multiply = 1000;
    int framesize = client->bottom - client->top;
    int actualsize = 0, currenttop = client->top, newbottom;
    if (cache[windows[0]]->type != LSTOOLBAR)
        framesize -= (wincount - 1) *FRAMEWIDTH;
    for (i = 0; i < wincount; i++)
    {
        actualsize += docks[windows[i]]->position.bottom - docks[windows[i]]
            ->position.top;
        if (docks[windows[i]]->flexparams &(FLEX_LARGE | FLEX_SMALL))
            docks[windows[i]]->position = docks[windows[i]]->flexposition;
    }
    if (actualsize != framesize && actualsize)
    {
        multiply = 1000 * framesize / actualsize;
    }
    for (i = 0; i < wincount; i++)
    {
        if (docks[windows[0]]->flags & DOCK_LEFT) 
        {
            docks[windows[i]]->position.right = adj
                    + docks[windows[0]]->position.right
                    - docks[windows[0]]->position.left ;
            docks[windows[i]]->position.left = adj;
        }
        else
        {
            docks[windows[i]]->position.left = adj
                    - docks[windows[0]]->position.right
                    + docks[windows[0]]->position.left ;
            docks[windows[i]]->position.right = adj;
        }
        if (cache[windows[0]]->type == LSTOOLBAR)
        {
//            currenttop = docks[windows[i]]->position.top - oldclient->top +
//                client->top;
            if (currenttop < client->top || currenttop >= client->bottom -
                GRIPWIDTH)
                if (i == 0)
                    currenttop = client->top;
                else
                    currenttop = docks[windows[i - 1]]->position.bottom;
            if (i > 0 && currenttop < docks[windows[i - 1]]->position.bottom)
                currenttop = docks[windows[i - 1]]->position.bottom;
            newbottom = currenttop + cache[windows[i]]->u.tb.vsize.cy +
                GRIPWIDTH;
            docks[windows[i]]->position.bottom = newbottom;
        }
        else
        {
            newbottom = docks[windows[i]]->position.bottom = currenttop + 
                (docks[windows[i]]->position.bottom - docks[windows[i]]
                ->position.top) *multiply / 1000;
        }
        docks[windows[i]]->position.top = currenttop;
        docks[windows[i]]->position.bottom = newbottom ;
        currenttop = newbottom;
        if (cache[windows[0]]->type != LSTOOLBAR) {
            currenttop += FRAMEWIDTH;
        }
    }
    if (cache[windows[0]]->type != LSTOOLBAR)
        docks[windows[wincount - 1]]->position.bottom = client->bottom;
    if (wincount == 1)
    {
        SendMessage(cache[windows[0]]->hwnd, LCF_SETFLEX, 0, wincount != 1);
    }
    else
    {
        for (i = 0; i < wincount; i++)
        {
            if (docks[windows[i]]->flexparams &FLEX_UNREQ)
            {
                for (j = 0; j < wincount; j++)
                    docks[windows[j]]->flexparams = 0;
                break;
            }
            else if (docks[windows[i]]->flexparams &FLEX_REQ)
            {
                for (j = 0; j < wincount; j++)
                    docks[windows[j]]->flexparams = FLEX_SMALL;
                docks[windows[i]]->flexparams = FLEX_LARGE;
                break;
            }
            docks[windows[i]]->flexparams &= ~(FLEX_UNREQ | FLEX_REQ);
        }
        if (docks[windows[0]]->flexparams &(FLEX_SMALL | FLEX_LARGE))
        {
            framesize -= (wincount - 1) *MIN_SIZE;
            currenttop = client->top;

            for (i = 0; i < wincount; i++)
                if (docks[windows[i]]->flexparams &FLEX_LARGE)
                    break;
            if (i < wincount)
            for (i = 0; i < wincount; i++)
            {
                docks[windows[i]]->flexposition = docks[windows[i]]->position;
                docks[windows[i]]->position.top = currenttop;
                if (docks[windows[i]]->flexparams &FLEX_LARGE)
                    currenttop = docks[windows[i]]->position.bottom =
                        currenttop + framesize;
                else
                    currenttop = docks[windows[i]]->position.bottom =
                        currenttop + MIN_SIZE;
                currenttop += FRAMEWIDTH;
            }
            else
                for (i = 0; i < wincount; i++)
                    docks[windows[i]]->flexparams = 0;
        }
        for (i = 0; i < wincount; i++)
        {
            SendMessage(cache[windows[i]]->hwnd, LCF_SETFLEX, docks[windows[i]]
                ->flexparams &FLEX_LARGE ? 1 : 0, wincount != 1);
            if (i != wincount - 1 && cache[windows[i]]->type != LSTOOLBAR)
                InsertHorizFrame(windows[i], 0, 0);
        }
    }
    if (cache[windows[0]]->type == LSTOOLBAR)
    {
        int top = client->top;
        RECT r = docks[windows[0]]->position;
        for (i = 0; i < wincount; i++)
        {
            if (top < docks[windows[i]]->position.top)
            {
                r.top = top;
                r.bottom = docks[windows[i]]->position.top;
                AllocBlank(&r, docks[windows[0]], FALSE);
            }
            top = docks[windows[i]]->position.bottom;
        }
        if (top < client->bottom)
        {
            r.top = top;
            r.bottom = client->bottom;
            AllocBlank(&r, docks[windows[0]], FALSE);
        }
    }
}

//-------------------------------------------------------------------------

void InsertVertFrame(int index, int left, RECT *fullframe)
{
    RECT r, r1;
    CCD_params *d = docks[index];
    CCW_params *f;
    GetFrameWindowRect(&r1);
    if (left)
    {
        r.right = d->position.left;
        r.left = d->position.left - FRAMEWIDTH;
    }
    else
    {
        r.left = d->position.right;
        r.right = d->position.right + FRAMEWIDTH;
    }
    if (fullframe)
    {
        r.top = fullframe->top;
        r.bottom = fullframe->bottom;
    }
    else
    {
        r.top = d->position.top;
        r.bottom = d->position.bottom;
    }
    OffsetRect(&r,  - r1.left,  - r1.top);
    f = CreateFrameWindow(hwndFrame);
    frames[frameCount++] = f;
    f->vertical = TRUE;
    f->u.fw.position = r;
    InvalidateRect(f->hwnd, 0, 0);
    MoveWindow(f->hwnd, r.left, r.top, r.right - r.left,
        r.bottom - r.top, 1);
}

//-------------------------------------------------------------------------

void InsertHorizFrame(int index, int top, int fullframe)
{
    RECT r, r1;
    CCD_params *d = docks[index];
    CCW_params *f;
    GetFrameWindowRect(&r1);
    if (top)
    {
        r.top = d->position.top - FRAMEWIDTH;
        r.bottom = d->position.top;
    }
    else
    {
        r.top = d->position.bottom;
        r.bottom = d->position.bottom + FRAMEWIDTH;
    }
    if (fullframe)
    {
        r.left = r1.left;
        r.right = r1.right;
    }
    else
    {
        r.left = d->position.left;
        r.right = d->position.right;
    }
    OffsetRect(&r,  - r1.left,  - r1.top);
    f = CreateFrameWindow(hwndFrame);
    frames[frameCount++] = f;
    f->vertical = FALSE;
    f->u.fw.position = r;
    InvalidateRect(f->hwnd, 0, 0);
    MoveWindow(f->hwnd, r.left, r.top, r.right - r.left,
        r.bottom - r.top, 1);
}

//-------------------------------------------------------------------------

void CalculateLayout(int index, int recalhidden)
{
    int i, j, pos, indexmax, min, adj2, found, hidden, totalsize;
    int windows[30], wincount, hwindows[30], hwincount;
    int srctabheight;
    int bottom;
    RECT r1, r2, temp, client, oldclient;
    GetWindowRect(hwndClient, &r1);
    GetFrameWindowRect(&r2);
    client = r2;
    oldclient = r1;
    oldclient.top -= SRCTABHEIGHT;
    if (IsWindowVisible(hwndJumpList))
        oldclient.top -= SendMessage(hwndJumpList, WM_GETHEIGHT, 0, 0);
    if (rundown)
        return ;

    for (i=0; i < frameCount; i++)
    {
        DestroyWindow(frames[i]->hwnd);
    }
    for (i=0; i < blankCount; i++)
    {
        DestroyWindow(blanks[i]->hwnd);
    }
    blankCount = 0;
    frameCount = 0;
    if (index >= 0)
        docks[index]->rowindex =  - 1;

    if (index >= 0 && (docks[index]->flags &DOCK_TOP))
        InsertRow(index, DOCK_TOP);
    indexmax =  - 1;
    for (i = 0, min =  - 1; i < currentWindows; i++)
    if (docks[i]->flags &DOCK_TOP)
    {
        if (docks[i]->rowindex > indexmax)
            indexmax = docks[i]->rowindex;
    }
    adj2 = 0;
    for (i = 0; i <= indexmax; i++)
    {
        found = FALSE, hidden = FALSE;
        hwincount = 0;
        wincount = 0;
        for (j = 0; j < currentWindows; j++)
        if ((docks[j]->flags &DOCK_TOP) && docks[j]->rowindex == i)
        {
            docks[j]->rowindex -= adj2;
            if (!docks[j]->hidden)
            {
                found = TRUE;
                min = docks[j]->position.top;
                if (j != index)
                    windows[wincount++] = j;
            }
            else
                hidden = TRUE;
            hwindows[hwincount++] = j;
        }
        if (!found)
        {
            if (!hidden)
                adj2++;
            continue;
        }
        OrderLeftRight(windows, wincount, FALSE);
        if (index >= 0 && docks[index]->rowindex == i - adj2 && (docks[index]
            ->flags &DOCK_TOP) && !docks[index]->hidden)
        {
            wincount += InsertLeftRight(windows, wincount, index, &r2, min);
            HideAdjLeftRight(hwindows, hwincount, index);
            index =  - 1;
        }
        for (j = 0; j < wincount; j++) {
            docks[windows[j]]->colindex = j;
        }
        SizeLeftRight(windows, wincount, client.top, &r2);
        if (recalhidden)
        {
            totalsize = 0;
            for (j = 0; j < wincount; j++)
            {
                totalsize += docks[j]->hiddenwidth = (docks[j]->position.right 
                    - docks[j]->position.left) *1000;
            }
            for (j = 0; j < hwincount; j++)
            {
                if (docks[j]->hidden)
                {
                    docks[j]->hiddenwidth = totalsize / wincount;
                }
            }
        }
        if (cache[windows[0]]->type != LSTOOLBAR)
        {
            InsertHorizFrame(windows[0], 0, 1);
            client.top = FRAMEWIDTH;
        }
        else
            client.top = 0;
        client.top += docks[windows[0]]->position.bottom;
    }
    if (index >= 0 && (docks[index]->flags &DOCK_BOTTOM))
        InsertRow(index, DOCK_BOTTOM);
    indexmax =  - 1;
    for (i = 0, min =  - 1; i < currentWindows; i++)
    if (docks[i]->flags &DOCK_BOTTOM)
    {
        if (docks[i]->rowindex > indexmax)
            indexmax = docks[i]->rowindex;
    }
    adj2 = 0;
    for (i = 0; i <= indexmax; i++)
    {
        found = FALSE, hidden = FALSE;
        hwincount = 0;
        wincount = 0;
        for (j = 0; j < currentWindows; j++)
        if ((docks[j]->flags &DOCK_BOTTOM) && docks[j]->rowindex == i)
        {
            docks[j]->rowindex -= adj2;
            if (!docks[j]->hidden)
            {
                found = TRUE;
                min = docks[j]->position.bottom;
                if (j != index)
                    windows[wincount++] = j;
            }
            else
                hidden = TRUE;
            hwindows[hwincount++] = j;
        }
        if (!found)
        {
            if (!hidden)
                adj2++;
            continue;
        }
        OrderLeftRight(windows, wincount, FALSE);
        if (index >= 0 && docks[index]->rowindex == i - adj2 && (docks[index]
            ->flags &DOCK_BOTTOM) && !docks[index]->hidden)
        {
            wincount += InsertLeftRight(windows, wincount, index, &r2, min);
            HideAdjLeftRight(hwindows, hwincount, index);
            index =  - 1;
        }
        for (j = 0; j < wincount; j++) {
            docks[windows[j]]->colindex = j;
        }
        SizeLeftRight(windows, wincount, client.bottom, &r2);
        if (recalhidden)
        {
            totalsize = 0;
            for (j = 0; j < wincount; j++)
            {
                totalsize += docks[j]->hiddenwidth = (docks[j]->position.right 
                    - docks[j]->position.left) *1000;
            }
            for (j = 0; j < hwincount; j++)
            {
                if (docks[j]->hidden)
                {
                    docks[j]->hiddenwidth = totalsize / wincount;
                }
            }
        }
        if (cache[windows[0]]->type != LSTOOLBAR)
        {
            InsertHorizFrame(windows[0], 1, 1);
            client.bottom =  - FRAMEWIDTH;
        }
        else
            client.bottom = 0;
        client.bottom += docks[windows[0]]->position.top;
    }
    indexmax =  - 1;
    if (index >= 0 && (docks[index]->flags &DOCK_LEFT))
        InsertColumn(index, DOCK_LEFT);
    for (i = 0, min =  - 1; i < currentWindows; i++)
    if (docks[i]->flags &DOCK_LEFT)
    {
        if (docks[i]->rowindex > indexmax)
            indexmax = docks[i]->rowindex;
    }
    adj2 = 0;
    if (index ==  - 2)
        oldclient = client;
    for (i = 0; i <= indexmax; i++)
    {
        found = FALSE, hidden = FALSE;
        hwincount = 0;
        wincount = 0;
        for (j = 0; j < currentWindows; j++)
        if ((docks[j]->flags &DOCK_LEFT) && docks[j]->rowindex == i)
        {
            docks[j]->rowindex -= adj2;
            if (!docks[j]->hidden)
            {
                found = TRUE;
                min = docks[j]->position.left;
                if (j != index)
                    windows[wincount++] = j;
            }
            else
                hidden = TRUE;
            hwindows[hwincount++] = j;
        }
        if (!found)
        {
            if (!hidden)
                adj2++;
            continue;
        }
        OrderTopBottom(windows, wincount, FALSE);
        if (index >= 0 && docks[index]->rowindex == i - adj2 && (docks[index]
            ->flags &DOCK_LEFT) && !docks[index]->hidden)
        {
            wincount += InsertTopBottom(windows, wincount, index, &client, min);
            HideAdjTopBottom(hwindows, hwincount, index);
            index =  - 1;
        }
        for (j = 0; j < wincount; j++) {
            docks[windows[j]]->colindex = j;
        }
        SizeTopBottom(windows, wincount, client.left, &client, &oldclient)
            ;
        if (recalhidden)
        {
            totalsize = 0;
            for (j = 0; j < wincount; j++)
            {
                totalsize += docks[j]->hiddenwidth = (docks[j]->position.bottom
                    - docks[j]->position.top) *1000;
            }
            for (j = 0; j < hwincount; j++)
            {
                if (docks[j]->hidden)
                {
                    docks[j]->hiddenwidth = totalsize / wincount;
                }
            }
        }
        if (cache[windows[0]]->type != LSTOOLBAR)
        {
            InsertVertFrame(windows[0], 0, &client);
            client.left = FRAMEWIDTH;
        }
        else
            client.left = 0;
        client.left += docks[windows[0]]->position.right;
    }
    if (index >= 0 && (docks[index]->flags &DOCK_RIGHT))
        InsertColumn(index, DOCK_RIGHT);
    indexmax =  - 1;
    for (i = 0, min =  - 1; i < currentWindows; i++)
    if (docks[i]->flags &DOCK_RIGHT)
    {
        if (docks[i]->rowindex > indexmax)
            indexmax = docks[i]->rowindex;
    }
    adj2 = 0;
    for (i = 0; i <= indexmax; i++)
    {
        found = FALSE, hidden = FALSE;
        hwincount = 0;
        wincount = 0;
        for (j = 0; j < currentWindows; j++)
        if ((docks[j]->flags &DOCK_RIGHT) && docks[j]->rowindex == i)
        {
            docks[j]->rowindex -= adj2;
            if (!docks[j]->hidden)
            {
                found = TRUE;
                min = docks[j]->position.right;
                if (j != index)
                    windows[wincount++] = j;
            }
            else
                hidden = TRUE;
            hwindows[hwincount++] = j;
        }
        if (!found)
        {
            if (!hidden)
                adj2++;
            continue;
        }
        OrderTopBottom(windows, wincount, FALSE);
        if (index >= 0 && docks[index]->rowindex == i - adj2 && (docks[index]
            ->flags &DOCK_RIGHT) && !docks[index]->hidden)
        {
            wincount += InsertTopBottom(windows, wincount, index, &client, min);
            HideAdjTopBottom(hwindows, hwincount, index);
            index =  - 1;
        }
        for (j = 0; j < wincount; j++) {
            docks[windows[j]]->colindex = j;
        }
        SizeTopBottom(windows, wincount, client.right, &client,
            &oldclient);
        if (recalhidden)
        {
            totalsize = 0;
            for (j = 0; j < wincount; j++)
            {
                totalsize += docks[j]->hiddenwidth = (docks[j]->position.bottom
                    - docks[j]->position.top) *1000;
            }
            for (j = 0; j < hwincount; j++)
            {
                if (docks[j]->hidden)
                {
                    docks[j]->hiddenwidth = totalsize / wincount;
                }
            }
        }
        if (cache[windows[0]]->type != LSTOOLBAR)
        {
            InsertVertFrame(windows[0], 1, &client);
            client.right =  - FRAMEWIDTH;
        }
        else
            client.right = 0;
        client.right += docks[windows[0]]->position.left;
    }
    srctabheight = SRCTABHEIGHT;
    bottom = client.bottom;
    if (srctabheight)
    {
        client.bottom = client.top + srctabheight;
        PutWindow(hwndSrcTab, &client);
        if (!IsWindowVisible(hwndSrcTab))
            ShowWindow(hwndSrcTab, SW_SHOW);
        client.top += srctabheight;
        client.bottom = bottom;
    }
    else
    {
        if (IsWindowVisible(hwndSrcTab))
            ShowWindow(hwndSrcTab, SW_HIDE);
    }
    if (PropGetBool(NULL, "BROWSE_INFORMATION") && !defaultWorkArea)
    {
        srctabheight = SendMessage(hwndJumpList, WM_GETHEIGHT, 0, 0);
        client.bottom = client.top + srctabheight;
        PutWindow(hwndJumpList, &client);
        if (!IsWindowVisible(hwndJumpList))
            ShowWindow(hwndJumpList, SW_SHOW);
        client.top += srctabheight;
        client.bottom = bottom;
    }
    else
    {
        if (IsWindowVisible(hwndJumpList))
            ShowWindow(hwndJumpList, SW_HIDE);
    }
    PutWindow(hwndClient, &client);
    for (i=0 ; i < frameCount; i++)
    {
        ShowWindow(frames[i]->hwnd, SW_SHOW);
        InvalidateRect(frames[i]->hwnd, 0, 1);	
    }

    /* put out docked windows, and self if just undocked */
    for (i = 0; i < currentWindows; i++)
        if (docks[i]->flags)
        {
            PutWindow(cache[i]->parent->hwnd, &docks[i]->position);
            UpdateWindow(cache[i]->parent->hwnd);
            ShowWindow(cache[i]->parent->hwnd, docks[i]->hidden ? SW_HIDE : SW_SHOW);
            ShowWindow(cache[i]->hwnd, docks[i]->hidden ? SW_HIDE : SW_SHOW);
        }
    for (i = 0; i < blankCount; i++)
    {
        PutWindow(blanks[i]->hwnd, &blanks[i]->u.bw.position);
        ShowWindow(blanks[i]->hwnd, SW_SHOW);
    }
}
//-------------------------------------------------------------------------

void DoDocks(CCW_params *p)
{
    int index;
    int resetLastPos = FALSE;
    CCD_params *d = FindParams(p, &index);
    RECT r;
    GetFrameWindowRect(&r);
    d->oldflags = d->flags;
    d->lastposition = d->position;
    d->position = lastbound;

    /*now do docking */
    if (suggestedDock)
    {
        if (d->flags &DOCK_PRESENT)
        {
            moverect = d->oldsize;
        }
        d->flags = suggestedDock | DOCK_PRESENT;
        switch (suggestedDock &~DOCK_PRESENT)
        {
            case DOCK_LEFT:
            case DOCK_RIGHT:
                p->vertical = p->type == LSTOOLBAR;
                break;
            case DOCK_TOP:
            case DOCK_BOTTOM:
                p->vertical = p->type != LSTOOLBAR;
                break;

        }
        AllocGrip(p, &lastbound, p->vertical, TRUE, d->hidden);
        d->flexparams |= FLEX_UNREQ;
    }
    else
    {
        if (d->flags &~DOCK_PRESENT)
        {
            p->vertical = FALSE;
            d->flags = 0;
            d->flexparams = 0;
            resetLastPos = TRUE;
            if (p->type == LSTOOLBAR)
            {
                d->position.bottom = d->position.top + p->u.tb.hsize.cy + 2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION);
                d->position.right = d->position.left + p->u.tb.hsize.cx + 2 * GetSystemMetrics(SM_CXFRAME) ;
            }
        }
        
        GetWindowRect(hwndClient, &r);
        OffsetRect(&d->position, -r.left, -r.top);
        AllocContainer(p, &d->position);
    }
    CalculateLayout(index, 0);
    if (d->flags &~DOCK_PRESENT)
    {
        OffsetRect(&moverect,  - moverect.left,  - moverect.top);
        d->oldsize = moverect;
    }
    /*
    if (!d->flags)
    {
        GetWindowRect(hwndClient, &r);
        // keep it out from under the source tab window
        if (d->position.top < r.top)
        {
            d->position.bottom += r.top - d->position.top;
            d->position.top += r.top - d->position.top;
        }
        PutWindow(p->parent->hwnd, &d->position);
        ShowWindow(p->hwnd, SW_SHOW);
        SendMessage(p->parent->hwnd, LCF_SETFLEX, 0, 0);
    }
    */
    if (resetLastPos)
        d->lastposition = d->position;
}

//-------------------------------------------------------------------------

void CalculateSizebarBounds(CCW_params *p, RECT *dest)
{
    int i;
    RECT r1;
    int min1 = 100000, min2 = 100000;
    GetFrameWindowRect(dest);
    dest->right -= 3 * FRAMEWIDTH;
    dest->left += 3 * FRAMEWIDTH;
    dest->top += 3 * FRAMEWIDTH;
    dest->bottom -= 3 * FRAMEWIDTH;
    for (i = 0; i < frameCount; i++)
    {
        if (p != frames[i])
        {
            if (frames[i]->vertical == p->vertical)
            {
                if (!p->vertical)
                {
                    if (frames[i]->u.fw.position.bottom > p->u.fw.position.bottom)
                    {
                        if (min1 > frames[i]->u.fw.position.top - p->u.fw.position.bottom)
                            min1 = frames[i]->u.fw.position.top - p->u.fw.position.bottom;
                    }
                    else
                    {
                        if (min2 > p->u.fw.position.top - frames[i]->u.fw.position.bottom)
                            min2 = p->u.fw.position.top - frames[i]->u.fw.position.bottom;
                    }
                }
            }
        }
    }
    if (p->vertical)
    {
        if (min1 != 100000)
            dest->right = p->u.fw.position.right + min1 - FRAMEWIDTH * 3;
        if (min2 != 100000)
            dest->left = p->u.fw.position.left - min2 + FRAMEWIDTH * 3;
    }
    else
    {
        if (min1 != 100000)
            dest->bottom = p->u.fw.position.bottom + min1 - FRAMEWIDTH * 3;
        if (min2 != 100000)
            dest->top = p->u.fw.position.top - min2 + FRAMEWIDTH * 3;
    }
}

//-------------------------------------------------------------------------

void Resize(CCW_params *p, RECT *r)
{
    int i, j, changed, left =  - 1, right =  - 1, leftw, rightw, row;
    RECT r1;
    GetWindowRect(hwndClient, &r1);
    changed = FALSE;
    if (!p->vertical)
    {
        if (p->u.fw.position.top == r1.bottom)
        {
            r1.bottom = r->top;
            changed = TRUE;
        }
        else if (p->u.fw.position.bottom == r1.top)
        {
            r1.top = r->bottom;
            changed = TRUE;
        }
    }
    else
    {
        if (p->u.fw.position.left == r1.right)
        {
            r1.right = r->left;
            changed = TRUE;
        }
        else if (p->u.fw.position.right == r1.left)
        {
            r1.left = r->right;
            changed = TRUE;
        }
    }
    if (changed)
    {
        int srctabheight;
        PutWindow(hwndClient, &r1);
        if (PropGetBool(NULL, "BROWSE_INFORMATION") && !defaultWorkArea)
        {
            srctabheight = SendMessage(hwndJumpList, WM_GETHEIGHT, 0, 0);
            r1.bottom = r1.top;
            r1.top -= srctabheight;
            PutWindow(hwndJumpList, &r1);
        }   
        srctabheight = SRCTABHEIGHT;
        r1.bottom = r1.top;
        r1.top -= srctabheight;
        PutWindow(hwndSrcTab, &r1);
    }

    // move windows
    for (i = 0; i < currentWindows; i++)
    {
        changed = FALSE;
        if (docks[i]->flags)
        {
            if (!p->vertical)
            {
                if (p->u.fw.position.top == docks[i]->position.bottom)
                {
                    docks[i]->position.bottom = r->top;
                    left = i;
                    row = docks[i]->rowindex;
                    changed = TRUE;
                }
                else if (p->u.fw.position.bottom == docks[i]->position.top)
                {
                    docks[i]->position.top = r->bottom;
                    right = i;
                    row = docks[i]->rowindex;
                    changed = TRUE;
                }
            }
            else
            {
                if (p->u.fw.position.left == docks[i]->position.right)
                {
                    docks[i]->position.right = r->left;
                    left = i;
                    row = docks[i]->rowindex;
                    changed = TRUE;
                }
                else if (p->u.fw.position.right == docks[i]->position.left)
                {
                    docks[i]->position.left = r->right;
                    right = i;
                    row = docks[i]->rowindex;
                    changed = TRUE;
                }
            }
            if (changed)
            {
                PutWindow(cache[i]->parent->hwnd, &docks[i]->position);
                ShowWindow(cache[i]->hwnd, SW_SHOW);
            }
        }
    }
    // recalculate hidden widths for later
    if (left >= 0 && right >= 0)
    {
        leftw = docks[left]->position.right - docks[left]->position.left;
        rightw = docks[right]->position.right - docks[right]->position.left;
        i = docks[left]->hiddenwidth + docks[right]->hiddenwidth;
        docks[left]->hiddenwidth = i * leftw / (leftw + rightw);
        docks[right]->hiddenwidth = i * rightw / (leftw + rightw);
    }

    // move frames
    for (i = 0; i < frameCount; i++)
    {
        changed = FALSE;
        if (!p->vertical)
        {
            if (frames[i]->u.fw.position.bottom == p->u.fw.position.top)
            {
                frames[i]->u.fw.position.bottom = r->top;
                changed = TRUE;
            }
            else if (frames[i]->u.fw.position.top == p->u.fw.position.bottom)
            {
                frames[i]->u.fw.position.top = r->bottom;
                changed = TRUE;
            }
        }
        else
        {
            if (frames[i]->u.fw.position.right == p->u.fw.position.left)
            {
                frames[i]->u.fw.position.right = r->left;
                changed = TRUE;
            }
            else if (frames[i]->u.fw.position.left == p->u.fw.position.right)
            {
                frames[i]->u.fw.position.left = r->right;
                changed = TRUE;
            }
        }
        if (changed)
            PutWindow(frames[i]->hwnd, &frames[i]->u.fw.position);
    }
    PutWindow(p->hwnd, r);
}

//-------------------------------------------------------------------------

void CalculateHidden(CCD_params *d, int index, int state)
{
    int j, unhiddenwidth = 0, totalwidth = 0;
    int windows[30], wincount = 0;
    RECT r;
    int sizewidth, scale;
    d->flexparams |= FLEX_UNREQ;
    if (d->flags &(DOCK_TOP | DOCK_BOTTOM))
    {
        GetFrameWindowRect(&r);
        sizewidth = r.right - r.left;
    }
    else
    {
        GetWindowRect(hwndClient, &r);
        sizewidth = r.bottom - r.top;
    }
    for (j = 0; j < currentWindows; j++)
    if ((docks[j]->flags == d->flags) && docks[j]->rowindex == d->rowindex)
    {
        windows[wincount++] = j;
        totalwidth += docks[j]->hiddenwidth;
        if (!docks[j]->hidden)
            unhiddenwidth += docks[j]->hiddenwidth;
    }
    if (unhiddenwidth == 0)
        return ;
    //   if (d->flags & (DOCK_TOP | DOCK_BOTTOM))
    //      OrderLeftRight(windows,wincount,FALSE) ;
    //   else
    //      OrderTopBottom(windows,wincount,FALSE) ;
    scale = unhiddenwidth / sizewidth;
    totalwidth = totalwidth * 1000 / scale;
    for (j = 0; j < wincount; j++)
    {
        int x = docks[windows[j]]->hiddenwidth = docks[windows[j]]->hiddenwidth
            *1000 / scale;
        if (!docks[windows[j]]->hidden)
        if (docks[windows[j]]->flags &(DOCK_TOP | DOCK_BOTTOM))
        {
            docks[windows[j]]->position.right = docks[windows[j]]
                ->position.left + x * sizewidth / totalwidth;
        }
        else
        {
            docks[windows[j]]->position.bottom = docks[windows[j]]
                ->position.top + x * sizewidth / totalwidth;
        }
    }
}

//-------------------------------------------------------------------------

int dmgrGetInfo(CCW_params *p, CCD_params *d)
{
    int i;
    RECT r;
//    GetWindowRect(hwndClient, &r);
    for (i = 0; i < currentWindows; i++)
    {
//        GetWindowRect(cache[i]->parent->hwnd, &docks[i]->position);
        p[i] = *(cache[i]);
        d[i] = *(docks[i]);
//        if (!docks[i]->flags)
//        {
//            OffsetRect(&d[i].position,  - r.left,  - r.top);
//            OffsetRect(&d[i].lastposition,  - r.left,  - r.top);
//        }
    }
    return currentWindows;
}

//-------------------------------------------------------------------------

int dmgrSetInfo(int *ids, CCD_params *d, int size)
{
    int i, j;
    int rv = 1;
    RECT r;
    for (j = 0; j < size; j++)
    {
        for (i = 0; i < currentWindows; i++)
            if (ids[j] == cache[i]->id)
            {
                CCD_params *s = docks[i];
                CCW_params *p = cache[i];
                int oldflags = s->flags;
                s->flexparams = FLEX_UNREQ;
                s->rowindex = d->rowindex;
                s->colindex = d->colindex;
                s->hidden = d->hidden;
                s->hiddenwidth = d->hiddenwidth;
                s->oldsize = d->oldsize;
                s->position = d->position;
                s->lastposition = d->position;
                s->flags = s->oldflags = d->flags;
                if (s->flags)
                {
                    s->flags |= DOCK_PRESENT;
                    s->oldflags |= DOCK_PRESENT;
                }
                if (p->type == LSTOOLBAR)
                {
                      int vertical = p->vertical = s->flags &(DOCK_LEFT | DOCK_RIGHT) ? 1 : 0;
                    if (s->flags)
                    {
                        if (!vertical)
                        {
                            s->lastposition.right = s->lastposition.left + p
                                ->u.tb.hsize.cx + GRIPWIDTH;
                            s->position.right = s->position.left + p->u.tb.hsize.cx
                                + GRIPWIDTH;
                            s->oldsize.right = s->oldsize.left + p->u.tb.hsize.cx +
                                GRIPWIDTH;
                            s->lastposition.bottom = s->lastposition.top + p
                                ->u.tb.hsize.cy;
                            s->position.bottom = s->position.top + p->u.tb.hsize.cy;
                            s->oldsize.bottom = s->oldsize.top + p->u.tb.hsize.cy +
                                GetSystemMetrics(SM_CYCAPTION);
                        }
                        else
                        {
                            s->lastposition.bottom = s->lastposition.top + p
                                ->u.tb.vsize.cy + GRIPWIDTH;
                            s->position.bottom = s->position.top + p->u.tb.vsize.cy
                                + GRIPWIDTH;
                            s->oldsize.bottom = s->oldsize.top + p->u.tb.vsize.cx +
                                GRIPWIDTH;
                            s->lastposition.right = s->lastposition.left + p
                                ->u.tb.vsize.cx;
                            s->position.right = s->position.left + p->u.tb.vsize.cx;
                            s->oldsize.right = s->oldsize.left + p->u.tb.vsize.cx +
                                2 * GetSystemMetrics(SM_CXFRAME);
                        }
                    }
                }
                else
                {
                    p->vertical = s->flags &(DOCK_TOP | DOCK_BOTTOM) ? 1 : 0;
                }
                if (s->flags || s->hidden)
                    AllocGrip(p, &s->position, p->vertical, FALSE, s->hidden);
                else
                    AllocContainer(p, &s->position);
                break;
            }
        if (i >= currentWindows)
            rv = 0;
        d++;
    }
    CalculateLayout( - 2, 1);
    GetWindowRect(hwndClient, &r);
    for (j = 0; j < size; j++)
        for (i = 0; i < currentWindows; i++)
            if (ids[j] == cache[i]->id)
            {
                CCD_params *s = docks[i];
                CCW_params *p = cache[i];
                if (!s->flags && !s->hidden)
                {
                    OffsetRect(&s->position, r.left, r.top);
                    OffsetRect(&s->lastposition, r.left, r.top);
                    PutWindow(p->parent->hwnd, &s->position);
                    ShowWindow(p->parent->hwnd, s->hidden ? SW_HIDE : SW_SHOW);
                }
            }
    return rv;
}

//-------------------------------------------------------------------------

void dmgrSetRundown(void)
{
    DestroyWindow(hwndFocus);
    rundown = 1;
}

//-------------------------------------------------------------------------

int adjustforbadplacement(int id, CCD_params *d)
{
    RECT r;
    if (d->flags)
        return FALSE ;
    GetClientRect(hwndClient, &r);
    if (d->oldsize.left < 0 || d->oldsize.right - d->oldsize.left < 40 || d
        ->oldsize.bottom - d->oldsize.top < GetSystemMetrics(SM_CYCAPTION) || d->position.right - d
        ->position.left < 40 || d->position.bottom - d->position.top < 40 || d
        ->position.left < 0 || d->position.top < 0 || d->position.left >
        r.right - 64 || d->position.top > r.bottom - GetSystemMetrics
        (SM_CYCAPTION) || d->lastposition.right - d->lastposition.left < 40 ||
        d->lastposition.bottom - d->lastposition.top < 40 || d
        ->lastposition.left < 0 || d->lastposition.top < 0 || d
        ->lastposition.left > r.right - 64 || d->lastposition.top > r.bottom -
        GetSystemMetrics(SM_CYCAPTION))
    {
        d->oldsize.left = d->oldsize.right = 0;
        d->position.top = d->position.left = 0;
        switch (id)
        {
            case DID_PROPSWND:
                d->oldsize.right = 200;
                d->oldsize.bottom = 200;
                break;
            case DID_TABWND:
                d->oldsize.right = 200;
                d->oldsize.bottom = 200;
                break;
            case DID_WATCHWND:
                d->oldsize.right = 200;
                d->oldsize.bottom = 200;
                break;
            case DID_ERRORWND:
                d->oldsize.right = 200;
                d->oldsize.bottom = 200;
                break;
            case DID_ASMWND:
                d->oldsize.right = 500;
                d->oldsize.bottom = 200;
                break;
            case DID_MEMWND:
                d->oldsize.right = 80 * 8;
                d->oldsize.bottom = 19 * 8+40;
                break;
            case DID_STACKWND:
                d->oldsize.right = 30 * 8;
                d->oldsize.bottom = 19 * 8;
                break;
            case DID_THREADWND:
                d->oldsize.right = 30 * 8;
                d->oldsize.bottom = 19 * 8;
                break;
            case DID_REGWND:
                d->oldsize.right = 200;
                d->oldsize.bottom = 400;
        }
        d->lastposition = d->oldsize;
        d->position.right = d->position.left + d->oldsize.right;
        d->position.bottom = d->position.top + d->oldsize.bottom;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

void SaveDocksToRegistry(void)
{
    int i;
//    CCW_params p[20];
//    CCD_params d[20];
    char regname[256];
    char iobuf[512];
    int len;
    WINDOWPLACEMENT wp;
    wp.length = sizeof(wp);
    GetWindowPlacement(hwndFrame, &wp);
    sprintf(iobuf, "%d %d %d %d %d %d %d %d %d %d", wp.flags, wp.showCmd,
        wp.ptMinPosition.x, wp.ptMinPosition.y, wp.ptMaxPosition.x,
        wp.ptMaxPosition.y, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
        wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
    StringToProfile("Placement", iobuf);
#ifdef XXXXX
    len = dmgrGetInfo(&p[0], &d[0]);
    for (i = 0; i < len; i++)
    {
        sprintf(regname, "DOCK%d", i);
        sprintf(iobuf, 
            "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d>",
            p[i].id, d[i].flags, d[i].flexparams, d[i].rowindex, d[i].colindex,
            d[i].hidden, d[i].hiddenwidth, d[i].oldsize.left, d[i].oldsize.top,
            d[i].oldsize.right, d[i].oldsize.bottom, d[i].position.left,
            d[i].position.top, d[i].position.right, d[i].position.bottom,
            d[i].lastposition.left, d[i].lastposition.top,
            d[i].lastposition.right, d[i].lastposition.bottom);
        StringToProfile(regname, iobuf);
    }
#endif
}

//-------------------------------------------------------------------------

int RestoreDocksFromRegistry(void)
{
#ifdef XXXXX
    char *buf, regname[256];
    CCD_params d[20];
    int ids[20];
    int count = 0;
    while (count < 20)
    {
        sprintf(regname, "DOCK%d", count);
        buf = ProfileToString(regname, "");
        if (!buf[0])
            break;
        sscanf(buf, 
            "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d>\n", 
            &ids[count],  &d[count].flags, &d[count].flexparams, 
            &d[count].rowindex, &d[count].colindex,  &d[count].hidden,
            &d[count].hiddenwidth,  &d[count].oldsize.left,
            &d[count].oldsize.top, &d[count].oldsize.right,
            &d[count].oldsize.bottom,  &d[count].position.left,
            &d[count].position.top, &d[count].position.right,
            &d[count].position.bottom,  &d[count].lastposition.left,
            &d[count].lastposition.top, &d[count].lastposition.right,
            &d[count].lastposition.bottom);
        adjustforbadplacement(ids[count], &d[count]);
        count++;
    }
    if (count)
        dmgrSetInfo(ids, &d[0], count);
#endif
    return 0;
}

//-------------------------------------------------------------------------

int RestorePlacementFromRegistry(WINDOWPLACEMENT *wp)
{
    char *buf = ProfileToString("Placement", "");
    if (buf[0])
    {
        wp->length = sizeof(WINDOWPLACEMENT);
        sscanf(buf, "%d %d %d %d %d %d %d %d %d %d",  &wp->flags, &wp->showCmd,
            &wp->ptMinPosition.x, &wp->ptMinPosition.y,  &wp->ptMaxPosition.x,
            &wp->ptMaxPosition.y,  &wp->rcNormalPosition.left, &wp
            ->rcNormalPosition.top,  &wp->rcNormalPosition.right, &wp
            ->rcNormalPosition.bottom);
        return 1;
    }
    return 0;
}
