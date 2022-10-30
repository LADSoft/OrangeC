#include "bhtypes.h"
#include "frame.h"
#include "bherr.h"

#ifndef _PARSER_H
#define _PARSER_H

/* Microsoft Network Monitor parser DLL definitions */

#include <pshpack1.h>

#define PROTOCOL_STATUS_RECOGNIZED  0
#define PROTOCOL_STATUS_NOT_RECOGNIZED  1
#define PROTOCOL_STATUS_CLAIMED  2
#define PROTOCOL_STATUS_NEXT_PROTOCOL  3

#define XCHG(x)  MAKEWORD( HIBYTE(x), LOBYTE(x) )
#define DXCHG(x)  MAKELONG( XCHG(HIWORD(x)), XCHG(LOWORD(x)) )
#define LONIBBLE(b) ((BYTE) ((b) & 0x0F))
#define HINIBBLE(b)  ((BYTE) ((b) >> 4))
#define HEX(b)  (HexTable[LONIBBLE(b)])
#define SWAPBYTES(w)  ((w) = XCHG(w))
#define SWAPWORDS(d)  ((d) = DXCHG(d))

#define GetPropertyInstanceDataValue(p,type)  ((type*)GetPropertyInstanceData(p))[0]

#define HOT_SIGNATURE  MAKE_IDENTIFIER('H','O','T','$')
#define HOE_SIGNATURE  MAKE_IDENTIFIER('H','O','E','$')

#define MAX_PROTOCOL_NAME_LEN  16
#define MAX_PROTOCOL_COMMENT_LEN  256

typedef VOID (WINAPIV *FORMAT)(LPPROPERTYINST,...);

typedef union _MACFRAME {
    LPBYTE MacHeader;
    LPETHERNET Ethernet;
    LPTOKENRING Tokenring;
    LPFDDI Fddi;
} MACFRAME, *LPMACFRAME;

typedef struct _HANDOFFENTRY {
#ifdef DEBUGNEVER
    DWORD hoe_sig;
#endif
    DWORD hoe_ProtIdentNumber;
    HPROTOCOL hoe_ProtocolHandle;
    DWORD hoe_ProtocolData;
} HANDOFFENTRY, *LPHANDOFFENTRY;

typedef struct _HANDOFFTABLE {
#ifdef DEBUGNEVER
    DWORD hot_sig;
#endif
    DWORD hot_NumEntries;
    LPHANDOFFENTRY hot_Entries;
} HANDOFFTABLE, *LPHANDOFFTABLE;

typedef enum {
    HANDOFF_VALUE_FORMAT_BASE_UNKNOWN = 0,
    HANDOFF_VALUE_FORMAT_BASE_DECIMAL = 10,
    HANDOFF_VALUE_FORMAT_BASE_HEX = 16
} PF_HANDOFFVALUEFORMATBASE;

typedef struct _PF_HANDOFFENTRY {
    char szIniFile[MAX_PATH];
    char szIniSection[MAX_PATH];
    char szProtocol[MAX_PROTOCOL_NAME_LEN];
    DWORD dwHandOffValue;
    PF_HANDOFFVALUEFORMATBASE ValueFormatBase;
} PF_HANDOFFENTRY, *PPF_HANDOFFENTRY;

typedef struct _PF_HANDOFFSET {
    DWORD nEntries;
    PF_HANDOFFENTRY Entry[];
} PF_HANDOFFSET, *PPF_HANDOFFSET;

typedef struct _PF_FOLLOWENTRY {
    char szProtocol[MAX_PROTOCOL_NAME_LEN];
} PF_FOLLOWENTRY, *PPF_FOLLOWENTRY;

typedef struct _PF_FOLLOWSET {
    DWORD nEntries;
    PF_FOLLOWENTRY Entry[];
} PF_FOLLOWSET, *PPF_FOLLOWSET;

typedef struct _PF_PARSERINFO {
    char szProtocolName[MAX_PROTOCOL_NAME_LEN];
    char szComment[MAX_PROTOCOL_COMMENT_LEN];
    char szHelpFile[MAX_PATH];
    PPF_FOLLOWSET pWhoCanPrecedeMe;
    PPF_FOLLOWSET pWhoCanFollowMe;
    PPF_HANDOFFSET pWhoHandsOffToMe;
    PPF_HANDOFFSET pWhoDoIHandOffTo;
} PF_PARSERINFO, *PPF_PARSERINFO;

typedef struct _PF_PARSERDLLINFO {
    DWORD nParsers;
    PF_PARSERINFO ParserInfo[];
} PF_PARSERDLLINFO, *PPF_PARSERDLLINFO;

INLINE LPVOID GetPropertyInstanceData(LPPROPERTYINST PropertyInst)
{
    if (PropertyInst->DataLength != (WORD)-1)
        return PropertyInst->lpData;
    else
        return (LPVOID)PropertyInst->lpPropertyInstEx->Byte;
}

