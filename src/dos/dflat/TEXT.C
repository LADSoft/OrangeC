/* -------------- text.c -------------- */

#include "dflat.h"

int TextProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    int i, len;
    CTLWINDOW *ct = GetControl(wnd);
    char *cp, *cp2 = ct->itext;
    switch (msg)    {
		case SETFOCUS:
			return TRUE;
        case LEFT_BUTTON:
			return TRUE;
        case PAINT:
            if (ct == NULL ||
                ct->itext == NULL ||
                    GetText(wnd) != NULL)
                break;
            len = min(ct->dwnd.h, MsgHeight(cp2));
            cp = cp2;
            for (i = 0; i < len; i++)    {
                int mlen;
                ATTRCHR *txt ;
                char *cp1 = cp;
                char *np = strchr(cp, '\n');
                if (np != NULL)
                    *np = '\0';
                mlen = strlen(cp);
                while ((cp1=strchr(cp1,SHORTCUTCHAR)) != NULL) {
                    mlen --;
                    cp1++;
                }
                if (np != NULL)
                    *np = '\n';
                txt = DFmalloc((mlen+1) * sizeof(ATTRCHR));
                CopyCommand(txt, cp, FALSE, WndForeground(wnd), 
							WndBackground(wnd));
                txt[mlen].ch = '\0';
                SendMessage(wnd, ADDTEXT, (PARAM)txt, 1);
                if ((cp = strchr(cp, '\n')) != NULL)
                    cp++;
                free(txt);
            }
            break;
        default:
            break;
    }
    return BaseWndProc(TEXT, wnd, msg, p1, p2);
}
