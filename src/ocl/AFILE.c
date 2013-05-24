#include "afx.h"
/*
#define SIZE_BLOCK 64

typedef struct {
    int size;
    int count;
    XFILE **array;
} AFILE;

*/
static void afile_resize( AFILE *afile, int newsize ) {
    XFILE **arr;
    assert( afile != NULL );
    if ( afile->size >= newsize )
        return;
    while ( afile->size < newsize )
        afile->size += SIZE_BLOCK;
    arr = (XFILE **)realloc( afile->array, sizeof(XFILE*) * afile->size );
    assert( arr != NULL );
    afile->array = arr;
}

static int afile_found( AFILE *afile, char *file ) {
    int i;
    assert( afile != NULL );
    assert( file != NULL );
    for ( i = 0; i < afile->count; i++ )
        if ( stricmp( afile->array[i]->fname, file ) == 0 )
            return 1;
    return 0;
}

int afile_add( AFILE *afile, char *fname, int temp ) {
    XFILE *xfile;
    
    assert( afile != NULL );
    assert( fname != NULL );
    if ( afile_found( afile, fname ) )
        return 0;
    xfile = xfile_new( fname, temp );
    assert( xfile != NULL );
    afile_resize( afile, afile->count+1 );
    afile->array[ afile->count ] = xfile;
    afile->count++;	
    return 1;
}


int afile_cnt( AFILE *afile ) {
    assert( afile != NULL );
    return afile->count;
}


char *afile_fname( AFILE *afile, int index ) {
    assert( afile != NULL );
    assert( index >= 0 && index < afile->count );
    return xfile_fname( afile->array[index] );
}


void afile_deltemp( AFILE *afile ) {
    int i;

    assert( afile != NULL );
    for( i = 0; i < afile->count; i++)
        if ( afile->array[i]->temp )
            unlink( afile->array[i]->fname );
}

void afile_del( AFILE *afile ) {
    int i;

    assert( afile != NULL );
    for( i = 0; i < afile->count; i++)
        if ( afile->array[i] )
            xfile_del( afile->array[i] );
    afile->count = afile->size = 0;
}
