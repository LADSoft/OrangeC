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
extern HINSTANCE hInstance;

HWND hwndeditPopup;

BOOL GetWordSpan(EDITDATA *ptr, CHARRANGE *range)
{
    int bottom = range->cpMin;
    int top = range->cpMax;
    while (keysym(ptr->cd->text[top].ch))
        top++;
    while (bottom > 0 && keysym(ptr->cd->text[bottom-1].ch))
        bottom--;
    if (!isdigit(ptr->cd->text[bottom].ch))
    {
        range->cpMin = bottom;
        range->cpMax = top;
        return TRUE;
    }
    return FALSE;
}
int CPPScanForward(EDITDATA *p, int pos, BOOL asExpression)
{
    int rv = pos;
    char parenNesting[400];
    int parenLevel = 0;
    while (1)
    {
        while (isspace(p->cd->text[pos].ch))
            pos++;
        switch(p->cd->text[pos].ch)
        {
            case '}':
            case '{':
            case ';':
                return rv;
        }
        if (parenLevel && (isalpha(p->cd->text[pos].ch) || p->cd->text[pos].ch == '_'))
        {
            CHARRANGE range;
            range.cpMin = range.cpMax = pos;
            GetWordSpan(p, &range);
            pos = range.cpMax;
        }
        else if (asExpression && (p->cd->text[pos].ch == '&' || p->cd->text[pos].ch == '*'))
        {
            pos++;
        }
        else if (asExpression && p->cd->text[pos].ch == '.')
        {
            pos++;
        }
        else if (asExpression && p->cd->text[pos].ch == '-')
        {
            if (p->cd->text[pos+1].ch == '>')
                pos += 2;
            else
                return rv;
        }
        else if (asExpression && p->cd->text[pos].ch == '(')
        {
            pos++;
            parenNesting[parenLevel++] = ')';
        }
        else if (asExpression && p->cd->text[pos].ch == '[')
        {
            pos++;
            parenNesting[parenLevel++] = ']';
        }
        else if (asExpression && (p->cd->text[pos].ch == ')' || p->cd->text[pos].ch == ']'))
        {
            if (!parenLevel || parenNesting[parenLevel--] != p->cd->text[pos].ch)
                return rv;
            else pos ++;
            
            if (!--parenLevel)
                rv = pos;
        }
        else if (p->cd->text[pos].ch == ':')
        {
            if (p->cd->text[pos+1].ch == ':')
            {
                CHARRANGE x;
                pos += 2;
                while (isspace(p->cd->text[pos].ch))
                    pos++;
                x.cpMin = x.cpMax = pos;
                if (!GetWordSpan(p, &x))
                    return rv;
                rv = pos = x.cpMax;
            }
            else
            {
                break;
            }
        }        
        else if (p->cd->text[pos].ch == '<')
        {
            pos++;
            while (isspace(p->cd->text[pos].ch))
                pos++;
            if (p->cd->text[pos].ch == '>')
            {
                rv = pos++;
            }
            else while (TRUE)
            {
                CHARRANGE x;
                while (isspace(p->cd->text[pos].ch))
                    pos++;
                x.cpMin = x.cpMax = pos;
                if (!GetWordSpan(p, &x))
                    return rv;
                pos = x.cpMax;
                pos = CPPScanForward(p, pos, asExpression);
                while (isspace(p->cd->text[pos].ch))
                    pos++;
                if (p->cd->text[pos].ch == '>')
                {
                    rv = pos++;
                    break;
                }
                else if (p->cd->text[pos].ch != ',')
                    return rv;
                pos++;

            }
        }
        else
            return rv;
    }
}
int CPPScanBackward(EDITDATA *p , int pos, BOOL asExpression)
{
    int rv = pos;
    char parenNesting[400];
    int parenLevel = 0;
    while (pos > 0)
    {
        while (pos > 0 && isspace(p->cd->text[pos-1].ch))
            pos--;
        if (pos)
        {
            switch(p->cd->text[pos-1].ch)
            {
                case '}':
                case '{':
                case ';':
                    return rv;
            }
        }
        if (asExpression && pos && keysym(p->cd->text[pos-1].ch))
        {
            CHARRANGE range;
            range.cpMin = range.cpMax = pos;
            GetWordSpan(p, &range);
            if (range.cpMin == range.cpMax)
                return rv;
            pos = range.cpMin;
            if (!parenLevel)
                rv = pos;
        }
        else if (asExpression && pos && (p->cd->text[pos-1].ch == '&' || p->cd->text[pos-1].ch == '*'))
        {
            pos--;
            if (!parenLevel)
                rv = pos;
        }
        else if (asExpression && p->cd->text[pos-1].ch == '.')
        {
            pos--;
        }
        else if (asExpression && pos > 1 && p->cd->text[pos-2].ch == '-' && p->cd->text[pos-1].ch == '>')
        {
            pos -= 2;
        }        
        else if (asExpression && pos && p->cd->text[pos-1].ch == ')')
        {
            pos--;
            parenNesting[parenLevel++] = '(';
        }
        else if (asExpression && pos && p->cd->text[pos-1].ch == ']')
        {
            pos--;
            parenNesting[parenLevel++] = '[';
        }
        else if (asExpression && pos && (p->cd->text[pos-1].ch == '(' || p->cd->text[pos-1].ch == '['))
        {
            if (!parenLevel || parenNesting[parenLevel-1] != p->cd->text[pos-1].ch)
                return rv;
            else pos --;
            if (!--parenLevel)
                rv = pos;
        }
        else if (pos > 0 && p->cd->text[pos-1].ch == ':')
        {
            if (p->cd->text[pos-2].ch == ':')
            {
                CHARRANGE temp;
                pos -= 2;
                while (pos > 0 && isspace(p->cd->text[pos-1].ch))
                    pos--;
                temp.cpMin = temp.cpMax = pos;
                if (!GetWordSpan(p, &temp))
                    return rv;
                rv = pos = temp.cpMin;

            }
            else
                break;
        }
        else if (pos > 0 && p->cd->text[pos-1].ch == '>')
        {
            pos--;
            while (pos > 0 && isspace(p->cd->text[pos-1].ch))
                pos--;
            if (pos > 0 && p->cd->text[pos-1].ch == '>')
            {
                rv = --pos;
            }
            else while (pos > 0)
            {
                int n = CPPScanBackward(p, pos, asExpression);
                if (pos == n)
                    return rv;
                pos = n;
                while (pos > 0 && isspace(p->cd->text[pos-1].ch))
                    pos--;
                if (pos > 0 && p->cd->text[pos-1].ch == '<')
                {
                    CHARRANGE temp;
                    pos--;
                    while (pos > 0 && isspace(p->cd->text[pos-1].ch))
                        pos--;
                    temp.cpMin = temp.cpMax = pos;
                    if (!GetWordSpan(p, &temp))
                        return rv;;
                    rv = pos = temp.cpMin;
                }
                else if (pos > 0 && p->cd->text[pos-1].ch == ',')
                {
                    pos--;
                }
                else
                    return rv;
            }
        }
        else
            return rv;
    }
    return rv;
}
void GetCPPWordSpan(EDITDATA *ptr, CHARRANGE *range, BOOL asExpression)
{
    if (GetWordSpan(ptr, range))
    {
        range->cpMax = CPPScanForward(ptr, range->cpMax, asExpression);
        range->cpMin = CPPScanBackward(ptr, range->cpMin, asExpression);
        
    }
}
/**********************************************************************
 * GetWordFromPos is a utility to find out what the word under the
 * cursor is, and various information about it
 **********************************************************************/
