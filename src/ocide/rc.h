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

#ifndef __RC_H
#define __RC_H
#include <windows.h>
#define ESC_A (007)
#define ESC_B (010)
#define ESC_F (014)
#define ESC_N (012)
#define ESC_R (015)
#define ESC_T (011)
#define ESC_V (013)

typedef struct _list
{
    struct _list* next;
    void* data;
} LIST;

enum e_sym
{
    /* first comes all the C++ overloadable operators */
    /* be sure to change expectlist if you insert things */
    kw_new,
    kw_delete,
    plus,
    minus,
    star,
    divide,
    lshift,
    rshift,
    modop,
    eq,
    neq,
    lt,
    leq,
    gt,
    geq,
    assign,
    asplus,
    asminus,
    astimes,
    asdivide,
    asmodop,
    aslshift,
    asrshift,
    asand,
    asor,
    asxor,
    autoinc,
    autodec,
    openbr,
    openpa,
    pointstar,
    pointsto,
    lor,
    land,
    not,
    or
    ,
    and,
    uparrow,
    compl,
    /* then generic stuff that isn't overloadable or is internal */
    ident,
    cconst,
    iconst,
    lconst,
    iuconst,
    luconst,
    sconst,
    lsconst,
    fconst,
    lrconst,
    rconst,
    hook,
    colon,
    begin,
    end,
    dot,
    dostar,
    closebr,
    closepa,
    ellipse,
    comma,
    semicolon,
    classsel,
    classselstar,
    dotstar,
    backslash,
    /* rc DEFINITIONS */
    kw_accelerator,
    kw_alt,
    kw_ascii,
    kw_auto3state,
    kw_autocheckbox,
    kw_autoradiobutton,
    kw_begin,
    kw_bitmap,
    kw_block,
    kw_caption,
    kw_characteristics,
    kw_checkbox,
    kw_checked,
    kw_class,
    kw_combobox,
    kw_control,
    kw_ctext,
    kw_cursor,
    kw_defpushbutton,
    kw_dialog,
    kw_discardable,
    kw_dlginclude,
    kw_edittext,
    kw_end,
    kw_exstyle,
    kw_filedate,
    kw_fileflagmask,
    kw_fileflags,
    kw_fileos,
    kw_filesubtype,
    kw_filetype,
    kw_fileversion,
    kw_fixed,
    kw_font,
    kw_grayed,
    kw_groupbox,
    kw_help,
    kw_icon,
    kw_impure,
    kw_inactive,
    kw_language,
    kw_listbox,
    kw_loadoncall,
    kw_ltext,
    kw_menu,
    kw_menubarbreak,
    kw_menubreak,
    kw_menuitem,
    kw_messagetable,
    kw_moveable,
    kw_nondiscardable,
    kw_not,
    kw_popup,
    kw_preload,
    kw_productversion,
    kw_pushbutton,
    kw_radiobutton,
    kw_rcdata,
    kw_rcinclude,
    kw_rtext,
    kw_scrollbar,
    kw_separator,
    kw_shift,
    kw_state3,
    kw_stringtable,
    kw_style,
    kw_value,
    kw_version,
    kw_versioninfo,
    kw_virtkey,
    kw_weight,
    kw_italic,
    kw_dialogex,
    kw_menuex,
    kw_pure,
    kw_noinvert,
    /* Finally tokens for keywords */
    //	kw_int, kw_void, kw_char, kw_float, kw_double, kw_struct, kw_union,
    //        kw_long, kw_short, kw_unsigned, kw_signed, kw_auto, kw_extern,
    //        kw_register, kw_typedef, kw_static, kw_goto, kw_return,
    //        kw_sizeof, kw_break, kw_continue, kw_if, kw_else, kw_for,
    //        kw_do, kw_while, kw_switch, kw_case, kw_default, kw_enum, kw_volatile,
    //	kw_const, kw_phitext, kw__trap, kw__interrupt, kw__abs, kw__genword,
    /* C++ */
    //	kw_public, kw_private, kw_protected, kw_class, kw_friend,
    //	kw_namespace, kw_this, kw_operator, kw_inline, kw_virtual,
    //	kw_try,kw_throw,kw_catch, kw_template, kw_bool, kw_true, kw_false,
    rceol,
    rceof
};

