/* Cabinet File compression API definitions */

#ifndef INCLUDED_TYPES_FCI_FDI
#define INCLUDED_TYPES_FCI_FDI  1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DIAMONDAPI
#define DIAMONDAPI __cdecl
#endif

#ifndef _WIN64
#include <pshpack4.h>
#endif

#if !defined(_WINDOWS_H)
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
#endif /* _WINDOWS_H */

typedef unsigned long CHECKSUM;

typedef unsigned long UOFF;
typedef unsigned long COFF;

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef NULL
#define NULL  ((void *)0)
#endif

typedef struct {
    int erfOper;
    int erfType;
    BOOL fError;
} ERF, *PERF;

#ifdef _DEBUG
#define STATIC
#else /* !_DEBUG */
#define STATIC static
#endif /* !_DEBUG */

#define CB_MAX_CHUNK  32768U
#define CB_MAX_DISK  0x7fffffffL
#define CB_MAX_FILENAME  256
#define CB_MAX_CABINET_NAME  256
#define CB_MAX_CAB_PATH  256
#define CB_MAX_DISK_NAME  256

typedef unsigned short TCOMP;

#define tcompMASK_TYPE  0x000F
#define tcompTYPE_NONE  0x0000
#define tcompTYPE_MSZIP  0x0001
#define tcompTYPE_QUANTUM  0x0002
#define tcompTYPE_LZX  0x0003
#define tcompBAD  0x000F

#define tcompMASK_LZX_WINDOW  0x1F00
#define tcompLZX_WINDOW_LO  0x0F00
#define tcompLZX_WINDOW_HI  0x1500
#define tcompSHIFT_LZX_WINDOW  8

#define tcompMASK_QUANTUM_LEVEL  0x00F0
#define tcompQUANTUM_LEVEL_LO  0x0010
#define tcompQUANTUM_LEVEL_HI  0x0070
#define tcompSHIFT_QUANTUM_LEVEL  4

#define tcompMASK_QUANTUM_MEM  0x1F00
#define tcompQUANTUM_MEM_LO  0x0A00
#define tcompQUANTUM_MEM_HI  0x1500
#define tcompSHIFT_QUANTUM_MEM  8

#define tcompMASK_RESERVED  0xE000

#define CompressionTypeFromTCOMP(tc)  ((tc) & tcompMASK_TYPE)
#define CompressionLevelFromTCOMP(tc)  (((tc) & tcompMASK_QUANTUM_LEVEL) >> tcompSHIFT_QUANTUM_LEVEL)
#define CompressionMemoryFromTCOMP(tc)  (((tc) & tcompMASK_QUANTUM_MEM) >> tcompSHIFT_QUANTUM_MEM)
#define TCOMPfromTypeLevelMemory(t,l,m)  (((m) << tcompSHIFT_QUANTUM_MEM)|((l) << tcompSHIFT_QUANTUM_LEVEL)|(t))
#define LZXCompressionWindowFromTCOMP(tc)  (((tc) & tcompMASK_LZX_WINDOW) >> tcompSHIFT_LZX_WINDOW)
#define TCOMPfromLZXWindow(w)  (((w) << tcompSHIFT_LZX_WINDOW)|(tcompTYPE_LZX))

#ifndef _WIN64
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* !INCLUDED_TYPES_FCI_FDI */

#include <basetsd.h>

#ifndef INCLUDED_FCI
#define INCLUDED_FCI  1

typedef char *LPSTR;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN64
#include <pshpack4.h>
#endif

typedef enum {
    FCIERR_NONE,
    FCIERR_OPEN_SRC,
    FCIERR_READ_SRC,
    FCIERR_ALLOC_FAIL,
    FCIERR_TEMP_FILE,
    FCIERR_BAD_COMPR_TYPE,
    FCIERR_CAB_FILE,
    FCIERR_USER_ABORT,
    FCIERR_MCI_FAIL,
    FCIERR_CAB_FORMAT_LIMIT
} FCIERROR;

#ifndef _A_NAME_IS_UTF
#define _A_NAME_IS_UTF  0x80
#endif

#ifndef _A_EXEC
#define _A_EXEC  0x40
#endif

typedef void *HFCI;

typedef struct {
    ULONG cb;
    ULONG cbFolderThresh;
    UINT cbReserveCFHeader;
    UINT cbReserveCFFolder;
    UINT cbReserveCFData;
    int iCab;
    int iDisk;
#ifndef REMOVE_CHICAGO_M6_HACK
    int fFailOnIncompressible;
#endif /* REMOVE_CHICAGO_M6_HACK */
    USHORT setID;
    char szDisk[CB_MAX_DISK_NAME];
    char szCab[CB_MAX_CABINET_NAME];
    char szCabPath[CB_MAX_CAB_PATH];
} CCAB, *PCCAB;

