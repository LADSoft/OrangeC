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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"
#include <setjmp.h>

EXPRESSION* ReadExp(void);
EXPRESSION* ReadExpOr(EXPRESSION*);
int Eval(EXPRESSION*);

extern char* errfile;
extern SYM* rcDefs;
extern int cantnewline;
extern WCHAR* lptr;
extern int laststrlen;
extern char* rcSearchPath;
extern enum e_sym lastst;
extern char lastid[];
extern char laststr[];
extern int lastch;
extern int ival;
extern char* infile;
extern int lineno;
extern int incconst;
extern TABLE defsyms;
/* We read the directory entries in a cursor or icon file into
instances of this structure.  */

char* rcIdFile;
char *inputBuffer, *ibufPtr;
int inputLength;
int rc_lineno;

jmp_buf errjump;

typedef struct compileData
{
    RESOURCE* resources;
    RESOURCE** resourcesTail;
    LIST* lines;
    EXPRESSION *base_language_high, *base_language_low;
} COMPILEDATA;

static HANDLE memHeap;

void* rcAlloc(int v)
{
    void* rv;
    if (!memHeap)
        memHeap = HeapCreate(0, 0, 0);
    rv = HeapAlloc(memHeap, HEAP_ZERO_MEMORY, v);
    if (!rv)
        fatal("out of memory");
    return rv;
}
char* rcStrdup(const char* s)
{
    char* v = rcAlloc(strlen(s) + 1);
    if (v)
        strcpy(v, s);
    return v;
}
void rcFree(void* p)
{
    HeapFree(memHeap, 0, p);  // fixme
}
#define GetByte(fil) fgetc(fil)

static int GetWord(FILE* fil)
{
    int b1, b2;

    b1 = GetByte(fil);
    b2 = GetByte(fil);
    return ((b2 & 0xff) << 8) | (b1 & 0xff);
}
static unsigned int GetLong(FILE* fil)
{
    int b1, b2, b3, b4;

    b1 = GetByte(fil);
    b2 = GetByte(fil);
    b3 = GetByte(fil);
    b4 = GetByte(fil);
    return (((((((b4 & 0xff) << 8) | (b3 & 0xff)) << 8) | (b2 & 0xff)) << 8) | (b1 & 0xff));
}

//-------------------------------------------------------------------------

static int GetData(FILE* fil, BYTE* p, int c)
{
    unsigned int got;

    got = fread(p, 1, c, fil);
    return (got == c);
}

static int CompareType(IDENT* id1, IDENT* id2)
{
    if (id1->symbolic != id2->symbolic)
        return FALSE;
    if (!id1->symbolic)
    {
        return id1->u.id == id2->u.id;
    }
    else
    {
        if (id1->u.n.length != id2->u.n.length)
            return FALSE;
        return !memcmp(id1->u.n.symbol, id2->u.n.symbol, id1->u.n.length * sizeof(WCHAR) + sizeof(WCHAR));
    }
}
static void AddResource(COMPILEDATA* cd, IDENT* specType, RESOURCE* r, IDENT* id, EXPRESSION* language_high,
                        EXPRESSION* language_low, char* fileName)
{
    IDENT type;
    if ((int)specType < 100)
    {
        type.symbolic = 0;
        type.u.id = rcAlloc(sizeof(EXPRESSION));
        type.u.id->type = e_int;
        type.u.id->val = (int)specType;
    }
    else
    {
        type = *specType;
    }
    r->next = NULL;
    r->type = type;
    r->id = *id;
    r->info.language_high = language_high;
    r->info.language_low = language_low;
    if (fileName)
        r->filename = rcStrdup(fileName);
    r->prevLines = cd->lines;
    *(cd->resourcesTail) = r;
    cd->resourcesTail = &r->next;
}
static void CreatePlaceholderResource(COMPILEDATA* cd)
{
    IDENT id;
    RESOURCE* r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_PLACEHOLDER;
    memset(&id, 0, sizeof(id));
    AddResource(cd, (IDENT*)RESTYPE_PLACEHOLDER, r, &id, cd->base_language_high, cd->base_language_low, 0);
}
static void CreateLanguageResource(COMPILEDATA* cd)
{
    IDENT id;
    RESOURCE* r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_LANGUAGE;
    memset(&id, 0, sizeof(id));
    AddResource(cd, (IDENT*)RESTYPE_LANGUAGE, r, &id, cd->base_language_high, cd->base_language_low, 0);
}
//-------------------------------------------------------------------------

static void CreateAcceleratorResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, ACCELERATOR* data)
{
    RESOURCE* r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_ACCELERATOR;
    r->u.accelerator = data;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_ACCELERATOR, r, id, resinfo->language_high, resinfo->language_low, 0);
}

#define BITMAP_SKIP (14)
static int filesize(FILE* fil)
{
    int rv;
    fseek(fil, 0L, SEEK_END);
    rv = ftell(fil);
    fseek(fil, 0L, SEEK_SET);
    return rv;
}

//-------------------------------------------------------------------------