int GetWordFromPos(HWND hwnd, char *outputbuf, int fullcpp, int charpos, int *linenum, int
    *startoffs, int *endoffs)
{
    int linepos;
    int linecharpos;
    int linecharindex;
    char buf[1000];
    CHARRANGE charrange;

    EDITDATA *p = (EDITDATA*)GetWindowLong(hwnd, 0);

    if (charpos ==  - 1)
    {
        SendMessage(hwnd, EM_EXGETSEL, (WPARAM)0, (LPARAM) &charrange);
        if (charrange.cpMax < charrange.cpMin)
        {
            int in = charrange.cpMax;
            charrange.cpMax = charrange.cpMin;
            charrange.cpMin = in;
        }
        charpos = charrange.cpMax;
    }
    else 
    {
        charrange.cpMin = charrange.cpMax = charpos;
    }
    linepos = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, (LPARAM)charpos);

    if (charrange.cpMin == charrange.cpMax)
    {
        if (fullcpp)
            GetCPPWordSpan(p, &charrange, fullcpp == 2);
        else
            GetWordSpan(p, &charrange);
    }
    if (charrange.cpMin != charrange.cpMax)
    {
        int i;
        if (charrange.cpMax - charrange.cpMin > 240)
            return FALSE;
        for (i= charrange.cpMin; i < charrange.cpMax; i++)
            outputbuf[i - charrange.cpMin] = p->cd->text[i].ch;
        outputbuf[i-charrange.cpMin] = 0;
        if (linenum)
            *linenum = linepos;
        if (startoffs)
            *startoffs = charrange.cpMin;
        if (endoffs)
            *endoffs = charrange.cpMax;
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
 * DoHelp handles the F1 key functionality - it gets the word under the
 * cursor then tries a lookup from the favorite help engine.  Again this
 * is kind of linked to the rest of CCIDE
 **********************************************************************/

void DoHelp(HWND edwin, int speced)
{
    char buf[512];
    if (!GetWordFromPos(edwin, buf,  FALSE, - 1, 0, 0, 0))
        return ;
    else
    {
        if (GetKeyState(VK_CONTROL) &0x80000000)
            MSDNHelp(buf);
        else
            RTLHelp(buf);
    }
}



//-------------------------------------------------------------------------


/**********************************************************************
 * ClientArea gets the client area.  We are leaving space at the bottom
 * because it would be overlayed with the scroll bar
 **********************************************************************/
void ClientArea(HWND hwnd, EDITDATA *p, RECT *r)
{
    GetClientRect(hwnd, r);
    //   r->bottom -= GetSystemMetrics(SM_CYHSCROLL) ;
    r->bottom -= r->bottom % (p->cd->txtFontHeight ? p->cd->txtFontHeight : 2);
}

    void GetEditPopupFrame(RECT *rect)
    {
        GetFrameWindowRect(rect);
        rect->top -= GetSystemMetrics(SM_CYMENU);
    }
    void PopupFullScreen (HWND hwnd, EDITDATA *p)
    {
        if (!(GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP))
        {
            RECT wrect;
            GetEditPopupFrame(&wrect);
            hwndeditPopup = CreateWindow("xedit","", 
                                       (WS_POPUP | WS_CHILD),
                                       wrect.left, wrect.top, 
                                       wrect.right - wrect.left, wrect.bottom - wrect.top,
                                       GetParent(hwnd),0, hInstance, p);
            SetWindowPos(hwndeditPopup, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
        }
    }
    void ReleaseFullScreen(HWND hwnd, EDITDATA *p)
    {
        if (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP)
        {
            hwndeditPopup = NULL;
            CloseWindow(hwnd);
            DestroyWindow(hwnd);
        }
    }
    void SendUpdate(HWND hwnd)
    {
        SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_UPDATE | 
            (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
    }
int KeyboardToAscii(int vk, int scan, BOOL fullstate)
{
    WCHAR data[10];
    BYTE state[256];
    data[0] = 0;
    if (fullstate)
        GetKeyboardState(state);
    else
        memset(state, 0, sizeof(state));
    ToUnicodeEx(vk, (scan>>16) & 0xff, state, data, 10, 0, 0);
    return data[0];  
    
}
