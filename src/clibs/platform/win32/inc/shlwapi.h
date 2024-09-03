#ifndef _SHLWAPI_H
#define _SHLWAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows shell "light-weight" utility API definitions */

#ifndef NOSHLWAPI

#include <objbase.h>
#include <shtypes.h>

#ifdef UNIX
typedef interface IInternetSecurityMgrSite IInternetSecurityMgrSite;
typedef interface IInternetSecurityManager IInternetSecurityManager;
typedef interface IInternetHostSecurityManager IInternetHostSecurityManager;
#endif /* UNIX */

#ifndef WINSHLWAPI
#define LWSTDAPI  EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define LWSTDAPI_(type)  EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#define LWSTDAPIV  EXTERN_C DECLSPEC_IMPORT HRESULT STDAPIVCALLTYPE
#define LWSTDAPIV_(type)  EXTERN_C DECLSPEC_IMPORT type STDAPIVCALLTYPE
#endif /* WINSHLWAPI */

#include <pshpack8.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NO_SHLWAPI_STRFCNS  /* string functions */

enum tagSFBS_FLAGS {
    SFBS_FLAGS_ROUND_TO_NEAREST_DISPLAYED_DIGIT = 0x0001,
    SFBS_FLAGS_TRUNCATE_UNDISPLAYED_DECIMAL_DIGITS = 0x0002,
};
typedef int SFBS_FLAGS;

#define STIF_DEFAULT  0x00000000L
#define STIF_SUPPORT_HEX  0x00000001L
typedef int STIF_FLAGS;

LWSTDAPI_(LPSTR) StrChrA(LPCSTR, WORD);
LWSTDAPI_(LPWSTR) StrChrW(LPCWSTR, WCHAR);
LWSTDAPI_(LPSTR) StrChrIA(LPCSTR, WORD);
LWSTDAPI_(LPWSTR) StrChrIW(LPCWSTR, WCHAR);
#if (_WIN32_IE >= _WIN32_IE_IE60)
LWSTDAPI_(LPWSTR) StrChrNW(LPCWSTR, WCHAR, UINT);
LWSTDAPI_(LPWSTR) StrChrNIW(LPCWSTR, WCHAR, UINT);
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */
LWSTDAPI_(int) StrCmpNA(LPCSTR, LPCSTR, int);
LWSTDAPI_(int) StrCmpNW(LPCWSTR, LPCWSTR, int);
LWSTDAPI_(int) StrCmpNIA(LPCSTR, LPCSTR, int);
LWSTDAPI_(int) StrCmpNIW(LPCWSTR, LPCWSTR, int);
LWSTDAPI_(int) StrCSpnA(LPCSTR, LPCSTR);
LWSTDAPI_(int) StrCSpnW(LPCWSTR, LPCWSTR);
LWSTDAPI_(int) StrCSpnIA(LPCSTR, LPCSTR);
LWSTDAPI_(int) StrCSpnIW(LPCWSTR, LPCWSTR);
LWSTDAPI_(LPSTR) StrDupA(LPCSTR);
LWSTDAPI_(LPWSTR) StrDupW(LPCWSTR);

LWSTDAPI StrFormatByteSizeEx(ULONGLONG, SFBS_FLAGS, LPWSTR, UINT);

LWSTDAPI_(LPSTR) StrFormatByteSizeA(DWORD, LPSTR, UINT);
LWSTDAPI_(LPSTR) StrFormatByteSize64A(LONGLONG, LPSTR, UINT);
LWSTDAPI_(LPWSTR) StrFormatByteSizeW(LONGLONG, LPWSTR, UINT);
LWSTDAPI_(LPWSTR) StrFormatKBSizeW(LONGLONG, LPWSTR, UINT);
LWSTDAPI_(LPSTR) StrFormatKBSizeA(LONGLONG, LPSTR, UINT);
LWSTDAPI_(int) StrFromTimeIntervalA(LPSTR, UINT, DWORD, int);
LWSTDAPI_(int) StrFromTimeIntervalW(LPWSTR, UINT, DWORD, int);
LWSTDAPI_(BOOL) StrIsIntlEqualA(BOOL, LPCSTR, LPCSTR, int);
LWSTDAPI_(BOOL) StrIsIntlEqualW(BOOL, LPCWSTR, LPCWSTR, int);
LWSTDAPI_(LPSTR) StrNCatA(LPSTR, LPCSTR, int);
LWSTDAPI_(LPWSTR) StrNCatW(LPWSTR, LPCWSTR, int);
LWSTDAPI_(LPSTR) StrPBrkA(LPCSTR, LPCSTR);
LWSTDAPI_(LPWSTR) StrPBrkW(LPCWSTR, LPCWSTR);
LWSTDAPI_(LPSTR) StrRChrA(LPCSTR, LPCSTR, WORD);
LWSTDAPI_(LPWSTR) StrRChrW(LPCWSTR, LPCWSTR, WCHAR);
LWSTDAPI_(LPSTR) StrRChrIA(LPCSTR, LPCSTR, WORD);
LWSTDAPI_(LPWSTR) StrRChrIW(LPCWSTR, LPCWSTR, WCHAR);
LWSTDAPI_(LPSTR) StrRStrIA(LPCSTR, LPCSTR, LPCSTR);
LWSTDAPI_(LPWSTR) StrRStrIW(LPCWSTR, LPCWSTR, LPCWSTR);
LWSTDAPI_(int) StrSpnA(LPCSTR, LPCSTR);
LWSTDAPI_(int) StrSpnW(LPCWSTR, LPCWSTR);
LWSTDAPI_(LPSTR) StrStrA(LPCSTR, LPCSTR);
LWSTDAPI_(LPWSTR) StrStrW(LPCWSTR, LPCWSTR);
LWSTDAPI_(LPSTR) StrStrIA(LPCSTR, LPCSTR);
LWSTDAPI_(LPWSTR) StrStrIW(LPCWSTR, LPCWSTR);
#if (_WIN32_IE >= _WIN32_IE_IE60)
LWSTDAPI_(LPWSTR) StrStrNW(LPCWSTR, LPCWSTR, UINT);
LWSTDAPI_(LPWSTR) StrStrNIW(LPCWSTR, LPCWSTR, UINT);
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */

LWSTDAPI_(int) StrToIntA(LPCSTR);
LWSTDAPI_(int) StrToIntW(LPCWSTR);
LWSTDAPI_(BOOL) StrToIntExA(LPCSTR, STIF_FLAGS, int *);
LWSTDAPI_(BOOL) StrToIntExW(LPCWSTR, STIF_FLAGS, int *);
#if (_WIN32_IE >= _WIN32_IE_IE60)
LWSTDAPI_(BOOL) StrToInt64ExA(LPCSTR, STIF_FLAGS, LONGLONG *);
LWSTDAPI_(BOOL) StrToInt64ExW(LPCWSTR, STIF_FLAGS, LONGLONG *);
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */

LWSTDAPI_(BOOL) StrTrimA(LPSTR, LPCSTR);
LWSTDAPI_(BOOL) StrTrimW(LPWSTR, LPCWSTR);

LWSTDAPI_(LPWSTR) StrCatW(LPWSTR, LPCWSTR);
LWSTDAPI_(int) StrCmpW(LPCWSTR, LPCWSTR);
LWSTDAPI_(int) StrCmpIW(LPCWSTR, LPCWSTR);
LWSTDAPI_(LPWSTR) StrCpyW(LPWSTR, LPCWSTR);
LWSTDAPI_(LPWSTR) StrCpyNW(LPWSTR, LPCWSTR, int);

LWSTDAPI_(LPWSTR) StrCatBuffW(LPWSTR, LPCWSTR, int);
LWSTDAPI_(LPSTR) StrCatBuffA(LPSTR, LPCSTR, int);
LWSTDAPI_(BOOL) ChrCmpIA(WORD, WORD);
LWSTDAPI_(BOOL) ChrCmpIW(WCHAR, WCHAR);

LWSTDAPI_(int) wvnsprintfA(LPSTR, int, LPCSTR, va_list);
LWSTDAPI_(int) wvnsprintfW(LPWSTR, int, LPCWSTR, va_list);
LWSTDAPIV_(int) wnsprintfA(LPSTR, int, LPCSTR, ...);
LWSTDAPIV_(int) wnsprintfW(LPWSTR, int, LPCWSTR, ...);

#define StrIntlEqNA(s1,s2,nChar)  StrIsIntlEqualA(TRUE, (s1), (s2), (nChar))
#define StrIntlEqNW(s1,s2,nChar)  StrIsIntlEqualW(TRUE, (s1), (s2), (nChar))
#define StrIntlEqNIA(s1,s2,nChar)  StrIsIntlEqualA(FALSE, (s1), (s2), (nChar))
#define StrIntlEqNIW(s1,s2,nChar)  StrIsIntlEqualW(FALSE, (s1), (s2), (nChar))

LWSTDAPI StrRetToStrA(STRRET *, PCUITEMID_CHILD, LPSTR *);
LWSTDAPI StrRetToStrW(STRRET *, PCUITEMID_CHILD, LPWSTR *);
LWSTDAPI StrRetToBufA(STRRET *, PCUITEMID_CHILD, LPSTR, UINT);
LWSTDAPI StrRetToBufW(STRRET *, PCUITEMID_CHILD, LPWSTR, UINT);

LWSTDAPI SHStrDupA(LPCSTR, LPWSTR *);
LWSTDAPI SHStrDupW(LPCWSTR, LPWSTR *);

#if (_WIN32_IE >= _WIN32_IE_IE55)
LWSTDAPI_(int) StrCmpLogicalW(LPCWSTR, LPCWSTR);
LWSTDAPI_(DWORD) StrCatChainW(LPWSTR, DWORD, DWORD, LPCWSTR);
LWSTDAPI StrRetToBSTR(STRRET *, PCUITEMID_CHILD, BSTR *);
LWSTDAPI SHLoadIndirectString(LPCWSTR, LPWSTR, UINT, void **);
#endif /* _WIN32_IE >= _WIN32_IE_IE55 */

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
LWSTDAPI_(BOOL)IsCharSpaceA(CHAR);
LWSTDAPI_(BOOL)IsCharSpaceW(WCHAR);
LWSTDAPI_(int) StrCmpCA(LPCSTR, LPCSTR);
LWSTDAPI_(int) StrCmpCW(LPCWSTR, LPCWSTR);
LWSTDAPI_(int) StrCmpICA(LPCSTR, LPCSTR);
LWSTDAPI_(int) StrCmpICW(LPCWSTR, LPCWSTR);

#ifdef UNICODE
#define IsCharSpace  IsCharSpaceW
#define StrCmpC  StrCmpCW
#define StrCmpIC  StrCmpICW
#else /* !UNICODE */
#define IsCharSpace  IsCharSpaceA
#define StrCmpC  StrCmpCA
#define StrCmpIC  StrCmpICA
#endif /* !UNICODE */
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */


#if (_WIN32_IE >= _WIN32_IE_IE50)
LWSTDAPI_(int) StrCmpNCA(LPCSTR, LPCSTR, int);
LWSTDAPI_(int) StrCmpNCW(LPCWSTR, LPCWSTR, int);
LWSTDAPI_(int) StrCmpNICA(LPCSTR, LPCSTR, int);
LWSTDAPI_(int) StrCmpNICW(LPCWSTR, LPCWSTR, int);
#ifdef UNICODE
#define StrCmpNC  StrCmpNCW
#define StrCmpNIC  StrCmpNICW
#else /* !UNICODE */
#define StrCmpNC  StrCmpNCA
#define StrCmpNIC  StrCmpNICA
#endif /* !UNICODE */
#endif /* _WIN32_IE >= _WIN32_IE_IE50 */

