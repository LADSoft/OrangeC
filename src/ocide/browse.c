/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"
#include "winconst.h"
#include "sqlite3.h"

#define DBVersion 100

extern int changedProject;
extern int defaultWorkArea;
extern PROJECTITEM *workArea;

int browsing;

static DWINFO **browsebacklist;
static int browseCount, browseMax;
static int version_ok;

void DBClose(sqlite3 *db)
{
    if (db)
        sqlite3_close(db);
}
static int verscallback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = TRUE;
    }
    return 0;
}
sqlite3 *BrowseDBOpen(PROJECTITEM *pj)
{
    char name[MAX_PATH], *p;
    sqlite3 *rv = NULL;
    strcpy(name, pj->realName);
    p = strrchr(name, '.');
    if (!p)
        p = name + strlen(name);
    strcpy(p, ".obr");
    if (sqlite3_open_v2(name, &rv,SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
        char *zErrMsg = NULL;
        version_ok = FALSE;
        if (sqlite3_exec(rv, "SELECT value FROM brPropertyBag WHERE property = \"brVersion\"", 
                              verscallback, 0, &zErrMsg) != SQLITE_OK || !version_ok)
        {
            sqlite3_free(zErrMsg);
            DBClose(rv);
            rv = NULL;
        }
        else
        {
            sqlite3_busy_timeout(rv, 800);
        }
    }
    else
    {
        DBClose(rv);
        rv = NULL;
    }
    
    return rv;
}
static sqlite3 *BrowseOpenDBByHWND(HWND hwnd, PROJECTITEM **returnProj)
{
    char *filname = (char*)SendMessage(hwnd, WM_FILENAME, 0, 0);
    PROJECTITEM *pj = HasFile(workArea, filname);
    while (pj && pj->type != PJ_PROJ)
        pj = pj->parent;
    if (!pj)
    {
        return NULL;
    }    
    *returnProj = pj;
    return BrowseDBOpen(pj);
}
void InsertBrowse(char *filname, int curline)
{
    DWINFO *info;
    char *p;
    if (browseCount >= browseMax)
    {
        if (browseCount >= 20)
        {
            memmove(browsebacklist, browsebacklist + 1, (--browseCount)
                *sizeof(void*));
        }
        else
        {
            browsebacklist = realloc(browsebacklist, (browseMax += 20)
                *sizeof(void*));
            if (!browsebacklist)
            {
                browseMax = 0;
                return;
            }
        }
    }
    info = calloc(sizeof(DWINFO), 1);
    if (!info)
        return;
    strcpy(info->dwName, filname);
    info->dwLineNo = curline;
    info->newFile = FALSE;
    p = strrchr(info->dwName, '\\');
    if (p)
        strcpy(info->dwTitle, p + 1);
    browsebacklist[browseCount++] = info;
}
static int FindLine(sqlite3 *db, char *file, int line, char *name, char *returnedFileName, int *startLine)
{
    // this query to attempt to find it in the current file
    static char *query = {
        "SELECT FileNames.Name, LineNumbers.Startline FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId "
        "    JOIN Names ON Names.id = LineNumbers.symbolId"
        "    WHERE FileNames.name = ?"
        "        AND Names.name = ?"
        "        AND LineNumbers.startLine <= ?"
        "    ORDER BY LineNumbers.startLine DESC;"
    };
    // this query to find it at file scope in any file the current main file includes
    static char *query2 = {
        "SELECT FileNames.name, LineNumbers.Startline FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId"
        "    JOIN Names ON Names.id = LineNumbers.symbolId"
        "    WHERE FileNames.name != ?"
        "        AND Names.name = ?;"
    };
    char fileName[260];
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    sqlite3_stmt *handle;
    for (i=0; i < l; i++)
        fileName[i] = tolower(file[i]);
    fileName[i] = 0;
    rc = sqlite3_prepare_v2(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, fileName, strlen(fileName), SQLITE_STATIC);
        sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
        sqlite3_bind_int(handle, 3, line);
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
                    strcpy(returnedFileName, (char *)sqlite3_column_text(handle, 0));
                    *startLine = sqlite3_column_int(handle, 1);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
        if (rc != SQLITE_OK)
        {
            rc = sqlite3_prepare_v2(db, query2, strlen(query2)+1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_text(handle, 1, fileName, strlen(fileName), SQLITE_STATIC);
                sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
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
                            strcpy(returnedFileName, (char *)sqlite3_column_text(handle, 0));
                            *startLine = sqlite3_column_int(handle, 1);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
            }
            sqlite3_finalize(handle);
        }
    }
    return rc == SQLITE_OK;
}

//-------------------------------------------------------------------------

void BrowseTo(HWND hwnd, char *msg)
{
    static char name[256];
    int ofs;
    if (defaultWorkArea)
        return ;
    if (!browsing)
    {
        if (msg)
        {
            strcpy(name, msg);
            browsing = TRUE;
        }
        else
            browsing = SendMessage(hwnd, WM_WORDUNDERCURSOR, 0, (LPARAM)name);
        if (!PropGetBool(NULL, "BROWSE_INFORMATION") && browsing)
        {
            ExtendedMessageBox("Browse Info Alert", MB_OK, 
                "Browse information not enabled");
            browsing = FALSE;
            return ;
        }
    }
    else
    {
        SendMessage(hwnd, WM_WORDUNDERCURSOR, 0, (LPARAM)name);
    }
    if (browsing)
    {
        sqlite3 *db = NULL;
        DWINFO info;
        CHARRANGE charrange;
        int curline;
        char *filname;
        PROJECTITEM *pj;
        if (msg)
        {
            curline =  - 2;
            filname = "";
        }
        else
        {
            SendDlgItemMessage(hwnd, ID_EDITCHILD, EM_EXGETSEL, (WPARAM)0, 
                (LPARAM) &charrange);
            curline = SendDlgItemMessage(hwnd, ID_EDITCHILD, EM_EXLINEFROMCHAR,
                0, (LPARAM)charrange.cpMin) + 1;
            filname = (char*)SendMessage(hwnd, WM_FILENAME, 0, 0);
        }
        memset(&info, 0, sizeof(info));
        db = BrowseOpenDBByHWND(hwnd, &pj);
        if (!db)
        {
            return ;
        }
        if (FindLine(db, filname, curline, name, info.dwName, &info.dwLineNo))
        {
            char *p = strrchr(info.dwName, '\\');
            if (p)
                strcpy(info.dwTitle, p + 1);
            info.logMRU = FALSE;
            info.newFile = FALSE;
            InsertBrowse(name, curline);
            CreateDrawWindow(&info, TRUE);
        }
        DBClose(db);
    }
    browsing = FALSE;
}


//-------------------------------------------------------------------------

void BrowseBack(void)
{
    if (!browseCount)
        return ;
    browsebacklist[--browseCount]->logMRU = FALSE;
    CreateDrawWindow(browsebacklist[browseCount], TRUE);
    free(browsebacklist[browseCount]);
}
