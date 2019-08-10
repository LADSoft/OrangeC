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

/* DESCRIPTION    : Functions for splash screen                               */
/* LANGUAGE       : "C"                                                       */
/* PLATFORM       : Windows 95/98/98SE/ME/NT4/2000                            */
/* AUTHOR         : Andrea Binello  <andrea.binello@tiscalinet.it>            */
/* LICENSE        : Freeware - Open Source                                    */
/* LAST UPDATE    : May 06, 2001                                              */
/******************************************************************************/

/* DAL - modified extensively to be an about box, to draw a version number, etc... */

#define STRICT
#include <windows.h>
#include <process.h>
#include <richedit.h>
#include "splash.h"
#include <commctrl.h>
#include <stdio.h>
#include "header.h"

#define SPLASH_CLASS "AbSplashScreenWndClass"

#define ID_BUTTON 100
#define ID_SHOW 101

typedef struct
{
    LPBYTE lpBuffer;
    INT iWidth;
    INT iHeight;
    LPSPLASH lpSplash;
    HWND hButton;
    DWORD uTimerId;
} SPLASH2, FAR* LPSPLASH2;

unsigned char* InflateWrap(unsigned char* data);

/*****************************************************************************/
static unsigned char* SplashScreen_LoadBmp(HINSTANCE hInstance, LPSTR lpszResource, LPINT lpiWidth, LPINT lpiHeight)
{
    DWORD dwLen;
    HRSRC hResource;
    HGLOBAL hGlobal;
    HGLOBAL hGlobalRet = NULL;
    LPVOID lpBuffer = NULL;
    LPBITMAPFILEHEADER lpbfh;
    LPBITMAPINFOHEADER lpbih;
    hResource = FindResource(hInstance, lpszResource, "SPLASH");

    if (hResource != NULL)
    {
        dwLen = SizeofResource(hInstance, hResource);

        if (dwLen != 0)
        {
            hGlobal = LoadResource(hInstance, hResource);

            if (hGlobal != NULL)
            {
                lpBuffer = LockResource(hGlobal);
                // The next line unzips the first item in a zip file,
                // and assumes it is a bitmap.  Comment out if the SPLASH
                // resource is a plain bitmap
                lpBuffer = InflateWrap(lpBuffer);

                if (lpBuffer != NULL)
                {
                    lpbfh = (LPBITMAPFILEHEADER)lpBuffer;
                    lpbih = (LPBITMAPINFOHEADER)(((LPBYTE)lpBuffer) + sizeof(BITMAPFILEHEADER));

                    if (lpbfh->bfType == 0x4D42)
                    {
                        hGlobalRet = hGlobal;

                        *lpiWidth = (INT)lpbih->biWidth >= 0 ? lpbih->biWidth : -lpbih->biWidth;
                        *lpiHeight = (INT)lpbih->biHeight >= 0 ? lpbih->biHeight : -lpbih->biHeight;
                    }
                }
            }
        }
    }

    return lpBuffer;
}

/*****************************************************************************/
static VOID SplashScreen_Paint(HWND hWnd)
{
    char buf[256];
    LPSPLASH2 lpSplash2;
    HDC hdc;
    PAINTSTRUCT ps;
    LPBITMAPINFOHEADER lpbih;
    LPVOID lpbits = NULL;
    SIZE size;
    lpSplash2 = (LPSPLASH2)GetWindowLong(hWnd, GWL_USERDATA);

    hdc = BeginPaint(hWnd, &ps);

    if (lpSplash2 != NULL && lpSplash2->lpBuffer != NULL)
    {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, lpSplash2->iWidth, lpSplash2->iHeight);
        lpbih = (LPBITMAPINFOHEADER)(lpSplash2->lpBuffer + sizeof(BITMAPFILEHEADER));
        if (lpbih->biBitCount == 24)
            lpbits = ((LPBYTE)lpbih) + lpbih->biSize;
        else if (lpbih->biBitCount == 8)
            lpbits = ((LPBYTE)lpbih) + lpbih->biSize + sizeof(RGBQUAD) * 256;
        else if (lpbih->biBitCount == 4)
            lpbits = ((LPBYTE)lpbih) + lpbih->biSize + sizeof(RGBQUAD) * 16;
        else if (lpbih->biBitCount == 1)
            lpbits = ((LPBYTE)lpbih) + lpbih->biSize + sizeof(RGBQUAD) * 2;
        SetDIBits(hdcMem, bmp, 0, lpSplash2->iHeight, lpbits, (BITMAPINFO*)lpbih, DIB_RGB_COLORS);
        SelectObject(hdcMem, bmp);
        BitBlt(hdc, 0, 0, lpSplash2->iWidth, lpSplash2->iHeight, hdcMem, 0, 0, SRCCOPY);
        DeleteObject(hdcMem);
    }
    if (lpSplash2->lpSplash->lpszVersion)
    {
        int n;
        strcpy(buf, lpSplash2->lpSplash->lpszVersion);
        GetTextExtentPoint32(hdc, buf, strlen(buf), &size);
        SetBkMode(hdc, TRANSPARENT);
        n = SetTextColor(hdc, 0x0000ff);
        TextOut(hdc, lpSplash2->iWidth - size.cx - 4, lpSplash2->iHeight - size.cy - 4, buf, strlen(buf));
        SetTextColor(hdc, n);
    }

    EndPaint(hWnd, &ps);
}

