/* ------------- editor.c ------------ */
#include "dflat.h"

#define pTab ('\t' + 0x80)
#define sTab ('\f' + 0x80)

/* ---------- SETTEXT Message ------------ */
static int SetTextMsg(WINDOW wnd, char *Buf, BOOL wide)
{
   	unsigned char *tp ;
	ATTRCHR *ep, *ttp, *tpb;
	int attr = clr(WndForeground(wnd), WndBackground(wnd));
   	int x = 0;
   	int sz = 0;
	int rtn;

	if (wide)
	{
		tpb = Buf;
	    /* --- compute the buffer size based on tabs in the text --- */
	    while (tpb->ch)    {
	        if (tpb->ch == '\t')    {
	            /* --- tab, adjust the buffer length --- */
	            int sps = cfg.Tabs - (x % cfg.Tabs);
	            sz += sps;
	            x += sps;
	        }
	        else    {
	            /* --- not a tab, count the character --- */
	            sz++;
	            x++;
	        }
	        if (tpb->ch == '\n')
	            x = 0;    /* newline, reset x --- */
	        tpb++;
	    }
	    /* --- allocate a buffer --- */
	    ep = DFcalloc(1, (sz+1) * sizeof(ATTRCHR));
	    /* --- detab the input file --- */
	    tpb = Buf;
	    ttp = ep;
	    x = 0;
	    while (tpb->ch)    {
	        /* --- put the character (\t, too) into the buffer --- */
	        x++;
	        /* --- expand tab into subst tab (\f + 0x80)
							and expansions (\t + 0x80) --- */
	        if (tpb->ch == '\t')	{
				if (cfg.TabsAsSpaces)
				{
			        ttp->ch = ' ';
					ttp++->attrib = tpb->attrib;
		            while ((x % cfg.Tabs) != 0)
		                ttp->ch = ' ', ttp++->attrib = tpb->attrib, x++;
				}
				else
				{
			        ttp->ch = sTab;	/* --- substitute tab character --- */
					ttp++->attrib = tpb->attrib;
		            while ((x % cfg.Tabs) != 0)
		                ttp->ch = pTab, ttp++->attrib = tpb->attrib, x++;
				}
			}
			else	{
		        *ttp = *tpb;
				ttp++;
	        	if (tpb->ch == '\n')
	            	x = 0;
			}
	        tpb++;
	    }
	}
	else
	{
		tp = Buf;
	    /* --- compute the buffer size based on tabs in the text --- */
	    while (*tp)    {
	        if (*tp == '\t')    {
	            /* --- tab, adjust the buffer length --- */
	            int sps = cfg.Tabs - (x % cfg.Tabs);
	            sz += sps;
	            x += sps;
	        }
	        else    {
	            /* --- not a tab, count the character --- */
	            sz++;
	            x++;
	        }
	        if (*tp == '\n')
	            x = 0;    /* newline, reset x --- */
	        tp++;
	    }
	    /* --- allocate a buffer --- */
	    ep = DFcalloc(1, (sz+1) * sizeof(ATTRCHR));
	    /* --- detab the input file --- */
	    tp = Buf;
	    ttp = ep;
	    x = 0;
	    while (*tp)    {
	        /* --- put the character (\t, too) into the buffer --- */
	        x++;
	        /* --- expand tab into subst tab (\f + 0x80)
							and expansions (\t + 0x80) --- */
	        if (*tp == '\t')	{
				if (cfg.TabsAsSpaces)
				{
			        ttp->ch = ' ';
					ttp++->attrib = attr;
		            while ((x % cfg.Tabs) != 0)
		                ttp->ch = ' ', ttp++->attrib = attr, x++;
				}
				else
				{
			        ttp->ch = sTab;	/* --- substitute tab character --- */
					ttp++->attrib = attr;
		            while ((x % cfg.Tabs) != 0)
		                ttp->ch = pTab, ttp++->attrib = attr, x++;
				}
			}
			else	{
		        ttp->ch = *tp;
				ttp++->attrib = attr;
	        	if (*tp == '\n')
	            	x = 0;
			}
	        tp++;
	    }
	}
    ttp->ch = '\0';
	rtn = BaseWndProc(EDITOR, wnd, SETTEXT, (PARAM) ep, 1);
    free(ep);
	return rtn;
}
static int GetTextMsg(WINDOW wnd, char *Buf, int len)
{
	ATTRCHR *cp = GetText(wnd);
	while (cp->ch && len)
	{
		if (cp->ch == sTab)
			*Buf++ = '\t';
		else if (cp->ch != pTab)
		{
			*Buf++ = cp->ch;
		}
		cp++;
	}
	*Buf++ = '\0';
}
void CollapseTabs(WINDOW wnd)
{
	ATTRCHR *cp = wnd->text, *cp2;
	while (cp->ch)	{
		if (cp->ch == sTab)	{
			cp->ch = '\t';
			cp2 = cp + 1;
			while ((cp2)->ch == pTab)
				cp2++;
			memmove(cp+1, cp2, (wnd->textlen + wnd->text - cp2 + 1) * sizeof(ATTRCHR));
			wnd->textlen -= cp2 - cp - 1;
		}
		cp++;
	}
}

