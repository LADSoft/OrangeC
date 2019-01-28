#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <float.h>
#include <richedit.h>
//#include <dir.h>
#include <sys\stat.h>
#include "header.h"
#include "rc.h"
#include "rcgui.h"
#include "img.h"

IMAGEDATA* NewImage(HWND hwndParent, IMAGEDATA* res);

#ifndef CLEARTYPE_QUALITY
#    define CLEARTYPE_QUALITY (5)
#endif
//************************************************
#define SCREENINDENT 10

#define IDB_CONTROLTOOLBAR 1000
#define IDB_COLORTOOLBAR 1001
#define IDB_COLORSELECTTOOLBAR 1002
#define IDB_FILLTOOLBAR 1003
#define IDB_BRUSHTOOLBAR 1004
#define IDB_ZOOMTOOLBAR 1005
#define IDB_LINETOOLBAR 1006
#define IDB_ERASETOOLBAR 1007
#define IDB_FONTTOOLBAR 1008

#define ID_IMG_STATUS_WINDOW 100
#define ID_CONTROLTOOLBAR 101
#define ID_COLORTOOLBAR 102
#define ID_COLORSELECTTOOLBAR 103
#define ID_SCREENCOLORTOOLBAR 104

#define ID_FILLTOOLBAR 105
#define ID_BRUSHTOOLBAR 106
#define ID_ZOOMTOOLBAR 107
#define ID_LINETOOLBAR 108
#define ID_ERASETOOLBAR 109
#define ID_FONTTOOLBAR 110

#define IDM_POLYSELECT 100
#define IDM_SELECT 101
#define IDM_ERASE 102
#define IDM_FILL 103
#define IDM_PICK 104
#define IDM_ZOOM 105
#define IDM_PENCIL 106
#define IDM_BRUSH 107
#define IDM_AIRBRUSH 108
#define IDM_TEXT 109
#define IDM_LINE 110
#define IDM_HOTSPOT 111
#define IDM_RECTANGLE 112
#define IDM_POLY 113
#define IDM_ELLIPSE 114
#define IDM_ROUNDEDRECTANGLE 115

#define IDM_NEWCURSOR 207
#define IDM_NEWBITMAP 208
#define IDM_NEWICON 209

#define IDM_QUERYSAVE 299

#define IDM_COLORSELECT 300

#define IDM_RESIZEDRAW 400

#define IDM_ERASE_1 900
#define IDM_ERASE_3 901
#define IDM_ERASE_7 902
#define IDM_ERASE_9 903

#define IDM_FILL_HOLLOW 910
#define IDM_FILL_BORDER 911
#define IDM_FILL_FILL 912

#define IDM_LINE_1 920
#define IDM_LINE_2 921
#define IDM_LINE_3 922
#define IDM_LINE_4 923
#define IDM_LINE_5 924

#define IDM_SHOWFONTTB 925

#define IDM_ZOOM_1x 930
#define IDM_ZOOM_2x 931
#define IDM_ZOOM_6x 932
#define IDM_ZOOM_8x 933

#define IDM_BRUSH_ROUND_7 940
#define IDM_BRUSH_ROUND_4 941
#define IDM_BRUSH_ROUND_2 942

#define IDM_BRUSH_SQUARE_7 943
#define IDM_BRUSH_SQUARE_4 944
#define IDM_BRUSH_SQUARE_2 945

#define IDM_BRUSH_DOWN_7 946
#define IDM_BRUSH_DOWN_4 947
#define IDM_BRUSH_DOWN_2 948

#define IDM_BRUSH_UP_7 949
#define IDM_BRUSH_UP_4 950
#define IDM_BRUSH_UP_2 951

#define IDM_BOLD 960
#define IDM_ITALIC 961
#define IDM_UNDERLINE 962
#define IDM_STRIKEOUT 963
#define ID_FONT 964
#define ID_POINTSIZE 965

#define IDM_AUXTOOLBAR 999

#define IDM_COLORBASE 4000
#define IDM_SCREENCOLORBASE 5000

extern HINSTANCE hInstance;
extern PROJECTITEM* workArea;
extern struct propertyFuncs imgFuncs;

char szimageClassName[] = "xccImg";
static char szControlClassName[] = "xccImgControl";
static char szAuxClassName[] = "xccImgAux";
static char szColorClassName[] = "xccImgColor";
static char szColor2ClassName[] = "xccImgColor2";
static char szScreenColorClassName[] = "xccImgScreenColor";
static char szDrawAreaClassName[] = "xccImgDrawArea";
static char szWorkspaceClassName[] = "xccImgWorkspace";

HRESULT WINAPI SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);

static char brushBitmaps[12];

DWORD customImageColors[16] = {
    RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
    RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
    RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
};

static DWORD defaultColors[MAX_COLORS] = {
    RGB(255, 255, 255), RGB(0, 0, 0),     RGB(192, 192, 192), RGB(128, 128, 128), RGB(255, 0, 0),     RGB(128, 0, 0),
    RGB(255, 255, 0),   RGB(128, 128, 0), RGB(0, 255, 0),     RGB(0, 128, 0),     RGB(0, 255, 255),   RGB(0, 128, 128),
    RGB(0, 0, 255),     RGB(0, 0, 128),   RGB(255, 0, 255),   RGB(128, 0, 128),   RGB(255, 255, 128), RGB(128, 128, 64),
    RGB(0, 255, 128),   RGB(0, 64, 64),   RGB(128, 255, 255), RGB(0, 128, 255),   RGB(128, 128, 255), RGB(0, 64, 128),
    RGB(255, 0, 128),   RGB(64, 0, 128),  RGB(255, 128, 64),  RGB(128, 64, 0)};

static HBITMAP drawToolBitmap;
static HBITMAP fillToolBitmap;
static HBITMAP zoomToolBitmap;
static HBITMAP eraseToolBitmap;
static HBITMAP lineToolBitmap;
static HBITMAP brushToolBitmap;
static HBITMAP fontToolBitmap;
static HBITMAP bmpUp;
static HBITMAP bmpDown;

static HCURSOR blankCursor;
static HCURSOR brushCursor;
static HCURSOR hotspotCursor;
static HCURSOR magnifyCursor;
static HCURSOR moveCursor;
static HCURSOR penCursor;
static HCURSOR pickCursor;
static HCURSOR fillCursor;
static HCURSOR objectCursor;
static HCURSOR arrowCursor;
static HCURSOR hcurs, vcurs, diagcurs;

static WNDPROC toolbarHookProc, toolbarScreenHookProc;

static char* tbhints[] = {
    "Poly Select", "PSelect", "Erase", "Flood Fill", "Pick",      "Zoom",    "Pencil",  "Brush",
    "Air Brush",   "Text",    "Line",  "Hot Spot",   "Rectangle", "Polygon", "Ellipse", "Rounded Rectangle",
};

