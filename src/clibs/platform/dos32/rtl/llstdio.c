/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <dpmi.h>
#include "llp.h"

extern int __maxfiles;
static unsigned char inbuf[512];

extern FILE *_pstreams[] ;

static struct __file2 _iextended[3] = 
{
{ "CON:" },
{ "CON:" },
{ "CON:" },
};

FILE _istreams[3] = { 
   { FILTOK, _F_READ | _F_LBUF | _F_TERM,0,0,0, 512,inbuf,inbuf,&_iextended[0]},
   { FILTOK, _F_WRIT | _F_LBUF | _F_TERM,0,1,0, 0,0,0,&_iextended[1] },
   { FILTOK, _F_WRIT | _F_LBUF | _F_TERM,0,2,0, 0,0,0,&_iextended[2] }
} ;

#undef stdin
#undef stdout
#undef stderr

#define stdin (&_istreams[0])
#define stdout (&_istreams[1])
#define stderr (&_istreams[2])

FILE *_RTL_FUNC  __getStream(int stream)
{
      return _pstreams[stream] ;
}
void __ll_init(void)
{
   static int done ;
   if (!done) {
      _pstreams[0] = &_istreams[0];
      _pstreams[1] = &_istreams[1];
      _pstreams[2] = &_istreams[2];
      __maxfiles = 3;
      done = 1 ;
   }
}
