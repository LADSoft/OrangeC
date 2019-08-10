/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
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
void SetEditField(HWND hWnd, int control, char* text)
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
void GetEditField(HWND hWnd, int control, char* text)
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
void SetCBField(HWND hWnd, int control, int value) { CheckDlgButton(hWnd, control, value); }

//-------------------------------------------------------------------------

int GetCBField(HWND hWnd, int control) { return IsDlgButtonChecked(hWnd, control); }

// ==============================================================
//
// set focus
void NewFocus(HWND hWnd, int control)
{
    HWND hwndc = GetDlgItem(hWnd, control);
    SetFocus(hwndc);
}

//-------------------------------------------------------------------------

void AddComboString(HWND hwnd, int control, char* string)
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

void AddListString(HWND hwnd, int control, char* string)
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
