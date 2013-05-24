/* 
   Copyright 1994-2003 Free Software Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
   USA

   You may contact the author at:

   mailto::camille@bluegrass.net

   or by snail mail at:

   David Lindauer
   850 Washburn Ave Apt 99
   Louisville, KY 40222
*/
#include <sys\stat.h>
#include <fcntl.h>
char buf[10000] ;
main()
{
   int handin,handout,a ;
   handin = open("q3.exe",O_BINARY) ;
   _fmode = O_BINARY ;
   handout = creat("temp",S_IWRITE) ;
   do {
      a = read(handin,buf,10000) ;
      if (a > 0)
         write(handout,buf,a) ;
   } while (a > 0) ;
   close(handin) ;
   close(handout) ;
}