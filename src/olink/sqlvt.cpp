
#include "../sqlite3/sqlite3.h"
#include "sqlvt.h"
#include <cstdio>
#include <cstring>

int IntegerColumnsVirtualTable::id;
sqlite3_module SQL3ReadOnlyVirtualTable::Module = {
    0,                                /* iVersion */
    SQL3VirtualTable::thunkCreate,    /* xCreate - create a new virtual table */
    SQL3VirtualTable::thunkCreate,    /* xConnect - connect to an existing vtab */
    SQL3VirtualTable::thunkBestIndex, /* xBestIndex - find the best query index */
    SQL3VirtualTable::thunkDestroy,   /* xDisconnect - disconnect a vtab */
    SQL3VirtualTable::thunkDestroy,   /* xDestroy - destroy a vtab */
    SQL3VirtualTable::thunkOpen,      /* xOpen - open a cursor */
    SQL3VirtualTable::thunkClose,     /* xClose - close a cursor */
    SQL3VirtualTable::thunkFilter,    /* xFilter - configure scan constraints */
    SQL3VirtualTable::thunkNext,      /* xNext - advance a cursor */
    SQL3VirtualTable::thunkEof,       /* xEof */
    SQL3VirtualTable::thunkColumn,    /* xColumn - read data */
    SQL3VirtualTable::thunkRowid,     /* xRowid - read data */
    0,                                /* xUpdate */
    0,                                /* xBegin */
    0,                                /* xSync */
    0,                                /* xCommit */
    0,                                /* xRollback */
    0,                                /* xFindMethod */
    0,                                /* xRename */
};
int SQL3VirtualTable::Exec(const char* str)
{
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, str, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        rc = sqlite3_exec(db, "ROLLBACK; ", 0, 0, &zErrMsg);
        if (rc != SQLITE_OK)
            sqlite3_free(zErrMsg);
    }
    return rc;
}
int SQL3VirtualTable::InsertIntoFrom(const char* str)
{
    char* zSql;
    zSql = sqlite3_mprintf("BEGIN; INSERT INTO %Q SELECT * FROM TEMP.%Q; COMMIT;", str, GetName());
    int rc = Exec(zSql);
    sqlite3_free(zSql);
    return rc;
}
int SQL3VirtualTable::thunkCreate(sqlite3* dbx, void* pAux, int argc, const char* const* argv, sqlite3_vtab** ppVtab, char** pzErr)
{
    SQL3VirtualTable* table = (SQL3VirtualTable*)pAux;
    return table->iCreate(argc, argv, ppVtab, pzErr);
}
int SQL3VirtualTable::thunkBestIndex(sqlite3_vtab* tab, sqlite3_index_info* pIdxInfo)
{
    struct sql3_vt_vtab* table = (struct sql3_vt_vtab*)tab;
    return table->table->iBestIndex(pIdxInfo);
}
int SQL3VirtualTable::thunkDestroy(sqlite3_vtab* tab)
{
    struct sql3_vt_vtab* table = (struct sql3_vt_vtab*)tab;
    return table->table->iDestroy(table);
}
int SQL3VirtualTable::thunkOpen(sqlite3_vtab* tab, sqlite3_vtab_cursor** ppCursor)
{
    struct sql3_vt_vtab* table = (struct sql3_vt_vtab*)tab;
    return table->table->iOpen(ppCursor);
}
int SQL3VirtualTable::thunkClose(sqlite3_vtab_cursor* cur)
{
    struct sql3_vt_cursor* cursor = (struct sql3_vt_cursor*)cur;
    return cursor->table->iClose(cursor);
}
int SQL3VirtualTable::thunkFilter(sqlite3_vtab_cursor* cur, int idxNum, const char* idxStr, int argc, sqlite3_value** argv)
{
    struct sql3_vt_cursor* cursor = (struct sql3_vt_cursor*)cur;
    return cursor->table->iFilter(cursor->userCursor, idxNum, idxStr, argc, argv);
}
int SQL3VirtualTable::thunkNext(sqlite3_vtab_cursor* cur)
{
    struct sql3_vt_cursor* cursor = (struct sql3_vt_cursor*)cur;
    return cursor->table->iNext(cursor->userCursor);
}
int SQL3VirtualTable::thunkEof(sqlite3_vtab_cursor* cur)
{
    struct sql3_vt_cursor* cursor = (struct sql3_vt_cursor*)cur;
    return cursor->table->iEof(cursor->userCursor);
}
int SQL3VirtualTable::thunkColumn(sqlite3_vtab_cursor* cur, sqlite3_context* ctx, int i)
{
    struct sql3_vt_cursor* cursor = (struct sql3_vt_cursor*)cur;
    return cursor->table->iColumn(cursor->userCursor, ctx, i);
}
int SQL3VirtualTable::thunkRowid(sqlite3_vtab_cursor* cur, sqlite_int64* pRowid)
{
    struct sql3_vt_cursor* cursor = (struct sql3_vt_cursor*)cur;
    return cursor->table->RowId(cursor->userCursor, pRowid);
}
int SQL3VirtualTable::Start(sqlite3* dbx)
{
    int rc = SQLITE_OK;
    if (!running)
    {
        db = dbx;
        rc = sqlite3_create_module_v2(dbx, GetName(), GetModule(), this, nullptr);
        if (rc == SQLITE_OK)
        {
            char* zSql;
            zSql = sqlite3_mprintf("CREATE VIRTUAL TABLE TEMP.%Q USING %Q", GetName(), GetName());
            rc = Exec(zSql);
            sqlite3_free(zSql);
        }
        if (rc == SQLITE_OK)
            running = true;
    }
    return rc;
}
int SQL3VirtualTable::Stop()
{
    if (running)
    {
        char* zSql;
        zSql = sqlite3_mprintf("DROP TABLE TEMP.%Q", GetName());
        int rc = Exec(zSql);
        sqlite3_free(zSql);
        running = false;
    }
    return SQLITE_OK;
}

