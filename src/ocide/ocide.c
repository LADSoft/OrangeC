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
#include <float.h>
#include "helpid.h"
#include "header.h"
//#include <dir.h>
#include "wargs.h"
#include "splash.h"
#include "..\version.h"
#include <sys\stat.h>
/* timer identifiers
 *
 * note that the current timer implementation assumes only one timer
 * will run at a time
 */
#define IDT_STARTING 1
#define IDT_STOPPING 2
#define IDT_CLIENTCONTEXTMENU 3
#define IDT_RETRIEVEFILENAMES 4

#define FONTNAME "veramono.ttf"
 
extern HWND hwndTbFind;
extern HWND hwndFind;
extern int WindowItemCount;
extern char *findhist[MAX_COMBO_HISTORY];
extern int restoredDocks;
extern DWINFO *editWindows;
extern char szHelpPath[]; 
extern int making;
extern HWND hwndProject, hwndASM, hwndThread, hwndRes;
extern HWND hwndRegister, hwndMem, hwndTab, hwndWatch, hwndStack;
extern HWND hwndBookmark;
extern DWINFO *mrulist[MAX_MRU],  *mruprojlist[MAX_MRU];
extern HANDLE BreakpointSem;
extern char *lines[];
extern int curline;
extern THREAD *activeThread, *stoppedThread;
extern HWND hwndToolNav, hwndToolEdit, hwndToolDebug, hwndToolBuild, hwndToolBookmark;
extern enum DebugState uState;
extern PROJECTITEM *workArea;

void ApplyDialogFont(HWND hwnd);
char *getcwd( char *__buf, int __buflen ); // can't include dir.h because it defines eof...

//extern int __argc;
//extern char **__argv;

//void PASCAL CoInitialize(LPVOID pvReserved);
//void PASCAL CoUninitialize(void);

// the offset of the WINDOW menu

#define psShow "Show"

BOOL ignoreAccel = FALSE;
HHOOK hCursHook;
HCURSOR hCursArrow, hCursHourglass;
char szInstallPath[1024];
HANDLE hMenuMain, hAccel, hwndFrame, hwndClient, hwndStatus;
HINSTANCE hInstance;
HANDLE editLib;
HWND hwndSrcTab; 

unsigned int helpMsg;

int programBusy;

DWORD threadMain;

char *watchhist[MAX_COMBO_HISTORY];

LOGFONT systemDialogFont, systemMenuFont, systemCaptionFont;

static char szFrameClassName[] = "ocideFrame";
static LOGFONT NormalFontData = 
{
    -13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN |
        FF_DONTCARE,
        "Arial"
};
static int bCmdLineWS, bCmdLineProj;
static char szNewWS[256], szNewProj[256];
static char browseToText[256];

void ProjSetup(char select, char *string);
void WorkAreaSetup(char select, char *string);
ARGLIST ArgList[] = 
{
    {
        'p', ARG_CONCATSTRING, ProjSetup
    }
    , 
    {
        'w', ARG_CONCATSTRING, WorkAreaSetup
    }
    , 
    {
        0, 0, 0
    }
};
void SetStatusMessage(char *str, int highlight)
{
    static char buf[1000];
    buf[0] = highlight + '0';
    strcpy(buf + 1, str);
    if (buf[1] == 0)
        strcpy(buf + 1, "    ");
    SendMessage(hwndStatus, SB_SETTEXT, 0 | SBT_NOBORDERS | SBT_OWNERDRAW, (LPARAM)
        buf);
}
static int readytodraw = FALSE;
void SetBusy(int state)
{
    if (state)
    {
        if (!programBusy)
            SetCursor(hCursHourglass);
        programBusy++;
    }
    else
    {
        programBusy--;
        if (programBusy <= 0)
        {
            SetCursor(hCursArrow);
            programBusy = 0;
        }
    }
}

//-------------------------------------------------------------------------

VOID WINAPI CenterWindow(HWND hWnd)
{
    RECT rect;
    WORD wWidth, wHeight;

    // find out how big we are
    GetWindowRect(hWnd, &rect);

    // get screen size
    wWidth = GetSystemMetrics(SM_CXSCREEN);
    wHeight = GetSystemMetrics(SM_CYSCREEN);

    // move to center
    MoveWindow(hWnd, (wWidth / 2) - ((rect.right - rect.left) / 2), (wHeight /
        2) - ((rect.bottom - rect.top) / 2), rect.right - rect.left,
        rect.bottom - rect.top, FALSE);
}

//-------------------------------------------------------------------------

int ExtendedMessageBox(char *title, int flag, char *fmt, ...)
{
    char string[1024];
    va_list argptr;

    va_start(argptr, fmt);
    vsprintf(string, fmt, argptr);
    va_end(argptr);
    return MessageBox(GetFocus(), string, title, flag | MB_SETFOREGROUND | MB_APPLMODAL);
}

//-------------------------------------------------------------------------

void ProjSetup(char select, char *string)
{
    char *p;
    bCmdLineProj = TRUE ;
    strcpy(szNewProj, string);
    abspath(szNewProj, 0);
    p = stristr(szNewProj,".cpj");
    if (p)
        *p = 0;
}

void WorkAreaSetup(char select, char *string)
{
    bCmdLineWS = TRUE;
    strcpy(szNewWS, string);
    abspath(szNewWS, 0);
    if (!strrchr(szNewWS, '.'))
        strcat(szNewWS,".cwa");
}

//-------------------------------------------------------------------------

int IsEditWindow(HWND hwnd)
{
    return (GetWindowLong(hwnd, 4) == EDITSIG);
}
int IsResourceWindow(HWND hwnd)
{
    return (GetWindowLong(hwnd, 4) == RESSIG);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK BrowseToProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    char buf[3];
    switch (iMessage)
    {
        case WM_COMMAND:
            if (wParam == IDOK)
            {
                GetEditField(hwnd, IDC_BROWSETO, browseToText);
                EndDialog(hwnd, 1);
                break;
            }
            if (HIWORD(wParam) == EN_CHANGE)
            {
                DisableControl(hwnd, IDOK, !GetWindowText((HWND)lParam, buf, 2))
                    ;
                break;
            }
            if (wParam != IDCANCEL)
                break;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            break;
        case WM_INITDIALOG:
            browseToText[0] = 0;
            CenterWindow(hwnd);
            SetEditField(hwnd, IDC_BROWSETO, "");
            SendDlgItemMessage(hwnd, IDC_BROWSETO, EM_LIMITTEXT, 250, 0);
            DisableControl(hwnd, IDOK, 1);
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

LRESULT CALLBACK WaitingProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_COMMAND:
            if (wParam == IDOK)
                EndDialog(hwnd, 0);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

LRESULT CALLBACK WatchAddProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static char buf[256];
    HWND editwnd;
    switch (iMessage)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDOK:
                editwnd = GetDlgItem(hwnd, IDC_EDWATCH);
                GetWindowText(editwnd, buf, 256);
                SendMessage(editwnd, WM_SAVEHISTORY, 0, 0);
                EndDialog(hwnd, (int)buf);
                break;
            case IDCANCEL:
                EndDialog(hwnd, 0);
                break;

            }
            switch (HIWORD(wParam))
            {
            case CBN_SELCHANGE:
                EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                break;
            case CBN_EDITCHANGE:
                EnableWindow(GetDlgItem(hwnd, IDOK), GetWindowText((HWND)lParam,
                    buf, 2));
                break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            editwnd = GetDlgItem(hwnd, IDC_EDWATCH);
            SubClassHistoryCombo(editwnd);
            SendMessage(editwnd, WM_SETHISTORY, 0, (LPARAM)watchhist);
            if (!watchhist[0])
                EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
            return TRUE;
    }
    return 0;
}