void ExpandTabs(WINDOW wnd)
{
	int Holdwtop = wnd->wtop;
	int Holdwleft = wnd->wleft;
	int HoldRow = wnd->CurrLine;
	int HoldCol = wnd->CurrCol;
	int HoldwRow = wnd->WndRow;
	SendMessage(wnd, SETTEXT, (PARAM) wnd->text, 1);
	wnd->wtop = Holdwtop;
	wnd->wleft = Holdwleft;
	wnd->CurrLine = HoldRow;
	wnd->CurrCol = HoldCol;
	wnd->WndRow = HoldwRow;
	SendMessage(wnd, PAINT, 0, 0);
	SendMessage(wnd, KEYBOARD_CURSOR, 0, wnd->WndRow);
}

/* --- When inserting or deleting, adjust next following tab, same line --- */
static void AdjustTab(WINDOW wnd)
{
    /* ---- test if there is a tab beyond this character ---- */
	int col = wnd->CurrCol;
    while (CurrChar->ch && CurrChar->ch != '\n')    {
		if (CurrChar->ch == sTab)	{
			int exp = (cfg.Tabs - 1) - (wnd->CurrCol % cfg.Tabs);
	        wnd->CurrCol++;
			while (CurrChar->ch == pTab)
				BaseWndProc(EDITOR, wnd, KEYBOARD, DEL, 0);
			while (exp--)
				BaseWndProc(EDITOR, wnd, KEYBOARD, pTab, 0);
			break;
		}
        wnd->CurrCol++;
    }
	wnd->CurrCol = col;
}

void TurnOffDisplay(WINDOW wnd)
{
	SendMessage(NULL, HIDE_CURSOR, 0, 0);
    ClearVisible(wnd);
}

void TurnOnDisplay(WINDOW wnd)
{
    SetVisible(wnd);
	SendMessage(NULL, SHOW_CURSOR, (wnd->InsertMode), 0);
}

static void RepaintLine(WINDOW wnd)
{
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	WriteTextLine(wnd, NULL, wnd->CurrLine, FALSE);
}

/* --------- KEYBOARD Message ---------- */
static int KeyboardMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int c = (int) p1;
	BOOL delnl;
	PARAM pn = p1;
    if (WindowMoving || WindowSizing || ((int)p2 & ALTKEY))
        return FALSE;
    switch (c)    {
		case PGUP:
		case PGDN:
		case UP:
		case DN:
			pn = (PARAM) BS;
		case FWD:
		case BS:
			BaseWndProc(EDITOR, wnd, KEYBOARD, p1, p2);
		    TurnOffDisplay(wnd);
			while (CurrChar->ch == pTab)
				BaseWndProc(EDITOR, wnd, KEYBOARD, pn, p2);
			TurnOnDisplay(wnd);
			return TRUE;
		case RUBOUT: // backspace
			if ((int)p2 & CTRLKEY)
				break;
			if (wnd->CurrCol == 0 && wnd->CurrLine == 0)
				return TRUE;
			if (!TextBlockMarked(wnd))
			{
			    TurnOffDisplay(wnd);
				BaseWndProc(EDITOR, wnd, KEYBOARD, BS, p2);
				while (CurrChar->ch == pTab)
					BaseWndProc(EDITOR, wnd, KEYBOARD, BS, p2);
				TurnOnDisplay(wnd);
			}
			p1 = DEL;
			/* fallthrough */
		case DEL:
		    TurnOffDisplay(wnd);
			delnl = CurrChar->ch == '\n' || TextBlockMarked(wnd);
			BaseWndProc(EDITOR, wnd, KEYBOARD, p1, p2);
			while (CurrChar->ch == pTab)
				BaseWndProc(EDITOR, wnd, KEYBOARD, p1, p2);
			AdjustTab(wnd);
			TurnOnDisplay(wnd);
			RepaintLine(wnd);
			if (delnl)
				SendMessage(wnd, PAINT, 0, 0);
			return TRUE;
		case '\t':
		    TurnOffDisplay(wnd);
			c = wnd->InsertMode;
			if (cfg.TabsAsSpaces)
			{
				BaseWndProc(EDITOR, wnd, KEYBOARD, (PARAM) ' ', p2);
				wnd->InsertMode = TRUE;
	    	    while ((wnd->CurrCol % cfg.Tabs) != 0)
					BaseWndProc(EDITOR, wnd, KEYBOARD, ' ', p2);
			}
			else
			{
				BaseWndProc(EDITOR, wnd, KEYBOARD, (PARAM) sTab, p2);
				wnd->InsertMode = TRUE;
	    	    while ((wnd->CurrCol % cfg.Tabs) != 0)
					BaseWndProc(EDITOR, wnd, KEYBOARD, pTab, p2);
			}
			wnd->InsertMode = c;
			TurnOnDisplay(wnd);
			RepaintLine(wnd);
			return TRUE;
		default:
			if (((c & OFFSET) == 0) && (isprint(c) || c == '\r'))
			{
			    TurnOffDisplay(wnd);
				BaseWndProc(EDITOR, wnd, KEYBOARD, p1, p2);
				AdjustTab(wnd);
				TurnOnDisplay(wnd);
				RepaintLine(wnd);
				if (c == '\r')
					SendMessage(wnd, PAINT, 0, 0);
				return TRUE;
			}
			break;
	}
    return FALSE;
}

/* ------- Window processing module for EDITBOX class ------ */
int EditorProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    switch (msg)    {
		case KEYBOARD:
            if (KeyboardMsg(wnd, p1, p2))
                return TRUE;
            break;
		case SETTEXT:
			return SetTextMsg(wnd, (char *) p1, p2);
		case GETTEXT:
			return GetTextMsg(wnd, (char *) p1, p2);
        default:
            break;
    }
    return BaseWndProc(EDITOR, wnd, msg, p1, p2);
}