LWSTDAPI_(BOOL)IntlStrEqWorkerA(BOOL, LPCSTR, LPCSTR, int);
LWSTDAPI_(BOOL)IntlStrEqWorkerW(BOOL, LPCWSTR, LPCWSTR, int);

#define IntlStrEqNA(s1,s2,nChar)  IntlStrEqWorkerA(TRUE, (s1), (s2), (nChar))
#define IntlStrEqNW(s1,s2,nChar)  IntlStrEqWorkerW(TRUE, (s1), (s2), (nChar))
#define IntlStrEqNIA(s1,s2,nChar)  IntlStrEqWorkerA(FALSE, (s1), (s2), (nChar))
#define IntlStrEqNIW(s1,s2,nChar)  IntlStrEqWorkerW(FALSE, (s1), (s2), (nChar))

#define SZ_CONTENTTYPE_HTMLA  "text/html"
#define SZ_CONTENTTYPE_HTMLW  L"text/html"
#define SZ_CONTENTTYPE_CDFA  "application/x-cdf"
#define SZ_CONTENTTYPE_CDFW  L"application/x-cdf"

#define PathIsHTMLFileA(pszPath)  PathIsContentTypeA(pszPath, SZ_CONTENTTYPE_HTMLA)
#define PathIsHTMLFileW(pszPath)  PathIsContentTypeW(pszPath, SZ_CONTENTTYPE_HTMLW)

#define StrCatA  lstrcatA
#define StrCmpA  lstrcmpA
#define StrCmpIA  lstrcmpiA
#define StrCpyA  lstrcpyA
#define StrCpyNA  lstrcpynA

#define StrToLong  StrToInt
#define StrNCmp  StrCmpN
#define StrNCmpI  StrCmpNI
#define StrNCpy  StrCpyN
#define StrCatN  StrNCat

#ifdef UNICODE
#define StrRetToStr  StrRetToStrW
#define StrRetToBuf  StrRetToBufW
#define SHStrDup  SHStrDupW
#define StrChr  StrChrW
#define StrRChr  StrRChrW
#define StrChrI  StrChrIW
#define StrRChrI  StrRChrIW
#define StrCmpN  StrCmpNW
#define StrCmpNI  StrCmpNIW
#define StrStr  StrStrW
#define StrStrI  StrStrIW
#define StrDup  StrDupW
#define StrRStrI  StrRStrIW
#define StrCSpn  StrCSpnW
#define StrCSpnI  StrCSpnIW
#define StrSpn  StrSpnW
#define StrToInt  StrToIntW
#define StrPBrk  StrPBrkW
#define StrToIntEx  StrToIntExW
#if (_WIN32_IE >= 0x0600)
#define StrToInt64Ex  StrToInt64ExW
#endif /* _WIN32_IE >= 0x0600 */
#define StrFromTimeInterval  StrFromTimeIntervalW
#define StrIntlEqN  StrIntlEqNW
#define StrIntlEqNI  StrIntlEqNIW
#define StrFormatByteSize  StrFormatByteSizeW
#define StrFormatByteSize64  StrFormatByteSizeW
#define StrFormatKBSize  StrFormatKBSizeW
#define StrNCat  StrNCatW
#define StrTrim  StrTrimW
#define StrCatBuff  StrCatBuffW
#define ChrCmpI  ChrCmpIW
#define wvnsprintf  wvnsprintfW
#define wnsprintf  wnsprintfW
#define StrIsIntlEqual  StrIsIntlEqualW
#define IntlStrEqN  IntlStrEqNW
#define IntlStrEqNI  IntlStrEqNIW
#define SZ_CONTENTTYPE_HTML  SZ_CONTENTTYPE_HTMLW
#define SZ_CONTENTTYPE_CDF  SZ_CONTENTTYPE_CDFW
#define StrCat  StrCatW
#define StrCmp  StrCmpW
#define StrCmpI  StrCmpIW
#define StrCpy  StrCpyW
#define StrCpyN  StrCpyNW
#define StrCatBuff  StrCatBuffW
#else /* !UNICODE */
#define StrRetToStr  StrRetToStrA
#define StrRetToBuf  StrRetToBufA
#define SHStrDup  SHStrDupA
#define StrChr  StrChrA
#define StrRChr  StrRChrA
#define StrChrI  StrChrIA
#define StrRChrI  StrRChrIA
#define StrCmpN  StrCmpNA
#define StrCmpNI  StrCmpNIA
#define StrStr  StrStrA
#define StrStrI  StrStrIA
#define StrDup  StrDupA
#define StrRStrI  StrRStrIA
#define StrCSpn  StrCSpnA
#define StrCSpnI  StrCSpnIA
#define StrSpn  StrSpnA
#define StrToInt  StrToIntA
#define StrPBrk  StrPBrkA
#define StrToIntEx  StrToIntExA
#if (_WIN32_IE >= 0x0600)
#define StrToInt64Ex  StrToInt64ExA
#endif /* _WIN32_IE >= 0x0600 */
#define StrFromTimeInterval  StrFromTimeIntervalA
#define StrIntlEqN  StrIntlEqNA
#define StrIntlEqNI  StrIntlEqNIA
#define StrFormatByteSize  StrFormatByteSizeA
#define StrFormatByteSize64  StrFormatByteSize64A
#define StrFormatKBSize  StrFormatKBSizeA
#define StrNCat  StrNCatA
#define StrTrim  StrTrimA
#define StrCatBuff  StrCatBuffA
#define ChrCmpI  ChrCmpIA
#define wvnsprintf  wvnsprintfA
#define wnsprintf  wnsprintfA
#define StrIsIntlEqual  StrIsIntlEqualA
#define IntlStrEqN  IntlStrEqNA
#define IntlStrEqNI  IntlStrEqNIA
#define SZ_CONTENTTYPE_HTML  SZ_CONTENTTYPE_HTMLA
#define SZ_CONTENTTYPE_CDF  SZ_CONTENTTYPE_CDFA
#define StrCat  lstrcatA
#define StrCmp  lstrcmpA
#define StrCmpI  lstrcmpiA
#define StrCpy  lstrcpyA
#define StrCpyN  lstrcpynA
#define StrCatBuff  StrCatBuffA
#endif /* !UNICODE */

#endif /* NO_SHLWAPI_STRFCNS */


#ifndef NO_SHLWAPI_PATH  /* path functions */

#define GCT_INVALID  0x0000
#define GCT_LFNCHAR  0x0001
#define GCT_SHORTCHAR  0x0002
#define GCT_WILD  0x0004
#define GCT_SEPARATOR  0x0008

