/*------------------------------------------------------------*/
/* filename -       tmenupop.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TMenuPopup member functions               */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TRect
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TMenuPopup
#define Uses_TEvent
#include "tv.h"


// this class did not originally conmpile it had lots of incongruitities with the
// rest of the library

TMenuPopup::TMenuPopup(TRect& bounds, TMenu* aMenu) :
    TMenuBox( bounds, aMenu, NULL)
{
}

void TMenuPopup::handleEvent(TEvent& event)
{
    switch (event.what)
    {
    case evKeyDown:
        TMenuItem* p = findItem(getCtrlChar(event.keyDown.keyCode));
        if (!p)
            p = hotKey(event.keyDown.keyCode);
        if (p && commandEnabled(p->command))
        {
            event.what = evCommand;
            event.message.command = p->command;
            event.message.infoPtr = NULL;
            putEvent(event);
            clearEvent(event);
        }
        else
            if (getAltChar(event.keyDown.keyCode))
                clearEvent(event);
        break;
    }
    TMenuBox::handleEvent(event);
}
