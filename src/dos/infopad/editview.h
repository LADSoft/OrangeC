#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TFileDialog
#define Uses_TMenuBar
#define Uses_TStatusLine
#define Uses_TScrollBar
#define Uses_TFrame

#include "tv.h"

struct INTERNAL_CHAR
{
    char ch;
	unsigned char Color;
}; 

struct ClipBoard
{
    char *text;
    int size;
} ;
#define LANGUAGE_NONE 0
#define LANGUAGE_ASM 1
#define LANGUAGE_C 2
#define LANGUAGE_RC 3

// The next few constants control the UNDO mechanism
#define UNDO_MAX 1000
#define UNDO_INSERT 1
#define UNDO_DELETE 2
#define UNDO_BACKSPACE 3
#define UNDO_MODIFY 4
#define UNDO_DELETESELECTION 5
#define UNDO_INSERTSELECTION 6
#define UNDO_CASECHANGE 7
#define UNDO_AUTOEND 8
#define UNDO_AUTOBEGIN 9
#define UNDO_AUTOCHAINBEGIN 10
#define UNDO_AUTOCHAINEND 11
// a list of UNDO structures describe each operation the user performs.
// The list is traversed backwards if 'undo' is pressed.
struct UNDO
{
    int preselstart;
    int preselend;
    int postselstart;
    int postselend;
	int charpos;
    int len;
    int max;
    unsigned char *data;
	int undotemp;
    char type;
    char modified: 1;
    char noChangeSel: 1;
};
// the EDITDATA structure holds all information the edit control has
// available on a per window basis.  Note that this isn't all the information
// there is about the window; things that aren't related to editing
// may be held in structures in the EDITOR module
struct COREDATA
{
	struct EDLIST *siblings ;
    INTERNAL_CHAR *text;
    int defbackground;
    int defforeground;
	char highlightText[256];
    int textlen;
    int textmaxlen;
    int tabs;
    int leftmargin;
    int updowncol;
    UNDO undolist[UNDO_MAX];
    int undohead;
    int undotail;
	int redopos;
    char undoing;
    char language;
    char modified;
    char nosel;
    char inserting;
    char selecting;
    char buttondown;
    char autoscrolldir;
    char ttup;
    char sendchangeonpaint;
    char readonly;
};

struct EDITDATA : public TScroller
{
	COREDATA *cd ;
    struct DWINFO *parent;
	int id ;
    int selstartcharpos;
    int selendcharpos;
    int textshowncharpos;
    int leftshownindex;
	int matchingStart;
	int matchingEnd;
	int wheelIncrement;
    char hasfocus;
    char hiddenCaret;

    TScrollBar *hScrollBar , *vScrollBar;    
    
    EDITDATA(TRect &bounds, TScrollBar *horiz, TScrollBar *vert,
             EDITDATA *old) : TScroller(bounds, horiz, vert), hScrollBar(horiz), vScrollBar(vert)
    {
        options |= ofSelectable;
        eventMask |= evMouseMove | evMouseUp;
        WinMessage(WM_CREATE,0,(DWORD)old);
    }
    virtual ~EDITDATA()
    {
        WinMessage(WM_DESTROY,0,0);
    }
    virtual DWORD WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam);
    virtual void draw();
    virtual void handleEvent( TEvent& event );
};


struct EDLIST
{
	EDLIST *next;
	EDITDATA *data;
};

struct LINEWND : public TView
{
    LINEWND(TRect &bounds) :TView(bounds) { startLine = 1; enabled = FALSE; lineNumberDigits = bounds.b.x - bounds.a.x - 1; }
    int startLine;
    int lineNumberDigits;
    bool enabled;
    virtual void draw();
} ;
struct DWINFO : public TWindow
{
    struct DWINFO *fwd, *back;
    struct InfoPad *application;
    TFileDialog *fileDlg;
	EDITDATA *child;
    LINEWND *lineWnd;
    char dwTitle[260];
    char dwName[260];
    FILETIME time;
    int dwLineNo;
    DWORD temp;
    TScrollBar *horizSB, *vertSB;
    CHARRANGE findStart;
    CHARRANGE findPos;
    int logMRU: 1;
    int newFile : 1;
	int timing : 1;
    int uninstantiated : 1;
    int dosStyle : 1;
    int inSaveDialog : 1;
    int deferClose:1;
    int shouldClose:1;
    DWINFO() :TWindow(TRect(0,0,0,0), dwTitle, 0), TWindowInit(&TWindow::initFrame), fwd(NULL),back(NULL), finding(true), findFlags(0), replaceFlags(0) { }
    DWINFO(InfoPad *appl, TRect &bounds, const char *title, DWINFO *old): TWindow(bounds, title, 0), finding(true), findFlags(0), replaceFlags(0),
        TWindowInit(&TWindow::initFrame), application(appl)
    {
        options |= ofFirstClick;
        WinMessage(WM_CREATE,0,(DWORD)old);
        memset(&findStart,0 , sizeof(findStart));
        memset(&findPos,0 , sizeof(findPos));
    }
    virtual ~DWINFO() {
        WinMessage(WM_CLOSE, 0, 0);
    }
    virtual void changeBounds( const TRect& bounds );
    virtual DWORD WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam);
    virtual void handleEvent( TEvent& event );
    
    int finding;
    int findFlags;
    int replaceFlags;
    void FindSetDirty();
    void EndFind();
    void OpenFindDialog();
    void OpenReplaceDialog();
    void DoFindNext();
    void DoReplaceNext();
};

