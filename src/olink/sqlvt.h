#ifndef SQLVT_H
#define SQLVT_H
#include "../sqlite3/sqlite3.h"
#include <vector>
#include <string>
class SQL3VirtualTable;
struct sql3_vt_vtab
{
    sqlite3_vtab base; /* Base class */
    SQL3VirtualTable* table;
};

/* A intarray cursor object */
struct sql3_vt_cursor
{
    sqlite3_vtab_cursor base; /* Base class */
    SQL3VirtualTable* table;
    void* userCursor;
};
class SQL3VirtualTable
{
  public:
    SQL3VirtualTable() : running(false) {}
    virtual ~SQL3VirtualTable() { Stop(); }

    int Start(sqlite3* db);
    int Stop();
    int InsertIntoFrom(const char* tableName);

    virtual void* Open() = 0;
    virtual int Close(void* cursor) = 0;
    virtual int Eof(void* cursor) = 0;
    virtual int Filter(void* cursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv) = 0;
    virtual int Next(void* cursor) = 0;
    virtual int Column(void* cursor, sqlite3_context* ctx, int i) = 0;
    virtual int RowId(void* cursor, sqlite3_int64* pRowid) = 0;
    virtual char* GetVTabDeclarator() = 0;
    virtual char* GetName() = 0;
    virtual sqlite3_module* GetModule() = 0;

    static int thunkCreate(sqlite3* db, void* pAux, int argc, const char* const* argv, sqlite3_vtab** ppVtab, char** pzErr);
    static int thunkBestIndex(sqlite3_vtab* tab, sqlite3_index_info* pIdxInfo);
    static int thunkDestroy(sqlite3_vtab* p);
    static int thunkOpen(sqlite3_vtab* pVTab, sqlite3_vtab_cursor** ppCursor);
    static int thunkClose(sqlite3_vtab_cursor* cur);
    static int thunkFilter(sqlite3_vtab_cursor* pVtabCursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv);
    static int thunkNext(sqlite3_vtab_cursor* cur);
    static int thunkEof(sqlite3_vtab_cursor* cur);
    static int thunkColumn(sqlite3_vtab_cursor* cur, sqlite3_context* ctx, int i);
    static int thunkRowid(sqlite3_vtab_cursor* cur, sqlite_int64* pRowid);

  protected:
    int Exec(const char* str);
    int iCreate(int argc, const char* const* argv, sqlite3_vtab** ppVtab, char** pzErr);
    int iDestroy(sql3_vt_vtab* vt);
    int iBestIndex(sqlite3_index_info* pIdxInfo);
    int iOpen(sqlite3_vtab_cursor** ppCursor);
    int iClose(struct sql3_vt_cursor* curs);
    int iFilter(void* userCursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv);
    int iNext(void* userCursor);
    int iEof(void* userCursor);
    int iColumn(void* userCursor, sqlite3_context* ctx, int i);
    int iRowid(void* userCursor, sqlite_int64* pRowid);

  private:
    sqlite3* db;
    bool running;
};

class SQL3ReadOnlyVirtualTable : public SQL3VirtualTable
{
  public:
    virtual ~SQL3ReadOnlyVirtualTable() {}
    virtual void* Open() = 0;
    virtual int Close(void* cursor) = 0;
    virtual int Eof(void* cursor) = 0;
    virtual int Filter(void* cursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv) = 0;
    virtual int Next(void* cursor) = 0;
    virtual int Column(void* cursor, sqlite3_context* ctx, int i) = 0;
    virtual int RowId(void* cursor, sqlite3_int64* pRowid) = 0;
    virtual char* GetVTabDeclarator() = 0;
    virtual char* GetName() = 0;
    virtual sqlite3_module* GetModule() { return &Module; }

  private:
    static sqlite3_module Module;
};

class IntegerColumnsVirtualTable : public SQL3ReadOnlyVirtualTable
{
    struct lfcurs
    {
        sqlite_int64 i;
    };

  public:
    IntegerColumnsVirtualTable(std::vector<sqlite3_int64>& Data, int Columns, bool primary = false);
    virtual ~IntegerColumnsVirtualTable() {}
    virtual void* Open();
    virtual int Close(void* cursor);
    virtual int Eof(void* cursor);
    virtual int Filter(void* cursor, int idxNum, const char* idxStr, int argc, sqlite3_value** argv);
    virtual int Next(void* cursor);
    virtual int Column(void* cursor, sqlite3_context* ctx, int i);
    virtual int RowId(void* cursor, sqlite3_int64* pRowid);
    virtual char* GetVTabDeclarator();
    virtual char* GetName();

  private:
    std::string name, declarator;
    std::vector<sqlite3_int64>& data;
    int columns;
    int rows;
    static int id;
};

#endif