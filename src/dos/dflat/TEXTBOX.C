/* ------------- textbox.c ------------ */

#include "dflat.h"

static void ComputeWindowTop(WINDOW);
static void ComputeWindowLeft(WINDOW);
static int ComputeVScrollBox(WINDOW);
static int ComputeHScrollBox(WINDOW);
static void MoveScrollBox(WINDOW, int);
static ATTRCHR *GetTextLine(WINDOW, int);

BOOL VSliding;
BOOL HSliding;

/* ------------ ADDTEXT Message -------------- */
static BOOL AddTextMsg(WINDOW wnd, char *txt, BOOL wide)
{
    /* --- append text to the textbox's buffer --- */
    unsigned adln ;
	if (wide)
		adln = tstrlen(txt);
	else
		adln = strlen(txt);
	if (adln == 0)
		return FALSE;
/*    if (adln > (unsigned)0xfff0)
        return FALSE;
*/
    if (wnd->text != NULL)    {
        /* ---- appending to existing text ---- */
/*		
        if ((long)txln+adln > (unsigned) 0xfff0)
            return FALSE;
*/
        if (wnd->textlen+adln + 2 > wnd->textalloc)    {
			wnd->textalloc = wnd->textlen + adln + 2;
            wnd->text = DFrealloc(wnd->text, wnd->textalloc * sizeof(ATTRCHR));
        }
    }
    else    {
        /* ------ 1st text appended ------ */
		wnd->textalloc = adln + 2;
        wnd->text = DFcalloc(1, wnd->textalloc * sizeof(ATTRCHR));
		wnd->textlen = 0;
    }
	ChangedText(wnd, TRUE);
    if (wnd->text != NULL)    {
        /* ---- append the text ---- */
		ATTRCHR *cp = wnd->text + wnd->textlen;
		int attr = clr(WndForeground(wnd),WndBackground(wnd));
		if (wide)
		{
			memcpy(cp, txt, adln * sizeof(ATTRCHR));
			cp += adln;
		}
		else
		{
			while (*txt)
			{
				cp->ch = *txt++;
				cp->attrib = attr;
				cp++;
			}
		}
		wnd->textlen += adln;
		if (cp[-1].ch != '\n')
		{
			cp->ch = '\n';
			cp->attrib = attr;
			cp++;
			wnd->textlen++;
		}
		cp->ch = '\0';
        BuildTextPointers(wnd);
		return TRUE;
    }
	return FALSE;
}

/* ------------ DELETETEXT Message -------------- */
static void DeleteTextMsg(WINDOW wnd, int lno)
{
	ATTRCHR *cp1 = TextLine(wnd, lno);
	--wnd->wlines;
	if (lno == wnd->wlines)
		cp1->ch = '\0';
	else 	{
		ATTRCHR *cp2 = TextLine(wnd, lno+1);
		memmove(cp1, cp2, (tstrlen(cp2)+1) * sizeof(ATTRCHR));
	}
    BuildTextPointers(wnd);
}

/* ------------ INSERTTEXT Message -------------- */
static void InsertTextMsg(WINDOW wnd, char *txt, int lno)
{
	if (AddTextMsg(wnd, txt, 0))	{
		int len = strlen(txt)+1;
		ATTRCHR *cp2 = TextLine(wnd, lno);
		ATTRCHR *cp1 = cp2+len;
		int attr = clr(WndForeground(wnd),WndBackground(wnd));
		memmove(cp1, cp2, (tstrlen(cp2)-len) * sizeof(ATTRCHR));
		while (*txt)
		{
			cp2->ch = *txt++;
			cp2->attrib = attr;
			cp2++;
		}
		if (cp2[-1].ch != '\n')
		{
			cp2->ch = '\n';
			cp2->attrib = attr;
		}
	    BuildTextPointers(wnd);
		ChangedText(wnd, TRUE);
	}
}

