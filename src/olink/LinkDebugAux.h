#include "sqlvt.h"
#include "ObjTypes.h"

class LinkerColumnsWithNameVirtualTable :public SQL3ReadOnlyVirtualTable
{
    struct lfcurs
    {
        sqlite_int64 i;
    };
public:
    LinkerColumnsWithNameVirtualTable(std::vector<sqlite3_int64> &Data, std::vector<ObjString *> &Names, int Columns, bool primary = false);
    virtual ~LinkerColumnsWithNameVirtualTable() { }
    virtual void *Open();
    virtual int Close(void *cursor);
    virtual int Eof(void *cursor);
    virtual int Filter(void *cursor, int idxNum, const char *idxStr,
                int argc, sqlite3_value **argv);
    virtual int Next(void *cursor);
    virtual int Column(void *cursor, sqlite3_context *ctx, int i);
    virtual int RowId(void *cursor, sqlite3_int64 *pRowid);
    virtual char *GetVTabDeclarator();
    virtual char *GetName();
private:
    std::string name, declarator;
    std::vector<sqlite3_int64> &data;
    std::vector<ObjString *> &names;
    int columns;
    int rows;
    static int id;
};