struct EnhancedStatusLine : public TStatusLine
{
    struct statusItem
    {
        const char *text;
        int offset;
    };
    EnhancedStatusLine( const TRect& bounds, TStatusDef& aDefs ) : TStatusLine(bounds, aDefs) { }
    virtual DWORD WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam);
    virtual void draw();
    
    struct statusItem *items;
    int count;
};
typedef struct LsTabNotify
{
	NMHDR hdr;
	char *text;
	LPARAM lParam;
} LSTABNOTIFY;
struct _stList
{
    struct _singleTab *head;
    struct _singleTab *tail;
};
struct _stItem
{
    struct _singleTab *prev;
    struct _singleTab *next;
};
struct _singleTab
{
    char text[MAX_PATH];
    struct _stItem chain;
    struct _stItem selectedChain;
    LPARAM lParam;
    BOOL displayed;
    BOOL modified;
    TRect displayRect;
    TRect closeBtn;
}; 
struct _tabStruct : public TView
{
    struct _stList active;
    struct _stList selected;
    struct _singleTab *highlighted;
    InfoPad *parent;
    int fontHeight;
    TRect windowBtn;
    TRect xRect;
    BOOL captured;
    BOOL flat;
    BOOL displayMenu;
    DWORD windowBtnMode;
    BOOL dragging;
    struct _singleTab *dragSrc;
    struct _singleTab **menuList;
    
    _tabStruct(TRect bounds) : TView(bounds)
    {
        eventMask |= evMouseMove | evMouseUp;
        options |= ofFirstClick;
        WinMessage(WM_CREATE, 0, 0);
    }
    ~_tabStruct()
    {
        WinMessage(WM_CLOSE, 0, 0);
    }
    virtual DWORD WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam);
    virtual void draw();
    virtual void handleEvent(TEvent &event);
} ;
#define EM_UPDATESIBLING 10000
#define EN_LINECHANGE 10001
#define EN_SETCURSOR 10002
#define WM_REDO 10003
#define EN_NEEDFOCUS 10004
#define EM_GETINSERTSTATUS 10005
#define EM_GETEDITDATA 10006
#define EM_LOADLINEDATA 10007
#define WM_SETEDITORSETTINGS 10008
#define WM_WORDUNDERCURSOR 10009
#define WM_WORDUNDERPOINT 10010
#define EM_GETSIZE 10011
#define EM_SETFIRSTVISIBLELINE 10012
#define EM_GETTEXTHEIGHT 10013
#define EM_LANGUAGE 10014
#define EM_GETCOLUMN 10015
#define EM_GETREADONLY 10016
#define EM_TOUPPER 10017
#define EM_SELECTINDENT 10018
#define EM_SELECTCOMMENT 10019
#define EM_TOLOWER 10020
#define WM_SETLINENUMBERMODE 10021
#define EN_SETFOCUS 10022
#define EN_NEEDFOCUS 10023
#define EN_CHANGE 10024
#define WM_SETSIBLINGID 10025
#define WM_FILETITLE 10026
#define WM_FILENAME 10027

#define IDM_SAVE 2000
#define IDM_SAVEAS 2001
#define IDM_SAVEAS2 2002
#define ID_QUERYHASFILE 2003
#define IDM_CLOSEFIND 2004
#define IDM_COPYWINDOW 2005
#define IDM_GOTO 2006
#define IDM_SETLINE 2007
#define ID_REDRAWSTATUS 2008
#define ID_QUERYSAVE 2009
#define IDM_CLOSE 2010
#define IDM_UNDO 2011
#define IDM_REDO 2012
#define IDM_CUT 2013
#define IDM_COPY 2014
#define IDM_PASTE 2015
#define IDM_SELECTALL 2016
#define IDM_FIND 2017
#define IDM_REPLACE 2018
#define IDM_FINDNEXT 2019
#define IDM_TOUPPER 2020
#define IDM_TOLOWER 2021
#define IDM_COMMENT 2022
#define IDM_UNCOMMENT 2023
#define IDM_INDENT 2024
#define IDM_UNINDENT 2025

#define TABM_ADD 	2000
#define TABM_REMOVE (2000 + 1)
#define TABM_REMOVEALL (2000 + 2)
#define TABM_SELECT (2000 + 3)
#define TABM_GETCURSEL (2000 + 4)
#define TABM_RENAME (2000 + 5)
#define TABM_SETMODIFY (2000 + 6)
#define TABM_GETITEM (2000 + 7)

#define TABN_SELECTED (2100)
#define TABN_CLOSED   (2101)

int PropGetInt(void *xx, char *name);
int PropGetBool(void *xx, char *name);

