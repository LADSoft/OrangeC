#include "afx.h"

#if defined(__CCDL__)
#    define F_OK 0
#else
#    include <io.h>
#    if defined(__TURBOC__)
#        define F_OK 0
#    endif
#endif

/*
//	search file
*/
#ifndef F_OK
#    define F_OK 0
#endif
char* xfind(char* fname, char* rundir)
{
    static char fpath[LINE_MAX];

    assert(fname != NULL);
    assert(rundir != NULL);
    /* where call cl386 */
    make_path(fname, rundir, fpath);
    /*#if DEBUG
        printf( "*** 1.FindFile '%s'\n", fpath );
    #endif*/
    if (access(fpath, F_OK) == 0)
        return fpath;
    /*#if DEBUG
        printf( "*** 2.FindFile in %%PATH%% '%s'\n", fname );
    #endif*/
    /* in PATH */
    _searchenv(fname, "PATH", fpath);
    /*#if DEBUG
        printf( "*** 3.FindFile found '%s'\n", fpath );
    #endif*/
    if (fpath[0])
        return fpath;
    strcpy(fpath, fname);
    ;
    return fpath;
}

/*
//	add path & fname
*/
void make_path(char* fname, char* dir, char* buf)
{
    char dird[2];

    assert(fname != NULL);
    assert(dir != NULL);
    assert(buf != NULL);

    strcpy(buf, dir);
    if (last_char(buf) != DIR_DELIM)
    {
        dird[0] = DIR_DELIM;
        dird[1] = 0;
        strcat(buf, dird);
    }
    strcat(buf, fname);
}