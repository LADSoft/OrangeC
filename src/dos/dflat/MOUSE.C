/* ------------- mouse.c ------------- */

#include "dflat.h"

DPMI_REGS regs ;
static struct SREGS sregs;

static void near mouse(int m1,int m2,int m3,int m4)
{
//	segread(&sregs);
	regs.h.flags = 0x72;
	regs.h.ss = 0;
	regs.h.sp = 0;
    regs.h.ds = 0; //es;
    regs.h.es = 0; //es;
    regs.h.dx = m4;
    regs.h.cx = m3;
    regs.h.bx = m2;
    regs.h.ax = m1;
    dpmi_simulate_real_interrupt(MOUSE,&regs);
}

/* ---------- reset the mouse ---------- */
void resetmouse(void)
{
    mouse(0,0,0,0);
}

/* ----- test to see if the mouse driver is installed ----- */
BOOL mouse_installed(void)
{
    unsigned char far *ms;
	unsigned short aa, bb;
	aa = peek(__seg0000, MOUSE *4 + 2);
	bb = peek(__seg0000, MOUSE * 4);
	asm {
		mov ax,2
		mov bx,[aa]
		int 0x31
		mov [aa],ax
		jnc ok
	}
	return FALSE;
ok:
	ms = MK_FP(aa, bb);
    return (SCREENWIDTH <= 80 && ms != NULL && *ms != 0xcf);
}

/* ------ return true if mouse buttons are pressed ------- */
int mousebuttons(void)
{
    if (mouse_installed())	{
        mouse(3,0,0,0);
	    return regs.h.bx & 3;
	}
	return 0;
}

/* ---------- return mouse coordinates ---------- */
void get_mouseposition(int *x, int *y)
{
	*x = *y = -1;
    if (mouse_installed())    {
        mouse(3,0,0,0);
        *x = regs.h.cx/8;
        *y = regs.h.dx/8;
		if (SCREENWIDTH == 40)
			*x /= 2;
    }
}

/* -------- position the mouse cursor -------- */
void set_mouseposition(int x, int y)
{
    if (mouse_installed())	{
		if (SCREENWIDTH == 40)
			x *= 2;
        mouse(4,0,x*8,y*8);
	}
}

/* --------- display the mouse cursor -------- */
void show_mousecursor(void)
{
    if (mouse_installed())	{
        mouse(1,0,0,0);
	}
}

/* --------- hide the mouse cursor ------- */
void hide_mousecursor(void)
{
    if (mouse_installed())	{
        mouse(2,0,0,0);
	}
}

/* --- return true if a mouse button has been released --- */
int button_releases(void)
{
    if (mouse_installed())	{
        mouse(6,0,0,0);
	    return regs.h.bx;
	}
	return 0;
}

/* ----- set mouse travel limits ------- */
void set_mousetravel(int minx, int maxx, int miny, int maxy)
{
    if (mouse_installed())	{
		if (SCREENWIDTH == 40)	{
			minx *= 2;
			maxx *= 2;
		}
        mouse(7, 0, minx*8, maxx*8);
		mouse(8, 0, miny*8, maxy*8);
	}
}

