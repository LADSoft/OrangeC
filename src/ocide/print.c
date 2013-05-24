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
#include <time.h>
#include "header.h"


extern HINSTANCE hInstance;
extern HWND hwndFrame;


static char printHeader[256] = "%F";
static char printFooter[256] = "%P of %#";
static int leftmargin = 10, rightmargin = 10;
static int topmargin = 10, bottommargin = 10;

static int printing;

static int tabSetting;
static int total_pages = 0;
static char dateBuf[256];
static char euroDateBuf[256];
static char timeBuf[256];
static char euroTimeBuf[256];

static LOGFONT fontdata = 
{
    16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH |
        FF_DONTCARE, "Courier New"
};

static BOOL CALLBACK AbortProc(HDC hDC, int error)
{
    return printing;
}

//-------------------------------------------------------------------------

static LRESULT CALLBACK CancelProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL)
                printing = FALSE;
            break;
    }
    return 0;
}

static int countPages(char *pos, int rows, int cols)
{
    int i,j ;
    int pages = 0;
    for ( ; *pos ; )
    {
        for (i = 0; *pos && i < rows; i++)
        {
            int count = 0;
            if (*pos == '\n')
            {
                pos++;
            }
            else if (*pos == '\f')
            {
                pos += 2;
                break;
            }
            else
            {
                for (j = 0; *pos && j < cols; j++, pos++)
                {
                    if (*pos == '\t')
                    {
                        
                        j += tabSetting - j % tabSetting - 1;
                    } 
                    else
                    {
                        if (*pos < 32 ||  *pos > 126)
                        {
                            if (*pos == '\n')
                            {
                                pos++;
                                break;
                            }
                        }
                    }
                }
            }
        }
        pages++;
    }
    return pages;
}
void split (char *fmt, char *left, char *right, char *center)
{
    char **active = &center;
    for ( ; *fmt; fmt++)
    {
        if (*fmt == '%')
        {
            switch(*(fmt + 1))
            {
                case 'C':
                    fmt+=1;
                    active =&center;
                    continue;
                case 'R':
                    fmt+=1;
                    active =&right;
                    continue;
                case 'L':
                    fmt+=1;
                    active =&left;
                    continue;
                default:
                    break;
            }
        }
        *(*active)++ = *fmt;
    }
    *left = *right = *center = 0;
}
int subs(HDC hDC, char *out, char *fmt, char *filename, int page)
{
    SIZE size;
    char *orig = out;
    while (*fmt)
    {
        if (*fmt == '%')
        {
            switch(*(fmt + 1))
            {
                case '%':
                    *out ++ = *fmt++;
                    *out = 0;
                    fmt++;
                    break;
                case 'T':
                    strcpy(out, timeBuf);
                    fmt+=2 ;
                    break;
                case 't':
                    strcpy(out, euroTimeBuf);
                    fmt+=2 ;
                    break;
                case 'D':
                    strcpy(out, dateBuf);
                    fmt+=2 ;
                    break;
                case 'd':
                    strcpy(out, euroDateBuf);
                    fmt+=2 ;
                    break;
                case 'P':
                    sprintf(out, "%d", page);
                    fmt += 2;
                    break;
                case '#':
                    sprintf(out, "%d", total_pages);
                    fmt += 2;
                    break;
                case 'F':
                    strcpy(out, filename);
                    fmt += 2;
                    break;
                default:
                    *out++ = *fmt++;
                    *out = 0;
                    break;
            }
            out += strlen(out);
        }
        else
        {
            *out++ = *fmt++;
        }
    }
    *out = 0;
    size.cx = 0;
    GetTextExtentPoint32(hDC, orig, strlen(orig), &size);
    return size.cx;
}
void setTimeFormats(void)
{
    time_t ttime = time(0);
    struct tm *tm;
    tm = localtime(&ttime);
    strftime(timeBuf, 256, "%I:%M%p", tm);
    strftime(euroTimeBuf, 256, "%H:%M", tm);
    strftime(dateBuf, 256, "%m/%d/%y", tm);
    strftime(euroDateBuf, 256, "%d/%m/%y", tm);
}
//-------------------------------------------------------------------------

