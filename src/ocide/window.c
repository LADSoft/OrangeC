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

#define INITIAL_DOCK_COUNT 13

int initialIDs[INITIAL_DOCK_COUNT] = 
{
    DID_TABWND, DID_INFOWND, DID_ERRWND, DID_WATCHWND, DID_LOCALSWND, DID_PROPSWND, DID_EDITTOOL, 
    DID_BUILDTYPETOOL, DID_NAVTOOL, DID_BUILDTOOL,
        DID_DEBUGTOOL, DID_BOOKMARKTOOL, DID_THREADSTOOL
};
CCD_params initialDocks[INITIAL_DOCK_COUNT] = 
{
    { // tabwnd
        DOCK_LEFT, DOCK_LEFT, FALSE, 0, 0, 0, 1000 *200, 
        {
            0, 0, 230, 200
        }
        , 
        {
            0, 0, 230, 200
        }
        , 
        {
            0, 0, 230, 200
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { //info wind
        DOCK_BOTTOM, DOCK_BOTTOM, FALSE, 0, 0, 0, 1000 *500, 
        {
            0, 0, 500, 200
        }
        , 
        {
            0, 0, 500, 200
        }
        , 
        {
            0, 0, 500, 200
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { //error wind
        DOCK_BOTTOM, DOCK_BOTTOM, TRUE, 0, 0, 0, 1000 *500, 
        {
            0, 0, 500, 200
        }
        , 
        {
            0, 0, 500, 200
        }
        , 
        {
            0, 0, 500, 200
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    ,
    { // watch wind
        DOCK_BOTTOM, DOCK_BOTTOM, TRUE, 0, 0, 0, 1000 *500, 
        {
            0, 0, 500, 160
        }
        , 
        {
            0, 0, 500, 160
        }
        , 
        {
            0, 0, 500, 160
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    ,
    { // locals wind
        DOCK_BOTTOM, DOCK_BOTTOM, TRUE, 0, 0, 0, 1000 *500, 
        {
            0, 0, 500, 160
        }
        , 
        {
            0, 0, 500, 160
        }
        , 
        {
            0, 0, 500, 160
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    ,
    { // props wind
        DOCK_RIGHT, DOCK_RIGHT, TRUE, 0, 0, 0, 1000 *200, 
        {
            0, 0, 230, 200
        }
        , 
        {
            0, 0, 230, 200
        }
        , 
        {
            0, 0, 230, 200
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    ,
    { // edit tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 0, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { // build type tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 0, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { // nav tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 0, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { // build tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 1, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { // debug tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 1, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { // bookmark tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 1, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
    { // threads tool
        DOCK_TOP, DOCK_TOP, FALSE, 0, 1, 0, 0, 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 200, 30
        }
        , 
        {
            0, 0, 0, 0
        }
    }
    , 
};

void CreateDocks(void)
{
    dmgrSetInfo(initialIDs, initialDocks, INITIAL_DOCK_COUNT);
}