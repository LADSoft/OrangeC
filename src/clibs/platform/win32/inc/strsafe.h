#ifndef _STRSAFE_H
#define _STRSAFE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* "Safer" string function definitions */

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>

#if defined(_M_AMD64)
#define ALIGNMENT_MACHINE
#define UNALIGNED __unaligned
#else
#undef ALIGNMENT_MACHINE
#define UNALIGNED
#endif

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif /* _HRESULT_DEFINED */

typedef unsigned long DWORD;

#define SUCCEEDED(hr)  (((HRESULT)(hr)) >= 0)
#define FAILED(hr)     (((HRESULT)(hr)) < 0)

#define S_OK  ((HRESULT)0L)

#define STRSAFEAPI   inline HRESULT __stdcall
#define STRSAFEAPIV  inline HRESULT __cdecl
#define STRSAFEWORKERAPI  static STRSAFEAPI

#ifdef STRSAFE_LOCALE_FUNCTIONS
#error No support for STRSAFE_LOCALE_FUNCTIONS !!
#endif /* STRSAFE_LOCALE_FUNCTIONS */

#if defined(STRSAFE_NO_CB_FUNCTIONS) && defined(STRSAFE_NO_CCH_FUNCTIONS)
#error Invalid combination of STRSAFE_NO_CB_FUNCTIONS and STRSAFE_NO_CCH_FUNCTIONS !!
#endif

#ifndef STRSAFE_MAX_CCH
#define STRSAFE_MAX_CCH  0x7fffffff /* 2147483647 */
#endif /* STRSAFE_MAX_CCH */

#if __STDC_VERSION__ >= 201101L
_Static_assert(STRSAFE_MAX_CCH <= 2147483647, "STRSAFE_MAX_CCH is too big !!");
_Static_assert(STRSAFE_MAX_CCH > 1, "STRSAFE_MAX_CCH is too small !!");
#endif

#define STRSAFE_MAX_LENGTH  (STRSAFE_MAX_CCH - 1)

/*      STRSAFE_FILL_BYTE(0xFF)   0x000000FF */
#define STRSAFE_IGNORE_NULLS      0x00000100
#define STRSAFE_FILL_BEHIND_NULL  0x00000200
#define STRSAFE_FILL_ON_FAILURE   0x00000400
#define STRSAFE_NULL_ON_FAILURE   0x00000800
#define STRSAFE_NO_TRUNCATION     0x00001000
#define STRSAFE_VALID_FLAGS  (0x000000FF|STRSAFE_IGNORE_NULLS|STRSAFE_FILL_BEHIND_NULL|STRSAFE_FILL_ON_FAILURE|STRSAFE_NULL_ON_FAILURE|STRSAFE_NO_TRUNCATION)

#define STRSAFE_FILL_BYTE(x)               ((DWORD)((x & 0x000000FF) | STRSAFE_FILL_BEHIND_NULL))
#define STRSAFE_FAILURE_BYTE(x)            ((DWORD)((x & 0x000000FF) | STRSAFE_FILL_ON_FAILURE))
#define STRSAFE_GET_FILL_PATTERN(dwFlags)  ((int)(dwFlags & 0x000000FF))

#define STRSAFE_E_INSUFFICIENT_BUFFER  ((HRESULT)0x8007007AL)
#define STRSAFE_E_INVALID_PARAMETER    ((HRESULT)0x80070057L)
#define STRSAFE_E_END_OF_FILE          ((HRESULT)0x80070026L)

typedef char *STRSAFE_LPSTR;
typedef const char *STRSAFE_LPCSTR;
typedef wchar_t *STRSAFE_LPWSTR;
typedef const wchar_t *STRSAFE_LPCWSTR;
typedef const wchar_t UNALIGNED *STRSAFE_LPCUWSTR;

typedef const char *STRSAFE_PCNZCH;
typedef const wchar_t *STRSAFE_PCNZWCH;
typedef const wchar_t UNALIGNED *STRSAFE_PCUNZWCH;

STRSAFEAPI StringGetsWorkerA(STRSAFE_LPSTR pszDest, size_t cchDest, size_t * pcchNewDestLength) {
    HRESULT hr = S_OK;
    size_t cchNewDestLength = 0;
    if (cchDest == 1) {
        *pszDest = '\0';
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    } else {
        while (cchDest > 1) {
            char ch;
            int i = getc(stdin);
            if (i == EOF) {
                if (cchNewDestLength == 0) {
                    hr = STRSAFE_E_END_OF_FILE;
                }
                break;
            }
            ch = (char)i;
            if (ch == '\n') {
                break;
            }
            *pszDest++ = ch;
            cchDest--;
            cchNewDestLength++;
        }
        *pszDest = '\0';
    }
    if (pcchNewDestLength) {
        *pcchNewDestLength = cchNewDestLength;
    }
    return hr;
}

STRSAFEAPI StringGetsWorkerW(STRSAFE_LPWSTR pszDest, size_t cchDest, size_t * pcchNewDestLength) {
    HRESULT hr = S_OK;
    size_t cchNewDestLength = 0;
    if (cchDest == 1) {
        *pszDest = '\0';
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    } else {
        while (cchDest > 1) {
            wchar_t ch = getwc(stdin);
            if (ch == WEOF) {
                if (cchNewDestLength == 0) {
                    hr = STRSAFE_E_END_OF_FILE;
                }
                break;
            }
            if (ch == L'\n') {
                break;
            }
            *pszDest++ = ch;
            cchDest--;
            cchNewDestLength++;
        }
        *pszDest = L'\0';
    }
    if (pcchNewDestLength) {
        *pcchNewDestLength = cchNewDestLength;
    }
    return hr;
}


STRSAFEWORKERAPI _StringLengthWorkerA(STRSAFE_PCNZCH psz, size_t cchMax, size_t *pcchLength)
{
    HRESULT hr = S_OK;
    size_t cchOriginalMax = cchMax;
    while (cchMax && (*psz != '\0')) {
        psz++;
        cchMax--;
    }
    if (cchMax == 0) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    if (pcchLength) {
        if (SUCCEEDED(hr)) {
            *pcchLength = cchOriginalMax - cchMax;
        } else {
            *pcchLength = 0;
        }
    }
    return hr;
}