int Print(HWND win)
{
    BOOL rv = TRUE;
    int start, end;
    PRINTDLG pd;
    HWND hDlgCancel = 0;
    memset(&pd, 0, sizeof(pd));
    pd.lStructSize = sizeof(pd);
    pd.hwndOwner = hwndFrame;
    pd.Flags = PD_NOPAGENUMS | PD_RETURNDC | PD_COLLATE ;
    pd.hInstance = hInstance;
    SendDlgItemMessage(win, ID_EDITCHILD, EM_GETSEL, (WPARAM) &start, (LPARAM)
        &end);
    tabSetting = PropGetInt(NULL, "TAB_INDENT");
    leftmargin = PropGetInt(NULL, "PRINTER_LEFT");
    rightmargin = PropGetInt(NULL, "PRINTER_RIGHT");
    topmargin = PropGetInt(NULL, "PRINTER_TOP");
    bottommargin = PropGetInt(NULL, "PRINTER_BOTTOM");
    PropGetString(NULL, "PRINTER_HEADER", printHeader, sizeof(printHeader));
    PropGetString(NULL, "PRINTER_FOOTER", printFooter, sizeof(printFooter));
    if (tabSetting <= 0)
        tabSetting = 4;
    if (end != start)
        pd.Flags |= PD_SELECTION;
    if (PrintDlg(&pd))
    {
        DOCINFO di;
        HWND child;
        char *buf,  *pos,  *savepos1;
        if (pd.Flags &PD_SELECTION)
        {
            if (end <= start)
                return TRUE ;
        }
        child = GetDlgItem(win, ID_EDITCHILD);
        buf = GetEditData(child);
        if (!buf)
            return TRUE ;
        if (pd.Flags &PD_SELECTION)
        {
            savepos1 = pos = buf + start;
            buf[end] = 0;
        }
        else
            savepos1 = pos = buf;
        printing = TRUE;
        memset(&di, 0, sizeof(di));
        di.cbSize = sizeof(di);
        di.lpszDocName = (char*)SendMessage(win, WM_FILETITLE, 0, 0);
        if (pd.Flags &PD_PRINTTOFILE)
            di.lpszOutput = "FILE:";
        SetAbortProc(pd.hDC, &AbortProc);

        if (StartDoc(pd.hDC, &di) > 0)
        {
            int pelsx, pelsy;
            int width, height, rows, cols;
            int done = FALSE;
            char headerLeft[256],headerRight[256],headerCenter[256];
            char FooterLeft[256],FooterRight[256],FooterCenter[256];
            SIZE strsize ;
            HFONT hFont, oldhFont;

            pelsx = GetDeviceCaps(pd.hDC, LOGPIXELSX);
            pelsy = GetDeviceCaps(pd.hDC, LOGPIXELSY);

            fontdata.lfHeight =  - MulDiv(11, pelsx, 72);
            hFont = CreateFontIndirect(&fontdata);
            oldhFont = SelectObject(pd.hDC, hFont);
            GetTextExtentPoint32(pd.hDC, "A", 1, &strsize);

            width = GetDeviceCaps(pd.hDC, HORZRES);
            height = GetDeviceCaps(pd.hDC, VERTRES);
//            buf = GetEditData(win);

            rows = (height - (topmargin + bottommargin) *pelsy * 10 / 254) /
                strsize.cy ;
            cols = (width - (leftmargin + rightmargin) *pelsx * 10 / 254) /
                strsize.cx;

            if (rows <= 0 || cols <= 0)
            {
                AbortDoc(pd.hDC);
                free(buf);
                DeleteObject(hFont);
                if (pd.hDC != NULL)
                    DeleteDC(pd.hDC);
                if (pd.hDevMode != NULL)
                    GlobalFree(pd.hDevMode);
                if (pd.hDevNames != NULL)
                    GlobalFree(pd.hDevNames);
                return FALSE;
            }
            SelectObject(pd.hDC, oldhFont);
            if (printHeader[0])
                rows -= 2;
            if (printFooter[0])
                rows -= 2;
            total_pages = countPages(savepos1, rows, cols);
            setTimeFormats();
            split(printHeader, headerLeft, headerRight, headerCenter);
            split(printFooter, FooterLeft, FooterRight, FooterCenter);
            hDlgCancel = CreateDialog(hInstance, "PRINTABORTDLG", 0, (DLGPROC)
                CancelProc);
            EnableWindow(hwndFrame, FALSE);
            do
            {
                int colcount = pd.nCopies;
                int pagenum = 1;
                pos = savepos1;
                while (printing &&  *pos && StartPage(pd.hDC) > 0)
                {
                    int i, j, k;
                    int hdrlines = 0;
                    char line[512];
                    char *savepos2 = pos;
                    oldhFont = SelectObject(pd.hDC, hFont);
                    if (printHeader[0])
                    {
                        char buf[256];
                        int cx ;
                        int cy = topmargin * pelsy * 10/ 254;
                        cx = subs(pd.hDC, buf, headerLeft, di.lpszDocName, pagenum);
                        cx = leftmargin * pelsx * 10 / 254;
                        TextOut(pd.hDC, cx, cy, buf, strlen(buf));
                        cx = subs(pd.hDC, buf, headerCenter, di.lpszDocName, pagenum);
                        cx = (width - cx)/2 ;
                        TextOut(pd.hDC, cx, cy, buf, strlen(buf));
                        cx = subs(pd.hDC, buf, headerRight, di.lpszDocName, pagenum);
                        cx = width - cx - rightmargin * pelsx * 10 / 254;
                        TextOut(pd.hDC, cx, cy, buf, strlen(buf));
                        hdrlines = 2;
                    }
                    for (i = 0; i < rows; i++)
                    {
                        int count = 0;
                        if (*pos == '\n')
                        {
                            pos++;
                        }
                        else if (*pos == '\f')
                        {
                            pos += 2;
                            break;
                        }
                        else
                        {
                            for (j = 0; j < cols; j++)
                            {
                                if (*pos == '\t')
                                {
                                    int n = tabSetting - j % tabSetting;
                                    pos++;
                                    for (k = 0; k < n; k++)
                                        line[count++] = ' ';
                                    j += n - 1;
                                } 
                                else
                                {
                                    if (*pos < 32 ||  *pos > 126)
                                    {
                                        if (*pos == '\n')
                                            pos++;
                                        break;
                                    }
                                    line[count++] =  *pos++;
                                }
                            }
                            TextOut(pd.hDC, leftmargin *pelsx * 10 / 254, (i + hdrlines)
                                *strsize.cy + topmargin * pelsy * 10 / 254, line, count);
                        }
                        if (! *pos)
                            break;
                    }
                    if (printFooter[0])
                    {
                        char buf[256];
                        int cx ;
                        int cy = height - strsize.cy - bottommargin * pelsy * 10 / 254;
                        cx = subs(pd.hDC, buf, FooterLeft, di.lpszDocName, pagenum);
                        cx = leftmargin * pelsx * 10 / 254;
                        TextOut(pd.hDC, cx, cy, buf, strlen(buf));
                        cx = subs(pd.hDC, buf, FooterCenter, di.lpszDocName, pagenum);
                        cx = (width - cx) / 2;
                        TextOut(pd.hDC, cx, cy, buf, strlen(buf));
                        cx = subs(pd.hDC, buf, FooterRight, di.lpszDocName, pagenum);
                        cx = width - cx - rightmargin * pelsx * 10 / 254;
                        TextOut(pd.hDC, cx, cy, buf, strlen(buf));
                    }
                    SelectObject(pd.hDC, oldhFont);
                    if (printing)
                        if (EndPage(pd.hDC) <= 0)
                            goto doneprinting;
                    if (!(pd.Flags &PD_COLLATE))
                    {
                        if (--colcount)
                        {
                            pos = savepos2;
                        }
                        else
                        {
                            colcount = pd.nCopies;
                            pagenum++;
                        }
                    }
                    else
                    {
                        pagenum++;
                    }
                        
                }
            }
            while (printing && (pd.Flags &PD_COLLATE) && --pd.nCopies)
                ;
            if (!printing)
                AbortDoc(pd.hDC);
            doneprinting: if (printing)
                EndDoc(pd.hDC);
            EnableWindow(hwndFrame, TRUE);
            if (hDlgCancel)
                DestroyWindow(hDlgCancel);
            DeleteObject(hFont);
        }
        FreeEditData(buf);

    }
    if (pd.hDC != NULL)
        DeleteDC(pd.hDC);
    if (pd.hDevMode != NULL)
        GlobalFree(pd.hDevMode);
    if (pd.hDevNames != NULL)
        GlobalFree(pd.hDevNames);
    return rv;
}
