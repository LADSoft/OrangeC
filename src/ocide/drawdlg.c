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

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "header.h"
#include "rc.h"
#include "rcgui.h"
#include "xml.h"

#define ERRORMSG ExtendedMessageBox("Error", 0, "Error reading ctl file")

#define F_TOP 1
#define F_BOTTOM 2
#define F_LEFT 4
#define F_RIGHT 8
#define SIZING 16
#define SIZINGUNDODONE 32

#define CTLPROPS "ctl.props"

#ifndef DC_BUTTONS
#    define DC_BUTTONS 0x1000
#endif

#define WM_MOVEDCHILD 20000

extern LOGFONT systemCaptionFont;
extern HWND hwndSrcTab;
extern HWND hwndClient;
extern HINSTANCE hInstance;
extern PROJECTITEM* workArea;
extern char szInstallPath[];
extern EXPRESSION* ResAllocateControlId(struct resRes* dlgData, char* name);

BOOL snapToGrid;
BOOL showGrid;
int gridSpacing = 4;

#define MAX_CONTROL 32
static struct ctlDB* dbs[MAX_CONTROL];
static char* dbNames[MAX_CONTROL];
static int dbCount;
static struct ctlDB* currentDB;

static char* szDlgDrawClassName = "xccDlgDrawClass";
static char* szDlgDialogClassName = "xccDlgDialogClass";
static char* szDlgUnknownClassName = "xccDlgUnknownClass";
static char* szDlgControlInputClassName = "xccDlgControlInputClass";

static char* szUntitled = "Dialog";
static HCURSOR dragCur, noCur, hcurs, vcurs;
static int clipboardFormatId;

static void InsertCtlProperties(HWND lv, struct ctlData* data);
static void GetCtlPropText(char* buf, HWND lv, struct ctlData* data, int row);
static HWND CtlPropStartEdit(HWND lv, int row, struct ctlData* data);
static void CtlPropEndEdit(HWND lv, int row, HWND editWnd, struct ctlData* data);
// callbacks to populate the properties window for this resource editor
static struct propertyFuncs CtlFuncs = {InsertCtlProperties, GetCtlPropText, CtlPropStartEdit, CtlPropEndEdit};
static void InsertDlgProperties(HWND lv, struct resRes* data);
void GetDlgPropText(char* buf, HWND lv, struct resRes* data, int row);
HWND DlgPropStartEdit(HWND lv, int row, struct resRes* data);
void DlgPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data);
HRESULT PASCAL SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
struct propertyFuncs dlgFuncs = {InsertDlgProperties, GetDlgPropText, DlgPropStartEdit, DlgPropEndEdit};
HANDLE ResGetHeap(PROJECTITEM* wa, struct resRes* data);
static void CreateSingleControl(HWND hwnd, struct resRes* dlgData, CONTROL* c, BOOL activate);
static BOOL CALLBACK RemoveDlgWindow(HWND hwnd, LPARAM lParam);
static void UndoChanged(struct resRes* dlgData, CONTROL* list);
BOOL RemoveFromStyle(EXPRESSION** style, int val);
void AddToStyle(EXPRESSION** style, char* text, int val);
HWND PropGetHWNDCombobox(HWND parent, BOOL vscroll);
HWND PropGetHWNDNumeric(HWND parent);
HWND PropGetHWNDText(HWND parent);

struct dlgUndo
{
    struct dlgUndo* next;
    enum
    {
        du_changed,
        du_insert,
        du_delete,
        du_reorder,
        du_groupstart,
        du_groupend
    } type;
    CONTROL** place;
    CONTROL* item;
    CONTROL* saved;
    LIST* order;
};

struct clipboardBuffer
{
    char* data;
    int maxSize;
    int currentSize;
};
struct ctlField
{
    struct ctlField* next;
    char* label;
    char* valueLabel;
    int value;
};
struct ctlDB
{
    struct ctlDB* next;
    enum
    {
        eCaption,
        eStyle,
        eEXStyle,
        eID,
        eNumber,
        eHeader
    } recType;
    enum
    {
        iNone,
        iYESNO,
        iNEGYESNO,
        iCombo,
        iID,
        iX,
        iY,
        iWidth,
        iHeight,
        iText
    } ctlType;
    int mask;
    char* label;
    struct ctlField* fields;
};

static SIZE GetAveCharSize(HDC dc)
{
    TEXTMETRIC tm;
    char* buffer = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    SIZE result;
    GetTextMetrics(dc, &tm);

    GetTextExtentPoint32(dc, buffer, 52, &result);

    result.cx = (result.cx / 26 + 1) / 2;  // div uses trunc rounding; we want arithmetic rounding
    result.cy = tm.tmHeight;

    return result;
}

static SIZE GetCheckBoxSize(HDC dc)
{

    const int dluCheckboxSpacing = 12;  // 12 horizontal dlus
    const int dluCheckboxHeight = 10;   // 10 vertical dlus

    SIZE internal = GetAveCharSize(dc);
    SIZE rv = {0, 0};
    rv.cx = MulDiv(dluCheckboxSpacing, internal.cx, 4);
    rv.cy = MulDiv(dluCheckboxSpacing, internal.cy, 8);
    return rv;
}

static SIZE GetTextSize(HDC dc, char* caption)
{
    RECT textRect = {0, 0, 0, 0};
    SIZE rv;
    DrawText(dc, caption, -1, &textRect, DT_CALCRECT | DT_LEFT | DT_SINGLELINE);

    rv.cx = textRect.right;
    rv.cy = textRect.bottom;
    return rv;
}

