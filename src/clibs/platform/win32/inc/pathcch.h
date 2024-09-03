#ifndef _PATHCCH_H
#define _PATHCCH_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-Path-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#define PATHCCH_ALLOW_LONG_PATHS  0x00000001

#define VOLUME_PREFIX      L"\\\\?\\Volume"
#define VOLUME_PREFIX_LEN  (ARRAYSIZE(VOLUME_PREFIX) - 1)

#define PATHCCH_MAX_CCH  0x8000

WINPATHCCHAPI BOOL APIENTRY PathIsUNCEx(PCWSTR, PCWSTR *);
WINPATHCCHAPI BOOL APIENTRY PathCchIsRoot(PCWSTR);
WINPATHCCHAPI HRESULT APIENTRY PathCchAddBackslashEx(PWSTR, size_t, PWSTR *, size_t *);
WINPATHCCHAPI HRESULT APIENTRY PathCchAddBackslash(PWSTR, size_t);
WINPATHCCHAPI HRESULT APIENTRY PathCchRemoveBackslashEx(PWSTR, size_t, PWSTR *, size_t *);
WINPATHCCHAPI HRESULT APIENTRY PathCchRemoveBackslash(PWSTR, size_t);
WINPATHCCHAPI HRESULT APIENTRY PathCchSkipRoot(PCWSTR, PCWSTR *);
WINPATHCCHAPI HRESULT APIENTRY PathCchStripToRoot(PWSTR, size_t);
WINPATHCCHAPI HRESULT APIENTRY PathCchRemoveFileSpec(PWSTR, size_t);
WINPATHCCHAPI HRESULT APIENTRY PathCchFindExtension(PCWSTR, size_t, PCWSTR *);
WINPATHCCHAPI HRESULT APIENTRY PathCchAddExtension(PWSTR, size_t, PCWSTR);
WINPATHCCHAPI HRESULT APIENTRY PathCchRenameExtension(PWSTR, size_t, PCWSTR);
WINPATHCCHAPI HRESULT APIENTRY PathCchRemoveExtension(PWSTR, size_t);
WINPATHCCHAPI HRESULT APIENTRY PathCchCanonicalizeEx(PWSTR, size_t, PCWSTR, unsigned long);
WINPATHCCHAPI HRESULT APIENTRY PathCchCanonicalize(PWSTR, size_t, PCWSTR);
WINPATHCCHAPI HRESULT APIENTRY PathCchCombineEx(PWSTR, size_t, PCWSTR, PCWSTR, unsigned long);
WINPATHCCHAPI HRESULT APIENTRY PathCchCombine(PWSTR, size_t, PCWSTR, PCWSTR);
WINPATHCCHAPI HRESULT APIENTRY PathCchAppendEx(PWSTR, size_t, PCWSTR, unsigned long);
WINPATHCCHAPI HRESULT APIENTRY PathCchAppend(PWSTR, size_t, PCWSTR);
WINPATHCCHAPI HRESULT APIENTRY PathCchStripPrefix(PWSTR, size_t);
WINPATHCCHAPI HRESULT APIENTRY PathAllocCombine(PCWSTR, PCWSTR, unsigned long, PWSTR *);
WINPATHCCHAPI HRESULT APIENTRY PathAllocCanonicalize(PCWSTR pszPathIn, unsigned long dwFlags, PWSTR *ppszPathOut);

#ifndef PATHCCH_NO_DEPRECATE

#undef PathAddBackslash
#undef PathAddBackslashA
#undef PathAddBackslashW

#undef PathAddExtension
#undef PathAddExtensionA
#undef PathAddExtensionW

#undef PathAppend
#undef PathAppendA
#undef PathAppendW

#undef PathCanonicalize
#undef PathCanonicalizeA
#undef PathCanonicalizeW

#undef PathCombine
#undef PathCombineA
#undef PathCombineW

