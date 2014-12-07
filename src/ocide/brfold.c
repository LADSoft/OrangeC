/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/

#include <shlobj.h>
#include <stdio.h>

int ExtendedMessageBox(char *title, int flag, char *fmt, ...);

extern char szInstallPath[1024];

static char selPath[MAX_PATH];
// Macros for interface casts
#ifdef __cplusplus
#define IID_PPV_ARG(IType, ppType) IID_##IType, reinterpret_cast<void **>(ppType)
#else
#define IID_PPV_ARG(IType, ppType) &IID_##IType, (void**)(ppType)
#endif

// Retrieves the UIObject interface for the specified full PIDL
STDAPI SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
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
 
#define ILSkip(pidl, cb)       ((LPITEMIDLIST)(((BYTE*)(pidl))+cb))
#define ILNext(pidl)           ILSkip(pidl, (pidl)->mkid.cb)
 
static HRESULT SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidl)
{
    DWORD cbTotal = 0;

    if (pidl)
    {
        LPCITEMIDLIST pidl_temp = pidl;
        cbTotal += sizeof (pidl_temp->mkid.cb);

        while (pidl_temp->mkid.cb) 
        {
            cbTotal += pidl_temp->mkid.cb;
            pidl_temp = ILNext (pidl_temp);
        }
    }
    
    *ppidl = (LPITEMIDLIST)CoTaskMemAlloc(cbTotal);
    
    if (*ppidl)
        CopyMemory(*ppidl, pidl, cbTotal);
 
    return  *ppidl ? S_OK: E_OUTOFMEMORY;
}
 
// Get the target PIDL for a folder PIDL. This also deals with cases of a folder  
// shortcut or an alias to a real folder.
STDAPI SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl)
{
    IShellLink *psl;
    
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
        SHGetPathFromIDListW(pidlTarget, pszPath);   // Make sure it is a path
        CoTaskMemFree(pidlTarget);
    }
    
    return *pszPath ? S_OK : E_FAIL;
}
void GetDefaultProjectsPath(LPSTR pszPath)
{
    HANDLE hndl;
    WIN32_FIND_DATA data;
    char ispth[MAX_PATH];
    sprintf(ispth,"%s\\appdata",szInstallPath);
    hndl = FindFirstFile(ispth, &data);
    if (hndl != INVALID_HANDLE_VALUE)
    {
    sprintf(pszPath, "%s\\Orange C Projects", szInstallPath);
        FindClose(hndl);
    }
    else if (SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, pszPath) == S_OK)
    {
        strcat(pszPath,"\\Orange C Projects");
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
            ExtendedMessageBox("Invalid selection",0,"Please select a folder");
            return 1; // keep going
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
    bi.pszDisplayName = pszDisplayName;
    bi.lpszTitle = "Choose a folder";
    bi.ulFlags = BIF_USENEWUI;
    bi.lpfn = selPath[0] ? selectionProc : NULL;
    bi.lParam = 0;

    pidlSelected = SHBrowseForFolder(&bi);

    
    if (pidlSelected)
    {
        rv = SHGetTargetFolderPath(pidlSelected, intermedPath, MAX_PATH);
        if (rv == S_OK)
        {
            WCHAR *s =intermedPath;
            char *d = pszPath;
            while (*s)
                *d++ = *s++;
            *d = 0;
        }
        CoTaskMemFree(pidlSelected);
    }
    return rv;
}