STRSAFEWORKERAPI _StringLengthWorkerW(STRSAFE_PCNZWCH psz, size_t cchMax, size_t *pcchLength)
{
    HRESULT hr = S_OK;
    size_t cchOriginalMax = cchMax;
    while (cchMax && (*psz != L'\0')) {
        psz++;
        cchMax--;
    }
    if (cchMax == 0) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    if (pcchLength) {
        if (SUCCEEDED(hr)) {
            *pcchLength = cchOriginalMax - cchMax;
        } else {
            *pcchLength = 0;
        }
    }
    return hr;
}

#ifdef ALIGNMENT_MACHINE
STRSAFEWORKERAPI _UnalignedStringLengthWorkerW(STRSAFE_PCUNZWCH psz, size_t cchMax, size_t *pcchLength)
{
    HRESULT hr = S_OK;
    size_t cchOriginalMax = cchMax;
    while (cchMax && (*psz != L'\0')) {
        psz++;
        cchMax--;
    }
    if (cchMax == 0) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    if (pcchLength) {
        if (SUCCEEDED(hr)) {
            *pcchLength = cchOriginalMax - cchMax;
        } else {
            *pcchLength = 0;
        }
    }
    return hr;
}
#endif /* ALIGNMENT_MACHINE */

STRSAFEWORKERAPI _StringExValidateSrcA(STRSAFE_PCNZCH *ppszSrc, size_t *pcchToRead, const size_t cchMax, DWORD dwFlags)
{
    HRESULT hr = S_OK;
    if (pcchToRead && (*pcchToRead >= cchMax)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else if ((dwFlags & STRSAFE_IGNORE_NULLS) && (*ppszSrc == NULL)) {
        *ppszSrc = "";
        if (pcchToRead) {
            *pcchToRead = 0;
        }
    }
    return hr;
}

STRSAFEWORKERAPI _StringExValidateSrcW(STRSAFE_PCNZWCH *ppszSrc, size_t *pcchToRead, const size_t cchMax, DWORD dwFlags)
{
    HRESULT hr = S_OK;
    if (pcchToRead && (*pcchToRead >= cchMax)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else if ((dwFlags & STRSAFE_IGNORE_NULLS) && (*ppszSrc == NULL)) {
        *ppszSrc = L"";

        if (pcchToRead) {
            *pcchToRead = 0;
        }
    }
    return hr;
}

STRSAFEWORKERAPI _StringValidateDestA(STRSAFE_PCNZCH pszDest, size_t cchDest, const size_t cchMax) {
    HRESULT hr = S_OK;
    if ((cchDest == 0) || (cchDest > cchMax)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    return hr;
    (void)(pszDest);
}

STRSAFEWORKERAPI _StringValidateDestAndLengthA(STRSAFE_LPCSTR pszDest, size_t cchDest, size_t * pcchDestLength, const size_t cchMax) {
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, cchMax);
    if (SUCCEEDED(hr)) {
        hr = _StringLengthWorkerA(pszDest, cchDest, pcchDestLength);
    } else {
        *pcchDestLength = 0;
    }
    return hr;
}

STRSAFEWORKERAPI _StringValidateDestW(STRSAFE_PCNZWCH pszDest, size_t cchDest, const size_t cchMax) {
    HRESULT hr = S_OK;
    if ((cchDest == 0) || (cchDest > cchMax)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    return hr;
    (void)(pszDest);
}

STRSAFEWORKERAPI _StringValidateDestAndLengthW(STRSAFE_LPCWSTR pszDest, size_t cchDest, size_t * pcchDestLength, const size_t cchMax) {
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, cchMax);
    if (SUCCEEDED(hr)) {
        hr = _StringLengthWorkerW(pszDest, cchDest, pcchDestLength);
    } else {
        *pcchDestLength = 0;
    }
    return hr;
}

STRSAFEWORKERAPI _StringExValidateDestA(STRSAFE_PCNZCH pszDest, size_t cchDest, const size_t cchMax, DWORD dwFlags) {
    HRESULT hr = S_OK;
    if (dwFlags & STRSAFE_IGNORE_NULLS) {
        if (((pszDest == NULL) && (cchDest != 0)) || (cchDest > cchMax)) {
            hr = STRSAFE_E_INVALID_PARAMETER;
        }
    } else {
        hr = _StringValidateDestA(pszDest, cchDest, cchMax);
    }
    return hr;
}

STRSAFEWORKERAPI _StringExValidateDestAndLengthA(STRSAFE_LPCSTR pszDest, size_t cchDest, size_t * pcchDestLength, const size_t cchMax, DWORD dwFlags) {
    HRESULT hr;
    if (dwFlags & STRSAFE_IGNORE_NULLS) {
        hr = _StringExValidateDestA(pszDest, cchDest, cchMax, dwFlags);
        if (FAILED(hr) || (cchDest == 0)) {
            *pcchDestLength = 0;
        } else {
            hr = _StringLengthWorkerA(pszDest, cchDest, pcchDestLength);
        }
    } else {
        hr = _StringValidateDestAndLengthA(pszDest, cchDest, pcchDestLength, cchMax);
    }
    return hr;
}

STRSAFEWORKERAPI _StringExValidateDestW(STRSAFE_PCNZWCH pszDest, size_t cchDest, const size_t cchMax, DWORD dwFlags) {
    HRESULT hr = S_OK;
    if (dwFlags & STRSAFE_IGNORE_NULLS) {
        if (((pszDest == NULL) && (cchDest != 0)) || (cchDest > cchMax)) {
            hr = STRSAFE_E_INVALID_PARAMETER;
        }
    } else {
        hr = _StringValidateDestW(pszDest, cchDest, cchMax);
    }
    return hr;
}

STRSAFEWORKERAPI _StringExValidateDestAndLengthW(STRSAFE_LPCWSTR pszDest, size_t cchDest, size_t * pcchDestLength, const size_t cchMax, DWORD dwFlags) {
    HRESULT hr;
    if (dwFlags & STRSAFE_IGNORE_NULLS) {
        hr = _StringExValidateDestW(pszDest, cchDest, cchMax, dwFlags);

        if (FAILED(hr) || (cchDest == 0)) {
            *pcchDestLength = 0;
        } else {
            hr = _StringLengthWorkerW(pszDest, cchDest, pcchDestLength);
        }
    } else {
        hr = _StringValidateDestAndLengthW(pszDest, cchDest, pcchDestLength, cchMax);
    }
    return hr;
}

STRSAFEWORKERAPI _StringCopyWorkerA(STRSAFE_LPSTR pszDest, size_t cchDest, size_t * pcchNewDestLength, STRSAFE_PCNZCH pszSrc, size_t cchToCopy) {
    HRESULT hr = S_OK;
    size_t cchNewDestLength = 0;
    while (cchDest && cchToCopy && (*pszSrc != '\0')) {
        *pszDest++ = *pszSrc++;
        cchDest--;
        cchToCopy--;

        cchNewDestLength++;
    }
    if (cchDest == 0) {
        pszDest--;
        cchNewDestLength--;
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    }
    *pszDest = '\0';
    if (pcchNewDestLength) {
        *pcchNewDestLength = cchNewDestLength;
    }
    return hr;
}

STRSAFEWORKERAPI _StringCopyWorkerW(STRSAFE_LPWSTR pszDest, size_t cchDest, size_t * pcchNewDestLength, STRSAFE_PCNZWCH pszSrc, size_t cchToCopy) {
    HRESULT hr = S_OK;
    size_t cchNewDestLength = 0;
    while (cchDest && cchToCopy && (*pszSrc != L'\0')) {
        *pszDest++ = *pszSrc++;
        cchDest--;
        cchToCopy--;
        cchNewDestLength++;
    }
    if (cchDest == 0) {
        pszDest--;
        cchNewDestLength--;
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    }
    *pszDest = L'\0';
    if (pcchNewDestLength) {
        *pcchNewDestLength = cchNewDestLength;
    }
    return hr;
}

STRSAFEWORKERAPI _StringVPrintfWorkerA(STRSAFE_LPSTR pszDest, size_t cchDest, size_t * pcchNewDestLength, STRSAFE_LPCSTR pszFormat, va_list argList) {
    HRESULT hr = S_OK;
    int iRet;
    size_t cchMax;
    size_t cchNewDestLength = 0;
    cchMax = cchDest - 1;
    iRet = _vsnprintf(pszDest, cchMax, pszFormat, argList);
    if ((iRet < 0) || (((size_t)iRet) > cchMax)) {
        pszDest += cchMax;
        *pszDest = '\0';
        cchNewDestLength = cchMax;
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    } else if (((size_t)iRet) == cchMax) {
        pszDest += cchMax;
        *pszDest = '\0';
        cchNewDestLength = cchMax;
    } else {
        cchNewDestLength = (size_t)iRet;
    }
    if (pcchNewDestLength) {
        *pcchNewDestLength = cchNewDestLength;
    }
    return hr;
}

STRSAFEWORKERAPI _StringVPrintfWorkerW(STRSAFE_LPWSTR pszDest, size_t cchDest, size_t * pcchNewDestLength, STRSAFE_LPCWSTR pszFormat, va_list argList) {
    HRESULT hr = S_OK;
    int iRet;
    size_t cchMax;
    size_t cchNewDestLength = 0;
    cchMax = cchDest - 1;
    iRet = _vsnwprintf(pszDest, cchMax, pszFormat, argList);
    if ((iRet < 0) || (((size_t)iRet) > cchMax)) {
        pszDest += cchMax;
        *pszDest = L'\0';
        cchNewDestLength = cchMax;
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    } else if (((size_t)iRet) == cchMax) {
        pszDest += cchMax;
        *pszDest = L'\0';
        cchNewDestLength = cchMax;
    } else {
        cchNewDestLength = (size_t)iRet;
    }
    if (pcchNewDestLength) {
        *pcchNewDestLength = cchNewDestLength;
    }
    return hr;
}

STRSAFEWORKERAPI _StringExHandleFillBehindNullA(STRSAFE_LPSTR pszDestEnd, size_t cbRemaining, DWORD dwFlags)
{
    if (cbRemaining > sizeof(char)) {
        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), cbRemaining - sizeof(char));
    }
    return S_OK;
}

