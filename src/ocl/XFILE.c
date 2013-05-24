#include "afx.h"


XFILE *xfile_new( char *file, int istempfile ) {
    XFILE *xfile;

    assert( file != NULL );
/*
    xfile = (XFILE *)malloc( sizeof(XFILE) );
    assert( xfile != NULL );
    xfile->fname = strdup( file );
    assert( xfile->fname != NULL );
    xfile->temp = istempfile;
*/
    xfile = (XFILE *)malloc( sizeof(XFILE) + strlen(file) );
    assert( xfile != NULL );
    strcpy( xfile->fname, file );
    xfile->temp = istempfile;
    return xfile;
}


void xfile_del( XFILE *xfile ) {
    assert( xfile != NULL );
//	free( xfile->fname );
    free( xfile );
}


int xfile_temp( XFILE *xfile ) {
    assert( xfile != NULL );
    return xfile->temp;
}


char *xfile_fname( XFILE *xfile ) {
    assert( xfile != NULL );
    return xfile->fname;
}