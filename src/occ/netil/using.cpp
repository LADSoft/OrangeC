#include "compiler.h"
#include "dllexportreader.h"
#include "CmdFiles.h"
#include <string>
struct data
{
    char *name;
    char *dllName;
};
static HASHTABLE *_using_hash;
static LIST *_global_using_list;
extern "C" void _using_init()
{
    _using_hash = CreateHashTable(2048);
}
extern "C" BOOLEAN _using_(char *file)
{
    DLLExportReader reader(file);
    if (reader.Read())
    {
        std::string rootName = reader.GetName();
        unsigned npos = rootName.find_last_of(CmdFiles::DIR_SEP);
        if (npos != std::string::npos && npos != rootName.size()-1)
        {
            rootName = rootName.substr(npos+1);
        }
        printf("%s\n", rootName.c_str());
        char *dllName = (char *)beLocalAlloc(rootName.size() + 1);
        strcpy(dllName, rootName.c_str());
        for (DLLExportReader::iterator it = reader.begin(); it != reader.end(); ++it)
        {
            char *name = (char *)beLocalAlloc((*it)->name.size() + 1);
            strcpy(name, (*it)->name.c_str());
            data *p = (data *)beLocalAlloc(sizeof(data));
            p->name = name;
            p->dllName = dllName;
            baseinsert((SYMBOL *)p, _using_hash);
        }
    }
    return TRUE;
}
extern "C" char *_dll_name(char *name)
{
    HASHREC **p = LookupName(name, _using_hash);
    if (p )
    {
        data *rec = (data *)(*p)->p;
        return rec->dllName;
    }
    return NULL;
}
// usually from the command line
extern "C" void _add_global_using(char *str)
{
    while (*str)
    {
        char buf[260], *q = buf;
        while (*str && *str != ';')
            *q ++ = *str++;
        *q = 0;
        if (*str)
            str++;
        // using standard C allocators since this crosses multiple input files
        LIST *lst = (LIST *)calloc(sizeof(LIST), 1), **find = & _global_using_list;
        lst->data = strdup(buf);
        while (*find)
            find = &(*find)->next;
        (*find) = lst;
    }
}
extern "C" void _apply_global_using(void)
{
    _using_("msvcrt");
    _using_("occmsil");
    LIST *lst = _global_using_list;
    while (lst)
    {
        _using_((char *)lst->data);
        lst = lst->next;
    }
}
extern "C" BOOLEAN msil_managed(SYMBOL *sp)
{
    if (sp->linkage2 == lk_msil_rtl)
        return TRUE;
    return !_dll_name(sp->name);
}