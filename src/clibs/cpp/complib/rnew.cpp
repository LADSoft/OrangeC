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

#include <new>
#include <stdlib.h>

namespace __dls
{
const char _RTL_DATA* __dls_bad_alloc = "bad_alloc";
};

/*
namespace std {

//   nothrow_t _RTL_DATA nothrow;
   bad_alloc::~bad_alloc()
   {
   }

} ;
*/

#ifdef STD_NEWHANDLER
static std::new_handler _new_handler;
namespace std
{
#else
static new_handler _new_handler;
#endif

new_handler _RTL_FUNC set_new_handler(new_handler __newv)
{
    new_handler rv = _new_handler;
    _new_handler = __newv;
    return rv;
}
#ifdef STD_NEWHANDLER
}
#endif

void* __realnew(size_t n)
{
    if (!n)
        n = 1;
    do
    {
        void* rv = malloc(n);
        if (rv)
            return rv;
        if (!_new_handler)
            throw std::bad_alloc();
        (*_new_handler)();
    } while (1);
    return 0;  // never gets here
}