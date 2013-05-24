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
//
// DIALOG.CPP
//
// dialog box utility functions, for setting retrieving values
//
#define STRICT
#include <windows.h>
#include <stdio.h>

// ==============================================================
// enable or disable a control
void DisableControl(HWND hwnd, int control, int disable)
{
    HWND hwndedit = GetDlgItem(hwnd, control);
    EnableWindow(hwndedit, !disable);
}

// ==============================================================
//
// set an edit field to read only
void SetEditFieldRO(HWND hwnd, int control)
{
    HWND hwndedit = GetDlgItem(hwnd, control);
    SendMessage(hwndedit, EM_SETREADONLY, TRUE, 0);
}

// ==============================================================
//
// set the value of an edit field
//
void SetEditField(HWND hWnd, int control, char *text)
{
    HWND hwndedit = GetDlgItem(hWnd, control);
//    SendMessage(hwndedit, EM_LIMITTEXT, 1020, 0);
//    SendMessage(hwndedit, EM_SETSEL, 0,  - 1);
//    SendMessage(hwndedit, EM_REPLACESEL, 0, (LPARAM)text);
//    SendMessage(hwndedit, EM_SETSEL, 0,  - 1);
    SendMessage(hwndedit, WM_SETTEXT, 0, (LPARAM)text);
    SendMessage(hwndedit, EM_SETLIMITTEXT, 255, 0);
}

//-------------------------------------------------------------------------

void SetEditFieldValue(HWND hWnd, int control, long value)
{
    char string[256];
    sprintf(string, "%ld", value);
    SetEditField(hWnd, control, string);
}

// ==============================================================
//
// get the value of an edit field
//
void GetEditField(HWND hWnd, int control, char *text)
{
    HWND hwndedit = GetDlgItem(hWnd, control);
    char itext[1024];
    int rv;
    rv = SendMessage(hwndedit, WM_GETTEXT, 1020, (LPARAM)itext);
    itext[rv] = 0;
    strcpy(text, itext);
}

//-------------------------------------------------------------------------

long GetEditFieldValue(HWND hWnd, int control)
{
    char string[256];
    GetEditField(hWnd, control, string);
    return atoi(string);
}

// ==============================================================
// 
// handle checkboxes
//
void SetCBField(HWND hWnd, int control, int value)
{
    CheckDlgButton(hWnd, control, value);
}

//-------------------------------------------------------------------------

int GetCBField(HWND hWnd, int control)
{
    return IsDlgButtonChecked(hWnd, control);
}

// ==============================================================
//
// set focus 
void NewFocus(HWND hWnd, int control)
{
    HWND hwndc = GetDlgItem(hWnd, control);
    SetFocus(hwndc);
}

//-------------------------------------------------------------------------

void AddComboString(HWND hwnd, int control, char *string)
{
    HWND hwndc = GetDlgItem(hwnd, control);
    SendMessage(hwndc, CB_ADDSTRING, 0, (LPARAM)string);
}

//-------------------------------------------------------------------------

void SetComboSel(HWND hwnd, int control, int sel)
{
    HWND hwndc = GetDlgItem(hwnd, control);
    SendMessage(hwndc, CB_SETCURSEL, sel, 0);
}

//-------------------------------------------------------------------------

int GetComboSel(HWND hwnd, int control)
{
    HWND hwndc = GetDlgItem(hwnd, control);
    return SendMessage(hwndc, CB_GETCURSEL, 0, 0);
}

//-------------------------------------------------------------------------

void AddListString(HWND hwnd, int control, char *string)
{
    HWND hwndc = GetDlgItem(hwnd, control);
    SendMessage(hwndc, LB_ADDSTRING, 0, (LPARAM)string);
}

//-------------------------------------------------------------------------

void SetListSel(HWND hwnd, int control, int sel)
{
    HWND hwndc = GetDlgItem(hwnd, control);
    SendMessage(hwndc, LB_SETCURSEL, sel, 0);
}

//-------------------------------------------------------------------------

int GetListSel(HWND hwnd, int control)
{
    HWND hwndc = GetDlgItem(hwnd, control);
    return SendMessage(hwndc, LB_GETCURSEL, 0, 0);
}
