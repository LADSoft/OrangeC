/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include <windows.h>
#include <richedit.h>
#define Uses_TView
#define Uses_TEvent
#define Uses_TDialog
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TButton
#define Uses_TRadioButtons
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_MsgBox
#include "tv.h"
#include "editors.h"
#include "editview.h"
#include "infopad.h"
#include "regexp.h"

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

char findText[256];
char replaceText[256];
ushort findOptions;
ushort findMode;
ushort replaceOptions;
ushort replaceMode;

static int replaceCount;
static const int textlen = 100;

char *findBuf;
int internalFlags;
static int canceled;
static bool inSetSel;

void rehighlight(char *text, int whole, int casesensitive);
char *GetEditData(EDITDATA *ptr);
DWINFO *GetFileInfo(char *name);
char *stristr(char *str1, char *str2)
{
    int l = strlen(str2);
    while (*str1)
    {
        char *str3 = str1, *str4 = str2;
        while (*str3 && *str4)
        {
            if (toupper(*str3) != toupper(*str4))
                break;
            str3++, str4++;
        }
        if (!*str4)
            return str1;
        str1++;
    }
    return 0;
}

static void nm_matches(char *search, int flags, char *buf, CHARRANGE *pos)
{
    if (flags &F_DOWN)
    {
        if (flags &FR_MATCHCASE)
            pos->cpMin = (int)strstr(buf + pos->cpMin, search);
        else
            pos->cpMin = (int)stristr(buf + pos->cpMin, search);
        if (pos->cpMin)
            pos->cpMin = pos->cpMin - (int)buf;
        else
            pos->cpMin = pos->cpMax;
    }
    else
    {
        int l = strlen(search);
        do
        {
            if (flags &F_MATCHCASE)
            {
                if (!strncmp(buf + pos->cpMax, search, l))
                    break;
            }
            else
                if (!strnicmp(buf + pos->cpMax, search, l))
                    break;
            pos->cpMax--;
        } while (pos->cpMax >= pos->cpMin && !canceled);
    }
}
static int getSegment(char *result, char **source)
{
    int n = 0;
    while (**source && **source != '?' && **source != '*')
    {
        *result ++= *(*source)++;
        n++;
    }
    *result = 0;
    return n;
}
static int matches_wc(char *search, int flags, char *buf, int pos, int *len)
{
    /* this assumes that a '*' will be prepended to whatever they ask for */
    int start=-1;
    char segment[1024];
    char *orgsearch = search;
    int orgpos = pos;
    
    while (pos != -1 && *search && !canceled)
    {
        if (search == orgsearch)
        {
            orgpos = pos + 1;
            start = -1;
        }
        if (*search == '*')
        {
            // this is a simplification, probably we ought to count the '?'
            // chars and make sure the match is at least that many...
            while (*search == '*' || *search == '?')
                search++;
            if (*search)
            {
                int l = getSegment(segment, &search);
                char *q, *p;
                p = strchr(buf+pos, '\n');
                
                if (flags &FR_MATCHCASE)
                    q = strstr(buf+pos, segment);
                else
                    q = stristr(buf+pos, segment);
                if (q)
                {
                    if (p && q > p)
                    {
                        pos = p - buf;
                        search = orgsearch;
                        if (!(flags & F_DOWN))
                            break;
                    }
                    else
                    {
                        if (start == -1)
                            start = pos;
                        pos = q - buf + l;
                    }
                }
                else
                {
                    pos = -1;
                }
            }
        }
        else if (*search == '?')
        {
            if (start == -1)
                start = pos;
            if (!buf[pos++])
                pos = -1;
            else
                search++;
        }
        else
        {
            char *q;
            if (search == orgsearch)
            {
                int l = getSegment(segment, &search);
                if (flags & FR_DOWN)
                {
                    if (flags &FR_MATCHCASE)
                        q = strstr(buf+pos, segment);
                    else
                        q = stristr(buf+pos, segment);
                }
                else
                {
                    q = 0;
                    if (flags &FR_MATCHCASE)
                    {
                        if (!strncmp(buf + pos, segment, l))
                            q = buf + pos;
                    }
                    else
                    {
                        if (!strnicmp(buf + pos, segment, l))
                            q = buf + pos;
                    }
                        
                }
                if (!q)
                    pos = -1;
                else
                {
                    pos = q - buf;
                    if (start == -1)
                        start = pos;
                    orgpos = pos + 1;
                    pos += l;
                }
            }
            else
            {
                int l = getSegment(segment, &search);
                if (flags &F_MATCHCASE)
                {
                    if (strncmp(buf + pos, segment, l))
                    {
                        search = orgsearch;
                        pos = orgpos;
                        if (!(flags & F_DOWN))
                            break;
                    }
                    else
                    {
                        pos += l;
                    }
                }
                else
                {
                    if (strncmp(buf + pos, segment, l))
                    {
                        search = orgsearch;
                        pos = orgpos;
                        if (!(flags & F_DOWN))
                            break;
                    }
                    else
                    {
                        pos += l;
                    }
                }
            }
        }
    }
    if (!*search)
    {
        *len = pos - start;
        return start;
    }
    return -1;
}
static void wc_matches(char *search, int flags, char *buf, CHARRANGE *pos, int *len)
{
    if (flags & FR_DOWN)
    {        
        pos->cpMin = matches_wc(search, flags, buf, pos->cpMin, len);
        if (pos->cpMin == -1)
            pos->cpMin = pos->cpMax;
    }
    else
    {
        int l = strlen(search);
        do
        {
            int n = matches_wc(search, flags, buf, pos->cpMax, len);
            if (n >= 0)
            {
                pos->cpMax = n;
                return;
            }
            pos->cpMax--;
        } while (pos->cpMax >= pos->cpMin && !canceled);
    }
}
static void rex_matches(RE_CONTEXT *context, int flags, char *buf, CHARRANGE *pos, int *len)
{
    if (flags &F_DOWN)
    {
        int i = re_matches(context, buf, pos->cpMin, pos->cpMax);
        if (!i)
            pos->cpMin = pos->cpMax;
        else
            pos->cpMin += context->m_so;
    }
    else   
    {
        int i = re_matches(context, buf, pos->cpMax, pos->cpMin);
        if (!i)
            pos->cpMax = -1;
        else
            pos->cpMax += context->m_so;
    }
    *len = context->m_eo - context->m_so;
}
static int xfind(char *fname, char *search, int flags, char *buf, int *len, CHARRANGE *pos, 
                 RE_CONTEXT *context)
{
    int i;
    *len = strlen(search);
    while (!canceled)
    {
        int rv;
        if (flags & F_REGULAR)
        {
            rex_matches(context, flags, buf, pos, len);
        }
        else if (flags & F_WILDCARD)
        {
            wc_matches(search, flags, buf, pos, len);
        }
        else
        {
            nm_matches(search, flags, buf, pos);
        }
        if (pos->cpMin >= pos->cpMax)
            return -1;
            
        if (flags &F_WHOLEWORD)
        {
            if (flags & FR_DOWN)
            {
            
                if (pos->cpMin && isalnum(buf[pos->cpMin - 1]))
                {
                    pos->cpMin +=  *len;
                }
                else if (!buf[pos->cpMin +  *len] || !isalnum(buf[pos->cpMin +  *len]))
                {
                    ////SendToOutput(fname, flags, buf, buf + pos->cpMin);
                    return pos->cpMin;
                }
                else
                {
                    pos->cpMin +=  *len;
                }
            }
            else
            {
                if (pos->cpMax && isalnum(buf[pos->cpMax - 1]))
                {
                    pos->cpMax --;
                }
                else if (!buf[pos->cpMax +  *len] || !isalnum(buf[pos->cpMax +  *len]))
                {
                    ////SendToOutput(fname, flags, buf, buf + pos->cpMax);
                    return pos->cpMax;
                }
                else
                {
                    pos->cpMax--;
                }
            }
        }
        else
        {

            if (flags & FR_DOWN)
            {
                ////SendToOutput(fname, flags, buf, buf + pos->cpMin);
                return pos->cpMin;
            }
            else
            {
                ////SendToOutput(fname, flags, buf, buf + pos->cpMax);
                return pos->cpMax;
            }
        }
    }
}
static RE_CONTEXT * getREContext(char *search, int flags)
{
    RE_CONTEXT *rv = NULL;
    if (flags & F_REGULAR)
    {
        rv = re_init(search, RE_F_REGULAR | 
                          ((flags & F_MATCHCASE) ? 0 : RE_F_INSENSITIVE) |
                          ((flags & F_WHOLEWORD) ? RE_F_WORD : 0), NULL);
        if (!rv)
        {
            messageBox("There is a problem with the regular expression", mfError);
            rv = (RE_CONTEXT *)-1;
        }
    }
    return rv;
}
static int FindNextInDocument(DWINFO *ptr, char *buf, CHARRANGE *pos, int flags, char *search, RE_CONTEXT *context)
{

    BOOL rv = FALSE;
    BOOL val;
    int len = 0;
    val = xfind(ptr->dwName, search, flags, buf, &len, pos, context);
    if (val >= 0)
    {
        CHARRANGE r;
        HWND win;
        if (flags & F_DOWN)
        {
            r.cpMax = pos->cpMin;
            r.cpMin = pos->cpMin + len;
            pos->cpMin += len;
        }
        else
        {
            r.cpMin = pos->cpMax;
            r.cpMax = pos->cpMax + len;
            pos->cpMax --;
        }
        inSetSel= true;
        ptr->child->WinMessage(EM_HIDESELECTION, 1, 0);
        ptr->child->WinMessage(EM_EXSETSEL, 0, (LPARAM)&r);
        ptr->child->WinMessage(EM_SCROLLCARET, 0, 1);
        ptr->child->WinMessage(EM_HIDESELECTION, 0, 0);
        ptr->WinMessage( WM_COMMAND, ID_REDRAWSTATUS, 0);
        inSetSel= false;
        rv = TRUE;
    }
    else
    {
        ptr->child->WinMessage(EM_SETSEL,  - 1, 0);
        ptr->WinMessage( WM_COMMAND, ID_REDRAWSTATUS, 0);
    }
    return rv;
}

