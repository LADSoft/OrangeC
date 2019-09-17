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

#include "idewinconst.h"
#include "progress.h"
#include "ctl.h"
#include "codecomp.h"

#define BEGINTHREAD_FUNC void (*)(void*)
#define BROWSE_VERSION 2

#pragma pack(1)

#ifdef BORLAND
#    define LLONG_TYPE __int64
#else
#    define LLONG_TYPE long long
#endif
#define MAX_COMBO_HISTORY 10

#define HBREAK

#define CONTROL_FONT "Courier New"
#define CONTROL_FONT_BASIC_SIZE 14

#define MAX_HANDLES 42

#define DID_PROJWND 1
#define DID_RESWND 2
#define DID_PROPSWND 3
#define DID_CTLTBWND 4
#define DID_ERRWND 5
#define DID_INFOWND 6
#define DID_FINDWND 7
#define DID_STACKWND 9
#define DID_THREADWND 10
#define DID_BREAKWND 11
#define DID_REGWND 12
#define DID_LOCALSWND 13
#define DID_MEMWND 14   /* through 17 */
#define DID_WATCHWND 18 /* through 21 */
#define DID_BROWSEWND 22
//#define DID_ASMWND 4  keep reserved for compatibility with older projects

#define DID_EDITTOOL 32
#define DID_BUILDTOOL 33
#define DID_DEBUGTOOL 34
#define DID_NAVTOOL 35
#define DID_BOOKMARKTOOL 36
#define DID_THREADSTOOL 37
#define DID_BUILDTYPETOOL 38

#define IML_BP 0
#define IML_CONTINUATION 1
#define IML_STOPBP 2
#define IML_BPGRAYED 3
#define IML_STOP 4
#define IML_BOOKMARK 5
#define IML_FIF1 6
#define IML_FIF2 7
#define IML_BLANK 8
#define IML_DISABLEDBP 9
#define ILEDIT_IMAGECOUNT 10

#define TAG_BP 0
#define TAG_FIF1 1
#define TAG_FIF2 2
#define TAG_BOOKMARK 3
#define TAG_BPGRAYED 4   /* no tags gathered here, this is just a return value */
#define TAG_DISABLEDBP 5 /* ditto */
#define TAG_MAX 6

#define TAGM_SAVEFILE 0
#define TAGM_DISCARDFILE 1
#define TAGM_UPDATEDEBUG 2

#define TAGF_ENABLED 1
#define TAGF_GRAYED 2

#define HELP_MSDN 0
#define HELP_SDK 1
#define HELP_SPECIFIED 2

#define ERR_NO_WINDOW -1
#define ERR_BUILD_WINDOW 0
#define ERR_EXTENDED_BUILD_WINDOW 1
#define ERR_DEBUG_WINDOW 2

#define LANGUAGE_NONE 0
#define LANGUAGE_C 1
#define LANGUAGE_ASM 2
#define LANGUAGE_RC 3
#define LANGUAGE_CPP 4

#define EDITSIG 0xf1823794
#define RESSIG 0x1F8237A5

#define IL_CWA 0
#define IL_CONSOLE 1
#define IL_GUI 2
#define IL_LIB 3
#define IL_DLL 4
#define IL_FILES 5
#define IL_ASM 6
#define IL_CPP 7
#define IL_C 8
#define IL_H 9
#define IL_RES 10
#define IL_RC 11
#define IL_IMAGECOUNT 12

#define DEFINE_TONASM 1

#define BF_DEBUGINFO 1
#define BF_MAPFILE 2
#define BF_COMPILEVIAASM 4
#define BF_CRTDLL 8 /* obsolete */
#define BF_BROWSEINFO 16
#define BF_SHOWWARNINGS 32
#define BF_C99 64
#define BF_ANSI 128
#define BF_SHOWRETURNCODE 0x10000
#define BF_BREAKDLL 0x20000
#define BF_DEBUGEXCEPTION 0x40000
#define BF_DEBUGTOOLTIPS 0x80000
#define BF_HWBP 0x100000
#define BF_CHANGEDTARGET 0x40000000
#define BF_ALIGNSTACK 0x80000000

