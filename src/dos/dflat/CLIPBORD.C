/* ----------- clipbord.c ------------ */
#include "dflat.h"

ATTRCHR *Clipboard;
unsigned ClipboardLength;

void CopyTextToClipboard(ATTRCHR *text)
{
    ClipboardLength = tstrlen(text);
    Clipboard = DFrealloc(Clipboard, ClipboardLength * sizeof(ATTRCHR));
    memmove(Clipboard, text, ClipboardLength * sizeof(ATTRCHR));
}

void CopyToClipboard(WINDOW wnd)
{
    if (TextBlockMarked(wnd))    {
        ATTRCHR *bbl=TextLine(wnd,wnd->BlkBegLine)+wnd->BlkBegCol;
        ATTRCHR *bel=TextLine(wnd,wnd->BlkEndLine)+wnd->BlkEndCol;
        ClipboardLength = (int) (bel - bbl);
        Clipboard = DFrealloc(Clipboard, ClipboardLength * sizeof(ATTRCHR));
        memmove(Clipboard, bbl, ClipboardLength * sizeof(ATTRCHR));
    }
}

void ClearClipboard(void)
{
    if (Clipboard != NULL)  {
        free(Clipboard);
		ClipboardLength = 0;
        Clipboard = NULL;
    }
}


BOOL PasteText(WINDOW wnd, ATTRCHR *SaveTo, unsigned len)
{
    if (SaveTo != NULL && len > 0)    {
        unsigned plen = wnd->textlen + len;

		if (plen <= wnd->MaxTextLength)	{
        	if (plen+1 > wnd->textalloc)    {
				wnd->textalloc = plen + 3 + GROWLENGTH;
            	wnd->text = DFrealloc(wnd->text, wnd->textalloc * sizeof(ATTRCHR));
        	}
          	memmove(CurrChar+len, CurrChar, (wnd->textlen - (CurrChar - wnd->text) +1) * sizeof(ATTRCHR));
           	memmove(CurrChar, SaveTo, len * sizeof(ATTRCHR));
           	wnd->textlen = plen;
           	BuildTextPointers(wnd);
			ChangedText(wnd, TRUE);
			return TRUE;
		}
    }
	return FALSE;
}