LWSTDAPI_(LPSTR) PathAddBackslashA(LPSTR);
LWSTDAPI_(LPWSTR) PathAddBackslashW(LPWSTR);
LWSTDAPI_(BOOL) PathAddExtensionA(LPSTR, LPCSTR);
LWSTDAPI_(BOOL) PathAddExtensionW(LPWSTR, LPCWSTR);
LWSTDAPI_(BOOL) PathAppendA(LPSTR, LPCSTR);
LWSTDAPI_(BOOL) PathAppendW(LPWSTR, LPCWSTR);
LWSTDAPI_(LPSTR) PathBuildRootA(LPSTR, int);
LWSTDAPI_(LPWSTR) PathBuildRootW(LPWSTR, int);
LWSTDAPI_(BOOL) PathCanonicalizeA(LPSTR, LPCSTR);
LWSTDAPI_(BOOL) PathCanonicalizeW(LPWSTR, LPCWSTR);
LWSTDAPI_(LPSTR) PathCombineA(LPSTR, LPCSTR, LPCSTR);
LWSTDAPI_(LPWSTR) PathCombineW(LPWSTR, LPCWSTR, LPCWSTR);
LWSTDAPI_(BOOL) PathCompactPathA(HDC, LPSTR, UINT);
LWSTDAPI_(BOOL) PathCompactPathW(HDC, LPWSTR, UINT);
LWSTDAPI_(BOOL) PathCompactPathExA(LPSTR, LPCSTR, UINT, DWORD);
LWSTDAPI_(BOOL) PathCompactPathExW(LPWSTR, LPCWSTR, UINT, DWORD);
LWSTDAPI_(int) PathCommonPrefixA(LPCSTR, LPCSTR, LPSTR);
LWSTDAPI_(int) PathCommonPrefixW(LPCWSTR, LPCWSTR, LPWSTR);
LWSTDAPI_(BOOL) PathFileExistsA(LPCSTR);
LWSTDAPI_(BOOL) PathFileExistsW(LPCWSTR);
LWSTDAPI_(LPSTR) PathFindExtensionA(LPCSTR);
LWSTDAPI_(LPWSTR) PathFindExtensionW(LPCWSTR);
LWSTDAPI_(LPSTR) PathFindFileNameA(LPCSTR);
LWSTDAPI_(LPWSTR) PathFindFileNameW(LPCWSTR);
LWSTDAPI_(LPSTR) PathFindNextComponentA(LPCSTR);
LWSTDAPI_(LPWSTR) PathFindNextComponentW(LPCWSTR);
LWSTDAPI_(BOOL) PathFindOnPathA(LPSTR, LPCSTR *);
LWSTDAPI_(BOOL) PathFindOnPathW(LPWSTR, LPCWSTR *);
LWSTDAPI_(LPCSTR) PathFindSuffixArrayA(LPCSTR, const LPCSTR *, int);
LWSTDAPI_(LPCWSTR) PathFindSuffixArrayW(LPCWSTR, const LPCWSTR *, int);
LWSTDAPI_(LPSTR) PathGetArgsA(LPCSTR);
LWSTDAPI_(LPWSTR) PathGetArgsW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsLFNFileSpecA(LPCSTR);
LWSTDAPI_(BOOL) PathIsLFNFileSpecW(LPCWSTR);
LWSTDAPI_(UINT) PathGetCharTypeA(UCHAR);
LWSTDAPI_(UINT) PathGetCharTypeW(WCHAR);
LWSTDAPI_(int) PathGetDriveNumberA(LPCSTR);
LWSTDAPI_(int) PathGetDriveNumberW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsDirectoryA(LPCSTR);
LWSTDAPI_(BOOL) PathIsDirectoryW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsDirectoryEmptyA(LPCSTR);
LWSTDAPI_(BOOL) PathIsDirectoryEmptyW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsFileSpecA(LPCSTR);
LWSTDAPI_(BOOL) PathIsFileSpecW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsPrefixA(LPCSTR, LPCSTR);
LWSTDAPI_(BOOL) PathIsPrefixW(LPCWSTR, LPCWSTR);
LWSTDAPI_(BOOL) PathIsRelativeA(LPCSTR);
LWSTDAPI_(BOOL) PathIsRelativeW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsRootA(LPCSTR);
LWSTDAPI_(BOOL) PathIsRootW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsSameRootA(LPCSTR, LPCSTR);
LWSTDAPI_(BOOL) PathIsSameRootW(LPCWSTR, LPCWSTR);
LWSTDAPI_(BOOL) PathIsUNCA(LPCSTR);
LWSTDAPI_(BOOL) PathIsUNCW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsNetworkPathA(LPCSTR);
LWSTDAPI_(BOOL) PathIsNetworkPathW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsUNCServerA(LPCSTR);
LWSTDAPI_(BOOL) PathIsUNCServerW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsUNCServerShareA(LPCSTR);
LWSTDAPI_(BOOL) PathIsUNCServerShareW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsContentTypeA(LPCSTR, LPCSTR);
LWSTDAPI_(BOOL) PathIsContentTypeW(LPCWSTR, LPCWSTR);
LWSTDAPI_(BOOL) PathIsURLA(LPCSTR);
LWSTDAPI_(BOOL) PathIsURLW(LPCWSTR);
LWSTDAPI_(BOOL) PathMakePrettyA(LPSTR);
LWSTDAPI_(BOOL) PathMakePrettyW(LPWSTR);
LWSTDAPI_(BOOL) PathMatchSpecA(LPCSTR, LPCSTR);
LWSTDAPI_(BOOL) PathMatchSpecW(LPCWSTR, LPCWSTR);
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define PMSF_NORMAL  0x00000000
#define PMSF_MULTIPLE  0x00000001
#define PMSF_DONT_STRIP_SPACES  0x00010000
LWSTDAPI PathMatchSpecExA(LPCSTR, LPCSTR, DWORD);
LWSTDAPI PathMatchSpecExW(LPCWSTR, LPCWSTR, DWORD);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
LWSTDAPI_(int) PathParseIconLocationA(LPSTR);
LWSTDAPI_(int) PathParseIconLocationW(LPWSTR);
LWSTDAPI_(BOOL) PathQuoteSpacesA(LPSTR);
LWSTDAPI_(BOOL) PathQuoteSpacesW(LPWSTR);
LWSTDAPI_(BOOL) PathRelativePathToA(LPSTR, LPCSTR, DWORD, LPCSTR, DWORD);
LWSTDAPI_(BOOL) PathRelativePathToW(LPWSTR, LPCWSTR, DWORD, LPCWSTR, DWORD);
LWSTDAPI_(void) PathRemoveArgsA(LPSTR);
LWSTDAPI_(void) PathRemoveArgsW(LPWSTR);
LWSTDAPI_(LPSTR) PathRemoveBackslashA(LPSTR);
LWSTDAPI_(LPWSTR) PathRemoveBackslashW(LPWSTR);
LWSTDAPI_(void) PathRemoveBlanksA(LPSTR);
LWSTDAPI_(void) PathRemoveBlanksW(LPWSTR);
LWSTDAPI_(void) PathRemoveExtensionA(LPSTR);
LWSTDAPI_(void) PathRemoveExtensionW(LPWSTR);
LWSTDAPI_(BOOL) PathRemoveFileSpecA(LPSTR);
LWSTDAPI_(BOOL) PathRemoveFileSpecW(LPWSTR);
LWSTDAPI_(BOOL) PathRenameExtensionA(LPSTR, LPCSTR);
LWSTDAPI_(BOOL) PathRenameExtensionW(LPWSTR, LPCWSTR);
LWSTDAPI_(BOOL) PathSearchAndQualifyA(LPCSTR, LPSTR, UINT);
LWSTDAPI_(BOOL) PathSearchAndQualifyW(LPCWSTR, LPWSTR, UINT);
LWSTDAPI_(void) PathSetDlgItemPathA(HWND, int, LPCSTR);
LWSTDAPI_(void) PathSetDlgItemPathW(HWND, int, LPCWSTR);
LWSTDAPI_(LPSTR) PathSkipRootA(LPCSTR);
LWSTDAPI_(LPWSTR) PathSkipRootW(LPCWSTR);
LWSTDAPI_(void) PathStripPathA(LPSTR);
LWSTDAPI_(void) PathStripPathW(LPWSTR);
LWSTDAPI_(BOOL) PathStripToRootA(LPSTR);
LWSTDAPI_(BOOL) PathStripToRootW(LPWSTR);
LWSTDAPI_(BOOL) PathUnquoteSpacesA(LPSTR);
LWSTDAPI_(BOOL) PathUnquoteSpacesW(LPWSTR);
LWSTDAPI_(BOOL) PathMakeSystemFolderA(LPCSTR);
LWSTDAPI_(BOOL) PathMakeSystemFolderW(LPCWSTR);
LWSTDAPI_(BOOL) PathUnmakeSystemFolderA(LPCSTR);
LWSTDAPI_(BOOL) PathUnmakeSystemFolderW(LPCWSTR);
LWSTDAPI_(BOOL) PathIsSystemFolderA(LPCSTR, DWORD);
LWSTDAPI_(BOOL) PathIsSystemFolderW(LPCWSTR, DWORD);
LWSTDAPI_(void) PathUndecorateA(LPSTR);
LWSTDAPI_(void) PathUndecorateW(LPWSTR);
LWSTDAPI_(BOOL) PathUnExpandEnvStringsA(LPCSTR, LPSTR, UINT);
LWSTDAPI_(BOOL) PathUnExpandEnvStringsW(LPCWSTR, LPWSTR, UINT);

#ifdef UNICODE
#define PathAddBackslash  PathAddBackslashW
#define PathAddExtension  PathAddExtensionW
#define PathBuildRoot  PathBuildRootW
#define PathCombine  PathCombineW
#define PathFileExists  PathFileExistsW
#define PathFindExtension  PathFindExtensionW
#define PathFindFileName  PathFindFileNameW
#define PathFindNextComponent  PathFindNextComponentW
#define PathFindSuffixArray  PathFindSuffixArrayW
#define PathGetArgs  PathGetArgsW
#define PathIsLFNFileSpec  PathIsLFNFileSpecW
#define PathGetDriveNumber  PathGetDriveNumberW
#define PathIsDirectory  PathIsDirectoryW
#define PathIsDirectoryEmpty  PathIsDirectoryEmptyW
#define PathIsFileSpec  PathIsFileSpecW
#define PathIsPrefix  PathIsPrefixW
#define PathIsRelative  PathIsRelativeW
#define PathIsRoot  PathIsRootW
#define PathIsSameRoot  PathIsSameRootW
#define PathIsUNC  PathIsUNCW
#define PathIsNetworkPath  PathIsNetworkPathW
#define PathIsUNCServer  PathIsUNCServerW
#define PathIsUNCServerShare  PathIsUNCServerShareW
#define PathIsURL  PathIsURLW
#define PathRemoveBackslash  PathRemoveBackslashW
#define PathSkipRoot  PathSkipRootW
#define PathStripPath  PathStripPathW
#define PathStripToRoot  PathStripToRootW
#define PathMakeSystemFolder  PathMakeSystemFolderW
#define PathUnmakeSystemFolder  PathUnmakeSystemFolderW
#define PathIsSystemFolder  PathIsSystemFolderW
#define PathUndecorate  PathUndecorateW
#define PathUnExpandEnvStrings  PathUnExpandEnvStringsW
#define PathAppend  PathAppendW
#define PathCanonicalize  PathCanonicalizeW
#define PathCompactPath  PathCompactPathW
#define PathCompactPathEx  PathCompactPathExW
#define PathCommonPrefix  PathCommonPrefixW
#define PathFindOnPath  PathFindOnPathW
#define PathGetCharType  PathGetCharTypeW
#define PathIsContentType  PathIsContentTypeW
#define PathIsHTMLFile  PathIsHTMLFileW
#define PathMakePretty  PathMakePrettyW
#define PathMatchSpec  PathMatchSpecW
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define PathMatchSpecEx  PathMatchSpecExW
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define PathParseIconLocation  PathParseIconLocationW
#define PathQuoteSpaces  PathQuoteSpacesW
#define PathRelativePathTo  PathRelativePathToW
#define PathRemoveArgs  PathRemoveArgsW
#define PathRemoveBlanks  PathRemoveBlanksW
#define PathRemoveExtension  PathRemoveExtensionW
#define PathRemoveFileSpec  PathRemoveFileSpecW
#define PathRenameExtension  PathRenameExtensionW
#define PathSearchAndQualify  PathSearchAndQualifyW
#define PathSetDlgItemPath  PathSetDlgItemPathW
#define PathUnquoteSpaces  PathUnquoteSpacesW
#else /* !UNICODE */
#define PathAddBackslash  PathAddBackslashA
#define PathAddExtension  PathAddExtensionA
#define PathBuildRoot  PathBuildRootA
#define PathCombine  PathCombineA
#define PathFileExists  PathFileExistsA
#define PathFindExtension  PathFindExtensionA
#define PathFindFileName  PathFindFileNameA
#define PathFindNextComponent  PathFindNextComponentA
#define PathFindSuffixArray  PathFindSuffixArrayA
#define PathGetArgs  PathGetArgsA
#define PathIsLFNFileSpec  PathIsLFNFileSpecA
#define PathGetDriveNumber  PathGetDriveNumberA
#define PathIsDirectory  PathIsDirectoryA
#define PathIsDirectoryEmpty  PathIsDirectoryEmptyA
#define PathIsFileSpec  PathIsFileSpecA
#define PathIsPrefix  PathIsPrefixA
#define PathIsRelative  PathIsRelativeA
#define PathIsRoot  PathIsRootA
#define PathIsSameRoot  PathIsSameRootA
#define PathIsUNC  PathIsUNCA
#define PathIsNetworkPath  PathIsNetworkPathA
#define PathIsUNCServer  PathIsUNCServerA
#define PathIsUNCServerShare  PathIsUNCServerShareA
#define PathIsURL  PathIsURLA
#define PathRemoveBackslash  PathRemoveBackslashA
#define PathSkipRoot  PathSkipRootA
#define PathStripPath  PathStripPathA
#define PathStripToRoot  PathStripToRootA
#define PathMakeSystemFolder  PathMakeSystemFolderA
#define PathUnmakeSystemFolder  PathUnmakeSystemFolderA
#define PathIsSystemFolder  PathIsSystemFolderA
#define PathUndecorate  PathUndecorateA
#define PathUnExpandEnvStrings  PathUnExpandEnvStringsA
#define PathAppend  PathAppendA
#define PathCanonicalize  PathCanonicalizeA
#define PathCompactPath  PathCompactPathA
#define PathCompactPathEx  PathCompactPathExA
#define PathCommonPrefix  PathCommonPrefixA
#define PathFindOnPath  PathFindOnPathA
#define PathGetCharType  PathGetCharTypeA
#define PathIsContentType  PathIsContentTypeA
#define PathIsHTMLFile  PathIsHTMLFileA
#define PathMakePretty  PathMakePrettyA
#define PathMatchSpec  PathMatchSpecA
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define PathMatchSpecEx  PathMatchSpecExA
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define PathParseIconLocation  PathParseIconLocationA
#define PathQuoteSpaces  PathQuoteSpacesA
#define PathRelativePathTo  PathRelativePathToA
#define PathRemoveArgs  PathRemoveArgsA
#define PathRemoveBlanks  PathRemoveBlanksA
#define PathRemoveExtension  PathRemoveExtensionA
#define PathRemoveFileSpec  PathRemoveFileSpecA
#define PathRenameExtension  PathRenameExtensionA
#define PathSearchAndQualify  PathSearchAndQualifyA
#define PathSetDlgItemPath  PathSetDlgItemPathA
#define PathUnquoteSpaces  PathUnquoteSpacesA
#endif /* !UNICODE */