#define TFO_MAIN 1
#define TFO_SOURCE 2
#define TFO_INCLUDE 4
#define TFO_RESOURCE 8
#define TFO_OTHER 16
typedef struct _namelist
{
    struct _namelist* next;
    char* data;
} NAMELIST;

enum
{
    BT_CONSOLE,
    BT_WINDOWS,
    BT_DLL,
    BT_LIBRARY,
    BT_DOS,
    BT_RAW
};
enum
{
    LT_STANDARD,
    LT_LSCRTDLL,
    LT_NONE,
    LT_CRTDLL,
    LT_MSVCRT
};
typedef struct _settingCombo
{
    struct _settingCombo* next;
    char* displayName;
    char* value;
} SETTINGCOMBO;

typedef struct _setting
{
    struct _setting* next;
    struct _setting* children;
    struct _setting* command;
    struct _setting* assignments;
    struct _setting* depends;
    enum slType
    {
        e_separator,
        e_tree,
        e_assign,
        e_assignments,
        e_values = 100,
        e_choose,
        e_combo,
        e_text,
        e_numeric,
        e_color,
        e_font,
        e_printformat,
        e_prependtext,
        e_separatedtext,
        e_multitext
    } type;
    HTREEITEM hTreeItem;
    HWND hWnd;
    char* displayName;
    char* ext;
    char* select;
    char* cls;
    char* id;
    char* deflt;
    char* value;
    char* tentative;
    char* aux;
    struct _settingCombo* combo;
    char *lowLimit, *highLimit;
    int order;
    int helpid;
    int clean : 1;
} SETTING;
typedef struct _profile
{
    struct _profile* next;
    char* name;
    SETTING* debugSettings;
    SETTING* releaseSettings;
} PROFILE;

typedef struct _profileNameList
{
    struct _profileNameList* next;
    char name[256];
} PROFILENAMELIST;
typedef struct BuildRule
{
    struct BuildRule* next;
    char name[MAX_PATH];
    PROFILE* profiles;
    int defaultRule;
    int active;
    int remove;
    int add;
} BUILDRULE;
typedef struct nameValuePair
{
    struct nameValuePair* next;
    char* name;
    char* value;
    BOOL alloced;
    BOOL assign;
} NameValuePair;

typedef struct _projectItem
{
    enum pi_type
    {
        PJ_WS,
        PJ_PROJ,
        PJ_FOLDER,
        PJ_FILE,
        PJ_RES,
        PJ_RESMENU = 200
    } type;  // shared with resources
    struct _projectItem* next;
    struct _projectItem* parent;
    struct _projectItem* children;
    struct _projectItem* projectBuildList;
    struct _projectItemList* depends;
    struct _projectItem* internalDependsLink;
    char displayName[256];
    char realName[MAX_PATH];
    char outputExt[MAX_PATH];
    char outputName[MAX_PATH];
    FILETIME fileTime, outputTime;
    HTREEITEM hTreeItem, hResTreeItem;
    struct resData* resData;
    PROFILE* profiles;
    NameValuePair* mmacro;
    NameValuePair* mmcmd;
    NameValuePair* mmcustomCmd;
    BOOL expanded;
    BOOL changed;
    BOOL loaded;
    BOOL visited;
    BOOL clean;
} PROJECTITEM;

typedef struct _projectItemList
{
    struct _projectItemList* next;
    PROJECTITEM* item;
} PROJECTITEMLIST;
// end new project stufff
typedef struct
{
    int id;
    char* text;
} POPUPMENUTEXT;

typedef struct
{
    int start, end;
    int color;
    int italic;
} EDITATTRIBS;

typedef struct _bkpt
{
    struct _bkpt* next;
    int* addresses;
    int* tempvals;
    BOOL active;
    int linenum;
    char module[256];
    char name[256];
    void* extra;  // to be defined later
} BREAKPOINT;

