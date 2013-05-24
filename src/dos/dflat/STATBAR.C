/* ---------------- statbar.c -------------- */

#include "dflat.h"

extern char time_string[];

void PutStatusLine(WINDOW wnd, void *s, int x, int y)
{
	int saved = FALSE, sv;
	if (x < ClientWidth(wnd) && y < ClientHeight(wnd))	{
		ATTRCHR *en = (ATTRCHR *)s+ClientWidth(wnd)-x;
		if (tstrlen(s)+x > ClientWidth(wnd))	{
			sv = en->ch;
			en->ch = '\0';
			saved = TRUE;
		}
		ClipString++;
		twputs(wnd, s, x+BorderAdj(wnd), y+TopBorderAdj(wnd));
		--ClipString;
		if (saved)
			en->ch = sv;
	}
}
int StatusBarProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	ATTRCHR *statusbar;
	int i;
	int attr;
	switch (msg)	{
		case CREATE_WINDOW:
			SendMessage(wnd, CAPTURE_CLOCK, 0, 0);
			break;
		case KEYBOARD:
			if ((int)p1 == CTRL_F4)
				return TRUE;
			break;
		case PAINT:	
			if (!isVisible(wnd))
				break;
			statusbar = DFcalloc(1, (WindowWidth(wnd)+1) * sizeof(ATTRCHR));
			attr = clr(WndForeground(wnd), WndBackground(wnd));
			for (i=0; i < WindowWidth(wnd); i++)
			{
				statusbar[i].ch = ' ';
				statusbar[i].attrib = attr;
			}
			statusbar[WindowWidth(wnd)].ch = '\0';
			for (i=0; i < 7; i++)
				statusbar[i+1].ch = "F1=Help"[i];
			if (wnd->text)	{
				int len = min(tstrlen(wnd->text), WindowWidth(wnd)-17);
				if (len > 0)	{
					int off=(WindowWidth(wnd)-len)/2;
					for (i= off; i < off + len; i++)
					{
						if (wnd->text[i-off].ch >= ' ')
						{
							statusbar[i].ch = wnd->text[i-off].ch;
							statusbar[i].attrib = attr;
						}
					}
				}
			}
			if (wnd->TimePosted)
				statusbar[WindowWidth(wnd)-8].ch = '\0';
			else
			{
				for (i=0; i < 9; i++)
				{
					statusbar[WindowWidth(wnd) - 8 + i].ch = time_string[i];
				}
			}
    	    PutStatusLine(wnd, statusbar, 0, 0);
			free(statusbar);
			return TRUE;
		case BORDER:
			return TRUE;
		case CLOCKTICK:
			SetStandardColor(wnd);
			PutWindowLine(wnd, (char *)p1, WindowWidth(wnd)-8, 0);
			wnd->TimePosted = TRUE;
			SendMessage(wnd->PrevClock, msg, p1, p2);
			return TRUE;
		case CLOSE_WINDOW:
			SendMessage(wnd, RELEASE_CLOCK, 0, 0);
			break;
		default:
			break;
	}
	return BaseWndProc(STATUSBAR, wnd, msg, p1, p2);
}