STRSAFEWORKERAPI _StringExHandleFillBehindNullW(STRSAFE_LPWSTR pszDestEnd, size_t cbRemaining, DWORD dwFlags)
{
    if (cbRemaining > sizeof(wchar_t)) {
        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), cbRemaining - sizeof(wchar_t));
    }
    return S_OK;
}

STRSAFEWORKERAPI _StringExHandleOtherFlagsA(STRSAFE_LPSTR pszDest, size_t cbDest, size_t cchOriginalDestLength, STRSAFE_LPSTR * ppszDestEnd, size_t * pcchRemaining, DWORD dwFlags) {
    size_t cchDest = cbDest / sizeof(char);

    if ((cchDest > 0) && (dwFlags & STRSAFE_NO_TRUNCATION)) {
        char *pszOriginalDestEnd;
        pszOriginalDestEnd = pszDest + cchOriginalDestLength;
        *ppszDestEnd = pszOriginalDestEnd;
        *pcchRemaining = cchDest - cchOriginalDestLength;
        *pszOriginalDestEnd = '\0';
    }
    if (dwFlags & STRSAFE_FILL_ON_FAILURE) {
        memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);
        if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0) {
            *ppszDestEnd = pszDest;
            *pcchRemaining = cchDest;
        } else if (cchDest > 0) {
            char *pszDestEnd;
            pszDestEnd = pszDest + cchDest - 1;
            *ppszDestEnd = pszDestEnd;
            *pcchRemaining = 1;
            *pszDestEnd = L'\0';
        }
    }
    if ((cchDest > 0) && (dwFlags & STRSAFE_NULL_ON_FAILURE)) {
        *ppszDestEnd = pszDest;
        *pcchRemaining = cchDest;
        *pszDest = '\0';
    }
    return S_OK;
}

