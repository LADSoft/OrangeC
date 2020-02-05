/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdio.h>
#include <stdarg.h>
#include <dpmi.h>

#include "conio.h"

extern int _breakflag ;

static int screenbase;
int chrheight,baseport,ulline;
int ungetchar;

SELECTOR realdata;
static struct text_info the_info = { 0,0,80,25,7,7,0,25,80,0,0 };

#pragma startup conio_init 225

#pragma rundown conio_rundown 31

void textini(int mode);

static void conio_init(void)
{
    unsigned char mode;
    unsigned short curs;
    unsigned char atr;
    int pos;
    SELECTOR sel;
    asm mov [sel],ds
    dpmi_get_alias_descriptor(&realdata, sel);
    dpmi_set_sel_base(realdata,0x0);
    dpmi_set_sel_limit(realdata,0xF0000);
    asm push fs
    asm mov fs,[realdata]
    asm mov al,fs:[0x449]
    asm mov [mode],al
    asm mov ax,fs:[0x450]
    asm mov [curs],ax
    asm pop fs
    textini(mode);
    the_info.curx = curs & 0xff;
    the_info.cury = curs >> 8;
   pos = screenbase + the_info.screenwidth * the_info.screenheight * 2 - 1;
    asm push fs;
    asm mov fs,[realdata];
    asm mov ecx,[pos];
    asm mov al,fs:[ecx]
    asm mov [atr],al
  asm pop fs
    the_info.normattr = the_info.attribute = atr;
        
    
}
static void conio_rundown(void)
{

  DPMI_REGS regs;
#ifdef XXXXX
    the_info.attribute = the_info.normattr;
    regs.b.ah = 0x2;
    regs.b.bh = 0;
  regs.b.dh = the_info.cury+the_info.wintop;
  regs.b.dl = the_info.curx+the_info.winleft;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    dpmi_simulate_real_interrupt(0x10,&regs);
#endif
    dpmi_free_selector(realdata);
}
int _RTL_FUNC           getch( void )
{
    int rv;
    if (ungetchar) {
        rv = ungetchar;
        ungetchar = 0;
    }
    else {
  DPMI_REGS regs;
#ifdef XXXXX
      regs.b.ah = 0x2;
      regs.b.bh = 0;
    regs.b.dh = the_info.cury+the_info.wintop;
    regs.b.dl = the_info.curx+the_info.winleft;
      regs.h.flags = 0x72;
      regs.h.ss = 0;
      regs.h.sp = 0;
      dpmi_simulate_real_interrupt(0x10,&regs);
#endif
      regs.b.ah = 0x0;
        regs.h.flags = 0x72;
        regs.h.ss = 0;
        regs.h.sp = 0;
        dpmi_simulate_real_interrupt(0x16,&regs);
        rv = regs.h.ax;
        if (rv & 0xff)
            rv &= 0xff;
        else {
            ungetchar = rv >> 8 ;
            rv = 0 ;
        }
        
    }
   if (_breakflag)
      raisecb() ;
    return rv;
}
int _RTL_FUNC          kbhit( void )
{
    DPMI_REGS regs;
#ifdef XXXXX
    regs.b.ah = 0x2;
    regs.b.bh = 0;
  regs.b.dh = the_info.cury+the_info.wintop;
  regs.b.dl = the_info.curx+the_info.winleft;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    dpmi_simulate_real_interrupt(0x10,&regs);
#endif
    regs.b.ah = 0x1;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    dpmi_simulate_real_interrupt(0x16,&regs);
   if (_breakflag)
      raisecb() ;
    return !(regs.h.flags & 0x40);
}
static void biosmode(int mode )
{
    DPMI_REGS regs;
    int newmode = mode;
    if (newmode == C4350)
        newmode = 3;
    regs.b.ah = 0x0;
    regs.b.al = newmode;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    dpmi_simulate_real_interrupt(0x10,&regs);
    if (mode == C4350) {
        regs.h.ax = 0x1112;
        regs.b.bl = 0;
        dpmi_simulate_real_interrupt(0x10,&regs);
        regs.h.ax = 0x1103;
        regs.b.bl = 0;
        dpmi_simulate_real_interrupt(0x10,&regs);
    }
}
unsigned char _RTL_FUNC    inportb( unsigned __portid )
{
    asm  mov edx,[__portid];
    asm  in al,dx;
    asm  movzx eax,al;
    return _EAX;
}
unsigned _RTL_FUNC         inport ( unsigned __portid )
{
    asm  mov edx,[__portid];
    asm  in ax,dx;
    asm  movzx eax,ax;
    return _EAX;
}
unsigned _RTL_FUNC         inportd ( unsigned __portid )
{
    asm  mov edx,[__portid];
    asm  in eax,dx;
    return _EAX;
}
int _RTL_FUNC              inp( unsigned __portid )
{
    asm  mov edx,[__portid];
    asm  in ax,dx;
    asm  movsx eax,ax;
    return _EAX;
}
unsigned _RTL_FUNC         inpw( unsigned __portid )
{
    asm  mov edx,[__portid];
    asm  in ax,dx;
    asm  movzx eax,ax;
    return _EAX;
}
void _RTL_FUNC             outportb( unsigned __portid, unsigned char __value )
{
    asm mov edx,[__portid];
    asm mov al,[__value];
    asm out dx,al;
}
void _RTL_FUNC             outport ( unsigned __portid, unsigned __value )
{
    asm mov edx,[__portid];
    asm mov ax,word ptr [__value];
    asm out dx,ax;
}
void _RTL_FUNC             outportd ( unsigned __portid, unsigned __value )
{
    asm mov edx,[__portid];
    asm mov eax,dword ptr [__value];
    asm out dx,eax;
}
int _RTL_FUNC              outp( unsigned __portid, int __value )
{
    asm mov edx,[__portid];
    asm mov al,byte ptr [__value];
    asm out dx,al;
    return __value;
}
unsigned _RTL_FUNC         outpw( unsigned __portid, unsigned __value )
{
    asm mov edx,[__portid];
    asm mov ax,word ptr [__value];
    asm out dx,ax;
    return __value;
}
static void putthechar(int x, int y, int chr)
{
    int pos = (y *the_info.screenwidth + x)*2 + screenbase;
    short value = (the_info.attribute << 8) + chr;
    asm push fs;
    asm mov ax,[realdata];
    asm mov fs,ax;
    asm mov ecx,[pos];
  asm mov ax,[value];
    asm mov word ptr fs:[ecx],ax;
    asm pop fs;
}
static void scrollup (int ytop)
{
    int y,x;
    int p1,p2;
    asm push fs;
    asm mov ax,[realdata];
    asm mov fs,ax;
    for (y=ytop+1+the_info.wintop; y <the_info.winbottom; y++) {
        p1 = screenbase + (y* the_info.screenwidth + the_info.winleft)*2;
        p2 = p1 + the_info.screenwidth*2;
        for (x=the_info.winleft; x < the_info.winright; x++) {
            asm mov eax,[p1];
            asm mov ecx,[p2];
            asm mov dx,word ptr fs:[ecx];
            asm mov word ptr fs:[eax],dx;
            p1+=2;
            p2+=2;
        }
    }
    asm pop fs
    y = the_info.attribute;
    the_info.attribute = the_info.normattr;
    for (x=the_info.winleft; x < the_info.winright; x++)
        putthechar(x,the_info.winbottom-1,' ');
    the_info.attribute = y;
}
static void scrolldown (int ytop)
{
    int y,x;
    int p1,p2;
    asm push fs;
    asm mov ax,[realdata];
    asm mov fs,ax;
    for (y = the_info.winbottom-1; y>ytop+the_info.wintop; y--) {
        p1 = (y* the_info.screenwidth + the_info.winleft)*2 + screenbase;
        p2 = p1 - the_info.screenwidth*2;
        for (x=the_info.winleft; x < the_info.winright; x++) {
            asm mov eax,[p1];
            asm mov ecx,[p2];
            asm mov dx,word ptr fs:[ecx];
            asm mov word ptr fs:[eax],dx;
            p1+=2;
            p2+=2;
        }
    }
    asm pop fs;
    for (x=the_info.winleft; x < the_info.winright; x++)
        putthechar(x,ytop - the_info.wintop,' ');
}
        