typedef enum {
    URL_SCHEME_INVALID = -1,
    URL_SCHEME_UNKNOWN = 0,
    URL_SCHEME_FTP,
    URL_SCHEME_HTTP,
    URL_SCHEME_GOPHER,
    URL_SCHEME_MAILTO,
    URL_SCHEME_NEWS,
    URL_SCHEME_NNTP,
    URL_SCHEME_TELNET,
    URL_SCHEME_WAIS,
    URL_SCHEME_FILE,
    URL_SCHEME_MK,
    URL_SCHEME_HTTPS,
    URL_SCHEME_SHELL,
    URL_SCHEME_SNEWS,
    URL_SCHEME_LOCAL,
    URL_SCHEME_JAVASCRIPT,
    URL_SCHEME_VBSCRIPT,
    URL_SCHEME_ABOUT,
    URL_SCHEME_RES,
#if (_WIN32_IE >= _WIN32_IE_IE60)
    URL_SCHEME_MSSHELLROOTED,
    URL_SCHEME_MSSHELLIDLIST,
    URL_SCHEME_MSHELP,
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */
#if (_WIN32_IE >= _WIN32_IE_IE70)
    URL_SCHEME_MSSHELLDEVICE,
    URL_SCHEME_WILDCARD,
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#if (NTDDI_VERSION >= NTDDI_VISTA)
    URL_SCHEME_SEARCH_MS,
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
#if (NTDDI_VERSION >= NTDDI_VISTASP1)
    URL_SCHEME_SEARCH,
#endif /* NTDDI_VERSION >= NTDDI_VISTASP1 */
#if (NTDDI_VERSION >= NTDDI_WIN7)
    URL_SCHEME_KNOWNFOLDER,
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */
    URL_SCHEME_MAXVALUE,
} URL_SCHEME;

typedef enum {
    URL_PART_NONE = 0,
    URL_PART_SCHEME = 1,
    URL_PART_HOSTNAME,
    URL_PART_USERNAME,
    URL_PART_PASSWORD,
    URL_PART_PORT,
    URL_PART_QUERY,
} URL_PART;

typedef enum {
    URLIS_URL,
    URLIS_OPAQUE,
    URLIS_NOHISTORY,
    URLIS_FILEURL,
    URLIS_APPLIABLE,
    URLIS_DIRECTORY,
    URLIS_HASQUERY,
} URLIS;

#define URL_UNESCAPE  0x10000000
#define URL_ESCAPE_UNSAFE  0x20000000
#define URL_PLUGGABLE_PROTOCOL  0x40000000
#define URL_WININET_COMPATIBILITY  0x80000000
#define URL_DONT_ESCAPE_EXTRA_INFO  0x02000000
#define URL_DONT_UNESCAPE_EXTRA_INFO  URL_DONT_ESCAPE_EXTRA_INFO
#define URL_BROWSER_MODE  URL_DONT_ESCAPE_EXTRA_INFO
#define URL_ESCAPE_SPACES_ONLY  0x04000000
#define URL_DONT_SIMPLIFY  0x08000000
#define URL_NO_META  URL_DONT_SIMPLIFY
#define URL_UNESCAPE_INPLACE  0x00100000
#define URL_CONVERT_IF_DOSPATH  0x00200000
#define URL_UNESCAPE_HIGH_ANSI_ONLY  0x00400000
#define URL_INTERNAL_PATH  0x00800000
#define URL_FILE_USE_PATHURL  0x00010000
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define URL_DONT_UNESCAPE  0x00020000
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
#if (NTDDI_VERSION >= NTDDI_WIN7)
#define URL_ESCAPE_AS_UTF8  0x00040000
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */
#if (NTDDI_VERSION >= NTDDI_WIN8)
#define URL_UNESCAPE_AS_UTF8  URL_ESCAPE_AS_UTF8
#define URL_ESCAPE_ASCII_URI_COMPONENT  0x00080000
#define URL_ESCAPE_URI_COMPONENT  (URL_ESCAPE_ASCII_URI_COMPONENT|URL_ESCAPE_AS_UTF8)
#define URL_UNESCAPE_URI_COMPONENT  URL_UNESCAPE_AS_UTF8
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */
#define URL_ESCAPE_PERCENT  0x00001000
#define URL_ESCAPE_SEGMENT_ONLY  0x00002000

#define URL_PARTFLAG_KEEPSCHEME  0x00000001

#define URL_APPLY_DEFAULT  0x00000001
#define URL_APPLY_GUESSSCHEME  0x00000002
#define URL_APPLY_GUESSFILE  0x00000004
#define URL_APPLY_FORCEAPPLY  0x00000008

