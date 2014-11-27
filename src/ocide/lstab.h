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
#ifndef _LSTAB_H
#define _LSTAB_H

struct _stList
{
    struct _singleTab *head;
    struct _singleTab *tail;
};
struct _stItem
{
    struct _singleTab *prev;
    struct _singleTab *next;
};
struct _singleTab
{
    struct _stItem chain;
    struct _stItem selectedChain;
    char *text;
    LPARAM lParam;
    BOOL displayed;
    BOOL modified;
    RECT displayRect;
    RECT closeBtn;
}; 
struct _tabStruct
{
    struct _stList active;
    struct _stList selected;
    struct _singleTab *highlighted;
    int fontHeight;
    HFONT normalFont, boldFont;
    RECT windowBtn;
    HPEN greyPen;
    HPEN whitePen;
    HWND toolTip;
    HBITMAP xBitmap, xBitmapSelected;
    HBITMAP xBack;
    HBITMAP menuBitmap;
     HBITMAP menuBitmap2;
    RECT xRect;
    BOOL captured;
    BOOL flat;
    BOOL displayMenu;
    DWORD windowBtnMode;
    BOOL dragging;
    HCURSOR oldCursor;
    struct _singleTab *dragSrc;
    struct _singleTab **menuList;
} ;

struct ttrack
{
    HANDLE tWait;
    HWND hwnd;
    HMENU menu;
    int rows;
    int cols;
    int rowHeight;
    int rowWidth;
    BOOL inWindow;
    int id;
    int oldTrack;
} ;

#endif