#include "afx.h"


static char full[ _MAX_PATH ];
static char drive[ _MAX_DRIVE ];
static char dir[ _MAX_DIR ];
static char name[ _MAX_FNAME ];
static char ext[ _MAX_EXT ];


void fn_split( char *fname ) {
    assert( fname != NULL );
    strcpy( full, fname);
    _splitpath( full, drive, dir, name, ext );
}

char *fn_drive(void) { return drive; }
char *fn_dir(void)	 { return dir; }
char *fn_name(void)  { return name; }
char *fn_ext(void)   { return ext; }

char *fn_path(void) {
    _makepath( full, drive, dir, NULL, NULL );
    return full;
}

char *fn_make(char *fext) {
    _makepath( full, drive, dir, name, fext == NULL ? ext : fext );	
    return full;
}

char *fn_temp(char *fext) {
    assert( fext != NULL );
    _makepath( full, NULL, NULL, name, fext );	
    return full;
}