/* Global symbol table is a hash table */
#define HASHTABLESIZE 1023

/* #define tracking */
typedef struct
{
    WCHAR* string;
    short argcount;
    WCHAR** args;
} DEFSTRUCT;

/* struct for preprocessor if tracking */
typedef struct ifstruct
{
    struct ifstruct* link; /* link */
    short iflevel;
    short elsetaken;
} IFSTRUCT;

enum err
{
    ERR_PREPROCMATCH,
    ERR_COMMENTMATCH,
    ERR_NEEDCHAR,
    ERR_CONSTTOOLARGE,
    ERR_FPCON,
    ERR_CHAR4CHAR,
    ERR_ILLCHAR,
    ERR_IDEXPECT,
    ERR_PREPROCID,
    ERR_ERROR,
    ERR_USERERR,
    ERR_USERWARN,
    ERR_PREPIG,
    ERR_UPDOWN,
    ERR_INCLFILE,
    ERR_CANTOPEN,
    ERR_PUNCT,
    ERR_MACROSUBS,
    ERR_WRONGMACROARGS,
    ERR_UNEXPECT,
    ERR_INSERT,
    ERR_NEEDCONST,
    ERR_UNDEFINED,
    ERR_DUPSYM,
    ERR_IDENTEXPECT,
    ERR_EXPREXPECT,
    ERR_INTERP,
    ERR_STRINGTOOBIG,
    ERR_INVALIDSTRING,
    ERR_EXTRA_DATA_ON_LINE,
    ERR_BEGIN_EXPECTED,
    ERR_END_EXPECTED,
    ERR_RESOURCE_ID_EXPECTED,
    ERR_STRING_EXPECTED,
    ERR_ACCELERATOR_CONSTANT_EXPECTED,
    ERR_NO_ASCII_VIRTKEY,
    ERR_NOT_DIALOGEX,
    ERR_UNKNOWN_DIALOG_CONTROL_CLASS,
    ERR_VERSIONINFO_TYPE_1,
    ERR_UNKNOWN_RESOURCE_TYPE,
    ERR_INVALIDCLASS,
    ERR_FIXEDDATAEXPECTED,
    ERR_BLOCK_EXPECTED,
    ERR_INVALID_VERSION_INFO_TYPE,
};
struct stab
{
    struct sym *head, *tail;
};

/* symbols */
struct sym
{
    struct sym* next; /* next symbol (local tabs only) */
    char* name;       /* symbol name */
    struct sym* xref; /* next symbol by definition in the file */
    LIST* lines;
    int marked : 1;
    /* these fields depend on storage_class */
    union
    {
        long i;          /* int val */
        unsigned long u; /* nsigned val */
        long double f;   /* float val */
        char* s;         /* string val */
    } value;
};
/* error list */
struct errl
{
    struct errl* link;
    short errornumber;
    void* data;
};
typedef struct stab TABLE;
typedef struct sym SYM;
#define ERRORS struct errl
#define BALANCE struct balance
#define BAL_PAREN 0
#define BAL_BRACKET 0
#define ERRORS struct errl

struct balance
{
    struct balance* back;
    short type;
    short count;
};
typedef struct kwblk
{
    struct kwblk* next;
    char* word;
    short stype;
    enum
    {
        KW_CPLUSPLUS = 1,
        KW_INLINEASM = 2,
        KW_NONANSI = 4
    } flags;
} KEYWORDS;

#define INPUT_BUFFER_LEN 32768

typedef struct _hashrec_
{
    struct _hashrec_* link; /* Link to next element in list */
    char* key;              /* Full key */
} HASHREC;

