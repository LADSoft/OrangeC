#ifndef _NTQUERY_H
#define _NTQUERY_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Query API definitions */

#if defined(__cplusplus)
extern "C" {
#endif

#define CI_VERSION_WDS30  0x102
#define CI_VERSION_WDS40  0x109
#define CI_VERSION_WIN70  0x700

#define CINULLCATALOG L"::_noindex_::"
#define CIADMIN L"::_nodocstore_::"

#define CLSID_INDEX_SERVER_DSO  {0xF9AE8980,0x7E52,0x11d0,{0x89,0x64,0x00,0xC0,0x4F,0xD6,0x11,0xD7}}
#define PSGUID_STORAGE  {0xb725f130,0x47ef,0x101a,{0xa5,0xf1,0x02,0x60,0x8c,0x9e,0xeb,0xac}}
#define DBPROPSET_FSCIFRMWRK_EXT  {0xA9BD1526,0x6A80,0x11D0,{0x8C,0x9D,0x00,0x20,0xAF,0x1D,0x74,0x0E}}
#define DBPROPSET_QUERYEXT  {0xA7AC77ED,0xF8D7,0x11CE,{0xA7,0x98,0x00,0x20,0xF8,0x00,0x80,0x25}}
#define DBPROPSET_CIFRMWRKCORE_EXT  {0xafafaca5,0xb5d1,0x11d0,{0x8c,0x62,0x00,0xc0,0x4f,0xc2,0xdb,0x8d}}
#define DBPROPSET_MSIDXS_ROWSETEXT  {0xaa6ee6b0,0xe828,0x11d0,{0xb2,0x3e,0x00,0xaa,0x00,0x47,0xfc,0x01}}

#define PID_STG_DIRECTORY  ((PROPID) 0x00000002)
#define PID_STG_CLASSID  ((PROPID) 0x00000003)
#define PID_STG_STORAGETYPE  ((PROPID) 0x00000004)
#define PID_STG_VOLUME_ID  ((PROPID) 0x00000005)
#define PID_STG_PARENT_WORKID  ((PROPID) 0x00000006)
#define PID_STG_SECONDARYSTORE  ((PROPID) 0x00000007)
#define PID_STG_FILEINDEX  ((PROPID) 0x00000008)
#define PID_STG_LASTCHANGEUSN  ((PROPID) 0x00000009)
#define PID_STG_NAME  ((PROPID) 0x0000000a)
#define PID_STG_PATH  ((PROPID) 0x0000000b)
#define PID_STG_SIZE  ((PROPID) 0x0000000c)
#define PID_STG_ATTRIBUTES  ((PROPID) 0x0000000d)
#define PID_STG_WRITETIME  ((PROPID) 0x0000000e)
#define PID_STG_CREATETIME  ((PROPID) 0x0000000f)
#define PID_STG_ACCESSTIME  ((PROPID) 0x00000010)
#define PID_STG_CHANGETIME  ((PROPID) 0x00000011)
#define PID_STG_ALLOCSIZE  ((PROPID) 0x00000012)
#define PID_STG_CONTENTS  ((PROPID) 0x00000013)
#define PID_STG_SHORTNAME  ((PROPID) 0x00000014)
#define PID_STG_MAX  PID_STG_SHORTNAME
#define CSTORAGEPROPERTY  0x15

#define DBPROP_CI_CATALOG_NAME  2
#define DBPROP_CI_INCLUDE_SCOPES  3
#define DBPROP_CI_DEPTHS  4
#define DBPROP_CI_SCOPE_FLAGS  4
#define DBPROP_CI_EXCLUDE_SCOPES  5
#define DBPROP_CI_SECURITY_ID  6
#define DBPROP_CI_QUERY_TYPE  7

#define DBPROP_USECONTENTINDEX  2
#define DBPROP_DEFERNONINDEXEDTRIMMING  3
#define DBPROP_USEEXTENDEDDBTYPES  4

#define DBPROP_MACHINE  2
#define DBPROP_CLIENT_CLSID 3

#define MSIDXSPROP_ROWSETQUERYSTATUS  2
#define MSIDXSPROP_COMMAND_LOCALE_STRING  3
#define MSIDXSPROP_QUERY_RESTRICTION  4

#define STAT_BUSY  (0)
#define STAT_ERROR  (0x1)
#define STAT_DONE  (0x2)
#define STAT_REFRESH  (0x3)
#define QUERY_FILL_STATUS(x)  ((x) & 0x7)

#define STAT_PARTIAL_SCOPE  (0x8)
#define STAT_NOISE_WORDS  (0x10)
#define STAT_CONTENT_OUT_OF_DATE  (0x20)
#define STAT_REFRESH_INCOMPLETE  (0x40)
#define STAT_CONTENT_QUERY_INCOMPLETE  (0x80)
#define STAT_TIME_LIMIT_EXCEEDED  (0x100)
#define STAT_SHARING_VIOLATION  (0x200)
#define QUERY_RELIABILITY_STATUS(x)  ((x) & 0xFFF8)

#define QUERY_SHALLOW  0
#define QUERY_DEEP  1
#define QUERY_PHYSICAL_PATH  0
#define QUERY_VIRTUAL_PATH  2

#define PROPID_QUERY_WORKID  5
#define PROPID_QUERY_UNFILTERED  7
#define PROPID_QUERY_VIRTUALPATH  9
#define PROPID_QUERY_LASTSEENTIME 10

#define CICAT_STOPPED  0x1
#define CICAT_READONLY  0x2
#define CICAT_WRITABLE  0x4
#define CICAT_NO_QUERY  0x8
#define CICAT_GET_STATE  0x10

STDAPI SetCatalogState(WCHAR const *pwcsCat, WCHAR const *pwcsMachine, DWORD dwNewState, DWORD * pdwOldState);

#define CI_STATE_SHADOW_MERGE  0x0001
#define CI_STATE_MASTER_MERGE  0x0002
#define CI_STATE_CONTENT_SCAN_REQUIRED 0x0004
#define CI_STATE_ANNEALING_MERGE  0x0008
#define CI_STATE_SCANNING  0x0010
#define CI_STATE_RECOVERING  0x0020
#define CI_STATE_INDEX_MIGRATION_MERGE 0x0040
#define CI_STATE_LOW_MEMORY  0x0080
#define CI_STATE_HIGH_IO  0x0100
#define CI_STATE_MASTER_MERGE_PAUSED  0x0200
#define CI_STATE_READ_ONLY  0x0400
#define CI_STATE_BATTERY_POWER  0x0800
#define CI_STATE_USER_ACTIVE  0x1000
#define CI_STATE_STARTING  0x2000
#define CI_STATE_READING_USNS  0x4000

#ifndef CI_STATE_DEFINED
#define CI_STATE_DEFINED
#include <pshpack4.h>
typedef struct _CI_STATE {
    DWORD cbStruct;
    DWORD cWordList;
    DWORD cPersistentIndex;
    DWORD cQueries;
    DWORD cDocuments;
    DWORD cFreshTest;
    DWORD dwMergeProgress;
    DWORD eState;
    DWORD cFilteredDocuments;
    DWORD cTotalDocuments;
    DWORD cPendingScans;
    DWORD dwIndexSize;
    DWORD cUniqueKeys;
    DWORD cSecQDocuments;
    DWORD dwPropCacheSize;
} CI_STATE;
#include <poppack.h>
#endif /* CI_STATE_DEFINED */

STDAPI CIState(WCHAR const *pwcsCat, WCHAR const *pwcsMachine, CI_STATE * pCiState);

#if defined __ICommand_INTERFACE_DEFINED__
STDAPI CIMakeICommand(ICommand**,ULONG,DWORD const*,WCHAR const* const*, WCHAR const* const*,WCHAR const* const*);
STDAPI CICreateCommand(IUnknown**,IUnknown*,REFIID,WCHAR const*,WCHAR const*);

#if defined __ICommandTree_INTERFACE_DEFINED__
typedef struct tagCIPROPERTYDEF {
    LPWSTR wcsFriendlyName;
    DWORD dbType;
    DBID dbCol;
} CIPROPERTYDEF;

#define ISQLANG_V1 1
#define ISQLANG_V2 2

STDAPI CITextToSelectTree(WCHAR const*,DBCOMMANDTREE**,ULONG,CIPROPERTYDEF*,LCID);
STDAPI CITextToSelectTreeEx(WCHAR const*,ULONG,DBCOMMANDTREE**,ULONG,CIPROPERTYDEF*,LCID);
STDAPI CITextToFullTree(WCHAR const*,WCHAR const*,WCHAR const*,WCHAR const*,DBCOMMANDTREE**,ULONG,CIPROPERTYDEF*,LCID);
STDAPI CITextToFullTreeEx(WCHAR const*,ULONG,WCHAR const*,WCHAR const*,WCHAR const*,DBCOMMANDTREE**,ULONG,CIPROPERTYDEF*,LCID);
STDAPI CIBuildQueryNode(WCHAR const*,DBCOMMANDOP,PROPVARIANT const*,DBCOMMANDTREE**,ULONG,CIPROPERTYDEF const*,LCID);
STDAPI CIBuildQueryTree(DBCOMMANDTREE const*,DBCOMMANDOP,ULONG,DBCOMMANDTREE const* const*,DBCOMMANDTREE**);
STDAPI CIRestrictionToFullTree(DBCOMMANDTREE const*,WCHAR const*,WCHAR const*,WCHAR const*,DBCOMMANDTREE**,ULONG,CIPROPERTYDEF*,LCID);

#endif /* __ICommandTree_INTERFACE_DEFINED__ */
#endif /* __ICommand_INTERFACE_DEFINED__ */

STDAPI LoadIFilter(WCHAR const *pwcsPath, IUnknown * pUnkOuter, void **ppIUnk);
STDAPI BindIFilterFromStorage(IStorage * pStg, IUnknown * pUnkOuter, void **ppIUnk);
STDAPI BindIFilterFromStream(IStream * pStm, IUnknown * pUnkOuter, void **ppIUnk);
STDAPI LocateCatalogsW(WCHAR const *pwszScope, ULONG iBmk, WCHAR * pwszMachine, ULONG * pccMachine, WCHAR * pwszCat, ULONG * pccCat);
STDAPI LocateCatalogsA(char const *pwszScope, ULONG iBmk, char *pwszMachine, ULONG * pccMachine, char *pwszCat, ULONG * pccCat);

#ifdef UNICODE
#define LocateCatalogs  LocateCatalogsW
#else
#define LocateCatalogs  LocateCatalogsA
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _NTQUERY_H */