/*****************************************************************************/
static LRESULT CALLBACK SplashScreen_Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPSPLASH2 lpSplash2;
    switch (msg)
    {
        case WM_PAINT:
            SplashScreen_Paint(hWnd);
            return 0L;

        case WM_ERASEBKGND:
            return 1L;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            lpSplash2 = (LPSPLASH2)GetWindowLong(hWnd, GWL_USERDATA);
            if (!lpSplash2->uTimerId)
                break;
            // Fall through
        case WM_TIMER:
            lpSplash2 = (LPSPLASH2)GetWindowLong(hWnd, GWL_USERDATA);
            if (lpSplash2->uTimerId)
                KillTimer(hWnd, lpSplash2->uTimerId);
            SendMessage(hWnd, WM_CLOSE, 0, 0L);
            return 0L;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BUTTON)
            {
                lpSplash2 = (LPSPLASH2)GetWindowLong(hWnd, GWL_USERDATA);
                if (lpSplash2->uTimerId)
                    KillTimer(hWnd, lpSplash2->uTimerId);
                SendMessage(hWnd, WM_CLOSE, 0, 0L);
            }
            else if (LOWORD(wParam) == ID_SHOW)
            {
                lpSplash2 = (LPSPLASH2)GetWindowLong(hWnd, GWL_USERDATA);
                if (lpSplash2->lpSplash->bAbout)
                {
                    lpSplash2->hButton = CreateWindowEx(0, "button", lpSplash2->lpSplash->lpszButtonTitle, WS_VISIBLE | WS_CHILD,
                                                        lpSplash2->lpSplash->bPos.left, lpSplash2->lpSplash->bPos.top,
                                                        lpSplash2->lpSplash->bPos.right - lpSplash2->lpSplash->bPos.left,
                                                        lpSplash2->lpSplash->bPos.bottom - lpSplash2->lpSplash->bPos.top, hWnd,
                                                        (HMENU)100, lpSplash2->lpSplash->hInstance, NULL);
                    lpSplash2->uTimerId = 0;
                    ShowWindow(hWnd, SW_SHOW);
                }
                else
                {
                    lpSplash2->hButton = 0;
                    lpSplash2->uTimerId = SetTimer(hWnd, 1, (UINT)lpSplash2->lpSplash->uTime, NULL);

                    if (lpSplash2->uTimerId != 0)
                        ShowWindow(hWnd, SW_SHOW);
                    else
                        PostMessage(hWnd, WM_CLOSE, 0, 0L);
                }
            }

            break;

        case WM_DESTROY:
            lpSplash2 = (LPSPLASH2)GetWindowLong(hWnd, GWL_USERDATA);
            if (lpSplash2->hButton)
                DestroyWindow(lpSplash2->hButton);
            free(lpSplash2->lpBuffer);
            GlobalFree(lpSplash2);
            PostQuitMessage(0);
            return 0L;
    }

    return (DefWindowProc(hWnd, msg, wParam, lParam));
}