static void LoadCtlFields(struct ctlDB* dbEntry, struct xmlNode* node)
{
    struct xmlNode* children = node->children;
    struct ctlField** store = &dbEntry->fields;
    while (children)
    {
        if (IsNode(children, "FIELD"))
        {
            struct xmlAttr* attr = children->attribs;
            struct ctlField* field = calloc(1, sizeof(struct ctlField));
            *store = field;
            store = &(*store)->next;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    field->label = strdup(attr->value);
                }
                else if (IsAttrib(attr, "VALUELABEL"))
                {
                    field->valueLabel = strdup(attr->value);
                }
                else if (IsAttrib(attr, "VALUE"))
                {
                    field->value = strtoul(attr->value, NULL, 0);
                }
                else
                {
                    ERRORMSG;
                }
                attr = attr->next;
            }
        }
        else
        {
            ERRORMSG;
        }
        children = children->next;
    }
}
static void LoadCtlDb(struct xmlNode* control)
{
    struct xmlNode* children = control->children;
    struct ctlDB *dbEntry = NULL, **store = &dbs[dbCount];
    struct xmlAttr* attr = control->attribs;
    while (attr)
    {
        if (IsAttrib(attr, "TYPE"))
        {
            dbNames[dbCount] = strdup(attr->value);
            break;
        }
        attr = attr->next;
    }
    while (children)
    {
        if (!dbEntry)
        {
            dbEntry = calloc(1, sizeof(struct ctlDB));
        }
        if (IsNode(children, "CAPTION"))
        {
            dbEntry->recType = eCaption;
            dbEntry->ctlType = iText;
            attr = children->attribs;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    dbEntry->label = strdup(attr->value);
                    break;
                }
                attr = attr->next;
            }
            *store = dbEntry;
            store = &(*store)->next;
            dbEntry = NULL;
        }
        else if (IsNode(children, "STYLE"))
        {
            dbEntry->recType = eStyle;
            attr = children->attribs;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    dbEntry->label = strdup(attr->value);
                }
                else if (IsAttrib(attr, "TYPE"))
                {
                    if (!strcmp(attr->value, "COMBO"))
                        dbEntry->ctlType = iCombo;
                    else if (!strcmp(attr->value, "YESNO"))
                        dbEntry->ctlType = iYESNO;
                    else if (!strcmp(attr->value, "NEGYESNO"))
                        dbEntry->ctlType = iNEGYESNO;
                    else
                    {
                        ERRORMSG;
                    }
                }
                else if (IsAttrib(attr, "MASK"))
                {
                    dbEntry->mask = strtoul(attr->value, NULL, 0);
                }
                attr = attr->next;
            }
            LoadCtlFields(dbEntry, children);
            *store = dbEntry;
            store = &(*store)->next;
            dbEntry = NULL;
        }
        else if (IsNode(children, "EXSTYLE"))
        {
            dbEntry->recType = eEXStyle;
            attr = children->attribs;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    dbEntry->label = strdup(attr->value);
                }
                else if (IsAttrib(attr, "TYPE"))
                {
                    if (!strcmp(attr->value, "COMBO"))
                        dbEntry->ctlType = iCombo;
                    else if (!strcmp(attr->value, "YESNO"))
                        dbEntry->ctlType = iYESNO;
                    else if (!strcmp(attr->value, "NEGYESNO"))
                        dbEntry->ctlType = iNEGYESNO;
                    else
                    {
                        ERRORMSG;
                    }
                }
                else if (IsAttrib(attr, "MASK"))
                {
                    dbEntry->mask = strtoul(attr->value, NULL, 0);
                }
                attr = attr->next;
            }
            LoadCtlFields(dbEntry, children);
            *store = dbEntry;
            store = &(*store)->next;
            dbEntry = NULL;
        }
        else if (IsNode(children, "NUMBER"))
        {
            dbEntry->recType = eNumber;
            attr = children->attribs;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    dbEntry->label = strdup(attr->value);
                }
                else if (IsAttrib(attr, "TYPE"))
                {
                    if (!stricmp(attr->value, "X"))
                        dbEntry->ctlType = iX;
                    else if (!stricmp(attr->value, "Y"))
                        dbEntry->ctlType = iY;
                    else if (!stricmp(attr->value, "WIDTH"))
                        dbEntry->ctlType = iWidth;
                    else if (!stricmp(attr->value, "HEIGHT"))
                        dbEntry->ctlType = iHeight;
                }
                attr = attr->next;
            }
            *store = dbEntry;
            store = &(*store)->next;
            dbEntry = NULL;
        }
        else if (IsNode(children, "ID"))
        {
            dbEntry->recType = eID;
            dbEntry->ctlType = iID;
            attr = children->attribs;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    dbEntry->label = strdup(attr->value);
                    break;
                }
                attr = attr->next;
            }
            *store = dbEntry;
            store = &(*store)->next;
            dbEntry = NULL;
        }
        else if (IsNode(children, "HEADER"))
        {
            dbEntry->recType = eHeader;
            attr = children->attribs;
            while (attr)
            {
                if (IsAttrib(attr, "LABEL"))
                {
                    dbEntry->label = strdup(attr->value);
                    break;
                }
                attr = attr->next;
            }
            *store = dbEntry;
            store = &(*store)->next;
            dbEntry = NULL;
        }
        else
        {
            ERRORMSG;
        }
        children = children->next;
    }
    if (dbEntry)
        free(dbEntry);
    dbCount++;
}
void drawdlginit(void)
{
    FILE* in;
    char name[MAX_PATH];
    struct xmlNode *root, *children;
    strcpy(name, szInstallPath);
    strcat(name, "\\bin\\");
    strcat(name, CTLPROPS);
    in = fopen(name, "r");
    if (!in)
    {
        return;
    }
    root = xmlReadFile(in);
    fclose(in);
    if (!root || !IsNode(root, "CTLPROPS"))
    {
        ExtendedMessageBox("Error", 0, "Invalid XML in ctl.props");
        if (root)
            xmlFree(root);
        return;
    }
    children = root->children;
    while (children)
    {
        if (IsNode(children, "CONTROL"))
        {
            LoadCtlDb(children);
        }
        children = children->next;
    }
    xmlFree(root);
}
char* GetCtlName(CONTROL* c)
{
    char* name = "GENERIC";
    int n = Eval(c->style);
    if (c->class.symbolic)
    {
        char buf[256];
        StringWToA(buf, c->class.u.n.symbol, c->class.u.n.length);
        name = rcStrdup(buf);
    }
    else
        switch (Eval(c->class.u.id))
        {
            case CTL_BUTTON:
                switch (n & 0xf)
                {
                    case BS_GROUPBOX:
                        name = "GROUPBOX";
                        break;
                    case BS_PUSHBUTTON:
                    case BS_DEFPUSHBUTTON:
                        name = "PUSHBUTTON";
                        break;
                    case BS_CHECKBOX:
                    case BS_AUTOCHECKBOX:
                    case BS_3STATE:
                    case BS_AUTO3STATE:
                        name = "CHECKBOX";
                        break;
                    case BS_RADIOBUTTON:
                    case BS_AUTORADIOBUTTON:
                        name = "RADIOBUTTON";
                        break;
                }
                break;
            case CTL_EDIT:
                name = "EDIT";
                break;
            case CTL_STATIC:
                switch (n & 0x1f)
                {
                    case SS_LEFT:
                    case SS_CENTER:
                    case SS_RIGHT:
                    case SS_SIMPLE:
                    case SS_LEFTNOWORDWRAP:
                        name = "STATICTEXT";
                        break;
                    case SS_ICON:
                    case SS_BITMAP:
                        name = "ICON";
                        break;
                    case SS_BLACKRECT:
                    case SS_GRAYRECT:
                    case SS_WHITERECT:
                        name = "RECTANGLE";
                        break;
                    case SS_BLACKFRAME:
                    case SS_GRAYFRAME:
                    case SS_WHITEFRAME:
                        name = "FRAME";
                        break;
                    case SS_ETCHEDHORZ:
                    case SS_ETCHEDVERT:
                    case SS_ETCHEDFRAME:
                        name = "ETCHED";
                        break;
                }
                break;
            case CTL_LISTBOX:
                name = "LISTBOX";
                break;
            case CTL_SCROLLBAR:
                if (n & SBS_VERT)
                {
                    name = "VSCROLL";
                }
                else
                {
                    name = "HSCROLL";
                }
                break;
            case CTL_COMBOBOX:
                name = "COMBOBOX";
                break;
        }
    return name;
}
static struct ctlDB* GetCtlDb(CONTROL* c)
{
    int i;
    char* name = GetCtlName(c);
    for (i = 0; dbs[i] && i < MAX_CONTROL; i++)
    {
        if (!stricmp(dbNames[i], name))
            return dbs[i];
    }
    return NULL;
}
static void GetCtlPropText(char* buf, HWND lv, struct ctlData* data, int row)
{
    struct ctlDB* db = data->db;
    while (db)
    {
        if (db->recType != eHeader)
        {
            if (!row--)
                break;
        }
        db = db->next;
    }
    if (db)
    {
        int n = 0;
        if (db->recType == eStyle)
            n = Eval(data->data->style);
        else if (db->recType == eEXStyle)
            n = Eval(data->data->exstyle);
        switch (db->ctlType)
        {
            struct ctlField* field;
            case iYESNO:
                n &= db->mask;
                field = db->fields;
                if (n == db->fields->value)
                    strcpy(buf, "Yes");
                else
                    strcpy(buf, "No");
                break;
            case iNEGYESNO:
                n &= db->mask;
                if (n == db->fields->value)
                    strcpy(buf, "No");
                else
                    strcpy(buf, "Yes");
                break;
            case iCombo:
                n &= db->mask;
                field = db->fields;
                strcpy(buf, "Unknown");
                while (field)
                {
                    if (n == field->value)
                    {
                        strcpy(buf, field->label);
                        break;
                    }
                    field = field->next;
                }
                break;
            case iID:
                buf[0] = 0;
                FormatExp(buf, data->data->id);
                break;
            case iX:
                sprintf(buf, "%d", Eval(data->data->x));
                break;
            case iY:
                sprintf(buf, "%d", Eval(data->data->y));
                break;
            case iWidth:
                sprintf(buf, "%d", Eval(data->data->width));
                break;
            case iHeight:
                sprintf(buf, "%d", Eval(data->data->height));
                break;
            case iText:
                if (data->data->text->symbolic)
                {
                    StringWToA(buf, data->data->text->u.n.symbol, data->data->text->u.n.length);
                }
                else
                {
                    buf[0] = 0;
                    FormatExp(buf, data->data->text->u.id);
                }
                break;
            default:
                buf[0] = 0;
                break;
        }
    }
}
static HWND CtlPropStartEdit(HWND lv, int row, struct ctlData* data)
{
    int irow = row;
    HWND rv = NULL;
    struct ctlDB* db = data->db;
    while (db)
    {
        if (db->recType != eHeader)
        {
            if (!irow--)
                break;
        }
        db = db->next;
    }
    if (db)
    {
        switch (db->ctlType)
        {
            case iYESNO:
            case iNEGYESNO:
            case iCombo:
                rv = PropGetHWNDCombobox(lv, FALSE);
                break;
            case iX:
            case iY:
            case iWidth:
            case iHeight:
                rv = PropGetHWNDNumeric(lv);
                break;
            case iText:
            case iID:
                rv = PropGetHWNDText(lv);
                break;
            case iNone:
                break;
        }
        if (rv)
        {
            char buf[512];
            switch (db->ctlType)
            {
                int v;
                struct ctlField* field;
                case iYESNO:
                    v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "No");
                    SendMessage(rv, CB_SETITEMDATA, v, 0);
                    v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "Yes");
                    SendMessage(rv, CB_SETITEMDATA, v, db->fields->value);
                    break;
                case iNEGYESNO:
                    v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "No");
                    SendMessage(rv, CB_SETITEMDATA, v, db->fields->value);
                    v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "Yes");
                    SendMessage(rv, CB_SETITEMDATA, v, 0);
                    break;
                case iCombo:
                    field = db->fields;
                    while (field)
                    {
                        v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM)field->label);
                        SendMessage(rv, CB_SETITEMDATA, v, field->value);
                        field = field->next;
                    }
                    break;
                default:
                    break;
            }
            GetCtlPropText(buf, lv, data, row);
            switch (db->ctlType)
            {
                int v;
                case iYESNO:
                case iNEGYESNO:
                case iCombo:
                    v = SendMessage(rv, CB_FINDSTRINGEXACT, 0, (LPARAM)buf);
                    if (v != CB_ERR)
                        SendMessage(rv, CB_SETCURSEL, v, 0);
                    break;
                default:
                    SendMessage(rv, WM_SETTEXT, 0, (LPARAM)buf);
                    break;
            }
        }
    }
    return rv;
}
static void AdjustControlWidths(struct ctlData* data, char* text)
{
    int n = Eval(data->data->style);
    int width = 0;
    EXPRESSION* exp;
    SIZE size;

    switch (Eval(data->data->class.u.id))
    {
        SIZE cbsize, textsize;
        HDC dc;
        case CTL_BUTTON:
            switch (n & 0xf)
            {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                case BS_3STATE:
                case BS_AUTO3STATE:
                case BS_RADIOBUTTON:
                case BS_AUTORADIOBUTTON:
                    dc = GetDC(data->hwndRedir);
                    size = GetAveCharSize(dc);
                    cbsize = GetCheckBoxSize(dc);
                    textsize = GetTextSize(dc, text);
                    width = cbsize.cx + textsize.cx;
                    ReleaseDC(data->hwndRedir, dc);
                    break;
                default:
                    return;
            }
            break;
        case CTL_STATIC:
            switch (n & 0x1f)
            {
                case SS_LEFT:
                case SS_SIMPLE:
                case SS_LEFTNOWORDWRAP:
                    dc = GetDC(data->hwndRedir);
                    size = GetAveCharSize(dc);
                    textsize = GetTextSize(dc, text);
                    width = textsize.cx;
                    ReleaseDC(data->hwndRedir, dc);
                    break;
                default:
                    return;
            }
            break;
        default:
            return;
    }
    width = MulDiv(width, 4, size.cx);
    exp = data->data->width;
    if (width != Eval(exp))
    {
        UndoChanged(data->dlg, data->data);
        ResSetDirty(data->dlg);
        exp->left = exp->right = 0;
        exp->rendition = 0;
        exp->type = e_int;
        exp->val = width;
    }
}
static void CtlPropEndEdit(HWND lv, int row, HWND editWnd, struct ctlData* data)
{
    int irow = row;
    struct ctlDB* db = data->db;
    while (db)
    {
        if (db->recType != eHeader)
        {
            if (!irow--)
                break;
        }
        db = db->next;
    }
    if (db)
    {
        ResGetHeap(workArea, data->dlg);
        switch (db->ctlType)
        {
            int v;
            EXPRESSION* exp;
            char buf[256];
            case iYESNO:
            case iNEGYESNO:
            case iCombo:
                v = SendMessage(editWnd, CB_GETCURSEL, 0, 0);
                if (v != CB_ERR)
                {
                    int n;
                    v = SendMessage(editWnd, CB_GETITEMDATA, v, 0);
                    if (db->recType == eStyle)
                    {
                        n = Eval(data->data->style) & db->mask;
                    }
                    else
                    {
                        n = Eval(data->data->exstyle) & db->mask;
                    }
                    if (v)
                    {
                        struct ctlField* field = db->fields;
                        while (field && field->value != v)
                            field = field->next;
                        if (field)
                        {
                            if (n != field->value)
                            {
                                UndoChanged(data->dlg, data->data);
                                if (db->recType == eStyle)
                                {
                                    if (n)
                                        RemoveFromStyle(&data->data->style, n);
                                    AddToStyle(&data->data->style, field->valueLabel, field->value);
                                }
                                else
                                {
                                    if (n)
                                        RemoveFromStyle(&data->data->exstyle, n);
                                    AddToStyle(&data->data->exstyle, field->valueLabel, field->value);
                                }
                                ResSetDirty(data->dlg);
                            }
                        }
                    }
                    else if (n)
                    {
                        UndoChanged(data->dlg, data->data);
                        if (db->recType == eStyle)
                            RemoveFromStyle(&data->data->style, n);
                        else
                            RemoveFromStyle(&data->data->exstyle, n);
                        ResSetDirty(data->dlg);
                    }
                }
                break;
            case iX:
            case iY:
            case iWidth:
            case iHeight:
                SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
                switch (db->ctlType)
                {
                    case iX:
                        exp = data->data->x;
                        break;
                    case iY:
                        exp = data->data->y;
                        break;
                    case iWidth:
                        exp = data->data->width;
                        break;
                    case iHeight:
                        exp = data->data->height;
                        break;
                    default:
                        break;
                }
                v = strtoul(buf, NULL, 0);
                if (v != Eval(exp))
                {
                    UndoChanged(data->dlg, data->data);
                    ResSetDirty(data->dlg);
                    exp->left = exp->right = 0;
                    exp->rendition = 0;
                    exp->type = e_int;
                    exp->val = v;
                }
                break;
            case iID:
                SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
                UndoChanged(data->dlg, data->data);
                PropSetIdName(data->dlg, buf, &data->data->id, data->dlg->resource->u.dialog->controls, FALSE);
                ResSetDirty(data->dlg);
                break;
            case iText:
                SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
                {
                    char buf1[512];
                    GetCtlPropText(buf1, lv, data, row);
                    if (strcmp(buf, buf1))
                    {
                        UndoChanged(data->dlg, data->data);
                        ResSetDirty(data->dlg);
                        data->data->text->symbolic = TRUE;
                        data->data->text->u.n.length = StringAsciiToWChar(&data->data->text->u.n.symbol, buf, strlen(buf));
                        AdjustControlWidths(data, buf);
                    }
                }
                break;
            case iNone:
                break;
        }
        {
            CONTROL* c = data->data;
            struct resRes* d = data->dlg;
            EnumChildWindows(d->gd.childWindow, RemoveDlgWindow, (LPARAM)c);
            CreateSingleControl(d->gd.childWindow, d, c, FALSE);
        }
    }
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    SetFocus(data->dlg->activeHwnd);
}
static void InsertCtlProperties(HWND lv, struct ctlData* data)
{
    struct ctlDB* db = data->db;
    int gnum = 101;
    int index = 0;
    while (db)
    {
        if (db->recType == eHeader)
        {
            WCHAR bufp[256], *p = bufp;
            char* s = db->label;
            while (*s)
                *p++ = *s++;
            *p = 0;

            PropSetGroup(lv, gnum++, bufp);
        }
        db = db->next;
    }
    gnum = 100;
    db = data->db;
    while (db)
    {
        if (db->recType == eHeader)
        {
            gnum++;
        }
        else
        {
            PropSetItem(lv, index++, gnum, db->label);
        }
        db = db->next;
    }
}
static HFONT MakeFont(HDC hDC, WCHAR* font, int pointsize, int weight, int italic, int charset)
{
    return CreateFontW(-MulDiv(pointsize, GetDeviceCaps(hDC, LOGPIXELSY), 72), 0, 0, 0, weight, !!italic, FALSE, FALSE, charset,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font);
}
static void GetBaseUnits(HWND hwnd, struct resRes* dlgData, LPSIZE base)
{
    DIALOG* d = dlgData->resource->u.dialog;
    HFONT font;
    HDC hDC = GetDC(hwnd);
    TEXTMETRIC tm;
    if (dlgData->gd.font)
    {
        font = dlgData->gd.font;
    }
    else if (d->font)
    {
        font = MakeFont(hDC, d->font, Eval(d->pointsize), Eval(d->ex.weight), Eval(d->ex.italic), Eval(d->ex.charset));
    }
    else
    {
        font = CreateFontIndirect(&systemCaptionFont);
    }
    font = SelectObject(hDC, font);
    GetTextExtentPoint32(hDC, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, base);
    GetTextMetrics(hDC, &tm);
    base->cx = ((base->cx / 26) + 1) / 2;
    base->cy = tm.tmHeight >= 0 ? tm.tmHeight : -tm.tmHeight;
    font = SelectObject(hDC, font);
    dlgData->gd.font = font;
    ReleaseDC(hwnd, hDC);
}
static void ConvertToPixels(RECT* r, LPSIZE base)
{
    r->left = (r->left * base->cx) / 4;
    r->top = (r->top * base->cy) / 8;
    r->right = (r->right * base->cx) / 4;
    r->bottom = (r->bottom * base->cy) / 8;
}
static void ConvertToDlgUnits(RECT* r, LPSIZE base)
{
    r->left = r->left * 4 / base->cx;
    r->top = r->top * 8 / base->cy;
    r->right = r->right * 4 / base->cx;
    r->bottom = r->bottom * 8 / base->cy;
}
static HWND CreateDlgWindow(HWND hwnd, struct resRes* dlgData)
{
    DIALOG* d = dlgData->resource->u.dialog;
    char buf[256];
    SIZE base;
    RECT r;
    HWND rv;
    GetBaseUnits(hwnd, dlgData, &base);
    r.left = Eval(d->x);
    r.top = Eval(d->y);
    r.right = Eval(d->width) + r.left;
    r.bottom = Eval(d->height) + r.top;
    ConvertToPixels(&r, &base);
    r.right += GetSystemMetrics(SM_CXDLGFRAME) * 2;
    r.bottom += GetSystemMetrics(SM_CYDLGFRAME) * 2;
    if ((Eval(d->style) & WS_CAPTION) == WS_CAPTION)
        r.bottom += GetSystemMetrics(SM_CYCAPTION);
    buf[0] = 0;
    if (d->caption)
    {
        StringWToA(buf, d->caption, wcslen(d->caption));
    }
    rv = CreateWindowEx(Eval(d->exstyle), szDlgDialogClassName, buf,
                        (Eval(d->style) | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN) & ~WS_POPUP, r.left, r.top, r.right - r.left,
                        r.bottom - r.top, hwnd, 0, hInstance, dlgData);
    if (rv)
    {
        HMENU hMenu = GetSystemMenu(rv, FALSE);
        EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | MF_GRAYED);
    }
    return rv;
}
static void ShowOrHideScrollBars(HWND hwnd, struct resRes* dlgData)
{
    RECT cr;
    POINT pt1, pt2;
    GetWindowRect(dlgData->gd.childWindow, &cr);
    pt1.x = cr.left + dlgData->gd.scrollPos.x;
    pt1.y = cr.top + dlgData->gd.scrollPos.y;
    ScreenToClient(hwnd, &pt1);
    dlgData->gd.origin = pt1;
    pt2.x = cr.right + dlgData->gd.scrollPos.x;
    pt2.y = cr.bottom + dlgData->gd.scrollPos.y;
    ScreenToClient(hwnd, &pt2);
    dlgData->gd.scrollMax = pt2;
    GetClientRect(hwnd, &cr);
    ShowScrollBar(hwnd, SB_HORZ, pt2.x >= cr.right);
    ShowScrollBar(hwnd, SB_VERT, pt2.y >= cr.bottom);
    if (pt2.x >= cr.right)
    {
        SetScrollRange(hwnd, SB_HORZ, 0, pt2.x, TRUE);
        SetScrollPos(hwnd, SB_HORZ, dlgData->gd.scrollPos.x, TRUE);
    }
    if (pt2.y >= cr.bottom)
    {
        SetScrollRange(hwnd, SB_VERT, 0, pt2.y, TRUE);
        SetScrollPos(hwnd, SB_VERT, dlgData->gd.scrollPos.y, TRUE);
    }
}
static void DrawRect(HDC dc, RECT* r)
{
    MoveToEx(dc, r->left, r->top, NULL);
    LineTo(dc, r->right, r->top);
    MoveToEx(dc, r->right - 1, r->top, NULL);
    LineTo(dc, r->right - 1, r->bottom - 1);
    MoveToEx(dc, r->right - 1, r->bottom - 1, NULL);
    LineTo(dc, r->left, r->bottom - 1);
    MoveToEx(dc, r->left, r->bottom - 1, NULL);
    LineTo(dc, r->left, r->top);
}
void ChangePosition(struct resRes* dlgData, EXPRESSION** pos, int offset)
{
    if (offset != 0)
    {
        char buf[256];
        ResGetHeap(workArea, dlgData);
        if ((*pos)->type == e_int)
        {
            (*pos)->val += offset;
            sprintf(buf, "%d", (*pos)->val);
            (*pos)->rendition = rcStrdup(buf);
            if (snapToGrid)
                (*pos)->val = ((*pos)->val / gridSpacing) * gridSpacing;
        }
        else if ((*pos)->type == plus && (*pos)->right->type == e_int)
        {
            (*pos)->right->val += offset;
            sprintf(buf, "%d", (*pos)->right->val);
            (*pos)->right->rendition = rcStrdup(buf);
            if (snapToGrid)
                (*pos)->right->val = ((*pos)->right->val / gridSpacing) * gridSpacing;
        }
        else if (snapToGrid)
        {
            (*pos)->val = ((Eval(*pos) + offset) / gridSpacing) * gridSpacing;
            (*pos)->type = e_int;
            (*pos)->left = (*pos)->right = 0;
            sprintf(buf, "%d", (*pos)->val);
            (*pos)->rendition = rcStrdup(buf);
        }
        else
        {
            EXPRESSION* n;
            n = rcAlloc(sizeof(EXPRESSION));
            n->left = *pos;
            n->right = rcAlloc(sizeof(EXPRESSION));
            *pos = n;
            n->type = plus;
            n->right->type = e_int;
            n->right->val = offset;
            sprintf(buf, "%d", n->right->val);
            n->right->rendition = rcStrdup(buf);
        }
    }
}
static void UndoGroupStart(struct resRes* dlgData)
{
    struct dlgUndo* cur = calloc(1, sizeof(struct dlgUndo));
    if (cur)
    {
        cur->type = du_groupstart;
        cur->next = dlgData->gd.undoData;
        dlgData->gd.undoData = cur;
    }
}
static void UndoGroupEnd(struct resRes* dlgData)
{
    struct dlgUndo* cur = calloc(1, sizeof(struct dlgUndo));
    if (cur)
    {
        cur->type = du_groupend;
        cur->next = dlgData->gd.undoData;
        dlgData->gd.undoData = cur;
    }
}
static void UndoInsert(struct resRes* dlgData, CONTROL* list)
{
    struct dlgUndo* cur = calloc(1, sizeof(struct dlgUndo));
    if (cur)
    {
        CONTROL** item = &dlgData->resource->u.dialog->controls;
        while (*item && *item != list)
            item = &(*item)->next;
        cur->type = du_insert;
        cur->place = item;
        cur->next = dlgData->gd.undoData;
        dlgData->gd.undoData = cur;
    }
}
static BOOL CALLBACK UndoDeleteOne(HWND hwnd, LPARAM lParam)
{
    struct resRes* dlgData = (struct resRes*)lParam;
    struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
    data->data->deleteme = SendMessage(hwnd, WM_QUERYSELECTEDSTATE, 0, 0);
    return TRUE;
}
static void UndoDeleteOneReally(struct resRes* dlgData, CONTROL** c)
{
    // this walks the list backwards...
    if ((*c)->next)
        UndoDeleteOneReally(dlgData, &(*c)->next);
    if ((*c)->deleteme)
    {
        struct dlgUndo* cur = calloc(1, sizeof(struct dlgUndo));
        if (cur)
        {
            cur->type = du_delete;
            cur->place = c;
            cur->item = *c;
            cur->next = dlgData->gd.undoData;
            dlgData->gd.undoData = cur;
        }
    }
}
static void UndoDeleteActive(struct resRes* dlgData)
{
    UndoGroupStart(dlgData);
    EnumChildWindows(dlgData->gd.childWindow, UndoDeleteOne, (LPARAM)dlgData);
    UndoDeleteOneReally(dlgData, &dlgData->resource->u.dialog->controls);
    UndoGroupEnd(dlgData);
}
static EXPRESSION* dupexpr(EXPRESSION* in)
{
    EXPRESSION* rv;
    if (!in)
        return in;
    rv = rcAlloc(sizeof(EXPRESSION));
    if (in->rendition)
        rv->rendition = rcStrdup(in->rendition);
    rv->type = in->type;
    rv->val = in->val;
    rv->left = dupexpr(in->left);
    rv->right = dupexpr(in->right);
    return rv;
}
static void UndoChanged(struct resRes* dlgData, CONTROL* list)
{
    struct dlgUndo* cur = calloc(1, sizeof(struct dlgUndo));
    if (cur)
    {
        cur->type = du_changed;
        cur->saved = calloc(1, sizeof(CONTROL));
        if (cur->saved)
        {
            cur->saved->generic = list->generic;
            cur->saved->baseStyle = list->baseStyle;
            cur->saved->class = list->class;
            cur->saved->text = list->text;
            cur->saved->id = dupexpr(list->id);
            cur->saved->style = dupexpr(list->style);
            cur->saved->exstyle = dupexpr(list->exstyle);
            cur->saved->x = dupexpr(list->x);
            cur->saved->y = dupexpr(list->y);
            cur->saved->width = dupexpr(list->width);
            cur->saved->height = dupexpr(list->height);
            cur->saved->help = dupexpr(list->help);
            cur->item = list;
            cur->next = dlgData->gd.undoData;
            dlgData->gd.undoData = cur;
        }
        else
        {
            free(cur);
        }
    }
}
static BOOL CALLBACK UndoMoveOne(HWND hwnd, LPARAM lParam)
{
    struct resRes* dlgData = (struct resRes*)lParam;
    if (SendMessage(hwnd, WM_QUERYSELECTEDSTATE, 0, 0))
    {
        struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
        UndoChanged(dlgData, data->data);
    }
    return TRUE;
}
static void UndoMoveActive(struct resRes* dlgData)
{
    UndoGroupStart(dlgData);
    EnumChildWindows(dlgData->gd.childWindow, UndoMoveOne, (LPARAM)dlgData);
    UndoGroupEnd(dlgData);
}
static void UndoReorder(struct resRes* dlgData)
{
    LIST *val = NULL, **cv = &val;
    struct dlgUndo* cur = calloc(1, sizeof(struct dlgUndo));
    CONTROL* list = dlgData->resource->u.dialog->controls;
    while (list)
    {
        *cv = calloc(1, sizeof(LIST));
        (*cv)->data = list;
        list = list->next;
        cv = &(*cv)->next;
    }
    if (cur)
    {
        cur->type = du_reorder;
        cur->order = val;
        cur->next = dlgData->gd.undoData;
        dlgData->gd.undoData = cur;
    }
}
static void CleanChildSpace(HWND hwnd, HWND parent)
{
    POINT pt1, pt2;
    RECT cl;
    GetWindowRect(hwnd, &cl);
    pt1.x = cl.left;
    pt1.y = cl.top;
    pt2.x = cl.right;
    pt2.y = cl.bottom;
    ScreenToClient(parent, &pt1);
    ScreenToClient(parent, &pt2);
    cl.left = pt1.x - 3;
    cl.top = pt1.y - 3;
    cl.right = pt2.x + 3;
    cl.bottom = pt2.y + 3;
    InvalidateRect(parent, &cl, TRUE);
}
static BOOL CALLBACK RemoveDlgWindow(HWND hwnd, LPARAM lParam)
{
    CONTROL* c = (CONTROL*)lParam;
    struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
    if (data->data == c)
    {
        CleanChildSpace(hwnd, data->dlg->gd.childWindow);
        DestroyWindow(hwnd);
        return FALSE;
    }
    return TRUE;
}
static void DlgUndoOne(struct resRes* dlgData)
{
    struct dlgUndo* undo = dlgData->gd.undoData;
    if (undo)
    {
        dlgData->gd.undoData = undo->next;
        switch (undo->type)
        {
            LIST* order;
            CONTROL* c;
            case du_groupstart:
            case du_groupend:
                break;
            case du_insert:
                c = *undo->place;
                *undo->place = NULL;
                if (dlgData->gd.dlgAltMode == DA_NONE)
                {
                    while (c)
                    {
                        EnumChildWindows(dlgData->gd.childWindow, RemoveDlgWindow, (LPARAM)c);
                        c = c->next;
                    }
                }
                else
                {
                    InvalidateRect(dlgData->gd.childWindow, NULL, FALSE);
                }
                break;
            case du_delete:
                undo->item->next = *undo->place;
                *undo->place = undo->item;
                if (dlgData->gd.dlgAltMode == DA_NONE)
                {
                    CreateSingleControl(dlgData->gd.childWindow, dlgData, undo->item, TRUE);
                }
                else
                {
                    InvalidateRect(dlgData->gd.childWindow, NULL, FALSE);
                }
                break;
            case du_changed:
                undo->saved->next = undo->item->next;
                *undo->item = *undo->saved;
                if (dlgData->gd.dlgAltMode == DA_NONE)
                {
                    EnumChildWindows(dlgData->gd.childWindow, RemoveDlgWindow, (LPARAM)undo->item);
                    CreateSingleControl(dlgData->gd.childWindow, dlgData, undo->item, FALSE);
                }
                else
                {
                    InvalidateRect(dlgData->gd.childWindow, NULL, FALSE);
                }
                break;
            case du_reorder:
                dlgData->gd.selectedColumn = 0;
                dlgData->resource->u.dialog->controls = undo->order->data;
                order = undo->order;
                while (order)
                {
                    if (order->next)
                        ((CONTROL*)order->data)->next = order->next->data;
                    else
                        ((CONTROL*)order->data)->next = NULL;
                    order = order->next;
                }
                InvalidateRect(dlgData->gd.childWindow, NULL, TRUE);
                break;
        }
        if (undo->saved)
            free(undo->saved);
        if (undo->order)
        {
            while (undo->order)
            {
                LIST* next = undo->order->next;
                free(undo->order);
                undo->order = next;
            }
        }
        free(undo);
        if (dlgData->gd.cantClearUndo)
            ResSetDirty(dlgData);
        else if (!dlgData->gd.undoData)
            ResSetClean(dlgData);
    }
}
static BOOL CALLBACK ActivateSizing(HWND child, LPARAM param)
{
    SendMessage(child, WM_ACTIVATESIZERECTANGLE, 0, 0);
    return TRUE;
}
static void DlgDoUndo(struct resRes* dlgData)
{
    struct dlgUndo* undo = dlgData->gd.undoData;
    if (undo)
    {
        ResGetHeap(workArea, dlgData);
        EnumChildWindows(dlgData->gd.childWindow, ActivateSizing, 0);
        while (undo && undo->type == du_groupend && undo->next && undo->next->type == du_groupstart)
        {
            DlgUndoOne(dlgData);
            DlgUndoOne(dlgData);
            undo = dlgData->gd.undoData;
        }
        if (undo)
        {
            if (undo->type == du_groupend)
            {
                int type;
                do
                {
                    undo = dlgData->gd.undoData;
                    type = undo->type;
                    DlgUndoOne(dlgData);
                } while (type != du_groupstart && dlgData->gd.undoData);
            }
            else
            {
                DlgUndoOne(dlgData);
            }
        }
        PropsWndRedraw();
    }
}
LRESULT CALLBACK DlgControlInputRedirProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    LRESULT rv;
    struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
    WNDPROC oldProc = data->oldWndProcRedir;
    switch (iMessage)
    {
        case WM_NCHITTEST:
            rv = CallWindowProc(oldProc, hwnd, iMessage, wParam, lParam);
            if (rv == HTTRANSPARENT)  // this is for the group box.  We also have
                                      // problems with static controls but use SS_NOTIFY there
                rv = HTCLIENT;
            return rv;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_NCMOUSEMOVE:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
        case WM_NCMOUSEHOVER:
        case WM_NCMOUSELEAVE:
        case WM_MOUSEMOVE:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ClientToScreen(hwnd, &pt);
            ScreenToClient(data->hwndParent, &pt);
            lParam = MAKELPARAM(pt.x, pt.y);
            return SendMessage(data->hwndParent, iMessage, wParam, lParam);
        }
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
            return SendMessage(data->hwndParent, iMessage, wParam, lParam);
        case WM_QUERYCTLDROPTARGET:
            return 1;
        case WM_CTLDROP:
            return SendMessage(data->hwndParent, iMessage, wParam, lParam);
        case WM_DESTROY:
            SetWindowLong(hwnd, GWL_WNDPROC, (long)oldProc);
            free(data);
            break;
        default:
            break;
    }
    return CallWindowProc(oldProc, hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK DlgControlInputProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    LRESULT rv;
    static int flags, oldflags;
    static HCURSOR oldCurs;
    static POINT start, delta;
    static BOOL moving;
    static LPARAM lastlParam;
    static RECT lastSizeRect;
    RECT r1, client;
    struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (iMessage)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            struct ctlDB* db = GetCtlDb(data->data);
            lastlParam = lParam;
            if (db)
            {
                currentDB = db;
                data->db = db;
                SetResourceProperties(data, &CtlFuncs);
            }
            else
            {
                SetResourceProperties(NULL, NULL);
            }
        }
            if (!data->sizing)
            {
                SendMessage(data->dlg->gd.childWindow, WM_ACTIVATESIZERECTANGLE, 0, 0);
                SendMessage(hwnd, WM_ACTIVATESIZERECTANGLE, TRUE, 0);
            }
            if (flags)  // in a sizing region
            {
                flags |= SIZING;
                start.x = GET_X_LPARAM(lParam);
                start.y = GET_Y_LPARAM(lParam);
                delta.x = delta.y = 0;
                return 0;
            }
            // fall through
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            lastlParam = lParam;
            if (flags & SIZING)
            {
                SIZE base;
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
                flags &= ~(SIZING | SIZINGUNDODONE);
                memset(&r1, 0, sizeof(r1));
                if (flags & F_TOP)
                {
                    r1.top = pt.y - start.y + delta.y;
                    delta.y = r1.top;
                }
                else if (flags & F_BOTTOM)
                {
                    r1.bottom = pt.y - start.y;
                }
                else if (flags & F_LEFT)
                {
                    r1.left = pt.x - start.x + delta.x;
                    delta.x = r1.left;
                }
                else
                {
                    r1.right = pt.x - start.x;
                }
                GetBaseUnits(data->dlg->gd.childWindow, data->dlg, &base);
                ConvertToDlgUnits(&r1, &base);
                SendMessage(hwnd, WM_RELMOVE, TRUE, (LPARAM)&r1);
            }
            else
            {
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
                ClientToScreen(hwnd, &pt);
                ScreenToClient(data->dlg->gd.childWindow, &pt);
                SendMessage(data->dlg->gd.childWindow, WM_POSTBUTTONSTATE, iMessage, MAKELPARAM(pt.x, pt.y));
            }
            return 0;
        case WM_QUERYSELECTEDSTATE:
            return data->sizing;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
        case WM_NCMOUSEMOVE:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
        case WM_NCMOUSEHOVER:
        case WM_NCMOUSELEAVE:
            return 0;
        case WM_MOUSEMOVE:
            if (lastlParam == lParam)
                return 0;
            lastlParam = lParam;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            if (flags & SIZING)
            {
                SIZE base;
                if (!(flags & SIZINGUNDODONE))
                {
                    flags |= SIZINGUNDODONE;
                    UndoMoveActive(data->dlg);  // size
                }
                memset(&r1, 0, sizeof(r1));
                if (flags & F_TOP)
                {
                    r1.top = pt.y - start.y + delta.y;
                    delta.y = r1.top;
                }
                else if (flags & F_BOTTOM)
                {
                    r1.bottom = pt.y - start.y;
                }
                else if (flags & F_LEFT)
                {
                    r1.left = pt.x - start.x + delta.x;
                    delta.x = r1.left;
                }
                else
                {
                    r1.right = pt.x - start.x;
                }
                GetBaseUnits(data->dlg->gd.childWindow, data->dlg, &base);
                ConvertToDlgUnits(&r1, &base);
                SendMessage(hwnd, WM_RELMOVE, FALSE, (LPARAM)&r1);
            }
            else if (data->sizing)
            {
                oldflags = flags;
                flags = 0;
                GetClientRect(hwnd, &client);
                r1.left = (client.right + client.left) / 2 - 3;
                r1.right = r1.left + 7;
                r1.top = client.top - 3;
                r1.bottom = client.top + 4;
                if (PtInRect(&r1, pt))
                    flags = F_TOP;
                r1.left = (client.right + client.left) / 2 - 3;
                r1.right = r1.left + 7;
                r1.top = client.bottom - 4;
                r1.bottom = client.bottom + 3;
                if (PtInRect(&r1, pt))
                    flags = F_BOTTOM;

                r1.left = client.left - 3;
                r1.right = client.left + 4;
                r1.top = (client.top + client.bottom) / 2 - 3;
                r1.bottom = r1.top + 7;
                if (PtInRect(&r1, pt))
                    flags = F_LEFT;

                r1.left = client.right - 4;
                r1.right = client.right + 3;
                r1.top = (client.top + client.bottom) / 2 - 3;
                r1.bottom = r1.top + 7;
                if (PtInRect(&r1, pt))
                    flags = F_RIGHT;
                if (flags != oldflags)
                {
                    if (oldflags == 0)
                    {
                        SetCapture(hwnd);
                        oldCurs = GetCursor();
                    }
                    if (flags & (F_TOP | F_BOTTOM))
                    {
                        SetCursor(vcurs);
                    }
                    else if (flags & (F_LEFT | F_RIGHT))
                    {
                        SetCursor(hcurs);
                    }
                    else if (!flags)
                    {
                        SetCursor(oldCurs);
                        ReleaseCapture();
                    }
                }
            }
            ClientToScreen(hwnd, &pt);
            ScreenToClient(data->dlg->gd.childWindow, &pt);
            SendMessage(data->dlg->gd.childWindow, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
            return 0;
        case WM_PAINT:
            rv = DefWindowProc(hwnd, iMessage, wParam, lParam);
            if (data && data->sizing)
            {
                RECT r, r1;
                HDC dc;
                HPEN pen = CreatePen(PS_DOT, 0, 0);
                GetClientRect(hwnd, &r);
                dc = GetDC(hwnd);
                pen = SelectObject(dc, pen);
                DrawRect(dc, &r);
                pen = SelectObject(dc, pen);
                r1.left = (r.right + r.left) / 2 - 3;
                r1.right = r1.left + 7;
                r1.top = r.top - 3;
                r1.bottom = r.top + 4;
                DrawRect(dc, &r1);
                r1.left = (r.right + r.left) / 2 - 3;
                r1.right = r1.left + 7;
                r1.top = r.bottom - 4;
                r1.bottom = r.bottom + 3;
                DrawRect(dc, &r1);
                r1.left = r.left - 3;
                r1.right = r.left + 4;
                r1.top = (r.top + r.bottom) / 2 - 3;
                r1.bottom = r1.top + 7;
                DrawRect(dc, &r1);
                r1.left = r.right - 4;
                r1.right = r.right + 3;
                r1.top = (r.top + r.bottom) / 2 - 3;
                r1.bottom = r1.top + 7;
                DrawRect(dc, &r1);
                DeleteObject(pen);
                ReleaseDC(hwnd, dc);
            }
            return rv;
        case WM_ACTIVATESIZERECTANGLE:
            if (data && data->sizing != !!wParam)
            {
                data->sizing = !!wParam;
                CleanChildSpace(hwnd, data->dlg->gd.childWindow);
                if (data->data->isCombo)
                {
                    RECT r;
                    if (wParam)
                    {
                        SIZE base;
                        GetBaseUnits(data->dlg->gd.childWindow, data->dlg, &base);
                        r.left = Eval(data->data->x);
                        r.top = Eval(data->data->y);
                        r.right = Eval(data->data->width) + r.left;
                        r.bottom = Eval(data->data->height) + r.top;
                        ConvertToPixels(&r, &base);
                    }
                    else
                    {
                        POINT x = {0};
                        GetWindowRect(data->hwndRedir, &r);
                        x.x = r.left;
                        x.y = r.top;
                        ScreenToClient(GetParent(hwnd), &x);
                        r.left = x.x - 1;
                        r.top = x.y - 1;
                        x.x = r.right;
                        x.y = r.bottom;
                        ScreenToClient(GetParent(hwnd), &x);
                        r.right = x.x - 1;
                        r.bottom = x.y - 1;
                    }
                    MoveWindow(hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
                }
                InvalidateRect(hwnd, NULL, TRUE);
                /*
                if (wParam)
                {
                    data->hwndPrev = GetWindow(hwnd, GW_HWNDPREV);
                    SetWindowPos(hwnd, HWND_TOP, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
                }
                else
                {
                    SetWindowPos(hwnd, data->hwndPrev, 0,0,0,0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
                }
                */
            }
            return 0;
        case WM_QUERYCTLDROPTARGET:
            return 1;
        case WM_CTLDROP:
            return SendMessage(data->dlg->gd.childWindow, iMessage, wParam, lParam);
        case WM_DESTROY:
            free(data);
            break;
        case WM_RELMOVE:
        {
            RECT* p = (RECT*)lParam;  // in dlg units...
            RECT r;
            SIZE base;
            GetBaseUnits(data->dlg->gd.childWindow, data->dlg, &base);
            r.left = Eval(data->data->x);
            r.top = Eval(data->data->y);
            r.right = Eval(data->data->width) + r.left;
            r.bottom = Eval(data->data->height) + r.top;
            r.left += p->left;
            r.right += p->right;
            r.top += p->top;
            r.bottom += p->bottom;
            if (snapToGrid)
            {
                int width;
                int height;
                r.left++;
                r.top++;
                r.right--;
                r.bottom--;
                width = r.right - r.left;
                height = r.bottom - r.top;
                if (p->left)
                    r.left = (r.left / gridSpacing) * gridSpacing;
                if (p->top)
                    r.top = (r.top / gridSpacing) * gridSpacing;
                if (p->bottom && p->top != p->bottom)
                    r.bottom = (r.bottom / gridSpacing) * gridSpacing;
                else
                    r.bottom = r.top + height;
                if (p->right && p->left != p->right)
                    r.right = (r.right / gridSpacing) * gridSpacing;
                else
                    r.right = r.left + width;
                r.left--;
                r.top--;
                r.right++;
                r.bottom++;
            }
            ConvertToPixels(&r, &base);
            lastSizeRect = r;
            if (wParam)
            {
                ChangePosition(data->dlg, &data->data->x, p->left);
                ChangePosition(data->dlg, &data->data->y, p->top);
                ChangePosition(data->dlg, &data->data->width, p->right - p->left);
                ChangePosition(data->dlg, &data->data->height, p->bottom - p->top);
                ResSetDirty(data->dlg);
                PropsWndRedraw();
            }
            CleanChildSpace(hwnd, data->dlg->gd.childWindow);
            MoveWindow(hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
            MoveWindow(data->hwndRedir, 1, 1, r.right - r.left - 2, r.bottom - r.top - 2, TRUE);
        }
            return 0;
        case WM_MOVE:
            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static BOOL CALLBACK HookAllChildren(HWND child, LPARAM old)
{
    struct ctlData* data = calloc(1, sizeof(struct ctlData));
    *data = *(struct ctlData*)old;
    data->oldWndProcRedir = (WNDPROC)GetWindowLong(child, GWL_WNDPROC);
    SetWindowLong(child, GWL_USERDATA, (long)data);
    SetWindowLong(child, GWL_WNDPROC, (long)DlgControlInputRedirProc);
    EnumChildWindows(child, HookAllChildren, (LPARAM)data);
    return TRUE;
}
static void CreateSingleControl(HWND hwnd, struct resRes* dlgData, CONTROL* c, BOOL activate)
{
    WCHAR* name;
    SIZE base;
    RECT r;
    HWND child, parent;
    int plusstyle;
    char className[256], classText[256];
    struct ctlData* data = calloc(1, sizeof(struct ctlData));
    if (!data)
    {
        return;
    }
    data->data = c;
    data->dlg = dlgData;
    data->db = currentDB;
    GetBaseUnits(hwnd, dlgData, &base);
    r.left = Eval(c->x);
    r.top = Eval(c->y);
    r.right = Eval(c->width) + r.left;
    r.bottom = Eval(c->height) + r.top;
    ConvertToPixels(&r, &base);
    if (c->class.symbolic)
        name = c->class.u.n.symbol;
    else
        switch (Eval(c->class.u.id))
        {
            case CTL_BUTTON:
                name = L"button";
                break;
            case CTL_EDIT:
                name = L"edit";
                break;
            case CTL_STATIC:
                name = L"static";
                break;
            case CTL_LISTBOX:
                name = L"listbox";
                break;
            case CTL_SCROLLBAR:
                name = L"scrollbar";
                break;
            case CTL_COMBOBOX:
                name = L"combobox";
                break;
            default:
                name = L"???UNKNOWN???WINDOW???CLASS???";
                break;
        }
    StringWToA(className, name, wcslen(name));
    if (!strcmp(className, "combobox"))
        c->isCombo = TRUE;
    if (c->text && c->text->symbolic)  // also need to handle retrieving the text by id...
        StringWToA(classText, c->text->u.n.symbol, c->text->u.n.length);
    else
        strcpy(classText, "{}");
    if (!stricmp(className, "static"))
        plusstyle = SS_NOTIFY;
    else if (!stricmp(className, "button"))
        plusstyle = BS_NOTIFY;
    else
        plusstyle = 0;
    parent = CreateWindowEx(0, szDlgControlInputClassName, "", (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS), r.left - 1, r.top - 1,
                            r.right - r.left + 2, r.bottom - r.top + 2, hwnd, 0, hInstance, 0);
    data->hwndParent = parent;
    SetWindowLong(parent, GWL_USERDATA, (long)data);
    child = CreateWindowEx(Eval(c->exstyle), className, classText, (Eval(c->style) | plusstyle | WS_CHILD | WS_VISIBLE), 1, 1,
                           r.right - r.left, r.bottom - r.top, parent, 0, hInstance, 0);
    if (!child)
        child = CreateWindowEx(Eval(c->exstyle), szDlgUnknownClassName, classText, (Eval(c->style) | WS_CHILD | WS_VISIBLE), 1, 1,
                               r.right - r.left, r.bottom - r.top, parent, 0, hInstance, 0);
    SetWindowTheme(child, L" ", L" ");
    if (child)
    {
        data->hwndRedir = child;
        HookAllChildren(child, (LPARAM)data);
        SendMessage(child, WM_ACTIVATESIZERECTANGLE, activate, 0);
        SendMessage(child, WM_SETFONT, (WPARAM)dlgData->gd.font, 0);
        if (c->isCombo)  // force a resize down to the 'slim' size
            data->sizing = TRUE;
        SendMessage(parent, WM_ACTIVATESIZERECTANGLE, activate, 0);
    }
}
static void CreateControls(HWND hwnd, struct resRes* dlgData)
{

    CONTROL* c = dlgData->resource->u.dialog->controls;
    while (c)
    {
        CreateSingleControl(hwnd, dlgData, c, FALSE);
        c = c->next;
    }
}
static BOOL CALLBACK SelectChildren(HWND child, LPARAM param)
{
    RECT* r = (RECT*)param;
    RECT cr;
    POINT start, end;
    BOOL active;
    GetWindowRect(child, &cr);
    start.x = cr.left;
    start.y = cr.top;
    end.x = cr.right;
    end.y = cr.bottom;
    active = PtInRect(r, start) && PtInRect(r, end);
    SendMessage(child, WM_ACTIVATESIZERECTANGLE, active, 0);
    return TRUE;
}
static BOOL CALLBACK MoveChildren(HWND child, LPARAM param)
{
    if (SendMessage(child, WM_QUERYSELECTEDSTATE, 0, 0))
    {
        SendMessage(child, WM_RELMOVE, 0, param);
    }
    return TRUE;
}
static BOOL CALLBACK MoveChildrenCommit(HWND child, LPARAM param)
{
    if (SendMessage(child, WM_QUERYSELECTEDSTATE, 0, 0))
    {
        SendMessage(child, WM_RELMOVE, 1, param);
    }
    return TRUE;
}
BOOL RemoveFromStyle(EXPRESSION** style, int val)
{
    if ((*style)->type == e_int)
    {
        if ((*style)->val == val)
        {
            *style = NULL;
            return TRUE;
        }
        else if ((*style)->val & val)
        {
            (*style)->val &= ~val;
            (*style)->rendition = NULL;
            return TRUE;
        }
    }
    else
    {
        if ((*style)->type == or || (*style)->type == plus)
        {
            if (RemoveFromStyle(&(*style)->left, val))
            {
                if ((*style)->left == NULL)
                    *style = (*style)->right;
                return TRUE;
            }
            else if (RemoveFromStyle(&(*style)->right, val))
            {
                if ((*style)->right == NULL)
                    *style = (*style)->left;
                return TRUE;
            }
        }
    }
    return FALSE;
}
void AddToStyle(EXPRESSION** style, char* text, int val)
{
    EXPRESSION* th = rcAlloc(sizeof(EXPRESSION));
    if (!*style)
    {
        *style = th;
    }
    else
    {
        EXPRESSION* top = rcAlloc(sizeof(EXPRESSION));
        top->type = or ;
        top->right = *style;
        top->left = th;
        *style = top;
    }
    th->type = e_int;
    th->val = val;
    if (text)
        th->rendition = rcStrdup(text);
}
static void InsertNewControl(struct resRes* dlgData, int type, POINT at)
{
    SIZE base;
    CONTROL *c, **parent = &dlgData->resource->u.dialog->controls;
    RECT r;
    int classtype;
    char* text = NULL;
    ResGetHeap(workArea, dlgData);
    c = rcAlloc(sizeof(CONTROL));
    while (*parent)
        parent = &(*parent)->next;
    *parent = c;
    GetBaseUnits(dlgData->gd.childWindow, dlgData, &base);
    memset(&r, 0, sizeof(r));
    AddToStyle(&c->style, "WS_CHILD", WS_TABSTOP);
    AddToStyle(&c->style, "WS_VISIBLE", WS_TABSTOP);
    switch (type)
    {
        case 0:  // push button
        default:
            AddToStyle(&c->style, "WS_TABSTOP", WS_TABSTOP);
            AddToStyle(&c->style, "BS_PUSHBUTTON", BS_PUSHBUTTON);
            classtype = CTL_BUTTON;
            text = "Push Button";
            break;
        case 1:  // default push button
            AddToStyle(&c->style, "WS_TABSTOP", WS_TABSTOP);
            AddToStyle(&c->style, "BS_DEFPUSHBUTTON", BS_DEFPUSHBUTTON);
            classtype = CTL_BUTTON;
            text = "Default Push Button";
            break;
        case 2:  // check box
            AddToStyle(&c->style, "WS_TABSTOP", WS_TABSTOP);
            AddToStyle(&c->style, "BS_AUTOCHECKBOX", BS_AUTOCHECKBOX);
            classtype = CTL_BUTTON;
            text = "Check Box";
            r.bottom = MulDiv(10, base.cy, 8);
            break;
        case 3:  // Radio Button
            AddToStyle(&c->style, "BS_AUTORADIOBUTTON", BS_AUTORADIOBUTTON);
            classtype = CTL_BUTTON;
            text = "Radio Button";
            r.bottom = 10;
            break;
        case 4:  // Static Text
            AddToStyle(&c->style, "WS_GROUP", WS_GROUP);
            AddToStyle(&c->style, "SS_LEFT", SS_LEFT);
            classtype = CTL_STATIC;
            text = "Static Text";
            r.right = 20 + strlen(text) * base.cx;
            r.bottom = base.cy;
            break;
        case 5:  // Icon
            AddToStyle(&c->style, "SS_ICON", SS_ICON);
            classtype = CTL_STATIC;
            r.right = 32;
            r.bottom = 32;
            break;
        case 6:  // Edit box
            AddToStyle(&c->style, "WS_TABSTOP", WS_TABSTOP);
            AddToStyle(&c->style, "WS_BORDER", WS_BORDER);
            AddToStyle(&c->style, "ES_LEFT", ES_LEFT);
            classtype = CTL_EDIT;
            text = "Edit Box";
            r.bottom = MulDiv(14, base.cy, 8);
            break;
        case 7:  // combo box
            AddToStyle(&c->style, "WS_TABSTOP", WS_TABSTOP);
            AddToStyle(&c->style, "CBS_DROPDOWNLIST", CBS_DROPDOWNLIST);
            classtype = CTL_COMBOBOX;
            text = "Combo Box";
            r.right = 20 + strlen(text) * base.cx;
            r.bottom = 4 * base.cy;
            break;
        case 8:  // list box
            AddToStyle(&c->style, "WS_TABSTOP", WS_TABSTOP);
            AddToStyle(&c->style, "WS_BORDER", WS_BORDER);
            AddToStyle(&c->style, "WS_VSCROLL", WS_VSCROLL);
            AddToStyle(&c->style, "LBS_NOTIFY", LBS_NOTIFY);
            classtype = CTL_LISTBOX;
            text = "List Box";
            r.right = 100;
            r.bottom = 100;
            break;
        case 9:  // group box
            AddToStyle(&c->style, "BS_GROUPBOX", BS_GROUPBOX);
            classtype = CTL_BUTTON;
            text = "Group Box";
            r.right = 100;
            r.bottom = 100;
            break;
        case 10:  // frame
            AddToStyle(&c->style, "SS_GRAYFRAME", SS_GRAYFRAME);
            classtype = CTL_STATIC;
            r.right = 50;
            r.bottom = 50;

            break;
        case 11:  // rectangle
            AddToStyle(&c->style, "SS_GRAYRECT", SS_GRAYRECT);
            classtype = CTL_STATIC;
            r.right = 50;
            r.bottom = 50;
            break;
        case 12:  // etched
            AddToStyle(&c->style, "SS_ETCHEDHORZ", SS_ETCHEDHORZ);
            classtype = CTL_STATIC;
            r.right = 50;
            r.bottom = 20;
            break;
        case 13:  // horizontal scroll
            AddToStyle(&c->style, "SBS_HORZ", SBS_HORZ);
            classtype = CTL_SCROLLBAR;
            r.right = 100;
            r.bottom = GetSystemMetrics(SM_CYHSCROLL);
            break;
        case 14:  // vertical scroll
            AddToStyle(&c->style, "SBS_VERT", SBS_VERT);
            classtype = CTL_SCROLLBAR;
            r.right = GetSystemMetrics(SM_CXVSCROLL);
            r.bottom = 100;
            break;
    }
    r.left = at.x;
    r.top = at.y;
    if (text)
    {
        c->text = rcAlloc(sizeof(IDENT));
        c->text->symbolic = TRUE;
        StringAsciiToWChar(&c->text->u.n.symbol, text, strlen(text));
        c->text->u.n.length = strlen(text);
        if (r.right == 0)
        {
            r.right = 20 + strlen(text) * base.cx;
        }
        if (r.bottom == 0)
        {
            r.bottom = 2 * base.cy;
        }
    }
    r.right += r.left;
    r.bottom += r.top;
    c->class.symbolic = FALSE;
    c->class.u.id = rcAlloc(sizeof(EXPRESSION));
    c->class.u.id->type = e_int;
    c->class.u.id->val = classtype;

    ConvertToDlgUnits(&r, &base);
    c->x = rcAlloc(sizeof(EXPRESSION));
    c->y = rcAlloc(sizeof(EXPRESSION));
    c->width = rcAlloc(sizeof(EXPRESSION));
    c->height = rcAlloc(sizeof(EXPRESSION));
    c->x->type = c->y->type = c->width->type = c->height->type = e_int;
    c->x->val = r.left;
    c->y->val = r.top;
    c->width->val = r.right - r.left;
    c->height->val = r.bottom - r.top;
    c->id = ResAllocateControlId(dlgData, GetCtlName(c));
    UndoInsert(dlgData, c);
    CreateSingleControl(dlgData->gd.childWindow, dlgData, c, TRUE);
    ResSetDirty(dlgData);
}
static BOOL CALLBACK DeleteActiveControls(HWND hwnd, LPARAM lParam)
{
    struct resRes* dlgData = (struct resRes*)lParam;
    if (SendMessage(hwnd, WM_QUERYSELECTEDSTATE, 0, 0))
    {
        struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
        CONTROL** p = &dlgData->resource->u.dialog->controls;
        while (*p && *p != data->data)
            p = &(*p)->next;
        if (*p)
            *p = (*p)->next;
        CleanChildSpace(hwnd, data->dlg->gd.childWindow);
        DestroyWindow(hwnd);
        ResSetDirty(dlgData);
    }
    return TRUE;
}
static void DrawGrid(HWND hwnd, HDC hDC, struct resRes* dlgData, BOOL show, int spacing, RECT* clip)
{
    if (show)
    {
        HPEN pen = CreatePen(PS_DOT, 0, 0x808080);
        RECT r;
        int i, j;
        SIZE base;
        RECT s;
        s.left = s.top = spacing;
        s.right = s.bottom = 0;
        GetBaseUnits(hwnd, dlgData, &base);
        ConvertToPixels(&s, &base);
        pen = SelectObject(hDC, pen);
        GetClientRect(hwnd, &r);
        for (i = s.top; i < r.bottom; i += s.top)
        {
            for (j = r.left; j < r.right; j += 2)
            {
                if (j >= clip->left && j < clip->right)
                    if (i >= clip->top && i < clip->bottom)
                    {
                        MoveToEx(hDC, j, i, NULL);
                        LineTo(hDC, j + 1, i);
                    }
            }
        }
        for (i = s.left; i < r.right; i += s.left)
        {
            for (j = r.top; j < r.bottom; j += 2)
            {
                if (i >= clip->left && i < clip->right)
                    if (j >= clip->top && j < clip->bottom)
                    {
                        MoveToEx(hDC, i, j, NULL);
                        LineTo(hDC, i, j + 1);
                    }
            }
        }
        pen = SelectObject(hDC, pen);
        DeleteObject(pen);
    }
}
static BOOL CALLBACK RemoveAllWindows(HWND hwnd, LPARAM lParam)
{
    struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
    CleanChildSpace(hwnd, data->dlg->gd.childWindow);
    DestroyWindow(hwnd);
    return TRUE;
}
static void SetAltMode(int mode, struct resRes* dlgData)
{
    if (mode != dlgData->gd.dlgAltMode)
    {
        CONTROL* c = dlgData->resource->u.dialog->controls;
        if (mode == DA_NONE)
        {
            InvalidateRect(dlgData->gd.childWindow, NULL, TRUE);
            CreateControls(dlgData->gd.childWindow, dlgData);
        }
        else
        {
            dlgData->gd.selectedColumn = 0;
            if (dlgData->gd.dlgAltMode == DA_NONE)
                EnumChildWindows(dlgData->gd.childWindow, RemoveAllWindows, 0);
            if (mode == DA_CREATION)
                UndoReorder(dlgData);
            InvalidateRect(dlgData->gd.childWindow, NULL, TRUE);
        }
        dlgData->gd.dlgAltMode = mode;
    }
}
static void DrawColoredRect(HDC hDC, int color, WCHAR* text, LPSIZE base, LPRECT r)
{
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    int x, y;
    int mode = SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, color);
    pen = SelectObject(hDC, pen);
    MoveToEx(hDC, r->left, r->top, NULL);
    LineTo(hDC, r->right, r->top);
    MoveToEx(hDC, r->right - 1, r->top, NULL);
    LineTo(hDC, r->right - 1, r->bottom);
    MoveToEx(hDC, r->left, r->top, NULL);
    LineTo(hDC, r->left, r->bottom);
    MoveToEx(hDC, r->left, r->bottom - 1, NULL);
    LineTo(hDC, r->right, r->bottom - 1);
    pen = SelectObject(hDC, pen);
    x = (r->left + r->right - base->cx * wcslen(text)) / 2;
    y = (r->top + r->bottom - base->cy) / 2;
    TextOutW(hDC, x, y, text, wcslen(text));
    DeleteObject(pen);
    SetBkMode(hDC, mode);
}
static void DrawAltMode(HWND hwnd, HDC hDC, struct resRes* dlgData)
{
    if (dlgData->gd.dlgAltMode != DA_NONE)
    {
        int count = 0;
        CONTROL* c = dlgData->resource->u.dialog->controls;
        SIZE base;
        GetBaseUnits(hwnd, dlgData, &base);
        while (c)
        {
            RECT r;
            int color = 0xff;  // red
            WCHAR buf[256];
            if (dlgData->gd.dlgAltMode != DA_CREATION && c->text &&
                c->text->symbolic)  // also need to handle retrieving the text by id...
            {
                memcpy(buf, c->text->u.n.symbol, c->text->u.n.length * sizeof(WCHAR));
                buf[c->text->u.n.length] = 0;
            }
            else
            {
                wsprintfW(buf, L"%d", count + 1);
            }
            r.left = Eval(c->x);
            r.top = Eval(c->y);
            r.right = Eval(c->width) + r.left;
            r.bottom = Eval(c->height) + r.top;
            switch (dlgData->gd.dlgAltMode)
            {
                case DA_CREATION:
                    if (count < dlgData->gd.selectedColumn)
                    {
                        color = 0xff0000;  // blue
                    }
                    break;
                case DA_TAB:
                    if (Eval(c->style) & WS_TABSTOP)
                    {
                        color = 0xff0000;  // blue
                    }
                    break;
                case DA_GROUP:
                    if (Eval(c->style) & WS_GROUP)
                    {
                        color = 0xff0000;  // blue
                    }
                    break;
            }
            ConvertToPixels(&r, &base);
            DrawColoredRect(hDC, color, buf, &base, &r);
            count++;
            c = c->next;
        }
    }
}
static void ClickAltMode(HWND hwnd, struct resRes* dlgData)
{
    struct altData
    {
        SIZE base;
        POINT mouse;
        CONTROL* match;
        int size;
    };
    struct altData ad;
    RECT r;
    CONTROL* x = dlgData->resource->u.dialog->controls;
    memset(&ad, 0, sizeof(ad));
    GetCursorPos(&ad.mouse);
    ScreenToClient(hwnd, &ad.mouse);
    GetBaseUnits(hwnd, dlgData, &ad.base);
    while (x)
    {
        r.left = Eval(x->x);
        r.top = Eval(x->y);
        r.right = Eval(x->width) + r.left;
        r.bottom = Eval(x->height) + r.top;
        ConvertToPixels(&r, &ad.base);
        if (PtInRect(&r, ad.mouse))
        {
            int sz = Eval(x->width) * Eval(x->height);
            if (!ad.match || ad.size > sz)
            {
                ad.match = x;
                ad.size = sz;
            }
        }
        x = x->next;
    }
    if (ad.match)
    {
        switch (dlgData->gd.dlgAltMode)
        {
            int i;
            CONTROL **c, **c1;
            case DA_CREATION:
                c = &dlgData->resource->u.dialog->controls;
                for (i = 0; i < dlgData->gd.selectedColumn; i++)
                {
                    if (*c == ad.match)
                    {
                        break;
                    }
                    c = &(*c)->next;
                }
                if (i == dlgData->gd.selectedColumn)
                {
                    c1 = c;
                    while (*c1)
                    {
                        if (*c1 == ad.match)
                        {
                            *c1 = (*c1)->next;
                            break;
                        }
                        c1 = &(*c1)->next;
                    }
                    ad.match->next = *c;
                    *c = ad.match;
                    if (!ad.match->next)
                        SendMessage(hwnd, WM_KEYDOWN, VK_ESCAPE, 0);
                    else
                        InvalidateRect(hwnd, 0, TRUE);
                    dlgData->gd.selectedColumn++;
                    ResSetDirty(dlgData);
                }
                break;
            case DA_TAB:
                ResSetDirty(dlgData);
                ResGetHeap(workArea, dlgData);
                UndoChanged(dlgData, ad.match);
                if (Eval(ad.match->style) & WS_TABSTOP)
                {
                    RemoveFromStyle(&ad.match->style, WS_TABSTOP);
                }
                else
                {
                    AddToStyle(&ad.match->style, "WS_TABSTOP", WS_TABSTOP);
                }
                InvalidateRect(hwnd, 0, TRUE);
                break;
            case DA_GROUP:
                ResSetDirty(dlgData);
                ResGetHeap(workArea, dlgData);
                UndoChanged(dlgData, ad.match);
                if (Eval(ad.match->style) & WS_GROUP)
                {
                    RemoveFromStyle(&ad.match->style, WS_GROUP);
                }
                else
                {
                    AddToStyle(&ad.match->style, "WS_GROUP", WS_GROUP);
                }
                InvalidateRect(hwnd, 0, TRUE);
                break;
        }
    }
}            
BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPTEXTMETRIC lpntm, DWORD FontType, LPVOID hwnd) 
{ 
    HWND rv = (HWND)hwnd;
    if (lplf->lfFaceName[0] != '@')
    {
        int v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) lplf->lfFaceName);
        SendMessage(rv, CB_SETITEMDATA, v, 0);
    }
    return TRUE;
} 
HWND LoadFontHWNDCombobox(HWND lv)
{
    HWND rv = PropGetHWNDCombobox(lv, TRUE);
    SendMessage(rv, CB_SETMINVISIBLE, (WPARAM)10, 0);
    HDC hdc = GetDC(rv);
    EnumFontFamilies(hdc, (LPCTSTR) NULL, 
        (FONTENUMPROC) EnumFamCallBack, (LPARAM) rv); 
    ReleaseDC(rv, hdc);
    return rv;
}