STRSAFEWORKERAPI _StringExHandleOtherFlagsW(STRSAFE_LPWSTR pszDest, size_t cbDest, size_t cchOriginalDestLength, STRSAFE_LPWSTR * ppszDestEnd, size_t * pcchRemaining, DWORD dwFlags) {
    size_t cchDest = cbDest / sizeof(wchar_t);
    if ((cchDest > 0) && (dwFlags & STRSAFE_NO_TRUNCATION)) {
        wchar_t *pszOriginalDestEnd;
        pszOriginalDestEnd = pszDest + cchOriginalDestLength;
        *ppszDestEnd = pszOriginalDestEnd;
        *pcchRemaining = cchDest - cchOriginalDestLength;
        *pszOriginalDestEnd = L'\0';
    }
    if (dwFlags & STRSAFE_FILL_ON_FAILURE) {
        memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);
        if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0) {
            *ppszDestEnd = pszDest;
            *pcchRemaining = cchDest;
        } else if (cchDest > 0) {
            wchar_t *pszDestEnd;
            pszDestEnd = pszDest + cchDest - 1;
            *ppszDestEnd = pszDestEnd;
            *pcchRemaining = 1;
            *pszDestEnd = L'\0';
        }
    }
    if ((cchDest > 0) && (dwFlags & STRSAFE_NULL_ON_FAILURE)) {
        *ppszDestEnd = pszDest;
        *pcchRemaining = cchDest;
        *pszDest = L'\0';
    }
    return S_OK;
}


#ifndef STRSAFE_NO_CCH_FUNCTIONS