int SQL3VirtualTable::iCreate(int argc, const char* const* argv, sqlite3_vtab** ppVtab, char** pzErr)
{
    int rc = SQLITE_NOMEM;
    sql3_vt_vtab* pVtab = (sql3_vt_vtab*)sqlite3_malloc(sizeof(sql3_vt_vtab));

    if (pVtab)
    {
        memset(pVtab, 0, sizeof(*pVtab));
        pVtab->table = this;
        rc = sqlite3_declare_vtab(db, GetVTabDeclarator());
    }
    *ppVtab = (sqlite3_vtab*)pVtab;
    return rc;
}
int SQL3VirtualTable::iDestroy(sql3_vt_vtab* vt)
{
    sqlite3_free(vt);
    return SQLITE_OK;
}
int SQL3VirtualTable::iBestIndex(sqlite3_index_info* pIdxInfo) { return SQLITE_OK; }
int SQL3VirtualTable::iOpen(sqlite3_vtab_cursor** ppCursor)
{
    int rc = SQLITE_NOMEM;
    sql3_vt_cursor* pCur;
    pCur = (sql3_vt_cursor*)sqlite3_malloc(sizeof(*pCur));
    if (pCur)
    {
        memset(pCur, 0, sizeof(*pCur));
        pCur->userCursor = Open();
        pCur->table = this;
        if (pCur->userCursor)
        {
            *ppCursor = (sqlite3_vtab_cursor*)pCur;
            rc = SQLITE_OK;
        }
        else
        {
            sqlite3_free(pCur);
        }
    }
    return rc;
}
int SQL3VirtualTable::iClose(struct sql3_vt_cursor* curs)
{
    int rc = Close(curs->userCursor);
    sqlite3_free(curs);
    return rc;
}
int SQL3VirtualTable::iFilter(void* userCursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv)
{
    return Filter(userCursor, idxNum, idxStr, argc, argv);
}
int SQL3VirtualTable::iNext(void* userCursor) { return Next(userCursor); }
int SQL3VirtualTable::iEof(void* userCursor) { return Eof(userCursor); }
int SQL3VirtualTable::iColumn(void* userCursor, sqlite3_context* ctx, int i) { return Column(userCursor, ctx, i); }
int SQL3VirtualTable::iRowid(void* userCursor, sqlite_int64* pRowid) { return RowId(userCursor, pRowid); }
IntegerColumnsVirtualTable::IntegerColumnsVirtualTable(std::vector<sqlite3_int64>& Data, int Columns, bool primary) :
    data(Data),
    columns(Columns),
    rows(Data.size() / Columns)
{
    char buf[1000];
    sprintf(buf, "icvt%d", id++);
    name = buf;
    strcpy(buf, "CREATE TABLE x(");
    int ch = ',';
    const char* prikey = "";
    if (primary)
        prikey = " PRIMARY KEY";
    for (int i = 0; i < columns; i++)
    {
        if (i == columns - 1)
            ch = ' ';
        sprintf(buf + strlen(buf), "v%d value INTEGER%s%c", i, prikey, ch);
        prikey = "";
    }

    strcat(buf, ")");
    declarator = buf;
}
void* IntegerColumnsVirtualTable::Open()
{
    struct lfcurs* c = new lfcurs;
    c->i = 0;
    return c;
}
int IntegerColumnsVirtualTable::Close(void* cursor)
{
    delete (struct lfcurs*)cursor;
    return 0;
}
int IntegerColumnsVirtualTable::Eof(void* cursor)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    return q->i >= rows;
}
int IntegerColumnsVirtualTable::Filter(void* cursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    q->i = 0;
    return SQLITE_OK;
}
int IntegerColumnsVirtualTable::Next(void* cursor)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    q->i++;
    return SQLITE_OK;
}
int IntegerColumnsVirtualTable::Column(void* cursor, sqlite3_context* ctx, int i)
{
    struct lfcurs* pCur = (struct lfcurs*)cursor;
    if (pCur->i >= 0 && pCur->i < rows)
        sqlite3_result_int64(ctx, data[pCur->i * columns + i]);
    return SQLITE_OK;
}
int IntegerColumnsVirtualTable::RowId(void* cursor, sqlite3_int64* pRowid)
{
    struct lfcurs* q = (struct lfcurs*)cursor;
    return q->i;
}
char* IntegerColumnsVirtualTable::GetVTabDeclarator() { return (char*)declarator.c_str(); }
char* IntegerColumnsVirtualTable::GetName() { return (char*)name.c_str(); }
