/* ---------------- search.c ------------- */
#include "dflat.h"

extern DBOX SearchTextDB;
extern DBOX ReplaceTextDB;

char highlightText[256] ;
BOOL highlightCaseSensitive;
BOOL highlightWholeWord;

static int CheckCase = TRUE;
static int Replacing = FALSE;

/* - case-insensitive, white-space-normalized char compare - */
static BOOL SearchCmp(int a, int b)
{
    if (b == '\n')
        b = ' ';
    if (CheckCase)
        return a != b;
    return tolower(a) != tolower(b);
}

/* do a single static allocate of the text buf, taking into account the size
 * of all possible replacements
 */
static int CalculateNewSize(WINDOW wnd, BOOL all)
{
    char *cr = GetEditBoxText(&ReplaceTextDB, ID_REPLACEWITH);
    char *cp = GetEditBoxText(&ReplaceTextDB, ID_SEARCHFOR);
	int plen = strlen(cp);
	int rlen = strlen(cr);
	ATTRCHR *cp1;
	int count = 0;
	if (rlen <= plen)
		return wnd->textalloc;
	if (!all)
		return wnd->textalloc + rlen - plen;
		
	/* replacing all occurrances */
    cp1 = CurrChar;
    /* --- compare at each character position --- */
    while (cp1->ch)    {
        char *s1 = cp;
        ATTRCHR *s2 = cp1;
        while (*s1 && *s1 != '\n')    {
            if (SearchCmp(*s1, s2->ch))
                break;
            s1++, s2++;
        }
        if (*s1 == '\0' || *s1 == '\n')
		{
			count++;
			cp1 += plen;
		}
		else
		{
	        cp1++;
		}
    }
	count = count * (rlen - plen);
	return wnd->textlen + count + 2 + GROWLENGTH;
 }
