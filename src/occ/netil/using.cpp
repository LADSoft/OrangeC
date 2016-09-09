#include "compiler.h"
#include "dllexportreader.h"
#include "CmdFiles.h"
#include <string>
struct data
{
    char *name;
    char *dllName;
};
#define HASHLEN 2048
static HASHREC *_using_hash[HASHLEN];
static LIST *_global_using_list;
extern "C" void _using_init()
{
}
static int hashVal(char *name)
{
    unsigned hash = 0;
    while (*name)
    {
        hash * 131;
        hash = (hash << 7 ) + (hash << 1) + hash + (*name++);
    }
    return hash % HASHLEN;
}
static void hashInsert(struct data *names)
{
    int hash = hashVal(names->name);
    HASHREC *hr  = _using_hash[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, names->name))
            return;
        hr = hr->next;
    }
    
    hr = (HASHREC *)calloc(1, sizeof(HASHREC));
    hr->p = (HASHREC::_hrintern_ *)names;
    hr->next = _using_hash[hash];
    _using_hash[hash] = hr;
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
        char *dllName = (char *)calloc(1, rootName.size() + 1);
        strcpy(dllName, rootName.c_str());
        for (DLLExportReader::iterator it = reader.begin(); it != reader.end(); ++it)
        {
            char *name = (char *)calloc(1, (*it)->name.size() + 1);
            strcpy(name, (*it)->name.c_str());
            data *p = (data *)calloc(1, sizeof(data));
            p->name = name;
            p->dllName = dllName;
            hashInsert(p);
        }
    }
    return TRUE;
}
extern "C" char *_dll_name(char *name)
{
    int hash = hashVal(name);
    HASHREC *hr  = _using_hash[hash];
    while (hr)
    {
        if (!strcmp(hr->p->name, name))
            return ((struct data *)hr->p)->dllName;
        hr = hr->next;
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