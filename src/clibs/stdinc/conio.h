/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#pragma GCC system_header

#if !defined(__CONIO_H)
#    define __CONIO_H

#    pragma pack(1)

#    include <stddef.h>

#    define _NOCURSOR 0
#    define _SOLIDCURSOR 1
#    define _NORMALCURSOR 2

#    ifdef __cplusplus
extern "C"
{
#    endif

#ifndef RC_INVOKED
	    struct text_info
    {
        unsigned char winleft;
        unsigned char wintop;
        unsigned char winright;
        unsigned char winbottom;
        unsigned char attribute;
        unsigned char normattr;
        unsigned char currmode;
        unsigned char screenheight;
        unsigned char screenwidth;
        unsigned char curx;
        unsigned char cury;
    };
#    if !defined(__COLORS)
#        define __COLORS

    enum COLORS
    {
        BLACK, /* dark colors */
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        BROWN,
        LIGHTGRAY,
        DARKGRAY, /* light colors */
        LIGHTBLUE,
        LIGHTGREEN,
        LIGHTCYAN,
        LIGHTRED,
        LIGHTMAGENTA,
        YELLOW,
        WHITE
    };

#    endif
#endif

#    define BLINK 128 /* blink bit */

#ifndef RC_INVOKED
    enum text_modes
    {
        LASTMODE = -1,
        BW40 = 0,
        C40,
        BW80,
        C80,
        MONO = 7,
        C4350 = 64
    };

    void _RTL_FUNC _IMPORT clreol(void);
    void _RTL_FUNC _IMPORT clrscr(void);
    void _RTL_FUNC _IMPORT gotoxy(int __x, int __y);
    int _RTL_FUNC _IMPORT getch(void);
    int _RTL_FUNC _IMPORT getche(void);
    int _RTL_FUNC _IMPORT kbhit(void);
    int _RTL_FUNC _IMPORT putch(int __c);
    int _RTL_FUNC _IMPORT wherex(void);
    int _RTL_FUNC _IMPORT wherey(void);

    unsigned char _RTL_INTRINS inportb(unsigned __portid);
    unsigned _RTL_INTRINS inport(unsigned __portid);
    unsigned _RTL_INTRINS inportd(unsigned __portid);
    int _RTL_INTRINS inp(unsigned __portid);
    unsigned _RTL_INTRINS inpw(unsigned __portid);
    void _RTL_INTRINS outportb(unsigned __portid, unsigned char __value);
    void _RTL_INTRINS outport(unsigned __portid, unsigned __value);
    void _RTL_INTRINS outportd(unsigned __portid, unsigned __value);
    int _RTL_INTRINS outp(unsigned __portid, int __value);
    unsigned _RTL_INTRINS outpw(unsigned __portid, unsigned __value);

    void _RTL_FUNC _IMPORT delline(void);
    int _RTL_FUNC _IMPORT gettext(int __left, int __top, int __right, int __bottom, void* __destin);
    void _RTL_FUNC _IMPORT gettextinfo(struct text_info* __r);
    void _RTL_FUNC _IMPORT highvideo(void);
    void _RTL_FUNC _IMPORT insline(void);
    void _RTL_FUNC _IMPORT lowvideo(void);
    int _RTL_FUNC _IMPORT movetext(int __left, int __top, int __right, int __bottom, int __destleft, int __desttop);
    void _RTL_FUNC _IMPORT normvideo(void);
    int _RTL_FUNC _IMPORT puttext(int __left, int __top, int __right, int __bottom, void* __source);
    void _RTL_FUNC _IMPORT textattr(int __newattr);
    void _RTL_FUNC _IMPORT textbackground(int __newcolor);
    void _RTL_FUNC _IMPORT textcolor(int __newcolor);
    void _RTL_FUNC _IMPORT textmode(int __newmode);
    void _RTL_FUNC _IMPORT window(int __left, int __top, int __right, int __bottom);

    void _RTL_FUNC _IMPORT _setcursortype(int __cur_t);
    char* ZSTR _RTL_FUNC _IMPORT cgets(char* ZSTR __str);
    int _RTL_FUNC _IMPORT cprintf(const char* ZSTR __format, ...);
    int _RTL_FUNC _IMPORT cputs(const char* ZSTR __str);
    int _RTL_FUNC _IMPORT cscanf(const char* ZSTR __format, ...);
    char* ZSTR _RTL_FUNC _IMPORT getpass(const char* ZSTR __prompt);
    int _RTL_FUNC _IMPORT ungetch(int __ch);

    char* ZSTR _RTL_FUNC _IMPORT _cgets(char* ZSTR);
    int _RTL_FUNC _IMPORT _cprintf(const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _cputs(const char* ZSTR);
    int _RTL_FUNC _IMPORT _cscanf(const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _getch(void);
    int _RTL_FUNC _IMPORT _getche(void);
    int _RTL_FUNC _IMPORT _inp(unsigned short);
    unsigned short _RTL_FUNC _IMPORT _inpw(unsigned short);
    unsigned long _RTL_FUNC _IMPORT _inpd(unsigned short);
    int _RTL_FUNC _IMPORT _kbhit(void);
    int _RTL_FUNC _IMPORT _outp(unsigned short, int);
    unsigned short _RTL_FUNC _IMPORT _outpw(unsigned short, unsigned short);
    unsigned long _RTL_FUNC _IMPORT _outpd(unsigned short, unsigned long);
    int _RTL_FUNC _IMPORT _putch(int);
    int _RTL_FUNC _IMPORT _ungetch(int);

#endif
#    ifdef __cplusplus
}
#    endif

#    pragma pack()

#endif /* __CONIO_H */