typedef void * (DIAMONDAPI *PFNFCIALLOC)(ULONG);
#define FNFCIALLOC(fn) void *DIAMONDAPI fn(ULONG cb)

typedef void (DIAMONDAPI *PFNFCIFREE)(void *);
#define FNFCIFREE(fn) void DIAMONDAPI fn(void *memory)

typedef INT_PTR (DIAMONDAPI *PFNFCIOPEN)(LPSTR,int,int,int*,void*);
typedef UINT (DIAMONDAPI *PFNFCIREAD)(INT_PTR,void*,UINT,int*,void*);
typedef UINT (DIAMONDAPI *PFNFCIWRITE)(INT_PTR,void*,UINT,int*,void*);
typedef int  (DIAMONDAPI *PFNFCICLOSE)(INT_PTR,int*,void*);
typedef long (DIAMONDAPI *PFNFCISEEK)(INT_PTR,long,int,int*,void*);
typedef int  (DIAMONDAPI *PFNFCIDELETE)(LPSTR,int*, void*);

#define FNFCIOPEN(fn) INT_PTR DIAMONDAPI fn(LPSTR pszFile, int oflag, int pmode, int *err, void *pv)
#define FNFCIREAD(fn) UINT DIAMONDAPI fn(INT_PTR hf, void *memory, UINT cb, int *err, void *pv)
#define FNFCIWRITE(fn) UINT DIAMONDAPI fn(INT_PTR hf, void *memory, UINT cb, int *err, void *pv)
#define FNFCICLOSE(fn) int DIAMONDAPI fn(INT_PTR hf, int *err, void *pv)
#define FNFCISEEK(fn) long DIAMONDAPI fn(INT_PTR hf, long dist, int seektype, int *err, void *pv)
#define FNFCIDELETE(fn) int DIAMONDAPI fn(LPSTR pszFile, int *err, void *pv)

typedef BOOL (DIAMONDAPI *PFNFCIGETNEXTCABINET)(PCCAB,ULONG,void*);

#define FNFCIGETNEXTCABINET(fn) BOOL DIAMONDAPI fn(PCCAB pccab, ULONG cbPrevCab, void *pv)

typedef int (DIAMONDAPI *PFNFCIFILEPLACED)(PCCAB,char*,long,BOOL,void*);

#define FNFCIFILEPLACED(fn) int DIAMONDAPI fn(PCCAB pccab, char *pszFile, long  cbFile, BOOL fContinuation, void *pv)

typedef INT_PTR (DIAMONDAPI *PFNFCIGETOPENINFO)(char*,USHORT*,USHORT*,USHORT*,int*,void*);

#define FNFCIGETOPENINFO(fn) INT_PTR DIAMONDAPI fn(char *pszName, USHORT *pdate, USHORT *ptime, USHORT *pattribs, int *err, void *pv)

#define statusFile  0
#define statusFolder  1
#define statusCabinet  2

typedef long (DIAMONDAPI *PFNFCISTATUS)(UINT,ULONG,ULONG,void*);

#define FNFCISTATUS(fn) long DIAMONDAPI fn(UINT typeStatus, ULONG cb1, ULONG cb2, void *pv)

typedef BOOL (DIAMONDAPI *PFNFCIGETTEMPFILE)(char *,int,void*);

#define FNFCIGETTEMPFILE(fn)  BOOL DIAMONDAPI fn(char *pszTempName, int cbTempName, void *pv)

HFCI DIAMONDAPI FCICreate(PERF,PFNFCIFILEPLACED,PFNFCIALLOC,PFNFCIFREE,PFNFCIOPEN,PFNFCIREAD,PFNFCIWRITE,PFNFCICLOSE,PFNFCISEEK,PFNFCIDELETE,PFNFCIGETTEMPFILE,PCCAB,void*);
BOOL DIAMONDAPI FCIAddFile(HFCI,LPSTR,LPSTR,BOOL,PFNFCIGETNEXTCABINET,PFNFCISTATUS,PFNFCIGETOPENINFO,TCOMP);
BOOL DIAMONDAPI FCIFlushCabinet(HFCI,BOOL,PFNFCIGETNEXTCABINET,PFNFCISTATUS);
BOOL DIAMONDAPI FCIFlushFolder(HFCI,PFNFCIGETNEXTCABINET,PFNFCISTATUS);
BOOL DIAMONDAPI FCIDestroy (HFCI);

#ifndef _WIN64
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* !INCLUDED_TYPES_FCI_FDI */