static void InsertDlgProperties(HWND lv, struct resRes* data)
{
    // style
    // x,y,width,height
    // menu
    // font
    // caption
    // class

    // extended
    // help,weight,italic, exstyle
    PropSetGroup(lv, 101, L"Dialog Characteristics");
    PropSetGroup(lv, 102, L"Font Characteristics");
    PropSetGroup(lv, 103, L"General Characteristics");
    PropSetItem(lv, 0, 101, "Caption");
    PropSetItem(lv, 1, 101, "Style");
    PropSetItem(lv, 2, 101, "ExtendedStyle");
    PropSetItem(lv, 3, 101, "X");
    PropSetItem(lv, 4, 101, "Y");
    PropSetItem(lv, 5, 101, "Width");
    PropSetItem(lv, 6, 101, "Height");

    PropSetItem(lv, 7, 102, "Typeface");
    PropSetItem(lv, 8, 102, "Point Size");
    PropSetItem(lv, 9, 102, "Bold");
    PropSetItem(lv, 10, 102, "Italic");
    PropSetItem(lv, 11, 102, "Charset");

    PropSetItem(lv, 12, 103, "Resource id");
    PropSetItem(lv, 13, 103, "Language");
    PropSetItem(lv, 14, 103, "SubLanguage");
    PropSetItem(lv, 15, 103, "Characteristics");
    PropSetItem(lv, 16, 103, "Version");
    PropSetItem(lv, 17, 103, "Class");
}
void GetDlgPropText(char* buf, HWND lv, struct resRes* data, int row)
{
    buf[0] = 0;
    switch (row)
    {
        case 0:
            if (data->resource->u.dialog->caption)
            {
                StringWToA(buf, data->resource->u.dialog->caption, wcslen(data->resource->u.dialog->caption));
            }
            break;
        case 1:
            FormatExp(buf, data->resource->u.dialog->style);
            break;
        case 2:
            FormatExp(buf, data->resource->u.dialog->exstyle);
            break;
        case 3:
            sprintf(buf, "%d", Eval(data->resource->u.dialog->x));
            break;
        case 4:
            sprintf(buf, "%d", Eval(data->resource->u.dialog->y));
            break;
        case 5:
            sprintf(buf, "%d", Eval(data->resource->u.dialog->width));
            break;
        case 6:
            sprintf(buf, "%d", Eval(data->resource->u.dialog->height));
            break;
        case 7:
            if (data->resource->u.dialog->font)
            {
                StringWToA(buf, data->resource->u.dialog->font, wcslen(data->resource->u.dialog->font));
            }
            break;
        case 8:
            sprintf(buf, "%d", Eval(data->resource->u.dialog->pointsize));
            break;
        case 9:
            if (Eval(data->resource->u.dialog->ex.weight) == FW_BOLD)
                strcpy(buf, "Yes");
            else
                strcpy(buf, "No");
            break;
        case 10:
            if (Eval(data->resource->u.dialog->ex.italic))
                strcpy(buf, "Yes");
            else
                strcpy(buf, "No");
            break;
        case 11:
            FormatExp(buf, data->resource->u.dialog->ex.charset);
            break;
        case 12:
            FormatResId(buf, &data->resource->id);
            break;
        case 13:
            FormatExp(buf, data->resource->info.language_high);
            break;
        case 14:
            FormatExp(buf, data->resource->info.language_low);
            break;
        case 15:
            FormatExp(buf, data->resource->info.characteristics);
            break;
        case 16:
            FormatExp(buf, data->resource->info.version);
            break;
        case 17:
            if (data->resource->u.dialog->class)
                FormatResId(buf, data->resource->u.dialog->class);
            break;
    }
}
HWND DlgPropStartEdit(HWND lv, int row, struct resRes* data)
{
    HWND rv;
    int v;
    switch (row)
    {
        case 3:
        case 4:
        case 5:
        case 6:
        case 13:
        case 14:
        case 15:
        case 16:
            rv = PropGetHWNDNumeric(lv);
            break;
        default:
            rv = PropGetHWNDText(lv);
            break;
        case 7:
        {
            rv = LoadFontHWNDCombobox(lv);
            char buf[256];
            GetDlgPropText(buf, lv, data, row);
            SendMessage(rv, CB_SELECTSTRING, -1, (LPARAM)buf);
            return rv;
        }
        case 9:
        case 10:
            rv = PropGetHWNDCombobox(lv, FALSE);
            v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "No");
            SendMessage(rv, CB_SETITEMDATA, v, 0);
            v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "Yes");
            SendMessage(rv, CB_SETITEMDATA, v, 1);
            return rv;
    }
    if (rv)
    {
        char buf[256];
        GetDlgPropText(buf, lv, data, row);
        SendMessage(rv, WM_SETTEXT, 0, (LPARAM)buf);
    }
    return rv;
}
void DlgPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data)
{
    char buf[256];
    char buf1[256];
    if (row == 7 || row == 9 || row == 10)
    {
        strcpy(buf, "");
        strcpy(buf1, "");

        int v = SendMessage(editWnd, CB_GETCURSEL, 0, 0);
        if (v != CB_ERR)
        {
            SendMessage(editWnd, CB_GETLBTEXT, v, (LPARAM)buf);
        }
        else
        {

            SendMessage(editWnd, WM_GETTEXT, 256, (LPARAM)buf);
            int v = CB_ERR;
            if (buf[0])
            {
                int v = SendMessage(editWnd, CB_SELECTSTRING, -1, (LPARAM)buf);
                if (v != CB_ERR)
                {
                    SendMessage(editWnd, CB_GETLBTEXT, v, (LPARAM)buf);
                }
                else
                {
                    strcpy(buf, "");
                }
            }
            else
            {
                strcpy(buf, "");
            }
        }
    }
    else
    {
        SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
        GetDlgPropText(buf1, lv, data, row);
    }
    if (strcmp(buf, buf1))
    {
        switch (row)
        {
            case 0:
                data->resource->u.dialog->caption = NULL;
                StringAsciiToWChar(&data->resource->u.dialog->caption, buf, strlen(buf));
                break;
            case 1:
                PropSetExp(data, buf, &data->resource->u.dialog->style);
                break;
            case 2:
                PropSetExp(data, buf, &data->resource->u.dialog->exstyle);
                break;
            case 3:
                PropSetExp(data, buf, &data->resource->u.dialog->x);
                break;
            case 4:
                PropSetExp(data, buf, &data->resource->u.dialog->y);
                break;
            case 5:
                PropSetExp(data, buf, &data->resource->u.dialog->width);
                break;
            case 6:
                PropSetExp(data, buf, &data->resource->u.dialog->height);
                break;
            case 7:
                data->resource->u.dialog->font = NULL;
                if (data->gd.font)
                {
                    DeleteObject(data->gd.font);
                    data->gd.font = NULL;
                }
                StringAsciiToWChar(&data->resource->u.dialog->font, buf, strlen(buf));
                break;
            case 8:
                if (data->gd.font)
                {
                    DeleteObject(data->gd.font);
                    data->gd.font = NULL;
                }
                PropSetExp(data, buf, &data->resource->u.dialog->pointsize);
                break;
            case 9:
                if (data->gd.font)
                {
                    DeleteObject(data->gd.font);
                    data->gd.font = NULL;
                }
                if (!strcmp(buf, "Yes"))
                    sprintf(buf, "%d", FW_BOLD);
                else
                    sprintf(buf, "%d", FW_NORMAL);
                PropSetExp(data, buf, &data->resource->u.dialog->ex.weight);
                break;
            case 10:
                if (data->gd.font)
                {
                    DeleteObject(data->gd.font);
                    data->gd.font = NULL;
                }
                if (!strcmp(buf, "Yes"))
                    strcpy(buf, "1");
                else
                    strcpy(buf, "0");
                PropSetExp(data, buf, &data->resource->u.dialog->ex.italic);
                break;
            case 11:
                PropSetExp(data, buf, &data->resource->u.dialog->ex.charset);
                break;
            case 12:
                PropSetIdName(data, buf, &data->resource->id.u.id, NULL, TRUE);
                break;
            case 13:
                PropSetExp(data, buf, &data->resource->info.language_high);
                break;
            case 14:
                PropSetExp(data, buf, &data->resource->info.language_low);
                break;
            case 15:
                PropSetExp(data, buf, &data->resource->info.characteristics);
                break;
            case 16:
                PropSetExp(data, buf, &data->resource->info.version);
                break;
            case 17:
                ResSetDirty(data);
                if (!data->resource->u.dialog->class)
                    data->resource->u.dialog->class = rcAlloc(sizeof(IDENT));
                if (isdigit(buf[0]))
                {
                    data->resource->u.dialog->class->symbolic = FALSE;
                    PropSetExp(data, buf, &data->resource->u.dialog->class->u.id);
                }
                else
                {
                    int len;
                    char* p = ParseVersionString(buf, &len);
                    data->resource->u.dialog->class->symbolic = TRUE;
                    data->resource->u.dialog->class->u.n.length =
                        StringAsciiToWChar(&data->resource->u.dialog->class->u.n.symbol, p, len);
                }
                break;
        }
        ResSetDirty(data);
    }
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    DestroyWindow(data->gd.childWindow);
    DeleteObject(data->gd.font);
    data->gd.font = NULL;
    data->gd.childWindow = CreateDlgWindow(data->activeHwnd, data);
    SetFocus(data->activeHwnd);
}
LRESULT CALLBACK DlgDlgProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static POINT start, end;
    static BOOL selecting, dragging;
    static BOOL saved;
    static HCURSOR oldCurs;
    POINT pt;
    RECT focus;
    LPCREATESTRUCT createStruct;
    struct resRes* dlgData;
    HDC hDC;
    PAINTSTRUCT ps;
    switch (iMessage)
    {
        case WM_CREATE:
            createStruct = (LPCREATESTRUCT)lParam;
            dlgData = (struct resRes*)(createStruct->lpCreateParams);
            SetWindowLong(hwnd, 0, (long)dlgData);
            CreateControls(hwnd, dlgData);
            break;
        case WM_ACTIVATESIZERECTANGLE:
            if (!(GetKeyState(VK_CONTROL) & 0x80000000))
                EnumChildWindows(hwnd, ActivateSizing, wParam);
            break;
        case WM_POSTBUTTONSTATE:
            switch (wParam)
            {
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                    dragging = TRUE;
                    saved = FALSE;
                    SetCapture(hwnd);
                    start.x = GET_X_LPARAM(lParam);
                    start.y = GET_Y_LPARAM(lParam);
                    oldCurs = GetCursor();
                    break;
            }
            break;
        case WM_RBUTTONUP:
        {
            HMENU menu, popup;
            dragging = FALSE;
            ReleaseCapture();
            menu = LoadMenuGeneric(hInstance, "RESDLGMENU");
            popup = GetSubMenu(menu, 0);
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ClientToScreen(hwnd, &pt);
            if (snapToGrid)
            {
                CheckMenuItem(menu, IDM_SNAPTOGRID, MF_BYCOMMAND | MF_CHECKED);
            }
            if (showGrid)
            {
                CheckMenuItem(menu, IDM_SHOWGRID, MF_BYCOMMAND | MF_CHECKED);
            }
            switch (gridSpacing)
            {
                case 2:
                    CheckMenuItem(menu, IDM_GRID2, MF_BYCOMMAND | MF_CHECKED);
                    break;
                case 4:
                    CheckMenuItem(menu, IDM_GRID4, MF_BYCOMMAND | MF_CHECKED);
                    break;
                case 8:
                    CheckMenuItem(menu, IDM_GRID8, MF_BYCOMMAND | MF_CHECKED);
                    break;
                case 10:
                    CheckMenuItem(menu, IDM_GRID10, MF_BYCOMMAND | MF_CHECKED);
                    break;
            }
            InsertBitmapsInMenu(popup);
            TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, hwnd, NULL);
            DestroyMenu(menu);
        }
            return 0;
        case WM_RBUTTONDOWN:
            SetAltMode(DA_NONE, (struct resRes*)GetWindowLong(hwnd, 0));
            return 0;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            SetResourceProperties(dlgData, &dlgFuncs);
            if (dlgData->gd.dlgAltMode != DA_NONE)
            {
                ClickAltMode(hwnd, dlgData);
            }
            else
            {
                SendMessage(hwnd, WM_ACTIVATESIZERECTANGLE, 0, 0);
                selecting = TRUE;
                start.x = GET_X_LPARAM(lParam);
                start.y = GET_Y_LPARAM(lParam);
                SetCapture(hwnd);
                ClientToScreen(hwnd, &start);
            }
            break;
        case WM_MOUSEMOVE:
            if (selecting)
            {
                end.x = GET_X_LPARAM(lParam);
                end.y = GET_Y_LPARAM(lParam);
                ClientToScreen(hwnd, &end);
                focus.left = start.x;
                focus.top = start.y;
                focus.right = end.x;
                focus.bottom = end.y;
                DrawBoundingRect(&focus);
                EnumChildWindows(hwnd, SelectChildren, (LPARAM)&focus);
            }
            else if (dragging)
            {
                if (!saved)
                {
                    if (abs(GET_X_LPARAM(lParam) - start.x) < 5 && abs(GET_Y_LPARAM(lParam) - start.y) < 5)
                        break;
                    saved = TRUE;
                    UndoMoveActive((struct resRes*)GetWindowLong(hwnd, 0));
                }
                // without this if we got an infinite loop while dragging
                if (end.x != GET_X_LPARAM(lParam) || end.y != GET_Y_LPARAM(lParam))
                {
                    RECT client;
                    GetClientRect(hwnd, &client);
                    end.x = GET_X_LPARAM(lParam);
                    end.y = GET_Y_LPARAM(lParam);
                    if (PtInRect(&client, end))
                    {
                        SIZE base;
                        SetCursor(dragCur);
                        // calculate deltas
                        client.left = end.x - start.x;
                        client.top = end.y - start.y;
                        GetBaseUnits(hwnd, (struct resRes*)GetWindowLong(hwnd, 0), &base);
                        ConvertToDlgUnits(&client, &base);
                        client.right = client.left;
                        client.bottom = client.top;
                        EnumChildWindows(hwnd, MoveChildren, (LPARAM)&client);
                    }
                    else
                    {
                        SetCursor(noCur);
                    }
                }
            }
            break;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
            if (selecting)
            {
                HideBoundingRect();
                ReleaseCapture();
                selecting = FALSE;
            }
            else if (dragging)
            {
                RECT client;
                GetClientRect(hwnd, &client);
                end.x = GET_X_LPARAM(lParam);
                end.y = GET_Y_LPARAM(lParam);
                if (end.x > client.right)
                    end.x = client.right - 1;
                if (end.y > client.bottom)
                    end.y = client.bottom - 1;
                if (saved && PtInRect(&client, end))
                {
                    SIZE base;
                    // calculate deltas
                    client.left = end.x - start.x;
                    client.top = end.y - start.y;
                    GetBaseUnits(hwnd, (struct resRes*)GetWindowLong(hwnd, 0), &base);
                    ConvertToDlgUnits(&client, &base);
                    client.right = client.left;
                    client.bottom = client.top;
                    EnumChildWindows(hwnd, MoveChildrenCommit, (LPARAM)&client);
                }
                ReleaseCapture();
                SetCursor(oldCurs);
                dragging = FALSE;
                saved = FALSE;
            }
            break;
        case WM_KEYDOWN:
            // posted from parent
            if (wParam == VK_ESCAPE)
            {
                SetAltMode(DA_NONE, (struct resRes*)GetWindowLong(hwnd, 0));
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_CREATIONORDER:
                    SetAltMode(DA_CREATION, (struct resRes*)GetWindowLong(hwnd, 0));
                    break;
                case IDM_SETTABSTOPS:
                    SetAltMode(DA_TAB, (struct resRes*)GetWindowLong(hwnd, 0));
                    break;
                case IDM_SETGROUPFLAGS:
                    SetAltMode(DA_GROUP, (struct resRes*)GetWindowLong(hwnd, 0));
                    break;
                case IDM_SHOWGRID:
                    showGrid = !showGrid;
                    InvalidateRect(hwnd, NULL, TRUE);
                    MarkChanged(workArea, TRUE);
                    break;
                case IDM_SNAPTOGRID:
                    snapToGrid = !snapToGrid;
                    InvalidateRect(hwnd, NULL, TRUE);
                    MarkChanged(workArea, TRUE);
                    break;
                case IDM_GRID2:
                    gridSpacing = 2;
                    InvalidateRect(hwnd, NULL, TRUE);
                    MarkChanged(workArea, TRUE);
                    break;
                case IDM_GRID4:
                    gridSpacing = 4;
                    InvalidateRect(hwnd, NULL, TRUE);
                    MarkChanged(workArea, TRUE);
                    break;
                case IDM_GRID8:
                    gridSpacing = 8;
                    InvalidateRect(hwnd, NULL, TRUE);
                    MarkChanged(workArea, TRUE);
                    break;
                case IDM_GRID10:
                    gridSpacing = 10;
                    InvalidateRect(hwnd, NULL, TRUE);
                    MarkChanged(workArea, TRUE);
                    break;
                case IDM_DELETE:
                    dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
                    UndoDeleteActive(dlgData);
                    EnumChildWindows(dlgData->gd.childWindow, DeleteActiveControls, (LPARAM)dlgData);
                    break;
            }
            break;
        case WM_DESTROY:
            break;
        case WM_QUERYCTLDROPTARGET:
            return 1;
        case WM_CTLDROP:
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(hwnd, &pt);
            InsertNewControl((struct resRes*)GetWindowLong(hwnd, 0), wParam, pt);
            break;
        case WM_MOVE:
            SendMessage(GetParent(hwnd), WM_MOVEDCHILD, 0, 0);
            break;
        case WM_PAINT:
            hDC = BeginPaint(hwnd, &ps);
            DrawGrid(hwnd, hDC, (struct resRes*)GetWindowLong(hwnd, 0), showGrid, gridSpacing, &ps.rcPaint);
            DrawAltMode(hwnd, hDC, (struct resRes*)GetWindowLong(hwnd, 0));
            EndPaint(hwnd, &ps);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static char* UnStreamExpr(char* p, EXPRESSION** c)
{
    int type = *((short*)p);
    p += sizeof(short);
    if (type >= 0)
    {
        EXPRESSION* rv = rcAlloc(sizeof(EXPRESSION));
        int len;
        *c = rv;
        rv->type = type;
        rv->val = *((int*)p);
        p += sizeof(int);
        len = *((unsigned char*)p);
        p += sizeof(unsigned char);
        if (len)
        {
            rv->rendition = rcAlloc(len + 1);
            memcpy(rv->rendition, p, len);
            p[len] = 0;
            p += len;
        }
        p = UnStreamExpr(p, &rv->left);
        p = UnStreamExpr(p, &rv->right);
    }
    return p;
}
static char* UnStreamIdent(char* p, IDENT* ident)
{
    int len;
    ident->symbolic = *((short*)p);
    p += sizeof(short);
    len = *((short*)p);
    p += sizeof(short);
    if (len)
    {
        WCHAR* name = rcAlloc(sizeof(WCHAR) * (len + 1));
        memcpy(name, p, sizeof(WCHAR) * len);
        name[len] = 0;
        ident->origName = name;
        p += sizeof(WCHAR) * len;
    }
    if (ident->symbolic)
    {
        WCHAR* name;
        len = *((short*)p);
        p += sizeof(short);
        name = rcAlloc(sizeof(WCHAR) * (len + 1));
        memcpy(name, p, sizeof(WCHAR) * len);
        name[len] = 0;
        ident->u.n.length = len;
        ident->u.n.symbol = name;
        p += sizeof(WCHAR) * len;
    }
    else
    {
        p = UnStreamExpr(p, &ident->u.id);
    }
    return p;
}
static char* UnStreamControl(char* p, struct resRes* dlgData, CONTROL* c)
{
    c->generic = *((int*)p);
    p += sizeof(int);
    c->baseStyle = *((int*)p);
    p += sizeof(int);
    p = UnStreamIdent(p, &c->class);
    c->text = rcAlloc(sizeof(IDENT));
    p = UnStreamIdent(p, c->text);
    p = UnStreamExpr(p, &c->id);
    p = UnStreamExpr(p, &c->style);
    p = UnStreamExpr(p, &c->exstyle);
    p = UnStreamExpr(p, &c->x);
    p = UnStreamExpr(p, &c->y);
    p = UnStreamExpr(p, &c->width);
    p = UnStreamExpr(p, &c->height);
    p = UnStreamExpr(p, &c->help);
    return p;
}
static BOOL DlgHasId(struct resRes* dlgData, EXPRESSION* id)
{
    int n = Eval(id);
    if (n)
    {
        CONTROL* c = dlgData->resource->u.dialog->controls;
        while (c)
        {
            int t = Eval(c->id);
            if (t == n)
                return TRUE;
            c = c->next;
        }
    }
    return FALSE;
}
static void StreamActiveControlsFromClipboard(struct resRes* dlgData)
{
    if (OpenClipboard(dlgData->gd.childWindow))
    {
        char* buf = NULL;
        HANDLE clh = GetClipboardData(clipboardFormatId);
        char* data = GlobalLock(clh);
        int l = GlobalSize(clh);
        if (l)
        {
            buf = malloc(l);
            if (buf)
            {
                memcpy(buf, data, l);
            }
        }
        GlobalUnlock(data);
        CloseClipboard();
        if (buf)
        {
            CONTROL *newControls = NULL, **cp = &newControls, *nc;
            char* p = buf;
            int count = *((int*)p);
            int i;
            int x = INT_MAX, y = INT_MAX;
            p += sizeof(int);
            ResGetHeap(workArea, dlgData);
            for (i = 0; i < count; i++)
            {
                *cp = rcAlloc(sizeof(CONTROL));
                p = UnStreamControl(p, dlgData, *cp);
                cp = &(*cp)->next;
            }
            nc = newControls;
            while (nc)
            {
                int x1 = Eval(nc->x);
                int y1 = Eval(nc->y);
                if (x1 < x)
                    x = x1;
                if (y1 < y)
                    y = y1;
                nc = nc->next;
            }
            nc = newControls;
            while (nc)
            {
                int x1 = Eval(nc->x);
                int y1 = Eval(nc->y);
                nc->x->val = x1 - x + 10;
                nc->x->type = e_int;
                nc->x->left = nc->x->right = 0;
                nc->x->rendition = NULL;
                nc->y->val = y1 - y + 10;
                nc->y->type = e_int;
                nc->y->left = nc->y->right = 0;
                nc->y->rendition = NULL;
                if (DlgHasId(dlgData, nc->id))
                    nc->id = ResAllocateControlId(dlgData, GetCtlName(nc));
                nc = nc->next;
            }
            cp = &dlgData->resource->u.dialog->controls;
            while (*cp)
                cp = &(*cp)->next;
            *cp = newControls;
            // ctrl is likely to be down at this point so we do this directly
            EnumChildWindows(dlgData->gd.childWindow, ActivateSizing, 0);
            nc = newControls;
            UndoInsert(dlgData, nc);
            while (nc)
            {
                CreateSingleControl(dlgData->gd.childWindow, dlgData, nc, TRUE);
                nc = nc->next;
            }
            free(buf);
            ResSetDirty(dlgData);
        }
    }
}
static char* StreamExpr(char* p, EXPRESSION* exp)
{
    if (!exp)
    {
        *((short*)p) = -1;
        p += sizeof(short);
    }
    else
    {
        *((short*)p) = exp->type;
        p += sizeof(short);
        *((int*)p) = exp->val;
        p += sizeof(int);
        if (exp->rendition)
        {
            *p++ = strlen(exp->rendition);
            strcpy(p, exp->rendition);
            p += strlen(exp->rendition);
        }
        else
        {
            *p++ = 0;
        }
        p = StreamExpr(p, exp->left);
        p = StreamExpr(p, exp->right);
    }
    return p;
}
static char* StreamIdent(char* p, IDENT* ident)
{
    *((short*)p) = ident->symbolic;
    p += sizeof(short);
    if (!ident->origName)
    {
        *((short*)p) = 0;
        p += sizeof(short);
    }
    else
    {
        *((short*)p) = wcslen(ident->origName);
        p += sizeof(short);
        wcscpy((WCHAR*)p, ident->origName);
        p += sizeof(WCHAR) * wcslen(ident->origName);
    }
    if (ident->symbolic)
    {
        *((short*)p) = ident->u.n.length;
        p += sizeof(short);
        memcpy(p, ident->u.n.symbol, ident->u.n.length * sizeof(WCHAR));
        p += sizeof(WCHAR) * ident->u.n.length;
    }
    else
    {
        p = StreamExpr(p, ident->u.id);
    }
    return p;
}
static BOOL CALLBACK StreamToClipboard(HWND hwnd, LPARAM lParam)
{
    struct clipboardBuffer* b = (struct clipboardBuffer*)lParam;
    if (b->maxSize == 0)
    {
        b->maxSize = 1000;
        b->data = realloc(b->data, b->maxSize);
        b->currentSize = 4;
        *(int*)b->data = 0;
    }
    if (SendMessage(hwnd, WM_QUERYSELECTEDSTATE, 0, 0))
    {
        char buf[10000], *p = buf;
        struct ctlData* data = (struct ctlData*)GetWindowLong(hwnd, GWL_USERDATA);
        int len;
        CONTROL* c = data->data;

        *((int*)p) = c->generic;
        p += sizeof(int);
        *((int*)p) = c->baseStyle;
        p += sizeof(int);
        p = StreamIdent(p, &c->class);
        p = StreamIdent(p, c->text);
        p = StreamExpr(p, c->id);
        p = StreamExpr(p, c->style);
        p = StreamExpr(p, c->exstyle);
        p = StreamExpr(p, c->x);
        p = StreamExpr(p, c->y);
        p = StreamExpr(p, c->width);
        p = StreamExpr(p, c->height);
        p = StreamExpr(p, c->help);
        if (p - buf + b->currentSize >= b->maxSize)
        {
            b->maxSize *= 2;
            b->data = realloc(b->data, b->maxSize);
        }
        memcpy(b->data + b->currentSize, buf, p - buf);
        b->currentSize += p - buf;
        len = *((int*)b->data);
        len++;
        *((int*)b->data) = len;  // increment count
    }
    return TRUE;
}
static void WriteClipboardData(struct resRes* dlgData, struct clipboardBuffer* b)
{
    if (OpenClipboard(dlgData->gd.childWindow))
    {
        HGLOBAL glmem;
        glmem = GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, b->currentSize);
        if (glmem != NULL)
        {
            char* data = GlobalLock(glmem);
            memcpy(data, b->data, b->currentSize);
            GlobalUnlock(data);
            EmptyClipboard();
            SetClipboardData(clipboardFormatId, glmem);
        }
        CloseClipboard();
    }
}
LRESULT CALLBACK DlgDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    LPCREATESTRUCT createStruct;
    struct resRes* dlgData;
    struct dlgUndo* undo;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            //            dlgData = (struct resRes *)GetWindowLong(hwnd, 0);
            //            SetResourceProperties(dlgData, &dlgFuncs);
            break;
        case WM_COMMAND:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case IDM_SAVE:
                    if (dlgData->resource->changed)
                    {
                        ResSaveCurrent(workArea, dlgData);
                    }
                    break;
                case IDM_UNDO:
                    DlgDoUndo(dlgData);
                    break;
                case IDM_CUT:
                {
                    struct clipboardBuffer b;
                    memset(&b, 0, sizeof(b));
                    EnumChildWindows(dlgData->gd.childWindow, StreamToClipboard, (LPARAM)&b);
                    WriteClipboardData(dlgData, &b);
                    free(b.data);
                    UndoDeleteActive(dlgData);
                    EnumChildWindows(dlgData->gd.childWindow, DeleteActiveControls, (LPARAM)dlgData);
                }
                break;
                case IDM_COPY:
                {
                    struct clipboardBuffer b;
                    memset(&b, 0, sizeof(b));
                    EnumChildWindows(dlgData->gd.childWindow, StreamToClipboard, (LPARAM)&b);
                    WriteClipboardData(dlgData, &b);
                    free(b.data);
                }
                break;
                case IDM_PASTE:
                    StreamActiveControlsFromClipboard(dlgData);
                    break;
                default:
                    PostMessage(dlgData->gd.childWindow, iMessage, wParam, lParam);
                    break;
            }
            break;
        case EM_CANUNDO:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            return dlgData->gd.undoData != NULL;
        case WM_KEYDOWN:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
                case VK_DELETE:
                    UndoDeleteActive(dlgData);
                    EnumChildWindows(dlgData->gd.childWindow, DeleteActiveControls, (LPARAM)dlgData);
                    break;
                case VK_ESCAPE:
                    PostMessage(dlgData->gd.childWindow, iMessage, wParam, lParam);
                    break;
                case 'S':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
                    }
                    break;
                case 'Z':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_UNDO, 0);
                    }
                    break;
                case 'C':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_COPY, 0);
                    }
                    break;
                case 'X':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_CUT, 0);
                    }
                    break;
                case 'V':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_PASTE, 0);
                    }
                    break;
            }
            switch (KeyboardToAscii(wParam, lParam, FALSE))
            {
                case '[':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PopupResFullScreen(hwnd);
                        return 0;
                    }
                    break;
                case ']':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        ReleaseResFullScreen(hwnd);
                        return 0;
                    }
                    break;
            }
            break;
        case WM_NCACTIVATE:
            PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
            return TRUE;
        case WM_NCPAINT:
            return PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
        case WM_CREATE:
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
            createStruct = (LPCREATESTRUCT)lParam;
            dlgData = (struct resRes*)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)dlgData);
            dlgData->activeHwnd = hwnd;
            dlgData->gd.childWindow = CreateDlgWindow(hwnd, dlgData);
            ShowOrHideScrollBars(hwnd, dlgData);
            InsertRCWindow(hwnd);
            break;
        case WM_CLOSE:
            RemoveRCWindow(hwnd);
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            SendMessage(dlgData->gd.childWindow, WM_CLOSE, 0, 0);
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            dlgData->activeHwnd = NULL;
            DeleteObject(dlgData->gd.font);
            undo = dlgData->gd.undoData;
            dlgData->gd.undoData = NULL;
            if (undo)
                dlgData->gd.cantClearUndo = TRUE;
            while (undo)
            {
                struct dlgUndo* next = undo->next;
                free(undo);
                undo = next;
            }
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            PostMessage(dlgData->gd.childWindow, WM_KEYDOWN, VK_ESCAPE, 0);
            // fall through
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
            SetFocus(hwnd);
            return 0;
        case WM_VSCROLL:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case SB_BOTTOM:
                    dlgData->gd.scrollPos.y = dlgData->gd.scrollMax.y;
                    break;
                case SB_TOP:
                    dlgData->gd.scrollPos.y = 0;
                    break;
                case SB_LINEDOWN:
                    dlgData->gd.scrollPos.y += 8;
                    break;
                case SB_LINEUP:
                    dlgData->gd.scrollPos.y -= 8;
                    break;
                case SB_PAGEDOWN:
                    dlgData->gd.scrollPos.y += 64;
                    break;
                case SB_PAGEUP:
                    dlgData->gd.scrollPos.y -= 64;
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                {
                    SCROLLINFO si;
                    memset(&si, 0, sizeof(si));
                    si.cbSize = sizeof(si);
                    si.fMask = SIF_TRACKPOS;
                    GetScrollInfo(hwnd, SB_VERT, &si);
                    dlgData->gd.scrollPos.y = si.nTrackPos;
                }
                break;
                default:
                    return 0;
            }
            if (dlgData->gd.scrollPos.y < 0)
                dlgData->gd.scrollPos.y = 0;
            if (dlgData->gd.scrollPos.y >= dlgData->gd.scrollMax.y)
                dlgData->gd.scrollPos.y = dlgData->gd.scrollMax.y;
            SetScrollPos(hwnd, SB_VERT, dlgData->gd.scrollPos.y, TRUE);
            SetWindowPos(dlgData->gd.childWindow, NULL, dlgData->gd.origin.x - dlgData->gd.scrollPos.x,
                         dlgData->gd.origin.y - dlgData->gd.scrollPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        case WM_HSCROLL:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case SB_LEFT:
                    dlgData->gd.scrollPos.x = 0;
                    break;
                case SB_RIGHT:
                    dlgData->gd.scrollPos.x = dlgData->gd.scrollMax.x;
                    break;
                case SB_LINELEFT:
                    dlgData->gd.scrollPos.x -= 8;
                    break;
                case SB_LINERIGHT:
                    dlgData->gd.scrollPos.x += 8;
                    break;
                case SB_PAGERIGHT:
                    dlgData->gd.scrollPos.x += 64;
                    break;
                case SB_PAGELEFT:
                    dlgData->gd.scrollPos.x -= 64;
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                {
                    SCROLLINFO si;
                    memset(&si, 0, sizeof(si));
                    si.cbSize = sizeof(si);
                    si.fMask = SIF_TRACKPOS;
                    GetScrollInfo(hwnd, SB_HORZ, &si);
                    dlgData->gd.scrollPos.x = si.nTrackPos;
                }
                break;
            }
            if (dlgData->gd.scrollPos.x < 0)
                dlgData->gd.scrollPos.x = 0;
            if (dlgData->gd.scrollPos.x >= dlgData->gd.scrollMax.x)
                dlgData->gd.scrollPos.x = dlgData->gd.scrollMax.x;
            SetScrollPos(hwnd, SB_HORZ, dlgData->gd.scrollPos.x, TRUE);
            SetWindowPos(dlgData->gd.childWindow, NULL, dlgData->gd.origin.x - dlgData->gd.scrollPos.x,
                         dlgData->gd.origin.y - dlgData->gd.scrollPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

            return 0;
        case WM_MOVEDCHILD:
        case WM_SIZE:
            dlgData = (struct resRes*)GetWindowLong(hwnd, 0);
            ShowOrHideScrollBars(hwnd, dlgData);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterDlgDrawWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &DlgDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szDlgDrawClassName;
    RegisterClass(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &DlgDlgProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szDlgDialogClassName;
    RegisterClass(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNSHADOW + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szDlgUnknownClassName;
    RegisterClass(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &DlgControlInputProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szDlgControlInputClassName;
    RegisterClass(&wc);

    dragCur = LoadCursor(hInstance, "ID_DRAGCTL");
    noCur = LoadCursor(hInstance, "ID_NODRAGCUR");
    hcurs = LoadCursor(0, IDC_SIZEWE);
    vcurs = LoadCursor(0, IDC_SIZENS);

    clipboardFormatId = RegisterClipboardFormat("xccControlFormat");

    drawdlginit();
}
void CreateDlgDrawWindow(struct resRes* info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maximized);
    hwnd = CreateMDIWindow(szDlgDrawClassName, name,
                           WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME |
                               MDIS_ALLCHILDSTYLES | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SIZEBOX |
                               (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndClient, hInstance, (LPARAM)info);
    if (hwnd)
    {
        SelectWindow(DID_CTLTBWND);
        SendMessage(hwndSrcTab, TABM_ADD, (WPARAM)name, (LPARAM)hwnd);

        if (info->resource->changed)
            ResRewriteTitle(info, TRUE);
    }
}
