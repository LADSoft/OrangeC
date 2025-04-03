/* Software License Agreement
 * 
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 * 
 */

#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TButton
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_MsgBox
#define Uses_TListBox
#define Uses_TSortedCollection
#include "tv.h"
#include <windows.h>
#include <richedit.h>
#include <commctrl.h>
#include "InfoPad.h"
#include "editview.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "ipconfig.h"
#include "..\..\version.h"

extern DWINFO *editWindows;
extern MRU WindowMRU;

EnhancedStatusLine *statusLine;
_tabStruct *srcTab;


DWINFO *newInfo;

InfoPadCfg cfg;

static int x = 5, y = 5;

extern void ApplyEditorSettings();
extern void CloseAll();

int main(int argc, char **argv)
{
    InfoPad app;
    app.run();
}
int PropGetInt(void *xx, char *name)
{
    if (!strcmp(name, "TAB_INDENT"))
        return cfg.TabIndent();
    return 0;
}
int PropGetBool(void *xx, char *name)
{
    if (!strcmp(name, "COLORIZE"))
        return cfg.IsToColorize();
    if (!strcmp(name, "AUTO_INDENT"))
        return cfg.IsToAutoIndent();
    if (!strcmp(name, "AUTO_FORMAT"))
        return cfg.IsToAutoFormat();
    if (!strcmp(name, "TABS_AS_SPACES"))
        return cfg.IsTabsAsSpaces();
    if (!strcmp(name, "MATCH_PARENTHESIS"))
        return cfg.IsToMatchParenthesis();
    if (!strcmp(name, "BACKUP_FILES"))
        return cfg.IsToBackupFiles();
    if (!strcmp(name, "LINE_NUMBERS"))
        return cfg.IsToShowLineNumbers();
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
    TRect xbounds = bounds;
    xbounds.a.y++;
    xbounds.b.y = xbounds.a.y + 1;
    srcTab = new _tabStruct(xbounds);
    srcTab->parent = (InfoPad *)TProgram::application;
    ((InfoPad *)TProgram::application)->insert(srcTab);
    bounds.a.y+=2;
    bounds.b.y--;
    return new TDeskTop(bounds);
}
static ushort optionsBoxRect( const TRect &bounds)
{
    char data[256];
    char *Title = "Options";
    char *aLabel = "Tab Spacing";
    int textlen = 5;
    TDialog *dialog;
    TView* control;
    TRect r;
    ushort c;
    ushort options = 0;
    sprintf(data,"%d", cfg.TabIndent());
    data[3] = 0;
    if (cfg.IsTabsAsSpaces())
        options |= 1;
    if (cfg.IsToColorize())
        options |= 2;
    if (cfg.IsToAutoIndent())
        options |= 4;
    if (cfg.IsToAutoFormat())
        options |= 8;
    if (cfg.IsToMatchParenthesis())
        options |= 16;
    if (cfg.IsToShowLineNumbers())
        options |= 32;
    if (cfg.IsToBackupFiles())
        options |= 64;

    *(ushort *)(data + textlen) = options;
    
    dialog = new TDialog(bounds, Title);

    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
    control = new TInputLine( r, textlen );
    dialog->insert( control );

    r = TRect(2, 2, 3 + strlen(aLabel), 3);
    dialog->insert( new TLabel( r, aLabel, control ) );
    
    r = TRect(2, 4, 47, 8);
    dialog->insert( new TCheckBoxes(r, new TSItem("Tabs As Spaces",
                                       new TSItem("Colorize",
                                       new TSItem("Auto Indent", 
                                       new TSItem("Auto Format", 
                                       new TSItem("Match Parenthesis", 
                                       new TSItem("Show Line Numbers", 
                                       new TSItem("Backup Files", 
                                                  NULL)))))))));    

    
    r = TRect( dialog->size.x - 24, dialog->size.y - 4,
               dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, MsgBoxText::cancelText, cmCancel, bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(False);
    dialog->setData(data);
    c = TProgram::application->execView(dialog);
    if( c != cmCancel )
    {
        dialog->getData(data);
        if (atoi(data) != 0)
            cfg.SetTabIndent(atoi(data));
        options = *(ushort *)(data + textlen);
        cfg.SetTabsAsSpaces(!!(options & 1));
        cfg.SetToColorize(!!(options & 2));
        cfg.SetToAutoIndent(!!(options & 4));
        cfg.SetToAutoFormat(!!(options & 8));
        cfg.SetToMatchParenthesis(!!(options & 16));
        cfg.SetToShowLineNumbers(!!(options & 32));
        cfg.SetToBackupFiles(!!(options & 64));
        ApplyEditorSettings();
    }
    TObject::destroy( dialog );
    return c;
}
static ushort optionsBox( )
{
    TRect r(0, 0, 50, 13);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return optionsBoxRect(r);
}
void InfoPad::AboutBox()
{
    TRect bounds(0, 0, 37, 11);
    bounds.move((TProgram::deskTop->size.x - bounds.b.x) / 2,
           (TProgram::deskTop->size.y - bounds.b.y) / 2);
    char *Title = "About InfoPad";
    char *aLabel = "InfoPad (C) 2006-2013 LADSoft";
    char *bLabel = "Version "STRING_VERSION;

    TDialog *dialog = new TDialog(bounds, Title);

    TRect r = TRect( (dialog->size.x - strlen(aLabel)-1)/2, 2, (dialog->size.x + strlen(aLabel)+1)/2, 3 );
    dialog->insert(new TLabel( r, aLabel, NULL ));

    r = TRect( (dialog->size.x - strlen(bLabel)-1)/2, 4, (dialog->size.x + strlen(bLabel)+1)/2, 5 );
    dialog->insert(new TLabel( r, bLabel, NULL ));
    r = TRect( (dialog->size.x - 6)/2, dialog->size.y - 4,
               (dialog->size.x + 6)/2, dialog->size.y - 2);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));
    TProgram::application->execView(dialog);
    TObject::destroy( dialog );
}
class TWindowCollection : public TSortedCollection
{
public:
    TWindowCollection() : TSortedCollection(MRU::MAX_WINDOW_MRU, 1) { }
    virtual ~TWindowCollection() { }
    virtual int compare( void *key1, void *key2 )
    {
        MRUItem *p = (MRUItem *)key1;
        MRUItem *q = (MRUItem *)key2;
        return strcmp(p->title, q->title);
    }
    virtual void *readItem( ipstream& )  { return NULL; }
    virtual void writeItem( void *xx, opstream&strm ) { }
};
void InfoPad::WindowMenuBox()
{
    TRect bounds(0, 0, 37, 19);
    bounds.move((TProgram::deskTop->size.x - bounds.b.x) / 2,
           (TProgram::deskTop->size.y - bounds.b.y) / 2);
    char *Title = "Available Windows";
    char *aLabel = "Select Window:";

    TDialog *dialog = new TDialog(bounds, Title);

    TRect r = TRect( 2, 2, 2 + strlen(aLabel), 3 );
    dialog->insert(new TLabel( r, aLabel, NULL ));

    r = TRect(3, 2, 33, 14);
    TRect sbRect = TRect(33,2, 34, 14);
    TScrollBar *vScroll = new TScrollBar(sbRect);
    dialog->insert( new TListBox(r, 1, vScroll));
    dialog->insert(vScroll);
    
    
    r = TRect( dialog->size.x - 24, dialog->size.y - 4,
               dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));
    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, MsgBoxText::cancelText, cmCancel, bfDefault));

    TListBoxRec rec;
    rec.items = new TWindowCollection;
    rec.selection = 0;
    for (int i=0; i < MRU::MAX_WINDOW_MRU; i++)
    {
        MRUItem *p = WindowMRU.Get(i);
        if (p)
        {
            rec.items->insert(new MRUItem(p->name, p->title));
        }
    }
    dialog->setData(&rec);

    ushort c = TProgram::application->execView(dialog);
    if (c == cmOK)
    {
        dialog->getData(&rec);
        MRUItem *p = (MRUItem *)rec.items->at(rec.selection);
        if (p)
        {
            static DWINFO temp;
            newInfo = &temp;
            newInfo->dwLineNo =  - 1;
            newInfo->logMRU = TRUE;
            strcpy(newInfo->dwTitle, p->title);
            strcpy(newInfo->dwName, p->name);
            openfile(newInfo, FALSE, TRUE);
        }
    }
    TObject::destroy(rec.items);
    TObject::destroy( dialog );
}
void InfoPad::handleEvent(TEvent& event)
{
    if (event.what == evCommand && event.message.command == cmQuit)
    {
        while (editWindows)
            editWindows->WinMessage(WM_COMMAND, IDM_CLOSE, 0);
    }
    TApplication::handleEvent(event);
    if (event.what == evCommand)
    {
        switch (event.message.command)
        {
            case ID_OPTIONS:
                optionsBox();
                break;
            case ID_MOREWINDOWS:
                if (WindowMRU.Get(0))
                    WindowMenuBox();
                else
                    messageBox("No windows available", mfInformation);
                break;
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
                    current->WinMessage(WM_COMMAND, IDM_SAVE, 0);
                }
                break;
            case ID_SAVEAS:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_SAVEAS, 0);
                }
                break;
            case ID_CLOSE:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_CLOSE, 0);
                    current->close();
                    current = NULL;
                }
                break;
            case ID_SEARCH:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_FIND, 0);
                }
                break;
            case ID_REPLACE:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_REPLACE, 0);
                }
                break;
            case ID_SEARCHNEXT:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_FINDNEXT, 0);
                }
                break;
            case ID_GOTO:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_GOTO, 0);
                }
                break;
            case ID_TOUPPER:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_TOUPPER, 0);
                }
                break;
            case ID_TOLOWER:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_TOLOWER, 0);
                }
                break;
            case ID_INDENT:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_INDENT, 0);
                }
                break;
            case ID_OUTDENT:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_UNINDENT, 0);
                }
                break;
            case ID_COMMENT:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_COMMENT, 0);
                }
                break;
            case ID_UNCOMMENT:
                if (current)
                {
                    current->WinMessage(WM_COMMAND, IDM_UNCOMMENT, 0);
                }
                break;
            case ID_CLOSEALL:
                CloseAll();
                break;
            case ID_ABOUT:
                AboutBox();
                break;
            default:
                if (event.message.command >= FILE_MRU_BASE && event.message.command < FILE_MRU_BASE + MRU::MAX_MRU)
                {
                    static DWINFO temp;
                    MRUItem *p = cfg.FileMRU().Get(event.message.command - FILE_MRU_BASE);
                    if (p)
                    {
                        newInfo = &temp;
                        newInfo->dwLineNo =  - 1;
                        newInfo->logMRU = TRUE;
                        strcpy(newInfo->dwTitle, p->title);
                        strcpy(newInfo->dwName, p->name);
                        openfile(newInfo, FALSE, TRUE);
                    }
                }
                else if (event.message.command >= WINDOW_MRU_BASE && event.message.command < WINDOW_MRU_BASE + MRU::MAX_WINDOW_MRU)
                {
                    static DWINFO temp;
                    MRUItem *p = WindowMRU.Get(event.message.command - WINDOW_MRU_BASE);
                    if (p)
                    {
                        newInfo = &temp;
                        newInfo->dwLineNo =  - 1;
                        newInfo->logMRU = TRUE;
                        strcpy(newInfo->dwTitle, p->title);
                        strcpy(newInfo->dwName, p->name);
                        openfile(newInfo, FALSE, TRUE);
                    }
                }
                break;
        }
    }

}
DWORD InfoPad::WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam)
{
    switch( iMessage)
    {
        case WM_NOTIFY:
        {
            LSTABNOTIFY *nt = (LSTABNOTIFY *)lParam;
            DWINFO *xx = (DWINFO *)nt->lParam;
            if (nt->hdr.code == TABN_CLOSED)
            {
                if (xx)
                {
                    xx->WinMessage(WM_COMMAND, IDM_CLOSE, 0);
                    xx->close();
                }
            }
            else if (nt->hdr.code == TABN_SELECTED)
            {
                if (xx)
                {
                    xx->select();
                }
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
                    ptr->select();
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