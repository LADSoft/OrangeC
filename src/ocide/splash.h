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

/* DESCRIPTION    : Structures/definitions for splash screen functions        */
/* LANGUAGE       : "C"                                                       */
/* PLATFORM       : Windows 95/98/98SE/ME/NT4/2000                            */
/* AUTHOR         : Andrea Binello  <andrea.binello@tiscalinet.it>            */
/* LICENSE        : Freeware - Open Source                                    */
/* LAST UPDATE    : May 06, 2001                                              */
/******************************************************************************/

#ifndef __SPLASH_H__
#define __SPLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        HWND hWndOwner;
        HINSTANCE hInstance;
        HINSTANCE hInstanceRes;
        LPSTR lpszResource;
        USHORT uTime;
        BOOL bCentered;
        BOOL bTopmost;
        BOOL bWait;
        BOOL bAbout;
        INT iPosX;
        INT iPosY;
        RECT bPos;
        LPSTR lpszButtonTitle;
        LPSTR lpszVersion;
    } SPLASH, FAR* LPSPLASH;

    VOID WINAPI SplashScreen(LPSPLASH lpSplash);

#ifdef __cplusplus
}
#endif

#endif