BITMAP_* RCLoadBitmap(char* fileName)
{
    BITMAP_* bd;
    FILE* fil;
    char buf[256];
    BYTE *headerData, *pixelData;
    int size;
    int pixelOffs;

    fil = MySearchPath(fileName, rcSearchPath, "rb");
    if (!fil)
        fatal("File Not Found  %s in line %d", fileName, lineno);

    size = filesize(fil);

    fread(buf, 1, BITMAP_SKIP, fil);
    if (buf[0] != 'B' || buf[1] != 'M' || *(int*)(buf + 2) != size)
        fatal("%s is not a bitmap file", fileName);

    pixelOffs = *(int*)(buf + 10);

    headerData = rcAlloc(pixelOffs - BITMAP_SKIP);
    pixelData = rcAlloc(size - pixelOffs);

    fseek(fil, (long)BITMAP_SKIP, SEEK_SET);

    if (!GetData(fil, headerData, pixelOffs - BITMAP_SKIP))
    {
        fatal("Data file too short");
    }
    if (!GetData(fil, pixelData, size - pixelOffs))
    {
        fatal("Data file too short");
    }
    bd = rcAlloc(sizeof(BITMAP_));
    bd->headerData = headerData;
    bd->pixelData = pixelData;
    bd->headerSize = pixelOffs - BITMAP_SKIP;
    bd->pixelSize = size - pixelOffs;

    fclose(fil);
    return bd;
}
static void CreateBitmapResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, char* fileName)
{
    BITMAP_* bd = RCLoadBitmap(fileName);
    RESOURCE* r;

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_BITMAP;
    r->u.bitmap = bd;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_BITMAP, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

CURSOR* RCLoadCursor(char* fileName)
{
    FILE* fil;
    int type, count, fill, i;
    CURSOR* cursor;
    CURSORDATA* cursors;
    fil = MySearchPath(fileName, rcSearchPath, "rb");
    if (!fil)
        fatal("File Not Found  %s in line %d", fileName, lineno);

    fill = GetWord(fil);
    type = GetWord(fil);
    count = GetWord(fil);
    if (type != 2)
        fatal("cursor file `%s' does not contain cursor data", fileName);

    cursor = rcAlloc(sizeof(CURSOR));
    cursors = rcAlloc(sizeof(CURSORDATA) * (count));
    cursor->cursors = cursors;
    cursor->count = count;
    for (i = 0; i < count; i++)
    {
        int t;
        cursors[i].width = GetByte(fil);
        cursors[i].height = GetByte(fil);
        cursors[i].colorcount = GetByte(fil);
        cursors[i].xtra = GetByte(fil);
        cursors[i].xhotspot = GetWord(fil);
        cursors[i].yhotspot = GetWord(fil);
        cursors[i].bytes = GetLong(fil);
        cursors[i].offset = GetLong(fil);

        if (!cursors[i].width)
            cursors[i].width = 32;
        if (!cursors[i].height)
            cursors[i].height = (cursors[i].bytes - 48) / ((32 / 8) * 2);
        t = ftell(fil);
        if (fseek(fil, cursors[i].offset, SEEK_SET) != 0)
        {
            fatal("file i/o error on %s", fileName);
        }
        cursors[i].data = rcAlloc(cursors[i].bytes);

        if (!GetData(fil, cursors[i].data, cursors[i].bytes))
        {
            fatal("Data file too short");
        }

        fseek(fil, t, SEEK_SET);
        if (feof(fil))
        {
            fatal("Data too short in %s", fileName);
        }
    }
    fclose(fil);
    return cursor;
}
static void CreateCursorResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, char* fileName)
{
    CURSOR* cursor = RCLoadCursor(fileName);
    RESOURCE* r;
    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_CURSOR;
    r->u.cursor = cursor;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_CURSOR, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

static void CreateDialogResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, DIALOG* dialog, BOOL extended)
{
    DIALOG* copy;
    RESOURCE* r;

    copy = rcAlloc(sizeof *copy);
    *copy = *dialog;

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_DIALOG;
    r->extended = extended;
    r->u.dialog = copy;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_DIALOG, r, id, resinfo->language_high, resinfo->language_low, 0);
}
int StringAsciiToWChar(WCHAR** text, char* string, int len)
{
    int i = 0;
    *text = rcAlloc(len * sizeof(WCHAR) + sizeof(WCHAR));
    while (len--)
    {
        (*text)[i++] = (WCHAR) * (unsigned char*)string++;
    }
    (*text)[i++] = 0;
    return (i - 1);
}

//-------------------------------------------------------------------------

static void IdentFromString(IDENT* val, char* string)
{
    val->symbolic = 1;
    val->u.n.length = StringAsciiToWChar(&val->u.n.symbol, string, strlen(string));
}

//-------------------------------------------------------------------------

static void CreateDlgIncludeResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, char* fileName)
{
    char* data;
    RESOURCE* r;
    int size;

    data = rcAlloc(size = strlen(fileName) + 1);
    strcpy(data, fileName);

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_DLGINCLUDE;
    r->u.data.length = size;
    r->u.data.data = (BYTE*)data;
    r->info = *resinfo;

    AddResource(cd, (IDENT*)RESTYPE_DLGINCLUDE, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

FONT* RCLoadFont(char* fileName)
{
    FILE* fil;
    BYTE* data;
    int offset;
    const char *device, *face;
    int size;
    FONT* fd;

    fil = MySearchPath(fileName, rcSearchPath, "rb");
    if (!fil)
        fatal("File Not Found  %s in line %d", fileName, lineno);

    size = filesize(fil);

    data = rcAlloc(size);

    if (!GetData(fil, data, size))
    {
        fatal("Data file too short");
    }

    fclose(fil);

    offset = ((((((data[47] << 8) | data[46]) << 8) | data[45]) << 8) | data[44]);
    if (offset > 0 && offset < size)
        device = (char*)data + offset;
    else
        device = "";

    offset = ((((((data[51] << 8) | data[50]) << 8) | data[49]) << 8) | data[48]);
    if (offset > 0 && offset < size)
        face = (char*)data + offset;
    else
        face = "";

    fd = rcAlloc(sizeof *fd);
    fd->length = size;
    fd->data = data;
    fd->device = rcStrdup(device);
    fd->face = rcStrdup(face);
    return fd;
}
static void CreateFontResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, char* fileName)
{
    RESOURCE* r;
    FONT* fd = RCLoadFont(fileName);

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_FONT;
    r->u.data.length = fd->length;
    r->u.font = fd;
    r->info = *resinfo;

    AddResource(cd, (IDENT*)RESTYPE_FONT, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

ICON* RCLoadIcon(char* fileName)
{
    FILE* fil;
    int type, count, i;
    ICONDATA* icons;
    ICON* icon;

    fil = MySearchPath(fileName, rcSearchPath, "rb");
    if (!fil)
        fatal("File Not Found  %s in line %d", fileName, lineno);

    GetWord(fil);
    type = GetWord(fil);
    count = GetWord(fil);
    if (type != 1)
        fatal("icon file `%s' does not contain icon data", fileName);

    /* Read in the icon directory entries.  */

    icons = rcAlloc(sizeof(ICONDATA) * (count + 1));
    icon = rcAlloc(sizeof(ICON));
    icon->icons = icons;
    icon->count = count;
    for (i = 0; i < count; i++)
    {
        int t;
        icons[i].width = GetByte(fil);
        icons[i].height = GetByte(fil);
        icons[i].colorcount = GetByte(fil);
        icons[i].xtra = GetByte(fil);
        icons[i].planes = GetWord(fil);
        icons[i].bits = GetWord(fil);
        icons[i].bytes = GetLong(fil);
        icons[i].offset = GetLong(fil);

        t = ftell(fil);
        if (fseek(fil, icons[i].offset, SEEK_SET) != 0)
        {
            fatal("file i/o error on %s", fileName);
        }

        icons[i].data = rcAlloc(icons[i].bytes);

        if (!GetData(fil, icons[i].data, icons[i].bytes))
        {
            fatal("Data file too short");
        }

        fseek(fil, t, SEEK_SET);
        if (feof(fil))
            fatal("Data too short in %s", fileName);
    }
    fclose(fil);
    return icon;
}
static void CreateIconResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, char* fileName)
{
    ICON* icon = RCLoadIcon(fileName);
    RESOURCE* r;

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_ICON;
    r->u.icon = icon;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_ICON, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

static void CreateMenuResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, MENUITEM* menuitems, int extended)
{
    MENU* m;
    RESOURCE* r;

    m = rcAlloc(sizeof *m);
    m->items = menuitems;
    m->help = 0;

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_MENU;
    r->extended = extended;
    r->u.menu = m;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_MENU, r, id, resinfo->language_high, resinfo->language_low, 0);
}

BYTE* RCLoadMessageTable(char* fileName, int* size)
{
    FILE* fil;
    BYTE* data;

    fil = MySearchPath(fileName, rcSearchPath, "rb");
    if (!fil)
        fatal("File Not Found  %s in line %d", fileName, lineno);

    *size = filesize(fil);

    data = rcAlloc(*size);

    if (!GetData(fil, data, *size))
    {
        fatal("Data file too short");
    }

    fclose(fil);
    return data;
}

static void CreateMessageTableResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, char* fileName)
{
    BYTE* data;
    RESOURCE* r;
    int size;

    data = RCLoadMessageTable(fileName, &size);
    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_MESSAGETABLE;
    r->u.data.length = size;
    r->u.data.data = data;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_MESSAGETABLE, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

static void CreateRCDataResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, RCDATA* data)
{
    RESOURCE* r;

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_RCDATA;
    r->u.rcdata = data;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_RCDATA, r, id, resinfo->language_high, resinfo->language_low, 0);
}

