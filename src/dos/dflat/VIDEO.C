/* --------------------- video.c -------------------- */

#include "dflat.h"

#define movedata(srcx,src,dstx,dst,len) \
	dpmi_arbitrary_copy(dstx, dst, srcx, src, len)
	
BOOL ClipString;
static BOOL snowy;

static unsigned video_address;
static int near vpeek(short far *vp);
static void near vpoke(short far *vp, int c);
void movefromscreen(void *bf, int offset, int len);
void movetoscreen(void *bf, int offset, int len);

/* -- read a rectangle of video memory into a save buffer -- */
void getvideo(RECT rc, void *bf)
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));
    hide_mousecursor();
    while (ht--)    {
		movefromscreen(bf, vadr, bytes_row);
        vadr += SCREENWIDTH*2;
        bf = (char *)bf + bytes_row;
    }
    show_mousecursor();
}

/* -- write a rectangle of video memory from a save buffer -- */
void storevideo(RECT rc, void *bf)
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));
    hide_mousecursor();
    while (ht--)    {
		movetoscreen(bf, vadr, bytes_row);
        vadr += SCREENWIDTH*2;
        bf = (char *)bf + bytes_row;
    }
    show_mousecursor();
}

/* -------- read a character of video memory ------- */
unsigned int GetVideoChar(int x, int y)
{
    int c;
    hide_mousecursor();
	if (snowy)
	    c = vpeek(MK_FP(video_address, vad(x,y)));
	else
	    c = peek(video_address, vad(x,y));
    show_mousecursor();
    return c;
}

/* -------- write a character of video memory ------- */
void PutVideoChar(int x, int y, int c)
{
    if (x < SCREENWIDTH && y < SCREENHEIGHT)    {
        hide_mousecursor();
		if (snowy)
	        vpoke(MK_FP(video_address, vad(x,y)), c);
		else
	        poke(video_address, vad(x,y), c);
        show_mousecursor();
    }
}

BOOL CharInView(WINDOW wnd, int x, int y)
{
	WINDOW nwnd = NextWindow(wnd);
	WINDOW pwnd;
	RECT rc;
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;

	if (!TestAttribute(wnd, VISIBLE))
		return FALSE;
    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW wnd1 = GetParent(wnd);
        while (wnd1 != NULL)    {
            /* --- clip character to parent's borders -- */
			if (!TestAttribute(wnd1, VISIBLE))
				return FALSE;
			if (!InsideRect(x1, y1, ClientRect(wnd1)))
                return FALSE;
            wnd1 = GetParent(wnd1);
        }
    }
	while (nwnd != NULL)	{
		if (!isHidden(nwnd) /* && !isAncestor(wnd, nwnd) */ )	{
			rc = WindowRect(nwnd);
    		if (TestAttribute(nwnd, SHADOW))    {
        		RectBottom(rc)++;
        		RectRight(rc)++;
    		}
			if (!TestAttribute(nwnd, NOCLIP))	{
				pwnd = nwnd;
				while (GetParent(pwnd))	{
					pwnd = GetParent(pwnd);
					rc = subRectangle(rc, ClientRect(pwnd));
				}
			}
			if (InsideRect(x1,y1,rc))
				return FALSE;
		}
		nwnd = NextWindow(nwnd);
	}
    return (x1 < SCREENWIDTH && y1 < SCREENHEIGHT);
}

/* -------- write a character to a window ------- */
void wputch(WINDOW wnd, int c, int x, int y)
{
	if (CharInView(wnd, x, y))	{
		int ch = (c & 255) | (clr(foreground, background) << 8);
		int xc = GetLeft(wnd)+x;
		int yc = GetTop(wnd)+y;
        hide_mousecursor();
		if (snowy)
        	vpoke(MK_FP(video_address, vad(xc, yc)), ch);
		else
        	poke(video_address, vad(xc, yc), ch);
        show_mousecursor();
	}
}

