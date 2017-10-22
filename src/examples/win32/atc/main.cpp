#define NOMINMAX
#include <windows.h>
#include "Resource.h"
#include "DisplayObjects.h"
#include <stdlib.h>
#include <time.h>
static char *szClassName = "atcWndClass";

static HINSTANCE hInstance;

LRESULT CALLBACK SetTimeProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                {
                    char buf[256];
                    GetDlgItemText(hwnd, IDC_SELECTTIME, buf, 256);
                    EndDialog(hwnd, atoi(buf));
                    break;
                }
                case IDCANCEL: 
                    EndDialog(hwnd, 0);
                    break;
            }
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static Board *board;
    static int selectedPlaneIndex = -1;
    switch (iMessage)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc;
            dc = BeginPaint(hwnd, &ps);
            if (board)
                board->Draw(dc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_RBUTTONDOWN:
            if (board)
            {
                int n;
                POINT pt;
                GetCursorPos(&pt);
                MapWindowPoints(HWND_DESKTOP, hwnd, &pt, 1);
                selectedPlaneIndex = board->GetPlaneIndex(pt.x, pt.y);
                if (selectedPlaneIndex >= 0)
                {
                    HMENU menu = LoadMenu(hInstance, MAKEINTRESOURCE(IDM_COMMAND_MENU));
                    HMENU popup = GetSubMenu(menu, 0);
                    POINT pos;
                    MENUITEMINFO mi;
                    std::string str = board->GetDisplayString(selectedPlaneIndex);
                    memset(&mi, 0, sizeof(mi));
                    mi.cbSize = sizeof(mi);
                    mi.fMask = MIIM_TYPE | MIIM_DATA;
                    mi.fType = MFT_STRING;
                    mi.dwTypeData = (LPSTR)str.c_str();
                    InsertMenuItem(popup, 0, TRUE, &mi);
                    GetCursorPos(&pos);
                    TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTBUTTON, pos.x,
                        pos.y, hwnd, NULL);
                    DestroyMenu(menu);
                }
            }
            break;
        case WM_TIMER:
            if (board)
            {
                std::string st = board->Timed();
                if (st != "")
                {
                    delete board;
                    board = NULL;
                    KillTimer(hwnd, 100);
                    MessageBox(0,st.c_str(), "Message", 0);
                }
                InvalidateRect(hwnd, 0, 1);
            }
            break;
        case WM_CREATE:
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_CCW_1:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateLeft, 1);
                    break;
                case IDM_CCW_2:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateLeft, 2);
                    break;
                case IDM_CCW_3:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateLeft, 3);
                    break;
                case IDM_CCW_4:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateLeft, 4);
                    break;
                case IDM_CW_1:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateRight, 1);
                    break;
                case IDM_CW_2:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateRight, 2);
                    break;
                case IDM_CW_3:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateRight, 3);
                    break;
                case IDM_CW_4:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateRight, 4);
                    break;
                case IDM_Stop_Rotate:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, RotateRight, 0);
                    break;
                case IDM_Circle_Navaid:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, CircleNavaid, 0);
                    break;
                case IDM_Cleared_To_Land:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ClearToLand, 0);
                    break;
                case IDM_A0:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 0);
                    break;
                case IDM_A1:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 1);
                    break;
                case IDM_A2:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 2);
                    break;
                case IDM_A3:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 3);
                    break;
                case IDM_A4:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 4);
                    break;
                case IDM_A5:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 5);
                    break;
                case IDM_A6:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 6);
                    break;
                case IDM_A7:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 7);
                    break;
                case IDM_A8:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 8);
                    break;
                case IDM_A9:
                    if (board)
                        board->CommandPlane(selectedPlaneIndex, ChangeAlt, 9);
                    break;
                case IDM_Exit:
                    PostQuitMessage(0);
                    break;
                case IDM_Start:
                {
                    if (board)
                    {
                        delete board;
                        board = NULL;
                    }
                    int n = DialogBox(hInstance, MAKEINTRESOURCE(IDD_SELECTTIME), hwnd, (DLGPROC)SetTimeProc);
                    if (n >= 16 && n <= 100)
                    {
                        board = new Board(n-16);
                        SetTimer(hwnd, 100, 1000, 0);
                        InvalidateRect(hwnd, 0, 1);
                    }
                    else
                    {
                        MessageBox(0, "Time value must be between 16 and 100 minutes", "Error", 0);
                    }
                    break;
                }
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
    int nCmdShow)
{
    HMENU hMenuMain;
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;
    hInstance =hInst;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1) ;
    wc.lpszMenuName = 0;
    wc.lpszClassName = szClassName;
    RegisterClass(&wc);

    srand(time(0));
    hMenuMain = LoadMenu(hInst, MAKEINTRESOURCE(MAINMENU));

    hwnd = CreateWindowEx(0, szClassName, "Air Traffic Controller",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT , CW_USEDEFAULT,
        0, hMenuMain, hInst, 0);
    UpdateWindow(hwnd);
    ShowWindow(hwnd, SW_SHOW);

    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}