LWSTDAPI_(int) UrlCompareA(LPCSTR, LPCSTR, BOOL);
LWSTDAPI_(int) UrlCompareW(LPCWSTR, LPCWSTR, BOOL);
LWSTDAPI UrlCombineA(LPCSTR, LPCSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI UrlCombineW(LPCWSTR, LPCWSTR, LPWSTR, LPDWORD, DWORD);
LWSTDAPI UrlCanonicalizeA(LPCSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI UrlCanonicalizeW(LPCWSTR, LPWSTR, LPDWORD, DWORD);
LWSTDAPI_(BOOL) UrlIsOpaqueA(LPCSTR);
LWSTDAPI_(BOOL) UrlIsOpaqueW(LPCWSTR);
LWSTDAPI_(BOOL) UrlIsNoHistoryA(LPCSTR);
LWSTDAPI_(BOOL) UrlIsNoHistoryW(LPCWSTR);
#define UrlIsFileUrlA(pszURL)  UrlIsA(pszURL, URLIS_FILEURL)
#define UrlIsFileUrlW(pszURL)  UrlIsW(pszURL, URLIS_FILEURL)
LWSTDAPI_(BOOL) UrlIsA(LPCSTR pszUrl, URLIS);
LWSTDAPI_(BOOL) UrlIsW(LPCWSTR pszUrl, URLIS);
LWSTDAPI_(LPCSTR) UrlGetLocationA(LPCSTR);
LWSTDAPI_(LPCWSTR) UrlGetLocationW(LPCWSTR);
LWSTDAPI UrlUnescapeA(LPSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI UrlUnescapeW(LPWSTR, LPWSTR, LPDWORD, DWORD);
LWSTDAPI UrlEscapeA(LPCSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI UrlEscapeW(LPCWSTR, LPWSTR, LPDWORD, DWORD);
LWSTDAPI UrlCreateFromPathA(LPCSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI UrlCreateFromPathW(LPCWSTR, LPWSTR, LPDWORD, DWORD);
LWSTDAPI PathCreateFromUrlA(LPCSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI PathCreateFromUrlW(LPCWSTR, LPWSTR, LPDWORD, DWORD);
#if (_WIN32_IE >= _WIN32_IE_IE70)
LWSTDAPI PathCreateFromUrlAlloc(LPCWSTR, LPWSTR *, DWORD);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
LWSTDAPI UrlHashA(LPCSTR, LPBYTE, DWORD);
LWSTDAPI UrlHashW(LPCWSTR, LPBYTE, DWORD);
LWSTDAPI UrlGetPartW(LPCWSTR, LPWSTR, LPDWORD, DWORD, DWORD);
LWSTDAPI UrlGetPartA(LPCSTR, LPSTR, LPDWORD, DWORD, DWORD);
LWSTDAPI UrlApplySchemeA(LPCSTR, LPSTR, LPDWORD, DWORD);
LWSTDAPI UrlApplySchemeW(LPCWSTR, LPWSTR, LPDWORD, DWORD);
LWSTDAPI HashData(LPBYTE, DWORD, LPBYTE, DWORD);
LWSTDAPI UrlFixupW(LPCWSTR, LPWSTR, DWORD);

#ifdef UNICODE
#define UrlCompare  UrlCompareW
#define UrlCombine  UrlCombineW
#define UrlCanonicalize  UrlCanonicalizeW
#define UrlIsOpaque  UrlIsOpaqueW
#define UrlIsFileUrl  UrlIsFileUrlW
#define UrlGetLocation  UrlGetLocationW
#define UrlUnescape  UrlUnescapeW
#define UrlEscape  UrlEscapeW
#define UrlCreateFromPath  UrlCreateFromPathW
#define PathCreateFromUrl  PathCreateFromUrlW
#define UrlHash  UrlHashW
#define UrlGetPart  UrlGetPartW
#define UrlApplyScheme  UrlApplySchemeW
#define UrlIs  UrlIsW
#define UrlFixup  UrlFixupW
#else /* !UNICODE */
#define UrlCompare  UrlCompareA
#define UrlCombine  UrlCombineA
#define UrlCanonicalize  UrlCanonicalizeA
#define UrlIsOpaque  UrlIsOpaqueA
#define UrlIsFileUrl  UrlIsFileUrlA
#define UrlGetLocation  UrlGetLocationA
#define UrlUnescape  UrlUnescapeA
#define UrlEscape  UrlEscapeA
#define UrlCreateFromPath  UrlCreateFromPathA
#define PathCreateFromUrl  PathCreateFromUrlA
#define UrlHash  UrlHashA
#define UrlGetPart  UrlGetPartA
#define UrlApplyScheme  UrlApplySchemeA
#define UrlIs  UrlIsA
/* no UrlFixupA */
#endif /* !UNICODE */

#define UrlEscapeSpaces(pszUrl,pszEscaped,pcchEscaped)  UrlCanonicalize((pszUrl), (pszEscaped), (pcchEscaped), URL_ESCAPE_SPACES_ONLY|URL_DONT_ESCAPE_EXTRA_INFO)
#define UrlUnescapeInPlace(pszUrl,dwFlags)  UrlUnescape((pszUrl), NULL, NULL, (dwFlags)|URL_UNESCAPE_INPLACE)

#if (_WIN32_IE >= _WIN32_IE_IE50)

typedef struct tagPARSEDURLA {
    DWORD cbSize;
    LPCSTR pszProtocol;
    UINT cchProtocol;
    LPCSTR pszSuffix;
    UINT cchSuffix;
    UINT nScheme;
} PARSEDURLA , *PPARSEDURLA;

typedef struct tagPARSEDURLW {
    DWORD cbSize;
    LPCWSTR pszProtocol;
    UINT cchProtocol;
    LPCWSTR pszSuffix;
    UINT cchSuffix;
    UINT nScheme;
} PARSEDURLW , *PPARSEDURLW;

LWSTDAPI ParseURLA(LPCSTR, PARSEDURLA *);
LWSTDAPI ParseURLW(LPCWSTR, PARSEDURLW *);

#ifdef UNICODE
typedef PARSEDURLW PARSEDURL;
typedef PPARSEDURLW PPARSEDURL;
#define ParseURL  ParseURLW
#else /* !UNICODE */
typedef PARSEDURLA PARSEDURL;
typedef PPARSEDURLA PPARSEDURL;
#define ParseURL  ParseURLA
#endif /* !UNICODE */

#endif /* _WIN32_IE >= _WIN32_IE_IE50 */

#endif /* NO_SHLWAPI_PATH */


#ifndef NO_SHLWAPI_REG  /* registry functions */

#define SHREGSET_HKCU  0x00000001
#define SHREGSET_FORCE_HKCU  0x00000002
#define SHREGSET_HKLM  0x00000004
#define SHREGSET_FORCE_HKLM  0x00000008
#define SHREGSET_DEFAULT  (SHREGSET_FORCE_HKCU|SHREGSET_HKLM)

typedef enum {
    SHREGDEL_DEFAULT = 0x00000000,
    SHREGDEL_HKCU = 0x00000001,
    SHREGDEL_HKLM = 0x00000010,
    SHREGDEL_BOTH = 0x00000011,
} SHREGDEL_FLAGS;

typedef enum {
    SHREGENUM_DEFAULT = 0x00000000,
    SHREGENUM_HKCU = 0x00000001,
    SHREGENUM_HKLM = 0x00000010,
    SHREGENUM_BOTH = 0x00000011,
} SHREGENUM_FLAGS;

typedef HANDLE HUSKEY;
typedef HUSKEY *PHUSKEY;

enum {
    ASSOCF_INIT_NOREMAPCLSID = 0x00000001,
    ASSOCF_INIT_BYEXENAME = 0x00000002,
    ASSOCF_OPEN_BYEXENAME = 0x00000002,
    ASSOCF_INIT_DEFAULTTOSTAR = 0x00000004,
    ASSOCF_INIT_DEFAULTTOFOLDER = 0x00000008,
    ASSOCF_NOUSERSETTINGS = 0x00000010,
    ASSOCF_NOTRUNCATE = 0x00000020,
    ASSOCF_VERIFY = 0x00000040,
    ASSOCF_REMAPRUNDLL = 0x00000080,
    ASSOCF_NOFIXUPS = 0x00000100,
    ASSOCF_IGNOREBASECLASS = 0x00000200,
    ASSOCF_INIT_IGNOREUNKNOWN = 0x00000400,
#if (NTDDI_VERSION >= NTDDI_WIN8)
    ASSOCF_INIT_FIXED_PROGID = 0x00000800,
    ASSOCF_IS_PROTOCOL = 0x00001000,
    ASSOCF_INIT_FOR_FILE = 0x00002000,
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */
};

typedef DWORD ASSOCF;

typedef enum {
    ASSOCSTR_COMMAND = 1,
    ASSOCSTR_EXECUTABLE,
    ASSOCSTR_FRIENDLYDOCNAME,
    ASSOCSTR_FRIENDLYAPPNAME,
    ASSOCSTR_NOOPEN,
    ASSOCSTR_SHELLNEWVALUE,
    ASSOCSTR_DDECOMMAND,
    ASSOCSTR_DDEIFEXEC,
    ASSOCSTR_DDEAPPLICATION,
    ASSOCSTR_DDETOPIC,
    ASSOCSTR_INFOTIP,
#if (_WIN32_IE >= _WIN32_IE_IE60)
    ASSOCSTR_QUICKTIP,
    ASSOCSTR_TILEINFO,
    ASSOCSTR_CONTENTTYPE,
    ASSOCSTR_DEFAULTICON,
    ASSOCSTR_SHELLEXTENSION,
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */
    ASSOCSTR_MAX
} ASSOCSTR;

typedef enum {
    ASSOCKEY_SHELLEXECCLASS = 1,
    ASSOCKEY_APP,
    ASSOCKEY_CLASS,
    ASSOCKEY_BASECLASS,
    ASSOCKEY_MAX
} ASSOCKEY;

typedef enum {
    ASSOCDATA_MSIDESCRIPTOR = 1,
    ASSOCDATA_NOACTIVATEHANDLER,
    ASSOCDATA_QUERYCLASSSTORE,
    ASSOCDATA_HASPERUSERASSOC,
#if (_WIN32_IE >= _WIN32_IE_IE60)
    ASSOCDATA_EDITFLAGS,
    ASSOCDATA_VALUE,
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */
    ASSOCDATA_MAX
} ASSOCDATA;

typedef enum {
    ASSOCENUM_NONE
} ASSOCENUM;

LWSTDAPI_(LSTATUS) SHDeleteEmptyKeyA(HKEY, LPCSTR);
LWSTDAPI_(LSTATUS) SHDeleteEmptyKeyW(HKEY, LPCWSTR);
LWSTDAPI_(LSTATUS) SHDeleteKeyA(HKEY, LPCSTR);
LWSTDAPI_(LSTATUS) SHDeleteKeyW(HKEY, LPCWSTR);
LWSTDAPI_(HKEY) SHRegDuplicateHKey(HKEY);
LWSTDAPI_(LSTATUS) SHDeleteValueA(HKEY, LPCSTR, LPCSTR);
LWSTDAPI_(LSTATUS) SHDeleteValueW(HKEY, LPCWSTR, LPCWSTR);
LWSTDAPI_(LSTATUS) SHGetValueA(HKEY, LPCSTR, LPCSTR, DWORD *, void *, DWORD *);
LWSTDAPI_(LSTATUS) SHGetValueW(HKEY, LPCWSTR, LPCWSTR, DWORD *, void *, DWORD *);
LWSTDAPI_(LSTATUS) SHSetValueA(HKEY, LPCSTR, LPCSTR, DWORD, LPCVOID, DWORD);
LWSTDAPI_(LSTATUS) SHSetValueW(HKEY, LPCWSTR, LPCWSTR, DWORD, LPCVOID, DWORD);
LWSTDAPI_(LSTATUS) SHQueryValueExA(HKEY, LPCSTR, DWORD *, DWORD *, void *, DWORD *);
LWSTDAPI_(LSTATUS) SHQueryValueExW(HKEY, LPCWSTR, DWORD *, DWORD *, void *, DWORD *);
LWSTDAPI_(LSTATUS) SHEnumKeyExA(HKEY, DWORD, LPSTR, LPDWORD);
LWSTDAPI_(LSTATUS) SHEnumKeyExW(HKEY, DWORD, LPWSTR, LPDWORD);
LWSTDAPI_(LSTATUS) SHEnumValueA(HKEY, DWORD, PSTR, LPDWORD, LPDWORD, void *, LPDWORD);
LWSTDAPI_(LSTATUS) SHEnumValueW(HKEY, DWORD, PWSTR, LPDWORD, LPDWORD, void *, LPDWORD);
LWSTDAPI_(LSTATUS) SHQueryInfoKeyA(HKEY, LPDWORD, LPDWORD, LPDWORD, LPDWORD);
LWSTDAPI_(LSTATUS) SHQueryInfoKeyW(HKEY, LPDWORD, LPDWORD, LPDWORD, LPDWORD);
LWSTDAPI_(LSTATUS) SHCopyKeyA(HKEY, LPCSTR, HKEY, DWORD);
LWSTDAPI_(LSTATUS) SHCopyKeyW(HKEY, LPCWSTR, HKEY, DWORD);
LWSTDAPI_(LSTATUS) SHRegGetPathA(HKEY, LPCSTR, LPCSTR, LPSTR, DWORD);
LWSTDAPI_(LSTATUS) SHRegGetPathW(HKEY, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
LWSTDAPI_(LSTATUS) SHRegSetPathA(HKEY, LPCSTR, LPCSTR, LPCSTR, DWORD);
LWSTDAPI_(LSTATUS) SHRegSetPathW(HKEY, LPCWSTR, LPCWSTR, LPCWSTR, DWORD);
LWSTDAPI_(LSTATUS) SHRegCreateUSKeyA(LPCSTR, REGSAM, HUSKEY, PHUSKEY, DWORD);
LWSTDAPI_(LSTATUS) SHRegCreateUSKeyW(LPCWSTR, REGSAM, HUSKEY, PHUSKEY, DWORD);
LWSTDAPI_(LSTATUS) SHRegOpenUSKeyA(LPCSTR, REGSAM, HUSKEY, PHUSKEY, BOOL);
LWSTDAPI_(LSTATUS) SHRegOpenUSKeyW(LPCWSTR, REGSAM, HUSKEY, PHUSKEY, BOOL);
LWSTDAPI_(LSTATUS) SHRegQueryUSValueA(HUSKEY, LPCSTR, DWORD *, void *, DWORD *, BOOL, void *, DWORD);
LWSTDAPI_(LSTATUS) SHRegQueryUSValueW(HUSKEY, LPCWSTR, DWORD *, void *, DWORD *, BOOL, void *, DWORD);
LWSTDAPI_(LSTATUS) SHRegWriteUSValueA(HUSKEY, LPCSTR, DWORD, const void *, DWORD, DWORD);
LWSTDAPI_(LSTATUS) SHRegWriteUSValueW(HUSKEY, LPCWSTR, DWORD, const void *, DWORD, DWORD);
LWSTDAPI_(LSTATUS) SHRegDeleteUSValueA(HUSKEY, LPCSTR, SHREGDEL_FLAGS);
LWSTDAPI_(LSTATUS) SHRegDeleteUSValueW(HUSKEY, LPCWSTR, SHREGDEL_FLAGS);
LWSTDAPI_(LSTATUS) SHRegDeleteEmptyUSKeyW(HUSKEY, LPCWSTR, SHREGDEL_FLAGS);
LWSTDAPI_(LSTATUS) SHRegDeleteEmptyUSKeyA(HUSKEY, LPCSTR, SHREGDEL_FLAGS);
LWSTDAPI_(LSTATUS) SHRegEnumUSKeyA(HUSKEY, DWORD, LPSTR, LPDWORD, SHREGENUM_FLAGS);
LWSTDAPI_(LSTATUS) SHRegEnumUSKeyW(HUSKEY, DWORD, LPWSTR, LPDWORD, SHREGENUM_FLAGS);
LWSTDAPI_(LSTATUS) SHRegEnumUSValueA(HUSKEY, DWORD, LPSTR, LPDWORD, LPDWORD, void *, LPDWORD, SHREGENUM_FLAGS);
LWSTDAPI_(LSTATUS) SHRegEnumUSValueW(HUSKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, void *, LPDWORD, SHREGENUM_FLAGS);
LWSTDAPI_(LSTATUS) SHRegQueryInfoUSKeyA(HUSKEY, LPDWORD, LPDWORD, LPDWORD, LPDWORD, SHREGENUM_FLAGS);
LWSTDAPI_(LSTATUS) SHRegQueryInfoUSKeyW(HUSKEY, LPDWORD, LPDWORD, LPDWORD, LPDWORD, SHREGENUM_FLAGS);
LWSTDAPI_(LSTATUS) SHRegCloseUSKey(HUSKEY);
STDAPI_(LSTATUS) SHRegGetUSValueA(LPCSTR, LPCSTR, DWORD *, void *, DWORD *, BOOL, void *, DWORD);
STDAPI_(LSTATUS) SHRegGetUSValueW(LPCWSTR, LPCWSTR, DWORD *, void *, DWORD *, BOOL, void *, DWORD);
LWSTDAPI_(LSTATUS) SHRegSetUSValueA(LPCSTR, LPCSTR, DWORD, const void *, DWORD, DWORD);
LWSTDAPI_(LSTATUS) SHRegSetUSValueW(LPCWSTR, LPCWSTR, DWORD, const void *, DWORD, DWORD);
LWSTDAPI_(int) SHRegGetIntW(HKEY, PCWSTR, int);
LWSTDAPI_(BOOL)SHRegGetBoolUSValueA(LPCSTR, LPCSTR, BOOL, BOOL);
LWSTDAPI_(BOOL)SHRegGetBoolUSValueW(LPCWSTR, LPCWSTR, BOOL, BOOL);

#ifdef UNICODE
#define SHDeleteEmptyKey  SHDeleteEmptyKeyW
#define SHDeleteKey  SHDeleteKeyW
#define SHDeleteValue  SHDeleteValueW
#define SHGetValue  SHGetValueW
#define SHSetValue  SHSetValueW
#define SHQueryValueEx  SHQueryValueExW
#define SHEnumKeyEx  SHEnumKeyExW
#define SHEnumValue  SHEnumValueW
#define SHQueryInfoKey  SHQueryInfoKeyW
#define SHCopyKey  SHCopyKeyW
#define SHRegGetPath  SHRegGetPathW
#define SHRegSetPath  SHRegSetPathW
#define SHRegCreateUSKey  SHRegCreateUSKeyW
#define SHRegOpenUSKey  SHRegOpenUSKeyW
#define SHRegQueryUSValue  SHRegQueryUSValueW
#define SHRegWriteUSValue  SHRegWriteUSValueW
#define SHRegDeleteUSValue  SHRegDeleteUSValueW
#define SHRegDeleteEmptyUSKey  SHRegDeleteEmptyUSKeyW
#define SHRegEnumUSKey  SHRegEnumUSKeyW
#define SHRegEnumUSValue  SHRegEnumUSValueW
#define SHRegQueryInfoUSKey  SHRegQueryInfoUSKeyW
#define SHRegGetUSValue  SHRegGetUSValueW
#define SHRegSetUSValue  SHRegSetUSValueW
#define SHRegGetInt  SHRegGetIntW
#define SHRegGetBoolUSValue  SHRegGetBoolUSValueW
#else /* !UNICODE */
#define SHDeleteEmptyKey  SHDeleteEmptyKeyA
#define SHDeleteKey  SHDeleteKeyA
#define SHDeleteValue  SHDeleteValueA
#define SHGetValue  SHGetValueA
#define SHSetValue  SHSetValueA
#define SHQueryValueEx  SHQueryValueExA
#define SHEnumKeyEx  SHEnumKeyExA
#define SHEnumValue  SHEnumValueA
#define SHQueryInfoKey  SHQueryInfoKeyA
#define SHCopyKey  SHCopyKeyA
#define SHRegGetPath  SHRegGetPathA
#define SHRegSetPath  SHRegSetPathA
#define SHRegCreateUSKey  SHRegCreateUSKeyA
#define SHRegOpenUSKey  SHRegOpenUSKeyA
#define SHRegQueryUSValue  SHRegQueryUSValueA
#define SHRegWriteUSValue  SHRegWriteUSValueA
#define SHRegDeleteUSValue  SHRegDeleteUSValueA
#define SHRegDeleteEmptyUSKey  SHRegDeleteEmptyUSKeyA
#define SHRegEnumUSKey  SHRegEnumUSKeyA
#define SHRegEnumUSValue  SHRegEnumUSValueA
#define SHRegQueryInfoUSKey  SHRegQueryInfoUSKeyA
#define SHRegGetUSValue  SHRegGetUSValueA
#define SHRegSetUSValue  SHRegSetUSValueA
#define SHRegGetBoolUSValue  SHRegGetBoolUSValueA
#endif /* !UNICODE */

#undef INTERFACE
#define INTERFACE IQueryAssociations
DECLARE_INTERFACE_IID_(IQueryAssociations, IUnknown, "c46ca590-3c3f-11d2-bee6-0000f805ca57" )
{
    STDMETHOD(QueryInterface)(THIS_ REFIID, void **) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Init)(THIS_ ASSOCF, LPCWSTR, HKEY, HWND) PURE;
    STDMETHOD(GetString)(THIS_ ASSOCF, ASSOCSTR, LPCWSTR, LPWSTR, DWORD *) PURE;
    STDMETHOD(GetKey)(THIS_ ASSOCF, ASSOCKEY, LPCWSTR, HKEY *) PURE;
    STDMETHOD(GetData)(THIS_ ASSOCF, ASSOCDATA, LPCWSTR, void *, DWORD *) PURE;
    STDMETHOD(GetEnum)(THIS_ ASSOCF, ASSOCENUM, LPCWSTR, REFIID, void **) PURE;
};

LWSTDAPI AssocCreate(CLSID, REFIID, void **);
LWSTDAPI AssocQueryStringA(ASSOCF, ASSOCSTR, LPCSTR, LPCSTR, LPSTR, DWORD *);
LWSTDAPI AssocQueryStringW(ASSOCF, ASSOCSTR, LPCWSTR, LPCWSTR, LPWSTR, DWORD *);
LWSTDAPI AssocQueryStringByKeyA(ASSOCF, ASSOCSTR, HKEY, LPCSTR, LPSTR, DWORD *);
LWSTDAPI AssocQueryStringByKeyW(ASSOCF, ASSOCSTR, HKEY, LPCWSTR, LPWSTR, DWORD *);
LWSTDAPI AssocQueryKeyA(ASSOCF, ASSOCKEY, LPCSTR, LPCSTR, HKEY *);
LWSTDAPI AssocQueryKeyW(ASSOCF, ASSOCKEY, LPCWSTR, LPCWSTR, HKEY *);

#ifdef UNICODE
#define AssocQueryString  AssocQueryStringW
#define AssocQueryStringByKey  AssocQueryStringByKeyW
#define AssocQueryKey  AssocQueryKeyW
#else /* !UNICODE */
#define AssocQueryString  AssocQueryStringA
#define AssocQueryStringByKey  AssocQueryStringByKeyA
#define AssocQueryKey  AssocQueryKeyA
#endif /* !UNICODE */

#if (_WIN32_IE >= 0x0601)
LWSTDAPI_(BOOL) AssocIsDangerous(LPCWSTR);
#endif  /* _WIN32_IE >= 0x0601 */

#if (_WIN32_IE >= 0x0602)

typedef int SRRF;

#define SRRF_RT_REG_NONE  0x00000001
#define SRRF_RT_REG_SZ  0x00000002
#define SRRF_RT_REG_EXPAND_SZ  0x00000004
#define SRRF_RT_REG_BINARY  0x00000008
#define SRRF_RT_REG_DWORD  0x00000010
#define SRRF_RT_REG_MULTI_SZ  0x00000020
#define SRRF_RT_REG_QWORD  0x00000040

#define SRRF_RT_DWORD  (SRRF_RT_REG_BINARY|SRRF_RT_REG_DWORD)
#define SRRF_RT_QWORD  (SRRF_RT_REG_BINARY|SRRF_RT_REG_QWORD)
#define SRRF_RT_ANY  0x0000ffff

#define SRRF_RM_ANY  0x00000000
#define SRRF_RM_NORMAL  0x00010000
#define SRRF_RM_SAFE  0x00020000
#define SRRF_RM_SAFENETWORK  0x00040000

#define SRRF_NOEXPAND  0x10000000
#define SRRF_ZEROONFAILURE  0x20000000
#define SRRF_NOVIRT  0x40000000

LWSTDAPI_(LSTATUS) SHRegGetValueA(HKEY, LPCSTR, LPCSTR, SRRF, DWORD *, void *, DWORD *);
LWSTDAPI_(LSTATUS) SHRegGetValueW(HKEY, LPCWSTR, LPCWSTR, SRRF, DWORD *, void *, DWORD *);
LWSTDAPI_(LSTATUS) SHRegSetValue(HKEY, LPCWSTR, LPCWSTR, SRRF, DWORD, LPCVOID, DWORD);
LWSTDAPI_(LSTATUS) SHRegGetValueFromHKCUHKLM(PCWSTR, PCWSTR, SRRF, DWORD *, void *, DWORD *);
STDAPI_(BOOL) SHRegGetBoolValueFromHKCUHKLM(PCWSTR, PCWSTR, BOOL);

#ifdef UNICODE
#define SHRegGetValue  SHRegGetValueW
#else /* !UNICODE */
#define SHRegGetValue  SHRegGetValueA
#endif /* !UNICODE */

#endif /* _WIN32_IE >= 0x0602 */

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
LWSTDAPI AssocGetPerceivedType(LPCWSTR, PERCEIVED *, PERCEIVEDFLAG *, LPWSTR *);
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */

#endif /* NO_SHLWAPI_REG */


#ifndef NO_SHLWAPI_STREAM  /* stream functions */

LWSTDAPI_(struct IStream *) SHOpenRegStreamA(HKEY, LPCSTR, LPCSTR, DWORD);
LWSTDAPI_(struct IStream *) SHOpenRegStreamW(HKEY, LPCWSTR, LPCWSTR, DWORD);
LWSTDAPI_(struct IStream *) SHOpenRegStream2A(HKEY, LPCSTR, LPCSTR, DWORD);
LWSTDAPI_(struct IStream *) SHOpenRegStream2W(HKEY, LPCWSTR, LPCWSTR, DWORD);
LWSTDAPI SHCreateStreamOnFileA(LPCSTR, DWORD, struct IStream **);
LWSTDAPI SHCreateStreamOnFileW(LPCWSTR, DWORD, struct IStream **);

#if (_WIN32_IE >= 0x0600)
LWSTDAPI SHCreateStreamOnFileEx(LPCWSTR, DWORD, DWORD, BOOL, struct IStream *, struct IStream **);
#endif /* _WIN32_IE >= 0x0600 */

#if (_WIN32_IE >= _WIN32_IE_IE501)
LWSTDAPI_(struct IStream *) SHCreateMemStream(const BYTE *, UINT);
#endif /* _WIN32_IE >= _WIN32_IE_IE501 */

#ifdef UNICODE
#define SHOpenRegStream  SHOpenRegStreamW
#define SHOpenRegStream2  SHOpenRegStream2W
#define SHCreateStreamOnFile  SHCreateStreamOnFileW
#else /* !UNICODE */
#define SHOpenRegStream  SHOpenRegStreamA
#define SHOpenRegStream2  SHOpenRegStream2A
#define SHCreateStreamOnFile  SHCreateStreamOnFileA
#endif /* !UNICODE */

#undef SHOpenRegStream  /* new code always(?) want new implementation */
#define SHOpenRegStream  SHOpenRegStream2

#endif /* NO_SHLWAPI_STREAM */


#ifndef NO_SHLWAPI_MLUI  /* multi-language UI functions */

#if (_WIN32_IE >= _WIN32_IE_IE501)

#define ML_NO_CROSSCODEPAGE  0
#define ML_CROSSCODEPAGE_NT  1
#define ML_CROSSCODEPAGE  2
#define ML_SHELL_LANGUAGE  4
#define ML_CROSSCODEPAGE_MASK  7

LWSTDAPI_(HINSTANCE) MLLoadLibraryA(LPCSTR, HMODULE, DWORD);
LWSTDAPI_(HINSTANCE) MLLoadLibraryW(LPCWSTR, HMODULE, DWORD);
LWSTDAPI_(BOOL) MLFreeLibrary(HMODULE);

#ifdef UNICODE
#define MLLoadLibrary  MLLoadLibraryW
#else /* !UNICODE */
#define MLLoadLibrary  MLLoadLibraryA
#endif /* !UNICODE */

#endif /* _WIN32_IE >= _WIN32_IE_IE501 */

#endif /* NO_SHLWAPI_MLUI */


#ifndef NO_SHLWAPI_HTTP  /* HTTP-helper functions */

#if (_WIN32_IE >= 0x0603)
LWSTDAPI GetAcceptLanguagesA(LPSTR, LPDWORD);
LWSTDAPI GetAcceptLanguagesW(LPWSTR, LPDWORD);
#ifdef UNICODE
#define GetAcceptLanguages  GetAcceptLanguagesW
#else /* !UNICODE */
#define GetAcceptLanguages  GetAcceptLanguagesA
#endif /* !UNICODE */
#endif /* _WIN32_IE >= 0x0603 */

#endif /* NO_SHLWAPI_HTTP */


#if (_WIN32_IE < _WIN32_IE_IE70) && !defined(NO_SHLWAPI_STOPWATCH)

#define SPMODE_SHELL  0x00000001
#define SPMODE_DEBUGOUT  0x00000002
#define SPMODE_TEST  0x00000004
#define SPMODE_BROWSER  0x00000008
#define SPMODE_FLUSH  0x00000010
#define SPMODE_EVENT  0x00000020
#define SPMODE_MSVM  0x00000040
#define SPMODE_FORMATTEXT  0x00000080
#define SPMODE_PROFILE  0x00000100
#define SPMODE_DEBUGBREAK  0x00000200
#define SPMODE_MSGTRACE  0x00000400
#define SPMODE_PERFTAGS  0x00000800
#define SPMODE_MEMWATCH  0x00001000
#define SPMODE_DBMON  0x00002000
#define SPMODE_MULTISTOP  0x00004000
#ifndef NO_ETW_TRACING
#define SPMODE_EVENTTRACE  0x00008000
#endif

DWORD WINAPI StopWatchMode(void);
DWORD WINAPI StopWatchFlush(void);

#endif /* (_WIN32_IE < _WIN32_IE_IE70) && !defined(NO_SHLWAPI_STOPWATCH) */


#ifndef __IConnectionPoint_FWD_DEFINED__
#define __IConnectionPoint_FWD_DEFINED__
typedef struct IConnectionPoint IConnectionPoint;
#endif /* __IConnectionPoint_FWD_DEFINED__ */

#if (_WIN32_IE >= _WIN32_IE_IE501)
LWSTDAPI_(void) IUnknown_Set(IUnknown **, IUnknown *);
LWSTDAPI_(void) IUnknown_AtomicRelease(void **);
LWSTDAPI IUnknown_GetWindow(IUnknown *, HWND *);
LWSTDAPI IUnknown_SetSite(IUnknown *, IUnknown *);
LWSTDAPI IUnknown_GetSite(IUnknown *, REFIID, void **);
LWSTDAPI IUnknown_QueryService(IUnknown *, REFGUID, REFIID, void **);
#endif /* _WIN32_IE >= _WIN32_IE_IE501 */


#if (_WIN32_IE >= _WIN32_IE_IE501)

#if !defined(__cplusplus) && defined(COBJMACROS)
#undef IStream_Read
#undef IStream_Write
#endif /* COBJMACROS */

LWSTDAPI IStream_Read(IStream *, void *, ULONG);
LWSTDAPI IStream_Write(IStream *, const void *, ULONG);
LWSTDAPI IStream_Reset(IStream *);
LWSTDAPI IStream_Size(IStream *, ULARGE_INTEGER *);

LWSTDAPI ConnectToConnectionPoint(IUnknown *, REFIID, BOOL, IUnknown *, DWORD *, IConnectionPoint **);

#endif /* _WIN32_IE >= _WIN32_IE_IE501 */

#if (_WIN32_IE >= _WIN32_IE_IE60)
LWSTDAPI IStream_ReadPidl(IStream *, PIDLIST_RELATIVE *);
LWSTDAPI IStream_WritePidl(IStream *, PCUIDLIST_RELATIVE);
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */

#if (_WIN32_IE >= _WIN32_IE_IE70)
LWSTDAPI IStream_ReadStr(IStream *, PWSTR *);
LWSTDAPI IStream_WriteStr(IStream *, PCWSTR);
LWSTDAPI IStream_Copy(IStream *, IStream *, DWORD);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */

#if (_WIN32_IE >= 0x0600)
#define SHGVSPB_PERUSER  0x00000001
#define SHGVSPB_ALLUSERS  0x00000002
#define SHGVSPB_PERFOLDER  0x00000004
#define SHGVSPB_ALLFOLDERS  0x00000008
#define SHGVSPB_INHERIT  0x00000010
#define SHGVSPB_ROAM  0x00000020
#define SHGVSPB_NOAUTODEFAULTS  0x80000000

#define SHGVSPB_FOLDER  (SHGVSPB_PERUSER | SHGVSPB_PERFOLDER)
#define SHGVSPB_FOLDERNODEFAULTS  (SHGVSPB_PERUSER | SHGVSPB_PERFOLDER | SHGVSPB_NOAUTODEFAULTS)
#define SHGVSPB_USERDEFAULTS  (SHGVSPB_PERUSER | SHGVSPB_ALLFOLDERS)
#define SHGVSPB_GLOBALDEAFAULTS  (SHGVSPB_ALLUSERS | SHGVSPB_ALLFOLDERS)

LWSTDAPI SHGetViewStatePropertyBag(PCIDLIST_ABSOLUTE, LPCWSTR , DWORD , REFIID, void**);
#endif /* _WIN32_IE >= 0x0600 */

#define FDTF_SHORTTIME  0x00000001
#define FDTF_SHORTDATE  0x00000002
#define FDTF_DEFAULT  (FDTF_SHORTDATE | FDTF_SHORTTIME)
#define FDTF_LONGDATE  0x00000004
#define FDTF_LONGTIME  0x00000008
#define FDTF_RELATIVE  0x00000010
#define FDTF_LTRDATE  0x00000100
#define FDTF_RTLDATE  0x00000200
#define FDTF_NOAUTOREADINGORDER 0x00000400

LWSTDAPI_(int) SHFormatDateTimeA(const FILETIME UNALIGNED *, DWORD *, LPSTR, UINT);
LWSTDAPI_(int) SHFormatDateTimeW(const FILETIME UNALIGNED *, DWORD *, LPWSTR, UINT);
#ifdef UNICODE
#define SHFormatDateTime  SHFormatDateTimeW
#else /* !UNICODE */
#define SHFormatDateTime  SHFormatDateTimeA
#endif /* !UNICODE */

#if (_WIN32_IE >= _WIN32_IE_IE501)

LWSTDAPI_(int) SHAnsiToUnicode(LPCSTR, LPWSTR, int);
LWSTDAPI_(int) SHAnsiToAnsi(LPCSTR, LPSTR, int);
LWSTDAPI_(int) SHUnicodeToAnsi(LPCWSTR, LPSTR, int);
LWSTDAPI_(int) SHUnicodeToUnicode(LPCWSTR, LPWSTR, int);

#ifdef UNICODE
#define SHTCharToUnicode(wzSrc, wzDest, cchSize)  SHUnicodeToUnicode(wzSrc, wzDest, cchSize)
#define SHTCharToAnsi(wzSrc, szDest, cchSize)  SHUnicodeToAnsi(wzSrc, szDest, cchSize)
#define SHUnicodeToTChar(wzSrc, wzDest, cchSize)  SHUnicodeToUnicode(wzSrc, wzDest, cchSize)
#define SHAnsiToTChar(szSrc, wzDest, cchSize)  SHAnsiToUnicode(szSrc, wzDest, cchSize)
#else /* !UNICODE */
#define SHTCharToUnicode(szSrc, wzDest, cchSize)  SHAnsiToUnicode(szSrc, wzDest, cchSize)
#define SHTCharToAnsi(szSrc, szDest, cchSize)  SHAnsiToAnsi(szSrc, szDest, cchSize)
#define SHUnicodeToTChar(wzSrc, szDest, cchSize)  SHUnicodeToAnsi(wzSrc, szDest, cchSize)
#define SHAnsiToTChar(szSrc, szDest, cchSize)  SHAnsiToAnsi(szSrc, szDest, cchSize)
#endif /* !UNICODE */

#endif /* _WIN32_IE >= _WIN32_IE_IE501 */


#if (_WIN32_IE >= _WIN32_IE_IE501)
LWSTDAPI_(int) SHMessageBoxCheckA(HWND, LPCSTR, LPCSTR, UINT, int, LPCSTR);
LWSTDAPI_(int) SHMessageBoxCheckW(HWND, LPCWSTR, LPCWSTR, UINT, int, LPCWSTR);
#ifdef UNICODE
#define SHMessageBoxCheck  SHMessageBoxCheckW
#else /* !UNICODE */
#define SHMessageBoxCheck  SHMessageBoxCheckA
#endif /* !UNICODE */
#endif /* _WIN32_IE >= _WIN32_IE_IE501 */


#if (_WIN32_IE >= _WIN32_IE_IE501)

#define OS_WINDOWS  0
#define OS_NT  1
#define OS_WIN95ORGREATER  2
#define OS_NT4ORGREATER  3
#define OS_WIN98ORGREATER  5
#define OS_WIN98_GOLD  6
#define OS_WIN2000ORGREATER  7

#define OS_WIN2000PRO  8
#define OS_WIN2000SERVER  9
#define OS_WIN2000ADVSERVER  10
#define OS_WIN2000DATACENTER  11
#define OS_WIN2000TERMINAL  12

#define OS_EMBEDDED  13
#define OS_TERMINALCLIENT  14
#define OS_TERMINALREMOTEADMIN  15
#define OS_WIN95_GOLD  16
#define OS_MEORGREATER  17
#define OS_XPORGREATER  18
#define OS_HOME  19
#define OS_PROFESSIONAL  20
#define OS_DATACENTER  21
#define OS_ADVSERVER  22
#define OS_SERVER  23
#define OS_TERMINALSERVER  24
#define OS_PERSONALTERMINALSERVER  25
#define OS_FASTUSERSWITCHING  26
#define OS_WELCOMELOGONUI  27
#define OS_DOMAINMEMBER  28
#define OS_ANYSERVER  29
#define OS_WOW6432  30
#define OS_WEBSERVER  31
#define OS_SMALLBUSINESSSERVER  32
#define OS_TABLETPC  33
#define OS_SERVERADMINUI  34
#define OS_MEDIACENTER  35
#define OS_APPLIANCE  36

LWSTDAPI_(LRESULT) SHSendMessageBroadcastA(UINT, WPARAM, LPARAM);
LWSTDAPI_(LRESULT) SHSendMessageBroadcastW(UINT, WPARAM, LPARAM);
LWSTDAPI_(CHAR) SHStripMneumonicA(LPSTR);
LWSTDAPI_(WCHAR) SHStripMneumonicW(LPWSTR);
LWSTDAPI_(BOOL) IsOS(DWORD dwOS);

#ifdef UNICODE
#define SHSendMessageBroadcast  SHSendMessageBroadcastW
#define SHStripMneumonic  SHStripMneumonicW
#else /* !UNICODE */
#define SHSendMessageBroadcast  SHSendMessageBroadcastA
#define SHStripMneumonic  SHStripMneumonicA
#endif /* !UNICODE */

#endif /* _WIN32_IE >= _WIN32_IE_IE501 */

typedef enum {
    GLOBALCOUNTER_SEARCHMANAGER,
    GLOBALCOUNTER_SEARCHOPTIONS,
    GLOBALCOUNTER_FOLDERSETTINGSCHANGE,
    GLOBALCOUNTER_RATINGS,
    GLOBALCOUNTER_APPROVEDSITES,
    GLOBALCOUNTER_RESTRICTIONS,
    GLOBALCOUNTER_SHELLSETTINGSCHANGED,
    GLOBALCOUNTER_SYSTEMPIDLCHANGE,
    GLOBALCOUNTER_OVERLAYMANAGER,
    GLOBALCOUNTER_QUERYASSOCIATIONS,
    GLOBALCOUNTER_IESESSIONS,
    GLOBALCOUNTER_IEONLY_SESSIONS,
    GLOBALCOUNTER_APPLICATION_DESTINATIONS,
    __UNUSED_RECYCLE_WAS_GLOBALCOUNTER_CSCSYNCINPROGRESS,
    GLOBALCOUNTER_BITBUCKETNUMDELETERS,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_SHARES,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_A,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_B,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_C,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_D,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_E,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_F,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_G,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_H,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_I,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_J,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_K,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_L,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_M,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_N,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_O,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_P,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_Q,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_R,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_S,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_T,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_U,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_V,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_W,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_X,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_Y,
    GLOBALCOUNTER_RECYCLEDIRTYCOUNT_DRIVE_Z,
    __UNUSED_RECYCLE_WAS_GLOBALCOUNTER_RECYCLEDIRTYCOUNT_SERVERDRIVE,
    __UNUSED_RECYCLE_WAS_GLOBALCOUNTER_RECYCLEGLOBALDIRTYCOUNT,
    GLOBALCOUNTER_RECYCLEBINENUM,
    GLOBALCOUNTER_RECYCLEBINCORRUPTED,
    GLOBALCOUNTER_RATINGS_STATECOUNTER,
    GLOBALCOUNTER_PRIVATE_PROFILE_CACHE,
    GLOBALCOUNTER_INTERNETTOOLBAR_LAYOUT,
    GLOBALCOUNTER_FOLDERDEFINITION_CACHE,
    GLOBALCOUNTER_COMMONPLACES_LIST_CACHE,
    GLOBALCOUNTER_PRIVATE_PROFILE_CACHE_MACHINEWIDE,
    GLOBALCOUNTER_ASSOCCHANGED,
#if (NTDDI_VERSION >= NTDDI_WIN8)
    GLOBALCOUNTER_APP_ITEMS_STATE_STORE_CACHE,
    GLOBALCOUNTER_SETTINGSYNC_ENABLED,
    GLOBALCOUNTER_APPSFOLDER_FILETYPEASSOCIATION_COUNTER,
    GLOBALCOUNTER_USERINFOCHANGED,
#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    GLOBALCOUNTER_SYNC_ENGINE_INFORMATION_CACHE_MACHINEWIDE,
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */
    GLOBALCOUNTER_MAXIMUMVALUE
} SHGLOBALCOUNTER;

#if (NTDDI_VERSION >= NTDDI_WIN7)
LWSTDAPI_(long) SHGlobalCounterGetValue(const SHGLOBALCOUNTER id);
LWSTDAPI_(long) SHGlobalCounterIncrement(const SHGLOBALCOUNTER id);
LWSTDAPI_(long) SHGlobalCounterDecrement(const SHGLOBALCOUNTER id);
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#if (_WIN32_IE >= 0x0603)
LWSTDAPI_(HANDLE)SHAllocShared(const void *, DWORD, DWORD);
LWSTDAPI_(BOOL)SHFreeShared(HANDLE, DWORD);
LWSTDAPI_(void *) SHLockShared(HANDLE, DWORD);
LWSTDAPI_(BOOL)SHUnlockShared(void *);
#endif /* _WIN32_IE >= 0x0603 */

#if (_WIN32_IE >= _WIN32_IE_IE501)

#define PLATFORM_UNKNOWN  0
#define PLATFORM_IE3  1
#define PLATFORM_BROWSERONLY  1
#define PLATFORM_INTEGRATED  2

typedef struct {
    const IID * piid;
    int dwOffset;
} QITAB, *LPQITAB;
typedef const QITAB *LPCQITAB;

#ifdef __cplusplus
#define QITABENTMULTI(Cthis,Ifoo,Iimpl)  { &__uuidof(Ifoo), OFFSETOFCLASS(Iimpl, Cthis) }
#else /* !__cplusplus */
#define QITABENTMULTI(Cthis,Ifoo,Iimpl)  { (IID*)&IID_##Ifoo, OFFSETOFCLASS(Iimpl, Cthis) }
#endif /* !__cplusplus */

#define QITABENTMULTI2(Cthis,Ifoo,Iimpl)  { (IID*)&Ifoo, OFFSETOFCLASS(Iimpl, Cthis) }
#define QITABENT(Cthis,Ifoo) QITABENTMULTI(Cthis, Ifoo, Ifoo)

LWSTDAPI_(UINT) WhichPlatform(void);
STDAPI QISearch(void *, LPCQITAB, REFIID, void **);

#endif /* _WIN32_IE >= _WIN32_IE_IE501 */

#ifndef STATIC_CAST
#define STATIC_CAST(typ)  (typ)
#endif /* STATIC_CAST */

#ifndef OFFSETOFCLASS
#define OFFSETOFCLASS(base,derived)  ((DWORD)(DWORD_PTR)(STATIC_CAST(base*)((derived*)8)) - 8)
#endif /* OFFSETOFCLASS */

#define ILMM_IE4  0

LWSTDAPI_(BOOL)SHIsLowMemoryMachine(DWORD);
LWSTDAPI_(int) GetMenuPosFromID(HMENU, UINT);
LWSTDAPI SHGetInverseCMAP(BYTE *, ULONG);

#if (_WIN32_IE >= 0x0500)

#define SHACF_DEFAULT  0x00000000
#define SHACF_FILESYSTEM  0x00000001
#define SHACF_URLALL  (SHACF_URLHISTORY | SHACF_URLMRU)
#define SHACF_URLHISTORY  0x00000002
#define SHACF_URLMRU  0x00000004
#define SHACF_USETAB  0x00000008
#define SHACF_FILESYS_ONLY  0x00000010
#if (_WIN32_IE >= 0x0600)
#define SHACF_FILESYS_DIRS  0x00000020
#endif /* _WIN32_IE >= 0x0600 */
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define SHACF_VIRTUAL_NAMESPACE  0x00000040
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define SHACF_AUTOSUGGEST_FORCE_ON  0x10000000
#define SHACF_AUTOSUGGEST_FORCE_OFF  0x20000000
#define SHACF_AUTOAPPEND_FORCE_ON  0x40000000
#define SHACF_AUTOAPPEND_FORCE_OFF  0x80000000

LWSTDAPI SHAutoComplete(HWND, DWORD);

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
LWSTDAPI SHCreateThreadRef(LONG *, IUnknown **);
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
LWSTDAPI SHSetThreadRef(IUnknown *);
LWSTDAPI SHGetThreadRef(IUnknown **);

LWSTDAPI_(BOOL)SHSkipJunction(struct IBindCtx *, const CLSID *);

#endif /* _WIN32_IE >= 0x0500 */

enum
{
    CTF_INSIST              = 0x00000001,   // call pfnThreadProc synchronously if CreateThread() fails
    CTF_THREAD_REF          = 0x00000002,   // hold a reference to the creating thread
    CTF_PROCESS_REF         = 0x00000004,   // hold a reference to the creating process
    CTF_COINIT_STA          = 0x00000008,   // init COM as STA for the created thread
    CTF_COINIT              = 0x00000008,   // init COM as STA for the created thread
#if (_WIN32_IE >= _WIN32_IE_IE60)
    CTF_FREELIBANDEXIT      = 0x00000010,   // hold a ref to the DLL and call FreeLibraryAndExitThread() when done
    CTF_REF_COUNTED         = 0x00000020,   // thread supports ref counting via SHGetThreadRef() or CTF_THREAD_REF so that child threads can keep this thread alive
    CTF_WAIT_ALLOWCOM       = 0x00000040,   // while waiting for pfnCallback, allow COM marshaling to the blocked calling thread
#endif // _WIN32_IE_IE60
#if (_WIN32_IE >= _WIN32_IE_IE70)
    CTF_UNUSED              = 0x00000080,
    CTF_INHERITWOW64        = 0x00000100,   // new thread should inherit the wow64 disable state for the file system redirector
#endif // _WIN32_IE_IE70
#if (NTDDI_VERSION >= NTDDI_VISTA)
    CTF_WAIT_NO_REENTRANCY  = 0x00000200,   // don't allow re-entrancy when waiting for the sync proc, this won't work with marshalled objects or SendMessages() from the sync proc
#endif // (NTDDI_VERSION >= NTDDI_VISTA)
#if (NTDDI_VERSION >= NTDDI_WIN7)
    CTF_KEYBOARD_LOCALE     = 0x00000400,   // carry the keyboard locale from creating to created thread
    CTF_OLEINITIALIZE       = 0x00000800,   // init OLE on the created thread (this will also init COM as STA)
    CTF_COINIT_MTA          = 0x00001000,   // init COM as MTA for the created thread
    CTF_NOADDREFLIB         = 0x00002000,   // this flag is the opposite of CTF_FREELIBANDEXIT that is now implicit as of Vista
                                            // this avoids the LoadLibrary/FreeLibraryAndExitThread calls that result in contention for the loader lock
                                            // only use this when the thread being created has some other means to ensure that the code
                                            // of the thread proc will remain loaded. This should not be used in the context of COM objects as those
                                            // need to ensure that the DLL stays loaded as COM will unload DLLs
#endif // (NTDDI_VERSION >= NTDDI_WIN7)
};

typedef DWORD SHCT_FLAGS;   // SHCreateThread flags values

LWSTDAPI_(BOOL) SHCreateThread(LPTHREAD_START_ROUTINE, void *, DWORD, LPTHREAD_START_ROUTINE);
LWSTDAPI_(BOOL) SHCreateThreadWithHandle(LPTHREAD_START_ROUTINE, void *, SHCT_FLAGS, LPTHREAD_START_ROUTINE, HANDLE *);

#if (NTDDI_VERSION >= NTDDI_WIN8)
LWSTDAPI_(void) SetProcessReference(IUnknown *);
LWSTDAPI GetProcessReference(IUnknown **);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#if (_WIN32_IE >= _WIN32_IE_IE60)
LWSTDAPI SHReleaseThreadRef(void);
#endif /* _WIN32_IE >= _WIN32_IE_IE60 */


#ifndef NO_SHLWAPI_GDI  /* GDI helper functions */

LWSTDAPI_(HPALETTE) SHCreateShellPalette(HDC);

#if (_WIN32_IE >= 0x0500)
LWSTDAPI_(void) ColorRGBToHLS(COLORREF, WORD *, WORD *, WORD *);
LWSTDAPI_(COLORREF)ColorHLSToRGB(WORD, WORD, WORD);
LWSTDAPI_(COLORREF)ColorAdjustLuma(COLORREF, int, BOOL);
#endif /* _WIN32_IE >= 0x0500 */

#endif /* NO_SHLWAPI_GDI */

/* DllGetVersion: */

#define DLLVER_PLATFORM_WINDOWS  0x00000001
#define DLLVER_PLATFORM_NT  0x00000002

typedef struct _DLLVERSIONINFO {
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
} DLLVERSIONINFO;

#if (_WIN32_IE >= 0x0501)
typedef struct _DLLVERSIONINFO2 {
    DLLVERSIONINFO info1;
    DWORD dwFlags;
    ULONGLONG ullVersion;
} DLLVERSIONINFO2;

#define DLLVER_MAJOR_MASK  0xFFFF000000000000
#define DLLVER_MINOR_MASK  0x0000FFFF00000000
#define DLLVER_BUILD_MASK  0x00000000FFFF0000
#define DLLVER_QFE_MASK  0x000000000000FFFF
#endif /* _WIN32_IE >= 0x0501 */

#define MAKEDLLVERULL(major,minor,build,qfe)  (((ULONGLONG)(major)<<48)|((ULONGLONG)(minor)<<32)|((ULONGLONG)(build)<<16)|((ULONGLONG)(qfe)<<0))

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);

STDAPI DllInstall(BOOL, LPCWSTR);

#if (_WIN32_IE >= 0x0602)
LWSTDAPI_(BOOL) IsInternetESCEnabled(void);
#endif /* _WIN32_IE >= 0x0602 */

#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif /* NOSHLWAPI */

#endif /* _SHLWAPI_H */