void DWINFO::FindSetDirty() 
{ 
    if (!inSetSel)
    {
        child->WinMessage(EM_EXGETSEL, 0, (LPARAM)&findStart);
        findPos = findStart;
        if (internalFlags & F_DOWN)
        {
            findPos.cpMax = child->WinMessage(WM_GETTEXTLENGTH, 0, 0);
        }
        else
        {
            findPos.cpMin = 0;
        }
    }
}
void DWINFO::EndFind() 
{ 
}
static ushort findBoxRect( const TRect &bounds,
                     char *find,
                     ushort &options,
                     ushort &mode)
{
    char *Title = "Find";
    char *aLabel = "Enter Find Text:";
    TDialog *dialog;
    TView* control;
    TRect r;
    ushort c;

    *(ushort *)(find + textlen) = options;
    *(ushort *)(find + textlen+2) = mode;
    dialog = new TDialog(bounds, Title);

    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
    control = new TInputLine( r, textlen );
    dialog->insert( control );

    r = TRect(2, 2, 3 + strlen(aLabel), 3);
    dialog->insert( new TLabel( r, aLabel, control ) );
    
    r = TRect(2, 4, 42, 6);
    dialog->insert( new TCheckBoxes(r, new TSItem("Case Sensitive",
                                              new TSItem("Whole Word",
                                                     new TSItem("Search Up", NULL)))));    

    r = TRect(2, 7, 42, 9);
    dialog->insert( new TRadioButtons(r, new TSItem("Normal Search",
                                              new TSItem("Wildcard Search",
                                                     new TSItem("Regexp Search", NULL)))));
    
    r = TRect( dialog->size.x - 24, dialog->size.y - 4,
               dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, MsgBoxText::cancelText, cmCancel, bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(False);
    dialog->setData(find);
    c = TProgram::application->execView(dialog);
    if( c != cmCancel )
    {
        dialog->getData(find);
        options = *(ushort *)(find + textlen);
        mode = *(ushort *)(find + textlen+2);
    }   
    TObject::destroy( dialog );
    return c;
}
static ushort findBox( char *s, ushort &options, ushort &mode )
{
    TRect r(0, 0, 45, 14);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return findBoxRect(r, s, options, mode);
}
void DWINFO::OpenFindDialog() 
{ 
    if (findBox(findText, findOptions, findMode) != cmCancel)
    {
        rehighlight(findText, !!(findOptions & 2), !!(findOptions & 1));
        internalFlags =0;
        if (findOptions & 1)
            internalFlags |= F_MATCHCASE;
        if (findOptions & 2)
            internalFlags |= F_WHOLEWORD;
        if (!(findOptions & 4))
            internalFlags |= F_DOWN;
        if (findMode == 1)
            internalFlags |= F_WILDCARD;
        if (findMode == 2)
            internalFlags |= F_REGULAR;
        FindSetDirty();
        DoFindNext();
    }
}
void DWINFO::DoFindNext() 
{
    if (!*findText)
        return;
    RE_CONTEXT *context = getREContext(findText, internalFlags);
    finding = TRUE;
    char *buf = GetEditData(child);
    if (!buf)
        return; 
    CHARRANGE a;
    child->WinMessage(EM_EXGETSEL, 0, (LPARAM)&a);
    if (internalFlags & F_DOWN)
    {
        findPos.cpMin = a.cpMin;
    }
    else
    {
        findPos.cpMax = a.cpMax == -1 ? child->WinMessage(WM_GETTEXTLENGTH, 0, 0) : a.cpMax;
    }
    while (!FindNextInDocument(this, buf, &findPos, internalFlags, findText, context) && !canceled)
    {
        if (internalFlags & F_DOWN)
        {
            if (findPos.cpMax == findStart.cpMin)
            {
                findPos = findStart;
                findPos.cpMax = child->WinMessage(WM_GETTEXTLENGTH, 0, 0);
                if (finding)
                    messageBox("Reached end of search", mfInformation | mfOKButton);
                break;
            }
            findPos.cpMin = 0;
            findPos.cpMax = findStart.cpMin;
        }
        else
        {
            if (findPos.cpMin == findStart.cpMax)
            {
                findPos = findStart;
                findPos.cpMin = 0;
                if (finding)
                    messageBox("Reached end of search", mfInformation | mfOKButton);
                break;
            }
            findPos.cpMin = findStart.cpMax;
            findPos.cpMax = child->WinMessage(WM_GETTEXTLENGTH, 0, 0);
        }
    }
    if (context)
        re_free(context);
    free(buf);
}
static void GetReplaceText(char *dest, char *replace, char *dta, int len, int flags, RE_CONTEXT *context)
{
    if (flags & F_REGULAR)
    {
        while (*replace)
        {
            if (*replace == '\\')
            {
                if (++*replace)
                {
                    int index = *replace;
                    if (isdigit(index))
                    {
                        index -= '0';
                        if (dta && index <= 10)
                        {
                            if (context->matchOffsets[index][1] > 0)
                            {
                                int j, k;
                                for (k=0, j = context->matchOffsets[index][0]; j <
                                    context->matchOffsets[index][1] &&  k <
                                    255; j++, k++)
                                    dest[k] = dta[j];
                            }
                        }
                    }
                    else
                    {
                        *dest++ = *replace++;
                    }
                }
            }
            else
            {
                *dest++ = *replace++;
            }
        }
        *dest = 0;
    }
    else
    {
        strcpy(dest, replace);
    }
}
static int ReplaceNextInDocument(DWINFO *ptr, char *buf, int *ofs, CHARRANGE *pos, int flags, char *search, char *replace, RE_CONTEXT *context)
{
    BOOL val;
    CHARRANGE find;
    int len = 0;
    val = xfind(ptr->dwName, search, flags, buf, &len, pos, context);
    if (val >= 0)
    {
        char repl[1000];
        int curpos;
        int l;
        CHARRANGE aa;
        if (!ptr->child)
        {
            DWINFO *ptrx = GetFileInfo(ptr->dwName);
            if (ptrx)
            {
                ptr->child = ptrx->child;
            }
        }
        if (flags & F_DOWN)
            curpos = pos->cpMin;            
        else 
            curpos = pos->cpMax;
        GetReplaceText(repl, replace, buf, curpos, len, context);
        aa.cpMin = curpos + *ofs;
        aa.cpMax = curpos + *ofs + len;
        if (flags & F_DOWN)
        {
//            if (flags & F_REPLACEALL)
            {
                pos->cpMin += len;
            }
        }
        l = strlen(repl) - strlen(search);
        *ofs += l;
        inSetSel= true;
        ptr->child->WinMessage( EM_HIDESELECTION, 1, 0);
        ptr->child->WinMessage( EM_EXSETSEL, 0, (LPARAM) &aa);
        ptr->child->WinMessage( EM_REPLACESEL, 1, (LPARAM)repl);
        ptr->child->WinMessage( EM_HIDESELECTION, 0, 0);
        ptr->WinMessage( WM_COMMAND, ID_REDRAWSTATUS, 0);
        inSetSel= false;
        replaceCount++;
        return TRUE;
    }
    return FALSE;
}
class ReplaceDialog : public TDialog
{
public:
    ReplaceDialog(DWINFO *self, TRect bounds, char *title) : TDialog(bounds, title), info(self),
            TWindowInit(&TWindow::initFrame) { }
    
    virtual void handleEvent(TEvent &event);
    void GetParams();
private:
    DWINFO *info;
};
void ReplaceDialog::GetParams()
{
    char data[textlen * 2 + 4];
    getData(data);
    strcpy(findText, data);
    findText[textlen-1] = 0;
    strcpy(replaceText, data+textlen);
    replaceText[textlen-1] = 0;
    replaceOptions = *(ushort *)(data + textlen*2);
    replaceMode = *(ushort *)(data + textlen*2+2);
    rehighlight(findText, !!(replaceOptions & 2), !!(replaceOptions & 1));
    internalFlags =0;
    if (replaceOptions & 1)
        internalFlags |= F_MATCHCASE;
    if (replaceOptions & 2)
        internalFlags |= F_WHOLEWORD;
    if (!(replaceOptions & 4))
        internalFlags |= F_DOWN;
    if (replaceMode == 1)
        internalFlags |= F_WILDCARD;
    if (replaceMode == 2)
        internalFlags |= F_REGULAR;
    info->FindSetDirty();
}
void ReplaceDialog::handleEvent(TEvent &event)
{
    TDialog::handleEvent(event);
    switch (event.what)
        {
        case evCommand:
            switch( event.message.command )
                {
                    case cmFind:
                        GetParams();
                        info->DoFindNext();
                        break;
                    case cmReplace:
                        GetParams();
                        info->DoReplaceNext();
                        break;
                    case ID_REPLACE: // replace all
                        GetParams();
                        internalFlags |= F_REPLACEALL;
                        info->findPos.cpMin = 0 ;
                        info->findPos.cpMax = info->child->WinMessage(WM_GETTEXTLENGTH, 0, 0);
                        info->DoReplaceNext();
                        break;
                }
        }        
}
static ushort replaceBoxRect( const TRect &bounds,
                     char *find,
                     char *replace,
                     DWINFO *info,
                     ushort &options,
                     ushort &mode)
{
    char *Title = "Replace";
    char *aLabel = "Enter Find Text   :";
    char *rLabel = "Enter Replace Text:";
    ReplaceDialog *dialog;
    TView* control;
    TRect r;
    ushort c;
    char *data = new char[textlen * 2 + 4];

    strcpy(data, find);
    data[textlen-1] = 0;
    strcpy(data + textlen, replace);
    data[textlen * 2 - 1] = 0;
    *(ushort *)(data + textlen*2) = options;
    *(ushort *)(data + textlen*2+2) = mode;
    dialog = new ReplaceDialog(info, bounds, Title);

    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
    control = new TInputLine( r, textlen );
    dialog->insert( control );

    r = TRect(2, 2, 3 + strlen(aLabel), 3);
    dialog->insert( new TLabel( r, aLabel, control ) );

    r = TRect( 4 + strlen(rLabel), 4, dialog->size.x - 3, 5 );
    control = new TInputLine( r, textlen );
    dialog->insert( control );

    r = TRect(2, 4, 3 + strlen(rLabel), 5);
    dialog->insert( new TLabel( r, rLabel, control ) );
    
    r = TRect(2, 6, 42, 7);
    dialog->insert( new TCheckBoxes(r, new TSItem("Case Sensitive",
                                              new TSItem("Whole Word", NULL))));

    r = TRect(2, 8, 42, 10);
    dialog->insert( new TRadioButtons(r, new TSItem("Normal Search",
                                              new TSItem("Wildcard Search",
                                                     new TSItem("Regexp Search", NULL)))));
    
    r = TRect( dialog->size.x - 32, dialog->size.y - 5,
               dialog->size.x - 18, dialog->size.y - 3);
    dialog->insert( new TButton(r, "Find Next", cmFind, bfDefault));

    r.a.x += 16;
    r.b.x += 16;
    dialog->insert( new TButton(r, "Replace", cmReplace, bfNormal));

    r.a.x += 16;
    r.b.x += 16;

    r = TRect( dialog->size.x - 32, dialog->size.y - 3,
               dialog->size.x - 18, dialog->size.y - 1);
    dialog->insert( new TButton(r, "Replace All", ID_REPLACE, bfNormal));

    r.a.x += 16;
    r.b.x += 16;
    dialog->insert( new TButton(r, "Close", cmCancel, bfNormal));

    r.a.x += 16;
    r.b.x += 16;
    
    dialog->selectNext(False);
    dialog->setData(data);
    c = TProgram::application->execView(dialog);
    if( c != cmCancel )
    {
        dialog->getData(data);
        strcpy(find, data);
        find[textlen-1] = 0;
        strcpy(replace, data+textlen);
        replace[textlen-1] = 0;
        options = *(ushort *)(data + textlen*2);
        mode = *(ushort *)(data + textlen*2+2);
    }   
    TObject::destroy( dialog );
    return c;
}
static ushort replaceBox( char *s, char *rep, DWINFO *data, ushort &options, ushort &mode )
{
    TRect r(0, 0, 45, 16);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return replaceBoxRect(r, s, rep, data, options, mode);
}
void DWINFO::OpenReplaceDialog() 
{
   replaceBox(findText, replaceText, this, replaceOptions, replaceMode);
}
void DWINFO::DoReplaceNext() 
{
    int offset = 0;
    if (!*findText)
        return;
    RE_CONTEXT *context = getREContext(findText, internalFlags);
    finding = FALSE;
    char *buf = GetEditData(child);
    if (!buf)
        return; 
    replaceCount =0 ;
    CHARRANGE aa;
    if (!(internalFlags & F_REPLACEALL))
    {
        child->WinMessage(EM_EXGETSEL,0, (LPARAM)&aa);
        findPos.cpMin = aa.cpMin < aa.cpMax ? aa.cpMin : aa.cpMax;
    }
    BOOL replaced;
    do 
    {
        replaced = ReplaceNextInDocument(this, buf, &offset, &findPos, internalFlags | F_DOWN, findText, replaceText, context);
    }
    while ((internalFlags & F_REPLACEALL) && replaced && !canceled);
    if (!(internalFlags & F_REPLACEALL))
    {
        DoFindNext();
    }
    else
    {
        char buf[256];
        sprintf(buf, "%d items replaced", replaceCount);
        messageBox(buf, mfInformation);
    }
    if (context)
    {
        re_free(context);
    }
    free(buf);
}