static RCDATA* CreateRCDataResourceString(char* string, int len)
{
    RCDATA* ri;
    char* s;

    ri = rcAlloc(sizeof *ri);
    ri->next = NULL;
    ri->type = RCDATA_STRING;
    ri->u.string.length = len;
    s = rcAlloc(len);
    memcpy(s, string, len);
    ri->u.string.s = (BYTE*)s;

    return ri;
}

static RCDATA* CreateRCDataResourceNumber(int val, int dword)
{
    RCDATA* ri;

    ri = rcAlloc(sizeof *ri);
    ri->next = NULL;
    ri->type = dword ? RCDATA_DWORD : RCDATA_WORD;
    ri->u.dword = val;

    return ri;
}

static void CreateStringTableResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* resinfo, STRINGS* strings)
{
    RESOURCE* r;
    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_STRING;
    r->u.stringtable = strings;
    r->info = *resinfo;
    AddResource(cd, (IDENT*)RESTYPE_STRING, r, id, resinfo->language_high, resinfo->language_low, 0);
}

static void CreateUserFileResource(COMPILEDATA* cd, IDENT* id, IDENT* type, CHARACTERISTICS* resinfo, char* fileName)
{
    FILE* fil;
    BYTE* data;
    RESOURCE* r;
    int size, xsize;

    fil = MySearchPath(fileName, rcSearchPath, "rb");
    if (!fil)
        fatal("File Not Found  %s in line %d", fileName, lineno);

    xsize = size = filesize(fil);
    //   if (size & 1)
    //      size++ ;

    data = rcAlloc(size);

    if (!GetData(fil, data, xsize))
    {
        fatal("Data file too short");
    }

    fclose(fil);

    r = rcAlloc(sizeof(*r));
    r->itype = RESTYPE_USERDATA;
    r->u.rcdata = rcAlloc(sizeof(RCDATA));
    r->u.rcdata->next = NULL;
    r->u.rcdata->type = RCDATA_BUFFER;
    r->u.rcdata->u.buffer.length = size;
    r->u.rcdata->u.buffer.data = data;
    r->info = *resinfo;
    AddResource(cd, type, r, id, resinfo->language_high, resinfo->language_low, fileName);
}

//-------------------------------------------------------------------------

static void CreateVersionInfoResource(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info, struct fixed* fixedverinfo,
                                      struct variable* verinfo)
{
    RESOURCE* r;

    r = rcAlloc(sizeof(RESOURCE));
    r->itype = RESTYPE_VERSION;
    r->u.versioninfo = rcAlloc(sizeof(VERSIONINFO));
    r->u.versioninfo->fixed = fixedverinfo;
    r->u.versioninfo->var = verinfo;
    r->info = *info;
    AddResource(cd, (IDENT*)RESTYPE_VERSION, r, id, info->language_high, info->language_low, 0);
}

//-------------------------------------------------------------------------

static void ReadResID(IDENT* id)
{
    int i;
    EXPRESSION* p;
    char buf[512];
    switch (lastst)
    {
        case ident:
            strcpy(buf, lastid);
            p = InternalLookup(lastid, FALSE);
            if (!p)
            {
                id->symbolic = 1;
                for (i = 0; i < strlen(buf); i++)
                    lastid[i] = toupper(buf[i]);
                id->u.n.length = StringAsciiToWChar(&id->u.n.symbol, buf, strlen(buf));
                getsym();
                break;
            }
            freeExpr(p);
            lastst = ident;
            id->symbolic = 0;
            id->u.id = ReadExp();
            StringAsciiToWChar(&id->origName, buf, strlen(buf));
            break;
        case iconst:
        case lconst:
        case iuconst:
        case luconst:
            id->symbolic = 0;
            id->u.id = ReadExp();
            break;
        default:
            generror(ERR_RESOURCE_ID_EXPECTED, 0);
            break;
    }
}

//-------------------------------------------------------------------------

static void ReadQuotedResID(IDENT* id)
{
    if (lastst == sconst)
    {
        id->symbolic = 1;
        id->u.n.length = StringAsciiToWChar(&id->u.n.symbol, laststr, laststrlen);
        getsym();
    }
    else if (lastst == ident)
    {
        WCHAR buf[2048], *p = buf;
        WCHAR nbuf[2048];
        EXPRESSION* expr = NULL;
        char* idp = lastid;
        while (*idp)
            *p++ = *idp++;
        *p++ = '\n';
        *p = 0;
        wcscpy(nbuf, buf);
        defcheck(buf);
        if (wcscmp(nbuf, buf) != 0)
        {
            cantnewline = TRUE;
            p = --lptr;
            lptr = buf;
            getch();
            getsym();
            ReadQuotedResID(id);
            lptr = p;
            getch();
            cantnewline = FALSE;
            id->origName = WStrDup(nbuf);
            //            StringAsciiToWChar(&id->origName, nbuf, wcslen(nbuf));
            return;
        }
        generror(ERR_INVALIDCLASS, 0);
    }
    else if (is_number())
        ReadResID(id);
    else
    {
        generror(ERR_INVALIDCLASS, 0);
    }
}

//-------------------------------------------------------------------------

static void ReadSecondaryCharacteristics(COMPILEDATA* cd, CHARACTERISTICS* info)
{
    int done = FALSE;
    info->language_high = 0;
    info->language_low = 0;
    while (!done)
    {
        while (lastst == rceol)
            getsym();
        switch (lastst)
        {
            case kw_language:
                getsym();
                info->language_low = ReadExp();
                skip_comma();
                info->language_high = ReadExp();
                break;
            case kw_version:
                getsym();
                info->version = ReadExp();
                break;
            case kw_characteristics:
                getsym();
                info->characteristics = ReadExp();
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            need_eol();
        }
    }
}

//-------------------------------------------------------------------------

