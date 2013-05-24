/* ------------- editbox.c ------------ */
#include "dflat.h"
#include <limits.h>

#define EditBufLen(wnd) (isMultiLine(wnd) ? EDITLEN : ENTRYLEN)
#define SetLinePointer(wnd, ln) (wnd->CurrLine = ln)
#define Ch(c) ((c)&0x7f)
#define isWhite(c) (Ch(c)==' '||Ch(c)=='\n'||Ch(c)=='\f'||Ch(c)=='\t')
/* ---------- local prototypes ----------- */
static void SaveDeletedText(WINDOW, ATTRCHR *, int);
static void Forward(WINDOW);
static void Backward(WINDOW);
static void End(WINDOW);
static void Home(WINDOW);
static void Downward(WINDOW);
static void Upward(WINDOW);
static void StickEnd(WINDOW);
static void NextWord(WINDOW);
static void PrevWord(WINDOW);
static void ModTextPointers(WINDOW, int, int);
static void SetAnchor(WINDOW, int, int);
static void UndoCmd(WINDOW);
/* -------- local variables -------- */
static BOOL KeyBoardMarking, ButtonDown;
static BOOL TextMarking;
static int ButtonX, ButtonY;
static int PrevY = -1;

/* ----------- CREATE_WINDOW Message ---------- */
static int CreateWindowMsg(WINDOW wnd)
{
    int rtn = BaseWndProc(EDITBOX, wnd, CREATE_WINDOW, 0, 0);
    wnd->MaxTextLength = MAXTEXTLEN+1;
    wnd->InsertMode = TRUE;
	if (isMultiLine(wnd))
	    wnd->WordWrapMode = TRUE;
	SendMessage(wnd, CLEARTEXT, 0, 0);
    return rtn;
}
/* ----------- SETTEXT Message ---------- */
static int SetTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int rtn = FALSE;
	int len;
	if (p2)
		len = tstrlen((char *)p1);
	else
		len = strlen((char *)p1);
    if (len <= wnd->MaxTextLength)	{
        rtn = BaseWndProc(EDITBOX, wnd, SETTEXT, p1, p2);
		ChangedText(wnd, FALSE);
	}
    return rtn;
}
/* ----------- CLEARTEXT Message ------------ */
static int ClearTextMsg(WINDOW wnd)
{
    int rtn = BaseWndProc(EDITBOX, wnd, CLEARTEXT, 0, 0);
    unsigned blen = EditBufLen(wnd)+2;
    wnd->text = DFrealloc(wnd->text, blen * sizeof(ATTRCHR));
    memset(wnd->text, 0, blen);
    wnd->wlines = 0;
    wnd->CurrLine = 0;
    wnd->CurrCol = 0;
    wnd->WndRow = 0;
    wnd->wleft = 0;
    wnd->wtop = 0;
    wnd->textwidth = 0;
	ChangedText(wnd, FALSE);
    return rtn;
}
/* ----------- ADDTEXT Message ---------- */
static int AddTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int rtn = FALSE;
	int len;
	if (p2)
		len = strlen((char *)p1);
	else
		len = tstrlen((ATTRCHR *)p1);
    if (len+wnd->textlen <= wnd->MaxTextLength) {
        rtn = BaseWndProc(EDITBOX, wnd, ADDTEXT, p1, p2);
        if (rtn != FALSE)    {
            if (!isMultiLine(wnd))    {
                wnd->CurrLine = 0;
                wnd->CurrCol = strlen((char *)p1);
                if (wnd->CurrCol >= ClientWidth(wnd))    {
                    wnd->wleft = wnd->CurrCol-ClientWidth(wnd);
                    wnd->CurrCol -= wnd->wleft;
                }
                wnd->BlkEndCol = wnd->CurrCol;
                SendMessage(wnd, KEYBOARD_CURSOR,
                                     WndCol, wnd->WndRow);
            }
        }
    }
    return rtn;
}
/* ----------- GETTEXT Message ---------- */
static int GetTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    char *cp1 = (char *)p1;
    ATTRCHR *cp2 = wnd->text;
    if (cp2 != NULL)    {
        while (p2-- && cp2->ch && cp2->ch != '\n')
            *cp1++ = cp2++->ch;
        *cp1 = '\0';
        return TRUE;
    }
    return FALSE;
}
/* ----------- SETTEXTLENGTH Message ---------- */
static int SetTextLengthMsg(WINDOW wnd, unsigned int len)
{
    if (len < MAXTEXTLEN)    {
        wnd->MaxTextLength = len;
        if (len < wnd->textlen)    {
			wnd->textalloc = len + 1;
            wnd->text=DFrealloc(wnd->text, wnd->textalloc * sizeof(ATTRCHR));
			wnd->textlen = len;
            wnd->text[len].ch = '\0';
            wnd->text[len + 1].ch = '\0';
            BuildTextPointers(wnd);
        }
        return TRUE;
    }
    return FALSE;
}
/* ----------- KEYBOARD_CURSOR Message ---------- */
static void KeyboardCursorMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    wnd->CurrCol = (int)p1 + wnd->wleft;
    wnd->WndRow = (int)p2;
    wnd->CurrLine = (int)p2 + wnd->wtop;
    if (wnd == inFocus)	{
		if (CharInView(wnd, (int)p1, (int)p2))
	        SendMessage(NULL, SHOW_CURSOR,
				(wnd->InsertMode && !TextMarking), 0);
    	else
			SendMessage(NULL, HIDE_CURSOR, 0, 0);
	}
}
/* ----------- SIZE Message ---------- */
int SizeMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int rtn = BaseWndProc(EDITBOX, wnd, SIZE, p1, p2);
    if (WndCol > ClientWidth(wnd)-1)
        wnd->CurrCol = ClientWidth(wnd)-1 + wnd->wleft;
    if (wnd->WndRow > ClientHeight(wnd)-1)    {
        wnd->WndRow = ClientHeight(wnd)-1;
        SetLinePointer(wnd, wnd->WndRow+wnd->wtop);
    }
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    return rtn;
}
/* ----------- SCROLL Message ---------- */
static int ScrollMsg(WINDOW wnd, PARAM p1)
{
    int rtn = FALSE;
    if (isMultiLine(wnd))    {
        rtn = BaseWndProc(EDITBOX,wnd,SCROLL,p1,0);
        if (rtn != FALSE)    {
            if (p1)    {
                /* -------- scrolling up --------- */
                if (wnd->WndRow == 0)    {
                    wnd->CurrLine++;
                    StickEnd(wnd);
                }
                else
                    --wnd->WndRow;
            }
            else    {
                /* -------- scrolling down --------- */
                if (wnd->WndRow == ClientHeight(wnd)-1)    {
                    if (wnd->CurrLine > 0)
                        --wnd->CurrLine;
                    StickEnd(wnd);
                }
                else
                    wnd->WndRow++;
            }
            SendMessage(wnd,KEYBOARD_CURSOR,WndCol,wnd->WndRow);
        }
    }
    return rtn;
}
/* ----------- HORIZSCROLL Message ---------- */
static int HorizScrollMsg(WINDOW wnd, PARAM p1)
{
    int rtn = FALSE;
    ATTRCHR *currchar = CurrChar;
    if (!(p1 &&
            wnd->CurrCol == wnd->wleft && currchar->ch == '\n'))  {
        rtn = BaseWndProc(EDITBOX, wnd, HORIZSCROLL, p1, 0);
        if (rtn != FALSE)    {
            if (wnd->CurrCol < wnd->wleft)
                wnd->CurrCol++;
            else if (WndCol == ClientWidth(wnd))
                --wnd->CurrCol;
            SendMessage(wnd,KEYBOARD_CURSOR,WndCol,wnd->WndRow);
        }
    }
    return rtn;
}
/* ----------- SCROLLPAGE Message ---------- */
static int ScrollPageMsg(WINDOW wnd, PARAM p1)
{
    int rtn = FALSE;
    if (isMultiLine(wnd))    {
        rtn = BaseWndProc(EDITBOX, wnd, SCROLLPAGE, p1, 0);
        SetLinePointer(wnd, wnd->wtop+wnd->WndRow);
        StickEnd(wnd);
        SendMessage(wnd, KEYBOARD_CURSOR,WndCol, wnd->WndRow);
    }
    return rtn;
}
/* ----------- HORIZSCROLLPAGE Message ---------- */
static int HorizPageMsg(WINDOW wnd, PARAM p1)
{
    int rtn = BaseWndProc(EDITBOX, wnd, HORIZPAGE, p1, 0);
    if ((int) p1 == FALSE)    {
        if (wnd->CurrCol > wnd->wleft+ClientWidth(wnd)-1)
            wnd->CurrCol = wnd->wleft+ClientWidth(wnd)-1;
    }
    else if (wnd->CurrCol < wnd->wleft)
        wnd->CurrCol = wnd->wleft;
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    return rtn;
}
/* ----- Extend the marked block to the new x,y position ---- */
static void ExtendBlock(WINDOW wnd, int x, int y)
{
    int bbl, bel;
    int ptop = min(wnd->BlkBegLine, wnd->BlkEndLine);
    int pbot = max(wnd->BlkBegLine, wnd->BlkEndLine);
    ATTRCHR *lp = TextLine(wnd, wnd->wtop+y), *lp2 = lp;
    int len ;
	while (lp2->ch && lp2->ch != '\n')
		lp2++;
	len = (int) (lp2 - lp);
    x = max(0, min(x, len));
	y = max(0, y);
    wnd->BlkEndCol = min(len, x+wnd->wleft);
    wnd->BlkEndLine = y+wnd->wtop;
    bbl = min(wnd->BlkBegLine, wnd->BlkEndLine);
    bel = max(wnd->BlkBegLine, wnd->BlkEndLine);
    while (ptop < bbl)    {
        WriteTextLine(wnd, NULL, ptop, FALSE);
        ptop++;
    }
    for (y = bbl; y <= bel; y++)
        WriteTextLine(wnd, NULL, y, FALSE);
    while (pbot > bel)    {
        WriteTextLine(wnd, NULL, pbot, FALSE);
        --pbot;
    }
}
/* ----------- LEFT_BUTTON Message ---------- */
static int LeftButtonMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int MouseX = (int) p1 - GetClientLeft(wnd);
    int MouseY = (int) p2 - GetClientTop(wnd);
    RECT rc = ClientRect(wnd);
    char *lp;
    int len;
    if (KeyBoardMarking)
        return TRUE;
    if (WindowMoving || WindowSizing)
        return FALSE;
    if (TextMarking)    {
        if (!InsideRect(p1, p2, rc))    {
			int x = MouseX, y = MouseY;
			int dir;
			MESSAGE msg = 0;
            if ((int)p2 == GetTop(wnd))
				y++, dir = FALSE, msg = SCROLL;
            else if ((int)p2 == GetBottom(wnd))
				--y, dir = TRUE, msg = SCROLL;
            else if ((int)p1 == GetLeft(wnd))
				--x, dir = FALSE, msg = HORIZSCROLL;
            else if ((int)p1 == GetRight(wnd))
				x++, dir = TRUE, msg = HORIZSCROLL;
			if (msg != 0)	{
                if (SendMessage(wnd, msg, dir, 0))
                    ExtendBlock(wnd, x, y);
	            SendMessage(wnd, PAINT, 0, 0);
			}
        }
        return TRUE;
    }
    if (!InsideRect(p1, p2, rc))
        return FALSE;
    if (TextBlockMarked(wnd))    {
        ClearTextBlock(wnd);
        SendMessage(wnd, PAINT, 0, 0);
    }
    if (wnd->wlines)    {
        if (MouseY > wnd->wlines-1)
            return TRUE;
        lp = TextLine(wnd, MouseY+wnd->wtop);
        len = (int) (strchr(lp, '\n') - lp) / sizeof(ATTRCHR);
        MouseX = min(MouseX + wnd->wleft, len) - wnd->wleft;
        if (MouseX < wnd->wleft)    {
            MouseX = 0;
            SendMessage(wnd, KEYBOARD, HOME, 0);
        }
        ButtonDown = TRUE;
        ButtonX = MouseX;
        ButtonY = MouseY;
    }
    else
        MouseX = MouseY = 0;
    wnd->WndRow = MouseY;
    SetLinePointer(wnd, MouseY+wnd->wtop);

    if (isMultiLine(wnd) ||
        (!TextBlockMarked(wnd)
            && MouseX+wnd->wleft < wnd->textlen))
        wnd->CurrCol = MouseX+wnd->wleft;
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    return TRUE;
}
/* ----------- MOUSE_MOVED Message ---------- */
static int MouseMovedMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int MouseX = (int) p1 - GetClientLeft(wnd);
    int MouseY = (int) p2 - GetClientTop(wnd);
    RECT rc = ClientRect(wnd);
    if (!InsideRect(p1, p2, rc))
        return FALSE;
    if (MouseY > wnd->wlines-1)
        return FALSE;
    if (ButtonDown)    {
        SetAnchor(wnd, ButtonX+wnd->wleft, ButtonY+wnd->wtop);
        TextMarking = TRUE;
		rc = WindowRect(wnd);
        SendMessage(NULL,MOUSE_TRAVEL,(PARAM) &rc, 0);
        ButtonDown = FALSE;
    }
    if (TextMarking && !(WindowMoving || WindowSizing))    {
        ExtendBlock(wnd, MouseX, MouseY);
        return TRUE;
    }
    return FALSE;
}
static void StopMarking(WINDOW wnd)
{
    TextMarking = FALSE;
    if (wnd->BlkBegLine > wnd->BlkEndLine)    {
        swap(wnd->BlkBegLine, wnd->BlkEndLine);
        swap(wnd->BlkBegCol, wnd->BlkEndCol);
    }
    if (wnd->BlkBegLine == wnd->BlkEndLine &&
            wnd->BlkBegCol > wnd->BlkEndCol)
        swap(wnd->BlkBegCol, wnd->BlkEndCol);
}
/* ----------- BUTTON_RELEASED Message ---------- */
static int ButtonReleasedMsg(WINDOW wnd)
{
    ButtonDown = FALSE;
    if (TextMarking && !(WindowMoving || WindowSizing))  {
        /* release the mouse ouside the edit box */
        SendMessage(NULL, MOUSE_TRAVEL, 0, 0);
        StopMarking(wnd);
        return TRUE;
    }
    PrevY = -1;
    return FALSE;
}
/* ---- Process text block keys for multiline text box ---- */
static void DoMultiLines(WINDOW wnd, int c, PARAM p2)
{
    if (!KeyBoardMarking)    {
        if ((int)p2 & (LEFTSHIFT | RIGHTSHIFT))    {
            switch (c)    {
                case HOME:
                case CTRL_HOME:
                case CTRL_BS:
                case PGUP:
                case CTRL_PGUP:
                case UP:
                case BS:
                case END:
                case CTRL_END:
                case PGDN:
                case CTRL_PGDN:
                case DN:
                case FWD:
                case CTRL_FWD:
                    KeyBoardMarking = TextMarking = TRUE;
                    SetAnchor(wnd, wnd->CurrCol, wnd->CurrLine);
                    break;
                default:
                    break;
            }
        }
    }
}
/* ---------- page/scroll keys ----------- */
static int DoScrolling(WINDOW wnd, int c, PARAM p2)
{
    switch (c)    {
        case PGUP:
        case PGDN:
            if (isMultiLine(wnd))
                BaseWndProc(EDITBOX, wnd, KEYBOARD, c, p2);
            break;
        case CTRL_PGUP:
        case CTRL_PGDN:
            BaseWndProc(EDITBOX, wnd, KEYBOARD, c, p2);
            break;
        case HOME:
            Home(wnd);
            break;
        case END:
            End(wnd);
            break;
        case CTRL_FWD:
            NextWord(wnd);
            break;
        case CTRL_BS:
            PrevWord(wnd);
            break;
        case CTRL_HOME:
            if (isMultiLine(wnd))    {
                SendMessage(wnd, SCROLLDOC, TRUE, 0);
                wnd->CurrLine = 0;
                wnd->WndRow = 0;
            }
            Home(wnd);
            break;
        case CTRL_END:
			if (isMultiLine(wnd) &&
					wnd->WndRow+wnd->wtop+1 < wnd->wlines
						&& wnd->wlines > 0) {
                SendMessage(wnd, SCROLLDOC, FALSE, 0);
                SetLinePointer(wnd, wnd->wlines-1);
                wnd->WndRow =
                    min(ClientHeight(wnd)-1, wnd->wlines-1);
                Home(wnd);
            }
            End(wnd);
            break;
        case UP:
            if (isMultiLine(wnd))
                Upward(wnd);
            break;
        case DN:
            if (isMultiLine(wnd))
                Downward(wnd);
            break;
        case FWD:
            Forward(wnd);
            break;
        case BS:
            Backward(wnd);
            break;
        default:
            return FALSE;
    }
    if (!KeyBoardMarking && TextBlockMarked(wnd))    {
        ClearTextBlock(wnd);
        SendMessage(wnd, PAINT, 0, 0);
    }
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    return TRUE;
}
/* -------------- Del key ---------------- */
static void DelKey(WINDOW wnd)
{
    ATTRCHR *currchar = CurrChar;
    int repaint = currchar->ch == '\n';
    if (TextBlockMarked(wnd))    {
        SendMessage(wnd, COMMAND, ID_DELETETEXT, 0);
        SendMessage(wnd, PAINT, 0, 0);
        return;
    }
    if (/*isMultiLine(wnd) &&*/ currchar->ch == '\n' && (currchar+1)->ch == '\0')
        return;
	if (currchar->ch == '\0')
		return;
	memcpy(currchar, currchar+1, (wnd->textlen - (currchar - wnd->text) + 1) * sizeof(ATTRCHR));
	wnd->textlen--;
    if (repaint)    {
        BuildTextPointers(wnd);
        SendMessage(wnd, PAINT, 0, 0);
    }
    else    {
        ModTextPointers(wnd, wnd->CurrLine+1, -1);
        WriteTextLine(wnd, NULL, wnd->WndRow+wnd->wtop, FALSE);
    }
	ChangedText(wnd, TRUE);
}
/* ------------ Tab key ------------ */
static void TabKey(WINDOW wnd, PARAM p2)
{
    if (isMultiLine(wnd))    {
        int insmd = wnd->InsertMode;
        do  {
            char *cc = CurrChar+1;
            if (!insmd && *cc == '\0')
                break;
            if (wnd->textlen == wnd->MaxTextLength)
                break;
            SendMessage(wnd,KEYBOARD,insmd ? ' ' : FWD,0);
        } while (wnd->CurrCol % cfg.Tabs);
    }
	else
	    PostMessage(GetParent(wnd), KEYBOARD, '\t', p2);
}
/* ------------ Shift+Tab key ------------ */
static void ShiftTabKey(WINDOW wnd, PARAM p2)
{
    if (isMultiLine(wnd))    {
        do  {
            if (CurrChar == GetText(wnd))
                break;
            SendMessage(wnd,KEYBOARD,BS,0);
        } while (wnd->CurrCol % cfg.Tabs);
    }
	else
	    PostMessage(GetParent(wnd), KEYBOARD, SHIFT_HT, p2);
}
/* --------- All displayable typed keys ------------- */
static void KeyTyped(WINDOW wnd, int c)
{
    ATTRCHR *currchar = CurrChar;
    if ((c != '\n' && c < ' ') || (c & OFFSET))
        /* ---- not recognized by editor --- */
        return;
    if (!isMultiLine(wnd) && TextBlockMarked(wnd))    {
		SendMessage(wnd, CLEARTEXT, 0, 0);
        currchar = CurrChar;
    }
    if (currchar->ch == '\0')    {
	    /* ---- test typing at end of text ---- */
	    if (wnd->textlen >= wnd->MaxTextLength)    {
	        /* ---- typing at the end of maximum buffer ---- */
	        beep();
	        return;
	    }
		if (wnd->textlen >= wnd->textalloc-2)
		{
			wnd->textalloc += GROWLENGTH+2;
			wnd->text = (ATTRCHR *)DFrealloc(wnd->text, wnd->textalloc * sizeof(ATTRCHR));
			currchar = CurrChar;
		}
        /* --- insert a newline at end of text --- */
        currchar->ch = '\n';
        (currchar+1)->ch = '\0';
		wnd->textlen++;
        BuildTextPointers(wnd);
    }
    /* --- displayable char or newline --- */
    if (c == '\n' || wnd->InsertMode || currchar->ch == '\n') {
        /* ------ inserting the keyed character ------ */
	    if (wnd->textlen == wnd->MaxTextLength)    {
	        /* --- text buffer is at maximum size --- */
	        beep();
	        return;
	    }
		if (wnd->textlen >= wnd->textalloc - 2) {
            /* --- the current text buffer is full --- */
            /* ---- increase the text buffer size ---- */
            wnd->textalloc += GROWLENGTH + 2;
            wnd->text = DFrealloc(wnd->text, wnd->textalloc * sizeof(ATTRCHR));
            currchar = CurrChar;
        }
        memmove(currchar+1, currchar, (wnd->textlen - (currchar - wnd->text)+1) * sizeof(ATTRCHR));
		wnd->textlen++;
        ModTextPointers(wnd, wnd->CurrLine+1, 1);
        if (isMultiLine(wnd) && wnd->wlines > 1)
            wnd->textwidth = max(wnd->textwidth,
                (int) (TextLine(wnd, wnd->CurrLine+1)-
                TextLine(wnd, wnd->CurrLine)));
        else
            wnd->textwidth = max(wnd->textwidth,
                wnd->textlen);
        WriteTextLine(wnd, NULL,
            wnd->wtop+wnd->WndRow, FALSE);
    }
    /* ----- put the char in the buffer ----- */
    currchar->ch = c;
	currchar->attrib = clr(WndForeground(wnd), WndBackground(wnd));
	ChangedText(wnd, TRUE);
    if (c == '\n')    {
        wnd->wleft = 0;
        BuildTextPointers(wnd);
        End(wnd);
        Forward(wnd);
        PostMessage(wnd, PAINT, 0, 0);
        return;
    }
    /* ---------- test end of window --------- */
    if (WndCol == ClientWidth(wnd)-1)    {
        if (!isMultiLine(wnd))	{
			if (!(currchar == wnd->text+wnd->MaxTextLength-2))
            SendMessage(wnd, HORIZSCROLL, TRUE, 0);
		}
		else	{
			ATTRCHR *cp = currchar;
	        while (cp->ch != ' ' && cp != TextLine(wnd, wnd->CurrLine))
	            --cp;
	        if (cp == TextLine(wnd, wnd->CurrLine) ||
	                !wnd->WordWrapMode)
	            SendMessage(wnd, HORIZSCROLL, TRUE, 0);
	        else    {
	            int dif = 0;
	            if (c != ' ')    {
	                dif = (int) (currchar - cp) - 1;
	                wnd->CurrCol -= dif;
    	            SendMessage(wnd, KEYBOARD, DEL, 0);
	            }
				else
				{
    	            SendMessage(wnd, KEYBOARD, DEL, 0);
				}
	            DefaultWndProc(wnd, KEYBOARD, '\n', 0);
	            currchar = CurrChar;
	            wnd->CurrCol = dif;
	            if (c == ' ')
	                return;
	        }
	    }
	}
    /* ------ display the character ------ */
    SetStandardColor(wnd);
	if (wnd->protect)
		c = '*';
    PutWindowChar(wnd, c, WndCol, wnd->WndRow);
    /* ----- advance the pointers ------ */
    wnd->CurrCol++;
}

