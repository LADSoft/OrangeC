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


void CancelParenMatch(HWND hwnd, EDITDATA *p)
{
    if (p->matchingEnd != 0)
    {
        p->matchingEnd = p->matchingStart = 0;
        InvalidateRect(hwnd,NULL,0);
    }
}
int FindParenMatchBackward(HWND hwnd, EDITDATA *p, int dec)
{
    int skip,match;
    int level = 1;
    int s = p->selstartcharpos;
    int quotechar = 0;
    if (s != p->selendcharpos || (s == p->cd->textlen && !dec))
        return FALSE;
    if (dec && s)
        s--;
    if (p->cd->text[s].ch == ')')
        skip = ')', match = '(';
    else if (p->cd->text[s].ch == '}')
        skip = '}', match = '{';
    else if (p->cd->text[s].ch == ']')
        skip = ']', match = '[';
    else
        return FALSE;
    while (s && --s)
    {
        if (quotechar == p->cd->text[s].ch && (!s || p->cd->text[s-1].ch != '\\' 
                                               || s < 2 || p->cd->text[s-2].ch == '\\'))
            quotechar = 0;
        else if (!quotechar)
        {
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"')
                && (!s || p->cd->text[s-1].ch != '\\'
                    || s < 2 || p->cd->text[s-2].ch == '\\'))
                quotechar = p->cd->text[s].ch;
            else if (p->cd->text[s].ch == skip)
                level++;
            else if (p->cd->text[s].ch == match)
            {
                if (!--level)
                    break;
            }
        }
    }
    if (level)
        return FALSE;
    p->matchingStart = p->selstartcharpos - dec;
    p->matchingEnd = s;
    InvalidateRect(hwnd, NULL, FALSE);
    return TRUE;
}
int FindParenMatchForward(HWND hwnd, EDITDATA *p, int dec)
{
    int skip,match;
    int level = 1;
    int s = p->selstartcharpos;
    int quotechar = 0;
    if (s != p->selendcharpos || (s == p->cd->textlen && !dec))
        return FALSE;
    if (dec && s)
        s--;
    if (p->cd->text[s].ch == '(')
        skip = '(', match = ')';
    else if (p->cd->text[s].ch == '{')
        skip = '{', match = '}';
    else if (p->cd->text[s].ch == '[')
        skip = '[', match = ']';
    else
        return FALSE;
    while (++s != p->cd->textlen)
    {
        if (quotechar == p->cd->text[s].ch && (p->cd->text[s-1].ch != '\\'
                                               || s < 2 || p->cd->text[s-2].ch == '\\'))
            quotechar = 0; 
        else if (!quotechar)
        {
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"')
                && (p->cd->text[s-1].ch != '\\' || s < 2 || p->cd->text[s-2].ch == '\\'))
                quotechar = p->cd->text[s].ch;
            else if (p->cd->text[s].ch == skip)
                level++;
            else if (p->cd->text[s].ch == match)
                if (!--level)
                    break;
        }
    }
    if (level)
        return FALSE;
    p->matchingStart = p->selstartcharpos - dec;
    p->matchingEnd = s;
    InvalidateRect(hwnd, NULL, FALSE);
    return TRUE;
}
void FindParenMatch(HWND hwnd, EDITDATA *p)
{
    if (PropGetBool(NULL, "MATCH_PARENTHESIS"))
        if (!FindParenMatchForward(hwnd, p ,TRUE ))
            FindParenMatchBackward(hwnd, p, TRUE);
}
void FindBraceMatchForward(HWND hwnd, EDITDATA *p)
{
    int n = 1;
    int x = p->selstartcharpos;
    while ( x && isspace(p->cd->text[x].ch))
        x--;
    if (p->cd->text[x].ch == '{')
    {
        x++;
    }
    else
    {
        x = p->selstartcharpos;
        while (isspace(p->cd->text[x].ch))
            x++ ;
        if (p->cd->text[x].ch == '{')
            x++;
    }
    if (x && p->cd->text[x-1].ch == '{')
    {
        while (n && p->cd->text[x].ch)
        {
            if (!instring(p->cd->text, p->cd->text + x))
            {
                if (p->cd->text[x].ch == '{')
                    n++;
                if (p->cd->text[x].ch == '}')
                    n--;
            }
            x++;
        }
        p->selstartcharpos = p->selendcharpos =x;
        ScrollCaretIntoView(hwnd, p, TRUE);
        PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
    }
}
void FindBraceMatchBackward(HWND hwnd, EDITDATA *p)
{
    int n = 1;
    int x = p->selstartcharpos;
    while (isspace(p->cd->text[x].ch))
        x++ ;
    if (p->cd->text[x].ch == '}')
    {
        x--;
    }
    else
    {
        x = p->selstartcharpos;
        while (x && isspace(p->cd->text[x-1].ch))
            x-- ;
        if (x && p->cd->text[x-1].ch == '}')
            x-=2;
        else
            return;
    }
    while (x && n)
    {
        if (!instring(p->cd->text, p->cd->text + x))
        {
            if (p->cd->text[x].ch == '{')
                n--;
            if (p->cd->text[x].ch == '}')
                n++;
        }
        if (!n)
            break;
        x--;
    }
    p->selstartcharpos = p->selendcharpos =x;
    ScrollCaretIntoView(hwnd, p, TRUE);
    PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
}
void FindBraceMatch(HWND hwnd, EDITDATA *p, int ch)
{
    if (ch == '{')
        FindBraceMatchBackward(hwnd, p);
    else if (ch == '}')
        FindBraceMatchForward(hwnd, p);
}
