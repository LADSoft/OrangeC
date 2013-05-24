/* ---------------- menubar.c ------------------ */

#include "dflat.h"

#define TAB_PREV_CHAR 0xae
#define TAB_NEXT_CHAR 0xaf

static WINDOW twnd;

typedef struct {
	struct _tls
	{
		struct _tls *next;
		char *name;
		WINDOW wnd;
	} *tabData;
	int selected;
	int shown;
} TABDATA ;
/* ----------- SETFOCUS Message ----------- */
static int SetFocusMsg(WINDOW wnd, PARAM p1)
{
	int rtn;
	rtn = BaseWndProc(TABBAR, wnd, SETFOCUS, p1, 0);
	return rtn;
}

/* ---------- ADD_TAB Message -------- */
static int AddTabMsg(WINDOW wnd, char *name, WINDOW link)
{
	TABDATA *td = (TABDATA *)wnd->extension;
	struct _tls **d = &td->tabData;
	int current = 0;
	while (*d)
	{
		if (!stricmp(name , (*d)->name))
			return FALSE;
		d = &(*d)->next;
		current++;
	}
	*d = DFmalloc(sizeof(struct _tls));
	(*d)->next = NULL;
	(*d)->name = DFmalloc(strlen(name)+ 1);
	strcpy((*d)->name, name);
	(*d)->wnd = link;
	td->selected = current;
	SendMessage(wnd, PAINT, 0, 0);
	return TRUE;
}
/* ---------- REMOVE_TAB Message ----- */
static int RemoveTabMsg(WINDOW wnd, char *name, WINDOW link)
{

	struct _tls **d = &((TABDATA *)wnd->extension)->tabData;
	while (*d)
	{
		if (link == (*d)->wnd)
		{
			struct _tls *m = *d;
			*d = (*d)->next;
			free(m->name);
			free(m);
			SendMessage(wnd, PAINT, 0, 0);
			return TRUE;
		}
		d = &(*d)->next;
	}
	return FALSE;
}
/* ---------- TAB_SELECT Message ----- */
static int SelectTabMsg(WINDOW wnd, WINDOW link)
{
	TABDATA *td = (TABDATA *)wnd->extension;
	struct _tls *d = td->tabData;
	int current = 0;
	while (d)
	{
		if (d->wnd == link)
		{
			td->selected = current;
			SendMessage(wnd, PAINT, 0, 0);
			return TRUE;
		}
		current++;
		d = d->next;
	}
	return FALSE;
}
/* ---------- PAINT Message ---------- */
static void PaintMsg(WINDOW wnd)
{
	TABDATA *td = (TABDATA *)wnd->extension;
	ATTRCHR buf[200];
	int attr = clr(WndForeground(wnd), WndBackground(wnd));
	BOOL done = FALSE;
	while (!done)
	{
		struct _tls *d = td->tabData;
		int n = td->shown, i;
		int current = 0;
		if (td->selected < td->shown)
		{
			td->shown = 0;
			continue;
		}
		for (i=0; i < ClientWidth(wnd); i++)
			buf[i].ch = ' ', buf[i].attrib = attr;
		buf[ClientWidth(wnd)].ch = '\0';
		while (n && d)
			d = d->next, n--, current++;
		n = 0;
		while (d)
		{
			char *p = d->name;
			p = strrchr(p, '\\');
			if (!p)
				p = d->name;
			else
				p++;
			if (((strlen(p) + 1 + n) >= (ClientWidth(wnd)-2)))
				break;
				
			if (n != 0)
				buf[n++].ch = '|';
			while (*p)
			{
				if (current == td->selected)
				{
					buf[n].attrib = clr(SelectForeground(wnd), SelectBackground(wnd));
				}
				buf[n++].ch = *p++;
			}
			current++;
			d = d->next;
		}
		if (current <= td->selected && d)
		{
			td->shown++;
			done = FALSE;
			continue;
		}
		if (td->shown)
			buf[ClientWidth(wnd)-2].ch = TAB_PREV_CHAR;
		if (d)
			buf[ClientWidth(wnd)-1].ch = TAB_NEXT_CHAR;
		done = TRUE;
	}
	twputs(wnd, buf, 0, 0);
}