static TBBUTTON toolbarButtons[] = {
    {1, IDM_POLYSELECT, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {0, IDM_SELECT, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {2, IDM_ERASE, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {3, IDM_FILL, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {4, IDM_PICK, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {5, IDM_ZOOM, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {6, IDM_PENCIL, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP},
    {7, IDM_BRUSH, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {10, IDM_LINE, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {9, IDM_TEXT, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {12, IDM_RECTANGLE, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {15, IDM_POLY, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {14, IDM_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {13, IDM_ROUNDEDRECTANGLE, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {11, IDM_HOTSPOT, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {0, 0, 0, 0},
};
static TBBUTTON eraseButtons[] = {
    {0, IDM_ERASE_1, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {1, IDM_ERASE_3, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {2, IDM_ERASE_7, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP},
    {3, IDM_ERASE_9, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {0, 0, 0, 0},
};
static TBBUTTON fillButtons[] = {
    {0, IDM_FILL_HOLLOW, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {1, IDM_FILL_BORDER, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {2, IDM_FILL_FILL, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP},
    {0, 0, 0, 0},
};
static TBBUTTON lineButtons[] = {
    {0, IDM_LINE_1, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP},
    {1, IDM_LINE_2, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {2, IDM_LINE_3, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {3, IDM_LINE_4, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {4, IDM_LINE_5, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {0, 0, 0, 0},
};
static TBBUTTON zoomButtons[] = {
    {0, IDM_ZOOM_1x, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP},
    {1, IDM_ZOOM_2x, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {2, IDM_ZOOM_6x, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {3, IDM_ZOOM_8x, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {0, 0, 0, 0},
};
static TBBUTTON brushButtons[] = {
    {0, IDM_BRUSH_ROUND_7, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {1, IDM_BRUSH_ROUND_4, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP},
    {2, IDM_BRUSH_ROUND_2, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {3, IDM_BRUSH_SQUARE_7, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {4, IDM_BRUSH_SQUARE_4, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {5, IDM_BRUSH_SQUARE_2, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {6, IDM_BRUSH_DOWN_7, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {7, IDM_BRUSH_DOWN_4, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {8, IDM_BRUSH_DOWN_2, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {9, IDM_BRUSH_UP_7, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {10, IDM_BRUSH_UP_4, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {11, IDM_BRUSH_UP_2, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP},
    {0, 0, 0, 0},
};
static char* fontHints[] = {"Font", "Point Size", "Bold", "Italic", "Underline"};
static TBBUTTON fontButtons[] = {{170, 10000, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1},
                                 {70, 10000, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1},
                                 {0, IDM_BOLD, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_CHECK},
                                 {1, IDM_ITALIC, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_CHECK},
                                 {2, IDM_UNDERLINE, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_CHECK},
                                 {0, 0, 0, 0}};
LRESULT CALLBACK toolProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    if (iMessage == WM_LBUTTONDBLCLK)
    {
        int i;
        RECT r1, r2;
        POINT pt;
        IMGDATA* p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
        GetCursorPos(&pt);
        GetWindowRect(hwnd, &r1);
        pt.x -= r1.left;
        pt.y -= r1.top;
        r1.left = pt.x;
        r1.top = pt.y;
        r1.right = pt.x + 1;
        r1.bottom = pt.y + 1;
        for (i = 0; i < MAX_COLORS; i++)
        {
            SendMessage(hwnd, TB_GETITEMRECT, i, (LPARAM)&r2);
            if (IntersectRect(&r2, &r1, &r2))
            {
                int n = i < MAX_COLORS / 2 ? i * 2 : (i - MAX_COLORS / 2) * 2 + 1;
                LRESULT rv = CallWindowProc(toolbarHookProc, hwnd, iMessage, wParam, lParam);
                CHOOSECOLOR c;
                memset(&c, 0, sizeof(c));
                c.lStructSize = sizeof(CHOOSECOLOR);
                c.hwndOwner = hwnd;
                c.rgbResult = GetWindowLong(hwnd, 0);
                c.lpCustColors = customImageColors;
                c.Flags = CC_RGBINIT;
                if (ChooseColor(&c))
                {
                    p->currentPalette[i] = c.rgbResult;
                    InvalidateRect(p->hwndColorToolbar, 0, 0);
                }
                else
                {
                    CommDlgExtendedError();
                }
                return rv;
            }
        }
    }
    return CallWindowProc(toolbarHookProc, hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK toolScreenProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    if (iMessage == WM_LBUTTONDBLCLK || iMessage == WM_RBUTTONDBLCLK)
    {
        int i;
        RECT r1, r2;
        POINT pt;
        IMGDATA* p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
        GetCursorPos(&pt);
        GetWindowRect(hwnd, &r1);
        pt.x -= r1.left;
        pt.y -= r1.top;
        r1.left = pt.x;
        r1.top = pt.y;
        r1.right = pt.x + 1;
        r1.bottom = pt.y + 1;
        for (i = 0; i < 2; i++)
        {
            SendMessage(hwnd, TB_GETITEMRECT, i, (LPARAM)&r2);
            if (IntersectRect(&r2, &r1, &r2))
            {
                if (i == 0)
                {
                    if (iMessage == WM_LBUTTONDBLCLK)
                    {
                        p->screenColor = p->currentLeftColor;
                    }
                    else
                    {
                        p->screenColor = p->currentRightColor;
                    }
                }
                InvalidateRect(p->hwndScreenColorToolbar, 0, 0);
                return 0;
            }
        }
    }
    return CallWindowProc(toolbarScreenHookProc, hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK ScreenColorWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    HDC dc;
    TBBUTTON buttons[40];
    int i;
    LPNMTBCUSTOMDRAW lptcd;
    IMGDATA* p;
    switch (iMessage)
    {
        case WM_NOTIFY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                lptcd = (LPNMTBCUSTOMDRAW)lParam;
                if (lptcd->nmcd.dwDrawStage == CDDS_PREPAINT)
                {
                    return CDRF_NOTIFYITEMDRAW;
                }
                else if (lptcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    int i = lptcd->nmcd.dwItemSpec;
                    if (i == IDM_SCREENCOLORBASE)
                    {
                        lptcd->clrBtnFace = p->screenColor;
                        lptcd->clrBtnHighlight = lptcd->clrBtnFace;
                        return TBCDRF_NOETCHEDEFFECT | TBCDRF_NOOFFSET | CDRF_NEWFONT;
                    }
                    else
                    {
                        lptcd->clrBtnFace = p->screenColor ^ 0xffffff;
                        lptcd->clrBtnHighlight = lptcd->clrBtnFace;
                        return TBCDRF_NOETCHEDEFFECT | TBCDRF_NOOFFSET | CDRF_NEWFONT;
                    }
                }
                return CDRF_DODEFAULT;
            }
            else
            {
                LPTBNOTIFY tbn = (LPTBNOTIFY)lParam;
                if (tbn->hdr.code == NM_RCLICK)
                {
                    if (tbn->iItem == IDM_SCREENCOLORBASE)
                        p->currentRightColor = p->screenColor;
                    else
                        p->currentRightColor = p->screenColor ^ 0xffffff;

                    InvalidateRect(p->hwndSelectToolbar, 0, 0);
                }
            }
            return 0;
        case WM_COMMAND:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            wParam = LOWORD(wParam);
            if (wParam == IDM_SCREENCOLORBASE)
            {
                SendMessage(p->hwndScreenColorToolbar, TB_CHECKBUTTON, wParam, MAKELONG(TRUE, 0));
                p->currentLeftColor = p->screenColor;
                InvalidateRect(p->hwndSelectToolbar, 0, 0);
                return 0;
            }
            else if (wParam == IDM_SCREENCOLORBASE + 1)
            {
                SendMessage(p->hwndScreenColorToolbar, TB_CHECKBUTTON, wParam, MAKELONG(TRUE, 0));
                p->currentLeftColor = p->screenColor ^ 0xffffff;
                InvalidateRect(p->hwndSelectToolbar, 0, 0);
                return 0;
            }
            break;
        case WM_CREATE:
            p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
            p->hwndScreenColorToolbar =
                CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR)NULL,
                               WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER, 0, 0, 0, 0, hwnd,
                               (HMENU)ID_COLORTOOLBAR, hInstance, NULL);
            SetWindowTheme(p->hwndScreenColorToolbar, NULL, L"");
            SendMessage(p->hwndScreenColorToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
            SendMessage(p->hwndScreenColorToolbar, TB_SETBITMAPSIZE, 0, (LPARAM)MAKELONG(10, 10));

            memset(buttons, 0, sizeof(buttons));
            for (i = 0; i < 1; i++)
            {
                buttons[i].iBitmap = i - 1;
                buttons[i].idCommand = IDM_SCREENCOLORBASE + i;
                buttons[i].fsStyle = TBSTYLE_CHECK;
                buttons[i].fsState = TBSTATE_CHECKED;
            }
            SendMessage(p->hwndScreenColorToolbar, TB_ADDBUTTONS, 1, (LPARAM)&buttons);

            SendMessage(p->hwndScreenColorToolbar, TB_GETITEMRECT, 0, (LPARAM)&p->screenColorTbSize);
            p->screenColorTbSize.top = p->screenColorTbSize.left = 0;

            SetWindowLong(p->hwndScreenColorToolbar, GWL_USERDATA, (LONG)p);
            toolbarScreenHookProc = (WNDPROC)SetWindowLong(p->hwndColorToolbar, GWL_WNDPROC, (LONG)toolProc);
            if (p->res->type != FT_BMP)
            {
                ShowWindow(p->hwndScreenColorToolbar, SW_SHOW);
            }
            return 0;
        case WM_DESTROY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            DestroyWindow(p->hwndScreenColorToolbar);
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            break;
        case WM_SIZE:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SendMessage(p->hwndScreenColorToolbar, TB_AUTOSIZE, 0, 0);
            return 0;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK Color2WndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    HDC dc;
    static TBBUTTON buttons[40];
    int i;
    LPNMTBCUSTOMDRAW lptcd;
    IMGDATA* p;
    if (iMessage != WM_GETTEXT)
        switch (iMessage)
        {
            case WM_NOTIFY:
                p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
                {
                    lptcd = (LPNMTBCUSTOMDRAW)lParam;
                    if (lptcd->nmcd.dwDrawStage == CDDS_PREPAINT)
                    {
                        return CDRF_NOTIFYITEMDRAW;
                    }
                    else if (lptcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                    {
                        int i = lptcd->nmcd.dwItemSpec;
                        if (i >= IDM_COLORBASE && i < IDM_COLORBASE + MAX_COLORS)
                        {

                            lptcd->clrBtnFace = p->currentPalette[i - IDM_COLORBASE];
                            lptcd->clrBtnHighlight = lptcd->clrBtnFace;
                            return TBCDRF_NOETCHEDEFFECT | TBCDRF_NOOFFSET | CDRF_NEWFONT;
                        }
                    }
                    return CDRF_DODEFAULT;
                }
                else
                {
                    LPTBNOTIFY tbn = (LPTBNOTIFY)lParam;
                    if (tbn->hdr.code == NM_RCLICK)
                    {
                        p->currentRightColor = p->currentPalette[tbn->iItem - IDM_COLORBASE];
                        InvalidateRect(p->hwndSelectToolbar, 0, 0);
                    }
                }
                break;
            case WM_COMMAND:
                p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                wParam = LOWORD(wParam);
                if (wParam >= IDM_COLORBASE && wParam < IDM_COLORBASE + MAX_COLORS)
                {
                    SendMessage(p->hwndColorToolbar, TB_CHECKBUTTON, wParam, MAKELONG(TRUE, 0));
                    p->currentLeftColor = p->currentPalette[wParam - IDM_COLORBASE];
                    InvalidateRect(p->hwndSelectToolbar, 0, 0);
                }
                break;
            case WM_CREATE:
                p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
                SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
                p->hwndColorToolbar =
                    CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR)NULL,
                                   WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                                   0, 0, 0, 0, hwnd, (HMENU)ID_COLORTOOLBAR, hInstance, NULL);
                SetWindowTheme(p->hwndColorToolbar, NULL, L"");
                SendMessage(p->hwndColorToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
                SendMessage(p->hwndColorToolbar, TB_SETBITMAPSIZE, 0, (LPARAM)MAKELONG(10, 10));
                memcpy(p->currentPalette, defaultColors, sizeof(p->currentPalette));
                memset(buttons, 0, sizeof(buttons));
                if (p->res->saveColors < 8 && p->res->saveColors != 0)
                {
                    for (i = 0; i < 2; i++)
                    {
                        buttons[i].iBitmap = i;
                        buttons[i].idCommand = IDM_COLORBASE + i;
                        buttons[i].fsStyle = TBSTYLE_CHECK;
                        buttons[i].fsState = TBSTATE_CHECKED | TBSTATE_ENABLED;
                    }
                    buttons[0].fsStyle |= TBSTYLE_WRAPABLE;
                    SendMessage(p->hwndColorToolbar, TB_ADDBUTTONS, 2, (LPARAM)buttons);
                }
                else
                {
                    for (i = 0; i < MAX_COLORS; i++)
                    {
                        buttons[i].iBitmap = i;
                        buttons[i].idCommand = IDM_COLORBASE + (i < MAX_COLORS / 2 ? i * 2 : (i - MAX_COLORS / 2) * 2 + 1);
                        buttons[i].fsStyle = TBSTYLE_CHECK;
                        buttons[i].fsState = TBSTATE_CHECKED | TBSTATE_ENABLED;
                    }
                    buttons[MAX_COLORS / 2].fsStyle |= TBSTYLE_WRAPABLE;
                    SendMessage(p->hwndColorToolbar, TB_ADDBUTTONS, MAX_COLORS, (LPARAM)buttons);
                }
                SendMessage(p->hwndColorToolbar, TB_SETROWS, MAKEWPARAM(2, FALSE), (LPARAM)&p->colorTbSize);
                SetWindowLong(p->hwndColorToolbar, GWL_USERDATA, (LONG)p);
                toolbarHookProc = (WNDPROC)SetWindowLong(p->hwndColorToolbar, GWL_WNDPROC, (LONG)toolProc);
                break;
            case WM_DESTROY:
                p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                DestroyWindow(p->hwndColorToolbar);
                break;
            case WM_PAINT:
                dc = BeginPaint(hwnd, &paint);
                EndPaint(hwnd, &paint);
                break;
            case WM_SIZE:
                p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                SendMessage(p->hwndColorToolbar, TB_AUTOSIZE, 0, 0);
                break;
        }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void PaintOneCurrentColor(HDC dc, RECT* r, DWORD color)
{
    int cx = GetSystemMetrics(SM_CXDLGFRAME);
    int cy = GetSystemMetrics(SM_CYDLGFRAME);
    HBRUSH hBr = CreateSolidBrush(color);
    RECT r2 = *r;
    r2.left -= cx;
    r2.top -= cy;
    r2.right += cx;
    r2.bottom += cy;
    DrawFrameControl(dc, &r2, DFC_BUTTON, 0);
    FillRect(dc, r, hBr);
    DeleteObject(hBr);
}

void PaintCurrentColors(IMGDATA* p, LPNMTBCUSTOMDRAW lptcd)
{
    int cx = GetSystemMetrics(SM_CXDLGFRAME);
    int cy = GetSystemMetrics(SM_CYDLGFRAME);
    RECT r = lptcd->nmcd.rc;
    //    OffsetRect(&r, p->controlTbSize.right + 3 * GetSystemMetrics(SM_CXDLGFRAME), 0);
    r.right -= 7;
    r.bottom -= 7;
    r.top = r.bottom - 10;
    r.left = r.right - 10;
    PaintOneCurrentColor(lptcd->nmcd.hdc, &r, p->currentRightColor);
    r = lptcd->nmcd.rc;
    //    OffsetRect(&r, p->controlTbSize.right + 3 * GetSystemMetrics(SM_CXDLGFRAME), 0);
    r.top += 7;
    r.left += 7;
    r.bottom = r.top + 10;
    r.right = r.left + 10;
    PaintOneCurrentColor(lptcd->nmcd.hdc, &r, p->currentLeftColor);
}

LRESULT CALLBACK ColorWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    HDC dc;
    RECT rf;
    TBBUTTON buttons[1];
    LPNMTBCUSTOMDRAW lptcd;
    IMGDATA* p;
    switch (iMessage)
    {
        case WM_NOTIFY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                lptcd = (LPNMTBCUSTOMDRAW)lParam;
                if (lptcd->nmcd.dwDrawStage == CDDS_PREPAINT)
                {
                    return CDRF_NOTIFYITEMDRAW;
                }
                else if (lptcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    int i = lptcd->nmcd.dwItemSpec;
                    if (i == IDM_COLORSELECT)
                    {
                        return TBCDRF_NOETCHEDEFFECT | TBCDRF_NOOFFSET | CDRF_NOTIFYPOSTPAINT;
                    }
                }
                else if (lptcd->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
                {
                    int i = lptcd->nmcd.dwItemSpec;
                    if (i == IDM_COLORSELECT)
                    {
                        PaintCurrentColors(p, lptcd);
                    }
                }
                return CDRF_DODEFAULT;
            }
            break;
        case WM_CREATE:
            p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
            p->hwndColor2 = CreateWindowEx(0, szColor2ClassName, "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0,
                                           0, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, 0, hInstance, p);
            p->hwndScreenColor =
                CreateWindowEx(0, szScreenColorClassName, "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, 0, hInstance, p);
            memset(buttons, 0, sizeof(buttons));
            buttons[0].iBitmap = 0;
            buttons[0].idCommand = IDM_COLORSELECT;
            buttons[0].fsStyle = TBSTYLE_CHECK;
            buttons[0].fsState = TBSTATE_CHECKED;
            p->hwndSelectToolbar = CreateToolbarEx(
                hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                ID_COLORTOOLBAR, 1, hInstance, 0 /*IDB_TOOLBAR*/, buttons, 1, 24, 24, 24, 24, sizeof(TBBUTTON));
            SetWindowLong(p->hwndSelectToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SetWindowLong(p->hwndSelectToolbar, GWL_USERDATA, (LONG)p);
            SendMessage(p->hwndSelectToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
            SendMessage(p->hwndSelectToolbar, TB_GETITEMRECT, 0, (LPARAM)&p->selectTbSize);
            p->selectTbSize.top = p->selectTbSize.left = 0;
            return 0;
        case WM_PAINT:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            dc = BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            break;
        case WM_DESTROY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            DestroyWindow(p->hwndColor2);
            DestroyWindow(p->hwndScreenColor);
            DestroyWindow(p->hwndSelectToolbar);
            break;
        case WM_SIZE:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            /*
            rf.left =  0;
            rf.top = 0;
            rf.right = rf.left + p->selectTbSize.right ;
            rf.bottom = HIWORD(lParam) ;
            MoveWindow(p->hwndSelectToolbar, rf.left + GetSystemMetrics(SM_CXFRAME),
                       rf.top + GetSystemMetrics(SM_CYFRAME),
                       p->selectTbSize.right - p->selectTbSize.left,
                       rf.bottom - rf.top, 1);
            */
            SendMessage(p->hwndSelectToolbar, TB_AUTOSIZE, 0, 0);

            rf.top = 0;
            rf.left = p->selectTbSize.right + SCREENINDENT;
            rf.right = LOWORD(lParam);
            rf.bottom = HIWORD(lParam);
            MoveWindow(p->hwndScreenColor, rf.left, rf.top, p->screenColorTbSize.right - p->screenColorTbSize.left,
                       rf.bottom - rf.top, 1);

            rf.top = 0;
            rf.left = p->selectTbSize.right + p->screenColorTbSize.right + SCREENINDENT * 2;
            rf.right = rf.left + p->colorTbSize.right - p->colorTbSize.left;
            rf.bottom = HIWORD(lParam);
            MoveWindow(p->hwndColor2, rf.left, rf.top, p->colorTbSize.right - p->colorTbSize.left + 4,
                       p->colorTbSize.bottom - p->colorTbSize.top + 4, 1);
            return 0;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void SetLastDrawMode(IMGDATA* p)
{
    SendMessage(p->hwndControlToolbar, TB_CHECKBUTTON, p->lastDrawMode, TRUE);
    SendMessage(p->hwndControl, WM_COMMAND, p->lastDrawMode, 0);
}
static void PositionCaret(IMGDATA* p)
{
    HFONT hf;
    HDC dc;
    SIZE sz;
    dc = GetDC(p->hwndDrawArea);
    hf = SelectObject(dc, p->drawFont);
    GetTextExtentPoint32(dc, p->textBuf, p->textLen, &sz);
    hf = SelectObject(dc, p->drawFont);
    if (sz.cx > (p->selectRect.right - p->selectRect.left) * p->zoom)
        sz.cx = (p->selectRect.right - p->selectRect.left) * p->zoom;
    SetCaretPos(p->selectRect.left * p->zoom - p->showX + sz.cx, p->selectRect.top * p->zoom - p->showY);
}
static void DoShowCaret(IMGDATA* p)
{
    HFONT hf;
    HDC dc;
    TEXTMETRIC tm;
    dc = GetDC(p->hwndDrawArea);
    hf = SelectObject(dc, p->drawFont);
    GetTextMetrics(dc, &tm);
    hf = SelectObject(dc, p->drawFont);
    ReleaseDC(p->hwndDrawArea, dc);
    CreateCaret(p->hwndDrawArea, 0, 5, tm.tmHeight + tm.tmInternalLeading);
    ShowCaret(p->hwndDrawArea);
    PositionCaret(p);
    p->shownCaret = TRUE;
}
static void DoHideCaret(IMGDATA* p)
{
    if (p->shownCaret)
    {
        p->shownCaret = FALSE;
        HideCaret(p->hwndDrawArea);
        DestroyCaret();
    }
}
static void unselectDo(IMGDATA* p)
{
    if (p->selected)
    {
        BitBlt(p->res->hdcImage, p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x,
               p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y, p->selectRect.right - p->selectRect.left,
               p->selectRect.bottom - p->selectRect.top, p->selectDC, 0, 0, SRCCOPY);
        DeleteObject(p->selectbmp);
        DeleteDC(p->selectDC);
        p->selectbmp = NULL;
        p->selectDC = NULL;
        p->selected = FALSE;
    }
    DoHideCaret(p);
}
static void selectDo(IMGDATA* p)
{
    if (!p->selected)
    {
        HBRUSH brush;
        p->selectDC = CreateCompatibleDC(p->res->hdcImage);
        if (!p->selectDC)
            return;
        p->selectbmp = CreateCompatibleBitmap(p->res->hdcImage,

                                              p->selectRect.right - p->selectRect.left, p->selectRect.bottom - p->selectRect.top);
        if (!p->selectbmp)
        {
            DeleteDC(p->selectDC);
            return;
        }
        DeleteObject(SelectObject(p->selectDC, p->selectbmp));
        BitBlt(p->selectDC, 0, 0, p->selectRect.right - p->selectRect.left, p->selectRect.bottom - p->selectRect.top,
               p->res->hdcImage, p->selectRect.left, p->selectRect.top, SRCCOPY);
        brush = CreateSolidBrush(RGB_WHITE);
        FillRect(p->res->hdcImage, &p->selectRect, brush);
        DeleteObject(brush);
        p->selectStartPos.x = 32767;
        p->selectStartPos.y = 32767;
        p->selectMovePos = p->selectStartPos;
        p->selected = TRUE;
    }
}
LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    RECT rf, rs;
    IMGDATA* p;
    switch (iMessage)
    {
        case WM_COMMAND:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            p->lastDrawMode = p->drawMode;
            p->drawMode = wParam;
            unselectDo(p);
            p->polyStarted = FALSE;
            SendMessage(p->hwndAux, WM_COMMAND, IDM_AUXTOOLBAR, 0);
            if (p->drawMode == IDM_TEXT)
                SendMessage(p->hwndSelf, WM_COMMAND, IDM_SHOWFONTTB, 1);
            else if (p->lastDrawMode == IDM_TEXT)
                SendMessage(p->hwndSelf, WM_COMMAND, IDM_SHOWFONTTB, 0);

            switch (p->drawMode)
            {
                case IDM_POLYSELECT:
                case IDM_SELECT:
                case IDM_TEXT:
                case IDM_ERASE:
                default:
                    p->cursor = arrowCursor;
                    break;
                case IDM_PICK:
                    p->cursor = pickCursor;
                    break;
                case IDM_PENCIL:
                    p->cursor = penCursor;
                    break;
                case IDM_FILL:
                    p->cursor = fillCursor;
                    break;
                case IDM_ZOOM:
                    p->cursor = magnifyCursor;
                    break;
                case IDM_BRUSH:
                    p->cursor = brushCursor;
                    break;
                case IDM_HOTSPOT:
                    p->cursor = hotspotCursor;
                    break;
                case IDM_LINE:
                case IDM_RECTANGLE:
                case IDM_POLY:
                case IDM_ELLIPSE:
                case IDM_ROUNDEDRECTANGLE:
                    p->cursor = objectCursor;
                    break;
            }
            break;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == TTN_NEEDTEXT)
            {
                LPTOOLTIPTEXT lpt = (LPTOOLTIPTEXT)lParam;
                lpt->lpszText = tbhints[lpt->hdr.idFrom - IDM_POLYSELECT];
            }
            break;
        case WM_CREATE:
            p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            p->cursor = arrowCursor;
            p->drawMode = IDM_PENCIL;
            p->cursor = penCursor;
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);

            p->hwndControlToolbar = CreateToolbarEx(hwnd,
                                                    WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS | WS_VISIBLE |
                                                        TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                                                    ID_CONTROLTOOLBAR, 16, 0 /*hInstance*/, (UINT)drawToolBitmap /*IDB_TOOLBAR*/,
                                                    &toolbarButtons[0], 15, 32, 32, 32, 32, sizeof(TBBUTTON));
            SetWindowLong(p->hwndControlToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SendMessage(p->hwndControlToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
            SendMessage(p->hwndControlToolbar, TB_SETROWS, MAKEWPARAM(5, FALSE), (LPARAM)&p->controlTbSize);
            return 0;
        case WM_DESTROY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            DestroyWindow(p->hwndControlToolbar);
            break;
        case WM_SIZE:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            rf.left = 0;
            rf.top = 0;
            rf.right = LOWORD(lParam);
            rf.bottom = HIWORD(lParam);
            MoveWindow(p->hwndControlToolbar, rf.left, rf.top, p->controlTbSize.right - p->controlTbSize.left,
                       p->controlTbSize.bottom - p->controlTbSize.top, 1);
            return 0;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static void SetupFont(IMGDATA* p)
{
    if (p->selected && p->drawMode == IDM_TEXT)
    {
        int n = SendMessage(p->hwndPointSizeCombo, CB_GETCURSEL, 0, 0);
        int m = SendMessage(p->hwndFontCombo, CB_GETCURSEL, 0, 0);
        if (n != CB_ERR && m != CB_ERR)
        {
            int nPtSize;
            char buf[256];
            LOGFONT lf;
            buf[SendMessage(p->hwndPointSizeCombo, CB_GETLBTEXT, n, (LPARAM)buf)] = 0;
            nPtSize = atoi(buf);
            lf = *(LOGFONT*)SendMessage(p->hwndFontCombo, CB_GETITEMDATA, m, 0);
            lf.lfHeight = -MulDiv(nPtSize, GetDeviceCaps(p->res->hdcImage, LOGPIXELSY), 72);
            lf.lfHeight *= p->zoom;
            lf.lfWidth = 0;
            if (SendMessage(p->hwndFontTB, TB_ISBUTTONCHECKED, IDM_BOLD, 0))
                lf.lfWeight = FW_BOLD;
            if (SendMessage(p->hwndFontTB, TB_ISBUTTONCHECKED, IDM_ITALIC, 0))
                lf.lfItalic = TRUE;
            if (SendMessage(p->hwndFontTB, TB_ISBUTTONCHECKED, IDM_UNDERLINE, 0))
                lf.lfUnderline = TRUE;

            DeleteObject(p->drawFont);
            p->drawFont = CreateFontIndirect(&lf);
            DoShowCaret(p);
        }
    }
}
LRESULT CALLBACK AuxWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    IMGDATA* p;
    int rows;
    LPNMTBCUSTOMDRAW lptcd;
    switch (iMessage)
    {
        case WM_NOTIFY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                lptcd = (LPNMTBCUSTOMDRAW)lParam;
                if (lptcd->nmcd.dwDrawStage == CDDS_PREPAINT)
                {
                    return CDRF_NOTIFYITEMDRAW;
                }
                else if (lptcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    return TBCDRF_NOETCHEDEFFECT | TBCDRF_NOOFFSET | TBCDRF_NOEDGES | CDRF_NOTIFYPOSTPAINT;
                }
                else if (lptcd->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
                {
                    if (lptcd->nmcd.dwItemSpec >= IDM_BRUSH_ROUND_7 && lptcd->nmcd.dwItemSpec <= IDM_BRUSH_UP_2)
                    {
                        HBRUSH brush = CreateSolidBrush(RGB_BLACK);
                        HPEN pen = CreatePen(PS_SOLID, 0, RGB_BLACK);
                        brush = SelectObject(lptcd->nmcd.hdc, brush);
                        pen = SelectObject(lptcd->nmcd.hdc, pen);
                        switch (lptcd->nmcd.dwItemSpec)
                        {
                            case IDM_BRUSH_ROUND_7:
                                Ellipse(lptcd->nmcd.hdc, lptcd->nmcd.rc.left, lptcd->nmcd.rc.top + 2, lptcd->nmcd.rc.left + 7,
                                        lptcd->nmcd.rc.top + 9);
                                break;
                            case IDM_BRUSH_ROUND_4:
                                Ellipse(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 2, lptcd->nmcd.rc.top + 3, lptcd->nmcd.rc.left + 6,
                                        lptcd->nmcd.rc.top + 7);
                                break;
                            case IDM_BRUSH_ROUND_2:
                                Ellipse(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 3, lptcd->nmcd.rc.top + 4, lptcd->nmcd.rc.left + 5,
                                        lptcd->nmcd.rc.top + 6);
                                break;

                            case IDM_BRUSH_SQUARE_7:
                                Rectangle(lptcd->nmcd.hdc, lptcd->nmcd.rc.left, lptcd->nmcd.rc.top + 2, lptcd->nmcd.rc.left + 7,
                                          lptcd->nmcd.rc.top + 9);
                                break;
                            case IDM_BRUSH_SQUARE_4:
                                Rectangle(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 2, lptcd->nmcd.rc.top + 3, lptcd->nmcd.rc.left + 6,
                                          lptcd->nmcd.rc.top + 7);
                                break;
                            case IDM_BRUSH_SQUARE_2:
                                Rectangle(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 3, lptcd->nmcd.rc.top + 4, lptcd->nmcd.rc.left + 5,
                                          lptcd->nmcd.rc.top + 6);
                                break;

                            case IDM_BRUSH_UP_7:
                                MoveToEx(lptcd->nmcd.hdc, lptcd->nmcd.rc.left, lptcd->nmcd.rc.top + 2, NULL);
                                LineTo(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 7, lptcd->nmcd.rc.top + 9);
                                break;
                            case IDM_BRUSH_UP_4:
                                MoveToEx(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 2, lptcd->nmcd.rc.top + 3, NULL);
                                LineTo(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 6, lptcd->nmcd.rc.top + 7);
                                break;
                            case IDM_BRUSH_UP_2:
                                MoveToEx(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 3, lptcd->nmcd.rc.top + 4, NULL);
                                LineTo(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 5, lptcd->nmcd.rc.top + 6);
                                break;

                            case IDM_BRUSH_DOWN_7:
                                MoveToEx(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 7, lptcd->nmcd.rc.top + 2, NULL);
                                LineTo(lptcd->nmcd.hdc, lptcd->nmcd.rc.left, lptcd->nmcd.rc.top + 9);
                                break;
                            case IDM_BRUSH_DOWN_4:
                                MoveToEx(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 6, lptcd->nmcd.rc.top + 3, NULL);
                                LineTo(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 2, lptcd->nmcd.rc.top + 7);
                                break;
                            case IDM_BRUSH_DOWN_2:
                                MoveToEx(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 5, lptcd->nmcd.rc.top + 4, NULL);
                                LineTo(lptcd->nmcd.hdc, lptcd->nmcd.rc.left + 3, lptcd->nmcd.rc.top + 6);
                                break;
                        }
                        brush = SelectObject(lptcd->nmcd.hdc, brush);
                        pen = SelectObject(lptcd->nmcd.hdc, pen);
                        DeleteObject(brush);
                        DeleteObject(pen);
                    }
                }
                return CDRF_DODEFAULT;
            }
            break;
        case WM_CREATE:
            p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
            p->hwndEraseToolbar = CreateToolbarEx(
                hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER, ID_ERASETOOLBAR, 4, 0,
                (UINT)eraseToolBitmap, &eraseButtons[0], 4, 28, 12, 28, 12, sizeof(TBBUTTON));
            SetWindowLong(p->hwndEraseToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SetParent(p->hwndEraseToolbar, NULL);
            p->hwndFillToolbar =
                CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                                ID_FILLTOOLBAR, 3, 0, (UINT)fillToolBitmap, &fillButtons[0], 3, 28, 18, 28, 18, sizeof(TBBUTTON));
            SetWindowLong(p->hwndFillToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SetParent(p->hwndFillToolbar, NULL);
            p->hwndLineToolbar =
                CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                                ID_LINETOOLBAR, 5, 0, (UINT)lineToolBitmap, &lineButtons[0], 5, 28, 8, 28, 8, sizeof(TBBUTTON));
            SetWindowLong(p->hwndLineToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SetParent(p->hwndLineToolbar, NULL);
            p->hwndZoomToolbar =
                CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                                ID_ZOOMTOOLBAR, 4, 0, (UINT)zoomToolBitmap, &zoomButtons[0], 4, 28, 12, 28, 12, sizeof(TBBUTTON));
            SetWindowLong(p->hwndZoomToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SetParent(p->hwndZoomToolbar, NULL);
            p->hwndBrushToolbar =
                CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
                                ID_BRUSHTOOLBAR, 12, hInstance, 0, &brushButtons[0], 12, 5, 12, 5, 12, sizeof(TBBUTTON));
            SetWindowLong(p->hwndBrushToolbar, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            SetParent(p->hwndBrushToolbar, NULL);
            break;
        case WM_COMMAND:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            switch (LOWORD(wParam))
            {
                case IDM_AUXTOOLBAR:
                    if (p->hwndSelectedToolbar)
                    {
                        ShowWindow(p->hwndSelectedToolbar, SW_HIDE);
                        SetParent(p->hwndSelectedToolbar, NULL);
                    }
                    p->hwndSelectedToolbar = NULL;
                    switch (p->drawMode)
                    {
                        case IDM_RECTANGLE:
                        case IDM_ELLIPSE:
                        case IDM_ROUNDEDRECTANGLE:
                        case IDM_POLY:
                            p->hwndSelectedToolbar = p->hwndFillToolbar;
                            rows = 3;
                            break;
                        case IDM_LINE:
                            p->hwndSelectedToolbar = p->hwndLineToolbar;
                            rows = 5;
                            break;
                        case IDM_BRUSH:
                            p->hwndSelectedToolbar = p->hwndBrushToolbar;
                            rows = 4;
                            break;
                        case IDM_ERASE:
                            p->hwndSelectedToolbar = p->hwndEraseToolbar;
                            rows = 4;
                            break;
                        case IDM_ZOOM:
                            p->hwndSelectedToolbar = p->hwndZoomToolbar;
                            rows = 4;
                            break;
                        default:
                            break;
                    }
                    if (p->hwndSelectedToolbar)
                    {
                        RECT rc;
                        SetParent(p->hwndSelectedToolbar, hwnd);
                        GetClientRect(hwnd, &rc);
                        MoveWindow(p->hwndSelectedToolbar, rc.left + GetSystemMetrics(SM_CYDLGFRAME), rc.top,
                                   rc.right + GetSystemMetrics(SM_CYDLGFRAME) - rc.left, rc.bottom - rc.top, 1);
                        SendMessage(p->hwndSelectedToolbar, TB_SETROWS, MAKEWPARAM(rows, FALSE), (LPARAM)&rc);
                        ShowWindow(p->hwndSelectedToolbar, SW_SHOW);
                    }
                    InvalidateRect(hwnd, 0, 0);
                    break;
                case IDM_ERASE_1:
                    p->eraseWidth = 1;
                    break;
                case IDM_ERASE_3:
                    p->eraseWidth = 3;
                    break;
                case IDM_ERASE_7:
                    p->eraseWidth = 7;
                    break;
                case IDM_ERASE_9:
                    p->eraseWidth = 9;
                    break;

                case IDM_FILL_HOLLOW:
                    p->fillType = FT_HOLLOW;
                    break;
                case IDM_FILL_BORDER:
                    p->fillType = FT_BORDER;
                    break;
                case IDM_FILL_FILL:
                    p->fillType = FT_FILL;
                    break;

                case IDM_LINE_1:
                    p->lineWidth = 0;
                    break;
                case IDM_LINE_2:
                    p->lineWidth = 2;
                    break;
                case IDM_LINE_3:
                    p->lineWidth = 3;
                    break;
                case IDM_LINE_4:
                    p->lineWidth = 4;
                    break;
                case IDM_LINE_5:
                    p->lineWidth = 5;
                    break;

                case IDM_ZOOM_1x:
                    p->zoom = 1;
                    SetupFont(p);
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SetLastDrawMode(p);
                    InvalidateRect(p->hwndDrawArea, 0, 0);
                    break;
                case IDM_ZOOM_2x:
                    p->zoom = 2;
                    SetupFont(p);
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SetLastDrawMode(p);
                    InvalidateRect(p->hwndDrawArea, 0, 0);
                    break;
                case IDM_ZOOM_6x:
                    p->zoom = 6;
                    SetupFont(p);
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SetLastDrawMode(p);
                    InvalidateRect(p->hwndDrawArea, 0, 0);
                    break;
                case IDM_ZOOM_8x:
                    p->zoom = 8;
                    SetupFont(p);
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SetLastDrawMode(p);
                    InvalidateRect(p->hwndDrawArea, 0, 0);
                    break;

                case IDM_BRUSH_ROUND_2:
                case IDM_BRUSH_ROUND_4:
                case IDM_BRUSH_ROUND_7:
                case IDM_BRUSH_SQUARE_2:
                case IDM_BRUSH_SQUARE_4:
                case IDM_BRUSH_SQUARE_7:
                case IDM_BRUSH_DOWN_2:
                case IDM_BRUSH_DOWN_4:
                case IDM_BRUSH_DOWN_7:
                case IDM_BRUSH_UP_2:
                case IDM_BRUSH_UP_4:
                case IDM_BRUSH_UP_7:
                    p->brushType = wParam;
                    break;
            }
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static void printSize(IMGDATA* p, int x, int y)
{
    char buf[256];
    RECT r;
    POINT pt;
    x -= p->sizePos.x;
    y -= p->sizePos.y;
    sprintf(buf, "%d x %d", (x + p->showX) / p->zoom + p->res->width, (y + p->showY) / p->zoom + p->res->height);
    SendMessage(p->hwndStatus, SB_SETTEXT, 3, (LPARAM)buf);
    pt.x = 0;
    pt.y = 0;
    ClientToScreen(p->hwndWorkspace, &pt);
    r.left = pt.x;
    r.top = pt.y;
    r.right = pt.x + x + p->res->width * p->zoom;
    r.bottom = pt.y + y + p->res->height * p->zoom;
    DrawBoundingRect(&r);
}
static void printPos(IMGDATA* p, int x, int y)
{
    char buf[256];
    sprintf(buf, "%d x %d", (x + p->showX) / p->zoom, (y + p->showY) / p->zoom);
    SendMessage(p->hwndStatus, SB_SETTEXT, 1, (LPARAM)buf);
}
static void buttonDown(HWND hwnd, IMGDATA* p, int right, int x, int y)
{
    HPEN pen;
    HBRUSH brush;
    POINT pt, opt;
    DWORD color;
    char buf[256];
    p->right = right;
    pt.x = (x + p->showX) / p->zoom;
    pt.y = (y + p->showY) / p->zoom;
    opt = pt;
    switch (p->drawMode)
    {
        case IDM_ZOOM:
        {
            POINT pt2;
            RECT r;
            GetClientRect(p->hwndDrawArea, &r);
            switch (p->zoom)
            {
                case 1:
                    p->zoom = 2;
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SendMessage(p->hwndZoomToolbar, TB_CHECKBUTTON, IDM_ZOOM_2x, TRUE);
                    break;
                case 2:
                    p->zoom = 6;
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SendMessage(p->hwndZoomToolbar, TB_CHECKBUTTON, IDM_ZOOM_6x, TRUE);
                    break;
                case 6:
                    p->zoom = 8;
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    SendMessage(p->hwndZoomToolbar, TB_CHECKBUTTON, IDM_ZOOM_8x, TRUE);
                    break;
                default:
                    break;
            }
            pt2 = pt;
            pt2.x = pt2.x - r.right / p->zoom / 2;
            pt2.y = pt2.y - r.bottom / p->zoom / 2;
            if (pt2.x > p->res->width - r.right / p->zoom)
                pt2.x = p->res->width - r.right / p->zoom;
            if (pt2.x < 0)
                pt2.x = 0;
            if (pt2.y > p->res->height - r.bottom / p->zoom)
                pt2.y = p->res->height - r.bottom / p->zoom;
            if (pt2.y < 0)
                pt2.y = 0;
            p->showX = pt2.x * p->zoom;
            p->showY = pt2.y * p->zoom;
            InvalidateRect(hwnd, 0, 0);
        }
            SetLastDrawMode(p);
            break;
        case IDM_HOTSPOT:
            p->res->hotspotX = pt.x;
            p->res->hotspotY = pt.y;
            sprintf(buf, "%d x %d", p->res->hotspotX, p->res->hotspotY);
            SendMessage(p->hwndStatus, SB_SETTEXT, 2, (LPARAM)buf);
            break;
        case IDM_PICK:
            unselectDo(p);
            color = GetPixel(p->res->hdcImage, pt.x, pt.y);
            if (right)
                p->currentRightColor = color;
            else
                p->currentLeftColor = color;
            SetLastDrawMode(p);
            SendMessage(p->hwndControlToolbar, TB_CHECKBUTTON, p->drawMode, TRUE);
            InvalidateRect(p->hwndSelectToolbar, 0, 0);
            break;
        case IDM_FILL:
            unselectDo(p);
            InsertImageUndo(p->hwndSelf, p->res);
            if (!right)
                brush = CreateSolidBrush(p->currentLeftColor);
            else
                brush = CreateSolidBrush(p->currentRightColor);
            DeleteObject(SelectObject(p->res->hdcImage, brush));
            Fill(p, p->res, pt);
            InvalidateRect(hwnd, 0, 0);
            break;
        case IDM_SELECT:
            if (p->selected)
            {
                if (pt.x >= p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x &&
                    pt.x < p->selectRect.right + p->selectMovePos.x - p->selectStartPos.x &&
                    pt.y >= p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y &&
                    pt.y < p->selectRect.bottom + p->selectMovePos.y - p->selectStartPos.y)
                {
                    p->captured = TRUE;
                    SetCursor(moveCursor);
                    p->cursor = moveCursor;
                    if (p->selectStartPos.x == 32767)
                    {
                        p->selectMovePos = pt;
                        p->selectStartPos = p->selectMovePos;
                    }
                    else
                    {
                        p->selectStartPos.x += pt.x - p->selectMovePos.x;
                        p->selectStartPos.y += pt.y - p->selectMovePos.y;
                        p->selectMovePos = pt;
                    }
                }
                return;
            }
            // fallthrough
        case IDM_TEXT:
            if (p->drawMode == IDM_TEXT && p->selected)
            {
                unselectDo(p);
                if (p->textLen)
                {
                    int n = SendMessage(p->hwndPointSizeCombo, CB_GETCURSEL, 0, 0);
                    int m = SendMessage(p->hwndFontCombo, CB_GETCURSEL, 0, 0);
                    if (n != CB_ERR && m != CB_ERR)
                    {
                        int nPtSize;
                        char buf[256];
                        LOGFONT lf;
                        HFONT hf;
                        HBITMAP bmp;
                        HDC wndDC;
                        HDC hdcMem;
                        buf[SendMessage(p->hwndPointSizeCombo, CB_GETLBTEXT, n, (LPARAM)buf)] = 0;
                        nPtSize = atoi(buf);
                        lf = *(LOGFONT*)SendMessage(p->hwndFontCombo, CB_GETITEMDATA, m, 0);
                        lf.lfHeight = -MulDiv(nPtSize, GetDeviceCaps(p->res->hdcImage, LOGPIXELSY), 72);
                        lf.lfWidth = 0;
                        lf.lfQuality = CLEARTYPE_QUALITY;
                        if (SendMessage(p->hwndFontTB, TB_ISBUTTONCHECKED, IDM_BOLD, 0))
                            lf.lfWeight = FW_BOLD;
                        if (SendMessage(p->hwndFontTB, TB_ISBUTTONCHECKED, IDM_ITALIC, 0))
                            lf.lfItalic = TRUE;
                        if (SendMessage(p->hwndFontTB, TB_ISBUTTONCHECKED, IDM_UNDERLINE, 0))
                            lf.lfUnderline = TRUE;

                        // most of the effort here is to get the aliasing, which
                        // doesn't work on palette-based color schemes
                        hdcMem = CreateCompatibleDC(p->res->hdcImage);
                        hf = CreateFontIndirect(&lf);
                        hf = SelectObject(hdcMem, hf);
                        wndDC = GetDC(p->hwndDrawArea);
                        bmp = LocalCreateBitmap(wndDC, p->res->width, p->res->height, 0);
                        bmp = SelectObject(hdcMem, bmp);
                        BitBlt(hdcMem, 0, 0, p->res->width, p->res->height, p->res->hdcImage, 0, 0, SRCCOPY);
                        SetBkMode(hdcMem, TRANSPARENT);
                        SetTextColor(hdcMem, p->currentLeftColor);
                        IntersectClipRect(hdcMem, p->selectRect.left, p->selectRect.top, p->selectRect.right, p->selectRect.bottom);
                        TextOut(hdcMem, p->selectRect.left, p->selectRect.top, p->textBuf, p->textLen);
                        BitBlt(p->res->hdcImage, 0, 0, p->res->width, p->res->height, hdcMem, 0, 0, SRCCOPY);
                        bmp = SelectObject(hdcMem, bmp);
                        DeleteObject(bmp);
                        hf = SelectObject(hdcMem, hf);
                        DeleteObject(hf);
                        DeleteDC(hdcMem);
                        ReleaseDC(p->hwndDrawArea, wndDC);
                        InvalidateRect(hwnd, 0, 0);
                        if (!p->res->imageDirty)
                        {
                            p->res->imageDirty = TRUE;
                            ResSetDirty(p->resource);
                        }
                    }
                }
                break;
            }
            // fallthrough
        case IDM_ELLIPSE:
        case IDM_RECTANGLE:
        case IDM_ROUNDEDRECTANGLE:
            unselectDo(p);
            p->captured = 1;
            SetCapture(hwnd);
            InsertImageUndo(p->hwndSelf, p->res);
            p->rubber = CreateRubberBand(p->res,
                                         p->drawMode == IDM_RECTANGLE || p->drawMode == IDM_SELECT || p->drawMode == IDM_TEXT
                                             ? RT_RECTANGLE
                                             : p->drawMode == IDM_ROUNDEDRECTANGLE ? RT_ROUNDEDRECTANGLE : RT_ELLIPSE,
                                         p->drawMode == IDM_SELECT || p->drawMode == IDM_TEXT ? FT_HOLLOW : p->fillType, pt);
            if ((right && (p->fillType != FT_FILL || p->drawMode == IDM_SELECT || p->drawMode == IDM_TEXT)) ||
                (p->drawMode != IDM_SELECT && p->drawMode != IDM_TEXT && !right && p->fillType == FT_FILL))
            {
                pen = CreatePen(PS_SOLID, 0, p->currentRightColor);
                DeleteObject(SelectObject(p->res->hdcImage, pen));
                brush = CreateSolidBrush(p->currentLeftColor);
                DeleteObject(SelectObject(p->res->hdcImage, brush));
            }
            else
            {
                pen = CreatePen(PS_SOLID, 0, p->currentLeftColor);
                DeleteObject(SelectObject(p->res->hdcImage, pen));
                brush = CreateSolidBrush(p->currentRightColor);
                DeleteObject(SelectObject(p->res->hdcImage, brush));
            }
            InvalidateRect(hwnd, 0, 0);
            break;
        case IDM_POLY:
            if (!p->polyStarted)
            {
                InsertImageUndo(p->hwndSelf, p->res);
                p->polyStarted = TRUE;
                p->polyPointCount = 0;
                p->polyPoints[p->polyPointCount++] = pt;
                if (right)
                {
                    brush = CreateSolidBrush(p->currentLeftColor);
                    DeleteObject(SelectObject(p->res->hdcImage, brush));
                }
                else
                {
                    brush = CreateSolidBrush(p->currentRightColor);
                    DeleteObject(SelectObject(p->res->hdcImage, brush));
                }
            }
            else
                opt = p->polyPoints[p->polyPointCount - 1];
            /* fallthrough */
        case IDM_LINE:
            unselectDo(p);
            p->captured = 1;
            SetCapture(hwnd);
            if (p->drawMode == IDM_LINE)
                InsertImageUndo(p->hwndSelf, p->res);
            if (!right)
                pen = CreatePen(PS_SOLID, p->lineWidth, p->currentLeftColor);
            else
                pen = CreatePen(PS_SOLID, p->lineWidth, p->currentRightColor);
            DeleteObject(SelectObject(p->res->hdcImage, pen));
            p->rubber = CreateRubberBand(p->res, RT_LINE, FT_HOLLOW, opt);
            RubberBand(p, p->rubber, pt);
            InvalidateRect(hwnd, 0, 0);
            break;
        case IDM_POLYSELECT:
            if (p->selected)
            {
                if (pt.x >= p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x &&
                    pt.x < p->selectRect.right + p->selectMovePos.x - p->selectStartPos.x &&
                    pt.y >= p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y &&
                    pt.y < p->selectRect.bottom + p->selectMovePos.y - p->selectStartPos.y)
                {
                    p->captured = TRUE;
                    SetCursor(moveCursor);
                    p->cursor = moveCursor;
                    if (p->selectStartPos.x == 32767)
                    {
                        p->selectMovePos = pt;
                        p->selectStartPos = p->selectMovePos;
                    }
                    else
                    {
                        p->selectStartPos.x += pt.x - p->selectMovePos.x;
                        p->selectStartPos.y += pt.y - p->selectMovePos.y;
                        p->selectMovePos = pt;
                    }
                }
                return;
            }
            p->selectRect.left = pt.x;
            p->selectRect.top = pt.y;

            p->selectRect.right = pt.x;
            p->selectRect.bottom = pt.y;
            /* fallthrough */
        case IDM_PENCIL:
        case IDM_BRUSH:
        case IDM_ERASE:
            if (p->drawMode == IDM_ERASE)
            {
                SetCursor(NULL);
                p->cursor = NULL;
            }
            unselectDo(p);
            p->captured = 1;
            SetCapture(hwnd);
            InsertImageUndo(p->hwndSelf, p->res);
            if (p->drawMode == IDM_PENCIL)
            {
                if (!right)
                    pen = CreatePen(PS_SOLID, p->lineWidth, p->currentLeftColor);
                else
                    pen = CreatePen(PS_SOLID, p->lineWidth, p->currentRightColor);
            }
            else
            {
                LOGBRUSH lbrush;
                if (p->drawMode == IDM_ERASE)
                    lbrush.lbColor = RGB_WHITE;
                else if (!right)
                    lbrush.lbColor = p->currentLeftColor;
                else
                    lbrush.lbColor = p->currentRightColor;
                if (p->drawMode == IDM_BRUSH)
                {
                    switch (p->brushType)
                    {
                        case IDM_BRUSH_ROUND_2:
                            lbrush.lbStyle = BS_SOLID;
                            pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND, 2, &lbrush, 0, 0);
                            break;
                        case IDM_BRUSH_ROUND_4:
                            lbrush.lbStyle = BS_SOLID;
                            pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND, 5, &lbrush, 0, 0);
                            break;
                        case IDM_BRUSH_ROUND_7:
                            lbrush.lbStyle = BS_SOLID;
                            pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND, 8, &lbrush, 0, 0);
                            break;
                        case IDM_BRUSH_SQUARE_2:
                            lbrush.lbStyle = BS_SOLID;
                            pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_SQUARE, 2, &lbrush, 0, 0);
                            break;
                        case IDM_BRUSH_SQUARE_4:
                            lbrush.lbStyle = BS_SOLID;
                            pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_SQUARE, 5, &lbrush, 0, 0);
                            break;
                        case IDM_BRUSH_SQUARE_7:
                            lbrush.lbStyle = BS_SOLID;
                            pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_SQUARE, 8, &lbrush, 0, 0);
                            break;
                        case IDM_BRUSH_DOWN_2:
                        case IDM_BRUSH_DOWN_4:
                        case IDM_BRUSH_DOWN_7:
                        case IDM_BRUSH_UP_2:
                        case IDM_BRUSH_UP_4:
                        case IDM_BRUSH_UP_7:
                            pen = CreatePen(PS_SOLID, 0, lbrush.lbColor);
                            brush = CreateSolidBrush(lbrush.lbColor);
                            DeleteObject(SelectObject(p->res->hdcImage, brush));
                            break;
                    }
                }
                else
                {
                    lbrush.lbStyle = BS_SOLID;
                    pen = ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND, 4, &lbrush, 0, 0);
                }
            }
            DeleteObject(SelectObject(p->res->hdcImage, pen));
            DrawPoint(p, p->res, pt);
            InvalidateRect(hwnd, 0, 0);
            break;
    }
}
static void buttonUp(HWND hwnd, IMGDATA* p, int right, int x, int y)
{
    switch (p->drawMode)
    {
        case IDM_SELECT:
        case IDM_TEXT:
            if (p->selected)
            {
                SetCursor(arrowCursor);
                p->cursor = arrowCursor;
                p->captured = FALSE;
                return;
            }
        case IDM_ELLIPSE:
        case IDM_RECTANGLE:
        case IDM_ROUNDEDRECTANGLE:
        case IDM_LINE:
        case IDM_POLY:
            /* this may happen if we get a double click */
            if (!p->rubber)
                return;
            if (p->drawMode == IDM_POLY)
            {
                if (p->polyPointCount >= POLY_POINTS_MAX)
                    return;
                p->polyPoints[p->polyPointCount].x = p->rubber->x + p->rubber->width;
                p->polyPoints[p->polyPointCount++].y = p->rubber->y + p->rubber->height;
            }
            if (p->drawMode == IDM_SELECT || p->drawMode == IDM_TEXT)
            {
                p->selectRect.left = p->rubber->x;
                p->selectRect.top = p->rubber->y;
                p->selectRect.right = p->rubber->x + p->rubber->width + 1;
                p->selectRect.bottom = p->rubber->y + p->rubber->height + 1;
                selectDo(p);
                if (p->drawMode == IDM_TEXT)
                {
                    p->textLen = 0;
                    SetupFont(p);
                }
            }
            else
                InstantiateRubberBand(p, p->res, p->rubber);
            DeleteRubberBand(p, p->rubber);
            p->rubber = NULL;
            /* fall through */
        case IDM_PENCIL:
        case IDM_BRUSH:
        case IDM_POLYSELECT:
        case IDM_ERASE:
            if (p->drawMode == IDM_ERASE)
            {
                SetCursor(arrowCursor);
                p->cursor = arrowCursor;
            }
            if (p->drawMode == IDM_POLYSELECT)
            {
                if (p->selected || p->skipSelect)
                {
                    p->captured = 0;
                    SetCursor(arrowCursor);
                    p->cursor = arrowCursor;
                    p->skipSelect = FALSE;
                    return;
                }
                p->selectRect.right++;
                p->selectRect.bottom++;
                /* get rid of what we just drew */
                ApplyImageUndo(p, p->res);
                SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                selectDo(p);
                p->captured = 0;
                ReleaseCapture();
            }
            p->captured = 0;
            ReleaseCapture();
        case IDM_FILL:
            if (p->res->type != FT_BMP)
                RecoverAndMask(p->res);
            InvalidateRect(hwnd, 0, 0);
            break;
    }
}
static void mouseMove(HWND hwnd, IMGDATA* p, int x, int y)
{
    POINT pt;
    pt.x = (x + p->showX) / p->zoom;
    pt.y = (y + p->showY) / p->zoom;
    if ((short)pt.x < 0)
        pt.x = 0;
    if ((short)pt.y < 0)
        pt.y = 0;
    if (p->captured)
    {
        switch (p->drawMode)
        {
            case IDM_SELECT:
            case IDM_TEXT:
                if (p->selected)
                {
                    if (pt.x < p->res->width && pt.y < p->res->height)
                        p->selectMovePos = pt;
                    break;
                }
            case IDM_POLY:
            case IDM_ELLIPSE:
            case IDM_RECTANGLE:
            case IDM_ROUNDEDRECTANGLE:
            case IDM_LINE:
                RubberBand(p, p->rubber, pt);
                break;
            case IDM_POLYSELECT:
                if (p->selected)
                {
                    p->selectMovePos = pt;
                    break;
                }
                if (pt.x < p->selectRect.left)
                    p->selectRect.left = pt.x;
                if (pt.x > p->selectRect.right)
                    p->selectRect.right = pt.x;
                if (pt.y < p->selectRect.top)
                    p->selectRect.top = pt.y;
                if (pt.y > p->selectRect.bottom)
                    p->selectRect.bottom = pt.y;
                /* fall through */
            case IDM_BRUSH:
            {
                int leftx = -100, lefty, rightx, righty;
                switch (p->brushType)
                {
                    case IDM_BRUSH_DOWN_2:
                        leftx = righty = -1;
                        lefty = rightx = 1;
                        break;
                    case IDM_BRUSH_DOWN_4:
                        leftx = righty = -2;
                        lefty = rightx = 3;
                        break;
                    case IDM_BRUSH_DOWN_7:
                        leftx = righty = -4;
                        lefty = rightx = 4;
                        break;
                    case IDM_BRUSH_UP_2:
                        leftx = lefty = -1;
                        rightx = righty = 1;
                        break;
                    case IDM_BRUSH_UP_4:
                        leftx = lefty = -2;
                        rightx = righty = 3;
                        break;
                    case IDM_BRUSH_UP_7:
                        leftx = lefty = -4;
                        rightx = righty = 4;
                        break;
                    default:
                        DrawLine(p->res->hdcImage, p->cursorPos, pt);
                        break;
                }
                if (leftx != -100)
                {
                    BeginPath(p->res->hdcImage);
                    MoveToEx(p->res->hdcImage, p->cursorPos.x + leftx, p->cursorPos.y + lefty, NULL);
                    LineTo(p->res->hdcImage, pt.x + leftx, pt.y + lefty);
                    LineTo(p->res->hdcImage, pt.x + rightx, pt.y + righty);
                    LineTo(p->res->hdcImage, p->cursorPos.x + rightx, p->cursorPos.y + righty);
                    LineTo(p->res->hdcImage, p->cursorPos.x + leftx, p->cursorPos.y + lefty);
                    EndPath(p->res->hdcImage);
                    StrokeAndFillPath(p->res->hdcImage);
                }
            }
            break;
            case IDM_PENCIL:
            case IDM_ERASE:
                DrawLine(p->res->hdcImage, p->cursorPos, pt);
                break;
            default:
                return;
        }
        InvalidateRect(hwnd, 0, 0);
    }
    printPos(p, x, y);
    p->cursorPos = pt;
    if (p->drawMode == IDM_ERASE || (p->selected && p->captured))
        InvalidateRect(hwnd, 0, 0);
}
IMAGEDATA* newRes(HWND hwndParent, int type, int width, int height, int colors)
{
    IMAGEDATA res;
    memset(&res, 0, sizeof(res));
    res.colors = colors; /* 24-bit */
    res.height = width;
    res.width = height;
    res.type = type;
    return NewImage(hwndParent, &res);
}
#if 0
IMAGEDATA *removeRes(IMGDATA *p)
{
    switch (SendMessage(p->hwndDrawArea, WM_COMMAND, IDM_QUERYSAVE, 0))
    {
        case IDYES:
            if (p->fileName[0])
                SendMessage(p->hwndDrawArea, WM_COMMAND, IDM_SAVE, 0);
            else
                SendMessage(p->hwndDrawArea, WM_COMMAND, IDM_SAVEAS, 0);
            /* fall through */
        case IDNO:
            FreeImageResource(p->res);
            p->res = NULL;
            return TRUE;
        case IDCANCEL:
            return FALSE;
    }
}
#endif
void FlipOrRotate(IMGDATA* p, XFORM* lpXForm, int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    if (p->selected)
    {
        int x = p->selectRect.right - p->selectRect.left;
        int y = p->selectRect.bottom - p->selectRect.top;
        int m = x > y ? x : y;
        if (m % 2)
            m++;
        unselectDo(p);
        InsertImageUndo(p->hwndSelf, p->res);
        SetGraphicsMode(p->res->hdcImage, GM_ADVANCED);
        SetWorldTransform(p->res->hdcImage, lpXForm);
        StretchBlt(p->res->hdcImage, x2, y2, w2, h2, p->res->undo->hdcImage, x1, y1, w1, h1, SRCCOPY);
        ModifyWorldTransform(p->res->hdcImage, NULL, MWT_IDENTITY);
        SetGraphicsMode(p->res->hdcImage, GM_COMPATIBLE);
        if (!p->res->imageDirty)
        {
            p->res->imageDirty = TRUE;
            ResSetDirty(p->resource);
        }
    }
}
LRESULT CALLBACK DrawAreaWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static int captured;
    PAINTSTRUCT paint;
    HDC dc;
    IMGDATA* p;
    RECT rf;
    char buf[256];
    switch (iMessage)
    {
        case WM_KEYDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            switch (wParam)
            {
                case VK_ESCAPE:
                    unselectDo(p);
                    break;
                case VK_PRIOR:
                    switch (p->zoom)
                    {
                        case 1:
                        default:
                            return 0;
                        case 2:
                            p->zoom = 1;
                            break;
                        case 6:
                            p->zoom = 2;
                            break;
                        case 8:
                            p->zoom = 6;
                            break;
                    }
                    SetupFont(p);
                    SendMessage(GetParent(hwnd), WM_COMMAND, IDM_RESIZEDRAW, 0);
                    InvalidateRect(hwnd, 0, 0);
                    break;
                case VK_NEXT:
                    switch (p->zoom)
                    {
                        case 1:
                        default:
                            p->zoom = 2;
                            break;
                        case 2:
                            p->zoom = 6;
                            break;
                        case 6:
                            p->zoom = 8;
                            break;
                        case 8:
                            return 0;
                    }
                    SetupFont(p);
                    SendMessage(GetParent(hwnd), WM_COMMAND, IDM_RESIZEDRAW, 0);
                    InvalidateRect(hwnd, 0, 0);
                    break;
                case 'S':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
                    }
                    break;
                case 'Z':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_UNDO, 0);
                    }
                    break;
                case 'C':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_COPY, 0);
                    }
                    break;
                case 'V':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_PASTE, 0);
                    }
                    break;
                case 'X':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_CUT, 0);
                    }
                    break;
                case 'A':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_SELECTALL, 0);
                    }
                    break;
                case 'H':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        if (p->selected && p->drawMode == IDM_TEXT && p->textLen)
                        {
                            p->textLen--;
                            PositionCaret(p);
                            InvalidateRect(hwnd, 0, 0);
                        }
                    }
                    break;
                case VK_BACK:
                case VK_DELETE:
                    if (p->selected && p->drawMode == IDM_TEXT && p->textLen)
                    {
                        p->textLen--;
                        PositionCaret(p);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    break;
            }
            switch (KeyboardToAscii(wParam, lParam, FALSE))
            {
                case '[':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PopupResFullScreen(GetParent(GetParent(GetParent(hwnd))));
                        return 0;
                    }
                    break;
                case ']':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        ReleaseResFullScreen(GetParent(GetParent(GetParent(hwnd))));
                        return 0;
                    }
                    break;
            }
            break;
        case WM_CHAR:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (wParam >= ' ' && p->selected && p->drawMode == IDM_TEXT)
            {
                if (p->textLen < sizeof(p->textBuf))
                {
                    p->textBuf[p->textLen++] = wParam;
                    PositionCaret(p);
                    InvalidateRect(hwnd, 0, 0);
                }
            }
            break;
        case WM_SETFOCUS:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetResourceProperties(p->resource, &imgFuncs);
            if (p->drawMode == IDM_TEXT && p->selected)
            {
                DoShowCaret(p);
            }
            break;
        case WM_KILLFOCUS:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            DoHideCaret(p);
            //            SetResourceProperties(NULL, NULL);
            break;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT)
            {
                p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                SetCursor(p->cursor);
                return TRUE;
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_SAVE:
                {
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    unselectDo(p);
                    if (p->resource->resource->changed)
                        ResSaveCurrent(workArea, p->resource);
                }
                break;
                case IDM_CLEAR:
                case IDM_CLEARSELECTION:
                {
                    int selected;
                    HBRUSH brush;
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    selected = p->selected;
                    p->polyStarted = FALSE;
                    unselectDo(p);
                    InsertImageUndo(p->hwndSelf, p->res);
                    brush = CreateSolidBrush(p->currentRightColor);
                    brush = SelectObject(p->res->hdcImage, brush);
                    if (selected)
                        PatBlt(p->res->hdcImage, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                               p->selectRect.bottom - p->selectRect.top, PATCOPY);
                    else
                        PatBlt(p->res->hdcImage, 0, 0, p->res->width, p->res->height, PATCOPY);
                    brush = CreateSolidBrush(p->res->rgbScreen);
                    DeleteObject(brush);
                    InvalidateRect(hwnd, 0, 0);
                    if (!p->res->imageDirty)
                    {
                        p->res->imageDirty = TRUE;
                        ResSetDirty(p->resource);
                    }
                }
                break;
                case IDM_FLIPHORZ:
                {
                    XFORM xx = {-1, 0, 0, 1};
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    xx.eDx = p->selectRect.right + p->selectRect.left - 1;
                    FlipOrRotate(p, &xx, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                                 p->selectRect.bottom - p->selectRect.top, p->selectRect.left, p->selectRect.top,
                                 p->selectRect.right - p->selectRect.left, p->selectRect.bottom - p->selectRect.top);
                    selectDo(p);
                    InvalidateRect(hwnd, 0, 0);
                }
                break;
                case IDM_FLIPVERT:
                {
                    XFORM xx = {1, 0, 0, -1};
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    xx.eDy = p->selectRect.bottom + p->selectRect.top - 1;
                    FlipOrRotate(p, &xx, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                                 p->selectRect.bottom - p->selectRect.top, p->selectRect.left, p->selectRect.top,
                                 p->selectRect.right - p->selectRect.left, p->selectRect.bottom - p->selectRect.top);
                    selectDo(p);
                    InvalidateRect(hwnd, 0, 0);
                }
                break;
                case IDM_ROT90CW:
                {
                    XFORM xx = {0, 1, -1, 0};
                    RECT r;
                    int x1;
                    int y1;
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    x1 = (p->selectRect.right + p->selectRect.left) / 2;
                    y1 = (p->selectRect.bottom + p->selectRect.top) / 2;
                    xx.eDx = x1 + y1;
                    xx.eDy = y1 - x1;
                    FlipOrRotate(p, &xx, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                                 p->selectRect.bottom - p->selectRect.top, -p->selectRect.bottom + xx.eDx,
                                 p->selectRect.left + xx.eDy, p->selectRect.bottom - p->selectRect.top,
                                 p->selectRect.right - p->selectRect.left);
                    r.left = -p->selectRect.bottom + xx.eDx;
                    r.top = p->selectRect.left + xx.eDy;
                    r.right = r.left + p->selectRect.bottom - p->selectRect.top;
                    r.bottom = r.top + p->selectRect.right - p->selectRect.left;
                    p->selectRect = r;
                    selectDo(p);
                    InvalidateRect(hwnd, 0, 0);
                }
                break;
                case IDM_ROT90CCW:
                {
                    XFORM xx = {0, -1, 1, 0};
                    RECT r;
                    int x1;
                    int y1;
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    x1 = (p->selectRect.right + p->selectRect.left) / 2;
                    y1 = (p->selectRect.bottom + p->selectRect.top) / 2;
                    xx.eDx = x1 - y1;
                    xx.eDy = y1 + x1;
                    FlipOrRotate(p, &xx, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                                 p->selectRect.bottom - p->selectRect.top, p->selectRect.top + xx.eDx,
                                 -p->selectRect.right + xx.eDy, p->selectRect.bottom - p->selectRect.top,
                                 p->selectRect.right - p->selectRect.left);
                    r.left = p->selectRect.top + xx.eDx;
                    r.top = -p->selectRect.right + xx.eDy;
                    r.right = r.left + p->selectRect.bottom - p->selectRect.top;
                    r.bottom = r.top + p->selectRect.right - p->selectRect.left;
                    p->selectRect = r;
                    selectDo(p);
                    InvalidateRect(hwnd, 0, 0);
                }
                break;
                case IDM_ROT180:
                {
                    XFORM xx = {-1, 0, 0, -1};
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    xx.eDx = p->selectRect.right + p->selectRect.left - 1;
                    xx.eDy = p->selectRect.bottom + p->selectRect.top - 1;
                    FlipOrRotate(p, &xx, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                                 p->selectRect.bottom - p->selectRect.top, p->selectRect.left, p->selectRect.top,
                                 p->selectRect.right - p->selectRect.left, p->selectRect.bottom - p->selectRect.top);
                    selectDo(p);
                    InvalidateRect(hwnd, 0, 0);
                }
                break;
                case IDM_UNDO:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    unselectDo(p);
                    ApplyImageUndo(p, p->res);
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    InvalidateRect(hwnd, 0, 0);
                    break;
                case IDM_CUT:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    if (p->selected)
                    {
                        unselectDo(p);
                        InsertImageUndo(p->hwndSelf, p->res);
                        CopyImageToClipboard(p->hwndSelf, p->res, &p->selectRect);
                        PatBlt(p->res->hdcImage, p->selectRect.left, p->selectRect.top, p->selectRect.right - p->selectRect.left,
                               p->selectRect.bottom - p->selectRect.top, WHITENESS);
                        InvalidateRect(hwnd, 0, 0);
                        if (!p->res->imageDirty)
                        {
                            p->res->imageDirty = TRUE;
                            ResSetDirty(p->resource);
                        }
                    }
                    break;
                case IDM_COPY:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    if (p->selected)
                    {
                        unselectDo(p);
                        CopyImageToClipboard(p->hwndSelf, p->res, &p->selectRect);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    break;
                case IDM_PASTE:
                {
                    IMAGEDATA* data;
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    unselectDo(p);
                    InsertImageUndo(p->hwndSelf, p->res);
                    p->polyStarted = FALSE;
                    data = PasteImageFromClipboard(p->hwndSelf, p->res);
                    if (data)
                    {
                        IMAGEDATA* oldres = p->res;
                        p->res = data;
                        SendMessage(p->hwndControlToolbar, TB_CHECKBUTTON, IDM_SELECT, MAKELPARAM(TRUE, 0));
                        p->drawMode = IDM_SELECT;
                        p->cursor = arrowCursor;
                        p->selectRect.left = 0;
                        p->selectRect.top = 0;
                        p->selectRect.right = p->selectRect.left + data->width;
                        p->selectRect.bottom = p->selectRect.top + data->height;
                        InsertImageUndo(p->hwndSelf, p->res);
                        selectDo(p);
                        p->selectRect.left = p->showX / p->zoom;
                        p->selectRect.top = p->showY / p->zoom;
                        p->selectRect.right = p->selectRect.left + data->width;
                        p->selectRect.bottom = p->selectRect.top + data->height;
                        p->res = oldres;
                        FreeImageResource(data);
                        InvalidateRect(hwnd, 0, 0);
                        if (!p->res->imageDirty)
                        {
                            p->res->imageDirty = TRUE;
                            ResSetDirty(p->resource);
                        }
                    }
                }
                break;
                case IDM_SELECTALL:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    if (p->res)
                    {
                        unselectDo(p);
                        GetClientRect(hwnd, &rf);
                        p->selectRect.left = p->showX / p->zoom;
                        p->selectRect.top = p->showY / p->zoom;
                        p->selectRect.right = p->res->width + p->showX / p->zoom;
                        p->selectRect.bottom = p->res->height + p->showY / p->zoom;
                        SendMessage(p->hwndControlToolbar, TB_CHECKBUTTON, IDM_SELECT, MAKELPARAM(TRUE, 0));
                        p->drawMode = IDM_SELECT;
                        p->cursor = arrowCursor;
                        selectDo(p);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    break;
                case IDM_RESIZEDRAW:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    if (p->res)
                        MoveWindow(hwnd, 0, 0, p->res->width * p->zoom, p->res->height * p->zoom, 0);
                    break;
            }
            break;
        case WM_CREATE:
            p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
            break;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (p->polyStarted)
            {
                if (p->polyPointCount >= 2)
                {
                    /* get rid of all the poly edges */
                    ApplyImageUndo(p, p->res);
                    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    InsertImageUndo(p->hwndSelf, p->res);
                    if (p->fillType != FT_HOLLOW)
                    {
                        HBRUSH brush;
                        if (p->right)
                        {
                            brush = CreateSolidBrush(p->currentLeftColor);
                        }
                        else
                        {
                            brush = CreateSolidBrush(p->currentRightColor);
                        }
                        DeleteObject(SelectObject(p->res->hdcImage, brush));
                        SetPolyFillMode(p->res->hdcImage, ALTERNATE);
                        if (p->fillType == FT_BORDER)
                        {
                            Polygon(p->res->hdcImage, p->polyPoints, p->polyPointCount);
                        }
                        else
                        {
                            HDC memDC = CreateCompatibleDC(p->res->hdcImage);
                            HBITMAP bitmap = CreateBitmap(p->res->width, p->res->height, 1, 1, NULL);
                            HPEN pen = CreatePen(PS_SOLID, 0, RGB_BLACK);
                            HBRUSH whitebrush = CreateSolidBrush(RGB_WHITE);
                            DeleteObject(SelectObject(memDC, bitmap));
                            DeleteObject(SelectObject(memDC, pen));
                            DeleteObject(SelectObject(memDC, whitebrush));
                            Polygon(memDC, p->polyPoints, p->polyPointCount);
                            SetBkColor(p->res->hdcImage, RGB_WHITE);
                            SetTextColor(p->res->hdcImage, RGB_BLACK);
                            BitBlt(p->res->hdcImage, 0, 0, p->res->width, p->res->height, memDC, 0, 0, DSPDxax);
                        }
                    }
                    else
                    {
                        p->polyPoints[p->polyPointCount] = p->polyPoints[0];
                        Polyline(p->res->hdcImage, p->polyPoints, p->polyPointCount + 1);
                    }
                    p->polyStarted = FALSE;
                }
            }
            unselectDo(p);
            if (p->drawMode == IDM_POLYSELECT)
                p->skipSelect = TRUE;
            InvalidateRect(hwnd, 0, 0);
            break;
        case WM_LBUTTONDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetFocus(p->hwndDrawArea);
            buttonDown(hwnd, p, FALSE, LOWORD(lParam), HIWORD(lParam));

            break;
        case WM_RBUTTONDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetFocus(p->hwndDrawArea);
            buttonDown(hwnd, p, TRUE, LOWORD(lParam), HIWORD(lParam));

            break;
        case WM_MBUTTONDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetFocus(p->hwndDrawArea);
            break;
        case WM_MOUSEMOVE:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            mouseMove(hwnd, p, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            buttonUp(hwnd, p, FALSE, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_NCHITTEST:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            p->mouseInClient = TRUE;
            //			GetWindowRect(hwnd, &rf);
            //			sprintf(buf,"%d x %d", (LOWORD(lParam) - rf.left)/p->zoom, (HIWORD(lParam) - rf.top)/ p->zoom);
            //			SendMessage(p->hwndStatus, SB_SETTEXT, 1, (LPARAM)buf);
            break;
        case WM_DESTROY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (p->res)
            {
                FreeImageResource(p->res);
                p->res = NULL;
            }
            break;
        case WM_PAINT:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            dc = BeginPaint(hwnd, &paint);
            if (p->res)
            {
                RECT rc;
                HDC hdcMem = CreateCompatibleDC(dc);
                HBITMAP bitmap = CreateCompatibleBitmap(p->res->hdcImage, p->res->width * p->zoom, p->res->height * p->zoom);
                HBITMAP oldbitmap = SelectObject(hdcMem, bitmap);
                GetClientRect(hwnd, &rc);
                StretchBlt(hdcMem, 0, 0, p->res->width * p->zoom, p->res->height * p->zoom, p->res->hdcImage, 0, 0, p->res->width,
                           p->res->height, SRCCOPY);
                if (p->rubber)
                {
                    HBRUSH brush;
                    if (p->drawMode == IDM_ELLIPSE || p->drawMode == IDM_RECTANGLE || p->drawMode == IDM_ROUNDEDRECTANGLE)

                        if ((p->right && p->fillType != FT_FILL) || (!p->right && p->fillType == FT_FILL))
                            brush = CreateSolidBrush(p->currentRightColor);
                        else
                            brush = CreateSolidBrush(p->currentLeftColor);
                    else if (p->right)
                        brush = CreateSolidBrush(p->currentRightColor);
                    else
                        brush = CreateSolidBrush(p->currentLeftColor);
                    brush = SelectObject(hdcMem, brush);
                    SetBkColor(hdcMem, RGB_WHITE);
                    SetTextColor(hdcMem, RGB_BLACK);
                    StretchBlt(hdcMem, 0, 0, p->res->width * p->zoom, p->res->height * p->zoom, p->rubber->hdcRubberBorder, 0, 0,
                               p->res->width, p->res->height, DSPDxax);
                    brush = SelectObject(hdcMem, brush);
                    DeleteObject(brush);
                    if (p->drawMode == IDM_ELLIPSE || p->drawMode == IDM_RECTANGLE || p->drawMode == IDM_ROUNDEDRECTANGLE)

                        if ((p->right && p->fillType != FT_FILL) || (!p->right && p->fillType == FT_FILL))
                            brush = CreateSolidBrush(p->currentLeftColor);
                        else
                            brush = CreateSolidBrush(p->currentRightColor);
                    else if (p->right)
                        brush = CreateSolidBrush(p->currentLeftColor);
                    else
                        brush = CreateSolidBrush(p->currentRightColor);
                    brush = SelectObject(hdcMem, brush);
                    StretchBlt(hdcMem, 0, 0, p->res->width * p->zoom, p->res->height * p->zoom, p->rubber->hdcRubberFill, 0, 0,
                               p->res->width, p->res->height, DSPDxax);
                    brush = SelectObject(hdcMem, brush);
                    DeleteObject(brush);
                }
                if (p->drawMode == IDM_ERASE && p->mouseInClient)
                {
                    Rectangle(hdcMem, (p->cursorPos.x - p->eraseWidth / 2) * p->zoom,
                              (p->cursorPos.y - p->eraseWidth / 2) * p->zoom, (p->cursorPos.x + p->eraseWidth / 2 + 1) * p->zoom,
                              (p->cursorPos.y + p->eraseWidth / 2 + 1) * p->zoom);
                }
                if (p->selected)
                {
                    HPEN pen = CreatePen(PS_DOT, 0, RGB_BLACK);
                    StretchBlt(hdcMem, (p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x) * p->zoom,
                               (p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y) * p->zoom,
                               (p->selectRect.right - p->selectRect.left) * p->zoom,
                               (p->selectRect.bottom - p->selectRect.top) * p->zoom, p->selectDC, 0, 0,
                               p->selectRect.right - p->selectRect.left, p->selectRect.bottom - p->selectRect.top, SRCCOPY);
                    pen = SelectObject(hdcMem, pen);
                    MoveToEx(hdcMem, (p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x) * p->zoom,
                             (p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y) * p->zoom, NULL);
                    LineTo(hdcMem, (p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x) * p->zoom,
                           (p->selectRect.bottom + p->selectMovePos.y - p->selectStartPos.y) * p->zoom);
                    LineTo(hdcMem, (p->selectRect.right + p->selectMovePos.x - p->selectStartPos.x) * p->zoom,
                           (p->selectRect.bottom + p->selectMovePos.y - p->selectStartPos.y) * p->zoom);
                    LineTo(hdcMem, (p->selectRect.right + p->selectMovePos.x - p->selectStartPos.x) * p->zoom,
                           (p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y) * p->zoom);
                    LineTo(hdcMem, (p->selectRect.left + p->selectMovePos.x - p->selectStartPos.x) * p->zoom,
                           (p->selectRect.top + p->selectMovePos.y - p->selectStartPos.y) * p->zoom);
                    pen = SelectObject(hdcMem, pen);
                    DeleteObject(pen);
                    if (p->drawMode == IDM_TEXT && p->textLen)
                    {
                        // this has to be last because it lessens the clip region...
                        HFONT hf = SelectObject(hdcMem, p->drawFont);
                        SetTextColor(hdcMem, p->currentLeftColor);
                        SetBkMode(hdcMem, TRANSPARENT);
                        IntersectClipRect(hdcMem, p->selectRect.left * p->zoom, p->selectRect.top * p->zoom,
                                          p->selectRect.right * p->zoom, p->selectRect.bottom * p->zoom);
                        TextOut(hdcMem, p->selectRect.left * p->zoom, p->selectRect.top * p->zoom, p->textBuf, p->textLen);
                        SelectObject(hdcMem, p->drawFont);
                    }
                }
                BitBlt(dc, 0, 0, rc.right, rc.bottom, hdcMem, p->showX, p->showY, SRCCOPY);
                SelectObject(hdcMem, oldbitmap);
                DeleteObject(hdcMem);
                DeleteObject(bitmap);
            }
            EndPaint(hwnd, &paint);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static void DrawSmallBox(HDC hDC, int x, int y)
{
    HPEN pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW));
    pen = SelectObject(hDC, pen);
    MoveToEx(hDC, x - 2, y - 2, NULL);
    LineTo(hDC, x - 2, y + 2);
    LineTo(hDC, x + 2, y + 2);
    LineTo(hDC, x + 2, y - 2);
    LineTo(hDC, x - 2, y - 2);
    pen = SelectObject(hDC, pen);
    DeleteObject(pen);
}
LRESULT CALLBACK WorkspaceWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    IMGDATA* p;
    RECT r;
    PAINTSTRUCT paint;
    HDC dc;
    POINT pt;
    switch (iMessage)
    {
        case WM_NCHITTEST:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            p->mouseInClient = FALSE;
            if (p->drawMode == IDM_ERASE)
                InvalidateRect(p->hwndDrawArea, 0, 0);
            SendMessage(p->hwndStatus, SB_SETTEXT, 1, (LPARAM) "");
            break;
        case WM_CREATE:
            p = (IMGDATA*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            {
                char buf[256];
                sprintf(buf, "%d x %d", p->res->hotspotX, p->res->hotspotY);
                SendMessage(p->hwndStatus, SB_SETTEXT, 2, (LPARAM)buf);
            }
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
            p->hwndDrawArea =
                CreateWindowEx(0, szDrawAreaClassName, "", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_BORDER, 0,
                               0, 16, 16, hwnd, 0, hInstance, p);
            break;
        case WM_SETFOCUS:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetFocus(p->hwndDrawArea);
            break;
        case WM_DESTROY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            DestroyWindow(p->hwndDrawArea);
            break;
        case WM_COMMAND:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            switch (LOWORD(wParam))
            {
                case IDM_RESIZEDRAW:
                {
                    RECT r;
                    GetClientRect(hwnd, &r);
                    SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(r.right, r.bottom));
                    InvalidateRect(hwnd, 0, 1);
                }
                break;
                case IDM_MI16161:
                case IDM_MI16164:
                case IDM_MI16168:
                case IDM_MI161624:

                case IDM_MI32321:
                case IDM_MI32324:
                case IDM_MI32328:
                case IDM_MI323224:
                case IDM_DELETE:
                    return SendMessage(p->hwndSelf, iMessage, wParam, lParam);
                default:
                    return SendMessage(p->hwndDrawArea, iMessage, wParam, lParam);
            }
            break;
        case WM_HSCROLL:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            switch (LOWORD(wParam))
            {
                case SB_BOTTOM:
                    p->showX = p->maxX;
                    break;
                case SB_TOP:
                    p->showX = 0;
                    break;
                case SB_LINELEFT:
                    if (p->showX)
                        p->showX -= p->zoom;
                    break;
                case SB_LINERIGHT:
                    if (p->showX < p->maxX)
                        p->showX += p->zoom;
                    break;
                case SB_PAGELEFT:
                    p->showX -= 16 * p->zoom;
                    if (p->showX < 0)
                        p->showX = 0;
                    break;
                case SB_PAGERIGHT:
                    p->showX += 16 * p->zoom;
                    if (p->showX > p->maxX)
                        p->showX = p->maxX;
                    break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    p->showX = HIWORD(wParam);
                    break;
                case SB_ENDSCROLL:
                    return 0;
            }
            SetScrollPos(hwnd, SB_HORZ, p->showX, TRUE);
            InvalidateRect(p->hwndDrawArea, 0, 0);
            break;
        case WM_VSCROLL:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            switch (LOWORD(wParam))
            {
                case SB_BOTTOM:
                    p->showY = p->maxY;
                    break;
                case SB_TOP:
                    p->showY = 0;
                    break;
                case SB_LINELEFT:
                    if (p->showY)
                        p->showY -= p->zoom;
                    break;
                case SB_LINERIGHT:
                    if (p->showY < p->maxY)
                        p->showY += p->zoom;
                    break;
                case SB_PAGELEFT:
                    p->showY -= 16 * p->zoom;
                    if (p->showY < 0)
                        p->showY = 0;
                    break;
                case SB_PAGERIGHT:
                    p->showY += 16 * p->zoom;
                    if (p->showY > p->maxY)
                        p->showY = p->maxY;
                    break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    p->showY = HIWORD(wParam);
                    break;
                case SB_ENDSCROLL:
                    return 0;
            }
            SetScrollPos(hwnd, SB_VERT, p->showY, TRUE);
            InvalidateRect(p->hwndDrawArea, 0, 0);
            break;
        case WM_PAINT:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            dc = BeginPaint(hwnd, &paint);
            GetClientRect(p->hwndDrawArea, &r);
            DrawSmallBox(dc, r.right + 4, (r.bottom + r.top) / 2);
            DrawSmallBox(dc, (r.right + r.left) / 2, r.bottom + 4);
            DrawSmallBox(dc, r.right + 4, r.bottom + 4);
            EndPaint(hwnd, &paint);
            break;
        case WM_SIZE:
        {
            int width, height;
            RECT r;
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            GetClientRect(hwnd, &r);
            if (r.right - 2 * GetSystemMetrics(SM_CXBORDER) < p->res->width * p->zoom)
                ShowScrollBar(hwnd, SB_HORZ, TRUE);
            else
                ShowScrollBar(hwnd, SB_HORZ, FALSE);
            if (r.bottom - 2 * GetSystemMetrics(SM_CYBORDER) < p->res->height * p->zoom)
                ShowScrollBar(hwnd, SB_VERT, TRUE);
            else
                ShowScrollBar(hwnd, SB_VERT, FALSE);
            GetClientRect(hwnd, &r);
            if (r.right - 2 * GetSystemMetrics(SM_CXBORDER) >= p->res->width * p->zoom)
            {
                width = p->res->width * p->zoom;
                p->showX = 0;
                p->maxX = 0;
            }
            else
            {
                width = r.right - 2 * GetSystemMetrics(SM_CXBORDER);
                SetScrollRange(hwnd, SB_HORZ, 0, p->maxX = (p->res->width * p->zoom - width), TRUE);
                //                p->showX = 0;
                SetScrollPos(hwnd, SB_HORZ, p->showX, FALSE);
                ShowScrollBar(hwnd, SB_HORZ, TRUE);
            }
            if (r.bottom - 2 * GetSystemMetrics(SM_CYBORDER) >= p->res->height * p->zoom)
            {

                height = p->res->height * p->zoom;
                ShowScrollBar(hwnd, SB_VERT, FALSE);
                p->showY = 0;
                p->maxY = 0;
            }
            else
            {
                height = r.bottom - 2 * GetSystemMetrics(SM_CYBORDER);
                SetScrollRange(hwnd, SB_VERT, 0, p->maxY = (p->res->height * p->zoom - height), TRUE);
                //                p->showY = 0;
                SetScrollPos(hwnd, SB_VERT, p->showY, FALSE);
                ShowScrollBar(hwnd, SB_VERT, TRUE);
            }
            MoveWindow(p->hwndDrawArea, 0, 0, width + 2 * GetSystemMetrics(SM_CXBORDER), height + 2 * GetSystemMetrics(SM_CYBORDER),
                       1);
        }
        break;
        case WM_LBUTTONUP:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (p->sizehorz || p->sizevert)
            {
                BOOL stretch = (GetKeyState(VK_CONTROL) & 0x80000000) != 0;
                int x, y;
                ReleaseCapture();
                pt.x = (int)(short)LOWORD(lParam);
                pt.y = (int)(short)HIWORD(lParam);
                if (pt.x < 0)
                    pt.x = 0;
                if (pt.y < 0)
                    pt.y = 0;
                if (p->sizehorz && p->sizevert)
                {
                    x = (pt.x - p->sizePos.x) / p->zoom + p->res->width;
                    y = (pt.y - p->sizePos.y) / p->zoom + p->res->height;
                }
                else if (p->sizehorz)
                {
                    x = (pt.x - p->sizePos.x) / p->zoom + p->res->width;
                    y = p->res->height;
                }
                else if (p->sizevert)
                {
                    x = p->res->width;
                    y = (pt.y - p->sizePos.y) / p->zoom + p->res->height;
                }
                p->sizehorz = p->sizevert = FALSE;
                HideBoundingRect();
                SendMessage(p->hwndStatus, SB_SETTEXT, 3, (LPARAM) "");
                if (x != p->res->width || y != p->res->height)
                {
                    ResizeImage(p->hwndDrawArea, p, p->res, x, y, stretch);
                    SendMessage(hwnd, WM_COMMAND, IDM_RESIZEDRAW, 0);
                    InvalidateRect(p->hwndDrawArea, 0, 0);
                }
            }
            break;
        case WM_LBUTTONDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            if (GetCursor() == diagcurs)
            {
                SetCapture(hwnd);
                p->sizePos.x = (int)(short)LOWORD(lParam);
                p->sizePos.y = (int)(short)HIWORD(lParam);
                printSize(p, p->sizePos.x, p->sizePos.y);
                p->sizehorz = TRUE;
                p->sizevert = TRUE;
                break;
            }
            else if (GetCursor() == hcurs)
            {
                SetCapture(hwnd);
                p->sizePos.x = (int)(short)LOWORD(lParam);
                p->sizePos.y = (int)(short)HIWORD(lParam);
                printSize(p, p->sizePos.x, p->sizePos.y);
                p->sizehorz = TRUE;
                break;
            }
            else if (GetCursor() == vcurs)
            {
                SetCapture(hwnd);
                p->sizePos.x = (int)(short)LOWORD(lParam);
                p->sizePos.y = (int)(short)HIWORD(lParam);
                printSize(p, p->sizePos.x, p->sizePos.y);
                p->sizevert = TRUE;
                break;
            }
            // fallthrough
        case WM_MBUTTONDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetFocus(p->hwndDrawArea);
            unselectDo(p);
            InvalidateRect(p->hwndDrawArea, NULL, FALSE);
            break;
        case WM_RBUTTONDOWN:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            {
                HMENU menu, popup;
                GetCursorPos(&pt);
                menu = LoadMenuGeneric(hInstance, "RESIMGMENU");
                popup = GetSubMenu(menu, 0);
                if (p->res->type == FT_BMP)
                {
                    EnableMenuItem(menu, IDM_INSERT, MF_GRAYED);
                    EnableMenuItem(menu, IDM_DELETE, MF_GRAYED);
                }
                else if (!p->resList->next)
                {
                    EnableMenuItem(menu, IDM_DELETE, MF_GRAYED);
                }
                if (!p->selected)
                {
                    EnableMenuItem(menu, IDM_FLIPMENU, MF_GRAYED);
                    EnableMenuItem(menu, IDM_ROTATEMENU, MF_GRAYED);
                    RemoveMenu(menu, IDM_CLEARSELECTION, MF_BYCOMMAND);
                }
                else
                {
                    RemoveMenu(menu, IDM_CLEAR, MF_BYCOMMAND);
                }
                InsertBitmapsInMenu(popup);
                TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, hwnd, NULL);
                DestroyMenu(menu);
            }
            break;
        case WM_MOUSEMOVE:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            pt.x = (int)(short)LOWORD(lParam);
            pt.y = (int)(short)HIWORD(lParam);
            if (pt.x < 0)
                pt.x = 0;
            if (pt.y < 0)
                pt.y = 0;
            if (p->sizehorz || p->sizevert)
            {
                if (p->sizehorz && p->sizevert)
                {
                    printSize(p, pt.x, pt.y);
                }
                else if (p->sizehorz)
                {
                    printSize(p, pt.x, p->sizePos.y);
                }
                else if (p->sizevert)
                {
                    printSize(p, p->sizePos.x, pt.y);
                }
            }
            else
            {
                GetClientRect(p->hwndDrawArea, &r);
                r.left = r.right;
                r.right += 6;
                r.top = r.bottom = (r.bottom + r.top) / 2;
                r.top -= 3;
                r.bottom += 3;
                if (PtInRect(&r, pt))
                {
                    SetCursor(hcurs);
                    break;
                }
                GetClientRect(p->hwndDrawArea, &r);
                r.left = r.right = (r.left + r.right) / 2;
                r.left -= 3;
                r.right += 3;
                r.top = r.bottom;
                r.bottom += 6;
                if (PtInRect(&r, pt))
                {
                    SetCursor(vcurs);
                    break;
                }
                GetClientRect(p->hwndDrawArea, &r);
                r.left = r.right;
                r.right += 6;
                r.top = r.bottom;
                r.bottom += 6;
                if (PtInRect(&r, pt))
                {
                    SetCursor(diagcurs);
                    break;
                }
                SetCursor(arrowCursor);
            }
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static void InsertTabs(IMGDATA* p, IMAGEDATA* res)
{
    char buf[80];
    if (res->next)
        InsertTabs(p, res->next);
    sprintf(buf, "%d x %d x %d", res->width, res->height, res->saveColors == 0 ? 24 : res->saveColors);
    SendMessage(p->hwndImageTab, TABM_ADD, (WPARAM)buf, (LPARAM)res);
}
static void ResetCursorList(IMGDATA* p)
{
    IMAGEDATA* ir = p->resList;
    CURSORDATA* cc = p->resource->resource->u.cursor->cursors;
    for (; ir; ir = ir->next, cc++)
    {
        ir->origData = cc;
    }
    SendMessage(p->hwndImageTab, TABM_REMOVEALL, 0, 0);
    InsertTabs(p, p->resList);
}
static void ResetIconList(IMGDATA* p)
{
    IMAGEDATA* ir = p->resList;
    ICONDATA* ic = p->resource->resource->u.icon->icons;
    for (; ir; ir = ir->next, ic++)
    {
        ir->origData = ic;
    }
    SendMessage(p->hwndImageTab, TABM_REMOVEALL, 0, 0);
    InsertTabs(p, p->resList);
}
static void LinkRes(IMGDATA* p, int w, int h, int c)
{
    ResGetHeap(workArea, p->resource);
    if (p->res->type == FT_ICO)
    {
        ICONDATA* ic = rcAlloc(sizeof(ICONDATA) * (p->resource->resource->u.icon->count + 1));
        memcpy(ic + 1, p->resource->resource->u.icon->icons, p->resource->resource->u.icon->count * sizeof(ICONDATA));
        rcFree(p->resource->resource->u.icon->icons);
        p->resource->resource->u.icon->icons = ic;
        p->resource->resource->u.icon->count++;
        p->res = newRes(p->hwndDrawArea, p->res->type, w, h, c);
        p->res->next = p->resList;
        p->resList = p->res;
        p->resource->gd.images = p->resList;
        ic->bytes = 0;
        ic->data = 0;
        ic->colorcount = c;
        ic->width = w;
        ic->height = h;
        ResetIconList(p);
    }
    else
    {
        CURSORDATA* cc = rcAlloc(sizeof(CURSORDATA) * (p->resource->resource->u.cursor->count + 1));
        memcpy(cc + 1, p->resource->resource->u.cursor->cursors, p->resource->resource->u.cursor->count * sizeof(CURSORDATA));
        rcFree(p->resource->resource->u.cursor->cursors);
        p->resource->resource->u.cursor->cursors = cc;
        p->resource->resource->u.cursor->count++;
        p->res = newRes(p->hwndDrawArea, p->res->type, w, h, c);
        p->res->next = p->resList;
        p->resList = p->res;
        p->resource->gd.images = p->resList;
        cc->bytes = 0;
        cc->data = 0;
        cc->colorcount = c;
        cc->width = w;
        cc->height = h;
        ResetCursorList(p);
    }
    p->res->imageDirty = TRUE;
    p->resource->gd.cantClearUndo = TRUE;
    ResSetDirty(p->resource);

    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
    InvalidateRect(p->hwndDrawArea, 0, 0);
}
static void UnlinkRes(IMGDATA* p)
{
    IMAGEDATA **ir = &p->resList, *irp = p->res;
    int count = 0;
    ResGetHeap(workArea, p->resource);
    while (*ir && (*ir) != irp)
    {
        count++;
        ir = &(*ir)->next;
    }
    *ir = (*ir)->next;
    FreeImageUndo(irp);
    DeleteImageDC(irp);
    free(irp);
    p->resource->gd.images = p->resList;
    p->res = p->resList;
    if (p->res->type == FT_ICO)
    {
        ICONDATA* ic = p->resource->resource->u.icon->icons;
        memcpy(ic + count, ic + count + 1, (p->resource->resource->u.icon->count - count - 1) * sizeof(ICONDATA));
        p->resource->resource->u.icon->count--;
        ResetIconList(p);
    }
    else
    {
        CURSORDATA* cc = p->resource->resource->u.cursor->cursors;
        memcpy(cc + count, cc + count + 1, (p->resource->resource->u.cursor->count - count - 1) * sizeof(CURSORDATA));
        p->resource->resource->u.cursor->count--;
        ResetCursorList(p);
    }
    p->resource->gd.cantClearUndo = TRUE;
    ResSetDirty(p->resource);
    SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
    InvalidateRect(p->hwndDrawArea, 0, 0);
}
static int CALLBACK fontEnumProc(const ENUMLOGFONTEX* lpelf, const NEWTEXTMETRICEX* lpntm, int FontType, LPARAM lParam)
{
    IMGDATA* p = (IMGDATA*)lParam;
    if (lpelf->elfFullName[0] != '@')
    {
        int id = SendMessage(p->hwndFontCombo, CB_ADDSTRING, 0, (LPARAM)lpelf->elfFullName);
        LOGFONT* x = calloc(1, sizeof(LOGFONT));
        memcpy(x, &lpelf->elfLogFont, sizeof(LOGFONT));
        SendMessage(p->hwndFontCombo, CB_SETITEMDATA, id, (LPARAM)x);
    }
    return 1;
}
static void PopulateFontCombos(IMGDATA* p)
{
    LOGFONT ft;
    int ptsz[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
    int i;
    for (i = 0; i < sizeof(ptsz) / sizeof(ptsz[0]); i++)
    {
        char bf[256];
        sprintf(bf, "%d", ptsz[i]);
        SendMessage(p->hwndPointSizeCombo, CB_ADDSTRING, 0, (LPARAM)bf);
    }
    SendMessage(p->hwndPointSizeCombo, CB_SETCURSEL, 3, 0);
    memset(&ft, 0, sizeof(ft));
    ft.lfCharSet = ANSI_CHARSET;
    EnumFontFamiliesEx(p->res->hdcImage, &ft, (FONTENUMPROC)fontEnumProc, (LPARAM)p, 0);
    SendMessage(p->hwndFontCombo, CB_SELECTSTRING, 0, (LPARAM) "Courier New");
}
LRESULT CALLBACK ImageWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    HDC dc;
    RECT rf, rs;
    int parts[4], i;
    IMGDATA* p;
    switch (iMessage)
    {
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == TABN_SELECTED)
            {
                LSTABNOTIFY* x = (LSTABNOTIFY*)lParam;
                IMAGEDATA* res = (IMAGEDATA*)x->lParam;
                p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                unselectDo(p);
                p->res = res;
                SendMessage(p->hwndWorkspace, WM_COMMAND, IDM_RESIZEDRAW, 0);
                InvalidateRect(p->hwndDrawArea, 0, 0);
            }
            break;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_SHOWFONTTB:
                    GetClientRect(hwnd, &rf);
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    p->showFontTB = lParam;
                    SetWindowPos(p->hwndFontTB, 0, 0, 0, 0, 0,
                                 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE |
                                     (lParam ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
                    SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rf.right, rf.bottom));
                    break;

                case IDM_EXIT:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    //					removeRes(p);
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_MI16161:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 16, 16, 2);
                    break;
                case IDM_MI16164:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 16, 16, 16);
                    break;
                case IDM_MI16168:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 16, 16, 256);
                    break;
                case IDM_MI161624:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 16, 16, 0);
                    break;

                case IDM_MI32321:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 32, 32, 2);
                    break;
                case IDM_MI32324:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 32, 32, 16);
                    break;
                case IDM_MI32328:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 32, 32, 256);
                    break;
                case IDM_MI323224:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    LinkRes(p, 32, 32, 0);
                    break;
                case IDM_DELETE:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    UnlinkRes(p);
                    break;
                default:
                    p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                    SendMessage(p->hwndDrawArea, iMessage, wParam, lParam);
                    break;
            }
            break;
        case EM_CANUNDO:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            return p->res->undo != NULL;
        case WM_SETFOCUS:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            SetFocus(p->hwndWorkspace);
            break;
        case WM_MEASUREITEM:
        {
            // Set the height of the items in the food groups combo box.
            LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;

            lpmis->itemHeight = 16;

            break;
        }
        case WM_DRAWITEM:
        {
            char achTemp[256];
            COLORREF clrBackground;
            COLORREF clrForeground;
            TEXTMETRIC tm;
            int x;
            int y;
            size_t cch;
            LOGFONT* lf;
            HFONT font;

            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

            if (lpdis->itemID == -1)  // Empty item)
                break;

            lf = (LOGFONT*)lpdis->itemData;
            lf->lfHeight = -12;
            lf->lfWidth = 0;
            font = CreateFontIndirect(lf);
            font = SelectObject(lpdis->hDC, font);

            // The colors depend on whether the item is selected.
            clrForeground =
                SetTextColor(lpdis->hDC, GetSysColor(lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

            clrBackground = SetBkColor(lpdis->hDC, GetSysColor(lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT : COLOR_WINDOW));

            // Calculate the vertical and horizontal position.
            GetTextMetrics(lpdis->hDC, &tm);
            y = (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2;
            x = LOWORD(GetDialogBaseUnits()) / 4;

            // Get and display the text for the list item.
            SendMessage(lpdis->hwndItem, CB_GETLBTEXT, lpdis->itemID, (LPARAM)achTemp);
            cch = strlen(achTemp);

            ExtTextOut(lpdis->hDC, 2 * x, y, ETO_CLIPPED | ETO_OPAQUE, &lpdis->rcItem, achTemp, (UINT)cch, NULL);

            // Restore the previous colors.
            SetTextColor(lpdis->hDC, clrForeground);
            SetBkColor(lpdis->hDC, clrBackground);

            // If the item has the focus, draw the focus rectangle.
            if (lpdis->itemState & ODS_FOCUS)
                DrawFocusRect(lpdis->hDC, &lpdis->rcItem);
            font = SelectObject(lpdis->hDC, font);
            DeleteObject(font);
            break;
        }

        case WM_CREATE:
            p = GlobalAlloc(GPTR, sizeof(IMGDATA));
            p->resource = (struct resRes*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            p->hwndSelf = hwnd;
            p->screenColor = RGB_DEFAULT_BACKGROUND;
            p->currentLeftColor = RGB_BLACK;
            p->currentRightColor = RGB_WHITE;
            p->zoom = 1;
            p->drawMode = 0;
            p->eraseWidth = 3;
            p->fillType = FT_FILL;
            p->brushType = IDM_BRUSH_ROUND_4;
            //				p->res = newRes(FT_BMP, 256,256, 0); /* 24 bit color */
            p->res = p->resList = p->resource->gd.images;
            p->cbImgData = sizeof(IMGDATA) - sizeof(SHORT);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)p);
            p->hwndStatus = CreateStatusWindow(WS_VISIBLE | WS_CHILD, "", hwnd, ID_IMG_STATUS_WINDOW);
            SetWindowLong(p->hwndStatus, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE));
            GetClientRect(hwnd, &rf);
            parts[0] = rf.right - 400;
            parts[1] = rf.right - 300;
            parts[2] = rf.right - 200;
            parts[3] = rf.right - 200;
            SendMessage(p->hwndStatus, SB_SETPARTS, 4, (LPARAM)&parts[0]);
            {
                p->hwndImageTab = CreateLsTabWindow(hwnd, TABS_BOTTOM | TABS_HOTTRACK | TABS_FLAT | WS_VISIBLE);
                ApplyDialogFont(p->hwndImageTab);
                InsertTabs(p, p->resList);
            }
            p->hwndFontTB = CreateToolBarWindow(-1, hwnd, hwnd, IDB_FONTTOOLBAR, 5, fontButtons, fontHints, "Font Tools",
                                                IDB_FONTTOOLBAR, FALSE);
            SendMessage(p->hwndFontTB, LCF_FLOATINGTOOL, 0, 0);
            //            p->hwndFontTB = CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPSIBLINGS | TBSTYLE_TRANSPARENT | CCS_NODIVIDER,
            //                ID_FONTTOOLBAR, 3, hInstance, (UINT)fontToolBitmap,
            //                &fontButtons, 5, 16, 16, 16, 16, sizeof(TBBUTTON));
            p->hwndFontCombo = CreateWindowEx(0, "COMBOBOX", "",
                                              WS_CHILD + WS_BORDER + WS_VISIBLE + WS_TABSTOP + WS_VSCROLL + CBS_DROPDOWN +
                                                      CBS_AUTOHSCROLL + CBS_SORT |
                                                  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
                                              0, 0, 100, 200, hwnd, (HMENU)ID_FONT, hInstance, 0);
            SetParent(p->hwndFontCombo, p->hwndFontTB);
            SendMessage(p->hwndFontTB, LCF_ADDCONTROL, 0, (LPARAM)p->hwndFontCombo);
            p->hwndPointSizeCombo =
                CreateWindowEx(0, "COMBOBOX", "", WS_CHILD + WS_BORDER + WS_VISIBLE + WS_TABSTOP + CBS_DROPDOWN + CBS_AUTOHSCROLL,
                               0, 0, 100, 200, p->hwndFontTB, (HMENU)ID_POINTSIZE, hInstance, 0);
            SendMessage(p->hwndFontTB, LCF_ADDCONTROL, 1, (LPARAM)p->hwndPointSizeCombo);
            PopulateFontCombos(p);
            p->hwndControl = CreateWindowEx(0, szControlClassName, "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, 0, hInstance, p);
            p->hwndAux =
                CreateWindowEx(WS_EX_CLIENTEDGE, szAuxClassName, "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, 0, hInstance, p);
            p->hwndColor = CreateWindowEx(0, szColorClassName, "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, 0, hInstance, p);
            p->hwndWorkspace = CreateWindowEx(WS_EX_CLIENTEDGE, szWorkspaceClassName, "",
                                              WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT,
                                              CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, 0, hInstance, p);
            //            SendMessage(hwndStatus, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)
            //                "    ");
            SetFocus(hwnd);
            if (!bmpUp)
            {
                HDC dc = GetDC(hwnd);
                HDC dcd = CreateCompatibleDC(dc);
                ReleaseDC(hwnd, dc);
                bmpUp = CreateCompatibleBitmap(dcd, 8, 8);
                bmpDown = CreateCompatibleBitmap(dcd, 8, 8);
                bmpUp = SelectObject(dcd, bmpUp);
                PatBlt(dcd, 0, 0, 8, 8, WHITENESS);
                MoveToEx(dcd, 0, 7, NULL);
                LineTo(dcd, 7, 0);
                bmpUp = SelectObject(dcd, bmpUp);
                bmpDown = SelectObject(dcd, bmpDown);
                PatBlt(dcd, 0, 0, 8, 8, WHITENESS);
                MoveToEx(dcd, 7, 0, NULL);
                LineTo(dcd, 0, 7);
                bmpDown = SelectObject(dcd, bmpDown);
                DeleteDC(dcd);
            }
            break;
        case WM_DESTROY:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            i = SendMessage(p->hwndFontCombo, CB_GETCOUNT, 0, 0) - 1;
            while (i >= 0)
            {
                void* x = (void*)SendMessage(p->hwndFontCombo, CB_GETITEMDATA, i, 0);
                free(x);
                i--;
            }
            DestroyWindow(p->hwndControl);
            DestroyWindow(p->hwndColor);
            DestroyWindow(p->hwndWorkspace);
            DestroyWindow(p->hwndStatus);
            GlobalFree(p);
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            break;
        case WM_SIZE:
            p = (IMGDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            rf.left = rf.top = 0;
            rf.right = LOWORD(lParam);
            rf.bottom = HIWORD(lParam);
            GetWindowRect(p->hwndStatus, &rs);
            rf.top = rf.bottom - rs.bottom + rs.top;
            MoveWindow(p->hwndStatus, rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 1);

            parts[0] = rf.right - 400;
            parts[1] = rf.right - 300;
            parts[2] = rf.right - 200;
            parts[3] = rf.right - 100;
            SendMessage(p->hwndStatus, SB_SETPARTS, 4, (LPARAM)&parts[0]);
            //            SendMessage(hwndStatus, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)
            //                "    ");

            rf.left = p->controlTbSize.right + 3 * GetSystemMetrics(SM_CXDLGFRAME);
            rf.bottom = rf.top;
            rf.top = rf.bottom - (p->colorTbSize.bottom - p->colorTbSize.top + 4);
            MoveWindow(p->hwndColor, rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 1);

            if (p->res->type != FT_BMP)
            {
                rf.bottom = rf.top;
                rf.top = rf.bottom - 30;
                rf.left = p->controlTbSize.right + 3 * GetSystemMetrics(SM_CXDLGFRAME);
                rf.right = LOWORD(lParam);
                MoveWindow(p->hwndImageTab, rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 1);
            }
            rf.bottom = rf.top;
            if (p->showFontTB)
                rf.top = 24;
            else
                rf.top = 0;
            rf.left = p->controlTbSize.right + 3 * GetSystemMetrics(SM_CXDLGFRAME);
            rf.right = LOWORD(lParam);
            MoveWindow(p->hwndWorkspace, rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 1);

            if (p->showFontTB)
            {
                rf.top = 0;
                rf.bottom = 24;
                rf.left = p->controlTbSize.right + 3 * GetSystemMetrics(SM_CXDLGFRAME);
                rf.right = LOWORD(lParam);
                MoveWindow(p->hwndFontTB, rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 1);
            }
            rf.top = 3 * GetSystemMetrics(SM_CYDLGFRAME);
            rf.bottom = rf.top + p->controlTbSize.bottom;
            rf.left = 0;
            rf.right = p->controlTbSize.right + 3 * GetSystemMetrics(SM_CYDLGFRAME);
            MoveWindow(p->hwndControl, rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 1);

            rf.top = rf.bottom + 2 * GetSystemMetrics(SM_CYDLGFRAME);
            rf.bottom = rf.top + 80;
            rf.right = p->controlTbSize.right + GetSystemMetrics(SM_CYDLGFRAME);
            MoveWindow(p->hwndAux, rf.left + GetSystemMetrics(SM_CYDLGFRAME), rf.top,
                       rf.right - rf.left - 2 * GetSystemMetrics(SM_CYDLGFRAME), rf.bottom - rf.top, 1);

            return 0;
        case WM_INITMENUPOPUP:
#ifdef XXXXX
            SetMenuCheckedState(DID_BUILDTOOL, IDM_VIEWBUILDBAR);

            mf_state = numberofdrawwindows ? MF_ENABLED : MF_GRAYED;
            EnableMenuItem(hMenuMain, IDM_SAVEALL, mf_state);
#endif
            return 0;

        case WM_CLOSE:
            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

void RegisterImageWindows(HINSTANCE hInstance)
{
    WNDCLASS wc;

    drawToolBitmap = LoadBitmap(hInstance, (LPSTR)IDB_CONTROLTOOLBAR);
    ChangeBitmapColor(drawToolBitmap, RGB(255, 0, 0), GetSysColor(COLOR_3DFACE));

    brushToolBitmap = LoadImage(hInstance, (LPSTR)IDB_BRUSHTOOLBAR, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
    fillToolBitmap = LoadImage(hInstance, (LPSTR)IDB_FILLTOOLBAR, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
    lineToolBitmap = LoadImage(hInstance, (LPSTR)IDB_LINETOOLBAR, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
    zoomToolBitmap = LoadImage(hInstance, (LPSTR)IDB_ZOOMTOOLBAR, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
    eraseToolBitmap = LoadImage(hInstance, (LPSTR)IDB_ERASETOOLBAR, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
    fontToolBitmap = LoadImage(hInstance, (LPSTR)IDB_FONTTOOLBAR, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);

    blankCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1001));
    brushCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1002));
    hotspotCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1003));
    magnifyCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1004));
    moveCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1005));
    penCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1006));
    pickCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1007));
    fillCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1008));
    objectCursor = LoadCursor(hInstance, MAKEINTRESOURCE(1009));
    arrowCursor = LoadCursor(0, IDC_ARROW);

    hcurs = LoadCursor(0, IDC_SIZEWE);
    vcurs = LoadCursor(0, IDC_SIZENS);
    diagcurs = LoadCursor(0, IDC_SIZENWSE);

    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &ImageWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;  // LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    wc.lpszMenuName = 0;
    wc.lpszClassName = szimageClassName;
    RegisterClass(&wc);

    wc.lpfnWndProc = ControlWndProc;
    wc.lpszClassName = szControlClassName;
    RegisterClass(&wc);

    wc.lpfnWndProc = AuxWndProc;
    wc.lpszClassName = szAuxClassName;
    RegisterClass(&wc);

    wc.lpfnWndProc = ColorWndProc;
    wc.lpszClassName = szColorClassName;
    RegisterClass(&wc);

    wc.lpfnWndProc = Color2WndProc;
    wc.lpszClassName = szColor2ClassName;
    RegisterClass(&wc);

    wc.lpfnWndProc = ScreenColorWndProc;
    wc.lpszClassName = szScreenColorClassName;
    RegisterClass(&wc);

    wc.hCursor = NULL;
    wc.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_APPWORKSPACE));
    wc.lpfnWndProc = WorkspaceWndProc;
    wc.lpszClassName = szWorkspaceClassName;
    RegisterClass(&wc);

    wc.style = CS_DBLCLKS;
    wc.hCursor = 0;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpfnWndProc = DrawAreaWndProc;
    wc.lpszClassName = szDrawAreaClassName;
    RegisterClass(&wc);
}
