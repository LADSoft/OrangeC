/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#if !defined(__CONIO_H)
#define __CONIO_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

struct text_info {
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

#if !defined(__COLORS)
#define __COLORS

enum COLORS {
    BLACK,          /* dark colors */
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,       /* light colors */
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};

#endif

#define BLINK       128 /* blink bit */

enum text_modes {  LASTMODE=-1,
                   BW40=0,
                   C40,
                   BW80,
                   C80,
                   MONO=7,
                   C4350=64 };


void         _RTL_FUNC clreol( void );
void         _RTL_FUNC clrscr( void );
void         _RTL_FUNC gotoxy( int __x, int __y );
int          _RTL_FUNC getch( void );
int          _RTL_FUNC getche( void );
int          _RTL_FUNC kbhit( void );
int          _RTL_FUNC putch( int __c );
int          _RTL_FUNC wherex( void );
int          _RTL_FUNC wherey( void );

unsigned char    _RTL_INTRINS inportb( unsigned __portid );
unsigned         _RTL_INTRINS inport ( unsigned __portid );
unsigned         _RTL_INTRINS inportd ( unsigned __portid );
int              _RTL_INTRINS inp( unsigned __portid );
unsigned         _RTL_INTRINS inpw( unsigned __portid );
void             _RTL_INTRINS outportb( unsigned __portid, unsigned char __value );
void             _RTL_INTRINS outport ( unsigned __portid, unsigned __value );
void             _RTL_INTRINS outportd ( unsigned __portid, unsigned __value );
int              _RTL_INTRINS outp( unsigned __portid, int __value );
unsigned         _RTL_INTRINS outpw( unsigned __portid, unsigned __value );

void         _RTL_FUNC delline( void );
int          _RTL_FUNC gettext( int __left, int __top,
                           int __right, int __bottom,
                           void *__destin);
void         _RTL_FUNC gettextinfo (struct text_info *__r );
void         _RTL_FUNC highvideo( void );
void         _RTL_FUNC insline( void );
void         _RTL_FUNC lowvideo( void );
int          _RTL_FUNC movetext( int __left, int __top,
                            int __right, int __bottom,
                            int __destleft, int __desttop );
void         _RTL_FUNC normvideo( void );
int          _RTL_FUNC puttext( int __left, int __top,
                           int __right, int __bottom,
                           void *__source );
void         _RTL_FUNC textattr( int __newattr );
void         _RTL_FUNC textbackground( int __newcolor );
void         _RTL_FUNC textcolor( int __newcolor );
void         _RTL_FUNC textmode( int __newmode );
void         _RTL_FUNC window( int __left, int __top, int __right, int __bottom);

void         _RTL_FUNC _setcursortype( int __cur_t );
char *       _RTL_FUNC cgets( char *__str );
int          _RTL_FUNC cprintf( const char *__format, ... );
int          _RTL_FUNC cputs( const char *__str );
int          _RTL_FUNC cscanf( const char *__format, ... );
char *       _RTL_FUNC getpass( const char *__prompt );
int          _RTL_FUNC ungetch( int __ch );

char * 		 _RTL_FUNC _cgets(char *);
int			 _RTL_FUNC _cprintf(const char *, ...);
int			 _RTL_FUNC _cputs(const char *);
int			 _RTL_FUNC _cscanf(const char *, ...);
int			 _RTL_FUNC _getch(void);
int			 _RTL_FUNC _getche(void);
int 		 _RTL_FUNC _inp(unsigned short);
unsigned short _RTL_FUNC _inpw(unsigned short);
unsigned long _RTL_FUNC _inpd(unsigned short);
int 		 _RTL_FUNC _kbhit(void);
int			 _RTL_FUNC _outp(unsigned short, int);
unsigned short _RTL_FUNC _outpw(unsigned short, unsigned short);
unsigned long _RTL_FUNC _outpd(unsigned short, unsigned long);
int			 _RTL_FUNC _putch(int);
int			 _RTL_FUNC _ungetch(int);

#ifdef __cplusplus
}
}
#endif

#pragma pack()

#endif  /* __CONIO_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__CONIO_H_USING_LIST)
#define __CONIO_H_USING_LIST
    using std::clreol;
    using std::clrscr;
    using std::gotoxy;
    using std::getch;
    using std::getche;
    using std::kbhit;
    using std::putch;
    using std::wherex;
    using std::wherey;
    using std::inportb;
    using std::inport;
    using std::inportd;
    using std::inp;
    using std::inpw;
    using std::outportb;
    using std::outport;
    using std::outportd;
    using std::outp;
    using std::outpw;
    using std::delline;
    using std::gettext;
    using std::gettextinfo;
    using std::highvideo;
    using std::insline;
    using std::lowvideo;
    using std::movetext;
    using std::normvideo;
    using std::puttext;
    using std::textattr;
    using std::textbackground;
    using std::textcolor;
    using std::textmode;
    using std::window;
    using std::_setcursortype;
    using std::cgets;
    using std::cprintf;
    using std::cputs;
    using std::cscanf;
    using std::getpass;
    using std::ungetch;
    using std::text_modes;
    using std::LASTMODE;
    using std::BW40;
    using std::C40;
    using std::BW80;
    using std::C80;
    using std::MONO;
    using std::C4350;
    using std::text_info;
    using std::COLORS;
    using std::BLACK;          /* dark colors */
    using std::BLUE;
    using std::GREEN;
    using std::CYAN;
    using std::RED;
    using std::MAGENTA;
    using std::BROWN;
    using std::LIGHTGRAY;
    using std::DARKGRAY;       /* light colors */
    using std::LIGHTBLUE;
    using std::LIGHTGREEN;
    using std::LIGHTCYAN;
    using std::LIGHTRED;
    using std::LIGHTMAGENTA;
    using std::YELLOW;
    using std::WHITE;
    using std::_cgets;
    using std::_cprintf;
    using std::_cputs;
    using std::_cscanf;
    using std::_getch;
    using std::_getche;
    using std::_inp;
    using std::_inpw;
    using std::_inpd;
    using std::_kbhit;
    using std::_outp;
    using std::_outpw;
    using std::_outpd;
    using std::_putch;
    using std::_ungetch;
#endif