void _RTL_FUNC         clreol( void )
{
    int x;
    for (x=the_info.curx+ the_info.winleft; x < the_info.winright; x++)
        putthechar(x,the_info.cury,' ');
}
void _RTL_FUNC         clrscr( void )
{
    int x,y;
    for (x=the_info.winleft; x < the_info.winright; x++)
        for (y  = the_info.wintop; y < the_info.winbottom; y++)
            putthechar(x,y,' ');
    the_info.curx = 0;
    the_info.cury = 0;
}
void _RTL_FUNC         gotoxy( int __x, int __y )
{
    the_info.curx = __x;
    the_info.cury = __y;
}
int _RTL_FUNC          getche( void )
{
    int inch = getch();
    putch(inch);
    return inch;
}
int _RTL_FUNC          putch( int __c )
{
    switch (__c) {
        case 8:
        case 0x7f:
            the_info.curx--;
            break;
        case '\r':
            the_info.curx = 0;
            break;
        case 7:
            fputc(__c,stdout);
            break;
        case '\n':
            the_info.curx = 0;
            if (++the_info.cury >= the_info.winbottom - the_info.winleft) {
                the_info.cury--;
                scrollup(0);
            }
            break;
        case '\t':
            the_info.curx += 8;
            the_info.curx &= 0xf8;
            if (the_info.curx >= the_info.winright - the_info.winleft) {
                the_info.curx = 0;
                if (++the_info.cury >= the_info.winbottom - the_info.winleft) {
                    the_info.cury--;
                    scrollup(0);
                }
            }
            break;
        default:
            putthechar(the_info.curx + the_info.winleft,
                the_info.cury + the_info.wintop,
                    __c);
            if (++the_info.curx >= the_info.winright - the_info.winleft) {
                the_info.curx = 0;
                if (++the_info.cury >= the_info.winbottom - the_info.winleft) {
                    the_info.cury--;
                    scrollup(0);
                }
            }
            break;
    }
}
int _RTL_FUNC          wherex( void )
{
    return the_info.curx;
}
int _RTL_FUNC          wherey( void )
{
    return the_info.cury;
}