typedef struct expression
{
#define e_int 0
    int type;
    int val;
    char* rendition;
    struct expression* left;
    struct expression* right;
} EXPRESSION;

typedef struct
{
    int symbolic;
    WCHAR* origName;
    union
    {
        struct
        {
            int length;
            WCHAR* symbol;
        } n;
        EXPRESSION* id;
    } u;
} IDENT;

typedef struct rcdata
{
    struct rcdata* next;
    enum
    {
        RCDATA_WORD,
        RCDATA_DWORD,
        RCDATA_STRING,
        RCDATA_WSTRING,
        RCDATA_BUFFER
    } type;
    union
    {
        unsigned word;
        unsigned dword;
        struct
        {
            int length;
            BYTE* s;
        } string;
        struct
        {
            int length;
            WCHAR* w;
        } wstring;
        struct
        {
            int length;
            BYTE* data;
        } buffer;
    } u;
} RCDATA;

enum
{
    CTL_BUTTON = 0x80,
    CTL_EDIT,
    CTL_STATIC,
    CTL_LISTBOX,
    CTL_SCROLLBAR,
    CTL_COMBOBOX
};

typedef struct control
{
    struct control* next;
    LIST* prevLines;
    int generic;
    IDENT class;
    IDENT* text;
    EXPRESSION* id;
    EXPRESSION* style;
    int baseStyle;
    EXPRESSION* exstyle;
    EXPRESSION* x;
    EXPRESSION* y;
    EXPRESSION* width;
    EXPRESSION* height;
    EXPRESSION* help;
    RCDATA* data;
    int deleteme : 1;
    int isCombo : 1;
} CONTROL;

typedef struct
{
    EXPRESSION* style;
    EXPRESSION* exstyle;
    EXPRESSION* x;
    EXPRESSION* y;
    EXPRESSION* width;
    EXPRESSION* height;
    IDENT* menu;
    IDENT* class;
    WCHAR* caption;
    EXPRESSION* pointsize;
    WCHAR* font;
    struct
    {
        EXPRESSION* help;
        EXPRESSION* weight;
        EXPRESSION* italic;
        EXPRESSION* charset;
    } ex;
    CONTROL* controls;
} DIALOG;

typedef struct menuitem
{
    struct menuitem* next;
    struct menuitem* popup;
    LIST* prevLines;
    EXPRESSION* type; /* MENUEX */
    enum
    {
        MI_GRAYED = 1,
        MI_INACTIVE = 2,
        MI_BITMAP = 4,
        MI_CHECKED = 8,
        MI_POPUP = 16,
        MI_MENUBARBREAK = 32,
        MI_MENUBREAK = 64,
        MI_ENDMENU = 128,
        MI_OWNERDRAW = 256,
        MI_SEPARATOR = 0x800,
        MI_HELP = 0x4000
    } flags; /* MENU */
    EXPRESSION* state;
    EXPRESSION* id;
    EXPRESSION* help;
    WCHAR* text;
    BOOL expanded;  // for the GUI
} MENUITEM;
typedef struct
{
    MENUITEM* items;
    EXPRESSION* help;
} MENU;

typedef struct accelerator
{
    struct accelerator* next;
    enum
    {
        ACC_VIRTKEY = 1,
        ACC_NOINVERT = 2,
        ACC_SHIFT = 4,
        ACC_CONTROL = 8,
        ACC_ALT = 16,
        ACC_LAST = 128
    } flags;
    EXPRESSION* key;
    WCHAR skey;
    EXPRESSION* id;
} ACCELERATOR;

typedef struct
{
    int headerSize;
    BYTE* headerData;
    int pixelSize;
    BYTE* pixelData;
} BITMAP_;

typedef struct cursordata
{
    int width;
    int height;
    int colorcount;
    int xhotspot;
    int yhotspot;
    int length;
    int bytes;
    int offset;
    int xtra;
    BYTE* data;
} CURSORDATA;