/* ------------ screen changing key strokes ------------- */
static void DoKeyStroke(WINDOW wnd, int c, PARAM p2)
{
    switch (c)    {
        case RUBOUT: // backspace
			if ((int)p2 & CTRLKEY)
				break;
			if (wnd->CurrCol == 0 && wnd->CurrLine == 0)
				break;
		    if (TextBlockMarked(wnd))    {
		        SendMessage(wnd, COMMAND, ID_DELETETEXT, 0);
		        SendMessage(wnd, PAINT, 0, 0);
		        return;
		    }
			SendMessage(wnd, KEYBOARD, BS, 0);
			SendMessage(wnd, KEYBOARD, DEL, 0);
			break;
        case DEL:
            DelKey(wnd);
            break;
        case SHIFT_HT:
            ShiftTabKey(wnd, p2);
            break;
        case '\t':
            TabKey(wnd, p2);
            break;
        case '\r':
            if (!isMultiLine(wnd))    {
                PostMessage(GetParent(wnd), KEYBOARD, c, p2);
                break;
            }
            c = '\n';
        default:
            if (TextBlockMarked(wnd))    {
                SendMessage(wnd, COMMAND, ID_DELETETEXT, 0);
                SendMessage(wnd, PAINT, 0, 0);
            }
            KeyTyped(wnd, c);
            break;
    }
}
/* ----------- KEYBOARD Message ---------- */
static int KeyboardMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int c = (int) p1;
    if (WindowMoving || WindowSizing || ((int)p2 & ALTKEY))
        return FALSE;
    switch (c)    {
        /* --- these keys get processed by lower classes --- */
        case ESC:
        case F1:
        case F2:
        case F3:
        case F4:
        case F5:
        case F6:
        case F7:
        case F8:
        case F9:
        case F10:
        case INS:
        case SHIFT_INS:
        case SHIFT_DEL:
            return FALSE;
        /* --- these keys get processed here --- */
        case CTRL_FWD:
        case CTRL_BS:
        case CTRL_HOME:
        case CTRL_END:
        case CTRL_PGUP:
        case CTRL_PGDN:
            break;
		case CTRL_Z:
			UndoCmd(wnd);
			break;
        default:
            /* other ctrl keys get processed by lower classes */
            if ((int)p2 & CTRLKEY)
                return FALSE;
            /* --- all other keys get processed here --- */
            break;
    }
    DoMultiLines(wnd, c, p2);
    if (DoScrolling(wnd, c, p2))    {
        if (KeyBoardMarking)
            ExtendBlock(wnd, WndCol, wnd->WndRow);
    }
    else if (!TestAttribute(wnd, READONLY))    {
        DoKeyStroke(wnd, c, p2);
        SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    }
	else if (c == '\t')
	    PostMessage(GetParent(wnd), KEYBOARD, '\t', p2);
	else
		beep();
    return TRUE;
}
/* ----------- SHIFT_CHANGED Message ---------- */
static void ShiftChangedMsg(WINDOW wnd, PARAM p1)
{
    if (!((int)p1 & (LEFTSHIFT | RIGHTSHIFT)) &&
                                   KeyBoardMarking)    {
        StopMarking(wnd);
        KeyBoardMarking = FALSE;
    }
}
/* ----------- ID_DELETETEXT Command ---------- */
static void DeleteTextCmd(WINDOW wnd)
{
    if (TextBlockMarked(wnd))    {
        ATTRCHR *bbl=TextLine(wnd,wnd->BlkBegLine)+wnd->BlkBegCol;
        ATTRCHR *bel=TextLine(wnd,wnd->BlkEndLine)+wnd->BlkEndCol;
        int len = (int) (bel - bbl);
        SaveDeletedText(wnd, bbl, len);
		ChangedText(wnd, TRUE);
		memcpy(bbl, bel, (wnd->textlen - (bel - wnd->text) + 1) * sizeof(ATTRCHR));
		wnd->textlen -= len;
        wnd->CurrLine = TextLineNumber(wnd, bbl-wnd->BlkBegCol);
        wnd->CurrCol = wnd->BlkBegCol;
        wnd->WndRow = wnd->BlkBegLine - wnd->wtop;
        if (wnd->WndRow < 0)    {
            wnd->wtop = wnd->BlkBegLine;
            wnd->WndRow = 0;
        }
        SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
        ClearTextBlock(wnd);
        BuildTextPointers(wnd);
    }
}
/* ----------- ID_CLEAR Command ---------- */
static void ClearCmd(WINDOW wnd)
{
    if (TextBlockMarked(wnd))    {
        ATTRCHR *bbl=TextLine(wnd,wnd->BlkBegLine)+wnd->BlkBegCol;
        ATTRCHR *bel=TextLine(wnd,wnd->BlkEndLine)+wnd->BlkEndCol;
        int len = (int) (bel - bbl);
        SaveDeletedText(wnd, bbl, len);
        wnd->CurrLine = TextLineNumber(wnd, bbl);
        wnd->CurrCol = wnd->BlkBegCol;
        wnd->WndRow = wnd->BlkBegLine - wnd->wtop;
        if (wnd->WndRow < 0)    {
            wnd->WndRow = 0;
            wnd->wtop = wnd->BlkBegLine;
        }
        /* ------ change all text lines in block to \n ----- */
        while (bbl < bel)    {
            ATTRCHR *cp = bbl;
			while (cp->ch && cp->ch != '\n')
				cp++;
            if (cp > bel)
                cp = bel;
            memcpy(bbl, cp, (wnd->textlen - (cp - wnd->text) + 1) * sizeof(ATTRCHR));
			wnd->textlen -= (int)(cp - bbl);
            bel -= (int) (cp - bbl);
            bbl++;
        }
        ClearTextBlock(wnd);
        BuildTextPointers(wnd);
        SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
		ChangedText(wnd, TRUE);
    }
}
/* ----------- ID_UNDO Command ---------- */
static void UndoCmd(WINDOW wnd)
{
    if (wnd->DeletedText != NULL)    {
        PasteText(wnd, wnd->DeletedText, wnd->DeletedLength);
        free(wnd->DeletedText);
        wnd->DeletedText = NULL;
        wnd->DeletedLength = 0;
        SendMessage(wnd, PAINT, 0, 0);
    }
}
/* ----------- ID_PARAGRAPH Command ---------- */
static void ParagraphCmd(WINDOW wnd)
{
    int bc, fl;
    ATTRCHR *bl, *bbl, *bel, *bb;

    ClearTextBlock(wnd);
    /* ---- forming paragraph from cursor position --- */
    fl = wnd->wtop + wnd->WndRow;
    bbl = bel = bl = TextLine(wnd, wnd->CurrLine);
    if ((bc = wnd->CurrCol) >= ClientWidth(wnd))
        bc = 0;
    Home(wnd);
    /* ---- locate the end of the paragraph ---- */
    while (bel->ch)    {
        int blank = TRUE;
        ATTRCHR *bll = bel;
        /* --- blank line marks end of paragraph --- */
        while (bel->ch && bel->ch != '\n')    {
            if (bel->ch != ' ')
                blank = FALSE;
            bel++;
        }
        if (blank)    {
            bel = bll;
            break;
        }
        if (bel->ch)
            bel++;
    }
    if (bel == bbl)    {
        SendMessage(wnd, KEYBOARD, DN, 0);
        return;
    }
    if (bel->ch == '\0')
        --bel;
    if (bel->ch == '\n')
        --bel;
    /* --- change all newlines in block to spaces --- */
    while (CurrChar < bel)    {
        if (CurrChar->ch == '\n')    {
            CurrChar->ch = ' ';
            wnd->CurrLine++;
            wnd->CurrCol = 0;
        }
        else
            wnd->CurrCol++;
    }
    /* ---- insert newlines at new margin boundaries ---- */
    bb = bbl;
    while (bbl < bel)    {
        bbl++;
        if ((int)(bbl - bb) == ClientWidth(wnd)-1)    {
            while (bbl->ch != ' ' && bbl > bb)
                --bbl;
            if (bbl->ch != ' ')    {
				while (bbl->ch && bbl->ch != ' ')
					bbl++;
				if (!bbl->ch)
					bbl = NULL;
                if (bbl == NULL || bbl >= bel)
                    break;
            }
            bbl->ch = '\n';
            bb = bbl+1;
        }
    }
    BuildTextPointers(wnd);
    /* --- put cursor back at beginning --- */
    wnd->CurrLine = TextLineNumber(wnd, bl);
    wnd->CurrCol = bc;
    if (fl < wnd->wtop)
        wnd->wtop = fl;
    wnd->WndRow = fl - wnd->wtop;
    SendMessage(wnd, PAINT, 0, 0);
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	ChangedText(wnd, TRUE);
    BuildTextPointers(wnd);
}
/* ----------- COMMAND Message ---------- */
static int CommandMsg(WINDOW wnd, PARAM p1)
{
    switch ((int)p1)    {
		case ID_SEARCH:
			SearchText(wnd);
			return TRUE;
		case ID_REPLACE:
			ReplaceText(wnd);
			return TRUE;
		case ID_SEARCHNEXT:
			SearchNext(wnd);
			return TRUE;
		case ID_CUT:
			CopyToClipboard(wnd);
			SendMessage(wnd, COMMAND, ID_DELETETEXT, 0);
			SendMessage(wnd, PAINT, 0, 0);
			return TRUE;
		case ID_COPY:
			CopyToClipboard(wnd);
			ClearTextBlock(wnd);
			SendMessage(wnd, PAINT, 0, 0);
			return TRUE;
		case ID_PASTE:
			PasteFromClipboard(wnd);
			SendMessage(wnd, PAINT, 0, 0);
			return TRUE;
        case ID_DELETETEXT:
            DeleteTextCmd(wnd);
			SendMessage(wnd, PAINT, 0, 0);
            return TRUE;
        case ID_CLEAR:
            ClearCmd(wnd);
			SendMessage(wnd, PAINT, 0, 0);
            return TRUE;
        case ID_UNDO:
            UndoCmd(wnd);
			SendMessage(wnd, PAINT, 0, 0);
            return TRUE;
        case ID_PARAGRAPH:
            ParagraphCmd(wnd);
			SendMessage(wnd, PAINT, 0, 0);
            return TRUE;
        default:
            break;
    }
    return FALSE;
}
/* ---------- CLOSE_WINDOW Message ----------- */
static int CloseWindowMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int rtn;
    SendMessage(NULL, HIDE_CURSOR, 0, 0);
    if (wnd->DeletedText != NULL)
        free(wnd->DeletedText);
    rtn = BaseWndProc(EDITBOX, wnd, CLOSE_WINDOW, p1, p2);
	if (wnd->text != NULL)	{
		free(wnd->text);
		wnd->text = NULL;
	}
    return rtn;
}
/* ------- Window processing module for EDITBOX class ------ */
int EditBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    int rtn;
    switch (msg)    {
        case CREATE_WINDOW:
            return CreateWindowMsg(wnd);
        case ADDTEXT:
            return AddTextMsg(wnd, p1, p2);
        case SETTEXT:
            return SetTextMsg(wnd, p1, p2);
        case CLEARTEXT:
			return ClearTextMsg(wnd);
        case GETTEXT:
            return GetTextMsg(wnd, p1, p2);
        case SETTEXTLENGTH:
            return SetTextLengthMsg(wnd, (unsigned) p1);
        case KEYBOARD_CURSOR:
            KeyboardCursorMsg(wnd, p1, p2);
			return TRUE;
        case SETFOCUS:
			if (!(int)p1)
				SendMessage(NULL, HIDE_CURSOR, 0, 0);
        case PAINT:
        case MOVE:
            rtn = BaseWndProc(EDITBOX, wnd, msg, p1, p2);
            SendMessage(wnd,KEYBOARD_CURSOR,WndCol,wnd->WndRow);
            return rtn;
        case SIZE:
            return SizeMsg(wnd, p1, p2);
        case SCROLL:
            return ScrollMsg(wnd, p1);
        case HORIZSCROLL:
            return HorizScrollMsg(wnd, p1);
        case SCROLLPAGE:
            return ScrollPageMsg(wnd, p1);
        case HORIZPAGE:
            return HorizPageMsg(wnd, p1);
        case LEFT_BUTTON:
            if (LeftButtonMsg(wnd, p1, p2))
                return TRUE;
            break;
        case MOUSE_MOVED:
            if (MouseMovedMsg(wnd, p1, p2))
                return TRUE;
            break;
        case BUTTON_RELEASED:
            if (ButtonReleasedMsg(wnd))
                return TRUE;
            break;
        case KEYBOARD:
            if (KeyboardMsg(wnd, p1, p2))
                return TRUE;
            break;
        case SHIFT_CHANGED:
            ShiftChangedMsg(wnd, p1);
            break;
        case COMMAND:
            if (CommandMsg(wnd, p1))
                return TRUE;
            break;
        case CLOSE_WINDOW:
            return CloseWindowMsg(wnd, p1, p2);
        default:
            break;
    }
    return BaseWndProc(EDITBOX, wnd, msg, p1, p2);
}
/* ------ save deleted text for the Undo command ------ */
static void SaveDeletedText(WINDOW wnd, ATTRCHR *bbl, int len)
{
	char *cp ;
    wnd->DeletedLength = len;
    wnd->DeletedText=DFrealloc(wnd->DeletedText, (len + 1) * sizeof(ATTRCHR));
	memcpy(wnd->DeletedText, bbl, (len + 1 ) * sizeof(ATTRCHR));
}
/* ---- cursor right key: right one character position ---- */
static void Forward(WINDOW wnd)
{
    ATTRCHR *cc = CurrChar+1;
    if (cc->ch == '\0')
        return;
    if (CurrChar->ch == '\n')    {
        Home(wnd);
        Downward(wnd);
    }
    else    {
        wnd->CurrCol++;
        if (WndCol == ClientWidth(wnd))
            SendMessage(wnd, HORIZSCROLL, TRUE, 0);
    }
}
/* ----- stick the moving cursor to the end of the line ---- */
static void StickEnd(WINDOW wnd)
{
    ATTRCHR *cp = TextLine(wnd, wnd->CurrLine);
    ATTRCHR *cp1 = cp; 
    int len ;
	while (cp1->ch && cp1->ch != '\n')
		cp1++;
	len = (cp1 - cp) ;
    wnd->CurrCol = min(len, wnd->CurrCol);
    if (wnd->wleft > wnd->CurrCol)    {
        wnd->wleft = max(0, wnd->CurrCol - 4);
        SendMessage(wnd, PAINT, 0, 0);
    }
    else if (wnd->CurrCol-wnd->wleft >= ClientWidth(wnd))    {
        wnd->wleft = wnd->CurrCol - (ClientWidth(wnd)-1);
        SendMessage(wnd, PAINT, 0, 0);
    }
}
/* --------- cursor down key: down one line --------- */
static void Downward(WINDOW wnd)
{
    if (isMultiLine(wnd) &&
            wnd->WndRow+wnd->wtop+1 < wnd->wlines)  {
        wnd->CurrLine++;
        if (wnd->WndRow == ClientHeight(wnd)-1)
			SendMessage(wnd, SCROLL, TRUE, 0);
        wnd->WndRow++;
        StickEnd(wnd);
    }
}
/* -------- cursor up key: up one line ------------ */
static void Upward(WINDOW wnd)
{
    if (isMultiLine(wnd) && wnd->CurrLine != 0)    {
        --wnd->CurrLine;
        if (wnd->WndRow == 0)
			SendMessage(wnd, SCROLL, FALSE, 0);
        --wnd->WndRow;
        StickEnd(wnd);
    }
}
/* ---- cursor left key: left one character position ---- */
static void Backward(WINDOW wnd)
{
    if (wnd->CurrCol)    {
        --wnd->CurrCol;
        if (wnd->CurrCol < wnd->wleft)
            SendMessage(wnd, HORIZSCROLL, FALSE, 0);
    }
    else if (isMultiLine(wnd) && wnd->CurrLine != 0)    {
        Upward(wnd);
        End(wnd);
    }
}
/* -------- End key: to end of line ------- */
static void End(WINDOW wnd)
{
    while (CurrChar->ch && CurrChar->ch != '\n')
        ++wnd->CurrCol;
    if (WndCol >= ClientWidth(wnd))    {
        wnd->wleft = wnd->CurrCol - (ClientWidth(wnd)-1);
        SendMessage(wnd, PAINT, 0, 0);
    }
}
/* -------- Home key: to beginning of line ------- */
static void Home(WINDOW wnd)
{
    wnd->CurrCol = 0;
    if (wnd->wleft != 0)    {
        wnd->wleft = 0;
        SendMessage(wnd, PAINT, 0, 0);
    }
}
/* -- Ctrl+cursor right key: to beginning of next word -- */
static void NextWord(WINDOW wnd)
{
    int savetop = wnd->wtop;
    int saveleft = wnd->wleft;
    ClearVisible(wnd);
    while (!isWhite(CurrChar->ch))    {
        ATTRCHR *cc = CurrChar+1;
        if (cc->ch == '\0')
            break;
        Forward(wnd);
    }
    while (isWhite(CurrChar->ch))    {
        ATTRCHR *cc = CurrChar+1;
        if (cc->ch == '\0')
            break;
        Forward(wnd);
    }
    SetVisible(wnd);
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    if (wnd->wtop != savetop || wnd->wleft != saveleft)
        SendMessage(wnd, PAINT, 0, 0);
}
/* -- Ctrl+cursor left key: to beginning of previous word -- */
static void PrevWord(WINDOW wnd)
{
    int savetop = wnd->wtop;
    int saveleft = wnd->wleft;
    ClearVisible(wnd);
    Backward(wnd);
    while (isWhite(CurrChar->ch))    {
        if (wnd->CurrLine == 0 && wnd->CurrCol == 0)
            break;
        Backward(wnd);
    }
    while (wnd->CurrCol != 0 && !isWhite(CurrChar->ch))
        Backward(wnd);
    if (isWhite(CurrChar->ch))
        Forward(wnd);
    SetVisible(wnd);
    if (wnd->wleft != saveleft)
        if (wnd->CurrCol >= saveleft)
            if (wnd->CurrCol - saveleft < ClientWidth(wnd))
                wnd->wleft = saveleft;
    SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
    if (wnd->wtop != savetop || wnd->wleft != saveleft)
        SendMessage(wnd, PAINT, 0, 0);
}
/* ----- modify text pointers from a specified position
                by a specified plus or minus amount ----- */
static void ModTextPointers(WINDOW wnd, int lineno, int var)
{
    while (lineno < wnd->wlines)
        *((wnd->TextPointers) + lineno++) += var ;
}
/* ----- set anchor point for marking text block ----- */
static void SetAnchor(WINDOW wnd, int mx, int my)
{
    ClearTextBlock(wnd);
    /* ------ set the anchor ------ */
    wnd->BlkBegLine = wnd->BlkEndLine = my;
    wnd->BlkBegCol = wnd->BlkEndCol = mx;
    SendMessage(wnd, PAINT, 0, 0);
}