void _RTL_FUNC         delline( void )
{
    scrollup(the_info.cury);
}
int _RTL_FUNC          gettext( int __left, int __top,
                           int __right, int __bottom,
                           void *__destin)
{
    int x,y;
    short *q = __destin;
    __left += the_info.winleft;
    __top += the_info.wintop;
    __right += the_info.winleft;
    __bottom += the_info.wintop;
    asm push fs
    asm mov fs,[realdata]
    for (y=__top; y < __bottom; y++) {
        int pos;
        short value;
        pos = (y *the_info.screenwidth + __left)*2 + screenbase;
        for (x= __left; x < __right; x++) {
            asm mov ecx,[pos];
            asm inc [pos];
            asm inc [pos];
            asm mov ax,word ptr fs:[ecx];
                asm mov [value],ax;
            *q++ = value;
        }
    }
    asm pop fs;
}
void _RTL_FUNC         gettextinfo (struct text_info *__r )
{
    *__r = the_info;
}
void _RTL_FUNC         highvideo( void )
{
    the_info.attribute |= BLINK;
}
void _RTL_FUNC         insline( void )
{
    scrolldown(the_info.cury);
}
void _RTL_FUNC         lowvideo( void )
{
    the_info.attribute &= ~BLINK;
}
int _RTL_FUNC          movetext( int __left, int __top,
                            int __right, int __bottom,
                            int __destleft, int __desttop );