static void ReadMemflags(CHARACTERISTICS* info)
{
    int done = FALSE;
    while (!done)
    {
        switch (lastst)
        {
            case kw_discardable:
                info->memflags |= MF_DISCARDABLE;
                break;
            case kw_pure:
                info->memflags |= MF_PURE;
                break;
            case kw_preload:
                info->memflags |= MF_PRELOAD;
                break;
            case kw_moveable:
                info->memflags |= MF_MOVEABLE;
                break;
            case kw_nondiscardable:
                info->memflags |= MF_NONDISCARDABLE;
                break;
            case kw_impure:
                info->memflags |= MF_IMPURE;
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            getsym();
        }
        skip_comma();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

static int ReadString(WCHAR** string)
{
    int rv = 0;
    if (lastst == sconst)
    {
        rv = StringAsciiToWChar(string, laststr, laststrlen);
        getsym();
    }
    else
        generror(ERR_STRING_EXPECTED, 0);
    return rv;
}

//-------------------------------------------------------------------------

static void ParseAccelerator(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    int mode;
    ACCELERATOR *data, **p = &data, *x;
    getsym();
    ReadMemflags(info);
    need_eol();
    ReadSecondaryCharacteristics(cd, info);
    need_begin();

    while (TRUE)
    {
        int done;
        x = *p = rcAlloc(sizeof(*data));
        x->next = NULL;
        x->flags = 0;
        if (lastst == sconst)
        {
            if (laststr[0] == '^')
                x->skey = laststr[1] & 0x1f;
            else
                x->skey = laststr[0];
            mode = 1;
            getsym();
        }
        else if (is_number())
        {
            x->key = ReadExp();
            mode = 0;
        }
        else
            break;
        skip_comma();
        x->id = ReadExp();
        skip_comma();
        done = FALSE;
        while (!done)
        {
            switch (lastst)
            {
                case kw_ascii:
                    x->flags &= ~ACC_VIRTKEY;
                    break;
                case kw_virtkey:
                    x->flags |= ACC_VIRTKEY;
                    break;
                case kw_noinvert:
                    x->flags |= ACC_NOINVERT;
                    break;
                case kw_shift:
                    x->flags |= ACC_SHIFT;
                    break;
                case kw_control:
                    x->flags |= ACC_CONTROL;
                    break;
                case kw_alt:
                    x->flags |= ACC_ALT;
                    break;
                default:
                    done = TRUE;
                    break;
            }
            if (!done)
            {
                getsym();
                skip_comma();
            }
        }
        need_eol();
        p = &(*p)->next;
    }
    *p = 0;
    need_end();
    CreateAcceleratorResource(cd, id, info, data);
}

//-------------------------------------------------------------------------

static void ParseBitmap(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateBitmapResource(cd, id, info, laststr);
    incconst = FALSE;
}

//-------------------------------------------------------------------------

static void ParseCursor(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();

    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateCursorResource(cd, id, info, laststr);
    incconst = FALSE;
}

//-------------------------------------------------------------------------

static void NotExDialogError(extended)
{
    if (!extended)
        generror(ERR_NOT_DIALOGEX, 0);
}

//-------------------------------------------------------------------------

static void ReadDialogSettings(COMPILEDATA* cd, DIALOG* dlg, CHARACTERISTICS* info, int extended)
{
    int done = FALSE;
    info->language_high = 0;
    info->language_low = 0;
    while (!done)
    {
        while (lastst == rceol)
            getsym();
        switch (lastst)
        {
            case kw_language:
                getsym();
                info->language_low = ReadExp();
                skip_comma();
                info->language_high = ReadExp();
                break;
            case kw_version:
                getsym();
                info->version = ReadExp();
                break;
            case kw_characteristics:
                getsym();
                info->characteristics = ReadExp();
                break;
            case kw_style:
                getsym();
                dlg->style = ReadExpOr(dlg->style);
                break;
            case kw_exstyle:
                getsym();
                dlg->exstyle = ReadExpOr(dlg->exstyle);
                break;
            case kw_menu:
                getsym();
                dlg->menu = rcAlloc(sizeof(IDENT));
                ReadResID(dlg->menu);
                break;
            case kw_font:
                getsym();
                dlg->pointsize = ReadExp();
                skip_comma();
                ReadString(&dlg->font);
                if (extended)
                {
                    skip_comma();
                    if (lastst != rceol)
                    {
                        dlg->ex.weight = ReadExp();
                        skip_comma();
                        if (lastst != rceol)
                        {
                            dlg->ex.italic = ReadExp();
                            skip_comma();
                            if (lastst != rceol)
                            {
                                dlg->ex.charset = ReadExp();
                            }
                        }
                    }
                }
                break;
            case kw_caption:
                //                dlg->style |= WS_DLGFRAME ;
                getsym();
                ReadString(&dlg->caption);
                break;
            case kw_class:
                getsym();
                dlg->class = rcAlloc(sizeof(IDENT));
                ReadQuotedResID(dlg->class);
                break;
            case kw_help:
                getsym();
                NotExDialogError(extended);
                dlg->ex.help = ReadExp();
                break;
            case kw_weight:
                getsym();
                NotExDialogError(extended);
                dlg->ex.weight = ReadExp();
                break;
            case kw_italic:
                getsym();
                NotExDialogError(extended);
                dlg->ex.italic = ReadExp();
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            need_eol();
        }
    }
}

//-------------------------------------------------------------------------

static void ParseExtendedControl(CONTROL* c, int extended)
{
    if (lastst != rceol)
    {
        c->exstyle = ReadExp();
        skip_comma();
    }
    if (lastst != rceol)
    {
        c->help = ReadExp();
        skip_comma();
    }
}

//-------------------------------------------------------------------------
static int GetIdentString(void)
{
    WCHAR buf[2048], *p = buf;
    WCHAR nbuf[2048];
    char* id = lastid;
    int rv = FALSE;
    EXPRESSION* expr = NULL;
    while (*id)
        *p++ = *id++;
    *p++ = '\n';
    *p = 0;
    wcscpy(nbuf, buf);
    defcheck(buf);
    if (wcscmp(nbuf, buf))
    {
        cantnewline = TRUE;
        p = --lptr;
        lptr = buf;
        getch();
        getsym();
        rv = lastst == sconst;
        lptr = p;
        getch();
        cantnewline = FALSE;
    }
    return rv;
}
int StrToClass(char* str)
{
    int v;
    if (!stricmp(str, "button"))
        v = CTL_BUTTON;
    else if (!stricmp(str, "edit"))
        v = CTL_EDIT;
    else if (!stricmp(str, "static"))
        v = CTL_STATIC;
    else if (!stricmp(str, "listbox"))
        v = CTL_LISTBOX;
    else if (!stricmp(str, "scrollbar"))
        v = CTL_SCROLLBAR;
    else if (!stricmp(str, "combobox"))
        v = CTL_COMBOBOX;
    else
        v = 0;
    return v;
}
static void SetClass(CONTROL* c, int st)
{
    int v = 0;
    int found = TRUE;
    switch (st)
    {
        case kw_auto3state:
        case kw_autocheckbox:
        case kw_autoradiobutton:
        case kw_checkbox:
        case kw_pushbutton:
        case kw_radiobutton:
        case kw_defpushbutton:
        case kw_state3:
            v = CTL_BUTTON;
            break;
        case kw_combobox:
            v = CTL_COMBOBOX;
            break;
        case kw_ctext:
        case kw_ltext:
        case kw_rtext:
        case kw_icon:
            v = CTL_STATIC;
            break;
        case kw_edittext:
            v = CTL_EDIT;
            break;
        case kw_groupbox:
            v = CTL_BUTTON;
            break;
        case kw_listbox:
            v = CTL_LISTBOX;
            break;
        case kw_scrollbar:
            v = CTL_SCROLLBAR;
            break;
        case ident:
        {
            ReadQuotedResID(&c->class);
            if (c->class.symbolic)
            {
                char buf[256], *p = buf;
                WCHAR* q;
                q = c->class.u.n.symbol;
                while (*q)
                    *p++ = (char)*q++;
                *p = 0;
                v = StrToClass(buf);
                if (!v)
                    return;
                rcFree(c->class.u.n.symbol);
                c->class.u.n.symbol = NULL;
            }
            break;
        }
        case sconst:
        {
            v = StrToClass(laststr);
            if (!v)
            {
                c->class.symbolic = TRUE;
                c->class.u.n.length = StringAsciiToWChar(&c->class.u.n.symbol, laststr, laststrlen);
                return;
            }
            break;
        }
    }
    if (!v)
        generror(ERR_UNKNOWN_DIALOG_CONTROL_CLASS, 0);
    else
    {
        c->class.symbolic = FALSE;
        c->class.u.id = rcAlloc(sizeof(EXPRESSION));
        c->class.u.id->type = e_int;
        c->class.u.id->val = v;
    }
}

//-------------------------------------------------------------------------

static void ParseGenericControl(CONTROL* c, int extended)
{
    c->generic = TRUE;
    getsym();
    c->text = rcAlloc(sizeof(IDENT));
    ReadQuotedResID(c->text);
    skip_comma();
    c->id = ReadExp();
    skip_comma();
    SetClass(c, lastst);
    getsym();
    skip_comma();
    c->style = ReadExp();
    skip_comma();
    c->x = ReadExp();
    skip_comma();
    c->y = ReadExp();
    skip_comma();
    c->width = ReadExp();
    skip_comma();
    c->height = ReadExp();
    skip_comma();
    ParseExtendedControl(c, extended);
}
static void ParseStandardControl(CONTROL* c, int class, int style, int extended, int text)
{
    int st = lastst;
    c->generic = FALSE;
    c->baseStyle = style;
    SetClass(c, st = lastst);
    getsym();
    if (text)
    {
        c->text = rcAlloc(sizeof(IDENT));
        ReadQuotedResID(c->text);
        skip_comma();
    }
    else if (class == CTL_EDIT)
    {
        c->text = rcAlloc(sizeof(IDENT));
        c->text->symbolic = TRUE;
        c->text->u.n.length = 8;
        c->text->u.n.symbol = WStrDup(L"Edit Box");
    }
    c->id = ReadExp();
    skip_comma();
    c->x = ReadExp();
    skip_comma();
    c->y = ReadExp();
    skip_comma();
    c->width = ReadExp();
    skip_comma();
    c->height = ReadExp();
    skip_comma();
    if (lastst != rceol)
    {
        c->style = ReadExp();
        skip_comma();
    }
    ParseExtendedControl(c, extended);
}

//-------------------------------------------------------------------------

static int ParseControl(CONTROL*** ctl, int extended)
{
    int rv = FALSE;
    CONTROL* c = rcAlloc(sizeof(CONTROL));
    memset(c, 0, sizeof(CONTROL));
    c->prevLines = GetCachedLines();
    switch (lastst)
    {
        case kw_auto3state:
            ParseStandardControl(c, CTL_BUTTON, BS_AUTO3STATE | WS_TABSTOP, extended, 1);
            break;
        case kw_autocheckbox:
            ParseStandardControl(c, CTL_BUTTON, BS_AUTOCHECKBOX | WS_TABSTOP, extended, 1);
            break;
        case kw_autoradiobutton:
            ParseStandardControl(c, CTL_BUTTON, BS_AUTORADIOBUTTON, extended, 1);
            break;
        case kw_checkbox:
            ParseStandardControl(c, CTL_BUTTON, BS_CHECKBOX | WS_TABSTOP, extended, 1);
            break;
        case kw_combobox:
            ParseStandardControl(c, CTL_COMBOBOX, WS_TABSTOP, extended, 0);
            break;
        case kw_ctext:
            ParseStandardControl(c, CTL_STATIC, SS_CENTER | WS_GROUP, extended, 1);
            break;
        case kw_defpushbutton:
            ParseStandardControl(c, CTL_BUTTON, BS_DEFPUSHBUTTON | WS_TABSTOP, extended, 1);
            break;
        case kw_edittext:
            ParseStandardControl(c, CTL_EDIT, ES_LEFT | WS_BORDER | WS_TABSTOP, extended, 0);
            break;
        case kw_groupbox:
            ParseStandardControl(c, CTL_STATIC, BS_GROUPBOX, extended, 1);
            break;
        case kw_icon:
            ParseStandardControl(c, CTL_STATIC, SS_ICON, extended, 1);
            break;
        case kw_listbox:
            ParseStandardControl(c, CTL_LISTBOX, LBS_NOTIFY | WS_BORDER | WS_VSCROLL | WS_TABSTOP, extended, 0);
            break;
        case kw_ltext:
            ParseStandardControl(c, CTL_STATIC, SS_LEFT | WS_GROUP, extended, 1);
            break;
        case kw_pushbutton:
            ParseStandardControl(c, CTL_BUTTON, BS_PUSHBUTTON | WS_TABSTOP, extended, 1);
            break;
        case kw_radiobutton:
            ParseStandardControl(c, CTL_BUTTON, BS_RADIOBUTTON, extended, 1);
            break;
        case kw_rtext:
            ParseStandardControl(c, CTL_STATIC, SS_RIGHT | WS_GROUP, extended, 1);
            break;
        case kw_scrollbar:
            ParseStandardControl(c, CTL_SCROLLBAR, SBS_HORZ, extended, 0);
            break;
        case kw_state3:
            ParseStandardControl(c, CTL_BUTTON, BS_3STATE | WS_TABSTOP, extended, 1);
            break;
        case kw_control:
            ParseGenericControl(c, extended);
            break;
        default:
            rv = TRUE;
            break;
    }
    if (!rv)
    {
        need_eol();
        **ctl = c;
        *ctl = &c->next;
    }
    return !rv;
}

//-------------------------------------------------------------------------

static void ParseDialog(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info, int extended)
{
    DIALOG* dialog = rcAlloc(sizeof(DIALOG));
    CONTROL** p = &dialog->controls;
    memset(dialog, 0, sizeof(*dialog));
    dialog->style = 0;  // WS_POPUPWINDOW ;
    getsym();
    ReadMemflags(info);
    dialog->x = ReadExp();
    skip_comma();
    dialog->y = ReadExp();
    skip_comma();
    dialog->width = ReadExp();
    skip_comma();
    dialog->height = ReadExp();
    skip_comma();
    need_eol();
    ReadDialogSettings(cd, dialog, info, extended);
    need_begin();
    while (ParseControl(&p, extended))
        ;
    need_end();
    CreateDialogResource(cd, id, info, dialog, extended);
}

//-------------------------------------------------------------------------

static void ParseDlgInclude(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateDlgIncludeResource(cd, id, info, laststr);
    incconst = FALSE;
}

//-------------------------------------------------------------------------

static void ParseFont(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateFontResource(cd, id, info, laststr);
    incconst = FALSE;
}

//-------------------------------------------------------------------------

static void ParseIcon(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateIconResource(cd, id, info, laststr);
    incconst = FALSE;
}

//-------------------------------------------------------------------------

static void ReadMenuFlags(MENUITEM* m)
{
    int done = FALSE;
    while (!done)
    {
        switch (lastst)
        {
            case kw_grayed:
                m->flags |= MI_GRAYED;
                break;
            case kw_inactive:
                m->flags |= MI_INACTIVE;
                break;
            case kw_checked:
                m->flags |= MI_CHECKED;
                break;
            case kw_menubarbreak:
                m->flags |= MI_MENUBARBREAK;
                break;
            case kw_menubreak:
                m->flags |= MI_MENUBREAK;
                break;
            case kw_help:
                m->flags |= MI_HELP;
                break;
            case kw_separator:
                m->flags |= MI_SEPARATOR;
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            getsym();
            skip_comma();
        }
        else
        {
            skip_comma();
            if (lastst != rceol)
            {
                m->state = ReadExp();
                skip_comma();
            }
        }
    }
}

//-------------------------------------------------------------------------

static void ReadMenuList(MENUITEM*** i, int extended)
{
    MENUITEM** p;
    int done = FALSE;
    need_begin();
    while (!done)
    {
        MENUITEM* m = rcAlloc(sizeof(MENUITEM));
        memset(m, 0, sizeof(*m));
        m->prevLines = GetCachedLines();
        switch (lastst)
        {
            case kw_menuitem:
                getsym();
                if (lastst == sconst)
                {
                    StringAsciiToWChar(&m->text, laststr, laststrlen);
                    getsym();
                    skip_comma();
                }
                if (is_number())
                    m->id = ReadExp();
                skip_comma();
                if (!extended)
                    ReadMenuFlags(m);
                else if (lastst != rceol)
                {
                    if (is_number())
                        m->type = ReadExp();
                    if (Eval(m->type) & MFT_SEPARATOR)
                        m->text = NULL;
                    skip_comma();
                    if (lastst != rceol)
                    {
                        if (is_number())
                            m->state = ReadExp();
                        skip_comma();
                    }
                    if (lastst != rceol)
                    {
                        if (is_number())
                            m->help = ReadExp();
                        skip_comma();
                    }
                }
                break;
            case kw_popup:
                getsym();
                if (lastst == sconst)
                {
                    StringAsciiToWChar(&m->text, laststr, laststrlen);
                    getsym();
                    skip_comma();
                }
                if (is_number())
                {
                    m->id = ReadExp();
                    skip_comma();
                }
                if (!extended)
                    ReadMenuFlags(m);
                else
                {
                    if (lastst != rceol)
                    {
                        m->type = ReadExp();
                        skip_comma();
                    }
                    if (lastst != rceol)
                    {
                        m->state = ReadExp();
                        skip_comma();
                    }
                    if (lastst != rceol)
                    {
                        m->help = ReadExp();
                        skip_comma();
                    }
                }
                p = &m->popup;
                need_eol();
                ReadMenuList(&p, extended);
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            need_eol();
            **i = m;
            *i = &(m->next);
        }
    }
    if (lastst != end && lastst != kw_end)
        generror(ERR_END_EXPECTED, 0);
    getsym();
}
static void ParseMenu(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info, int extended)
{
    MENUITEM *s = 0, **p = &s;
    getsym();
    ReadMemflags(info);
    need_eol();
    ReadSecondaryCharacteristics(cd, info);
    ReadMenuList(&p, extended);

    need_eol();
    CreateMenuResource(cd, id, info, s, extended);
}
static void ParseRC(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    int done = FALSE;
    RCDATA *r = 0, **p = &r;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    if (lastst == sconst)
    {
        ReadFileName();
        need_eol();
        CreateUserFileResource(cd, id, (IDENT*)RESTYPE_RCDATA, info, laststr);
        return;
    }
    need_eol();
    need_begin();
    while (!done)
    {
        switch (lastst)
        {
            case sconst:
                *p = CreateRCDataResourceString(laststr, laststrlen);
                getsym();
                break;
            case iconst:
            case iuconst:
            case cconst:
                *p = CreateRCDataResourceNumber(ival, FALSE);
                getsym();
                break;
            case lconst:
            case luconst:
                *p = CreateRCDataResourceNumber(ival, TRUE);
                getsym();
                break;
            case rceol:
                getsym();
                continue;
            default:
                done = TRUE;
        }
        if (!done)
        {
            skip_comma();
            p = &(*p)->next;
        }
    }
    need_end();
    CreateRCDataResource(cd, id, info, r);
}
static void ParseMessageTable(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateMessageTableResource(cd, id, info, laststr);
    incconst = FALSE;
}
static void ParseStringTable(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    STRINGS *strings = NULL, **next = &strings;
    incconst = TRUE;
    defcheck(--lptr);
    getch();
    getsym();
    ReadMemflags(info);
    need_eol();
    need_begin();
    while (is_number())
    {
        EXPRESSION* val;
        defcheck(lptr);
        val = ReadExp();
        skip_comma();
        if (lastst != sconst)
            generror(ERR_STRING_EXPECTED, 0);
        *next = rcAlloc(sizeof(STRINGS));
        (*next)->id = val;
        (*next)->length = StringAsciiToWChar(&(*next)->string, laststr, strlen(laststr));
        next = &(*next)->next;
        getsym();
        need_eol();
    }
    CreateStringTableResource(cd, id, info, strings);
    need_end();
    incconst = FALSE;
}
static void ParseVersionInfo(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    int val, val1;
    int done = 0, did1 = 0;
    struct fixed* fixedverinfo = rcAlloc(sizeof(struct fixed));
    struct variable *verinfo = 0, **verinfop = &verinfo;
    getsym();
    info->memflags = MF_PURE | MF_MOVEABLE;
    while (!done)
    {
        val = val1 = 0;
        switch (lastst)
        {
            case kw_fileversion:
                getsym();
                val = intexpr() << 16;
                if (lastst == comma)
                {
                    getsym();
                    val |= intexpr() & 0xffff;
                    if (lastst == comma)
                    {
                        getsym();
                        val1 = intexpr() << 16;
                        if (lastst == comma)
                        {
                            getsym();
                            val1 |= intexpr() & 0xffff;
                        }
                    }
                }
                fixedverinfo->file_version_ms = val;
                fixedverinfo->file_version_ls = val1;
                break;
            case kw_productversion:
                getsym();
                val = intexpr() << 16;
                if (lastst == comma)
                {
                    getsym();
                    val |= intexpr() & 0xffff;
                    if (lastst == comma)
                    {
                        getsym();
                        val1 = intexpr() << 16;
                        if (lastst == comma)
                        {
                            getsym();
                            val1 |= intexpr() & 0xffff;
                        }
                    }
                }
                fixedverinfo->product_version_ms = val;
                fixedverinfo->product_version_ls = val1;
                break;
            case kw_fileflagmask:
                getsym();
                fixedverinfo->file_flags_mask = ReadExp();
                break;
            case kw_fileflags:
                getsym();
                fixedverinfo->file_flags = ReadExp();
                break;
            case kw_fileos:
                getsym();
                fixedverinfo->file_os = ReadExp();
                break;
            case kw_filetype:
                getsym();
                fixedverinfo->file_type = ReadExp();
                break;
            case kw_filesubtype:
                getsym();
                fixedverinfo->file_subtype = ReadExp();
                break;
            case rceol:
                getsym();
                break;
                //         case kw_filedate:
                //            break ;
            default:
                if (!did1)
                    generror(ERR_FIXEDDATAEXPECTED, 0);
                done = 1;
                break;
        }
        did1 = 1;
    }
    need_begin();
    if (lastst != kw_block)
        generror(ERR_BLOCK_EXPECTED, 0);
    while (lastst == kw_block)
    {
        getsym();
        if (lastst != sconst)
            generror(ERR_INVALID_VERSION_INFO_TYPE, 0);
        if (!strcmp(laststr, "StringFileInfo"))
        {
            struct ver_stringinfo** current;
            getsym();
            need_eol();
            need_begin();
            if (lastst != kw_block)
                generror(ERR_BLOCK_EXPECTED, 0);
            getsym();
            *verinfop = rcAlloc(sizeof(struct variable));
            (*verinfop)->type = VERINFO_STRING;
            ReadString(&(*verinfop)->u.string.language);
            need_eol();
            current = &(*verinfop)->u.string.strings;
            need_begin();
            while (lastst == kw_value)
            {
                defcheck(lptr);
                getsym();
                if (lastst != sconst)
                    generror(ERR_STRING_EXPECTED, 0);
                *current = rcAlloc(sizeof(struct ver_stringinfo));
                ReadString(&(*current)->key);
                if (lastst != comma)
                    generror(ERR_NEEDCHAR, ',');
                getsym();
                (*current)->length = laststrlen;
                if (lastst != sconst)
                    generror(ERR_STRING_EXPECTED, 0);
                (*current)->value = rcAlloc(laststrlen * 2 + 2);
                for (val = 0; val < laststrlen; val++)
                    (*current)->value[val] = laststr[val];
                getsym();
                need_eol();
                current = &(*current)->next;
            }
            need_end();
        }
        else if (!strcmp(laststr, "VarFileInfo"))
        {
            struct ver_varinfo** current;
            getsym();
            need_eol();
            need_begin();
            *verinfop = rcAlloc(sizeof(struct variable));
            (*verinfop)->type = VERINFO_VAR;
            current = &((*verinfop)->u.var.var);
            while (lastst == kw_value)
            {
                struct ver_varlangchar** cur1;
                defcheck(lptr);
                *current = rcAlloc(sizeof(struct ver_varinfo));
                getsym();
                ReadString(&(*current)->key);
                if (lastst != comma)
                    generror(ERR_NEEDCHAR, ',');
                getsym();
                cur1 = &(*current)->intident;
                while (TRUE)
                {
                    *cur1 = rcAlloc(sizeof(struct ver_varlangchar));
                    (*cur1)->language = ReadExp();
                    if (lastst == comma)
                    {
                        getsym();
                        (*cur1)->charset = ReadExp();
                        if (lastst == comma)
                        {
                            getsym();
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                    cur1 = &(*cur1)->next;
                }
                need_eol();
                current = &(*current)->next;
            }
        }
        else
            generror(ERR_INVALID_VERSION_INFO_TYPE, 0);

        verinfop = &(*verinfop)->next;
        need_end();
    }
    need_end();
    CreateVersionInfoResource(cd, id, info, fixedverinfo, verinfo);
}
static void ParseSpecial(COMPILEDATA* cd, IDENT* id, CHARACTERISTICS* info)
{
    IDENT type;
    memset(&type, 0, sizeof(IDENT));
    incconst = TRUE;
    ReadResID(&type);
    defcheck(--lptr);
    getch();
    ReadMemflags(info);
    ReadFileName();
    need_eol();
    CreateUserFileResource(cd, id, &type, info, laststr);
    incconst = FALSE;
}
static void Parse(COMPILEDATA* cd)
{
    IDENT id;
    CHARACTERISTICS info;

    getch();
    getsym();
    cd->lines = GetCachedLines();
    CreatePlaceholderResource(cd);
    while (lastst != rceof)
    {
        while (lastst == rceol)
            getsym();
        memset(&id, 0, sizeof(id));
        if (lastst == kw_stringtable)
        {
            id.symbolic = TRUE;
            id.u.n.length = StringAsciiToWChar(&id.u.n.symbol, "stringtable", strlen("stringtable"));
        }
        else
        {
            if (lastst == ident || is_number())
                ReadResID(&id);
            else
            {
                int st = lastst;
                getsym();
                if (lastst >= kw_accelerator && lastst < rceol)
                {
                    if (st >= kw_accelerator && st < rceol)
                    {
                        char* s = namefromkw(st);
                        id.symbolic = 1;
                        id.u.n.length = StringAsciiToWChar(&id.u.n.symbol, s, strlen(s));
                    }
                    else
                        backup(st);
                }
                else
                    backup(st);
            }
        }
        cd->lines = GetCachedLines();
        memset(&info, 0, sizeof(info));
        info.language_high = cd->base_language_high;
        info.language_low = cd->base_language_low;
        switch (lastst)
        {
            case kw_accelerator:
                ParseAccelerator(cd, &id, &info);
                break;
            case kw_bitmap:
                ParseBitmap(cd, &id, &info);
                break;
            case kw_cursor:
                ParseCursor(cd, &id, &info);
                break;
            case kw_dialog:
                ParseDialog(cd, &id, &info, FALSE);
                break;
            case kw_dialogex:
                ParseDialog(cd, &id, &info, TRUE);
                break;
            case kw_dlginclude:
                ParseDlgInclude(cd, &id, &info);
                break;
            case kw_font:
                ParseFont(cd, &id, &info);
                break;
            case kw_icon:
                ParseIcon(cd, &id, &info);
                break;
            case kw_menu:
                ParseMenu(cd, &id, &info, FALSE);
                break;
            case kw_menuex:
                ParseMenu(cd, &id, &info, TRUE);
                break;
            case kw_rcdata:
                ParseRC(cd, &id, &info);
                break;
            case kw_versioninfo:
                ParseVersionInfo(cd, &id, &info);
                break;
            case kw_messagetable:
                ParseMessageTable(cd, &id, &info);
                break;
            case kw_stringtable:
                ParseStringTable(cd, &id, &info);
                break;
            case kw_language:
                getsym();
                cd->base_language_low = ReadExp();
                skip_comma();
                cd->base_language_high = ReadExp();
                CreateLanguageResource(cd);
                need_eol();
                break;
            case kw_rcinclude:
                doinclude(TRUE);
                break;
            case iconst:
            case iuconst:
            case lconst:
            case luconst:
            case ident:
                ParseSpecial(cd, &id, &info);
                break;
            default:
                generror(ERR_UNKNOWN_RESOURCE_TYPE, 0);
                break;
        }
    }
}
void FreeResources(RESOURCE_DATA* r) { HeapDestroy(r->memHeap); }
void SetRCMallocScope(RESOURCE_DATA* r) { memHeap = r->memHeap; }
static void SelectControlIdBase(RESOURCE_DATA* select)
{
    RESOURCE* res;
    select->nextControlId = 1;
    for (res = select->resources; res; res = res->next)
    {
        if (res->itype == RESTYPE_DIALOG)
        {
            CONTROL* controls = res->u.dialog->controls;
            while (controls)
            {
                int n = Eval(controls->id);
                if (n >= select->nextControlId)
                    select->nextControlId = n + 1;
                controls = controls->next;
            }
        }
    }
}
static void RecurseMenuIdBase(RESOURCE_DATA* select, MENUITEM* items)
{
    while (items)
    {
        int n = Eval(items->id);
        if (n >= select->nextMenuId)
            select->nextMenuId = n + 1;
        if (items->popup)
            RecurseMenuIdBase(select, items->popup);
        items = items->next;
    }
}
static void SelectMenuIdBase(RESOURCE_DATA* select)
{
    RESOURCE* res;
    select->nextMenuId = 10000;
    for (res = select->resources; res; res = res->next)
    {
        if (res->itype == RESTYPE_MENU)
        {
            RecurseMenuIdBase(select, res->u.menu->items);
        }
    }
}
static void SelectResourceIdBase(RESOURCE_DATA* select)
{
    RESOURCE* res;
    select->nextResourceId = 10;
    for (res = select->resources; res; res = res->next)
    {
        if (!res->id.symbolic)
        {
            int n = Eval(res->id.u.id);
            if (n >= select->nextResourceId)
                select->nextResourceId = n + 1;
        }
    }
}
static void SelectStringIdBase(RESOURCE_DATA* select)
{
    RESOURCE* res;
    select->nextStringId = 1000;
    for (res = select->resources; res; res = res->next)
    {
        if (res->itype == RESTYPE_STRING)
        {
            STRINGS* strings = res->u.stringtable;
            while (strings)
            {
                int n = Eval(strings->id);
                if (n >= select->nextStringId)
                    select->nextStringId = n + 1;
                strings = strings->next;
            }
        }
    }
}
static int GetIdVal(SYM* sym)
{
    char buf[256];
    DEFSTRUCT* d = (DEFSTRUCT*)sym->value.s;
    StringWToA(buf, d->string, wcslen(d->string));
    return atoi(buf);
}
static SYM* GetIds(RESOURCE_DATA* select, SYM* syms)
{
    SYM** curs = &syms;
    BOOL done = FALSE;
    // we are going to search for our special id #defines in any order
    // in case the user went happily editing them
    while (*curs && (stricmp((*curs)->name, "__NEXT_CONTROL_ID") && stricmp((*curs)->name, "__NEXT_MENU_ID") != 0 &&
                     stricmp((*curs)->name, "__NEXT_RESOURCE_ID") != 0 && stricmp((*curs)->name, "__NEXT_STRING_ID") != 0))
    {
        curs = &(*curs)->xref;
    }
    if (*curs)
    {
        SYM** begin = curs;
        // ok found something, try to get the values
        while (!done && *curs)
        {
            if (!stricmp((*curs)->name, "__NEXT_CONTROL_ID"))
            {
                select->nextControlId = GetIdVal(*curs);
                *curs = (*curs)->xref;
                continue;
            }
            if (!stricmp((*curs)->name, "__NEXT_MENU_ID"))
            {
                select->nextMenuId = GetIdVal(*curs);
                *curs = (*curs)->xref;
                continue;
            }
            if (!stricmp((*curs)->name, "__NEXT_RESOURCE_ID"))
            {
                select->nextResourceId = GetIdVal(*curs);
                *curs = (*curs)->xref;
                continue;
            }
            if (!stricmp((*curs)->name, "__NEXT_STRING_ID"))
            {
                select->nextStringId = GetIdVal(*curs);
                *curs = (*curs)->xref;
                continue;
            }
            done = TRUE;
        }
    }
    // now if something was unspecified try to recover
    if (select->nextControlId == 0)
    {
        SelectControlIdBase(select);
    }
    if (select->nextMenuId == 0)
    {
        SelectMenuIdBase(select);
    }
    if (select->nextResourceId == 0)
    {
        SelectResourceIdBase(select);
    }
    if (select->nextStringId == 0)
    {
        SelectStringIdBase(select);
    }
    return syms;
}
RESOURCE_DATA* ReadResources(char* fileName)
{
    COMPILEDATA cd;
    rcIdFile = NULL;
    memset(&cd, 0, sizeof(cd));
    cd.resourcesTail = &cd.resources;
    memHeap = NULL;
    int fileFlags;

    if (setjmp(errjump))
    {
        cd.resources = NULL;
    }
    else
    {
        symini();
        preprocini();
        initsym();
        initerr();
        kwini();
        glbdefine("__CCDL__", "");
        glbdefine("__ORANGEC__", "");
        glbdefine("__386__", "");
        glbdefine("__i386__", "");
        glbdefine("RC_INVOKED", "");
        glbdefine("__IDE_RC_INVOKED", "");

        errfile = infile = fileName;
        inputBuffer = ReadFileData(fileName, &fileFlags);
        if (!inputBuffer)
            fatal("file %s not found", fileName);
        inputLength = strlen(inputBuffer);
        ibufPtr = inputBuffer;
        Parse(&cd);
    }
    free(inputBuffer);
    inputBuffer = NULL;
    ibufPtr = NULL;
    inputLength = 0;
    if (cd.resources)
    {
        RESOURCE_DATA* rv = rcAlloc(sizeof(RESOURCE_DATA));
        rv->memHeap = memHeap;
        memHeap = NULL;
        rv->resources = cd.resources;
        rv->resourceIdFile = rcIdFile;
        rv->syms = defsyms;
        rv->headerDefinitions = GetIds(rv, rcDefs);
        rv->fileFlags = fileFlags;
        return rv;
    }
    else
    {
        HeapDestroy(memHeap);
        memHeap = NULL;
    }
    return NULL;
}