/* ------------ SETTEXT Message -------------- */
static void SetTextMsg(WINDOW wnd, char *txt, BOOL wide)
{
    /* -- assign new text value to textbox buffer -- */
    unsigned int len ;
	int attr = clr(WndForeground(wnd),WndBackground(wnd));
	ATTRCHR *cp;
	if (wide)
		len = tstrlen(txt);
	else
		len = strlen(txt);
	SendMessage(wnd, CLEARTEXT, 0, 0);
	wnd->textalloc = len + 2;
    wnd->textlen = len;
    wnd->text=DFrealloc(wnd->text, wnd->textalloc * sizeof(ATTRCHR));
	cp = wnd->text ;
	if (wide)
	{	
		memcpy(cp, txt, len* sizeof(ATTRCHR));
		cp += len;
	}
	else
	{
		while (*txt)
		{
			cp->ch = *txt++;
			cp->attrib = attr;
			cp++;
		}
	}
	if (cp[-1].ch != '\n')
	{
		cp->ch = '\n';
		cp->attrib = attr;
		cp++;
		wnd->textlen++;
	}
	cp->ch = '\0';
    BuildTextPointers(wnd);
}

/* ------------ CLEARTEXT Message -------------- */
static void ClearTextMsg(WINDOW wnd)
{
    /* ----- clear text from textbox ----- */
    if (wnd->text != NULL)
        free(wnd->text);
    wnd->text = NULL;
    wnd->textlen = 0;
	wnd->textalloc = 0;
    wnd->wlines = 0;
    wnd->textwidth = 0;
    wnd->wtop = wnd->wleft = 0;
    ClearTextBlock(wnd);
    ClearTextPointers(wnd);
}

/* ------------ KEYBOARD Message -------------- */
static int KeyboardMsg(WINDOW wnd, PARAM p1)
{
    switch ((int) p1)    {
        case UP:
            return SendMessage(wnd,SCROLL,FALSE,0);
        case DN:
            return SendMessage(wnd,SCROLL,TRUE,0);
        case FWD:
            return SendMessage(wnd,HORIZSCROLL,TRUE,0);
        case BS:
            return SendMessage(wnd,HORIZSCROLL,FALSE,0);
        case PGUP:
            return SendMessage(wnd,SCROLLPAGE,FALSE,0);
        case PGDN:
            return SendMessage(wnd,SCROLLPAGE,TRUE,0);
        case CTRL_PGUP:
            return SendMessage(wnd,HORIZPAGE,FALSE,0);
        case CTRL_PGDN:
            return SendMessage(wnd,HORIZPAGE,TRUE,0);
        case HOME:
            return SendMessage(wnd,SCROLLDOC,TRUE,0);
        case END:
            return SendMessage(wnd,SCROLLDOC,FALSE,0);
        default:
            break;
    }
    return FALSE;
}

