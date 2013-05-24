/* ----------- dialogs.c --------------- */

#include "dflat.h"
#include "idconst.h"
/* -------------- the File Open dialog box --------------- */
DIALOGBOX( dosDialog )
    DB_TITLE(        "Install Configuration",    -1,-1,8,70)
    CONTROL(TEXT,    "~Install Path:", 3, 1, 1, 22, ID_INSTALLPATH)
    CONTROL(EDITBOX, NULL,           25, 1, 1,40, ID_INSTALLPATH)
    CONTROL(TEXT,    "~Modify autoexec.bat:", 3, 2, 1, 22, ID_MODIFYAUTO)
	CONTROL(CHECKBOX,  NULL,                 25, 2, 1,  3, ID_MODIFYAUTO)
    CONTROL(BUTTON,  " ~Install",    20, 4, 1, 8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ",    42,4, 1, 8, ID_CANCEL)
ENDDB

DIALOGBOX( NTDialog )
    DB_TITLE(        "Install Configuration",    -1,-1,8,70)
    CONTROL(TEXT,    "~Install Path:", 3, 1, 1, 22, ID_INSTALLPATH)
    CONTROL(EDITBOX, NULL,           25, 1, 1,40, ID_INSTALLPATH)
    CONTROL(TEXT,    "~Modify Registry Path:", 3, 2, 1, 22, ID_MODIFYAUTO)
	CONTROL(CHECKBOX,  NULL,                 25, 2, 1,  3, ID_MODIFYAUTO)
    CONTROL(BUTTON,  " ~Install",    20, 4, 1, 8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ",    42,4, 1, 8, ID_CANCEL)
ENDDB
