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
#include <stdio.h>
#include <commctrl.h>
#include <stdarg.h>

#include "header.h"
#include "progress.h"
#include "idewinconst.h"

static char *szProgName = "LADPROGRESS";
// ==============================================================
//
/*
 * Progress bar window, or zero if no progress bar
 */
HWND hWndProgress;

/* limit of progress bar */
static int proglim = 0;

// ==============================================================
//
/* progress bar dialog box */
int FAR PASCAL ProgressProc(HWND hWndDlg, UINT wmsg, WPARAM wparam, LPARAM
    lparam)
{
    switch (wmsg)
    {
        case WM_INITDIALOG:
            CenterWindow(hWndDlg);
            SetProgress(0, "");
            return TRUE;
        case WM_CLOSE:
            PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wparam))
            {
            case IDCANCEL:
                /* They pressed cancel, get out */
                DeleteProgress();
                break;
            }
    }
    return 0;
}

// ==============================================================
//
/* Build the progress bar, set a limit and title */
void MakeProgress(HWND hWnd, HINSTANCE hInst, LPCSTR title, long value)
{
    /* Can't have a progress bar with a limit of 0 */
    if (value == 0)
        return ;

    /* Set new limit */
    proglim = value;

    /* Get out if bar already up */
    if (hWndProgress)
        return ;

    /* Make window and set title */
    hWndProgress = CreateDialog(hInst, "DLG_PROGRESS", hWnd, ProgressProc);
    SetWindowText(hWndProgress, title);

    SendMessage(hWndProgress, PBM_SETSTEP, 4, 0);
}

// ==============================================================
//
/* Delete the progress bar window */
void DeleteProgress(void)
{
    if (hWndProgress)
    {
        DestroyWindow(hWndProgress);
        hWndProgress = NULL;
    }
}

// ==============================================================
//
/* Set the text field and the progress bar current value fields */
void SetProgress(long value, LPCSTR fmt, ...)
{
    /* calculate percentage */
    long percent = value * 100 / proglim;
    char string[64], text[1024];
    HWND hWndTemp;
    va_list argptr;

    /* Never greater than 100 percent */
    if (percent > 100)
        percent = 100;

    /* Collect the string for the caption field */

    va_start(argptr, fmt);
    vsprintf(text, fmt, argptr);
    va_end(argptr);

    /* Now set the text of the caption field */
    hWndTemp = GetDlgItem(hWndProgress, IDC_PBNAME);
    SendMessage(hWndTemp, WM_SETTEXT, 0, (LPARAM)text);

    /* Now set the percentage field on the right */
    sprintf(string, "%d%%   ", percent);
    hWndTemp = GetDlgItem(hWndProgress, IDC_PBPERCENT);
    SendMessage(hWndTemp, WM_SETTEXT, 0, (LPARAM)string);

    /* Now set the progress bar percentage */
    hWndTemp = GetDlgItem(hWndProgress, IDC_PROGRESSBAR);
    SendMessage(hWndTemp, PBM_SETPOS, percent, 0);
}