/* ------------ LEFT_BUTTON Message -------------- */
static int LeftButtonMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int mx = (int) p1 - GetLeft(wnd);
    int my = (int) p2 - GetTop(wnd);
    if (TestAttribute(wnd, VSCROLLBAR) &&
                        mx == WindowWidth(wnd)-1)    {
        /* -------- in the right border ------- */
        if (my == 0 || my == ClientHeight(wnd)+1)
            /* --- above or below the scroll bar --- */
            return FALSE;
        if (my == 1)
            /* -------- top scroll button --------- */
            return SendMessage(wnd, SCROLL, FALSE, 0);
        if (my == ClientHeight(wnd))
            /* -------- bottom scroll button --------- */
            return SendMessage(wnd, SCROLL, TRUE, 0);
        /* ---------- in the scroll bar ----------- */
        if (!VSliding && my-1 == wnd->VScrollBox)    {
            RECT rc;
            VSliding = TRUE;
            rc.lf = rc.rt = GetRight(wnd);
            rc.tp = GetTop(wnd)+2;
            rc.bt = GetBottom(wnd)-2;
            return SendMessage(NULL, MOUSE_TRAVEL,
                (PARAM) &rc, 0);
        }
        if (my-1 < wnd->VScrollBox)
            return SendMessage(wnd,SCROLLPAGE,FALSE,0);
        if (my-1 > wnd->VScrollBox)
            return SendMessage(wnd,SCROLLPAGE,TRUE,0);
    }
    if (TestAttribute(wnd, HSCROLLBAR) &&
                        my == WindowHeight(wnd)-1) {
        /* -------- in the bottom border ------- */
        if (mx == 0 || my == ClientWidth(wnd)+1)
            /* ------  outside the scroll bar ---- */
            return FALSE;
        if (mx == 1)
            return SendMessage(wnd, HORIZSCROLL,FALSE,0);
        if (mx == WindowWidth(wnd)-2)
            return SendMessage(wnd, HORIZSCROLL,TRUE,0);
        if (!HSliding && mx-1 == wnd->HScrollBox)    {
            /* --- hit the scroll box --- */
            RECT rc;
            rc.lf = GetLeft(wnd)+2;
            rc.rt = GetRight(wnd)-2;
            rc.tp = rc.bt = GetBottom(wnd);
            /* - keep the mouse in the scroll bar - */
            SendMessage(NULL,MOUSE_TRAVEL,(PARAM)&rc,0);
            HSliding = TRUE;
            return TRUE;
        }
        if (mx-1 < wnd->HScrollBox)
            return SendMessage(wnd,HORIZPAGE,FALSE,0);
        if (mx-1 > wnd->HScrollBox)
            return SendMessage(wnd,HORIZPAGE,TRUE,0);
    }
    return FALSE;
}

/* ------------ MOUSE_MOVED Message -------------- */
static BOOL MouseMovedMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int mx = (int) p1 - GetLeft(wnd);
    int my = (int) p2 - GetTop(wnd);
    if (VSliding)    {
        /* ---- dragging the vertical scroll box --- */
        if (my-1 != wnd->VScrollBox)    {
            foreground = FrameForeground(wnd);
            background = FrameBackground(wnd);
            wputch(wnd, SCROLLBARCHAR, WindowWidth(wnd)-1,
                    wnd->VScrollBox+1);
            wnd->VScrollBox = my-1;
            wputch(wnd, SCROLLBOXCHAR, WindowWidth(wnd)-1,
                    my);
        }
        return TRUE;
    }
    if (HSliding)    {
        /* --- dragging the horizontal scroll box --- */
        if (mx-1 != wnd->HScrollBox)    {
            foreground = FrameForeground(wnd);
            background = FrameBackground(wnd);
            wputch(wnd, SCROLLBARCHAR, wnd->HScrollBox+1,
                    WindowHeight(wnd)-1);
            wnd->HScrollBox = mx-1;
            wputch(wnd, SCROLLBOXCHAR, mx, WindowHeight(wnd)-1);
        }
        return TRUE;
    }
    return FALSE;
}

/* ------------ BUTTON_RELEASED Message -------------- */
static void ButtonReleasedMsg(WINDOW wnd)
{
    if (HSliding || VSliding)    {
        /* release the mouse ouside the scroll bar */
        SendMessage(NULL, MOUSE_TRAVEL, 0, 0);
        VSliding ? ComputeWindowTop(wnd):ComputeWindowLeft(wnd);
        SendMessage(wnd, PAINT, 0, 0);
        SendMessage(wnd, KEYBOARD_CURSOR, 0, 0);
        VSliding = HSliding = FALSE;
    }
}

