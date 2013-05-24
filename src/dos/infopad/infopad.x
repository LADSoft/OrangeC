#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#include "tv.h"
#include <windows.h>
#include <commctrl.h>
#include "InfoPad.h"
#include "editview.h"
#include <stdlib.h>
#include <sys/stat.h>

extern DWINFO *editWindows;

EnhancedStatusLine *statusLine;
SRCTAB *srcTab;

int toColorize = TRUE;
int toAutoIndent = TRUE;
int tabsAsSpaces = TRUE;
int toAutoFormat = TRUE;
int tabIndent = 4;
int toMatchParenthesis = TRUE;
int toBackupFiles = TRUE;
int toShowLineNumbers = FALSE;

DWINFO *newInfo;

static int x = 5, y = 5;
int main(int argc, char **argv)
{
    InfoPad app;
    app.run();
}

int PropGetInt(void *xx, char *name)
{
    if (!strcmp(name, "TAB_INDENT"))
        return tabIndent;
    return 0;
}
int PropGetBool(void *xx, char *name)
{
    if (!strcmp(name, "COLORIZE"))
        return toColorize;
    if (!strcmp(name, "AUTO_INDENT"))
        return toAutoIndent;
    if (!strcmp(name, "AUTO_FORMAT"))
        return toAutoFormat;
    if (!strcmp(name, "TABS_AS_SPACES"))
        return tabsAsSpaces;
    if (!strcmp(name, "MATCH_PARENTHESIS"))
        return toMatchParenthesis;
    if (!strcmp(name, "BACKUP_FILES"))
        return toBackupFiles;
    if (!strcmp(name, "LINE_NUMBERS"))
        return toShowLineNumbers;
    return FALSE;    
}
InfoPad::~InfoPad()
{
}
InfoPad::InfoPad() : TApplication(), TProgInit( &InfoPad::initStatusLine,
                  &InfoPad::initMenuBar,
                  &InfoPad::initDeskTop
                ), fileDlg(NULL), current(NULL)
{
    for (int i=1; i < __argc; i++)
    {
        static DWINFO temp;
        newInfo = &temp;
        char *p = __argv[i];
        struct stat xx;
        if (stat(p, &xx) >= 0)
        {
            char *q = strrchr(p, '\\');
            if (q)
                q++;
            else
                q = p;
            strcpy(newInfo->dwTitle, q);
            strcpy(newInfo->dwName, p);
            newInfo->dwLineNo = -1;
            openfile(newInfo, FALSE, True);
        }
    }
}
TStatusLine *InfoPad::initStatusLine( TRect bounds)
{
    TRect bounds1 = bounds;
    bounds1.a.y++;
    bounds1.b.y = bounds1.a.y+1;
    srcTab = new SRCTAB(bounds1);
    
    bounds.a.y = bounds.b.y - 1;
    ::statusLine = new EnhancedStatusLine( bounds,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( "Exit ~Alt-X~" ,kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu ) +
            *new TStatusItem( 0, kbAltF3, cmClose ) +
            *new TStatusItem( 0, kbF5, cmZoom ) +
            *new TStatusItem( 0, kbCtrlF5, cmResize )
            );
    int parts[6] = { 23, 36, 49, 62, 71, 75} ;
    ::statusLine->WinMessage(SB_SETPARTS, 6, (LPARAM)parts);
    return ::statusLine;
}
TDeskTop *InfoPad::initDeskTop(TRect bounds)
{
    bounds.a.y++;
    bounds.b.y--;
    return new TDeskTop(bounds);
}
void InfoPad::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);
    if (event.what == evCommand)
    {
        switch (event.message.command)
        {
            case ID_UNDO:
                if (current)
                {
                    current->child->WinMessage(WM_UNDO, 0, 0);
                }
                break;
            case IDM_REDO:
                if (current)
                {
                    current->child->WinMessage(WM_REDO, 0, 0);
                }
                break;
            case ID_CUT:
                if (current)
                {
                    current->child->WinMessage(WM_CUT, 0, 0);
                }
                break;
            case ID_COPY:
                if (current)
                {
                    current->child->WinMessage(WM_COPY, 0, 0);
                }
                break;
            case ID_PASTE:
                if (current)
                {
                    current->child->WinMessage(WM_PASTE, 0, 0);
                }
                break;
            case ID_NEW:
                openfile((DWINFO *)-1, True, True);
                break;
            case ID_OPEN:
                CreateDrawWindow((DWINFO *)0, True);
                break;
            case ID_SAVE:
                if (current)
                {
                    current->WinMessage(IDM_SAVE, 0, 0);
                }
                break;
            case ID_SAVEAS:
                if (current)
                {
                    current->WinMessage(IDM_SAVEAS, 0, 0);
                }
                break;
            case ID_CLOSE:
                if (current)
                {
                    current->WinMessage(ID_QUERYSAVE, 0, 0);
                    current->close();
                    current = NULL;
                }
                break;
        }
    }
}
//-------------------------------------------------------------------------
DWINFO *InfoPad::openfile(DWINFO *newInfo, int newwindow, int visible)
{
    int i;
    void *extra = newInfo == (DWINFO *)-1 ? newInfo : NULL ;
    MSG msg;
    if (newInfo && newInfo != (DWINFO*) - 1)
    {
        DWINFO *ptr = editWindows;
        while (ptr)
        {
            if (ptr->WinMessage(WM_COMMAND, ID_QUERYHASFILE, 
                (LPARAM)newInfo))
            {
                if (newwindow)
                {
                    extra = (EDITDATA *)ptr->WinMessage(EM_GETEDITDATA, 0, 0);
                }
                else
                {
                    setCurrent(ptr, normalSelect);
                    if (newInfo->dwLineNo !=  - 1)
                            ptr->WinMessage(WM_COMMAND, IDM_SETLINE, newInfo
                                ->dwLineNo);
                    return ptr;
                }
            }
            ptr = ptr->fwd;
        }
    }
    TRect bounds;
    bounds.a.x = x;
    bounds.a.y = y;
    bounds.b.x = 45+bounds.a.x;
    bounds.b.y = 15+bounds.a.y;
    x++;
    y++;
    if (x > 20)
        x = 5;
    if (y > 15)
        y = 5;
    char p[1];
    p[0] = 0;
    DWINFO *rv = new DWINFO(this, bounds, p, (DWINFO *)extra);
    insertWindow(rv);
    return rv;
}
//-------------------------------------------------------------------------
DWINFO *InfoPad::CreateDrawWindow(DWINFO *baseinfo, int visible)
{
    int i;
    static DWINFO temp;
    OPENFILENAME ofn;

    newInfo = baseinfo;
    if (!newInfo || newInfo == (DWINFO *)-1)
    {
        newInfo = &temp;
        newInfo->dwLineNo =  - 1;
        newInfo->logMRU = TRUE;
        newInfo->newFile = newInfo == (DWINFO *)-1;
        fileDlg = new TFileDialog( "*.*", newInfo->newFile ? "Create new file" : "Open File", "", fdOKButton, 0);
        char buf[512];
        buf[0] = 0;
        if (executeDialog(fileDlg,buf) == cmFileOpen)
        {
            fileDlg = NULL;
            char *q = strrchr(buf, '\\');
            if (q)
                q++;
            else
                q = buf;
            strcpy(newInfo->dwTitle, q);
            strcpy(newInfo->dwName, buf);
            openfile(newInfo, FALSE, True);
        }
    }
    else
    {
        openfile(newInfo, FALSE, visible);
    }
}
void InfoPad::idle()
{
    TApplication::idle();
    
}