/* ------- write a string to a window ---------- */
void wputs(WINDOW wnd, void *s, int x, int y)
{
	BOOL changecolor = FALSE;
	int x1 = GetLeft(wnd)+x;
	int x2 = x1;
	int y1 = GetTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT && isVisible(wnd))	{
		short ln[200];
		short *cp1 = ln;
	    unsigned char *str = s;
	    int fg = foreground;
    	int bg = background;
	    int len;
		int off = 0;
        while (*str)    {
            if (*str == CHANGECOLOR)    {
				if (changecolor)
				{
					str += 3;
				}
				else
				{
	                str++;
    	            foreground = (*str++) & 0x7f;
        	        background = (*str++) & 0x7f;
				}
				changecolor++;
           	    continue;
            }
            if (*str == RESETCOLOR)    {
				if (changecolor)
					changecolor--;
				if (!changecolor)
				{
	                foreground = fg & 0x7f;
    	            background = bg & 0x7f;
				}
       	        str++;
                continue;
            }
			if (*str == ('\t' | 0x80) || *str == ('\f' | 0x80))
	   	        *cp1 = ' ' | (clr(foreground, background) << 8);
			else 
	   	        *cp1 = (*str & 255) | (clr(foreground, background) << 8);
			if (ClipString)
				if (!CharInView(wnd, x, y))
					*cp1 = peek(video_address, vad(x2,y1));
			cp1++;
			str++;
			x++;
			x2++;
        }
        foreground = fg;
        background = bg;
   		len = (int)(cp1-ln);
   		if (x1+len > SCREENWIDTH)
       		len = SCREENWIDTH-x1;

		if (!ClipString && !TestAttribute(wnd, NOCLIP))	{
			/* -- clip the line to within ancestor windows -- */
			RECT rc = WindowRect(wnd);
			WINDOW nwnd = GetParent(wnd);
			while (len > 0 && nwnd != NULL)	{
				if (!isVisible(nwnd))	{
					len = 0;
					break;
				}
				rc = subRectangle(rc, ClientRect(nwnd));
				nwnd = GetParent(nwnd);
			}
			while (len > 0 && !InsideRect(x1+off,y1,rc))	{
				off++;
				--len;
			}
			if (len > 0)	{
				x2 = x1+len-1;
				while (len && !InsideRect(x2,y1,rc))	{
					--x2;
					--len;
				}
			}
		}
		if (len > 0)	{
        	hide_mousecursor();
			movetoscreen(ln+off, vad(x1+off,y1), len*2);
        	show_mousecursor();
		}
    }
}
/* ------- write a string to a window ---------- */
void twputs(WINDOW wnd, void *s, int x, int y)
{
	BOOL changecolor = FALSE;
	int x1 = GetLeft(wnd)+x;
	int x2 = x1;
	int y1 = GetTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT && isVisible(wnd))	{
		short ln[200];
		short *cp1 = ln;
	    ATTRCHR *str = s;
	    int len;
		int off = 0;
        while (str->ch)    {
            if (str->ch == CHANGECOLOR)    {
				if (changecolor)
				{
					str += 3;
				}
				else
				{
	                str++;
    	            foreground = (str++->ch) & 0x7f;
        	        background = (str++->ch) & 0x7f;
				}
				changecolor++;
           	    continue;
            }
            if (str->ch == RESETCOLOR)    {
				if (changecolor)
					changecolor--;
       	        str++;
                continue;
            }
			if (str->ch == ('\t' | 0x80) || str->ch == ('\f' | 0x80))
				if (changecolor)
					*cp1 = ' ' | (clr(foreground, background) << 8);
				else
		   	        *cp1 = ' ' | (str->attrib << 8);
			else 
				if (changecolor)
		   	        *cp1 = (str->ch & 255) | (clr(foreground, background) << 8);
				else
		   	        *cp1 = (str->ch & 255) | (str->attrib << 8);
			if (ClipString)
				if (!CharInView(wnd, x, y))
					*cp1 = peek(video_address, vad(x2,y1));
			cp1++;
			str++;
			x++;
			x2++;
        }
   		len = (int)(cp1-ln);
   		if (x1+len > SCREENWIDTH)
       		len = SCREENWIDTH-x1;

		if (!ClipString && !TestAttribute(wnd, NOCLIP))	{
			/* -- clip the line to within ancestor windows -- */
			RECT rc = WindowRect(wnd);
			WINDOW nwnd = GetParent(wnd);
			while (len > 0 && nwnd != NULL)	{
				if (!isVisible(nwnd))	{
					len = 0;
					break;
				}
				rc = subRectangle(rc, ClientRect(nwnd));
				nwnd = GetParent(nwnd);
			}
			while (len > 0 && !InsideRect(x1+off,y1,rc))	{
				off++;
				--len;
			}
			if (len > 0)	{
				x2 = x1+len-1;
				while (len && !InsideRect(x2,y1,rc))	{
					--x2;
					--len;
				}
			}
		}
		if (len > 0)	{
        	hide_mousecursor();
			movetoscreen(ln+off, vad(x1+off,y1), len*2);
        	show_mousecursor();
		}
    }
}

/* --------- get the current video mode -------- */
void get_videomode(void)
{
    videomode();
    /* ---- Monochrome Display Adaptor or text mode ---- */
	snowy = FALSE;
    if (ismono())
        video_address = __segB000;
    else	{
        /* ------ Text mode -------- */
        video_address = __segB800;
		if (!isEGA() && !isVGA())
			/* -------- CGA --------- */
			snowy = cfg.snowy;
	}
}

/* --------- scroll the window. d: 1 = up, 0 = dn ---------- */
void scroll_window(WINDOW wnd, RECT rc, int d)
{
	if (RectTop(rc) != RectBottom(rc))	{
		union REGS regs;
		regs.h.cl = RectLeft(rc);
		regs.h.ch = RectTop(rc);
		regs.h.dl = RectRight(rc);
		regs.h.dh = RectBottom(rc);
		regs.h.bh = clr(WndForeground(wnd),WndBackground(wnd));
		regs.h.ah = 7 - d;
		regs.h.al = 1;
    	hide_mousecursor();
    	_int386(VIDEO, &regs, &regs);
    	show_mousecursor();
	}
}


static void near waitforretrace(void)
{
#ifndef WATCOM
asm		mov		dx,0x3da
loop1:
asm		mov		cx,6
loop2:
asm		in		al,dx
asm		test	al,8
asm		jnz		loop2
asm		test	al,1
asm		jz		loop2
asm		cli
loop3:
asm		in		al,dx
asm		test	al,1
asm		loopnz	loop3
asm		sti
asm		jz		loop1
#endif
}

void movetoscreen(void *bf, int offset, int len)
{
	if (snowy)
		waitforretrace();
	movedata(_DS, (int)bf, video_address, offset + video_page * 16, len);
}

void movefromscreen(void *bf, int offset, int len)
{
	if (snowy)
		waitforretrace();
	movedata(video_address, offset + video_page * 16,	_DS, (int)bf,	len);
}


static int near vpeek(short far *vp)
{
	int c;
	waitforretrace();
	c = *vp;
	return c;
}

static void near vpoke(short far *vp, int c)
{
	waitforretrace();
	*vp = c;
}

