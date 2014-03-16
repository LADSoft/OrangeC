#include "afx.h"


static struct {
    char opt;
    int flag;	
} options[] = {
    { 'e', OPT_ERRFILE  },
    { 'i', OPT_CPPFILE  },
    { 'l', OPT_LSTFILE  },
    { 'A', OPT_EXTEND   },
    { 'm', OPT_MAPFILE  },
    { 'n', OPT_NODEFLIB },
    { 'v', OPT_DEBUG	},
    { 'k', OPT_KEEPGEN 	},
    { 'K', OPT_KEEPRSP 	},
    { 'X', OPT_EXCEPTION},
    { 'q', OPT_QUIET },
    { '9', OPT_C99 }
};


static struct {
    char *name;
    int flag;
} assemblers[] = {
    { "oasm",	OPT_NASM },
//	{ "tasm",	OPT_TASM },
//	{ "tasmx",	OPT_TASMX },
//	{ "tasm32", OPT_TASM32 },
//	{ "masm",	OPT_MASM },
//	{ "ml",		OPT_ML },
//	{ "wasm",	OPT_WASM },
//	{ "lasm", 	OPT_LASM },
//	{ "lasm32", OPT_LASM32 }
};


static struct {
    char *name;
    int flag;
} linkers[] = {
    { "olink", OPT_OLINK },
//	{ "valx",	OPT_VALX },
//	{ "tlink",	OPT_TLINK },
//	{ "link",	OPT_LINK },
//	{ "wlink", 	OPT_WLINK }
};


static struct {
    char *name;
    int flag;
} dosexs[] = {
    { "pmode",	OPT_PMODE },
    { "pmodew",	OPT_PMODEW },
    { "dos4g",	OPT_DOS4G },
    { "wdosx", 	OPT_WDOSX },
    { "dos32a",	OPT_DOS32A },
    { "zrdx",	OPT_ZRDX },
    { "causeway", OPT_CAUSEWAY },
    { "hx", OPT_HXDOS },
    { "whx", OPT_WHXDOS },
    { "d3x", OPT_D3X },
    { "w32con", OPT_W32CON },
    { "w32gui", OPT_W32GUI },
    { "w32dll", OPT_W32DLL }
};


static int make_opt = OPT_COMPILE | OPT_ASSEMBLY | OPT_LINKING;	
static int opts = OPT_ERRFILE;	/* -e+ -A+ */
static int assembler = OPT_NASM;		   
static int linker = OPT_OLINK;
static int dosex = OPT_HXDOS;

int get_assembler(void) { return assembler; }
int get_linker(void) { return linker; }
int get_dosex(void) { return dosex; }


static void set_option( int what, int value ) {
    if ( value ) {
        opts |= what;
    } else {
        opts &= ~what;
    }
}

int get_option( int what ) {
    return (opts & what);
}

int get_make_opt( int what ) {
    return (make_opt & what);
}


static char cc_opt[ LINE_MAX ];
static char inc_path[ LINE_MAX ];
static char lib_path[ LINE_MAX ];


char *get_cc_opt(void) { return cc_opt; }
char *get_inc_path(void) { return inc_path; }
char *get_lib_path(void) { return lib_path; }

char *get_syslib_path(void) {
    static char lib[LINE_MAX];
    int i;
    for (i = 0; lib_path[i] || lib_path[i] == ';'; i++)
        lib[i] = lib_path[i];
    lib[i] = 0;
    return lib;
}

void option( char *opt ) {
    int i;

    assert( opt != NULL );
    for ( i = 0; i < sizeof(options)/sizeof(options[0]); i++) {
        if ( opt[1] == options[i].opt ) {
            if ( opt[2] == '\0' || (opt[2] == '+' && opt[3] == '\0') ) {
                set_option( options[i].flag, 1 );
                return;
            } else if ( opt[2] == '-' && opt[3] == '\0' ) {
                set_option( options[i].flag, 0 );
                return;
            }
        }
    }

    for ( i = 0; i < sizeof(assemblers)/sizeof(assemblers[0]); i++) {
        if ( stricmp( opt+1, assemblers[i].name ) == 0 ) {
            assembler = assemblers[i].flag;
            return;
        }
    }
    
    for ( i = 0; i < sizeof(linkers)/sizeof(linkers[0]); i++) {
        if ( stricmp( opt+1, linkers[i].name ) == 0 ) {
            linker = linkers[i].flag;
            return;
        }
    }

    for ( i = 0; i < sizeof(dosexs)/sizeof(dosexs[0]); i++) {
        if ( stricmp( opt+1, dosexs[i].name ) == 0 ) {
            dosex = dosexs[i].flag;
            return;
        }
    }

    if ( opt[1] == 'e' && (opt[2] == '=' || opt[2] == '#' ) ) {
        set_exe_name( opt+3 );
        return;
    }

    if ( opt[1] == 'a' && opt[2] == '\0' ) {
        make_opt = OPT_ASSEMBLY;
        return;
    }

    if ( opt[1] == 'c' && opt[2] == '\0' ) {
        make_opt = OPT_ASSEMBLY | OPT_COMPILE;
        return;
    }

    if ( (opt[1] == '?' || opt[1] == 'h') && opt[2] == '\0' )
        help();

    if (   opt[1] == 'D' 
        || opt[1] == 'w' 
        || opt[1] == 'C' 
        || opt[1] == 'E'
        || opt[1] == 'O'
        ) {
        add2path( cc_opt, opt, CC_DELIM /* ' ' */ );
        return;
    }

    if ( opt[1] == 'I' ) {
        add2path( inc_path, opt+2, PATH_DELIM /* ';' */);
        return;
    }

    if ( opt[1] == 'L' ) {
        add2path( lib_path, opt+2, PATH_DELIM /* ';' */);
        return;
    }

    fprintf( stderr, "Unknown option : '%s'\n", opt );
}