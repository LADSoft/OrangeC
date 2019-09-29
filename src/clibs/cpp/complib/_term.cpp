/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include <stdlib.h>
#include <exception>
#include "_rtti.h"

extern int ___xcflags;

static CPPDATA* dataCache;

#pragma startup __init_cppdata 119
#pragma rundown __rundown_cppdata 6

namespace __dls
{

const char _RTL_DATA* __dls_NoNamedException = "No Named Exception";
const char _RTL_DATA* __dls_BadException = "Bad Exception";

};  // namespace __dls

namespace std
{
exception::~exception() throw() {}
bad_exception::~bad_exception() throw() {}
};  // namespace std

void _RTL_FUNC std::terminate()
{
    ___xcflags = 0;
    abort();
}
static CPPDATA* getCPPData() { return dataCache; }
extern "C" void _RTL_FUNC __call_terminate()
{
    try
    {
        flushall();
        CPPDATA* data = getCPPData();
        (*data->term)();
    }
    catch (...)
    {
    }
    abort();
}
void _RTL_FUNC std::unexpected()
{
    ___xcflags = 0;
    __call_terminate();
}
std::terminate_handler _RTL_FUNC std::set_terminate(std::terminate_handler __t)
{
    CPPDATA* data = getCPPData();
    terminate_handler* rv = (terminate_handler)data->term;
    if (__t)
    {
        data->term = __t;
    }
    return rv;
}
std::unexpected_handler _RTL_FUNC std::set_unexpected(std::unexpected_handler __u)
{
    CPPDATA* data = getCPPData();
    unexpected_handler* rv = (unexpected_handler)data->unexpected;
    if (__u)
    {
        CPPDATA* data = getCPPData();
        data->unexpected = __u;
    }
    return rv;
}
extern "C" void _RTL_FUNC __call_unexpected()
{
    CPPDATA* data = getCPPData();
    (*data->unexpected)();
}
extern "C" void _RTL_FUNC __init_cppdata()
{
    CPPDATA* block;
    block = (CPPDATA*)malloc(sizeof(CPPDATA));
    if (!block)
        exit(1);
    dataCache = block;
    block->term = std::terminate;
    block->unexpected = std::unexpected;
}
extern "C" void _RTL_FUNC __rundown_cppdata()
{
    CPPDATA* block = getCPPData();
    free(block);
}