typedef struct _threads
{
    CONTEXT regs;  // keep this first, it has to be DWORD aligned
    // malloc will always return DWORD alignment so we are safe
    struct _threads* next;
    struct _process* process;
    DWORD idThread;
    HANDLE hThread;
    DWORD oldaddr;
    DWORD suspcount;
    BREAKPOINT breakpoint;
    char name[256];
} THREAD;

typedef struct _scope
{
    struct _scope* next;
    int address;
    int basePtr;
    int lineno;
    char name[MAX_PATH];
    char fileName[MAX_PATH];
} SCOPE;
typedef struct dbg_info
{
    void* dbPointer;
    DWORD linkbase;
    DWORD loadbase;
} DEBUG_INFO;

typedef struct function_list
{
    struct function_list* next;
    char name[256];
    int address;
} FUNCTIONLIST;
typedef struct dll_info
{
    struct dll_info* next;
    DEBUG_INFO* dbg_info;
    DWORD base;
    char name[MAX_PATH];
    int fUnicode;
    int breakpoint;
    HANDLE hFile;
} DLL_INFO;

typedef struct _process
{
    struct _process* next;
    HANDLE hProcess;
    DWORD idProcess;
    DWORD base;
    DLL_INFO* dll_info;
    THREAD* threads;
    DEBUG_INFO* dbg_info;
    BREAKPOINT breakpoints;
    DWORD idTempBpThread;
    DWORD ExitAddr;
    char name[MAX_PATH];
} PROCESS;
enum DebugState
{
    notDebugging,
    SteppingOut,
    SteppingOver,
    SteppingIn,
    StepInOut,
    FinishStepOut,
    Running,
    atBreakpoint,
    atException,
    Aborting,
    nullState
};

typedef struct varinfo
{
    TCData watchhead;
    HTREEITEM hTreeItem, hTreeHolder;
    THREAD* thread;
    SCOPE* scope;
    struct varinfo* subtype; /* for substructures */
    struct varinfo* link;    /* for next link in structure */
    int type;
    int enumfield;
    int address;
    int size;
    int derefaddress;
    int offset;
    long double fval, fvali;
    LLONG_TYPE ival;
    int bitstart;
    int bitlength;
    int outofscopereg : 1;
    int constant : 1;
    int pointer : 1;
    int lref : 1;
    int rref : 1;
    int enumx : 1;
    int structure : 1;
    int unionx : 1;
    int bitfield : 1;
    int array : 1;
    int outofscope : 1;
    int editable : 1;
    int udt : 1;
    int explicitreg : 1;
    int inreg : 1;
    int vararray : 1;
    int argument : 1;
    int arraysize;
    int itemsize;
    int vararraylevel;
    int* vararraylist;
    int* vararraylisttofree;
    int watchindex;
    char membername[256];
    char structtag[256];
    char value[256];
    char screenname[256];
} VARINFO;

typedef struct
{
    VARINFO* info;
    DEBUG_INFO* dbg_info;
    int cursoreip;
    int marked : 1;
} WATCHINFO;

struct tag
{
    struct tag *next, *prev;
    int drawnLineno;
    int editingLineno;
    int debugLineno;
    int fileLineno;
    int charpos;
    int enabled;
    int disable;
    void* extra;
};

struct tagchangeln
{
    struct tagchangeln* next;
    int lineno;
    int delta;
};
struct tagfile
{
    struct tagfile *next, *prev;
    char name[256];
    struct tag* tagArray[TAG_MAX];
    struct tagchangeln* changedLines;
};
typedef struct
{
    //    COLORREF color;
    unsigned short Color : 10;
    unsigned short effect : 3;
    unsigned short squiggle : 1;
    unsigned short pad1 : 2;
    unsigned char pad2;
    unsigned char ch;
    //    char pad;
} INTERNAL_CHAR;

#define CRLF_DUMMY 255

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
typedef struct
{
    int preselstart;
    int preselend;
    int postselstart;
    int postselend;
    int charpos;
    int len;
    int max;
    unsigned char* data;
    int undotemp;
    char type;
    char modified : 1;
    char noChangeSel : 1;
} UNDO;