INLINE DWORD GetPropertyInstanceFrameDataLength(LPPROPERTYINST PropertyInst)
{
    if (PropertyInst->DataLength != (WORD)-1)
        return PropertyInst->DataLength;
    else
        return PropertyInst->lpPropertyInstEx->Length;
}

INLINE DWORD GetPropertyInstanceExDataLength(LPPROPERTYINST PropertyInst)
{
    if (PropertyInst->DataLength == (WORD)-1)
        return PropertyInst->lpPropertyInstEx->Length;
    else
        return (WORD)-1;
}

extern BYTE HexTable[];

extern LPLABELED_WORD WINAPI GetProtocolDescriptionTable(LPDWORD);
extern LPLABELED_WORD WINAPI GetProtocolDescription(DWORD);
extern DWORD WINAPI GetMacHeaderLength(LPVOID,DWORD);
extern DWORD WINAPI GetLLCHeaderLength(LPLLC);
extern DWORD WINAPI GetEtype(LPVOID,DWORD);
extern DWORD WINAPI GetSaps(LPVOID,DWORD);
extern BOOL WINAPI IsLLCPresent(LPVOID,DWORD);
extern VOID WINAPI CanonicalizeHexString(LPSTR,LPSTR,DWORD);
extern void WINAPI CanonHex(UCHAR*,UCHAR*,int,BOOL);
extern DWORD WINAPI ByteToBinary(LPSTR,DWORD);
extern DWORD WINAPI WordToBinary(LPSTR,DWORD);
extern DWORD WINAPI DwordToBinary(LPSTR,DWORD);
extern LPSTR WINAPI AddressToString(LPSTR,BYTE*);
extern LPBYTE WINAPI StringToAddress(BYTE*,LPSTR);
extern LPDWORD WINAPI VarLenSmallIntToDword(LPBYTE,WORD,BOOL,LPDWORD);
extern LPBYTE WINAPI LookupByteSetString(LPSET,BYTE);
extern LPBYTE WINAPI LookupWordSetString(LPSET,WORD);
extern LPBYTE WINAPI LookupDwordSetString(LPSET,DWORD);
extern DWORD WINAPIV FormatByteFlags(LPSTR,DWORD,DWORD);
extern DWORD WINAPIV FormatWordFlags(LPSTR,DWORD,DWORD);
extern DWORD WINAPIV FormatDwordFlags(LPSTR,DWORD,DWORD);
extern LPSTR WINAPIV FormatTimeAsString(SYSTEMTIME*,LPSTR);
extern VOID WINAPIV FormatLabeledByteSetAsFlags(LPPROPERTYINST);
extern VOID WINAPIV FormatLabeledWordSetAsFlags(LPPROPERTYINST);
extern VOID WINAPIV FormatLabeledDwordSetAsFlags(LPPROPERTYINST);
extern VOID WINAPIV FormatPropertyDataAsByte(LPPROPERTYINST,DWORD);
extern VOID WINAPIV FormatPropertyDataAsWord(LPPROPERTYINST,DWORD);
extern VOID WINAPIV FormatPropertyDataAsDword(LPPROPERTYINST,DWORD);
extern VOID WINAPIV FormatLabeledByteSet(LPPROPERTYINST);
extern VOID WINAPIV FormatLabeledWordSet(LPPROPERTYINST);
extern VOID WINAPIV FormatLabeledDwordSet(LPPROPERTYINST);
extern VOID WINAPIV FormatPropertyDataAsInt64(LPPROPERTYINST,DWORD);
extern VOID WINAPIV FormatPropertyDataAsTime(LPPROPERTYINST);
extern VOID WINAPIV FormatPropertyDataAsString(LPPROPERTYINST);
extern VOID WINAPIV FormatPropertyDataAsHexString(LPPROPERTYINST);
extern LPBYTE WINAPI ParserTemporaryLockFrame(HFRAME);
extern LPVOID WINAPI GetCCInstPtr(VOID);
extern VOID WINAPI SetCCInstPtr(LPVOID);
extern LPVOID WINAPI CCHeapAlloc(DWORD,BOOL);
extern LPVOID WINAPI CCHeapReAlloc(LPVOID,DWORD,BOOL);
extern BOOL WINAPI CCHeapFree(LPVOID);
extern SIZE_T WINAPI CCHeapSize(LPVOID);

extern BOOL _cdecl BERGetInteger(LPBYTE,LPBYTE*,LPDWORD,LPDWORD,LPBYTE*);
extern BOOL _cdecl BERGetString(LPBYTE,LPBYTE*,LPDWORD,LPDWORD,LPBYTE*);
extern BOOL _cdecl BERGetHeader(LPBYTE,LPBYTE,LPDWORD,LPDWORD,LPBYTE*);

#include <poppack.h>

#endif /* _PARSER_H */
