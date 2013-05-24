/* -------------- menus.c ------------- */

#include "dflat.h"

/* --------------------- the main menu --------------------- */
DEFMENU(MainMenu)
    /* --------------- the File popdown menu ----------------*/
    POPDOWN( "~File",  PrepFileMenu, "Read/write/print files. Go to DOS" )
        SELECTION( "E~xit",       ID_EXIT,     ALT_X, 0 )
    ENDPOPDOWN

ENDMENU

/* ------------- the System Menu --------------------- */
DEFMENU(SystemMenu)
    POPDOWN("System Menu", NULL, NULL)
#ifdef INCLUDE_RESTORE
        SELECTION("~Restore",  ID_SYSRESTORE,  0,         0 )
#endif
        SELECTION("~Move",     ID_SYSMOVE,     0,         0 )
        SELECTION("~Size",     ID_SYSSIZE,     0,         0 )
#ifdef INCLUDE_MINIMIZE
        SELECTION("Mi~nimize", ID_SYSMINIMIZE, 0,         0 )
#endif
#ifdef INCLUDE_MAXIMIZE
        SELECTION("Ma~ximize", ID_SYSMAXIMIZE, 0,         0 )
#endif
        SEPARATOR
        SELECTION("~Close",    ID_SYSCLOSE,    CTRL_F4,   0 )
    ENDPOPDOWN
ENDMENU

