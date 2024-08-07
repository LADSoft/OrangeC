/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include "delayimp.h"

#define VcppException(sev,err)  ((sev) | (FACILITY_VISUALCPP<<16) | err)

extern const DWORD ___imageBase;

static PBYTE imageBase;


void aquireWriteAccess()
{
}
void releaseWriteAccess()
{
}
 
FARPROC WINAPI
___delayLoadHelper2(PCIMAGE_DELAYLOAD_DESCRIPTOR desc, FARPROC*iatEntry)
{
    FARPROC rv = NULL;
    aquireWriteAccess();
    DelayLoadInfo   dli = {
        sizeof DelayLoadInfo,
        desc,
        iatEntry,
        (LPCSTR)(desc->DllNameRVA + ___imageBase),
        { 0 },
        0,
        0,
        0
        };
    PDelayLoadInfo  info[1] = { &dli };
    HMODULE hmod = *(HMODULE*)(desc->ModuleHandleRVA + ___imageBase);
    int index = (PBYTE)iatEntry - ___imageBase - desc->ImportAddressTableRVA;
    PIMAGE_THUNK_DATA iNameEntry = (PIMAGE_THUNK_DATA)(index + desc->ImportNameTableRVA + ___imageBase); 
    dli.dlp.fImportByName = !IMAGE_SNAP_BY_ORDINAL(iNameEntry->u1.Ordinal);

    if (dli.dlp.fImportByName)
        dli.dlp.szProcName = (LPCSTR)(*(DWORD*)iNameEntry + ___imageBase+ sizeof(short));
    else
        dli.dlp.dwOrdinal = (DWORD)IMAGE_ORDINAL(iNameEntry->u1.Ordinal);
    rv = __DliNotifyHook2(dliStartProcessing, &dli);
    if (!rv)
    {
        if (hmod == 0)
        {
            hmod = (HMODULE)__DliNotifyHook2(dliNotePreLoadLibrary, &dli);
            if (hmod == 0)
                hmod = LoadLibraryEx(dli.szDll, NULL, 0);
        
            if (hmod == 0) 
            {
                dli.dwLastError = GetLastError();
                hmod = (HMODULE)__DliNotifyHook2(dliFailLoadLib, &dli);
            }

            if (hmod == 0)
            {
                releaseWriteAccess();
                RaiseException(
                    VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND),
                    0,
                    1,
                    (PULONG_PTR)info
                    );

                /* if the exception is satisfied it is assumed someone filled in pfnCur */
                return dli.pfnCur;
            }

            HMODULE hmodx = (HMODULE)InterlockedExchange((PLONG)(desc->ModuleHandleRVA + ___imageBase), (LONG)hmod);
            if (hmodx == hmod) {
                // race condition, free the library to decrement its reference count
                FreeLibrary(hmod);
            }
            dli.hmodCur = hmod;
        }
        rv = __DliNotifyHook2(dliNotePreGetProcAddress, &dli);
        if (rv == 0) 
        {
            if (desc->BoundImportAddressTableRVA && desc->TimeDateStamp) 
            {
                PIMAGE_DOS_HEADER hdr = (PIMAGE_DOS_HEADER)hmod;
                PIMAGE_NT_HEADERS pehdr = (PIMAGE_NT_HEADERS)(hdr->e_lfanew + (BYTE*)hmod);

                if (pehdr->FileHeader.TimeDateStamp == desc->TimeDateStamp && 
                    (DWORD)hmod == pehdr->OptionalHeader.ImageBase)
                {
                    // binding exists and we have the right time and base address
                    FARPROC* funcEntry = (FARPROC *)(index + desc->BoundImportAddressTableRVA + ___imageBase); 
                    rv = *funcEntry;
                }
            }
        }
        if (rv == 0)       
            rv = GetProcAddress(hmod, dli.dlp.szProcName);
        if (rv == 0)
        {
            dli.dwLastError = GetLastError();
            rv = __DliFailureHook2(dliFailGetProc, &dli);
        }
        if (rv == 0)
        {
            releaseWriteAccess();

            RaiseException(
                VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND),
                0,
                1,
                (PULONG_PTR)info
                );

            aquireWriteAccess();
            // assume the app left a valid function pointer for us
            rv = dli.pfnCur;
        }
        *iatEntry = rv;
    }
    releaseWriteAccess();
    return rv;
}
#define DELAYLOAD_DIRECTORY_ENTRY 13
static PCIMAGE_DELAYLOAD_DESCRIPTOR getDesc(LPCSTR dllName)
{
    PIMAGE_DOS_HEADER hdr = (PIMAGE_DOS_HEADER)___imageBase;
    PIMAGE_NT_HEADERS pehdr = (PIMAGE_NT_HEADERS)(hdr->e_lfanew + ___imageBase);
  
    PCIMAGE_DELAYLOAD_DESCRIPTOR desc = (PCIMAGE_DELAYLOAD_DESCRIPTOR)(pehdr->OptionalHeader.DataDirectory[DELAYLOAD_DIRECTORY_ENTRY].VirtualAddress + ___imageBase);
    if (desc != ___imageBase)
    {
        while (desc->DllNameRVA)
        {
            if (!strcmp(dllName, (LPCSTR)(desc->DllNameRVA + ___imageBase)))
                return desc;
            ++desc;
        }        
    }
    return NULL;
}
BOOL WINAPI
__FUnloadDelayLoadedDLL2(LPCSTR szDll) {
    BOOL        rv = FALSE;
    PCIMAGE_DELAYLOAD_DESCRIPTOR desc = getDesc(szDll);

    if (desc && desc->UnloadInformationTableRVA)
    {
        HMODULE * hmod_ptr = (HMODULE *)(desc->ModuleHandleRVA + ___imageBase);
        if (*hmod_ptr != NULL) 
        {
            aquireWriteAccess();
            FARPROC* src = (FARPROC *)(desc->UnloadInformationTableRVA + ___imageBase);
            FARPROC* dest = (FARPROC *)(desc->ImportAddressTableRVA + ___imageBase);
            while(*dest) *src++ = *dest++;
            FreeLibrary(*hmod_ptr);
            *hmod_ptr = NULL;
            releaseWriteAccess();
            rv = TRUE;
        }

    }
    return rv;
}

HRESULT WINAPI __HrLoadAllImportsForDll(LPCSTR szDll) 
{
    HRESULT rv = HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    PCIMAGE_DELAYLOAD_DESCRIPTOR desc= getDesc(szDll);

    if (desc)
    {
        FARPROC *iatPtr = (FARPROC *)(desc->ImportAddressTableRVA + ___imageBase)	;

        while (*iatPtr)
        {
            __delayLoadHelper2(desc, iatPtr++);
        }
        rv = S_OK;
    }
    return rv;
}