typedef struct cursor
{
    CURSORDATA* cursors;
    int count;
} CURSOR;
typedef struct
{
    char* device;
    char* face;
    int length;
    BYTE* data;
} FONT;

typedef struct icondata
{
    struct icon* next;
    char width;
    char height;
    char colorcount;
    int planes;
    int bits;
    int bytes;
    int index;
    int offset;
    int xtra;
    BYTE* data;
} ICONDATA;
typedef struct icon
{
    ICONDATA* icons;
    int count;
} ICON;

typedef struct strings
{
    struct strings* next;
    EXPRESSION* id;
    int length;
    WCHAR* string;
} STRINGS;

typedef struct
{
    struct fixed
    {
        int file_version_ms;
        int file_version_ls;
        int product_version_ms;
        int product_version_ls;
        EXPRESSION* file_flags_mask;
        EXPRESSION* file_flags;
        EXPRESSION* file_os;
        EXPRESSION* file_type;
        EXPRESSION* file_subtype;
        int file_date_ms;
        int file_date_ls;
    } * fixed;
    struct variable
    {
        struct variable* next;
        enum
        {
            VERINFO_STRING,
            VERINFO_VAR
        } type;
        union
        {
            struct
            {
                WCHAR* language;
                struct ver_stringinfo
                {
                    struct ver_stringinfo* next;
                    WCHAR* key;
                    WCHAR* value;
                    int length;
                } * strings;
            } string;
            struct
            {
                struct ver_varinfo
                {
                    WCHAR* key;
                    struct ver_varinfo* next;
                    struct ver_varlangchar
                    {
                        struct ver_varlangchar* next;
                        EXPRESSION* language;
                        EXPRESSION* charset;
                    } * intident;
                } * var;
            } var;
        } u;
    } * var;
} VERSIONINFO;

typedef struct
{
    EXPRESSION *language_high, *language_low;
    EXPRESSION* characteristics;
    EXPRESSION* version;
    enum
    {
        MF_MOVEABLE = 0x10,
        MF_PURE = 0x20,
        MF_PRELOAD = 0x40,
        MF_DISCARDABLE = 0x1000,
        MF_NONDISCARDABLE = 1,
        MF_IMPURE = 2,
    } memflags;
} CHARACTERISTICS;

enum resType
{
    RESTYPE_PLACEHOLDER,
    RESTYPE_ACCELERATOR,
    RESTYPE_BITMAP,
    RESTYPE_CURSOR,
    RESTYPE_DIALOG,
    RESTYPE_DLGINCLUDE,
    RESTYPE_FONT,
    RESTYPE_ICON,
    RESTYPE_LANGUAGE,
    RESTYPE_MENU,
    RESTYPE_MESSAGETABLE,
    RESTYPE_RCDATA,
    RESTYPE_STRING,
    RESTYPE_USERDATA,
    RESTYPE_VERSION,
};

typedef struct _resource
{
    struct _resource* next;
    int changed;
    int deleted;
    int itype;
    BOOL extended; /* MENUEX, DIALOGEX */
    IDENT type;
    IDENT id;
    char* filename;
    LIST* prevLines;
    union
    {
        struct
        {
            int length;
            const BYTE* data;
        } data;
        ACCELERATOR* accelerator;
        BITMAP_* bitmap;
        CURSOR* cursor;
        DIALOG* dialog;
        ICON* icon;
        FONT* font;
        MENU* menu;
        RCDATA* rcdata;
        STRINGS* stringtable;
        VERSIONINFO* versioninfo;
    } u;
    CHARACTERISTICS info;
} RESOURCE;

typedef struct
{
    HANDLE memHeap;
    RESOURCE* resources;
    char* resourceIdFile;
    TABLE syms;
    SYM* headerDefinitions;
    SYM* newDefinitions;
    int nextStringId;
    int nextControlId;
    int nextMenuId;
    int nextResourceId;
    int fileFlags;
} RESOURCE_DATA;

#include "rcgui.h"
#include "rc.p"

#endif