/* ------------ SCROLL Message -------------- */
static BOOL ScrollMsg(WINDOW wnd, PARAM p1)
{
    /* ---- vertical scroll one line ---- */
    if (p1)    {
        /* ----- scroll one line up ----- */
        if (wnd->wtop+ClientHeight(wnd) >= wnd->wlines)
            return FALSE;
        wnd->wtop++;
    }
    else    {
        /* ----- scroll one line down ----- */
        if (wnd->wtop == 0)
            return FALSE;
        --wnd->wtop;
    }
    if (isVisible(wnd))    {
        RECT rc;
        rc = ClipRectangle(wnd, ClientRect(wnd));
        if (ValidRect(rc))    {
            /* ---- scroll the window ----- */
            if (wnd != inFocus)
                SendMessage(wnd, PAINT, 0, 0);
            else    {
                scroll_window(wnd, rc, (int)p1);
                if (!(int)p1)
                    /* -- write top line (down) -- */
                    WriteTextLine(wnd,NULL,wnd->wtop,FALSE);
                else    {
                    /* -- write bottom line (up) -- */
                    int y=RectBottom(rc)-GetClientTop(wnd);
                    WriteTextLine(wnd, NULL,
                        wnd->wtop+y, FALSE);
                }
            }
        }
        /* ---- reset the scroll box ---- */
        if (TestAttribute(wnd, VSCROLLBAR))    {
            int vscrollbox = ComputeVScrollBox(wnd);
            if (vscrollbox != wnd->VScrollBox)
                MoveScrollBox(wnd, vscrollbox);
        }
    }
    return TRUE;
}

/* ------------ HORIZSCROLL Message -------------- */
static BOOL HorizScrollMsg(WINDOW wnd, PARAM p1)
{
    /* --- horizontal scroll one column --- */
    if (p1)    {
        /* --- scroll left --- */
        if (wnd->wleft + ClientWidth(wnd)-1 >= wnd->textwidth)
			return FALSE;
        wnd->wleft++;
    }
    else	{
        /* --- scroll right --- */
        if (wnd->wleft == 0)
			return FALSE;
        --wnd->wleft;
	}
    SendMessage(wnd, PAINT, 0, 0);
	return TRUE;
}

/* ------------  SCROLLPAGE Message -------------- */
static void ScrollPageMsg(WINDOW wnd, PARAM p1)
{
    /* --- vertical scroll one page --- */
    if ((int) p1 == FALSE)    {
        /* ---- page up ---- */
        if (wnd->wtop)
            wnd->wtop -= ClientHeight(wnd);
    }
    else     {
        /* ---- page down ---- */
        if (wnd->wtop+ClientHeight(wnd) < wnd->wlines) {
            wnd->wtop += ClientHeight(wnd);
            if (wnd->wtop>wnd->wlines-ClientHeight(wnd))
                wnd->wtop=wnd->wlines-ClientHeight(wnd);
        }
    }
    if (wnd->wtop < 0)
        wnd->wtop = 0;
    SendMessage(wnd, PAINT, 0, 0);
}

/* ------------ HORIZSCROLLPAGE Message -------------- */
static void HorizScrollPageMsg(WINDOW wnd, PARAM p1)
{
    /* --- horizontal scroll one page --- */
    if ((int) p1 == FALSE)
        /* ---- page left ----- */
        wnd->wleft -= ClientWidth(wnd);
    else    {
        /* ---- page right ----- */
        wnd->wleft += ClientWidth(wnd);
        if (wnd->wleft > wnd->textwidth-ClientWidth(wnd))
            wnd->wleft = wnd->textwidth-ClientWidth(wnd);
    }
    if (wnd->wleft < 0)
        wnd->wleft = 0;
    SendMessage(wnd, PAINT, 0, 0);
}

/* ------------ SCROLLDOC Message -------------- */
static void ScrollDocMsg(WINDOW wnd, PARAM p1)
{
    /* --- scroll to beginning or end of document --- */
    if ((int) p1)
        wnd->wtop = wnd->wleft = 0;
    else if (wnd->wtop+ClientHeight(wnd) < wnd->wlines){
        wnd->wtop = wnd->wlines-ClientHeight(wnd);
        wnd->wleft = 0;
    }
    SendMessage(wnd, PAINT, 0, 0);
}