#undef PathRenameExtension
#undef PathRenameExtensionA
#undef PathRenameExtensionW

#ifdef DEPRECATE_SUPPORTED

#pragma deprecated(PathIsRelativeWorker)
#pragma deprecated(StrIsEqualWorker)
#pragma deprecated(FindPreviousBackslashWorker)
#pragma deprecated(IsHexDigitWorker)
#pragma deprecated(StringIsGUIDWorker)
#pragma deprecated(PathIsVolumeGUIDWorker)
#pragma deprecated(IsValidExtensionWorker)

#pragma deprecated(PathAddBackslash)
#pragma deprecated(PathAddBackslashA)
#pragma deprecated(PathAddBackslashW)

#pragma deprecated(PathAddExtension)
#pragma deprecated(PathAddExtensionA)
#pragma deprecated(PathAddExtensionW)

#pragma deprecated(PathAppend)
#pragma deprecated(PathAppendA)
#pragma deprecated(PathAppendW)

#pragma deprecated(PathCanonicalize)
#pragma deprecated(PathCanonicalizeA)
#pragma deprecated(PathCanonicalizeW)

#pragma deprecated(PathCombine)
#pragma deprecated(PathCombineA)
#pragma deprecated(PathCombineW)

#pragma deprecated(PathRenameExtension)
#pragma deprecated(PathRenameExtensionA)
#pragma deprecated(PathRenameExtensionW)

#else /* !DEPRECATE_SUPPORTED */

#define PathIsRelativeWorker    PathIsRelativeWorker_is_internal_to_pathcch;
#define StrIsEqualWorker        StrIsEqualWorker_is_internal_to_pathcch;
#define FindPreviousBackslashWorker FindPreviousBackslashWorker_is_internal_to_pathcch;
#define IsHexDigitWorker        IsHexDigitWorker_is_internal_to_pathcch;
#define StringIsGUIDWorker      StringIsGUIDWorker_is_internal_to_pathcch;
#define PathIsVolumeGUIDWorker  PathIsVolumeGUIDWorker_is_internal_to_pathcch;
#define IsValidExtensionWorker  IsValidExtensionWorker_is_internal_to_pathcch;

#define PathAddBackslash        PathAddBackslash_instead_use_PathCchAddBackslash;
#define PathAddBackslashA       PathAddBackslash_instead_use_PathCchAddBackslash;
#define PathAddBackslashW       PathAddBackslash_instead_use_PathCchAddBackslash;

#define PathAddExtension        PathAddExtension_instead_use_PathCchAddExtension;
#define PathAddExtensionA       PathAddExtension_instead_use_PathCchAddExtension;
#define PathAddExtensionW       PathAddExtension_instead_use_PathCchAddExtension;

#define PathAppend              PathAppend_instead_use_PathCchAppend;
#define PathAppendA             PathAppend_instead_use_PathCchAppend;
#define PathAppendW             PathAppend_instead_use_PathCchAppend;

#define PathCanonicalize        PathCanonicalize_instead_use_PathCchCanonicalize;
#define PathCanonicalizeA       PathCanonicalize_instead_use_PathCchCanonicalize;
#define PathCanonicalizeW       PathCanonicalize_instead_use_PathCchCanonicalize;

#define PathCombine             PathCombine_instead_use_PathCchCombine;
#define PathCombineA            PathCombine_instead_use_PathCchCombine;
#define PathCombineW            PathCombine_instead_use_PathCchCombine;

#define PathRenameExtension     PathRenameExtension_instead_use_PathCchRenameExtension;
#define PathRenameExtensionA    PathRenameExtension_instead_use_PathCchRenameExtension;
#define PathRenameExtensionW    PathRenameExtension_instead_use_PathCchRenameExtension;

#endif /* !DEPRECATE_SUPPORTED */

#endif /* PATHCCH_NO_DEPRECATE */

#endif /* _PATHCCH_H */
