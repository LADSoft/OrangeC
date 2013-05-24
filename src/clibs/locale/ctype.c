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
#include <stddef.h>
#include <ctype.h>
#include <locale.h>
#include "_locale.h"

#define ct_data (*(CTYPE_DATA *)(__locale_data[LC_CTYPE]))

// yes we are redefining it without a const for this one file only
unsigned short const _RTL_DATA * _pctype ;

static char C_qualified_name[] = "C";

ABBREVIATED_CTYPE_DATA C_ctype_data= {
    &C_qualified_name, 0,
    {
    _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, 
    _IS_CTL, _IS_SP |_IS_CTL, _IS_SP |_IS_CTL, _IS_SP |_IS_CTL, _IS_SP |_IS_CTL, _IS_SP |_IS_CTL, _IS_CTL, _IS_CTL, 
    _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, 
    _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, _IS_CTL, 
    _IS_SP | _IS_BLK, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, 
    _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, 
    _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, 
    _IS_DIG|_IS_HEX, _IS_DIG|_IS_HEX, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, 
    _IS_PUN, _IS_UPP|_IS_HEX, _IS_UPP|_IS_HEX, _IS_UPP|_IS_HEX, _IS_UPP|_IS_HEX, _IS_UPP|_IS_HEX, _IS_UPP|_IS_HEX, _IS_UPP, 
    _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, 
    _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, _IS_UPP, 
    _IS_UPP, _IS_UPP, _IS_UPP, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, 
    _IS_PUN, _IS_LOW|_IS_HEX, _IS_LOW|_IS_HEX, _IS_LOW|_IS_HEX, _IS_LOW|_IS_HEX, _IS_LOW|_IS_HEX, _IS_LOW|_IS_HEX, _IS_LOW, 
    _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, 
    _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, _IS_LOW, 
    _IS_LOW, _IS_LOW, _IS_LOW, _IS_PUN, _IS_PUN, _IS_PUN, _IS_PUN, _IS_CTL, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    }
};
NUMERIC_DATA C_numeric_data= {
    &C_qualified_name,
    ".",
    "",
    "",
};
void *__locale_data[LC_LAST+1] = {
    0,
    0,
    &C_ctype_data,
    0,
    &C_numeric_data,
    0,
    0,
    0
} ;

#pragma startup __ctype_init 231 /* must be initted after __locale_reinit */
void __ctype_init(void)
{
    _pctype = (unsigned short *)(((unsigned char *)__locale_data[LC_CTYPE]) + offsetof(CTYPE_DATA, ctypetab));
}
int _RTL_FUNC (tolower)(int c)
{
   if ((unsigned)c >= 256)
      return c ;
    if (ct_data.flags & 1)
        c = ct_data.tolower[c];
    else
        if (_pctype[c] & _IS_UPP)
            c+=32;
    return c;
}
int _RTL_FUNC (toupper)(int c)
{
   if ((unsigned)c >= 256)
      return c ;
    if (ct_data.flags & 1)
        c = ct_data.toupper[c];
    else
        if (_pctype[c] & _IS_LOW)
            c-=32;
    return c;
}
int _RTL_FUNC (_tolower)(int c)
{
    return tolower(c);
}
int _RTL_FUNC (_toupper)(int c)
{
    return toupper(c);
}
int _RTL_FUNC (_ltolower)(int c)
{
    return tolower(c);
}
int _RTL_FUNC (_ltoupper)(int c)
{
    return toupper(c);
}
int _RTL_FUNC (isspace)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_SP) ;
}
int _RTL_FUNC (isblank)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_BLK) ;
}
int _RTL_FUNC (isalpha)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & (_IS_UPP | _IS_LOW)) ;
}
int _RTL_FUNC (ispunct)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_PUN) ;
}
int _RTL_FUNC (islower)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_LOW) ;
}
int _RTL_FUNC (isupper)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_UPP) ;
}
int _RTL_FUNC (isprint)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & (_IS_PRINT)) ;
}

int _RTL_FUNC (isalnum)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & (_IS_UPP | _IS_LOW | _IS_DIG)) ;
}
int _RTL_FUNC (isgraph)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_GRAPH) ;
}
int _RTL_FUNC (isdigit)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_DIG) ;
}
int _RTL_FUNC (isxdigit)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
   return (_pctype[c] & _IS_HEX) ;
}
int _RTL_FUNC (toascii)(int c)
{
   return (c & 0x7f) ;
}
int _RTL_FUNC (__toascii)(int c)
{
   return (c & 0x7f) ;
}
int _RTL_FUNC (isascii)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
    return ( c < 0x80);
}
int _RTL_FUNC (__isascii)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
    return ( c < 0x80);
}
int _RTL_FUNC (iscntrl)(int c)
{
   if ((unsigned)c >= 256)
      return 0 ;
  return (_pctype[c] & _IS_CTL) ;
}
int _RTL_FUNC (_isctype)(int c, int flag)
{
   if ((unsigned)c >= 256)
      return 0 ;
    return(_pctype[c] & flag);
}
int _RTL_FUNC (__iscsymf)(int c)
{
    return isalpha(c) || c == '_';
}
int _RTL_FUNC (__iscsym)(int c)
{
    return isalnum(c) || c == '_';
}
