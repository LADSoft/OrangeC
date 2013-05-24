/* ---------------- menubar.c ------------------ */

#include "dflat.h"

static void reset_menubar(WINDOW);

static struct {
    int x1, x2;     /* position in menu bar */
    char sc;        /* shortcut key value   */
} menu[10];
static int mctr;

MBAR *ActiveMenuBar;
static MENU *ActiveMenu;

static WINDOW mwnd;
static BOOL Selecting;

static WINDOW Cascaders[MAXCASCADES];
static int casc;
static WINDOW GetDocFocus(void);

/* ----------- SETFOCUS Message ----------- */
static int SetFocusMsg(WINDOW wnd, PARAM p1)
{
	int rtn;
	rtn = BaseWndProc(MENUBAR, wnd, SETFOCUS, p1, 0);
	if (!(int)p1)
		SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
	else
		SendMessage(NULL, HIDE_CURSOR, 0, 0);
	return rtn;
}

/* --------- BUILDMENU Message --------- */
static void BuildMenuMsg(WINDOW wnd, PARAM p1)
{
    int offset = 3;
    reset_menubar(wnd);
    mctr = 0;
    ActiveMenuBar = (MBAR *) p1;
    ActiveMenu = ActiveMenuBar->PullDown;
    while (ActiveMenu->Title != NULL &&
            ActiveMenu->Title != (void*)-1)    {
        char *cp;
        if (tstrlen(GetText(wnd)+offset) <
                strlen(ActiveMenu->Title)+3)
            break;
        GetText(wnd) = DFrealloc(GetText(wnd),
            (tstrlen(GetText(wnd))+5) * sizeof(ATTRCHR));
        memmove(GetText(wnd) + offset+4, GetText(wnd) + offset,
                (tstrlen(GetText(wnd)+offset)+1) * sizeof(ATTRCHR));
        CopyCommand(GetText(wnd)+offset,ActiveMenu->Title,FALSE,
                wnd->WindowColors [STD_COLOR] [FG], wnd->WindowColors [STD_COLOR] [BG]);
        menu[mctr].x1 = offset;
        offset += strlen(ActiveMenu->Title) + (MSPACE);
        menu[mctr].x2 = offset-MSPACE - 1;
        cp = strchr(ActiveMenu->Title, SHORTCUTCHAR);
        if (cp)
            menu[mctr].sc = tolower(*(cp+1));
        mctr++;
        ActiveMenu++;
    }
    ActiveMenu = ActiveMenuBar->PullDown;
}

/* ---------- PAINT Message ---------- */
static void PaintMsg(WINDOW wnd)
{
	if (Selecting)
		return;
    if (wnd == inFocus)
        SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
    twputs(wnd, GetText(wnd), 0, 0);
    if (ActiveMenuBar->ActiveSelection != -1 &&
            (wnd == inFocus || mwnd != NULL))    {
        ATTRCHR *sel, *cp;
        int offset, offset1;
		int i;
		int len;
		
        sel = DFmalloc(200 * sizeof(ATTRCHR));
        offset=menu[ActiveMenuBar->ActiveSelection].x1;
        offset1=menu[ActiveMenuBar->ActiveSelection].x2;
        GetText(wnd)[offset1].ch = '\0';
		for (i=0; i < 200; i++)
		{
			sel[i].ch = 0;
		}
		len = offset1 - offset;
		memcpy(sel, GetText(wnd) + offset, (len + 1) * sizeof(ATTRCHR));
		for (i=0; i < len + 1; i++)
		{
			sel[i].attrib = clr((sel[i].attrib & 0xf), SelectBackground(wnd));
		}
        twputs(wnd, sel, offset, 0);
        GetText(wnd)[offset1].ch = ' ';
        if (mwnd == NULL && wnd == inFocus) {
            char *st = ActiveMenu
                [ActiveMenuBar->ActiveSelection].StatusText;
            if (st != NULL)
                SendMessage(GetParent(wnd), ADDSTATUS,
                    (PARAM)st, 0);
        }
        free(sel);
    }
}

