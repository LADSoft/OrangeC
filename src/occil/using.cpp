#include "be.h"
#include "DotNetPELib.h"
#include <string>
#include "Utils.h"
#include "CmdFiles.h"

using namespace DotNetPELib;

extern PELib* peLib;
extern const char* pinvoke_dll;

struct data
{
    const char* name;
    const char* dllName;
};
#define HASHLEN 2048
static LIST* _global_using_list;
void _using_init() {}
bool _using_(const char* file)
{
    char name[260], *p;
    strcpy(name, file);
    p = strrchr(name, '.');
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
        LIST *lst = (LIST*)calloc(sizeof(LIST), 1), **find = &_global_using_list;
        lst->data = strdup(buf);
        while (*find)
            find = &(*find)->next;
        (*find) = lst;
    }
}
void _apply_global_using(void)
{
    if (!cparams.managed_library)
    {
        char buf[256];
        strcpy(buf, pinvoke_dll);
        char* p = strrchr(buf, '.');
        if (p)
            *p = 0;
        _using_(buf);
    }
    _using_("occmsil");
    LIST* lst = _global_using_list;
    while (lst)
    {
        _using_((char*)lst->data);
        lst = lst->next;
    }
}
bool msil_managed(SimpleSymbol* sp)
{
    if (sp->msil_rtl)
        return true;
    return !_dll_name(sp->name).size();
}