/* ------------ PAINT Message -------------- */
static void PaintMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    /* ------ paint the client area ----- */
    RECT rc, rcc;
    int y;
    char blankline[201];

    /* ----- build the rectangle to paint ----- */
    if ((RECT *)p1 == NULL)
        rc=RelativeWindowRect(wnd, WindowRect(wnd));
    else
        rc= *(RECT *)p1;
    if (TestAttribute(wnd, HASBORDER) &&
            RectRight(rc) >= WindowWidth(wnd)-1) {
        if (RectLeft(rc) >= WindowWidth(wnd)-1)
            return;
        RectRight(rc) = WindowWidth(wnd)-2;
    }
    rcc = AdjustRectangle(wnd, rc);

	if (!p2 && wnd != inFocus)
		ClipString++;

    /* ----- blank line for padding ----- */
    memset(blankline, ' ', SCREENWIDTH);
    blankline[RectRight(rcc)+1] = '\0';

    /* ------- each line within rectangle ------ */
    for (y = RectTop(rc); y <= RectBottom(rc); y++){
        int yy;
        /* ---- test outside of Client area ---- */
        if (TestAttribute(wnd,
                    HASBORDER | HASTITLEBAR))    {
            if (y < TopBorderAdj(wnd))
                continue;
            if (y > WindowHeight(wnd)-2)
                continue;
        }
        yy = y-TopBorderAdj(wnd);
        if (yy < wnd->wlines - wnd->wtop)
            /* ---- paint a text line ---- */
            WriteTextLine(wnd, &rc,
                        yy+wnd->wtop, FALSE);
        else    {
            /* ---- paint a blank line ---- */
            SetStandardColor(wnd);
            writeline(wnd, blankline+RectLeft(rcc),
                    RectLeft(rcc)+BorderAdj(wnd), y, FALSE);
        }
    }
    /* ------- position the scroll box ------- */
    if (TestAttribute(wnd, VSCROLLBAR|HSCROLLBAR)) {
        int hscrollbox = ComputeHScrollBox(wnd);
        int vscrollbox = ComputeVScrollBox(wnd);
        if (hscrollbox != wnd->HScrollBox ||
                vscrollbox != wnd->VScrollBox)    {
            wnd->HScrollBox = hscrollbox;
            wnd->VScrollBox = vscrollbox;
            SendMessage(wnd, BORDER, p1, 0);
        }
    }
	if (!p2 && wnd != inFocus)
		--ClipString;
}

/* ------------ CLOSE_WINDOW Message -------------- */
static void CloseWindowMsg(WINDOW wnd)
{
    SendMessage(wnd, CLEARTEXT, 0, 0);
    if (wnd->TextPointers != NULL)    {
        free(wnd->TextPointers);
        wnd->TextPointers = NULL;
    }
}

/* ----------- TEXTBOX Message-processing Module ----------- */
int TextBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    switch (msg)    {
        case CREATE_WINDOW:
            wnd->HScrollBox = wnd->VScrollBox = 1;
            ClearTextPointers(wnd);
            break;
        case ADDTEXT:
            return AddTextMsg(wnd, (char *) p1, p2);
		case DELETETEXT:
            DeleteTextMsg(wnd, (int) p1);
            return TRUE;
		case INSERTTEXT:
            InsertTextMsg(wnd, (char *) p1, (int) p2);
            return TRUE;
        case SETTEXT:
            SetTextMsg(wnd, (char *) p1, p2);
			return TRUE;
        case CLEARTEXT:
            ClearTextMsg(wnd);
            break;
        case KEYBOARD:
            if (WindowMoving || WindowSizing)
                break;
            if (KeyboardMsg(wnd, p1))
                return TRUE;
            break;
        case LEFT_BUTTON:
            if (WindowSizing || WindowMoving)
                return FALSE;
            if (LeftButtonMsg(wnd, p1, p2))
                return TRUE;
            break;
        case MOUSE_MOVED:
            if (MouseMovedMsg(wnd, p1, p2))
                return TRUE;
            break;
        case BUTTON_RELEASED:
            ButtonReleasedMsg(wnd);
            break;
        case SCROLL:
            return ScrollMsg(wnd, p1);
        case HORIZSCROLL:
            return HorizScrollMsg(wnd, p1);
        case SCROLLPAGE:
            ScrollPageMsg(wnd, p1);
            return TRUE;
        case HORIZPAGE:
            HorizScrollPageMsg(wnd, p1);
            return TRUE;
        case SCROLLDOC:
            ScrollDocMsg(wnd, p1);
            return TRUE;
        case PAINT:
            if (isVisible(wnd))    {
                PaintMsg(wnd, p1, p2);
                return FALSE;
            }
            break;
        case CLOSE_WINDOW:
            CloseWindowMsg(wnd);
            break;
        default:
            break;
    }
    return BaseWndProc(TEXTBOX, wnd, msg, p1, p2);
}

