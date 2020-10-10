/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <stdlib.h>
#include <windows.h>
typedef ULONG_PTR HCRYPTPROV;

#define PROV_RSA_FULL 1
#define CRYPT_VERIFYCONTEXT 0xf0000000
#define CRYPT_SILENT 0x00000040

BOOL WINAPI CryptAcquireContextW(HCRYPTPROV* phProv, LPCWSTR pszContainer, LPCWSTR pszProvider, DWORD dwProvType, DWORD dwFlags);

BOOL WINAPI CryptReleaseContext(HCRYPTPROV hProv, ULONG_PTR dwFlags);
BOOL WINAPI CryptGenRandom(HCRYPTPROV hProv, DWORD dwLen, BYTE* pbBuffer);

int _RTL_FUNC rand_s(unsigned int* __rand_value)
{
    int rv = 0;
    HCRYPTPROV hProvider = 0;

    if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
        return 1;

    if (!CryptGenRandom(hProvider, sizeof(*__rand_value), (BYTE*)__rand_value))
        rv = 1;
    if (!CryptReleaseContext(hProvider, 0))
        rv = 1;
    return rv;
}