// the EDITDATA structure holds all information the edit control has
// available on a per window basis.  Note that this isn't all the information
// there is about the window; things that aren't related to editing
// may be held in structures in the EDITOR module
typedef struct
{
    struct _edlist_* siblings;
    unsigned char* lineData;
    int lineDataMax;
    HFONT hFont, hBoldFont, hItalicFont, hItalicBoldFont;
    HBRUSH hbrBackground;
    COLORREF defforeground;
    COLORREF defbackground;
    INTERNAL_CHAR* text;
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
    UINT reparseTimerID;
    char undoing;
    char language;
    char modified;
    char txtFontHeight;
    char txtFontWidth;
    char nosel;
    char inserting;
    char selecting;
    char buttondown;
    char autoscrolldir;
    char ttup;
    char sendchangeonpaint;
    char readonly;
    char colorizing;
} COREDATA;

#define COLORIZE_HASH_SIZE 251
typedef struct _colorize_hash_entry
{
    struct _colorize_hash_entry* next;
    char* name;
    struct _colorize_lines_entry
    {
        struct _colorize_lines_entry* next;
        int start;
        int end;
        int type;
    } * lines;
} COLORIZE_HASH_ENTRY;
typedef struct _editData
{
    COREDATA* cd;
    HWND self;
    HWND tooltip;
    struct _editData* popupDerivedFrom;
    int marks[10];
    int id;
    int insertcursorcolumn;
    int selstartcharpos;
    int selendcharpos;
    int textshowncharpos;
    int leftshownindex;
    int matchingStart;
    int matchingEnd;
    int wheelIncrement;
    char hasfocus;
    char hiddenCaret;
    char lastWasColon;
    COLORIZE_HASH_ENTRY* colorizeEntries[COLORIZE_HASH_SIZE];
} EDITDATA;

typedef struct _edlist_
{
    struct _edlist_* next;
    EDITDATA* data;
} EDLIST;

#define C_BACKGROUND 0
#define C_READONLYBACKGROUND 1
#define C_TEXT 2
#define C_HIGHLIGHT 3
#define C_KEYWORD 4
#define C_COMMENT 5
#define C_NUMBER 6
#define C_STRING 7
#define C_ESCAPE 8
#define C_DEFINE 9
#define C_FUNCTION 10
#define C_PARAMETER 11
#define C_TYPEDEF 12
#define C_TAG 13
#define C_AUTO 14
#define C_LOCALSTATIC 15
#define C_STATIC 16
#define C_GLOBAL 17
#define C_EXTERNAL 18
#define C_LABEL 19
#define C_MEMBER 20
#define C_SYS_WINDOWBACKGROUND 31
// The KEYLIST structure holds information about text which should be
// colorized.  It gives the name of a keyword (e.g. 'int') and the color
// to display it in.

typedef struct
{
    char* text;
    unsigned char Color;
    //    COLORREF *color;
} KEYLIST;

typedef struct cinfo
{
    EDITDATA* ed;
    HWND wnd;
    int creation;
} CINFO;

// fileflags field
#define FD_DOS 1
#define FD_UTF8 2
#define FD_UCS2LE 4

typedef struct dwinfo
{
    struct dwinfo *prev, *next;
    EDITDATA* editData;
    char dwTitle[260];
    char dwName[260];
    HWND dwHandle;
    HWND self;
    FILETIME time;
    int editorOffset;
    int dwLineNo;
    int jumplistLineno;
    int lineNumberDigits;
    DWORD temp;
    unsigned timerId;
    int fileFlags;
    int logMRU : 1;
    int newFile : 1;
    int timing : 1;
    int uninstantiated : 1;
    int inSaveDialog : 1;
    int deferClose : 1;
    int shouldClose : 1;
    int active : 1;
} DWINFO;

typedef struct hdwebkpt
{
    char name[256];
    DWORD address;
    BYTE mode;
    BYTE size;
    BOOL active;
    BOOL disable;
} HDWEBKPT;