/* ------ compute the vertical scroll box position from
                   the text pointers --------- */
static int ComputeVScrollBox(WINDOW wnd)
{
    int pagelen = wnd->wlines - ClientHeight(wnd);
    int barlen = ClientHeight(wnd)-2;
    int lines_tick;
    int vscrollbox;

    if (pagelen < 1 || barlen < 1)
        vscrollbox = 1;
    else    {
        if (pagelen > barlen)
            lines_tick = pagelen / barlen;
        else
            lines_tick = barlen / pagelen;
        vscrollbox = 1 + (wnd->wtop / lines_tick);
        if (vscrollbox > ClientHeight(wnd)-2 ||
                wnd->wtop + ClientHeight(wnd) >= wnd->wlines)
            vscrollbox = ClientHeight(wnd)-2;
    }
    return vscrollbox;
}

/* ---- compute top text line from scroll box position ---- */
static void ComputeWindowTop(WINDOW wnd)
{
    int pagelen = wnd->wlines - ClientHeight(wnd);
    if (wnd->VScrollBox == 0)
        wnd->wtop = 0;
    else if (wnd->VScrollBox == ClientHeight(wnd)-2)
        wnd->wtop = pagelen;
    else    {
        int barlen = ClientHeight(wnd)-2;
        int lines_tick;

        if (pagelen > barlen)
            lines_tick = barlen ? (pagelen / barlen) : 0;
        else
            lines_tick = pagelen ? (barlen / pagelen) : 0;
        wnd->wtop = (wnd->VScrollBox-1) * lines_tick;
        if (wnd->wtop + ClientHeight(wnd) > wnd->wlines)
            wnd->wtop = pagelen;
    }
    if (wnd->wtop < 0)
        wnd->wtop = 0;
}

/* ------ compute the horizontal scroll box position from
                   the text pointers --------- */
static int ComputeHScrollBox(WINDOW wnd)
{
    int pagewidth = wnd->textwidth - ClientWidth(wnd);
    int barlen = ClientWidth(wnd)-2;
    int chars_tick;
    int hscrollbox;

    if (pagewidth < 1 || barlen < 1)
        hscrollbox = 1;
    else     {
        if (pagewidth > barlen)
            chars_tick = barlen ? (pagewidth / barlen) : 0;
        else
            chars_tick = pagewidth ? (barlen / pagewidth) : 0;
        hscrollbox = 1 + (chars_tick ? (wnd->wleft / chars_tick) : 0);
        if (hscrollbox > ClientWidth(wnd)-2 ||
                wnd->wleft + ClientWidth(wnd) >= wnd->textwidth)
            hscrollbox = ClientWidth(wnd)-2;
    }
    return hscrollbox;
}

