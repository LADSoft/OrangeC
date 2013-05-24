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
        } SPLASH, FAR *LPSPLASH;

        VOID WINAPI SplashScreen(LPSPLASH lpSplash);

        #ifdef __cplusplus
        }
    #endif 

#endif