typedef struct _databreak_
{
    struct _databreak_* next;
    char name[256];
    BOOL active;
    BOOL disable;
    DWORD address;
    DWORD size;
} DATABREAK;

#define MAX_BROWSE 50

typedef struct s_FileBrowse
{
    struct s_FileBrowse *next, *prev;
    DWINFO* info;
    int lineno;
} FILEBROWSE;

#define EDIT_PROPERTY_SHEET_COUNT 4
#define EDITPROP 0
#define PRINTFORMATPROP 1
#define GENERALPROP 2
#define INSTALLPROP 3

#define F_DOWN 1
#define F_WHOLEWORD 2
#define F_MATCHCASE 4
#define F_REGULAR 8
#define F_WILDCARD 16
#define F_OUTPUT1 32
#define F_OUTPUT2 64
#define F_OPENWINDOW 128
#define F_REPLACEALL 256
#define F_SUBDIR 512
#define F_ALL 1024

#define F_M_CURRENTDOCUMENT 0
#define F_M_OPENDOCUMENTS 1
#define F_M_CURRENTPROJECT 2
#define F_M_ALLPROJECTS 3
#define F_M_INCLUDEDIR 4
#define F_M_SPECIFIEDPATH 5
#define F_M_SELECTION 6
#define F_M_MAX 7

// find dialog
typedef struct
{
    HWND hwndTab;      // tab control
    HWND hwndDisplay;  // current child dialog box
    RECT rcDisplay;
    HBITMAP plusBmp;
    HBITMAP minusBmp;
    int findOffs;
    int replaceOffs;
    int iSel;
    int findEnabled;
    DLGTEMPLATE* apRes[8 + 4];
} DLGHDR;

typedef struct
{
    char file[MAX_PATH];
    char error[512];
    int lineno;
    int isWarning;
} ERRWNDDATA;

struct _propsData
{
    char* title;
    int protocount;
    PROFILE** prototype;
    PROJECTITEM* saveTo;
};

#define MAKEHASH_MAX 32
typedef struct hashTable
{
    struct hashTable* next;
    PROJECTITEM* pj;
    NameValuePair* pairs[MAKEHASH_MAX];
} HashTable;

typedef struct
{
    int ctl;
    HWND hwnd;
    RECT r;
    RECT hr;
    RECT display;
} DOCK_STR;

typedef struct _browselist
{
    struct _browselist* next;
    char name[512];
    char file[MAX_PATH];
    int line;
    int id;
    int definition : 1;
    int declaration : 1;
} BROWSELIST;

#include "rc.h"
#include "xml.h"
#include "ocide.p"

#ifndef LVN_BEGINSCROLL
#    define LVN_BEGINSCROLL (LVN_FIRST - 80)
#    define LVN_ENDSCROLL (LVN_FIRST - 81)
typedef struct tagNMLVSCROLL
{
    NMHDR hdr;
    int dx;
    int dy;
} NMLVSCROLL, *LPNMLVSCROLL;
#endif

#if defined(BORLAND_BCC55) && !defined(__ORANGEC__)
#    define UNNAMED_UNION u.
#else
#    define UNNAMED_UNION
#endif

#ifndef TTM_POPUP
#    define TTM_POPUP 34
#endif

#ifdef XXXXX
#    define free(x) my_free(x)
#    define calloc(x, y) my_calloc(x, y)
#    define malloc(x, y) my_malloc(x, y)
#    define realloc(x, n) my_realloc(x, n)
#    define strdup(s) my_strdup(s)
#endif

