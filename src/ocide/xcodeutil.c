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

// assumes tabs aren't going to get reset yet
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

extern LOGFONT systemDialogFont;
extern HINSTANCE hInstance;
extern KEYLIST C_keywordList[1];
extern HWND hwndShowFunc;

#define TRANSPARENT_COLOR 0x872395

extern HWND codecompleteBox;
extern BOOL inStructBox;

BOOL GetContainerData(int lineno, char* file, char* ns, char* func)
{
    ns[0] = func[0] = 0;
    ccLookupContainingNamespace(file, lineno, ns);
    return ccLookupContainingMemberFunction(file, lineno, func);
}
static void LookupBaseType(int line, char* file, char* name, sqlite_int64* struct_id, int* indir)
{
    char funcbase[2048], nsbase[512];
    GetContainerData(line, file, nsbase, funcbase);
    if (funcbase[0])
    {
        strcat(funcbase, name);
        if (ccLookupMemberType(funcbase, file, line, struct_id, indir))
            return;
        if (ccLookupStructType(funcbase, file, line, struct_id, indir))
            return;
    }
    if (nsbase[0])
    {
        char nsbase2[2048], *p = nsbase, *last;
        while (*p)
        {
            char* q = nsbase2;
            while (*p && *p != ';')
                *q++ = *p++;
            if (*p)
                p++;
            *q = 0;
            last = nsbase2 + strlen(nsbase2);
            strcat(nsbase2, name);
            while (last != nsbase2)
            {
                char* p = last;
                if (ccLookupStructType(nsbase2, file, line, struct_id, indir))
                    return;
                while (--last != nsbase2 && *last != '@')
                    ;
                strcpy(last, p);
            }
        }
    }
    if (ccLookupStructType(name, file, line, struct_id, indir))
        return;
    name[0] = '_';
    ccLookupStructType(name, file, line, struct_id, indir);
    name[0] = '@';
}
CCSTRUCTDATA* FindStructData(CCSTRUCTDATA* structData, char* file, char* name, size_t len, int* indir)
{
    int i;
    int id = 0;
    if (!structData)
        return NULL;
    for (i = 0; i < structData->fieldCount; i++)
    {
        int sz = strlen(structData->data[i].fieldName);
        if (len < sz)
            if (!strncmp(structData->data[i].fieldName + sz - len, name, len))
            {
                id = structData->data[i].subStructId;
                *indir = structData->data[i].indirectCount;
                break;
            }
    }
    if (i < structData->fieldCount)
    {

        if (id)
        {
            ccFreeStructData(structData);
            return ccLookupStructElems(file, id, *indir);
        }
    }
    return NULL;
}
CCSTRUCTDATA* GetStructType(char** name, int lineno, char* file, CCSTRUCTDATA* structData, int* indir_in)
{
    sqlite_int64 id = 0;
    char parenNesting[400];
    int indir[400];
    int parenLevel = 0;
    int lastWasOpenPa = 0;
    char buf[2048];
    indir[0] = indir_in ? *indir_in : 0;

    while (*(*name))
    {
        while (isspace(*(*name)))
            (*name)++;
        switch (*(*name))
        {
            case '-':
                (*name)++;
                if (*(*name) != '>')
                {
                    ccFreeStructData(structData);
                    return NULL;
                }
                {
                    CCSTRUCTDATA* found;
                    // look for operator ->
                    found = FindStructData(structData, file, "@$barrow$qv", 11, &indir[parenLevel]);
                    if (found)
                    {
                        if (indir_in)
                            *indir_in = indir[parenLevel];
                        structData = found;
                    }
                    else if (indir[parenLevel])
                    {
                        indir[parenLevel]--;
                        if (!structData || indir[parenLevel])
                        {
                            ccFreeStructData(structData);
                            return NULL;
                        }
                    }
                    else
                    {
                        ccFreeStructData(structData);
                        return NULL;
                    }
                }
                // FALLTHROUGH
            case '.':
                if (!structData || indir[parenLevel])
                {
                    ccFreeStructData(structData);
                    return NULL;
                }
                (*name)++;
                while (isspace(*(*name)))
                    (*name)++;
                if (!*(*name))
                {
                    return structData;
                }
                // for now only support (*name)s, none of the extra C++ stuff you can do...
                if (!isalpha(*(*name)) && (*name) != '_')
                {
                    ccFreeStructData(structData);
                    return NULL;
                }
                {
                    char* end = (*name);
                    CCSTRUCTDATA* found;

                    id = 0;
                    while (keysym(*end))
                        end++;
                    found = FindStructData(structData, file, (*name), end - (*name), &indir[parenLevel]);
                    if (!found)
                    {
                        ccFreeStructData(structData);
                        return NULL;
                    }
                    structData = found;
                    if (indir_in)
                        *indir_in = indir[parenLevel];
                    (*name) = end;
                    continue;
                }
                break;
            case '(':
                lastWasOpenPa = 2;
                parenNesting[parenLevel++] = ')';
                indir[parenLevel] = 0;
                break;
            case ')':
                if (parenLevel)
                {
                    --parenLevel;
                    indir[parenLevel] += indir[parenLevel + 1];
                }
                else
                {
                    return structData;
                }
                break;
            case '[':  // eats to ]
            {
                int nesting = 1;
                indir[parenLevel]++;
                parenNesting[parenLevel++] = ']';
                while (*(*name) && nesting)
                {
                    switch (*(*name))
                    {
                        case '[':
                            parenNesting[parenLevel++] = ']';
                            nesting++;
                            break;
                        case ']':
                            if (parenNesting[--parenLevel] != ']')
                            {
                                ccFreeStructData(structData);
                                return NULL;
                            }
                            nesting--;
                            break;
                        case '(':
                            parenNesting[parenLevel++] = ')';
                            break;
                        case ')':
                            if (parenNesting[--parenLevel] != ')')
                            {
                                ccFreeStructData(structData);
                                return NULL;
                            }
                            break;
                    }
                    (*name)++;
                }
                continue;
            }
            case '*':
            {
                int n = 0;
                (*name)++;
                structData = GetStructType(name, lineno, file, structData, &n);
                if (structData)
                {
                    CCSTRUCTDATA* found;
                    // look for operator *()
                    found = FindStructData(structData, file, "@$bmul$qv", 9, &indir[parenLevel]);
                    if (found)
                    {
                        structData = found;
                        if (indir_in)
                            *indir_in = indir[parenLevel];
                    }
                    else
                    {
                        indir[parenLevel] = n;
                    }
                }
                else
                {
                    indir[parenLevel]--;
                }
                continue;
            }
            case '&':
                indir[parenLevel]++;
                break;
            case ':': /* end of sequence */
                break;
            default: /* must be a word */
            {
                if (!structData)
                {
                    int ind;
                    GetQualifiedName(buf, &(*name), FALSE, FALSE);
                    if ((*name)[0] == ':')
                    {
                        ccLookupStructId(buf, file, lineno, &id);
                    }
                    else
                    {
                        LookupBaseType(lineno, file, buf, &id, &ind);
                    }
                    indir[parenLevel] += ind;
                }
                else if (lastWasOpenPa)
                {
                    char* q = buf;
                    // try to parse a type..
                    GetQualifiedName(buf, &(*name), TRUE, FALSE);
                    while (*q && !isdigit(*q))
                    {
                        switch (*q)
                        {
                            case 'x':
                            case 'y':
                                break;
                            case 'r':
                            case 'p':
                                indir[parenLevel]++;
                                break;
                            default:
                            {
                                ccFreeStructData(structData);
                                return NULL;
                            }
                        }
                        q++;
                    }
                    if (isdigit(*q))
                    {
                        int ind;
                        while (isdigit(*q))
                            q++;
                        LookupBaseType(lineno, file, q, &id, &ind);
                        indir[parenLevel] += ind;
                    }
                    while (isspace(*(*name)))
                        (*name)++;
                    if (*(*name) != ')')
                    {
                        ccFreeStructData(structData);
                        return NULL;
                    }
                }
                if (id)
                {
                    structData = ccLookupStructElems(file, id, indir[parenLevel]);
                    if (indir_in)
                        *indir_in = indir[parenLevel];
                }
                else
                {
                    return NULL;
                }
                (*name)--;
                break;
            }
        }
        lastWasOpenPa--;
        (*name)++;
    }
    return structData;
}
void GetCompletionPos(HWND hwnd, EDITDATA* p, LPPOINT pt, int width, int height)