/* ------------ KEYBOARD Message ------------- */
static void KeyboardMsg(WINDOW wnd, PARAM p1)
{
	TABDATA *td = (TABDATA *)wnd->extension;
	struct _tls *d;
	int n = td->selected;
    switch ((int)p1)    {
        case F1:
            break;
        case '\r':
            break;
        case FWD:
			d = td->tabData;
			while (n)
				d = d->next, n--;
			if (d->next)
			{
				d = d->next;
				td->selected++;
				SendMessage(wnd, PAINT, 0, 0);
	            SendMessage(d->wnd, SETFOCUS, TRUE, 0);
			}
            break;
        case BS:
			if (td->selected)
			{
				td->selected--;
				n = td->selected;
				d = td->tabData;
				while (n && d)
					d = d->next, n--;
				SendMessage(wnd, PAINT, 0, 0);
	            SendMessage(d->wnd, SETFOCUS, TRUE, 0);
			}
            break;
        default:
            break;
    }
}

/* --------------- LEFT_BUTTON Message ---------- */
static void LeftButtonMsg(WINDOW wnd, PARAM p1)
{
	int mx = p1 - GetLeft(wnd);
	if (mx == ClientWidth(wnd)-2)
		KeyboardMsg(wnd, BS);
	else if (mx == ClientWidth(wnd)-1)
		KeyboardMsg(wnd, FWD);
	else
	{
		TABDATA *td = (TABDATA *)wnd->extension;
		struct _tls *d = td->tabData;
		int n = td->shown;
		int current = 0;
		while (n && d)
			d = d->next, n--, current++;
		if (d)
		{
			n = 0;
			while (TRUE)
			{
				char *p = d->name;
				p = strrchr(p, '\\');
				if (p)
					p++;
				else
					p = d->name;
				if (mx <= n + strlen(p))
					break;
				n += strlen(p) + 1;
				if (!d->next)
					break;
				d = d->next;
				current++;
			}
            SendMessage(d->wnd, SETFOCUS, TRUE, 0);
			td->selected = current;
			SendMessage(wnd, PAINT, 0, 0);
		}
	}
}



/* ---------------- CLOSE_WINDOW Message --------------- */
static void CloseWindowMsg(WINDOW wnd)
{

	struct _tls *d = ((TABDATA *)wnd->extension)->tabData;
	while (d)
	{
		struct _tls *q = d->next;
		free(d->name);
		free(d);
		d = q;
	}
	free(wnd->extension);
	wnd->extension = NULL;
}

/* --- Window processing module for MENUBAR window class --- */
int TabBarProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    int rtn;

    switch (msg)    {
        case CREATE_WINDOW:
			wnd->extension = DFcalloc(sizeof(TABDATA), 1);
            break;
        case SETFOCUS:
			return SetFocusMsg(wnd, p1);
		case TAB_ADD:
			return AddTabMsg(wnd, (char *)p1, (WINDOW)p2);
		case TAB_REMOVE:
			return RemoveTabMsg(wnd, (char *)p1, (WINDOW)p2);
		case TAB_SELECT:
			return SelectTabMsg(wnd, (WINDOW)p2);
        case PAINT:    
            if (!isVisible(wnd))
                break;
            PaintMsg(wnd);
            return FALSE;
        case KEYBOARD:
            KeyboardMsg(wnd, p1);
            return TRUE;
        case LEFT_BUTTON:
            LeftButtonMsg(wnd, p1);
            return TRUE;
        case INSIDE_WINDOW:
            return InsideRect(p1, p2, WindowRect(wnd));
        case CLOSE_WINDOW:
            rtn = BaseWndProc(MENUBAR, wnd, msg, p1, p2);
            CloseWindowMsg(wnd);
            return rtn;
        default:
            break;
    }
    return BaseWndProc(MENUBAR, wnd, msg, p1, p2);
}