/* ------------ KEYBOARD Message ------------- */
static void KeyboardMsg(WINDOW wnd, PARAM p1)
{
    MENU *mnu;
	int sel;
//    if (mwnd == NULL)    {
        /* ----- search for menu bar shortcut keys ---- */
        int c = tolower((int)p1);
        int a = AltConvert((int)p1);
        int j;
        for (j = 0; j < mctr; j++)    {
            if ((inFocus == wnd && menu[j].sc == c) ||
                    (a && menu[j].sc == a))    {
				if (mwnd)
					SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
				SendMessage(wnd, SETFOCUS, TRUE, 0);
                SendMessage(wnd, MB_SELECTION, j, 0);
                return;
            }
        }
//    }
    /* -------- search for accelerator keys -------- */
    mnu = ActiveMenu;
    while (mnu->Title != (void *)-1)    {
        struct PopDown *pd = mnu->Selections;
        if (mnu->PrepMenu)
            (*(mnu->PrepMenu))(GetDocFocus(), mnu);
        while (pd->SelectionTitle != NULL)    {
            if (pd->Accelerator == (int) p1)    {
                if (pd->Attrib & INACTIVE)
                    beep();
                else    {
                    if (pd->Attrib & TOGGLE)
                        pd->Attrib ^= CHECKED;
					if (mwnd)
						SendMessage(mwnd, CLOSE_WINDOW, 0,0);
                    SendMessage(GetDocFocus(),
                        SETFOCUS, TRUE, 0);
                    PostMessage(GetParent(wnd),
                        COMMAND, pd->ActionId, 0);
                }
                return;
            }
            pd++;
        }
        mnu++;
    }
    switch ((int)p1)    {
        case F1:
            if (ActiveMenu == NULL || ActiveMenuBar == NULL)
				break;
			sel = ActiveMenuBar->ActiveSelection;
			if (sel == -1)	{
	    		BaseWndProc(MENUBAR, wnd, KEYBOARD, F1, 0);
				return;
			}
			mnu = ActiveMenu+sel;
			if (mwnd == NULL ||
					mnu->Selections[0].SelectionTitle == NULL) {
               	DisplayHelp(wnd,mnu->Title);
            	return;
			}
            break;
        case '\r':
            if (mwnd == NULL &&
                    ActiveMenuBar->ActiveSelection != -1)
                SendMessage(wnd, MB_SELECTION,
                    ActiveMenuBar->ActiveSelection, 0);
            break;
        case F10:
            if (wnd != inFocus && mwnd == NULL)    {
                SendMessage(wnd, SETFOCUS, TRUE, 0);
			    if ( ActiveMenuBar->ActiveSelection == -1)
			        ActiveMenuBar->ActiveSelection = 0;
			    SendMessage(wnd, PAINT, 0, 0);
                break;
            }
            /* ------- fall through ------- */
        case ESC:
            if (inFocus == wnd && mwnd == NULL)    {
                ActiveMenuBar->ActiveSelection = -1;
                SendMessage(GetDocFocus(),SETFOCUS,TRUE,0);
                SendMessage(wnd, PAINT, 0, 0);
            }
            break;
        case FWD:
            ActiveMenuBar->ActiveSelection++;
            if (ActiveMenuBar->ActiveSelection == mctr)
                ActiveMenuBar->ActiveSelection = 0;
            if (mwnd != NULL)
                SendMessage(wnd, MB_SELECTION,
                    ActiveMenuBar->ActiveSelection, 0);
            else 
                SendMessage(wnd, PAINT, 0, 0);
            break;
        case BS:
            if (ActiveMenuBar->ActiveSelection == 0 ||
					ActiveMenuBar->ActiveSelection == -1)
                ActiveMenuBar->ActiveSelection = mctr;
            --ActiveMenuBar->ActiveSelection;
            if (mwnd != NULL)
                SendMessage(wnd, MB_SELECTION,
                    ActiveMenuBar->ActiveSelection, 0);
            else 
                SendMessage(wnd, PAINT, 0, 0);
            break;
        default:
            break;
    }
}

/* --------------- LEFT_BUTTON Message ---------- */
static void LeftButtonMsg(WINDOW wnd, PARAM p1)
{
    int i;
    int mx = (int) p1 - GetLeft(wnd);
    /* --- compute the selection that the left button hit --- */
    for (i = 0; i < mctr; i++)
        if (mx >= menu[i].x1 &&
                mx <= menu[i].x2)
            break;
    if (i < mctr)
        if (i != ActiveMenuBar->ActiveSelection || mwnd == NULL)
            SendMessage(wnd, MB_SELECTION, i, 0);
}

/* -------------- MB_SELECTION Message -------------- */
static void SelectionMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int wd, mx, my;
    MENU *mnu;

	if (!p2)	{
	    ActiveMenuBar->ActiveSelection = -1;
    	SendMessage(wnd, PAINT, 0, 0);
	}
    Selecting = TRUE;
    mnu = ActiveMenu+(int)p1;
    if (mnu->PrepMenu != NULL)
        (*(mnu->PrepMenu))(GetDocFocus(), mnu);
    wd = MenuWidth(mnu->Selections);
    if (p2)    {
		int brd = GetRight(wnd);
        mx = GetLeft(mwnd) + WindowWidth(mwnd) - 1;
		if (mx + wd > brd)
			mx = brd - wd;
        my = GetTop(mwnd) + mwnd->selection;
    }
    else    {
        int offset = menu[(int)p1].x1 ;
        if (mwnd != NULL)
            SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
        ActiveMenuBar->ActiveSelection = (int) p1;
        if (offset > WindowWidth(wnd)-wd)
            offset = WindowWidth(wnd)-wd;
        mx = GetLeft(wnd)+offset;
        my = GetTop(wnd)+1;
    }
    mwnd = CreateWindow(POPDOWNMENU, NULL,
                mx, my,
                MenuHeight(mnu->Selections),
                wd,
                NULL,
                wnd,
                NULL,
                SHADOW);
	if (!p2)	{
	    Selecting = FALSE;
    	SendMessage(wnd, PAINT, 0, 0);
	    Selecting = TRUE;
	}
    if (mnu->Selections[0].SelectionTitle != NULL)    {
        SendMessage(mwnd, BUILD_SELECTIONS, (PARAM) mnu, 0);
		SendMessage(mwnd, SETFOCUS, TRUE, 0);
		SendMessage(mwnd, SHOW_WINDOW, 0, 0);
    }
    Selecting = FALSE;
}

