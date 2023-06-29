#include "LinkDebugAux.h"
#include <cstdio>
#include <cstring>

int LinkerColumnsWithNameVirtualTable::id;

LinkerColumnsWithNameVirtualTable::LinkerColumnsWithNameVirtualTable(std::vector<sqlite3_int64>& Data,
                                                                     std::vector<ObjString>& Names, int Columns, bool primary) :
    data(Data), names(Names), columns(Columns), rows(names.size())
{
    char buf[1000];
    sprintf(buf, "lkwnvt%d", id++);
    name = buf;
    strcpy(buf, "CREATE TABLE x(");
    const char* prikey = "";
    if (primary)
        prikey = " PRIMARY KEY";
    for (int i = 0; i < columns - 1; i++)
    {
        sprintf(buf + strlen(buf), "v%d INTEGER%s,", i, prikey);
        prikey = "";
    }
    sprintf(buf + strlen(buf), "v%d VARCHAR(260))", columns - 1);

    declarator = buf;
}
void* LinkerColumnsWithNameVirtualTable::Open()
{
    struct lfcurs* c = new lfcurs;
    c->i = 0;
    return c;
}
int LinkerColumnsWithNameVirtualTable::Close(void* cursor)
{
    delete (struct lfcurs*)cursor;
    return 0;
}
int LinkerColumnsWithNameVirtualTable::Eof(void* cursor)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    return q->i >= rows;
}
int LinkerColumnsWithNameVirtualTable::Filter(void* cursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    q->i = 0;
    return SQLITE_OK;
}
int LinkerColumnsWithNameVirtualTable::Next(void* cursor)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    q->i++;
    return SQLITE_OK;
}
int LinkerColumnsWithNameVirtualTable::Column(void* cursor, sqlite3_context* ctx, int i)
{
    struct lfcurs* pCur = (struct lfcurs*)cursor;
    if (pCur->i >= 0 && pCur->i < rows)
    {
        if (i < columns - 1)
            sqlite3_result_int64(ctx, data[pCur->i * (columns - 1) + i]);
        else
            sqlite3_result_text(ctx, (char*)names[pCur->i].c_str(), names[pCur->i].size(), SQLITE_STATIC);
    }
    return SQLITE_OK;
}
int LinkerColumnsWithNameVirtualTable::RowId(void* cursor, sqlite3_int64* pRowid)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    return q->i;
}
char* LinkerColumnsWithNameVirtualTable::GetVTabDeclarator() { return (char*)declarator.c_str(); }
char* LinkerColumnsWithNameVirtualTable::GetName() { return (char*)name.c_str(); }