//-------------------------------------------------------------------------

char *exceptval(int num)
{
    static char buf[256];
    switch (num)
    {
        case STATUS_ACCESS_VIOLATION:
            return "ACCESS VIOLATION";
        case STATUS_DATATYPE_MISALIGNMENT:
            return "DATATYPE MISALIGNMENT";
        case STATUS_ARRAY_BOUNDS_EXCEEDED:
            return "ARRAY BOUNDS EXCEEDED";
        case STATUS_FLOAT_DENORMAL_OPERAND:
            return "FLOAT DENORMAL OPERAND";
        case STATUS_FLOAT_DIVIDE_BY_ZERO:
            return "FLOAT DIVIDE BY ZERO";
        case STATUS_FLOAT_INEXACT_RESULT:
            return "FLOAT INEXACT RESULT";
        case STATUS_FLOAT_INVALID_OPERATION:
            return "FLOAT INVALID OPERATION";
        case STATUS_FLOAT_OVERFLOW:
            return "FLOAT OVERFLOW";
        case STATUS_FLOAT_STACK_CHECK:
            return "FLOAT STACK CHECK";
        case STATUS_FLOAT_UNDERFLOW:
            return "FLOAT UNDERFLOW";
        case STATUS_INTEGER_DIVIDE_BY_ZERO:
            return "INTEGER DIVIDE BY ZERO";
        case STATUS_INTEGER_OVERFLOW:
            return "INTEGER OVERFLOW";
        case STATUS_PRIVILEGED_INSTRUCTION:
            return "PRIVILEGED INSTRUCTION";
        case STATUS_IN_PAGE_ERROR:
            return "IN PAGE ERROR";
        case STATUS_ILLEGAL_INSTRUCTION:
            return "ILLEGALINSTRUCTION";
        case STATUS_NONCONTINUABLE_EXCEPTION:
            return "NONCONTINUABLE EXCEPTION";
        case STATUS_STACK_OVERFLOW:
            return "STACK OVERFLOW";
        case STATUS_INVALID_DISPOSITION:
            return "INVALID DISPOSITION";
        case STATUS_GUARD_PAGE_VIOLATION:
            return "GUARD PAGE VIOLATION";
        case STATUS_SEGMENT_NOTIFICATION:
            return "SEGMENT NOTIFICATION";
        case STATUS_INVALID_HANDLE:
            return "INVALID HANDLE";
        case STATUS_NO_MEMORY:
            return "NO MEMORY";
        case STATUS_CONTROL_C_EXIT:
            return "CONTROL C EXIT";
        case STATUS_FLOAT_MULTIPLE_FAULTS:
            return "FLOAT MULTIPLE FAULTS";
        case STATUS_FLOAT_MULTIPLE_TRAPS:
            return "FLOAT MULTIPLE TRAPS";
//        case STATUS_ILLEGAL_VLM_REFERENCE:
//            return "ILLEGAL VLM REFERENCE";
        case STATUS_REG_NAT_CONSUMPTION:
            return "REG NAT CONSUMPTION";

        default:
            sprintf(buf, "Unknown Exception(%08x)", num);
            return buf;
    }

}


int GetHelpID(void )
{
    int helpID = HELP_CONTENTS;
    return helpID;
}
    