/******************************************************************************/
static unsigned __stdcall SplashScreen_Thread(void* in)
{
    LPSPLASH lpSplash = (LPSPLASH)in;
    LPBYTE lpBuffer;
    WNDCLASSEX wcx;
    BOOL bReg;
    DWORD dwExStyles = 0;
    DWORD dwStyles;
    INT iWidth, iHeight, iScrWidth, iScrHeight, iPosX, iPosY;
    LPSPLASH2 lpSplash2;
    HWND hWnd;
    MSG msg;
    {
        lpBuffer = SplashScreen_LoadBmp(lpSplash->hInstanceRes, lpSplash->lpszResource, &iWidth, &iHeight);

        if (lpBuffer != NULL)
        {
            INT wWidth = iWidth + GetSystemMetrics(SM_CXDLGFRAME) * 2;
            INT wHeight = iHeight + GetSystemMetrics(SM_CYDLGFRAME) * 2;

            //            if (lpSplash->bAbout)
            //                wHeight += GetSystemMetrics(SM_CYCAPTION) ;

            bReg = FALSE;

            if (GetClassInfoEx(lpSplash->hInstance, SPLASH_CLASS, &wcx))
                bReg = TRUE;
            else
            {
                memset(&wcx, 0, sizeof(wcx));
                wcx.cbSize = sizeof(wcx);
                wcx.style = 0;
                wcx.lpfnWndProc = (WNDPROC)SplashScreen_Proc;
                wcx.cbClsExtra = 0;
                wcx.cbWndExtra = 0;
                wcx.hInstance = lpSplash->hInstance;
                wcx.hIcon = NULL;
                wcx.hIconSm = NULL;
                wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
                wcx.hbrBackground = NULL;
                wcx.lpszMenuName = NULL;
                wcx.lpszClassName = SPLASH_CLASS;

                if (RegisterClassEx(&wcx))
                    bReg = TRUE;
            }

            if (bReg)
            {
                if (lpSplash->bCentered)
                {
                    iScrWidth = GetSystemMetrics(SM_CXSCREEN);
                    iScrHeight = GetSystemMetrics(SM_CYSCREEN);

                    iPosX = (iScrWidth - iWidth) / 2;
                    iPosY = (iScrHeight - iHeight) / 2;
                }
                else
                {
                    iPosX = lpSplash->iPosX - iWidth / 2;
                    iPosY = lpSplash->iPosY - iHeight / 2;
                }

                //               dwExStyles = WS_EX_TOOLWINDOW;

                if (lpSplash->bTopmost)
                    dwExStyles |= WS_EX_TOPMOST;

                dwStyles = WS_POPUP | WS_DLGFRAME;

                lpSplash2 = GlobalAlloc(GPTR, sizeof(SPLASH2));
                if (lpSplash2 != NULL)
                {
                    lpSplash2->lpBuffer = lpBuffer;
                    lpSplash2->iWidth = iWidth;
                    lpSplash2->iHeight = iHeight;
                    lpSplash2->lpSplash = lpSplash;

                    hWnd = CreateWindowEx(dwExStyles, SPLASH_CLASS, "About CCIDE", dwStyles, iPosX, iPosY, wWidth, wHeight,
                                          lpSplash->hWndOwner, NULL, lpSplash->hInstance, NULL);

                    SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpSplash2);

                    SendMessage(hWnd, WM_COMMAND, ID_SHOW, 0);

                    while (GetMessage(&msg, NULL, 0, 0))
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
                else
                    free(lpBuffer);
            }
        }
    }

    GlobalFree((HGLOBAL)lpSplash);
    return 0;
}

/******************************************************************************/
VOID WINAPI SplashScreen(LPSPLASH lpSplash)
{
    HANDLE hThread;
    unsigned int dwTid;
    LPSPLASH lpSplash_t;

    lpSplash_t = (LPSPLASH)GlobalAlloc(GPTR, sizeof(SPLASH));

    if (lpSplash_t != NULL)
    {
        memcpy(lpSplash_t, lpSplash, sizeof(SPLASH));

        hThread = (HANDLE)_beginthreadex(NULL, 0, SplashScreen_Thread, (LPVOID)lpSplash_t, 0, &dwTid);

        if (lpSplash->bWait && hThread != NULL)
            WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
}