/* ------- search for the occurrance of a string ------- */
static void SearchTextBox(WINDOW wnd, int incr)
{
    char *s1 = NULL;
	ATTRCHR *cp1, *s2;
    DBOX *db = Replacing ? &ReplaceTextDB : &SearchTextDB;
    char *cp = GetEditBoxText(db, ID_SEARCHFOR);
	char *cr = GetEditBoxText(db, ID_REPLACEWITH);
    BOOL rpl = TRUE, FoundOne = FALSE;
	BOOL changed = FALSE;
	int searchsize = strlen(cp);
	int replacesize = strlen(cr);
	int newLength;
	ATTRCHR *newBuf, *dst;
	int attr = clr(WndForeground(wnd), WndBackground(wnd));
	int counted = 0;
	cp1 = CurrChar;
    /* search for a match starting at cursor position */
    if (incr)
        cp1 += replacesize;    /* start past the last hit */
	if (!Replacing)
	{
        highlightCaseSensitive = CheckCase;
		strcpy(highlightText , cp);
	}
	else
	{
		newLength = CalculateNewSize(wnd, CheckBoxSetting(&ReplaceTextDB, ID_REPLACEALL));
		dst = newBuf = (ATTRCHR *)DFcalloc(newLength, sizeof(ATTRCHR));
		memcpy(dst, GetText(wnd), (cp1 - GetText(wnd)) * sizeof(ATTRCHR));
		dst += cp1 - GetText(wnd);
	}
    while (rpl == TRUE && cp != NULL && *cp)    {
        rpl = Replacing ?
                CheckBoxSetting(&ReplaceTextDB, ID_REPLACEALL) :
                FALSE;
        if (TextBlockMarked(wnd))    {
            ClearTextBlock(wnd);
            SendMessage(wnd, PAINT, 0, 0);
        }
        /* --- compare at each character position --- */
        while (cp1->ch)    {
            s1 = cp;
            s2 = cp1;
            while (*s1 && *s1 != '\n')    {
                if (SearchCmp(*s1, s2->ch))
                    break;
                s1++, s2++;
            }
            if (*s1 == '\0' || *s1 == '\n')
                break;
			if (Replacing)
				*dst++ = *cp1++;
			else
				cp1++;
        }
        if (s1 != NULL && (*s1 == 0 || *s1 == '\n'))    {
            /* ----- match at *cp1 ------- */
            FoundOne = TRUE;

			
			if (!rpl)
			{
	            /* position the cursor at the matching text */
	            wnd->CurrLine = TextLineNumber(wnd, cp1);
	            wnd->CurrCol = (int)(s2 - TextLine(wnd, wnd->CurrLine));
	            wnd->WndRow = wnd->CurrLine - wnd->wtop;

	            /* mark a block at beginning of matching text */
	            wnd->BlkEndLine = TextLineNumber(wnd, s2);
	            wnd->BlkBegLine = TextLineNumber(wnd, cp1);
	            if (wnd->BlkEndLine < wnd->BlkBegLine)
	                wnd->BlkEndLine = wnd->BlkBegLine;
	            wnd->BlkEndCol =
	                (int)(s2 - TextLine(wnd, wnd->BlkEndLine));
	            wnd->BlkBegCol =
	                (int)(cp1 - TextLine(wnd, wnd->BlkBegLine));
	
	            /* align the window scroll to matching text */
	            if (WndCol > ClientWidth(wnd)-1)
	                wnd->wleft = wnd->CurrCol;
	            if (wnd->WndRow > ClientHeight(wnd)-1)    {
	                wnd->wtop = wnd->CurrLine;
	                wnd->WndRow = 0;
	            }

	            SendMessage(wnd, PAINT, 0, 0);
    	        SendMessage(wnd, KEYBOARD_CURSOR,
        	        WndCol, wnd->WndRow);
			}
				
            if (Replacing)    {
                if (rpl || YesNoBox("Replace the text?"))  {
					int i;
					s1 = cr;
					for (i=0; i < replacesize; i++)
					{
						dst->ch = *s1++;
						dst++->attrib = attr; 
					}
					cp1 += searchsize;
					changed = TRUE;
					counted++;
                	if (rpl)    {
						continue;
                	}
                }
            }
        }
		break;
    }
	if (Replacing && changed)
	{
		wnd->textalloc = newLength;
		if (rpl)
		{
			dst->ch = '\0';
			dst->attrib = attr;
			wnd->textlen = dst - newBuf;
		}
		else
		{
			memcpy(dst, cp1, (wnd->textlen - (cp1 - wnd->text)+ 1) * sizeof(ATTRCHR));
			wnd->textlen += replacesize;
		}
		free(wnd->text);
		wnd->text = newBuf;
        ClearTextBlock(wnd);
        BuildTextPointers(wnd);
		ChangedText(wnd, TRUE);
        SendMessage(wnd, PAINT, 0, 0);
	}
	SendMessage(ApplicationWindow, SEARCHDONE, 0, 0);
    if (!FoundOne)
	{
        MessageBox("Search/Replace Text", "No match found");
	}
	else if (Replacing && CheckBoxSetting(&ReplaceTextDB, ID_REPLACEALL))
	{
		char buf[256];
		sprintf(buf, "%d items replaced", counted);
        MessageBox("Search/Replace Text", buf);
	}
}

/* ------- search for the occurrance of a string,
        replace it with a specified string ------- */
void ReplaceText(WINDOW wnd)
{
	Replacing = TRUE;
    if (CheckCase)
        SetCheckBox(&ReplaceTextDB, ID_MATCHCASE);
    if (DialogBox(NULL, &ReplaceTextDB, TRUE, NULL))    {
        CheckCase=CheckBoxSetting(&ReplaceTextDB,ID_MATCHCASE);
        SearchTextBox(wnd, FALSE);
    }
}

/* ------- search for the first occurrance of a string ------ */
void SearchText(WINDOW wnd)
{
	Replacing = FALSE;
    if (CheckCase)
        SetCheckBox(&SearchTextDB, ID_MATCHCASE);
    if (DialogBox(NULL, &SearchTextDB, TRUE, NULL))    {
        CheckCase=CheckBoxSetting(&SearchTextDB,ID_MATCHCASE);
        SearchTextBox(wnd, FALSE);
    }
}

/* ------- search for the next occurrance of a string ------- */
void SearchNext(WINDOW wnd)
{
    SearchTextBox(wnd, TRUE);
}

