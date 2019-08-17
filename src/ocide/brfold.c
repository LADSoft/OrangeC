/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <shlobj.h>
#include <stdio.h>

int ExtendedMessageBox(char* title, int flag, char* fmt, ...);

extern char szInstallPath[1024];

static char selPath[MAX_PATH];
static char tempPath[MAX_PATH];

#ifdef HAVE_UNISTD_H

const IID IID_IShellLinkA = {0};
const IID IID_IShellFolder = {0};

#endif
// Macros for interface casts
#ifdef __cplusplus
#    define IID_PPV_ARG(IType, ppType) IID_##IType, reinterpret_cast<void**>(ppType)
#else
#    define IID_PPV_ARG(IType, ppType) &IID_##IType, (void**)(ppType)
#endif

// Retrieves the UIObject interface for the specified full PIDL
STDAPI SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void** ppv)
{
    LPCITEMIDLIST pidlChild;
    IShellFolder* psf;

    HRESULT hr = SHBindToParent(pidl, IID_PPV_ARG(IShellFolder, &psf), &pidlChild);

    *ppv = NULL;

    if (SUCCEEDED(hr))
    {
        hr = psf->lpVtbl->GetUIObjectOf(psf, hwnd, 1, &pidlChild, riid, NULL, ppv);
        psf->lpVtbl->Release(psf);
    }
    return hr;
}
#ifndef ILSkip  // MSVC defines this better
#    define ILSkip(pidl, cb) ((LPITEMIDLIST)(((BYTE*)(pidl)) + cb))
#endif
#define ILNext(pidl) ILSkip(pidl, (pidl)->mkid.cb)

static HRESULT SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST* ppidl)
{
    DWORD cbTotal = 0;

    if (pidl)
    {
        LPCITEMIDLIST pidl_temp = pidl;
        cbTotal += sizeof(pidl_temp->mkid.cb);

        while (pidl_temp->mkid.cb)
        {
            cbTotal += pidl_temp->mkid.cb;
            pidl_temp = ILNext(pidl_temp);
        }
    }

    *ppidl = (LPITEMIDLIST)CoTaskMemAlloc(cbTotal);

    if (*ppidl)
        CopyMemory(*ppidl, pidl, cbTotal);

    return *ppidl ? S_OK : E_OUTOFMEMORY;
}

// Get the target PIDL for a folder PIDL. This also deals with cases of a folder
// shortcut or an alias to a real folder.
STDAPI SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST* ppidl)
{
    IShellLink* psl;

    HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, IID_PPV_ARG(IShellLink, &psl));

    *ppidl = NULL;

    if (SUCCEEDED(hr))
    {
        hr = psl->lpVtbl->GetIDList(psl, ppidl);
        psl->lpVtbl->Release(psl);
    }

    // It's not a folder shortcut so get the PIDL normally.
    if (FAILED(hr))
        hr = SHILClone(pidlFolder, ppidl);

    return hr;
}

// Get the target folder for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, the My Documents folder, and
// other items of that nature.
STDAPI SHGetTargetFolderPath(LPCITEMIDLIST pidlFolder, LPWSTR pszPath, UINT cchPath)
{
    LPITEMIDLIST pidlTarget;

    HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);

    *pszPath = 0;

    if (SUCCEEDED(hr))
    {
        SHGetPathFromIDListW(pidlTarget, pszPath);  // Make sure it is a path
        CoTaskMemFree(pidlTarget);
    }

    return *pszPath ? S_OK : E_FAIL;
}
void GetDefaultProjectsPath(LPSTR pszPath)
{
    HANDLE hndl;
    WIN32_FIND_DATA data;
    char ispth[MAX_PATH];
    sprintf(ispth, "%s\\appdata", szInstallPath);
    hndl = FindFirstFile(ispth, &data);
    if (hndl != INVALID_HANDLE_VALUE)
    {
        sprintf(pszPath, "%s\\Orange C Projects", szInstallPath);
        FindClose(hndl);
        CreateDirectory(pszPath, NULL);
    }
    else if (getenv("PUBLIC"))
    {
        strcpy(pszPath, getenv("PUBLIC"));
        strcat(pszPath, "\\Orange C Projects");
    }
    else
    {
        pszPath[0] = 0;
    }
}
static int CALLBACK selectionProc(HWND hwnd, UINT iMessage, LPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case BFFM_INITIALIZED:
            SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)selPath);
            break;
        case BFFM_VALIDATEFAILED:
            ExtendedMessageBox("Invalid selection", 0, "Please select a folder");
            return 1;  // keep going
    }
    return 0;
}
DWORD BrowseForFile(HWND hwnd, LPSTR pszDisplayName, LPSTR pszPath, UINT cchPath)
{
    DWORD rv = E_FAIL;
    LPITEMIDLIST pidlSelected = NULL;
    WCHAR intermedPath[MAX_PATH];
    BROWSEINFO bi = {0};

    if (pszPath[0])
        strcpy(selPath, pszPath);
    else
        GetDefaultProjectsPath(selPath);

    bi.hwndOwner = hwnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = tempPath;
    bi.lpszTitle = pszDisplayName;
    bi.ulFlags = BIF_USENEWUI;
    bi.lpfn = selPath[0] ? selectionProc : NULL;
    bi.lParam = 0;

    pidlSelected = SHBrowseForFolder(&bi);

    if (pidlSelected)
    {
        rv = SHGetTargetFolderPath(pidlSelected, intermedPath, MAX_PATH);
        if (rv == S_OK)
        {
            WCHAR* s = intermedPath;
            char* d = pszPath;
            while (*s)
                *d++ = *s++;
            *d = 0;
        }
        CoTaskMemFree(pidlSelected);
    }
    return rv;
}