#ifdef OPENWATCOM
#    define LWA_COLORKEY 1
#    define LWA_ALPHA 2
#    define WS_EX_LAYERED 0x80000
#    define WS_EX_NOACTIVATE 0x8000000
#    define WM_NCMOUSEHOVER 0x2a0
#    define WM_NCMOUSELEAVE 0x2a2
#    define OFN_DONTADDTORECENT (33554432)
#    define ASFW_ANY ((DWORD)-1)
#    define BTNS_SEP TBSTYLE_SEP
#    define TTS_BALLOON 0x40
#    define LVS_EX_LABELTIP 0x4000
#    define FR_PRIVATE 0x10
#    define MIIM_BITMAP 0x80
#    define MIIM_STRING 0x40
typedef struct tagMENUITEMINFOAxxx
{
    UINT cbSize;
    UINT fMask;
    UINT fType;
    UINT fState;
    UINT wID;
    HMENU hSubMenu;
    HBITMAP hbmpChecked;
    HBITMAP hbmpUnchecked;
    DWORD dwItemData;
    LPSTR dwTypeData;
    UINT cch;
    HBITMAP hbmpItem;
} MENUITEMINFOAxxx, *LPMENUITEMINFOAxxx;
#    define MENUITEMINFO MENUITEMINFOAxxx
#    define LPMENUITEMINFO LPMENUITEMINFOAxxx
typedef struct tagNMTOOLBARAxxx
{
    NMHDR hdr;
    int iItem;
    TBBUTTON tbButton;
    int cchText;
    LPSTR pszText;
    RECT rcButton;
} NMTOOLBARAxxx, *LPNMTOOLBARAxxx;
#    define NMTOOLBAR NMTOOLBARAxxx
#    define LPNMTOOLBAR LPNMTOOLBARAxxx
int WINAPI AddFontResourceExA(LPCSTR, DWORD, PVOID);
#    define AddFontResourceEx AddFontResourceExA
BOOL WINAPI SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
BOOL WINAPI AllowSetForegroundWindow(DWORD dwProcessId);
#endif
#if defined(GNUC) && !defined(__MINGW64__) && !defined(__clang__)
#    pragma pack(push, 1)
typedef struct tagTVKEYDOWN
{
    NMHDR hdr;
    WORD wVKey;
    UINT flags;
} NMTVKEYDOWN, *LPNMTVKEYDOWN;
#    pragma pack(pop)
typedef struct tagNMTTCUSTOMDRAW
{
    NMCUSTOMDRAW nmcd;
    UINT uDrawFlags;
} NMTTCUSTOMDRAW, *LPNMTTCUSTOMDRAW;
#    define STATUS_FLOAT_MULTIPLE_FAULTS ((DWORD)0xC00002B4L)
#    define STATUS_FLOAT_MULTIPLE_TRAPS ((DWORD)0xC00002B5L)
#    define STATUS_ILLEGAL_VLM_REFERENCE ((DWORD)0xC00002C0L)
#    define STATUS_REG_NAT_CONSUMPTION ((DWORD)0xC00002C9L)

#    define MCW_EM 0x0008001F

#endif
#ifndef LVS_EX_DOUBLEBUFFER
#    define LVS_EX_DOUBLEBUFFER 0x10000
#    define TVS_EX_DOUBLEBUFFER 4
#    define TVM_SETEXTENDEDSTYLE (TV_FIRST + 44)
#    define TreeView_SetExtendedStyle(hwnd, dw, mask) SendMessage((hwnd), TVM_SETEXTENDEDSTYLE, mask, dw)
#endif

#ifdef OPENWATCOM
#    undef TreeView_GetItemRect
#    define TreeView_GetItemRect(hwnd, hitem, prc, code) \
        (*(HTREEITEM FAR*)prc = (hitem), (BOOL)SendMessage((hwnd), TVM_GETITEMRECT, (WPARAM)(code), (LPARAM)(RECT FAR*)(prc)))
#endif

#if defined(__MINGW64__)
#    define Eax Rax
#    define Ebx Rbx
#    define Ecx Rdx
#    define Edx Rcx
#    define Esp Rsp
#    define Esi Rsi
#    define Edi Rdi
#    define Ebp Rbp
#    define Eip Rip
#    define GWL_WNDPROC GWLP_WNDPROC
#    define GWL_HWNDPARENT GWLP_HWNDPARENT
#    define GWL_USERDATA GWLP_USERDATA
#    define GWL_HINSTANCE GWLP_HINSTANCE
#    define DWL_MSGRESULT DWLP_MSGRESULT
#endif

#pragma pack()
