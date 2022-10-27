#include <nmsupp.h>

#ifndef _INILIB_H
#define _INILIB_H

/* Microsoft Network Monitor definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define INI_PATH_LENGTH  256
#define MAX_HANDOFF_ENTRY_LENGTH  80
#define MAX_PROTOCOL_NAME  40
#define NUMALLOCENTRIES  10
#define RAW_INI_STR_LEN  200

#define PARSERS_SUBDIR  "PARSERS"
#define INI_EXTENSION  "INI"
#define BASE10_FORMAT_STR  "%ld=%s %ld"
#define BASE16_FORMAT_STR  "%lx=%s %lx"

extern LPSTR _cdecl BuildINIPath(char*,char*);
extern DWORD WINAPI CreateHandoffTable(LPSTR,LPSTR,LPHANDOFFTABLE*,DWORD,DWORD);
extern HPROTOCOL WINAPI GetProtocolFromTable(LPHANDOFFTABLE,DWORD,PDWORD_PTR);
extern VOID WINAPI DestroyHandoffTable(LPHANDOFFTABLE);
extern BOOLEAN WINAPI IsRawIPXEnabled(LPSTR,LPSTR,LPSTR);

#ifdef __cplusplus
}
#endif

#endif /* _INILIB_H */
