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

#include "be.h"
#include "DotNetPELib.h"
#include <string>
#include "Utils.h"
#include "msilInit.h"
#include "ildata.h"
using namespace DotNetPELib;

#ifdef HAVE_UNISTD_H
#    define _strdup strdup
#endif

namespace occmsil
{
struct data
{
    const char* name;
    const char* dllName;
};
#define HASHLEN 2048
static Optimizer::LIST* _global_using_list;
void _using_init() {}
bool _using_(const char* file)
{
    char name[260], *p;
    strcpy(name, file);
    p = (char*)strrchr(name, '.');
    if (p && Utils::iequal(p, ".dll"))
        *p = 0;
    if (!peLib->LoadAssembly(name))
        return true;
    if (peLib->LoadUnmanaged(file))
    {
        char* a = getenv("ORANGEC");
        if (a)
        {
            strcpy(name, a);
            strcat(name, "\\bin\\");
            strcat(name, file);
            if (!peLib->LoadUnmanaged(name))
                return true;
        }
    }
    return false;
}
std::string _dll_name(const char* name) { return peLib->FindUnmanagedName(name); }
// usually from the command line
void _add_global_using(const char* str)
{
    while (*str)
    {
        char buf[260], *q = buf;
        while (*str && *str != ';')
            *q++ = *str++;
        *q = 0;
        if (*str)
            str++;
        // using standard C allocators since this crosses multiple input files
        Optimizer::LIST *lst = (Optimizer::LIST*)calloc(sizeof(Optimizer::LIST), 1), **find = &_global_using_list;
        lst->data = _strdup(buf);
        while (*find)
            find = &(*find)->next;
        (*find) = lst;
    }
}
void _apply_global_using(void)
{
    if (!Optimizer::cparams.managed_library)
    {
        char buf[256];
        strcpy(buf, Optimizer::pinvoke_dll);
        char* p = (char*)strrchr(buf, '.');
        if (p)
            *p = 0;
        _using_(buf);
    }
    _using_("occmsil");
    Optimizer::LIST* lst = _global_using_list;
    while (lst)
    {
        _using_((char*)lst->data);
        lst = lst->next;
    }
}
bool msil_managed(Optimizer::SimpleSymbol* sp)
{
    if (sp->msil_rtl)
        return true;
    return !_dll_name(sp->name).size();
}
}  // namespace occmsil