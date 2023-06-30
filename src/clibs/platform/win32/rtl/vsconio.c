/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "conio.h"

int ungetchar;

static struct text_info the_info = {0, 0, 80, 25, 7, 7, 0, 25, 80, 0, 0};
static HANDLE inhand, outhand;
#pragma startup conio_init 31
#pragma rundown conio_rundown 31

void textini(int mode);

static void conio_init(void)
{
    unsigned char mode;
    unsigned short curs;
    unsigned char atr;
    CONSOLE_SCREEN_BUFFER_INFO info;
    int pos;
    inhand = CreateFile("CONIN$", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    outhand = CreateFile("CONOUT$", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    GetConsoleScreenBufferInfo(outhand, &info);
    the_info.screenwidth = info.srWindow.Right - info.srWindow.Left + 1;
    the_info.screenheight = info.srWindow.Bottom - info.srWindow.Top + 1;
    the_info.curx = info.dwCursorPosition.X;
    the_info.cury = info.dwCursorPosition.Y;
    the_info.normattr = the_info.attribute = info.wAttributes;
}
static void conio_rundown(void)
{
    CloseHandle(inhand);
    CloseHandle(outhand);
}
int _RTL_FUNC kbhit(void)
{
    unsigned long read;
    INPUT_RECORD r;

    do
    {
        if (!GetNumberOfConsoleInputEvents(inhand, &read) || !read)
            return 0;
        if (!PeekConsoleInput(inhand, &r, 1, &read) || !read)
            return 0;
        if (r.EventType != KEY_EVENT || !r.Event.KeyEvent.bKeyDown)
        {  //  || !(r.Event.KeyEvent.uChar.AsciiChar)) {
            if (!ReadConsoleInput(inhand, &r, 1, &read) || !read)
                return 0;
        }
        else
        {
            return 1;
        }
    } while (TRUE);
    return 0;
}
int _RTL_FUNC getch(void)
{
    int rv;
    DWORD read;
    INPUT_RECORD r;
    if (ungetchar)
    {
        rv = ungetchar;
        ungetchar = 0;
    }
    else
    {
        while (!kbhit())
            Sleep(100);
        ReadConsoleInput(inhand, &r, 1, &read);
        rv = r.Event.KeyEvent.uChar.AsciiChar;
        if (rv == 0)
            ungetchar = r.Event.KeyEvent.wVirtualScanCode;
    }
    return rv;
}
static void biosmode(int oldmode, int newmode)
{
    SMALL_RECT r;
    COORD cx;
    if (oldmode < 2)
        oldmode = 0;
    else if (oldmode < 8)
        oldmode = 1;
    else
        oldmode = 2;
    if (newmode < 2)
        newmode = 0;
    else if (newmode < 8)
        newmode = 1;
    else
        newmode = 2;
    if (oldmode == newmode)
        return;
    r.Left = r.Top = 0;
    cx.X = r.Right = the_info.screenwidth;
    cx.Y = r.Bottom = the_info.screenheight;
    if (oldmode > newmode)
    {
        SetConsoleWindowInfo(outhand, FALSE, &r);
        //      SetConsoleScreenBufferSize(outhand,cx) ;
    }
    else
    {
        SetConsoleScreenBufferSize(outhand, cx);
        //      SetConsoleWindowInfo(outhand,FALSE,&r) ;
    }
}
static void scrollup(int ytop)
{
    SMALL_RECT src, clip;
    COORD dest;
    CHAR_INFO atr;
    src.Top = ytop + 1 + the_info.wintop;
    src.Bottom = the_info.winbottom;
    src.Left = the_info.winleft;
    src.Right = the_info.winright;
    clip = src;
    clip.Top--;
    dest.X = src.Left;
    dest.Y = clip.Top;
    atr.Char.UnicodeChar = ' ';
    atr.Attributes = the_info.attribute;
    ScrollConsoleScreenBuffer(outhand, &src, &clip, dest, &atr);
}
static void scrolldown(int ytop)
{
    SMALL_RECT src, clip;
    COORD dest;
    CHAR_INFO atr;
    src.Top = ytop + the_info.wintop;
    src.Bottom = the_info.winbottom - 1;
    src.Left = the_info.winleft;
    src.Right = the_info.winright;
    clip = src;
    //   clip.Bottom++ ;
    dest.X = src.Left;
    dest.Y = clip.Top + 1;
    atr.Char.UnicodeChar = ' ';
    atr.Attributes = the_info.attribute;
    ScrollConsoleScreenBuffer(outhand, &src, &clip, dest, &atr);
}

void _RTL_FUNC clreol(void)
{
    DWORD written;
    COORD cx;
    cx.X = the_info.winleft + the_info.curx;
    cx.Y = the_info.wintop + the_info.cury;
    //   FillConsoleOutputAttribute(outhand,the_info.attribute,
    //      the_info.winright -the_info.winleft - the_info.curx,
    //      cx,&written) ;
    FillConsoleOutputCharacter(outhand, ' ', the_info.winright - the_info.winleft - the_info.curx, cx, &written);
}
void _RTL_FUNC clrscr(void)
{
    int y;
    the_info.curx = 0;
    the_info.cury = 0;
    for (y = the_info.wintop; y < the_info.winbottom; y++)
    {
        clreol();
        the_info.cury++;
    }
    gotoxy(0, 0);
}
void _RTL_FUNC gotoxy(int __x, int __y)
{
    COORD pos;
    the_info.curx = __x;
    the_info.cury = __y;
    pos.X = the_info.curx + the_info.winleft;
    pos.Y = the_info.cury + the_info.wintop;
    SetConsoleCursorPosition(outhand, pos);
}
int _RTL_FUNC getche(void)
{
    int inch = getch();
    putch(inch);
    return inch;
}
int _RTL_FUNC putch(int __c)
{
    DWORD written;
    switch (__c)
    {
        case 8:
        case 0x7f:
            the_info.curx--;
            gotoxy(the_info.curx, the_info.cury);
            break;
        case '\r':
            the_info.curx = 0;
            gotoxy(the_info.curx, the_info.cury);
            break;
        case 7:
            Beep(3000, 300);
            break;
        case '\n':
            the_info.curx = 0;
            if (++the_info.cury >= the_info.winbottom - the_info.wintop)
            {
                the_info.cury--;
                if (the_info.winbottom < the_info.screenheight)
                    scrollup(0);
            }
            gotoxy(the_info.curx, the_info.cury);
            break;
        case '\t':
            the_info.curx += 8;
            the_info.curx &= 0xf8;
            if (the_info.curx >= the_info.winright - the_info.winleft)
            {
                the_info.curx = 0;
                if (++the_info.cury >= the_info.winbottom - the_info.wintop)
                {
                    the_info.cury--;
                    if (the_info.winbottom < the_info.screenheight)
                        scrollup(0);
                }
            }
            gotoxy(the_info.curx, the_info.cury);
            break;
        default:
            WriteConsole(outhand, &__c, 1, &written, 0);
            if (++the_info.curx >= the_info.winright - the_info.winleft)
            {
                the_info.curx = 0;
                if (++the_info.cury >= the_info.winbottom - the_info.wintop)
                {
                    the_info.cury--;
                    if (the_info.cury < the_info.screenheight - 1)
                        scrollup(0);
                }
                gotoxy(the_info.curx, the_info.cury);
            }
            break;
    }
    return 0;
}
int _RTL_FUNC wherex(void) { return the_info.curx; }
int _RTL_FUNC wherey(void) { return the_info.cury; }
void _RTL_FUNC delline(void) { scrollup(the_info.cury); }
int _RTL_FUNC gettext(int __left, int __top, int __right, int __bottom, void* __destin)
{
    CHAR_INFO info[50 * 80];
    COORD destsize, destlt;
    SMALL_RECT src;
    int i;
    short* dest = __destin;
    destsize.X = __right - __left;
    destsize.Y = __bottom - __top;
    destlt.X = 0;
    destlt.Y = 0;
    src.Left = __left;
    src.Right = __right;
    src.Top = __top;
    src.Bottom = __bottom;
    ReadConsoleOutput(outhand, &info[0], destsize, destlt, &src);
    for (i = 0; i < destsize.X * destsize.Y; i++)
        *dest++ = info[i].Char.AsciiChar + (info[i].Attributes << 8);
    return 0;
}
void _RTL_FUNC gettextinfo(struct text_info* __r) { *__r = the_info; }
static void newatr(void) { SetConsoleTextAttribute(outhand, the_info.attribute); }
void _RTL_FUNC highvideo(void)
{
    the_info.attribute |= BLINK;
    newatr();
}
void _RTL_FUNC insline(void) { scrolldown(the_info.cury); }
void _RTL_FUNC lowvideo(void)
{
    the_info.attribute &= ~BLINK;
    newatr();
}
int _RTL_FUNC movetext(int __left, int __top, int __right, int __bottom, int __destleft, int __desttop)
{
    CHAR_INFO info[50 * 80];
    COORD destsize, destlt;
    SMALL_RECT src;
    int i;
    destsize.X = __right - __left;
    destsize.Y = __bottom - __top;
    destlt.X = 0;
    destlt.Y = 0;
    src.Left = __left;
    src.Right = __right;
    src.Top = __top;
    src.Bottom = __bottom;
    ReadConsoleOutput(outhand, &info[0], destsize, destlt, &src);
    src.Left = __destleft;
    src.Right = __destleft + __right - __left;
    src.Top = __desttop;
    src.Bottom = __desttop + __bottom - __top;
    WriteConsoleOutput(outhand, &info[0], destsize, destlt, &src);
    return 0;
}
void _RTL_FUNC normvideo(void)
{
    the_info.attribute = the_info.normattr;
    newatr();
}
int _RTL_FUNC puttext(int __left, int __top, int __right, int __bottom, void* __source)
{
    int i;
    CHAR_INFO info[50 * 80];
    COORD sourcesize, sourcelt;
    SMALL_RECT dest;
    short* source = __source;
    sourcesize.X = __right - __left;
    sourcesize.Y = __bottom - __top;
    sourcelt.X = 0;
    sourcelt.Y = 0;
    dest.Left = __left;
    dest.Right = __right;
    dest.Top = __top;
    dest.Bottom = __bottom;
    for (i = 0; i < sourcesize.X * sourcesize.Y; i++)
    {
        info[i].Char.AsciiChar = *source & 0xff;
        info[i].Attributes = *source++ >> 8;
    }
    WriteConsoleOutput(outhand, &info[0], sourcesize, sourcelt, &dest);
    return 0;
}
void _RTL_FUNC textattr(int __newattr)
{
    the_info.attribute = __newattr;
    newatr();
}
void _RTL_FUNC textbackground(int __newcolor)
{
    the_info.attribute &= 0xf;
    the_info.attribute |= __newcolor << 4;
    newatr();
}
void _RTL_FUNC textcolor(int __newcolor)
{
    the_info.attribute &= 0xf0;
    the_info.attribute |= __newcolor & 0xf;
    newatr();
}
static void textini(int __newmode)
{
    int oldmode;
    the_info.winleft = the_info.wintop = 0;
    the_info.screenheight = the_info.winbottom = 25;
    oldmode = the_info.currmode;
    the_info.currmode = __newmode;
    switch (__newmode)
    {
        case 0:
        case 1:
            the_info.winright = 40;
            the_info.screenwidth = 40;
            break;
        default:
        case 2:
        case 3:
        case 7:
            the_info.winright = 80;
            the_info.screenwidth = 80;
            break;
        case 64:
            the_info.winright = 80;
            the_info.screenwidth = 80;
            the_info.winbottom = 50;
            break;
    }
    biosmode(oldmode, __newmode);
}
void _RTL_FUNC textmode(int __newmode)
{
    if (__newmode == LASTMODE)
        __newmode = the_info.currmode;
    gotoxy(0, 0);
    textini(__newmode);
}
void _RTL_FUNC window(int __left, int __top, int __right, int __bottom)
{
    the_info.winleft = __left;
    the_info.winright = __right;
    the_info.wintop = __top;
    the_info.winbottom = __bottom;
    gotoxy(0, 0);
}

void _RTL_FUNC _setcursortype(int __cur_t)
{
    CONSOLE_CURSOR_INFO info;
    the_info.currmode = __cur_t;
    switch (__cur_t)
    {
        case _NOCURSOR:
            info.bVisible = FALSE;
            info.dwSize = 1;
            break;
        case _SOLIDCURSOR:
            info.bVisible = TRUE;
            info.dwSize = 99;
            break;
        case _NORMALCURSOR:
        default:
            info.bVisible = TRUE;
            info.dwSize = 5;
            break;
    }
    SetConsoleCursorInfo(outhand, &info);
}
char* _RTL_FUNC cgets(char* __str)
{
    char* q = __str;
    int val;
    while ((val = getche()) != '\r')
        *q++ = val;
    *q++ = 0;
    return __str;
}
int _RTL_FUNC cprintf(const char* __format, ...)
{
    char buf[1024];
    va_list argptr;

    va_start(argptr, __format);
    vsprintf(buf, __format, argptr);
    va_end(argptr);
    cputs(buf);
    return 0;
}
int _RTL_FUNC cputs(const char* __str)
{
    for (; *__str; __str++)
    {
        putch(*__str);
    }
    return 0;
}
#ifndef VSDLL
int _RTL_FUNC cscanf(const char* __format, ...)
{
    va_list argptr;
    char buf[256];
    cgets(buf);
    va_start(argptr, __format);
    vsscanf(buf, __format, argptr);
    va_end(argptr);
    return 0;
}
#endif
char* _RTL_FUNC getpass(const char* __prompt)
{
    static char passbuf[256];
    char* q = passbuf;
    int inch;
    cputs(__prompt);
    inch = getch();
    while (inch != 13)
    {
        switch (inch)
        {
            case 8:
            case 0x7f:
                if (q == passbuf)
                    Beep(3000, 300);
                else
                {
                    putch(8);
                    putch(' ');
                    putch(8);
                    q--;
                }
                break;
            default:
                if (inch < 0x20 || inch > 0x7f)
                    Beep(3000, 300);
                else
                {
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
int _RTL_FUNC ungetch(int __ch) { ungetchar = __ch; }
unsigned char _RTL_FUNC inportb(unsigned __portid)
{
    __asm mov edx, [__portid];
    __asm in al, dx;
    __asm movzx eax, al;
    return _EAX;
}
unsigned _RTL_FUNC inport(unsigned __portid)
{
    __asm mov edx, [__portid];
    __asm in ax, dx;
    __asm movzx eax, ax;
    return _EAX;
}
unsigned _RTL_FUNC inportd(unsigned __portid)
{
    __asm mov edx, [__portid];
    __asm in eax, dx;
    return _EAX;
}
int _RTL_FUNC inp(unsigned __portid)
{
    __asm mov edx, [__portid];
    __asm in ax, dx;
    __asm movsx eax, ax;
    return _EAX;
}
unsigned _RTL_FUNC inpw(unsigned __portid)
{
    __asm mov edx, [__portid];
    __asm in ax, dx;
    __asm movzx eax, ax;
    return _EAX;
}
void _RTL_FUNC outportb(unsigned __portid, unsigned char __value)
{
    __asm mov edx, [__portid];
    __asm mov al, [__value];
    __asm out dx, al;
}
void _RTL_FUNC outport(unsigned __portid, unsigned __value)
{
    __asm mov edx, [__portid];
    __asm mov ax, word ptr[__value];
    __asm out dx, ax;
}
void _RTL_FUNC outportd(unsigned __portid, unsigned __value)
{
    __asm mov edx, [__portid];
    __asm mov eax, dword ptr[__value];
    __asm out dx, eax;
}
int _RTL_FUNC outp(unsigned __portid, int __value)
{
    __asm mov edx, [__portid];
    __asm mov al, byte ptr[__value];
    __asm out dx, al;
    return __value;
}
unsigned _RTL_FUNC outpw(unsigned __portid, unsigned __value)
{
    __asm mov edx, [__portid];
    __asm mov ax, word ptr[__value];
    __asm out dx, ax;
    return __value;
}
#ifdef XXXXX
main()
{
    short buf[80 * 50];
    conio_init();
    //   window(5,5,20,20) ;
    clrscr();
    textcolor(RED);
    _setcursortype(_NORMALCURSOR);
    cputs("hello there david let us see if this fits in the window");
    while (1)
        cputs("x");
    cgets(buf);
    movetext(5, 5, 20, 20, 30, 6);
    getch();
    conio_rundown();
}
#endif