{
    if (posfromchar(hwnd, p, pt, p->selstartcharpos))
    {
        RECT rect, frame;
        GetWindowRect(hwnd, &rect);
        GetFrameWindowRect(&frame);  // undefined in local context
        pt->x -= width / 2;
        pt->x += rect.left;
        //			pt->x -= frame.left;
        pt->y += p->cd->txtFontHeight;  //+ GetSystemMetrics(SM_CYCAPTION) * 2;
        pt->y += rect.top;
        //			pt->y -= frame.top;
        if (pt->y + height > frame.bottom - frame.top)
        {
            pt->y -= p->cd->txtFontHeight;
            pt->y -= height;
        }
    }
}

KEYLIST* partialmatchkeyword(KEYLIST* table, int tabsize, char* name)
{
    int len = strlen(name);
    int top = tabsize;
    int bottom = -1;
    int v;
    char* p = name;
    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    while (top - bottom > 1)
    {
        int mid = (top + bottom) / 2;
        char trans[256];
        p = &trans[0];
        strcpy(trans, table[mid].text);
        while (*p)
        {
            *p = tolower(*p);
            p++;
        }
        v = strncmp(name, trans, len);
        if (v < 0)
        {
            top = mid;
        }
        else
        {
            bottom = mid;
        }
    }
    if (bottom == -1)
        return 0;
    return &table[bottom];
}
char** GetCodeCompKeywords(char* name, char** names, int* size, int* max, HWND hwnd, EDITDATA* p)
{
    KEYLIST* table = partialmatchkeyword(C_keywordList, sizeof(C_keywordList) / sizeof(C_keywordList[0]), name);
    if (table)
    {
        int len = strlen(name);
        while (C_keywordList != table && !memcmp(table[-1].text, name, len))
            table--;
        while (!memcmp(name, table->text, len))
        {
            if (*size >= *max)
            {
                if (!*max)
                    *max = 10;
                else
                    *max *= 2;
                names = realloc(names, *max * sizeof(void*));
            }
            names[(*size)++] = strdup(table->text);
            table++;
        }
    }
    return names;
}
char** GetCodeCompVariables(char* name, char** names, int* size, int* max, HWND hwnd, EDITDATA* p)
{
    names = ccGetMatchingNames(name, names, size, max);
    return names;
}
void showVariableOrKeyword(HWND hwnd, EDITDATA* p)
{
    if (instring(p->cd->text, &p->cd->text[p->selstartcharpos]))
        return;
    if (inStructBox || PropGetInt(NULL, "CODE_COMPLETION") < 2)
        return;
    {
        char buf[4096], *q = buf + sizeof(buf);
        int pos = p->selstartcharpos - 1;
        char** names = NULL;
        int size = 0, max = 0;
        *--q = 0;
        if (pos <= 0)
            return;
        while (pos && (isalnum(p->cd->text[pos].ch) || p->cd->text[pos].ch == '_' || p->cd->text[pos].ch == '#'))
            *--q = p->cd->text[pos--].ch;
        if (codecompleteBox)
        {
            if (strlen(q) == PropGetInt(NULL, "CODE_COMPLETION_VARIABLE_THRESHOLD") ||
                strlen(q) == PropGetInt(NULL, "CODE_COMPLETION_KEYWORD_THRESHOLD"))
            {
                SendMessage(codecompleteBox, WM_CLOSE, 0, 0);
                codecompleteBox = NULL;
            }
        }
        if (!codecompleteBox)
        {
            if (strlen(q) >= PropGetInt(NULL, "CODE_COMPLETION_VARIABLE_THRESHOLD"))
                names = GetCodeCompVariables(q, names, &size, &max, hwnd, p);
            if (strlen(q) >= PropGetInt(NULL, "CODE_COMPLETION_KEYWORD_THRESHOLD"))
                names = GetCodeCompKeywords(q, names, &size, &max, hwnd, p);
            if (size)
            {
                codecompleteBox = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE, "xcccodeclass", "",
                                                 (WS_POPUP) | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                                 CW_USEDEFAULT, hwnd, 0, hInstance, 0);
                if (codecompleteBox)
                {
                    int count = 0;
                    int max = 0;
                    SIZE siz1;
                    int width, height;
                    POINT cpos;
                    TEXTMETRIC t;
                    HDC dc = GetDC(codecompleteBox);
                    xdrawline(hwnd, p, p->selstartcharpos);  // undefiend in local context
                    GetTextMetrics(dc, &t);
                    SendMessage(codecompleteBox, LB_RESETCONTENT, 0, 0);
                    p->cd->selecting = FALSE;
                    width = 0;
                    for (count = 0; count < size; count++)
                    {
                        int n;
                        char* name = names[count];
                        GetTextExtentPoint32(dc, name, strlen(name), &siz1);
                        if (siz1.cx > width)
                            width = siz1.cx;
                        n = SendMessage(codecompleteBox, LB_ADDSTRING, 0, (LPARAM)name);
                        free(name);
                    }
                    free(names);
                    ReleaseDC(codecompleteBox, dc);
                    if (count > 8)
                        count = 8;
                    if (width < 70)
                        width = 70;
                    if (count)
                    {
                        height = t.tmHeight * count;
                        width += 10 + GetSystemMetrics(SM_CXVSCROLL) + 2 * GetSystemMetrics(SM_CXFRAME);
                        GetCompletionPos(hwnd, p, &cpos, width, height);
                        SendMessage(codecompleteBox, WM_USER, 0, (LPARAM)hwnd);
                        SendMessage(codecompleteBox, WM_USER + 2, 0, (LPARAM)q);
                        MoveWindow(codecompleteBox, cpos.x, cpos.y, width + 4, height + 4, TRUE);
                        ShowWindow(codecompleteBox, SW_SHOW);
                        if (IsWindowVisible(hwndShowFunc))
                            ShowWindow(hwndShowFunc, SW_HIDE);
                        return;
                    }
                    else
                        DestroyWindow(codecompleteBox);
                }
            }
        }
    }
}