/* --------- COMMAND Message ---------- */
static void CommandMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	if (p1 == ID_HELP)	{
    	BaseWndProc(MENUBAR, wnd, COMMAND, p1, p2);
		return;
	}
    if (isCascadedCommand(ActiveMenuBar, (int)p1))    {
        /* find the cascaded menu based on command id in p1 */
        MENU *mnu = ActiveMenu+mctr;
        while (mnu->Title != (void *)-1)    {
            if (mnu->CascadeId == (int) p1)    {
                if (casc < MAXCASCADES)    {
                    Cascaders[casc++] = mwnd;
                    SendMessage(wnd, MB_SELECTION,
                        (PARAM)(mnu-ActiveMenu), TRUE);
                }
                break;
            }
            mnu++;
        }
    }
    else     {
        if (mwnd != NULL)
            SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
        SendMessage(GetDocFocus(), SETFOCUS, TRUE, 0);
        PostMessage(GetParent(wnd), COMMAND, p1, p2);
    }
}

/* --------------- CLOSE_POPDOWN Message --------------- */
static void ClosePopdownMsg(WINDOW wnd)
{
    if (casc > 0)
        SendMessage(Cascaders[--casc], CLOSE_WINDOW, 0, 0);
    else     {
        mwnd = NULL;
        ActiveMenuBar->ActiveSelection = -1;
        if (!Selecting)	{
            SendMessage(GetDocFocus(), SETFOCUS, TRUE, 0);
	        SendMessage(wnd, PAINT, 0, 0);
		}
    }
}

/* ---------------- CLOSE_WINDOW Message --------------- */
static void CloseWindowMsg(WINDOW wnd)
{
    if (GetText(wnd) != NULL)    {
        free(GetText(wnd));
        GetText(wnd) = NULL;
    }
    mctr = 0;
    ActiveMenuBar->ActiveSelection = -1;
    ActiveMenu = NULL;
    ActiveMenuBar = NULL;
}

/* --- Window processing module for MENUBAR window class --- */
int MenuBarProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    int rtn;

    switch (msg)    {
        case CREATE_WINDOW:
            reset_menubar(wnd);
            break;
        case SETFOCUS:
			return SetFocusMsg(wnd, p1);
        case BUILDMENU:
            BuildMenuMsg(wnd, p1);
            break;
        case PAINT:    
            if (!isVisible(wnd) || GetText(wnd) == NULL)
                break;
            PaintMsg(wnd);
            return FALSE;
        case BORDER:
		    if (mwnd == NULL)
				SendMessage(wnd, PAINT, 0, 0);
            return TRUE;
        case KEYBOARD:
            KeyboardMsg(wnd, p1);
            return TRUE;
        case LEFT_BUTTON:
            LeftButtonMsg(wnd, p1);
            return TRUE;
        case MB_SELECTION:
            SelectionMsg(wnd, p1, p2);
            break;
        case COMMAND:
            CommandMsg(wnd, p1, p2);
            return TRUE;
        case INSIDE_WINDOW:
            return InsideRect(p1, p2, WindowRect(wnd));
        case CLOSE_POPDOWN:
            ClosePopdownMsg(wnd);
            return TRUE;
        case CLOSE_WINDOW:
            rtn = BaseWndProc(MENUBAR, wnd, msg, p1, p2);
            CloseWindowMsg(wnd);
            return rtn;
        default:
            break;
    }
    return BaseWndProc(MENUBAR, wnd, msg, p1, p2);
}

/* ------------- reset the MENUBAR -------------- */
static void reset_menubar(WINDOW wnd)
{
    ATTRCHR * cp = GetText(wnd) = DFrealloc(GetText(wnd), (SCREENWIDTH+5) * sizeof(ATTRCHR));
	int attr = clr(WndForeground(wnd), WndBackground(wnd));
	int i;
	for (i=0; i < SCREENWIDTH +5; i++)
	{
		cp->ch = ' ';
		cp->attrib = attr;
		cp++;
	}
    (GetText(wnd)+WindowWidth(wnd))->ch = '\0';
}

static WINDOW GetDocFocus(void)
{
	WINDOW wnd = ApplicationWindow;
	if (wnd != NULL)	{
		wnd = LastWindow(wnd);
		while (wnd != NULL && (GetClass(wnd) == MENUBAR ||
							GetClass(wnd) == TABBAR ||
							GetClass(wnd) == STATUSBAR))
			wnd = PrevWindow(wnd);
		if (wnd != NULL)
			while (wnd->childfocus != NULL)
				wnd = wnd->childfocus;
	}
	return wnd ? wnd : ApplicationWindow;
}