void _RTL_FUNC         normvideo( void )
{
    the_info.attribute = the_info.normattr;
}
int _RTL_FUNC          puttext( int __left, int __top,
                           int __right, int __bottom,
                           void *__source )
{
    int x,y;
    short *q = __source;
    __left += the_info.winleft;
    __top += the_info.wintop;
    __right += the_info.winleft;
    __bottom += the_info.wintop;
    asm push fs;
    asm mov fs,[realdata]
    for (y=__top; y < __bottom; y++) {
        int pos;
        short value;
        pos = (y *the_info.screenwidth + __left)*2 + screenbase;
        for (x= __left; x < __right; x++) {
            value = *q++;
            asm mov ecx,[pos]
            asm inc [pos]
            asm inc [pos]
                asm mov ax,[value]
            asm mov word ptr fs:[ecx],ax
        }
    }
    asm pop fs
}
void _RTL_FUNC         textattr( int __newattr )
{
    the_info.attribute = __newattr;
}
void _RTL_FUNC         textbackground( int __newcolor )
{
    the_info.attribute &= 0xf;
    the_info.attribute |= __newcolor << 4;
}
void _RTL_FUNC         textcolor( int __newcolor )
{
    the_info.attribute &= 0xf0;
    the_info.attribute |= __newcolor & 0xf;
}
static void textini(int __newmode)
{
    the_info.winleft = the_info.wintop = 0;
    the_info.screenheight = the_info.winbottom = 25;
    the_info.currmode = __newmode;
    screenbase = 0xb8000;
    chrheight = 16;
    baseport = 0x3d4;
    ulline = 12;
    switch (__newmode) {
        case 0:
            ulline = 7;
        case 1:
            the_info.winright = 40;
            the_info.screenwidth = 40;
            biosmode(__newmode);
            break;
        default:
        case 2:
            ulline = 7;
        case 3:
            the_info.winright = 80;
            the_info.screenwidth = 80;
            break;
        case 7:
            the_info.winright = 80;
            the_info.screenwidth = 80;
            screenbase = 0xb0000;
            baseport = 0x3b4;
            break;
        case 64:
            the_info.winright = 80;
            the_info.winbottom = 50;
            chrheight = 8;
            
            break;
    }
}
void _RTL_FUNC         textmode( int __newmode )
{
    if (__newmode == LASTMODE)
        __newmode = the_info.currmode;
    the_info.curx = the_info.cury = 0;
    textini(__newmode);
    biosmode(__newmode);
}
void _RTL_FUNC         window( int __left, int __top, int __right, int __bottom)
{
    the_info.winleft = __left;
    the_info.winright = __right;
    the_info.wintop = __top;
    the_info.winbottom = __bottom;
    the_info.curx = 0;
    the_info.cury = 0;
}

void _RTL_FUNC         _setcursortype( int __cur_t )
{
    the_info.currmode = __cur_t;
    switch (__cur_t) {
        case _NOCURSOR:
            outp(baseport,10);
            outp(baseport+1,chrheight);
            outp(baseport,11);
            outp(baseport+1,0);
            break;
        case _SOLIDCURSOR:
            outp(baseport,10);
            outp(baseport+1,0);
            outp(baseport,11);
            outp(baseport+1,chrheight);
            break;
        case _NORMALCURSOR:
            outp(baseport,10);
            outp(baseport+1,ulline-1);
            outp(baseport,11);
            outp(baseport+1,ulline);
            break;
    }
}
char *_RTL_FUNC        cgets( char *__str )
{
    char *q = __str;
    int val;
    while ((val = getche()) != '\r')
        *q++ = val;
    *q++ = 0;
    return __str;
}
int _RTL_FUNC          cprintf( const char *__format, ... )
{
    char buf[4096];
    va_list argptr;

    va_start( argptr, __format);
    vsprintf( buf,__format, argptr);
    va_end(argptr);
    cputs(buf);
    return 0;
}
int	_RTL_FUNC       cputs( const char *__str)
{
    for (; *__str; __str++) {
        putch(*__str);
    }
    return 0;
}
int _RTL_FUNC          cscanf( const char *__format, ... )
{
    va_list argptr;
    char buf[1024];
    cgets(buf);
    va_start( argptr, __format);
    vsscanf( buf,__format, argptr);
    va_end(argptr);
    return 0;
}
char *_RTL_FUNC        getpass( const char *__prompt )
{
    static char passbuf[256];
    char *q = passbuf;
    int inch;
    cputs(__prompt);
    inch = getch();
    while (inch != 13) {
        switch (inch) {
            case 8:
            case 0x7f:
                if (q == passbuf)
                    fputc(7,stdout);
                else {
                    putch(8);
                    putch(' ');
                    putch(8);
                    q--;
                }
                break;
            default:
                if (inch < 0x20 || inch > 0x7f)
                    fputc(7,stdout);
                else {
                    cputs("*");
                    *q++ = inch;
                }
                break;
        }
        inch = getch();
                
    }
    *q = 0;
    return passbuf;
}
int _RTL_FUNC          ungetch( int __ch )
{
    ungetchar = __ch;
}