/* ---- compute left column from scroll box position ---- */
static void ComputeWindowLeft(WINDOW wnd)
{
    int pagewidth = wnd->textwidth - ClientWidth(wnd);

    if (wnd->HScrollBox == 0)
        wnd->wleft = 0;
    else if (wnd->HScrollBox == ClientWidth(wnd)-2)
        wnd->wleft = pagewidth;
    else    {
        int barlen = ClientWidth(wnd)-2;
        int chars_tick;

        if (pagewidth > barlen)
            chars_tick = pagewidth / barlen;
        else
            chars_tick = barlen / pagewidth;
        wnd->wleft = (wnd->HScrollBox-1) * chars_tick;
        if (wnd->wleft + ClientWidth(wnd) > wnd->textwidth)
            wnd->wleft = pagewidth;
    }
    if (wnd->wleft < 0)
        wnd->wleft = 0;
}

/* ----- get the text to a specified line ----- */
static ATTRCHR *GetTextLine(WINDOW wnd, int selection)
{
    ATTRCHR *line;
    int len = 0;
    ATTRCHR *cp, *cp1;
    cp = cp1 = TextLine(wnd, selection);
    while (cp->ch && cp->ch != '\n')    {
        len++;
        cp++;
    }
    line = DFmalloc((len+7) * sizeof(ATTRCHR));
    memmove(line, cp1, len * sizeof(ATTRCHR));
    line[len].ch = '\0';
    return line;
}

/* ------- write a line of text to a textbox window ------- */
void WriteTextLine(WINDOW wnd, RECT *rcc, int y, BOOL reverse)
{
    int len = 0;
//    int dif = 0;
    ATTRCHR line[4096];
    RECT rc;
    ATTRCHR *lp, *svlp;
    int lnlen;
    int i;

    /* ------ make sure y is inside the window ----- */
    if (y < wnd->wtop || y >= wnd->wtop+ClientHeight(wnd))
        return;

    /* ---- build the retangle within which can write ---- */
    if (rcc == NULL)    {
        rc = RelativeWindowRect(wnd, WindowRect(wnd));
        if (TestAttribute(wnd, HASBORDER) &&
                RectRight(rc) >= WindowWidth(wnd)-1)
            RectRight(rc) = WindowWidth(wnd)-2;
    }
    else
        rc = *rcc;

    /* ----- make sure rectangle is within window ------ */
    if (RectLeft(rc) >= WindowWidth(wnd)-1)
        return;
    if (RectRight(rc) == 0)
        return;
    rc = AdjustRectangle(wnd, rc);
    if (y-wnd->wtop<RectTop(rc) || y-wnd->wtop>RectBottom(rc))
        return;

    /* --- get the text and length of the text line --- */
    lp = svlp = GetTextLine(wnd, y);
    if (svlp == NULL)
        return;
    lnlen = tstrlen(lp);

	if (wnd->protect)	{
		ATTRCHR *pp = lp;
		while (pp->ch)	{
			if (isprint(pp->ch))
				pp->ch = '*';
			pp++;
		}
	}

    /* -------- insert block color change controls ------- */
    if (TextBlockMarked(wnd))    {
        int bbl = wnd->BlkBegLine;
        int bel = wnd->BlkEndLine;
        int bbc = wnd->BlkBegCol;
        int bec = wnd->BlkEndCol;
        int by = y;

        /* ----- put lowest marker first ----- */
        if (bbl > bel)    {
            swap(bbl, bel);
            swap(bbc, bec);
        }
        if (bbl == bel && bbc > bec)
            swap(bbc, bec);

        if (by >= bbl && by <= bel)    {
            /* ------ the block includes this line ----- */
            int blkbeg = 0;
            int blkend = lnlen;
			int attr = clr(SelectForeground(wnd),SelectBackground(wnd));
            if (!(by > bbl && by < bel))    {
                /* --- the entire line is not in the block -- */
                if (by == bbl)
                    /* ---- the block begins on this line --- */
                    blkbeg = bbc;
                if (by == bel)
                    /* ---- the block ends on this line ---- */
                    blkend = bec;
            }
			if (blkend == 0 && lnlen == 0)	{
				strcpy(lp, " ");
				blkend++;
			}
			/* Reset to select color */
			for (i=blkbeg; i < blkend; i++)
				lp[i].attrib = attr;
        }
    }
    /* ------ build the line to display -------- */
    if (lnlen < wnd->wleft)
        lnlen = 0;
    else
	{
		lnlen -= wnd->wleft;
        lp += wnd->wleft;
	}
	len = min(lnlen, RectWidth(rc));
	if (strchr(lp,'~'))
		len++;
    if (len > 0)
	{
		memcpy(line, lp, len * sizeof(ATTRCHR));
		line[len].ch = '\0';
	}
    /* -------- pad the line --------- */
    while (len < RectWidth(rc)/*_+dif*/)
	{
        line[len].ch = ' ';
		line[len++].attrib = clr(WndForeground(wnd), WndBackground(wnd));
	}
    line[len].ch = '\0';
	if (reverse)
	{
		ATTRCHR *pp = line;
		
		while (pp->ch)
		{
			int fg = pp->attrib & 0xf;
			if (fg == WndForeground(wnd))
				fg = SelectForeground(wnd);
			pp->attrib = clr(fg, SelectBackground(wnd));
			pp++;
		}
	}
//    dif = 0;
    /* ------- display the line -------- */
    twriteline(wnd, line/*+dif*/,
                RectLeft(rc)+BorderAdj(wnd),
                    y-wnd->wtop+TopBorderAdj(wnd), FALSE);
    free(svlp);
}