STRSAFEAPI StringCchCopyA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc)
{
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerA(pszDest, cchDest, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchCopyW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCWSTR pszSrc)
{
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerW(pszDest, cchDest, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchCopyExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if (*pszSrc != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDest, cchDest, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining;
                    cbRemaining = cchRemaining * sizeof(char);
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest;
            cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }

    return hr;
}

STRSAFEAPI StringCchCopyExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCWSTR pszSrc, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if (*pszSrc != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDest, cchDest, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining;
                    cbRemaining = cchRemaining * sizeof(wchar_t);
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }

    return hr;
}

STRSAFEAPI StringCchCopyNA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_PCNZCH pszSrc, size_t cchToCopy)
{
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        if (cchToCopy > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            *pszDest = '\0';
        } else {
            hr = _StringCopyWorkerA(pszDest, cchDest, NULL, pszSrc, cchToCopy);
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchCopyNW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_PCNZWCH pszSrc, size_t cchToCopy)
{
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        if (cchToCopy > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            *pszDest = L'\0';
        } else {
            hr = _StringCopyWorkerW(pszDest, cchDest, NULL, pszSrc, cchToCopy);
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchCopyNExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_PCNZCH pszSrc, size_t cchToCopy, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszSrc, &cchToCopy, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if ((cchToCopy != 0) && (*pszSrc != '\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDest, cchDest, &cchCopied, pszSrc, cchToCopy);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(char);
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchCopyNExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_PCNZWCH pszSrc, size_t cchToCopy, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszSrc, &cchToCopy, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if ((cchToCopy != 0) && (*pszSrc != L'\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDest, cchDest, &cchCopied, pszSrc, cchToCopy);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(wchar_t);
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchCatA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc)
{
    size_t cchDestLength;
    HRESULT hr = _StringValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerA(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    }
    return hr;
}

STRSAFEAPI StringCchCatW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCWSTR pszSrc)
{
    size_t cchDestLength;
    HRESULT hr = _StringValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerW(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    }
    return hr;
}

STRSAFEAPI StringCchCatExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        hr = _StringExValidateSrcA(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if (*pszSrc != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDestEnd, cchRemaining, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(char);
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCchCatExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCWSTR pszSrc, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    size_t cchDestLength;

    HRESULT hr = _StringExValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        hr = _StringExValidateSrcW(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if (*pszSrc != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDestEnd, cchRemaining, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(wchar_t);
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCchCatNA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_PCNZCH pszSrc, size_t cchToAppend)
{
    size_t cchDestLength;
    HRESULT hr = _StringValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        if (cchToAppend > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
        } else {
            hr = _StringCopyWorkerA(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, cchToAppend);
        }
    }
    return hr;
}

STRSAFEAPI StringCchCatNW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_PCNZWCH pszSrc, size_t cchToAppend)
{
    size_t cchDestLength;
    HRESULT hr = _StringValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        if (cchToAppend > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
        } else {
            hr = _StringCopyWorkerW(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, cchToAppend);
        }
    }
    return hr;
}

STRSAFEAPI StringCchCatNExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_PCNZCH pszSrc, size_t cchToAppend, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        hr = _StringExValidateSrcA(&pszSrc, &cchToAppend, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if ((cchToAppend != 0) && (*pszSrc != '\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDestEnd, cchRemaining, &cchCopied, pszSrc, cchToAppend);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(char);
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }

        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCchCatNExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_PCNZWCH pszSrc, size_t cchToAppend, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        hr = _StringExValidateSrcW(&pszSrc, &cchToAppend, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if ((cchToAppend != 0) && (*pszSrc != L'\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDestEnd, cchRemaining, &cchCopied, pszSrc, cchToAppend);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(wchar_t);
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCchVPrintfA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszFormat, va_list argList)
{
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringVPrintfWorkerA(pszDest, cchDest, NULL, pszFormat, argList);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchVPrintfW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCWSTR pszFormat, va_list argList)
{
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringVPrintfWorkerW(pszDest, cchDest, NULL, pszFormat, argList);
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

__declspec(vaformat(printf,3,4)) STRSAFEAPIV StringCchPrintfA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszFormat, ...)
{
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        va_list argList;
        va_start(argList, pszFormat);
        hr = _StringVPrintfWorkerA(pszDest, cchDest, NULL, pszFormat, argList);
        va_end(argList);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

__declspec(vaformat(printf,3,4)) STRSAFEAPIV StringCchPrintfW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCWSTR pszFormat, ...)
{
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        va_list argList;
        va_start(argList, pszFormat);
        hr = _StringVPrintfWorkerW(pszDest, cchDest, NULL, pszFormat, argList);
        va_end(argList);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

__declspec(vaformat(printf,6,7)) STRSAFEAPIV StringCchPrintfExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags, STRSAFE_LPCSTR pszFormat, ...)
{
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                va_list argList;
                va_start(argList, pszFormat);
                hr = _StringVPrintfWorkerA(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                va_end(argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(char);
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }

        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

__declspec(vaformat(printf,6,7)) STRSAFEAPIV StringCchPrintfExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags, STRSAFE_LPCWSTR pszFormat, ...)
{
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                va_list argList;
                va_start(argList, pszFormat);
                hr = _StringVPrintfWorkerW(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                va_end(argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(wchar_t);
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchVPrintfExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags, STRSAFE_LPCSTR pszFormat, va_list argList)
{
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                hr = _StringVPrintfWorkerA(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(char);
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchVPrintfExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags, STRSAFE_LPCWSTR pszFormat, va_list argList)
{
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                hr = _StringVPrintfWorkerW(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = cchRemaining * sizeof(wchar_t);
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchGetsA(STRSAFE_LPSTR pszDest, size_t cchDest)
{
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = StringGetsWorkerA(pszDest, cchDest, NULL);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchGetsW(STRSAFE_LPWSTR pszDest, size_t cchDest)
{
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = StringGetsWorkerW(pszDest, cchDest, NULL);
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchGetsExA(STRSAFE_LPSTR pszDest, size_t cchDest, STRSAFE_LPSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        } else if (cchDest == 0) {
            if (pszDest == NULL) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else {
                hr = STRSAFE_E_INSUFFICIENT_BUFFER;
            }
        } else {
            size_t cchNewDestLength = 0;
            hr = StringGetsWorkerA(pszDest, cchDest, &cchNewDestLength);
            pszDestEnd = pszDest + cchNewDestLength;
            cchRemaining = cchDest - cchNewDestLength;
            if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                size_t cbRemaining = cchRemaining * sizeof(char);
                _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(char);
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER) || (hr == STRSAFE_E_END_OF_FILE)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCchGetsExW(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcchRemaining, DWORD dwFlags)
{
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        } else if (cchDest == 0) {
            if (pszDest == NULL) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else {
                hr = STRSAFE_E_INSUFFICIENT_BUFFER;
            }
        } else {
            size_t cchNewDestLength = 0;
            hr = StringGetsWorkerW(pszDest, cchDest, &cchNewDestLength);
            pszDestEnd = pszDest + cchNewDestLength;
            cchRemaining = cchDest - cchNewDestLength;
            if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                size_t cbRemaining = cchRemaining * sizeof(wchar_t);
                _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cchDest != 0)) {
            size_t cbDest = cchDest * sizeof(wchar_t);
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER) || (hr == STRSAFE_E_END_OF_FILE)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcchRemaining) {
                *pcchRemaining = cchRemaining;
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCchLengthA(STRSAFE_PCNZCH psz, size_t cchMax, size_t *pcchLength)
{
    HRESULT hr;
    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else {
        hr = _StringLengthWorkerA(psz, cchMax, pcchLength);
    }
    if (FAILED(hr) && pcchLength) {
        *pcchLength = 0;
    }
    return hr;
}

STRSAFEAPI StringCchLengthW(STRSAFE_PCNZWCH psz, size_t cchMax, size_t *pcchLength)
{
    HRESULT hr;
    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else {
        hr = _StringLengthWorkerW(psz, cchMax, pcchLength);
    }
    if (FAILED(hr) && pcchLength) {
        *pcchLength = 0;
    }
    return hr;
}

#ifdef ALIGNMENT_MACHINE
STRSAFEAPI UnalignedStringCchLengthW(STRSAFE_PCUNZWCH psz, size_t cchMax, size_t *pcchLength)
{
    HRESULT hr;
    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else {
        hr = _UnalignedStringLengthWorkerW(psz, cchMax, pcchLength);
    }
    if (FAILED(hr) && pcchLength) {
        *pcchLength = 0;
    }
    return hr;
}
#else /* !ALIGNMENT_MACHINE */
#define UnalignedStringCchLengthW  StringCchLengthW
#endif /* !ALIGNMENT_MACHINE */

#ifdef UNICODE
#define StringCchCopy  StringCchCopyW
#define StringCchCopyEx  StringCchCopyExW
#define StringCchCopyN  StringCchCopyNW
#define StringCchCopyNEx  StringCchCopyNExW
#define StringCchCat  StringCchCatW
#define StringCchCatEx  StringCchCatExW
#define StringCchCatN  StringCchCatNW
#define StringCchCatNEx  StringCchCatNExW
#define StringCchVPrintf  StringCchVPrintfW
#define StringCchPrintf  StringCchPrintfW
#define StringCchPrintfEx  StringCchPrintfExW
#define StringCchVPrintfEx  StringCchVPrintfExW
#define StringCchGets  StringCchGetsW
#define StringCchGetsEx  StringCchGetsExW
#define StringCchLength  StringCchLengthW
#define UnalignedStringCchLength  UnalignedStringCchLengthW
#else /* !UNICODE */
#define StringCchCopy  StringCchCopyA
#define StringCchCopyEx  StringCchCopyExA
#define StringCchCopyN  StringCchCopyNA
#define StringCchCopyNEx  StringCchCopyNExA
#define StringCchCat  StringCchCatA
#define StringCchCatEx  StringCchCatExA
#define StringCchCatN  StringCchCatNA
#define StringCchCatNEx  StringCchCatNExA
#define StringCchVPrintf  StringCchVPrintfA
#define StringCchPrintf  StringCchPrintfA
#define StringCchPrintfEx  StringCchPrintfExA
#define StringCchVPrintfEx  StringCchVPrintfExA
#define StringCchGets  StringCchGetsA
#define StringCchGetsEx  StringCchGetsExA
#define StringCchLength  StringCchLengthA
#define UnalignedStringCchLength  StringCchLengthA
#endif /* !UNICODE */

/* See TR24731-1 and TR24731-2, for example */
#ifndef __ORANGEC__
#pragma deprecated(StringCchCopy)
#pragma deprecated(StringCchCopyEx)
#pragma deprecated(StringCchCopyN)
#pragma deprecated(StringCchCopyNEx)
#pragma deprecated(StringCchCat)
#pragma deprecated(StringCchCatEx)
#pragma deprecated(StringCchCatN)
#pragma deprecated(StringCchCatNEx)
#pragma deprecated(StringCchVPrintf)
#pragma deprecated(StringCchPrintf)
#pragma deprecated(StringCchPrintfEx)
#pragma deprecated(StringCchVPrintfEx)
#pragma deprecated(StringCchGets)
#pragma deprecated(StringCchGetsEx)
#pragma deprecated(StringCchLength)
#pragma deprecated(UnalignedStringCchLength)
#endif

#endif /* !STRSAFE_NO_CCH_FUNCTIONS */


#ifndef STRSAFE_NO_CB_FUNCTIONS

STRSAFEAPI StringCbCopyA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPCSTR pszSrc)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerA(pszDest, cchDest, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCbCopyW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPCWSTR pszSrc)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerW(pszDest, cchDest, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCbCopyExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPCSTR pszSrc, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if (*pszSrc != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDest, cchDest, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCbCopyExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPCWSTR pszSrc, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if (*pszSrc != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDest, cchDest, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                    size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCbCopyNA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_PCNZCH pszSrc, size_t cbToCopy)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        size_t cchToCopy = cbToCopy / sizeof(char);
        if (cchToCopy > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            *pszDest = '\0';
        } else {
            hr = _StringCopyWorkerA(pszDest, cchDest, NULL, pszSrc, cchToCopy);
        }
    }
    return hr;
}

STRSAFEAPI StringCbCopyNW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_PCNZWCH pszSrc, size_t cbToCopy)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        size_t cchToCopy = cbToCopy / sizeof(wchar_t);
        if (cchToCopy > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            *pszDest = L'\0';
        } else {
            hr = _StringCopyWorkerW(pszDest, cchDest, NULL, pszSrc, cchToCopy);
        }
    }
    return hr;
}

STRSAFEAPI StringCbCopyNExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_PCNZCH pszSrc, size_t cbToCopy, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        size_t cchToCopy = cbToCopy / sizeof(char);
        hr = _StringExValidateSrcA(&pszSrc, &cchToCopy, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if ((cchToCopy != 0) && (*pszSrc != '\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDest, cchDest, &cchCopied, pszSrc, cchToCopy);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbCopyNExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_PCNZWCH pszSrc, size_t cbToCopy, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        size_t cchToCopy = cbToCopy / sizeof(wchar_t);
        hr = _StringExValidateSrcW(&pszSrc, &cchToCopy, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if ((cchToCopy != 0) && (*pszSrc != L'\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDest, cchDest, &cchCopied, pszSrc, cchToCopy);
                pszDestEnd = pszDest + cchCopied;
                cchRemaining = cchDest - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                    size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbCatA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPCSTR pszSrc)
{
    size_t cchDestLength;
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerA(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    }
    return hr;
}

STRSAFEAPI StringCbCatW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPCWSTR pszSrc)
{
    size_t cchDestLength;
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringCopyWorkerW(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, STRSAFE_MAX_LENGTH);
    }
    return hr;
}

STRSAFEAPI StringCbCatExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPCSTR pszSrc, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(char);
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        hr = _StringExValidateSrcA(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if (*pszSrc != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDestEnd, cchRemaining, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbCatExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPCWSTR pszSrc, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        hr = _StringExValidateSrcW(&pszSrc, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if (*pszSrc != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDestEnd, cchRemaining, &cchCopied, pszSrc, STRSAFE_MAX_LENGTH);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                    size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbCatNA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_PCNZCH pszSrc, size_t cbToAppend)
{
    size_t cchDest = cbDest / sizeof(char);
    size_t cchDestLength;
    HRESULT hr = _StringValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        size_t cchToAppend = cbToAppend / sizeof(char);
        if (cchToAppend > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
        } else {
            hr = _StringCopyWorkerA(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, cchToAppend);
        }
    }
    return hr;
}

STRSAFEAPI StringCbCatNW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_PCNZWCH pszSrc, size_t cbToAppend)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    size_t cchDestLength;
    HRESULT hr = _StringValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        size_t cchToAppend = cbToAppend / sizeof(wchar_t);

        if (cchToAppend > STRSAFE_MAX_LENGTH) {
            hr = STRSAFE_E_INVALID_PARAMETER;
        } else {
            hr = _StringCopyWorkerW(pszDest + cchDestLength, cchDest - cchDestLength, NULL, pszSrc, cchToAppend);
        }
    }
    return hr;
}

STRSAFEAPI StringCbCatNExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_PCNZCH pszSrc, size_t cbToAppend, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(char);
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthA(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        size_t cchToAppend = cbToAppend / sizeof(char);
        hr = _StringExValidateSrcA(&pszSrc, &cchToAppend, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if ((cchToAppend != 0) && (*pszSrc != '\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerA(pszDestEnd, cchRemaining, &cchCopied, pszSrc, cchToAppend);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbCatNExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_PCNZWCH pszSrc, size_t cbToAppend, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    size_t cchDestLength;
    HRESULT hr = _StringExValidateDestAndLengthW(pszDest, cchDest, &cchDestLength, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest + cchDestLength;
        size_t cchRemaining = cchDest - cchDestLength;
        size_t cchToAppend = cbToAppend / sizeof(wchar_t);
        hr = _StringExValidateSrcW(&pszSrc, &cchToAppend, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else if (cchRemaining <= 1) {
                if ((cchToAppend != 0) && (*pszSrc != L'\0')) {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchCopied = 0;
                hr = _StringCopyWorkerW(pszDestEnd, cchRemaining, &cchCopied, pszSrc, cchToAppend);
                pszDestEnd = pszDestEnd + cchCopied;
                cchRemaining = cchRemaining - cchCopied;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                    size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, cchDestLength, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbVPrintfA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPCSTR pszFormat, va_list argList)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringVPrintfWorkerA(pszDest, cchDest, NULL, pszFormat, argList);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCbVPrintfW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPCWSTR pszFormat, va_list argList)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = _StringVPrintfWorkerW(pszDest, cchDest, NULL, pszFormat, argList);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

__declspec(vaformat(printf,3,4)) STRSAFEAPIV StringCbPrintfA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPCSTR pszFormat, ...)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        va_list argList;
        va_start(argList, pszFormat);
        hr = _StringVPrintfWorkerA(pszDest, cchDest, NULL, pszFormat, argList);
        va_end(argList);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

__declspec(vaformat(printf,3,4)) STRSAFEAPIV StringCbPrintfW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPCWSTR pszFormat, ...)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        va_list argList;
        va_start(argList, pszFormat);
        hr = _StringVPrintfWorkerW(pszDest, cchDest, NULL, pszFormat, argList);
        va_end(argList);
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

__declspec(vaformat(printf,6,7)) STRSAFEAPIV StringCbPrintfExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags, STRSAFE_LPCSTR pszFormat, ...)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                va_list argList;
                va_start(argList, pszFormat);
                hr = _StringVPrintfWorkerA(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                va_end(argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

__declspec(vaformat(printf,6,7)) STRSAFEAPIV StringCbPrintfExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags, STRSAFE_LPCWSTR pszFormat, ...)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                va_list argList;
                va_start(argList, pszFormat);
                hr = _StringVPrintfWorkerW(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                va_end(argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                    size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCbVPrintfExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags, STRSAFE_LPCSTR pszFormat, va_list argList)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcA(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = '\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != '\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                hr = _StringVPrintfWorkerA(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                    size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                    _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbVPrintfExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags, STRSAFE_LPCWSTR pszFormat, va_list argList)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        hr = _StringExValidateSrcW(&pszFormat, NULL, STRSAFE_MAX_CCH, dwFlags);
        if (SUCCEEDED(hr)) {
            if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
                hr = STRSAFE_E_INVALID_PARAMETER;
                if (cchDest != 0) {
                    *pszDest = L'\0';
                }
            } else if (cchDest == 0) {
                if (*pszFormat != L'\0') {
                    if (pszDest == NULL) {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    } else {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            } else {
                size_t cchNewDestLength = 0;
                hr = _StringVPrintfWorkerW(pszDest, cchDest, &cchNewDestLength, pszFormat, argList);
                pszDestEnd = pszDest + cchNewDestLength;
                cchRemaining = cchDest - cchNewDestLength;
                if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                    size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                    _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
                }
            }
        } else {
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    }
    return hr;
}

STRSAFEAPI StringCbGetsA(STRSAFE_LPSTR pszDest, size_t cbDest)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = StringGetsWorkerA(pszDest, cchDest, NULL);
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCbGetsW(STRSAFE_LPWSTR pszDest, size_t cbDest)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);
    if (SUCCEEDED(hr)) {
        hr = StringGetsWorkerW(pszDest, cchDest, NULL);
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCbGetsExA(STRSAFE_LPSTR pszDest, size_t cbDest, STRSAFE_LPSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(char);
    HRESULT hr = _StringExValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            if (cchDest != 0) {
                *pszDest = '\0';
            }
        } else if (cchDest == 0) {
            if (pszDest == NULL) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else {
                hr = STRSAFE_E_INSUFFICIENT_BUFFER;
            }
        } else {
            size_t cchNewDestLength = 0;
            hr = StringGetsWorkerA(pszDest, cchDest, &cchNewDestLength);
            pszDestEnd = pszDest + cchNewDestLength;
            cchRemaining = cchDest - cchNewDestLength;
            if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL) && (cchRemaining > 1)) {
                size_t cbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
                _StringExHandleFillBehindNullA(pszDestEnd, cbRemaining, dwFlags);
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsA(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER) || (hr == STRSAFE_E_END_OF_FILE)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
            }
        }
    } else if (cchDest > 0) {
        *pszDest = '\0';
    }
    return hr;
}

STRSAFEAPI StringCbGetsExW(STRSAFE_LPWSTR pszDest, size_t cbDest, STRSAFE_LPWSTR *ppszDestEnd, size_t *pcbRemaining, DWORD dwFlags)
{
    size_t cchDest = cbDest / sizeof(wchar_t);
    HRESULT hr = _StringExValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH, dwFlags);
    if (SUCCEEDED(hr)) {
        STRSAFE_LPWSTR pszDestEnd = pszDest;
        size_t cchRemaining = cchDest;
        if (dwFlags & (~STRSAFE_VALID_FLAGS)) {
            hr = STRSAFE_E_INVALID_PARAMETER;
            if (cchDest != 0) {
                *pszDest = L'\0';
            }
        } else if (cchDest == 0) {
            if (pszDest == NULL) {
                hr = STRSAFE_E_INVALID_PARAMETER;
            } else {
                hr = STRSAFE_E_INSUFFICIENT_BUFFER;
            }
        } else {
            size_t cchNewDestLength = 0;
            hr = StringGetsWorkerW(pszDest, cchDest, &cchNewDestLength);
            pszDestEnd = pszDest + cchNewDestLength;
            cchRemaining = cchDest - cchNewDestLength;
            if (SUCCEEDED(hr) && (dwFlags & STRSAFE_FILL_BEHIND_NULL)) {
                size_t cbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
                _StringExHandleFillBehindNullW(pszDestEnd, cbRemaining, dwFlags);
            }
        }
        if (FAILED(hr) && (dwFlags & (STRSAFE_NO_TRUNCATION | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)) && (cbDest != 0)) {
            _StringExHandleOtherFlagsW(pszDest, cbDest, 0, &pszDestEnd, &cchRemaining, dwFlags);
        }
        if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER) || (hr == STRSAFE_E_END_OF_FILE)) {
            if (ppszDestEnd) {
                *ppszDestEnd = pszDestEnd;
            }
            if (pcbRemaining) {
                *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
            }
        }
    } else if (cchDest > 0) {
        *pszDest = L'\0';
    }
    return hr;
}

STRSAFEAPI StringCbLengthA(STRSAFE_PCNZCH psz, size_t cbMax, size_t *pcbLength)
{
    HRESULT hr;
    size_t cchMax = cbMax / sizeof(char);
    size_t cchLength = 0;
    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else {
        hr = _StringLengthWorkerA(psz, cchMax, &cchLength);
    }
    if (pcbLength) {
        if (SUCCEEDED(hr)) {
            *pcbLength = cchLength * sizeof(char);
        } else {
            *pcbLength = 0;
        }
    }
    return hr;
}

STRSAFEAPI StringCbLengthW(STRSAFE_PCNZWCH psz, size_t cbMax, size_t *pcbLength)
{
    HRESULT hr;
    size_t cchMax = cbMax / sizeof(wchar_t);
    size_t cchLength = 0;
    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else {
        hr = _StringLengthWorkerW(psz, cchMax, &cchLength);
    }
    if (pcbLength) {
        if (SUCCEEDED(hr)) {
            *pcbLength = cchLength * sizeof(wchar_t);
        } else {
            *pcbLength = 0;
        }
    }
    return hr;
}

#ifdef ALIGNMENT_MACHINE
STRSAFEAPI UnalignedStringCbLengthW(STRSAFE_PCUNZWCH psz, size_t cbMax, size_t *pcbLength)
{
    HRESULT hr;
    size_t cchMax = cbMax / sizeof(wchar_t);
    size_t cchLength = 0;
    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH)) {
        hr = STRSAFE_E_INVALID_PARAMETER;
    } else {
        hr = _UnalignedStringLengthWorkerW(psz, cchMax, &cchLength);
    }
    if (pcbLength) {
        if (SUCCEEDED(hr)) {
            *pcbLength = cchLength * sizeof(wchar_t);
        } else {
            *pcbLength = 0;
        }
    }
    return hr;
}
#else /* !ALIGNMENT_MACHINE */
#define UnalignedStringCbLengthW  StringCbLengthW
#endif /* !ALIGNMENT_MACHINE */

#ifdef UNICODE
#define StringCbCopy  StringCbCopyW
#define StringCbCopyEx  StringCbCopyExW
#define StringCbCopyN  StringCbCopyNW
#define StringCbCopyNEx  StringCbCopyNExW
#define StringCbCat  StringCbCatW
#define StringCbCatEx  StringCbCatExW
#define StringCbCatN  StringCbCatNW
#define StringCbCatNEx  StringCbCatNExW
#define StringCbVPrintf  StringCbVPrintfW
#define StringCbPrintf  StringCbPrintfW
#define StringCbPrintfEx  StringCbPrintfExW
#define StringCbVPrintfEx  StringCbVPrintfExW
#define StringCbGets  StringCbGetsW
#define StringCbGetsEx  StringCbGetsExW
#define StringCbLength  StringCbLengthW
#define UnalignedStringCbLength UnalignedStringCbLengthW
#else /* !UNICODE */
#define StringCbCopy  StringCbCopyA
#define StringCbCopyEx  StringCbCopyExA
#define StringCbCopyN  StringCbCopyNA
#define StringCbCopyNEx  StringCbCopyNExA
#define StringCbCat  StringCbCatA
#define StringCbCatEx  StringCbCatExA
#define StringCbCatN  StringCbCatNA
#define StringCbCatNEx  StringCbCatNExA
#define StringCbVPrintf  StringCbVPrintfA
#define StringCbPrintf  StringCbPrintfA
#define StringCbPrintfEx  StringCbPrintfExA
#define StringCbVPrintfEx  StringCbVPrintfExA
#define StringCbGets  StringCbGetsA
#define StringCbGetsEx  StringCbGetsExA
#define StringCbLength  StringCbLengthA
#define UnalignedStringCbLength StringCbLengthA
#endif /* !UNICODE */

/* See TR24731-1 and TR24731-2, for example */
#ifndef __ORANGEC__
#pragma deprecated(StringCbCopy)
#pragma deprecated(StringCbCopyEx)
#pragma deprecated(StringCbCopyN)
#pragma deprecated(StringCbCopyNEx)
#pragma deprecated(StringCbCat)
#pragma deprecated(StringCbCatEx)
#pragma deprecated(StringCbCatN)
#pragma deprecated(StringCbCatNEx)
#pragma deprecated(StringCbVPrintf)
#pragma deprecated(StringCbPrintf)
#pragma deprecated(StringCbPrintfEx)
#pragma deprecated(StringCbVPrintfEx)
#pragma deprecated(StringCbGets)
#pragma deprecated(StringCbGetsEx)
#pragma deprecated(StringCbLength)
#pragma deprecated(UnalignedStringCbLength)
#endif

#endif /* !STRSAFE_NO_CB_FUNCTIONS */

#endif /* _STRSAFE_H */
