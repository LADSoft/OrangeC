#include "..\..\sqlite3\sqlite3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DBVersion 100

#define FALSE 0
#define TRUE 1

static sqlite3 *dbPointer;
static void unregister(void)
{
    if (dbPointer)
    {
#ifdef TEST
        printf("closing...\n");
#endif
        sqlite3_close(dbPointer);
        dbPointer = NULL;
    }
}
static int version_ok;
static int verscallback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = TRUE;
    }
    return 0;
}
static int checkDb(void)
{
    char *zErrMsg  = 0;
    int rv = FALSE;
    int rc = sqlite3_exec(dbPointer, "SELECT value FROM ccPropertyBag WHERE property = \"ccVersion\"", 
                          verscallback, 0, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
        rv = version_ok;
    }
    return rv;
}
int ccDBOpen(char *name)
{
    int rv = FALSE;
    if (sqlite3_open_v2(name, &dbPointer,SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
#ifdef TEST
        printf("checkdb...");
#endif
        rv = checkDb();
    }
    {
#ifdef TEST
        printf("open...");
#endif
    }
    if (rv)
        sqlite3_busy_timeout(dbPointer, 2000);
    atexit(unregister);
    return rv;
}
static char *fullqualify(char *string)
{
    static char buf[265];
    if (string[0] == '\\')
    {
        getcwd(buf, 265);
        strcpy(buf+ 2, string);
        return buf;
    }
    else if (string[1] != ':')
    {
        char *p,  *q = string;
        getcwd(buf, 265);
        p = buf + strlen(buf);
        if (!strncmp(q, ".\\", 2))
            q += 2;
        while (!strncmp(q, "..\\", 3))
        {
            q += 3;
            while (p > buf &&  *p != '\\')
                p--;
        }
        *p++ = '\\';
        strcpy(p, q);
        return buf;
    }
    return string;
}
static int findLine(char *file, char *line, char *name, sqlite3_int64 *line_id, 
                    sqlite3_int64 * struct_id,
                    sqlite_int64 * sym_id, sqlite_int64 *type_id, int * flags)
{
    // this query to attempt to find it in the current file
    static char *query = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags FROM LineNumbers"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    JOIN FileNames ON linenumbers.fileId = fileNames.id"
        "    WHERE Names.Name=?"
        "        AND ? >= startLine AND (? <= endLine OR endLine = 0)"
        "        AND fileNames.name = ?"
        "    ORDER BY lineNumbers.startLine ASC;"
    };
    static char *query2 = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags FROM LineNumbers"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    JOIN FileNames ON linenumbers.mainId = fileNames.id"
        "    WHERE Names.Name=?"
        "        AND LineNumbers.mainId != LineNumbers.fileId AND endLine = 0"
        "        AND fileNames.name = ?"
        "    ORDER BY lineNumbers.startLine ASC;"
    };
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    for (i=0; i < l; i++)
        file[i] = tolower(file[i]);
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, atoi(line));
        sqlite3_bind_int(handle, 3, atoi(line));
        sqlite3_bind_text(handle, 4, file, strlen(file), SQLITE_STATIC);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    *line_id = sqlite3_column_int64(handle, 0);
                    *struct_id = sqlite3_column_int64(handle, 1);
                    *sym_id = sqlite3_column_int64(handle, 2);
                    *type_id = sqlite3_column_int64(handle, 3);
                    *flags = sqlite3_column_int(handle, 4);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        if (rc != SQLITE_OK)
        {
            static sqlite3_stmt *handle2;
            rc = SQLITE_OK;
            if (!handle2)
            {
                rc = sqlite3_prepare_v2(dbPointer, query2, strlen(query2)+1, &handle2, NULL);
            }
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle2);
                sqlite3_bind_text(handle2, 1, name, strlen(name), SQLITE_STATIC);
                sqlite3_bind_text(handle2, 2, file, strlen(file), SQLITE_STATIC);
                while (!done)
                {
                    switch(rc = sqlite3_step(handle2))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                            *line_id = sqlite3_column_int64(handle2, 0);
                            *struct_id = sqlite3_column_int64(handle2, 1);
                            *sym_id = sqlite3_column_int64(handle2, 2);
                            *type_id = sqlite3_column_int64(handle2, 3);
                            *flags = sqlite3_column_int(handle2, 4);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
            }
        }
    }
    return rc == SQLITE_OK;
}
static int LookupName(sqlite_int64 name, char *buf)
{
    static char *query = "SELECT name FROM NAMES WHERE id = ?";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, name);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    strncpy(buf, sqlite3_column_text(handle, 0), 250);
                    buf[250] = 0;
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
    }
    return rc == SQLITE_OK;
}
static int showArgs( sqlite3_int64 line_id)
{
    static char *query = {
       "SELECT a.name, b.name FROM Names a, Names b"
       "    JOIN GlobalArgs ON GlobalArgs.lineid = ?"
       "    WHERE a.id = GlobalArgs.symbolId"
       "        AND b.id = GlobalArgs.typeId"
       "    ORDER BY GlobalArgs.fieldOrder ASC"
    } ;
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, line_id);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    printf("\t\t%s %s,\n", sqlite3_column_text(handle, 1), sqlite3_column_text(handle, 0));
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
    }
    return rc == SQLITE_OK;
}
static int showFields( sqlite3_int64 struct_id)
{
    static char *query = {
       "SELECT a.name, b.name FROM Names a, Names b"
       "    JOIN StructFields ON StructFields.nameId = ?"
       "    WHERE a.id = StructFields.symbolId"
       "        AND b.id = StructFields.typeId"
       "    ORDER BY StructFields.fieldOrder ASC"
    } ;
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, struct_id);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    printf("\t\t%s %s,\n", sqlite3_column_text(handle, 1), sqlite3_column_text(handle, 0));
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
    }
    return rc == SQLITE_OK;
}
static int showFunc(sqlite3_int64 sym_id, sqlite3_int64 type_id, sqlite3_int64 line_id)
{
    char symname[256], typename[256];
    if (LookupName(sym_id, symname))
    {
        if (LookupName(type_id, typename))
        {
            printf("function:\n\t%s %s(\n", typename, symname);
            showArgs(line_id);
            printf("\t);");
            return TRUE;
        }
    }
    return FALSE;
}
static int showStruct(sqlite3_int64 sym_id, sqlite3_int64 type_id, sqlite3_int64 struct_id)
{
    char symname[256], typename[256];
    if (LookupName(sym_id, symname))
    {
        if (LookupName(type_id, typename))
        {
            printf("structure:\n\t%s %s {\n", typename, symname);
            showFields(struct_id);
            printf("\t};");
            return TRUE;
        }
    }
    return FALSE;
}
static int showType(sqlite3_int64 sym_id,  sqlite3_int64 type_id)
{
    char symname[256], typename[256];
    if (LookupName(sym_id, symname))
    {
        if (LookupName(type_id, typename))
        {
            printf("simple type:\n\t%s %s\n", typename, symname);
            return TRUE;
        }
    }
    return FALSE;
}
main(int argc, char *argv[])
{
    if (argc >= 4)
    {
        if (ccDBOpen(argv[1]))
        {
            int flags;
            sqlite3_int64 struct_id, line_id, type_id, sym_id;
            printf("success\n");
            if (findLine(fullqualify(argv[2]), argv[3], argv[4],&line_id, &struct_id, &sym_id, & type_id, &flags))
            {
                int rv;
                if (flags &1)
                {
                    rv = showFunc(sym_id, type_id, line_id);
                }
                else if (struct_id)
                {
                    rv =showStruct(sym_id, type_id, struct_id);
                }
                else
                {
                    rv = showType(sym_id, type_id);
                }
                if (!rv)
                    printf("failed\n");
            }
        }
        else
        {
            printf("failure\n");
        }
    }
    return 0;
}