void MarkTextBlock(WINDOW wnd, int BegLine, int BegCol,
                               int EndLine, int EndCol)
{
    wnd->BlkBegLine = BegLine;
    wnd->BlkEndLine = EndLine;
    wnd->BlkBegCol = BegCol;
    wnd->BlkEndCol = EndCol;
}

/* ----- clear and initialize text line pointer array ----- */
void ClearTextPointers(WINDOW wnd)
{
    wnd->TextPointers = DFrealloc(wnd->TextPointers, sizeof(int));
    *(wnd->TextPointers) = 0;
}

#define INITLINES 100

/* ---- build array of pointers to text lines ---- */
void BuildTextPointers(WINDOW wnd)
{
    ATTRCHR *cp = wnd->text, *cp1;
    int i;
    unsigned int off;
    wnd->textwidth = 0;
	wnd->wlines = 0;
	while (cp->ch)
	{
		if (cp->ch == '\n')
			wnd->wlines++;
		cp++;
	}
    wnd->TextPointers = DFmalloc((wnd->wlines) * sizeof(int));
	
	cp = wnd->text;
	i = 0;
    while (cp->ch)    {
        off = (unsigned int) (cp - wnd->text);
        *((wnd->TextPointers) + i) = off;
        i++;
        cp1 = cp;
        while (cp->ch && cp->ch != '\n')
            cp++;
        wnd->textwidth = max(wnd->textwidth,
                        (unsigned int) (cp - cp1));
        if (cp->ch)
            cp++;
    }
}

static void MoveScrollBox(WINDOW wnd, int vscrollbox)
{
    foreground = FrameForeground(wnd);
    background = FrameBackground(wnd);
    wputch(wnd, SCROLLBARCHAR, WindowWidth(wnd)-1,
            wnd->VScrollBox+1);
    wputch(wnd, SCROLLBOXCHAR, WindowWidth(wnd)-1,
            vscrollbox+1);
    wnd->VScrollBox = vscrollbox;
}

int TextLineNumber(WINDOW wnd, ATTRCHR *lp)
{
    int lineno;
    char *cp;
    for (lineno = 0; lineno < wnd->wlines; lineno++)    {
        cp = wnd->text + *((wnd->TextPointers) + lineno);
        if (cp == lp)
            return lineno;
        if (cp > lp)
            break;
    }
    return lineno-1;
}