void LoadFirstWorkArea(void)
{
    int argc = __argc;
    char **argv = __argv;
    if (argv)
    {
        int todo = parse_args(&argc, argv, 1) && argc > 1 ;
        if (!bCmdLineWS)
        {
            CloseWorkArea();
            if (bCmdLineProj)
            {
                LoadProject(szNewProj);
                dmgrHideWindow(DID_TABWND, FALSE);
            }
        }
        else
        {
            LoadWorkArea(szNewWS, TRUE);
        }

        if (todo)
        {
            int i;
            char cwd[256];
            int munged = FALSE;
            StringToProfile("FILEDIR", (char *)getcwd(cwd, 256));
            for (i = 1; i < argc; i++)
            {
                DWINFO info;
                char *p = strrchr(argv[i], '\\');
                struct stat statbuf;
                if (p)
                    strcpy(info.dwTitle, p + 1);
                else
                    strcpy(info.dwTitle, argv[i]);
                strcpy(info.dwName, argv[i]);
                abspath(info.dwName, 0);
                info.dwLineNo =  - 1;
                info.logMRU = TRUE;
                info.newFile = FALSE;
                if (stat(info.dwName, &statbuf) !=  - 1)
                    CreateDrawWindow(&info, TRUE);
                else
                    if (ExtendedMessageBox("File Error", 
                        MB_SETFOREGROUND | MB_SYSTEMMODAL | MB_YESNO, 
                        "File '%s' not found, would you like to create it?", 
                        argv[i]) == IDYES)
                    {
                        FILE *fil = fopen(argv[i],"wb");
                        if (fil)
                        {
                            fclose(fil);
                        }
                        info.newFile = TRUE;
                        CreateDrawWindow(&info, TRUE);
                    }                
            }
        }
    } else
        CloseWorkArea();
}
static void OwnerDrawStatusBar(DRAWITEMSTRUCT *di)
{
    int colorfg, colorbg;
    HBRUSH hbr;
    /* for drawing the first section of the status window */
    if (*(char *)(di->itemData) == '1')
    {
        colorfg = SetTextColor(di->hDC, RetrieveSysColor(COLOR_HIGHLIGHTTEXT));
        colorbg = SetBkColor(di->hDC, RetrieveSysColor(COLOR_HIGHLIGHT));
        hbr = CreateSolidBrush(RetrieveSysColor(COLOR_HIGHLIGHT));
    }
    else
    {
        colorfg = SetTextColor(di->hDC, RetrieveSysColor(COLOR_BTNTEXT));
        colorbg = SetBkColor(di->hDC, RetrieveSysColor(COLOR_BTNFACE));
        hbr = CreateSolidBrush(RetrieveSysColor(COLOR_BTNFACE));
    }				
    FillRect(di->hDC, &di->rcItem, hbr);
    
    TextOut(di->hDC, di->rcItem.left, di->rcItem.top, (char *)(di->itemData + 1), 
                strlen((char *)di->itemData + 1));
    SetTextColor(di->hDC, colorfg);
    SetBkColor(di->hDC, colorbg);
    SetTextColor(di->hDC, colorfg);
    DeleteObject(hbr);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    int maxed;
    LRESULT rv;
    static int timerid;
    CLIENTCREATESTRUCT csx;
    static DEBUG_EVENT *dbe;
    static int initted;
    HWND win;
    int  x_state;
    RECT rs, rf;
    int parts[10], i;
    char *name;
    char module[256];
    int linenum;
    char buf[256];
    DRAWITEMSTRUCT *di;
    NMHDR *h;
    switch (iMessage)
    {
        case WM_NOTIFY:
            h = (NMHDR*)lParam;
            switch (h->code)
            {
                case TABN_SELECTED:
                {
                    LSTABNOTIFY *p = (LSTABNOTIFY *)h;
                    if ((HWND)p->lParam == (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0,0))
                    {
                        SetFocus((HWND)p->lParam);
                    }
                    else
                    {
                        HWND hwnd = (HWND)p->lParam;
                        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MAXIMIZEBOX);                        
                        SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)p->lParam, 0);
                        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
                    }
                    break;
                }
                case TABN_CLOSED:
                {
                    LSTABNOTIFY *p = (LSTABNOTIFY *)h;
                    if (!IsEditWindow((HWND)p->lParam))
                        SendMessage((HWND)p->lParam, WM_CLOSE, 0, 0);
                    else
                        SendMessage((HWND)p->lParam, WM_COMMAND, IDM_CLOSE, 0);
                    break;
                }
            }
            break;
        case WM_DRAWITEM:
            di = (DRAWITEMSTRUCT *)lParam;
            switch (wParam)
            {
                case ID_STATUS_WINDOW:
                    OwnerDrawStatusBar(di);
                    break;
                case ID_MAIN_MENU:
                    break;
            }
            return TRUE;

        case WM_WINDOWPOSCHANGING:
            // Keeps it from drawing a smaller window prior to the initial
            // ShowWindow....
            if (!readytodraw)
            {
                ((LPWINDOWPOS)lParam)->flags |= SWP_NOREDRAW;
            }
            break;
        case WM_ACTIVATE:
            // check if edit windows changed
            if (LOWORD(wParam) != WA_INACTIVE)
                CheckEditWindowChanged();
            // first activation, init all the windows
            if (!initted)
            {
                HFONT font;
                initted = TRUE;
                CreateStackWindow();
                CreateInfoWindow();
                CreateTabWindow();
                CreatePropertyTabWindow();
                CreateWatchWindow();
                CreateASMWindow();
                CreateMemWindow();
                CreateThreadWindow();
                CreateRegisterWindow();
                CreateJumpListWindow();
                MakeToolBar(hwnd);
                hwndSrcTab = CreateLsTabWindow(hwnd, TABS_HOTTRACK | TABS_FLAT | TABS_CLOSEBTN | TABS_WINDOWBTN | TABS_DRAGABLE | WS_VISIBLE);
                ApplyDialogFont(hwndSrcTab);
                timerid = SetTimer(hwnd, IDT_STARTING, 100, 0);
                SetTimer(hwnd, IDT_RETRIEVEFILENAMES, 500, 0);
            }
            return 0;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                PostMessage(hwnd, WM_COMMAND, IDM_EXIT, 0);
                return 0;
            }
            break;
        case WM_TOOLBARDROPDOWN2:
            return ToolBarDropDown(wParam, lParam);
        case WM_TOOLBARDROPDOWN:
            PostMessage(hwnd, WM_TOOLBARDROPDOWN2, wParam, lParam);
            return 1;
        case WM_EXCEPTION:
            dmgrHideWindow(DID_ASMWND, FALSE);
            dbe = (DEBUG_EVENT*)lParam;
            if (!GetBreakpointLine((DWORD)dbe
                ->u.Exception.ExceptionRecord.ExceptionAddress, &module[0],
                &linenum, FALSE))
                dmgrHideWindow(DID_ASMWND, FALSE);
            else
                ApplyBreakAddress(module, linenum);
            if (hwndASM)
                SendMessage(hwndASM, WM_COMMAND, ID_SETADDRESS, (LPARAM)
                            dbe->u.Exception.ExceptionRecord.ExceptionAddress);
            SendInfoMessage(ERR_DEBUG_WINDOW, exceptval(dbe->u.Exception.ExceptionRecord.ExceptionCode));
            SendInfoMessage(ERR_DEBUG_WINDOW, "\r\n");
            ExtendedMessageBox("Exception", MB_SYSTEMMODAL, "%s", exceptval(dbe
                ->u.Exception.ExceptionRecord.ExceptionCode));
            break;
        case WM_PARENTNOTIFY:
            if (LOWORD(wParam) == WM_RBUTTONDOWN) {
                POINT pt;
                HWND xhwnd;
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);
                xhwnd = ChildWindowFromPoint(hwnd, pt);
                if (xhwnd == hwndClient)
                {
                    ClientToScreen(hwnd, &pt);
                    ScreenToClient(hwndClient, &pt);
                    xhwnd = ChildWindowFromPoint(hwndClient, pt);
                    if (xhwnd == hwndClient)
                        timerid = SetTimer(hwnd, IDT_CLIENTCONTEXTMENU, 100, 0);
                }
            }
            break;
        case WM_BREAKPOINT:
            dbe = (DEBUG_EVENT*)lParam;
            if (uState != notDebugging)
            {
                if (hwndASM)
                    SendMessage(hwndASM, WM_COMMAND, ID_SETADDRESS, (LPARAM)
                                dbe->u.Exception.ExceptionRecord.ExceptionAddress);
                
                if (!GetBreakpointLine((DWORD)dbe
                    ->u.Exception.ExceptionRecord.ExceptionAddress, &module[0],
                    &linenum, FALSE))
                    dmgrHideWindow(DID_ASMWND, FALSE);
                else
                    ApplyBreakAddress(module, linenum);
            }
            return 0;
        case WM_CREATE:
            if (IsDebuggerPresent())
                SetWindowText(hwnd, "Orange C IDE (DEBUGEE)");
            InitTheme(hwnd);
            csx.hWindowMenu = hMenuMain;
            csx.idFirstChild = IDM_FIRSTCHILD;
             hwndStatus = CreateWindowEx(0L, "msctls_statusbar32", "",
                WS_CHILD | WS_BORDER | WS_VISIBLE | CCS_NODIVIDER,
                    -100, -100, 10, 10,
                    hwnd, (HMENU)ID_STATUS_WINDOW, hInstance, NULL);
            GetClientRect(hwnd, &rf);

            hwndClient = CreateWindowEx(0, "MDICLIENT", 0, WS_CHILD +
                WS_CLIPCHILDREN + WS_VISIBLE | MDIS_ALLCHILDSTYLES,
                rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, hwnd,
                0, hInstance, &csx);
            SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maxed);
            SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuMain, (LPARAM)
                GetSubMenu(hMenuMain, WindowMenuItem + maxed));
            parts[0] = rf.right - 500;
            parts[1] = rf.right - 420;
            parts[2] = rf.right - 340;
            parts[3] = rf.right - 260;
            parts[4] = rf.right - 180;
            parts[5] = rf.right - 100;
            parts[6] = rf.right - 20;
            SendMessage(hwndStatus, SB_SETPARTS, 7, (LPARAM) &parts[0]);
             for (i=0; i < 7; i++)
                 SendMessage(hwndStatus, SB_SETTEXT, i | SBT_NOBORDERS | SBT_OWNERDRAW, (LPARAM)"   ");
            dmgrInit(hInstance, hwnd, hwndClient, 20);
            dmgrAddStatusBar(hwndStatus);
            break;
        case WM_HELP:
            if (GetKeyState(VK_CONTROL) &0x80000000)
            {
                win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (editWindows && IsWindow(win))
                    if (IsEditWindow(win))
                    {
                        SendMessage(win, WM_COMMAND, IDM_SPECIFIEDHELP, 0);
                        break;
                    }
            }
            else if (GetKeyState(VK_SHIFT) & 0x80000000)
            {
                win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (editWindows && IsWindow(win))
                    if (IsEditWindow(win))
                    {
                        SendMessage(win, WM_COMMAND, IDM_RTLHELP, 0);
                        break;
                    }
            }
            else
                PostMessage(hwnd, WM_COMMAND, IDM_OCIDEHELP, 0);
            break;
        case WM_COMMAND:
            switch (wParam &0xffff)
            {
            case IDM_SPECIFIEDHELP:
               SpecifiedHelp(0);
               break;
            case IDM_RTLHELP:
                RTLHelp(0);
                break;
            case IDM_LANGUAGEHELP:
                LanguageHelp(0);
                break;
            case IDM_TOOLSHELP:
                strcpy(buf, szInstallPath);
                strcat(buf, "\\help\\tools.chm");
                GenericHelp(buf, -1);
                break;
            case IDM_OCIDEHELP:
                strcpy(buf, szInstallPath);
                strcat(buf, "\\help\\ocide.chm");
                GenericHelp(buf, GetHelpID());
                break;
            case IDM_GETTINGSTARTED:
                strcpy(buf, szInstallPath);
                strcat(buf, "\\help\\ocide.chm");
                GenericHelp(buf, IDH_GETTING_STARTED);
                break;
            case IDM_ADDWATCH:
                if (uState != atBreakpoint && uState != atException)
                    break;
                name = (char *)DialogBoxParam(hInstance, "ADDWATCHDIALOG", hwnd, 
                    (DLGPROC)WatchAddProc, 0);
                if (name)
                    SendMessage(hwndWatch, WM_ADDWATCH, activeThread->regs.Eip,
                        (LPARAM)name);
                break;
            case IDM_ADDWATCHINDIRECT:
                if (uState != atBreakpoint && uState != atException)
                    break;
                if (IsWindow(win = (HWND)SendMessage(hwndClient,
                    WM_MDIGETACTIVE, 0, 0)) && IsEditWindow(win))
                    SendMessage(hwndWatch, WM_ADDWATCHINDIRECT, (WPARAM)win, 0);
                break;
            case IDM_DATABREAKPOINT:
                if (uState != atBreakpoint && uState != atException)
                    break;
                databp(NULL);
                break;
            case IDM_DATABREAKPOINTINDIRECT:
                if (uState != atBreakpoint && uState != atException)
                    break;
                if (IsWindow(win = (HWND)SendMessage(hwndClient,
                    WM_MDIGETACTIVE, 0, 0)) && IsEditWindow(win))
                    databp(win);
                break;
            case IDM_DELETEWATCH:
            case IDM_DELETEALLWATCH:
                if (uState != atBreakpoint && uState != atException)
                    break;
                SendMessage(hwndWatch, iMessage, wParam, lParam);
                break;
                case IDM_HBREAK:
                    hbpDialog();
                    break;
            case IDM_STOPDEBUGGING:
                if (uState == notDebugging)
                    break;
                abortDebug();
                break;
            case IDM_STEPIN:
                if (uState == notDebugging)
                    dbgRebuildMain(wParam);
                else if (uState != notDebugging && uState != Running)
                {
                    StepIn(dbe);
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 1);
                }
                return 0;
            case IDM_RUN:
                if (uState != notDebugging && uState != Running)
                {
                    SaveRegisterContext();
                    ReleaseSemaphore(BreakpointSem, 1, 0);
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 1);
                }
                else if (uState == notDebugging)
                {
                    PostMessage(hwndProject, WM_COMMAND, IDM_RUN, 0);
                }
                else
                {
                    MessageBeep(MB_OK);
                }
                break;
            case IDM_STOP:
                if (uState != Running)
                    break;
                StopRunning(nullState);
                break;
            case IDM_STEPOVER:
                if (uState == notDebugging)
                    dbgRebuildMain(wParam);
                else if (uState != notDebugging && uState != Running)
                {
                    StepOver(dbe);
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 1);
                }
                return 0;
            case IDM_STEPOUT:
                if (uState == notDebugging)
                    dbgRebuildMain(wParam);
                else if (uState != notDebugging && uState != Running)
                {
                    StepOut(dbe);
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 1);
                }
                return 0;
            case IDM_RUNTO:
                if (RunTo(dbe))
                {
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 1);
                }
                return 0;
            case IDM_BREAKPOINT:
                if (GetFocus() == hwndASM)
                {
                    SendMessage(hwndASM, WM_COMMAND, IDM_BREAKPOINT, 0);
                }
                else
                {
                    SetBP(dbe);
                }
                return 0;
            case IDM_SCROLLTOBP:
                if (!GetBreakpointLine((DWORD)stoppedThread->regs.Eip,
                    &module[0], &linenum, FALSE))
                    dmgrHideWindow(DID_ASMWND, FALSE);
                else
                    ApplyBreakAddress(module, linenum);
                activeThread = stoppedThread;
                if (hwndRegister)
                    PostMessage(hwndRegister, WM_COMMAND, ID_SETADDRESS, (LPARAM)
                        activeThread->hThread);
                PostMessage(hwndWatch, WM_COMMAND, ID_SETADDRESS, 0);
                if (hwndStack)
                    PostMessage(hwndStack, WM_RESTACK, (WPARAM)1, 0);
                if (hwndThread)
                    PostMessage(hwndThread, WM_RESTACK, (WPARAM)1, 0);
                if (hwndMem)
                    PostMessage(hwndMem, WM_RESTACK, 0, 0);
                if (hwndASM)
                    SendMessage(hwndASM, WM_COMMAND, ID_SETADDRESS, (LPARAM)
                                activeThread->regs.Eip);
                if (hwndThread)
                    SendMessage(hwndThread, WM_RESTACK, 0, 0);
                break;
            case IDM_RUNNODEBUG:
                SendMessage(hwndProject, WM_COMMAND, IDM_RUNNODEBUG, 0);
                break;
            case IDM_TOOLCUSTOM:
                TBCustomize();
                break;
            case IDM_BUILDRULES:
                BuildRulesCustomize();
                break;
                /*
            case IDM_VIEWBUILDBAR:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWBUILDBAR);
                dmgrHideWindow(DID_BUILDTOOL, x_state);
                SetMenuCheckedState(hMenuMain, DID_BUILDTOOL, IDM_VIEWBUILDBAR);
                break;
            case IDM_VIEWBOOKMARKBAR:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWBOOKMARKBAR);
                dmgrHideWindow(DID_BOOKMARKTOOL, x_state);
                SetMenuCheckedState(hMenuMain, DID_BOOKMARKTOOL, IDM_VIEWBOOKMARKBAR);
                break;
            case IDM_VIEWNAVBAR:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWNAVBAR);
                dmgrHideWindow(DID_NAVTOOL, x_state);
                SetMenuCheckedState(hMenuMain, DID_NAVTOOL, IDM_VIEWNAVBAR);
                break;
            case IDM_VIEWEDITBAR:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWEDITBAR);
                dmgrHideWindow(DID_EDITTOOL, x_state);
                SetMenuCheckedState(hMenuMain, DID_EDITTOOL, IDM_VIEWEDITBAR);
                break;
            case IDM_VIEWDEBUGBAR:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWDEBUGBAR);
                dmgrHideWindow(DID_DEBUGTOOL, x_state);
                SetMenuCheckedState(hMenuMain, DID_DEBUGTOOL, IDM_VIEWDEBUGBAR);
                break;
                */
            case IDM_VIEWSTACK:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWSTACK);
                dmgrHideWindow(DID_STACKWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_STACKWND, IDM_VIEWSTACK);
                break;
            case IDM_VIEWTHREAD:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWTHREAD);
                dmgrHideWindow(DID_THREADWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_THREADWND, IDM_VIEWTHREAD);
                break;
            case IDM_VIEWASM:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWASM);
                dmgrHideWindow(DID_ASMWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_ASMWND, IDM_VIEWASM);
                break;
            case IDM_VIEWMEM:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWMEM);
                dmgrHideWindow(DID_MEMWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_MEMWND, IDM_VIEWMEM);
                break;
            case IDM_VIEWREGISTER:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWREGISTER);
                dmgrHideWindow(DID_REGWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_REGWND, IDM_VIEWREGISTER);
                break;
            case IDM_VIEWPROJECT:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWPROJECT);
                dmgrHideWindow(DID_TABWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_TABWND, IDM_VIEWPROJECT);
                return 0;
            case IDM_VIEWPROPS:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWPROPS);
                dmgrHideWindow(DID_PROPSWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_PROPSWND, IDM_VIEWPROPS);
                return 0;
            case IDM_VIEWWATCH:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWWATCH);
                dmgrHideWindow(DID_WATCHWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_WATCHWND, IDM_VIEWWATCH);
                return 0;
            case IDM_VIEWERROR:
                x_state = GetMenuCheckedState(hMenuMain, IDM_VIEWERROR);
                dmgrHideWindow(DID_ERRORWND, x_state);
                SetMenuCheckedState(hMenuMain, DID_ERRORWND, IDM_VIEWERROR);
                return 0;
            case IDM_ABOUT:
                doAbout();
                //               DialogBoxParam(hInstance,"ABOUTDLG",hwnd,(DLGPROC)WaitingProc,0) ;
                return 0;
            case IDM_DEFERREDOPEN:
                CreateDrawWindow((DWINFO *)lParam, TRUE);
                free((DWINFO *)lParam);
                return 0;
            case IDM_OPEN:
                CreateDrawWindow(0, TRUE);
                return 0;
            case IDM_CLOSEWINDOW:
                win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (IsWindow(win))
                {
                    if (SendMessage(win, WM_COMMAND, ID_QUERYSAVE, 0) != IDCANCEL)
                        SendMessage(win, WM_CLOSE, 0, 0);
                }
                 return 0;
            case IDM_CLOSEALLWINDOWS:
                if (QuerySaveAll() != IDCANCEL)
                    CloseAll();
                return 0;
            case IDM_EXIT:
                if (SendMessage(hwnd, WM_QUERYENDSESSION, 0, 0))
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                return 0;
            case IDM_CASCADE:
                SendMessage(hwndClient, WM_MDICASCADE, 0, 0);
                return 0;
            case IDM_TILEHORIZ:
                SendMessage(hwndClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
                return 0;
            case IDM_TILEVERT:
                SendMessage(hwndClient, WM_MDITILE, MDITILE_VERTICAL, 0);
                return 0;
            case IDM_ARRANGE:
                SendMessage(hwndClient, WM_MDIICONARRANGE, 0, 0);
                return 0;
            case IDM_NEWPROJECT:
            case IDM_EXISTINGPROJECT:
            case IDM_NEWWS:
                dmgrHideWindow(DID_TABWND, FALSE);
            case IDM_OPENWS:
            case IDM_CLOSEWS:
            case IDM_SAVEWS:
            case IDM_IMPORT_CTG:
            case IDM_IMPORT_CWS:
                if (uState != notDebugging)
                {
                    if (ExtendedMessageBox("WorkArea", MB_OKCANCEL, 
                        "This action requires the debugger be stopped.") != IDOK)
                    {
                        break ;
                    }
                    abortDebug();
                }
                SendMessage(hwndProject, iMessage, wParam, lParam);
                break;
            case IDM_GENERALPROPERTIES:
                ShowGeneralProperties();
                //DisplayProperties(hInstance, hwndClient, EDITPROP);
                break;
            case IDM_NEWFILE:
                openfile((DWINFO *)-1, TRUE, TRUE);
//				CreateDrawWindow((DWINFO*)-1, TRUE);
                break;
            case IDM_COMPILEFILE:
            case IDM_COMPILEFILEFROMTREE:
            case IDM_MAKE:
            case IDM_BUILDSELECTED:
            case IDM_BUILDALL:
                if (uState != notDebugging)
                {
                    if (ExtendedMessageBox("Make", MB_OKCANCEL, 
                        "This action requires the debugger to be stopped.") == IDOK)
                        abortDebug();
                    else
                        break;
                }
                // fall through
            case IDM_NEWFILE_P:
            case IDM_SELECTPROFILE:
            case IDM_PROJECTPROPERTIES:
            case IDM_OPENFILES:
            case IDM_EXISTINGFILE:
            case IDM_NEWFOLDER:
            case IDM_EXISTINGFOLDER:
            case IDM_GENMAKE:
            case IDM_REMOVE:
            case IDM_RENAME:
            case IDM_SETACTIVEPROJECT:
            case IDM_CALCULATEDEPENDS:
            case IDM_ACTIVEPROJECTPROPERTIES:
                if (making)
                    break;
                // fall through
            case IDM_STOPBUILD:
                if (hwndProject)
                {
                    PostMessage(hwndProject, WM_COMMAND, wParam, lParam);
                }
                return 0;
            case IDM_FINDINFILES:
                OpenFindInFilesDialog();
                break;
            case IDM_BOOKMARK:
                ToggleBookMark(-1);
                SendMessage(hwnd, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_NEXTBOOKMARK:
                NextBookMark();
                break;
            case IDM_PREVBOOKMARK:
                PreviousBookMark();
                break;
            case IDM_NEXTBOOKMARKFILE:
                NextBookMarkFile();
                break;
            case IDM_PREVBOOKMARKFILE:
                PreviousBookMarkFile();
                break;
            case IDM_BOOKMARKWINDOW:
                ShowBookMarks();
                break;
            case IDM_PRINT:
                win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (editWindows && IsWindow(win))
                    if (IsEditWindow(win))
                    {
                        if (!Print(win))
                        {
                            ExtendedMessageBox("Print Error", MB_OK, "Incorrect margin settings\n");
                        }
                    }
                break;
            case IDM_BROWSE:
                if (IsWindow(win = (HWND)SendMessage(hwndClient,
                    WM_MDIGETACTIVE, 0, 0)) && IsEditWindow(win))
                {
                    BrowseTo(win, 0);
                }
                break;
            case IDM_BROWSETO:
                browseToText[0] = 0;
                if (DialogBoxParam(hInstance, "BROWSETODIALOG", hwnd, (DLGPROC)
                    BrowseToProc, 0))
                    BrowseTo(0, browseToText);
                break;
            case IDM_BROWSEBACK:
                BrowseBack();
                break;
            case IDM_REMOVEALLBREAKPOINTS:
                TagRemoveAll(TAG_BP);
                hbpDisable();
                databpRemove();
                SendMessage(hwnd, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_REMOVEBOOKMARKS:
                TagRemoveAll(TAG_BOOKMARK);
                SendMessage(hwnd, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_FORWARD:
                FileBrowseRight();
                break;
            case IDM_BACK:
                FileBrowseLeft();
                break;
            case IDM_SAVEALL:
            case IDM_SAVEALL2:
                SaveDrawAll();
                ResSaveAll();
                break;                
            case IDM_WINDOW_MORE:
                ShowWindowList();
                break;
            case IDM_DOSWINDOW:
                SendMessage(hwndProject, WM_COMMAND, IDM_DOSWINDOW, 0);
                break;
            case IDM_MAKEWINDOW:
                SendMessage(hwndProject, WM_COMMAND, IDM_MAKEWINDOW, 0);
                break;
            case IDM_EDITEXTERNALTOOLS:
                EditExternalTools();
                break;
            case IDM_NEWRESOURCE:
                return SendMessage(hwndRes, iMessage, wParam, lParam);
            case IDM_CLOSEFIND:
            {
                HWND x = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (IsEditWindow(x))
                    SendMessage(x, WM_COMMAND, IDM_CLOSEFIND, 0);
                    
                if (IsWindow(hwndFind))
                    SendMessage(hwndFind, WM_COMMAND, IDCANCEL, 0);
            }
                 break;
            default:
                if (wParam >= ID_FILEBROWSE_LIST && wParam < ID_FILEBROWSE_LIST + MAX_BROWSE)
                {
                    FileBrowseTrigger(wParam - ID_FILEBROWSE_LIST);
                }
                else if (wParam >= ID_EXTERNALTOOLS && wParam < ID_EXTERNALTOOLS + MAX_EXTERNALTOOLS)
                {
                    RunExternalTool(wParam);
                }
                else if (wParam >= ID_MRU_LIST && wParam < ID_MRU_LIST + MAX_MRU)
                {
                    mrulist[wParam - ID_MRU_LIST]->dwLineNo =  - 1;
                    mrulist[wParam - ID_MRU_LIST]->logMRU = FALSE;
                    CreateDrawWindow(mrulist[wParam - ID_MRU_LIST], TRUE);
                    MoveMRUUp(wParam - ID_MRU_LIST, FALSE);
                    return 0;
                }
                else if (wParam >= ID_MRU_PROJ_LIST && wParam <
                    ID_MRU_PROJ_LIST + MAX_MRU)
                {
                    IndirectProjectWindow(mruprojlist[wParam -
                        ID_MRU_PROJ_LIST]);
                    MoveMRUUp(wParam - ID_MRU_PROJ_LIST, TRUE);
                    return 0;
                }
                else if (wParam >= ID_WINDOW_LIST && wParam < ID_WINDOW_LIST +
                    MAX_WINMENU)
                {
                    MenuActivateWinmenu(wParam);
                }
                else if (LOWORD(wParam) == ID_TBFIND)
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_KILLFOCUS:
                            SendMessage((HWND)lParam, WM_SAVEHISTORY, 0, (LPARAM)findhist);
                            break;
                    }
                }
                else if (LOWORD(wParam) == 4000 + VK_RETURN)
                {
                    char buf[256];
                    int n = GetWindowText(hwndTbFind, buf, 256);
                    if (n > 0)
                    {
                        buf[n] = 0;
                        FindStringFromToolbar(buf);
                    }
                }
                else if (LOWORD(wParam) == 4000 + VK_ESCAPE)
                {
                    HWND x = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                    SetFocus(x);
                }
                else if (IsWindow(win = GetFocus()))
                {
                    HWND win2 = win;
                    if (!IsResourceWindow(win2))
                    {
                        win2 = GetParent(win);
                        if (win2 == hwndTbFind)
                            win2 = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                    }
                    if (IsResourceWindow(win2))
                    {
                        struct resRes *res = (struct resRes *)GetWindowLong(win2, 0);
                        PostMessage(win2, WM_COMMAND, wParam, lParam);
                    }
                    else if (!IsEditWindow(win2))
                    {
                        win2 = GetParent(win2);
                        if (win2 != hwnd)
                            PostMessage(win2, WM_COMMAND, wParam, lParam);
                    }
                    else
                    {
                        DWINFO *ptr = editWindows;
                        while (ptr)
                        {
                            if (ptr->active && win2 == ptr->self)
                            {
                                PostMessage(win2, WM_COMMAND, wParam, lParam);
                                break;
                            }
                            ptr = ptr->next;
                        }
                    }
                }
                break;
            }
            break;
        case WM_DESTROY:
            DestructTheme();
            PostQuitMessage(0);
            return 0;
        case WM_REDRAWTOOLBAR:
            RedrawToolBar();

            break;
        case WM_INITMENUPOPUP:
            InitMenuPopup(hMenuMain);
            return 0;

        case WM_SIZE:
            switch (wParam)
            {
            case SIZEFULLSCREEN:
                IntToProfile(psShow, SW_SHOWMAXIMIZED);
                break;
            case SIZEICONIC:
                //                    IntToProfile(psShow,SW_SHOWMINIMIZED) ;
                break;
            case SIZENORMAL:
                IntToProfile(psShow, SW_SHOWNORMAL);
                break;
            }
            if (wParam != SIZEICONIC)
            {
                rf.left = rf.top = 0;
                rf.right = LOWORD(lParam);
                rf.bottom = HIWORD(lParam);
                GetWindowRect(hwndStatus, &rs);
                rf.top = rf.bottom - rs.bottom + rs.top;
                MoveWindow(hwndStatus, rf.left, rf.top, rf.right - rf.left,
                    rf.bottom - rf.top, 1);
                parts[0] = rf.right - 500;
                parts[1] = rf.right - 420;
                parts[2] = rf.right - 340;
                parts[3] = rf.right - 260;
                parts[4] = rf.right - 180;
                parts[5] = rf.right - 100;
                parts[6] = rf.right - 20;
                SendMessage(hwndStatus, SB_SETPARTS, 7, (LPARAM) &parts[0]);
                SendMessage(hwndStatus, SB_SETTEXT, 0 | SBT_NOBORDERS  | SBT_OWNERDRAW, (LPARAM)
                    "    ");
    
                rv = 0; //DefFrameProc(hwnd, hwndClient, iMessage, wParam, lParam);
                dmgrSizeFrame();
                return rv;
            }
            break;
        case WM_TIMER:
            if (wParam == IDT_STARTING)
            {
                // i don't know if this one is necessary
                KillTimer(hwnd, timerid);
                CreateMenuBitmaps();
                InsertBitmapsInMenu(hMenuMain);
                LoadFirstWorkArea();
                if (!restoredDocks)
                {
                    CreateDocks();
                    SendMessage(hwnd, WM_REDRAWTOOLBAR, 0, 0);
                }
            } 
            else if (wParam == IDT_CLIENTCONTEXTMENU)
            {
                // this one is necessary because trying to do a trackpopupmenuex within
                // the WM_PARENTNOTIFY message didn't work out
                HMENU menu = LoadMenuGeneric(hInstance, "WINDOWMENU");
                HMENU popup = GetSubMenu(menu, 0);
                POINT pos;
                HWND win;
                if (!editWindows)
                {
                        EnableMenuItem(menu, IDM_CLOSEWINDOW, MF_GRAYED);
                        EnableMenuItem(menu, IDM_CLOSEALLWINDOWS, MF_GRAYED);
                        EnableMenuItem(menu, IDM_SAVEALL, MF_GRAYED);
                        EnableMenuItem(menu, IDM_TILEHORIZ, MF_GRAYED);
                        EnableMenuItem(menu, IDM_TILEVERT, MF_GRAYED);
                        EnableMenuItem(menu, IDM_CASCADE, MF_GRAYED);
                        EnableMenuItem(menu, IDM_ARRANGE, MF_GRAYED);
                        EnableMenuItem(menu, IDM_WINDOW_MORE, MF_GRAYED);
                        EnableMenuItem(menu, IDM_COPYWINDOW, MF_GRAYED);
                    
                } 
                else
                {
                    win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                    if (!IsWindow(win) || !IsEditWindow(win))
                        EnableMenuItem(menu, IDM_CLOSEWINDOW, MF_GRAYED);
                }
                GetCursorPos(&pos);
                KillTimer(hwnd, timerid);
                InsertBitmapsInMenu(popup);
                TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x,
                    pos.y, hwndFrame, NULL);
                DestroyMenu(menu);
            }
            
            else if (wParam == IDT_STOPPING)
            {
                // This one is necessary to give the debugger time to stop
                if (uState != notDebugging || making)
                    return 0;
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                KillTimer(hwnd, timerid);
            } else if (wParam == IDT_RETRIEVEFILENAMES)
            {
                DWINFO info;
                // this timer runs continuously
                while (RetrieveInstanceFile(&info))
                {
                    CreateDrawWindow(&info, TRUE);
                }
                return 0;
            } else
                KillTimer(hwnd, timerid);
            break;
        case WM_HIDEDEBUGWINDOWS:
            if (wParam)
            {
                int old = workArea->dbgview;
                workArea->dbgview = 0;
                workArea->dbgview |= dmgrHideWindow(DID_ASMWND, TRUE) << DID_ASMWND;
                workArea->dbgview |= dmgrHideWindow(DID_WATCHWND, TRUE) <<
                    DID_WATCHWND;
                workArea->dbgview |= dmgrHideWindow(DID_MEMWND, TRUE) << DID_MEMWND;
                workArea->dbgview |= dmgrHideWindow(DID_STACKWND, TRUE) <<
                    DID_STACKWND;
                workArea->dbgview |= dmgrHideWindow(DID_THREADWND, TRUE) <<
                    DID_THREADWND;
                workArea->dbgview |= dmgrHideWindow(DID_REGWND, TRUE) << DID_REGWND;
                if (old != workArea->dbgview)
                    workArea->changed = TRUE;
            }
            else
            {
                if (workArea->dbgview &(1 << DID_ASMWND))
                    dmgrHideWindow(DID_ASMWND, 0);
                if (workArea->dbgview &(1 << DID_WATCHWND))
                    dmgrHideWindow(DID_WATCHWND, 0);
                if (workArea->dbgview &(1 << DID_MEMWND))
                    dmgrHideWindow(DID_MEMWND, 0);
                if (workArea->dbgview &(1 << DID_STACKWND))
                    dmgrHideWindow(DID_STACKWND, 0);
                if (workArea->dbgview &(1 << DID_THREADWND))
                    dmgrHideWindow(DID_THREADWND, 0);
                if (workArea->dbgview &(1 << DID_REGWND))
                    dmgrHideWindow(DID_REGWND, 0);
            }
            break;
        case WM_CLOSE:
            if (uState != notDebugging)
            {
                if (ExtendedMessageBox("Debugger", MB_OKCANCEL, 
                    "Closing the editor will end your debug session.")
                    != IDOK)
                    return 0;
                abortDebug();
                // It takes time for the debuggee to end, we can't close until
                // it is done unless we want to put the system into an abnormal
                // state.  We will keep trying with a timer until the debuggee
                // ends and then we will close out the window.
                timerid = SetTimer(hwnd, IDT_STOPPING, 500, 0);
                return 0;
            }
            if (QuerySaveAll() == IDCANCEL)
                return 0;
            if (making)
            {
                if (ExtendedMessageBox("Build", MB_OKCANCEL, 
                    "Closing the editor will end your build.")
                    != IDOK)
                    return 0;
                SendMessage(hwnd, WM_COMMAND, IDM_STOPBUILD, 0);
                timerid = SetTimer(hwnd, IDT_STOPPING, 500, 0);
                return 0;
            }
            MRUToProfile(0);
            MRUToProfile(1);
            MRDToProfile();
            ExternalToolsToProfile();
            SaveDocksToRegistry();
            SendMessage(hwndProject, WM_COMMAND, IDM_SAVEWS, 0);
            dmgrSetRundown();
            if (hwndProject)
            // MUST be after files are saved for tags to be updated
                DestroyWindow(hwndProject);
            break;
        default:
            break;
    }
    return DefFrameProc(hwnd, hwndClient, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void doSplash(void)
{
    static char buf[256];
    SPLASH splash;

    sprintf(buf, "Version %s", STRING_VERSION);
    splash.hWndOwner = NULL; /* Window owner                  */
    splash.hInstance = hInstance; /* Instance of application       */
    splash.hInstanceRes = hInstance; /* Instance of application (res) */
    splash.lpszResource = "BITMAP_SPLASH"; /* Bitmap resource               */
    splash.uTime = 4000; /* Timeout in milliseconds       */
    splash.bCentered = TRUE; /* Centered-on-screen flag       */
    splash.bTopmost = TRUE; /* Topmost flag                  */
    splash.bWait = FALSE; /* Wait-end flag                 */
    splash.bAbout = FALSE; /* About box */
    splash.iPosX = 0; /* X position                    */
    splash.iPosY = 0; /* Y position                    */
    splash.lpszVersion = buf;

    SplashScreen(&splash);
}

//-------------------------------------------------------------------------

void doAbout(void)
{
    static char buf[256];
    SPLASH splash;

    sprintf(buf, "Version %s", STRING_VERSION);
    splash.hWndOwner = NULL; /* Window owner                  */
    splash.hInstance = hInstance; /* Instance of application       */
    splash.hInstanceRes = hInstance; /* Instance of application (res) */
    splash.lpszResource = "BITMAP_SPLASH"; /* Bitmap resource               */
    splash.bCentered = TRUE; /* Centered-on-screen flag       */
    splash.bTopmost = FALSE; /* Topmost flag                  */
    splash.bWait = FALSE; /* Wait-end flag                 */
    splash.bAbout = TRUE; /* About box */
    splash.iPosX = 0; /* X position                    */
    splash.iPosY = 0; /* Y position                    */
    splash.lpszVersion = buf;
    splash.lpszButtonTitle = "Close";
    splash.bPos.left = 120;
    splash.bPos.top = 180;
    splash.bPos.right = 220;
    splash.bPos.bottom = 210;

    SplashScreen(&splash);
}

//-------------------------------------------------------------------------

LRESULT CALLBACK CursorHookProc(int code, int wParam, int lParam)
{
    CWPRETSTRUCT *s = (CWPRETSTRUCT*)lParam;
    if (s->message == WM_SETCURSOR)
    {
        if (programBusy > 0)
            SetCursor(hCursHourglass);
    }
    return CallNextHookEx(hCursHook, code, wParam, lParam);
}

//-------------------------------------------------------------------------

int IsBusyMessage(MSG *msg)
{
    int rv = 0;
    if (programBusy <= 0)
        return 0;
    switch (msg->message)
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
            rv = 1;
            break;
    }
    return rv;
}
void ProcessMessage(MSG *msg)
{
//	printwmsg(msg->hwnd, msg->message, msg->wParam, msg->lParam);
    HookMouseMovement(msg);
    if (!IsBusyMessage(msg))
    {
        if (ignoreAccel || !TranslateMDISysAccel(hwndClient, msg))
        {
            if (ignoreAccel || !TranslateAccelerator(hwndFrame, hAccel, msg))
            {
                if (IsWindow(hwndFind))
                {
                       DLGHDR *pHdr = (DLGHDR *) GetWindowLong(hwndFind, GWL_USERDATA);
                    if (IsDialogMessage(pHdr->hwndDisplay, msg))
                        return;
                    if (IsDialogMessage(hwndFind, msg))
                        return;
                }
                if (!IsWindow(hwndBookmark) || !IsDialogMessage
                    (hwndBookmark, msg))
                {
                    switch(msg->message)
                    {
                        case WM_LBUTTONDOWN:
                        case WM_RBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                        case WM_KEYDOWN:
                            PropsWndClearEditBox(msg);
                            PostMessage(hwndStatus, SB_SETTEXT, 0 | SBT_NOBORDERS | SBT_OWNERDRAW, (LPARAM)"    ");
                            break;
                    } 
                    TranslateMessage(msg);
                    DispatchMessage(msg);
                }
            }
        }
    }
}
static void GetSystemDialogFont(void)
{
    OSVERSIONINFO osvi;
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
#if !defined(GNUC) && !defined(BORLAND) && !defined(OPENWATCOM)
    memset(&osvi,0,sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx(&osvi);
    if (osvi.dwMajorVersion < 6)
        ncm.cbSize -= sizeof(ncm.iPaddedBorderWidth);
#endif
    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
    {
        systemDialogFont = ncm.lfMessageFont;
        systemMenuFont = ncm.lfMenuFont;
        systemCaptionFont = ncm.lfCaptionFont;
    }
    else
    {
        systemCaptionFont = systemMenuFont = systemDialogFont = NormalFontData;
    }
}
void ApplyDialogFont(HWND hwnd)
{
    HFONT xfont = CreateFontIndirect(&systemDialogFont);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)xfont, 0);
}
void InitFont(BOOL up)
{
    if (up)
    {
        char name[MAX_PATH];
        strcpy(name, szInstallPath);
        strcat(name, "\\bin\\");
        strcat(name, FONTNAME);
        AddFontResourceEx(name, FR_PRIVATE, 0);
    }
}
//-------------------------------------------------------------------------

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
    int nCmdShow)
{
    INITCOMMONCONTROLSEX ccEx;
    char buf[260], buf2[260], *p;
    HWND hwnd;
    HANDLE hsub;
    MSG msg;
    WINDOWPLACEMENT wp;
    lpszCmdLine = GetCommandLineA();
    threadMain = GetCurrentThreadId();
    hInstance = hInst;
//    if (FindWindow(szFrameClassName,0)) {
//		return 0;
//	}
    // so we don't get problems reading unused FP reg values
    _control87(MCW_EM, MCW_EM); 
    hCursArrow = LoadCursor(0, IDC_ARROW);
    hCursHourglass = LoadCursor(0, IDC_WAIT);
    hwnd = FindWindow(szFrameClassName, NULL);
    if ((hwnd && strstr(lpszCmdLine, " /r")) || strstr(lpszCmdLine, "-r"))
    {
        PassFilesToInstance();
        return 0;
    }
    else
    {
        StartInstanceComms();
    }

    GetModuleFileName(0,buf, 260);
    p =strrchr(buf,'\\');
    if (p)
    {
        *p = 0;
    }
    strcpy(szInstallPath, buf);
    p =strrchr(szInstallPath,'\\');
    if (p)
    {
        *p = 0;
    }
    strcat(buf,"\\ocide.cfg");
    strcpy(buf2, szInstallPath);
    GetPrivateProfileString("Init", "InstallPath", buf2, szInstallPath, MAX_PATH,
            buf);
    InitFont(TRUE);
    InitProps();
    findInit();
    RestorePreferences();	
    //CoInitialize(NULL);
    ccEx.dwSize = 0;
    ccEx.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | 
                 ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&ccEx);
    helpMsg = RegisterWindowMessage("commdlg_help");
    editLib = LoadLibrary("riched32.dll"); /* Version 1.0 */
    //   if (!FindWindow(szFrameClassName,0)) {
    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = &WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = 0; // COLOR_APPWORKSPACE + 1 ;
        wc.lpszMenuName = 0;
        wc.lpszClassName = szFrameClassName;
        RegisterClass(&wc);

        RegisterXeditWindow();
        RegisterextTreeWindow();
        RegisterControlWindow(hInstance);
        RegisterFrameWindow(hInstance);
        RegisterToolBarWindow(hInstance);
        RegisterGripWindow(hInstance);
        RegisterContainerWindow(hInstance);
        RegisterBlankWindow(hInstance);
        RegisterLsTabWindow(hInstance);
        RegisterTTIPWindow(hInstance);
        RegisterHistoryComboWindow();
        RegisterPropWindows(hInstance);

        RegisterDrawWindow();
        RegisterProjectWindow();
        RegisterResourceWindow();
        RegisterInfoWindow();
        RegisterASMWindow();
        RegisterStackWindow();
        RegisterThreadWindow();
        RegisterRegisterWindow();
        RegisterMemWindow();
        RegisterTabWindow();
        RegisterPropertyTabWindow();
        RegisterPropsWindow();
        RegisterCtlTbWindow();
        RegisterWatchWindow();
        RegisterJumpListWindow();
        RegisterDlgDrawWindow();
        RegisterAcceleratorDrawWindow();
        RegisterImageDrawWindow();
        RegisterMenuDrawWindow();
        RegisterVersionDrawWindow();
        RegisterStringTableDrawWindow();
        RegisterRCDataDrawWindow();
    }
    
    GetSystemDialogFont();
    
    hMenuMain = LoadMenuGeneric(hInstance, "MAINMENU");

    hsub = GetSubMenu(hMenuMain, WindowMenuItem);
    WindowItemCount = GetMenuItemCount(hsub);
    hAccel = LoadAccelerators(hInstance, "MAINACCELERATORS");

    if (!RestorePlacementFromRegistry(&wp))
    {
        wp.rcNormalPosition.left = wp.rcNormalPosition.right = CW_USEDEFAULT;
        wp.rcNormalPosition.top = wp.rcNormalPosition.bottom = CW_USEDEFAULT;
    }
    else
    {
        wp.rcNormalPosition.right -= wp.rcNormalPosition.left;
        wp.rcNormalPosition.bottom -= wp.rcNormalPosition.top;
    }
    hwndFrame = CreateWindowEx(0, szFrameClassName, "Orange C IDE",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        wp.rcNormalPosition.left, wp.rcNormalPosition.top,
        wp.rcNormalPosition.right, wp.rcNormalPosition.bottom, 0, hMenuMain,
        hInstance, 0);
    UpdateWindow(hwndFrame);
    readytodraw = !(ProfileToInt(psShow, SW_MAXIMIZE) == SW_MAXIMIZE);
    ShowWindow(hwndFrame, ProfileToInt(psShow, SW_MAXIMIZE));
    readytodraw = TRUE;
//    doSplash();

    ProfileToMRU(0);
    ProfileToMRU(1);
    ProfileToMRD();
    ProfileToExternalTools();
    MRUToMenu(0);
    MRUToMenu(1);
    ExternalToolsToMenu();

    InitHelp();
        hbpInit();
    databpInit();
    StartCodeCompletionServer();
    InitMake();
        
    hCursHook = SetWindowsHookEx(WH_CALLWNDPROCRET, (HOOKPROC)CursorHookProc, 0,
        GetCurrentThreadId());
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        ProcessMessage(&msg);
    }
    //CoUninitialize();
    EditorRundown();
    StopInstanceComms();
    FreeLibrary(editLib);
    RundownHelp();
    InitFont(FALSE);
    return msg.wParam;
}
