/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "_rtti.h"


namespace __dls
{

const char _RTL_DATA* __dls_NoNamedException = "No Named Exception";
const char _RTL_DATA* __dls_BadException = "Bad Exception";

};  // namespace __dls

namespace std
{
    terminate_handler __term = abort;
    unexpected_handler __unexpected = std::terminate;
exception::~exception() throw() {}
bad_exception::~bad_exception() throw() {}
};  // namespace std

extern "C" void _RTL_FUNC __call_terminate()
{
    try
    {
        flushall();
        std::__term();
    }
    catch (...)
    {
    }
}
void _RTL_FUNC std::terminate()
{
    __call_terminate();
}
std::terminate_handler _RTL_FUNC std::set_terminate(std::terminate_handler __t)
{
    terminate_handler rv = __term;
    if (__t)
    {
        __term = __t;
    }
    return rv;
}
std::unexpected_handler _RTL_FUNC std::set_unexpected(std::unexpected_handler __u)
{
    unexpected_handler rv = (unexpected_handler)__unexpected;
    if (__u)
    {
        __unexpected = __u;
    }
    return rv;
}
std::unexpected_handler _RTL_FUNC std::get_unexpected() {
    return __unexpected;
}
extern "C" void _RTL_FUNC __call_unexpected(std::exception_ptr *e)
{
    try
    {
        std::__unexpected();
        __call_terminate();
    }
    catch (...)
    {
    }
    *e = std::current_exception();
}
void _RTL_FUNC std::unexpected()
{
    std::exception_ptr e;
    __call_unexpected(&e);
}
