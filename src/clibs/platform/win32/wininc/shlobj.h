#ifndef _SHLOBJ_H
#define _SHLOBJ_H
#if __GNUC__ >= 3
#    pragma GCC system_header
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <ole2.h>
#include <shlguid.h>
#include <shellapi.h>
#pragma pack(push, 1)
#include <commctrl.h>

#define BIF_RETURNONLYFSDIRS 1
#define BIF_DONTGOBELOWDOMAIN 2
#define BIF_STATUSTEXT 4
#define BIF_RETURNFSANCESTORS 8
#define BIF_EDITBOX 16
#define BIF_VALIDATE 32
#define BIF_NEWDIALOGSTYLE 64
#define BIF_BROWSEINCLUDEURLS 128
#define BIF_USENEWUI (BIF_EDITBOX | BIF_NEWDIALOGSTYLE)
#define BIF_BROWSEFORCOMPUTER 0x1000
#define BIF_BROWSEFORPRINTER 0x2000
#define BIF_BROWSEINCLUDEFILES 0x4000
#define BIF_SHAREABLE 0x8000
#define BFFM_INITIALIZED 1
#define BFFM_SELCHANGED 2
#define BFFM_VALIDATEFAILEDA 3
#define BFFM_VALIDATEFAILEDW 4
#define BFFM_SETSTATUSTEXTA (WM_USER + 100)
#define BFFM_SETSTATUSTEXTW (WM_USER + 104)
#define BFFM_ENABLEOK (WM_USER + 101)
#define BFFM_SETSELECTIONA (WM_USER + 102)
#define BFFM_SETSELECTIONW (WM_USER + 103)
#define BFFM_SETOKTEXT (WM_USER + 105)
#define BFFM_SETEXPANDED (WM_USER + 106)
#ifdef UNICODE
#    define BFFM_SETSTATUSTEXT BFFM_SETSTATUSTEXTW
#    define BFFM_SETSELECTION BFFM_SETSELECTIONW
#    define BFFM_VALIDATEFAILED BFFM_VALIDATEFAILEDW
#else
#    define BFFM_SETSTATUSTEXT BFFM_SETSTATUSTEXTA
#    define BFFM_SETSELECTION BFFM_SETSELECTIONA
#    define BFFM_VALIDATEFAILED BFFM_VALIDATEFAILEDA
#endif
#define DVASPECT_SHORTNAME 2
    typedef enum tagSHARD
    {
        SHARD_PIDL = 0x00000001,
        SHARD_PATHA = 0x00000002,
        SHARD_PATHW = 0x00000003,
        SHARD_APPIDINFO = 0x00000004,
        SHARD_APPIDINFOIDLIST = 0x00000005,
        SHARD_LINK = 0x00000006,
        SHARD_APPIDINFOLINK = 0x00000007,
        SHARD_SHELLITEM = 0x00000008
    } SHARD;
#ifdef UNICODE
#    define SHARD_PATH SHARD_PATHW
#else
#    define SHARD_PATH SHARD_PATHA
#endif
#define SHCNE_RENAMEITEM 1
#define SHCNE_CREATE 2
#define SHCNE_DELETE 4
#define SHCNE_MKDIR 8
#define SHCNE_RMDIR 16
#define SHCNE_MEDIAINSERTED 32
#define SHCNE_MEDIAREMOVED 64
#define SHCNE_DRIVEREMOVED 128
#define SHCNE_DRIVEADD 256
#define SHCNE_NETSHARE 512
#define SHCNE_NETUNSHARE 1024
#define SHCNE_ATTRIBUTES 2048
#define SHCNE_UPDATEDIR 4096
#define SHCNE_UPDATEITEM 8192
#define SHCNE_SERVERDISCONNECT 16384
#define SHCNE_UPDATEIMAGE 32768
#define SHCNE_DRIVEADDGUI 65536
#define SHCNE_RENAMEFOLDER 0x20000
#define SHCNE_FREESPACE 0x40000
#define SHCNE_ASSOCCHANGED 0x8000000
#define SHCNE_DISKEVENTS 0x2381F
#define SHCNE_GLOBALEVENTS 0xC0581E0
#define SHCNE_ALLEVENTS 0x7FFFFFFF
#define SHCNE_INTERRUPT 0x80000000
#define SHCNF_IDLIST 0
#define SHCNF_PATHA 1
#define SHCNF_PRINTERA 2
#define SHCNF_DWORD 3
#define SHCNF_PATHW 5
#define SHCNF_PRINTERW 6
#define SHCNF_TYPE 0xFF
#define SHCNF_FLUSH 0x1000
#define SHCNF_FLUSHNOWAIT 0x2000
#ifdef UNICODE
#    define SHCNF_PATH SHCNF_PATHW
#    define SHCNF_PRINTER SHCNF_PRINTERW
#else
#    define SHCNF_PATH SHCNF_PATHA
#    define SHCNF_PRINTER SHCNF_PRINTERA
#endif
#define SFGAO_CANCOPY DROPEFFECT_COPY
#define SFGAO_CANMOVE DROPEFFECT_MOVE
#define SFGAO_CANLINK DROPEFFECT_LINK
#define SFGAO_CANRENAME 0x00000010L
#define SFGAO_CANDELETE 0x00000020L
#define SFGAO_HASPROPSHEET 0x00000040L
#define SFGAO_DROPTARGET 0x00000100L
#define SFGAO_CAPABILITYMASK 0x00000177L
#define SFGAO_ISSLOW 0x00004000L
#define SFGAO_GHOSTED 0x00008000L
#define SFGAO_LINK 0x00010000L
#define SFGAO_SHARE 0x00020000L
#define SFGAO_READONLY 0x00040000L
#define SFGAO_HIDDEN 0x00080000L
#define SFGAO_DISPLAYATTRMASK (SFGAO_ISSLOW | SFGAO_GHOSTED | SFGAO_LINK | SFGAO_SHARE | SFGAO_READONLY | SFGAO_HIDDEN)
#define SFGAO_FILESYSANCESTOR 0x10000000L
#define SFGAO_FOLDER 0x20000000L
#define SFGAO_FILESYSTEM 0x40000000L
#define SFGAO_HASSUBFOLDER 0x80000000L
#define SFGAO_CONTENTSMASK 0x80000000L
#define SFGAO_VALIDATE 0x01000000L
#define SFGAO_REMOVABLE 0x02000000L
#define SFGAO_COMPRESSED 0x04000000L
#define STRRET_WSTR 0
#define STRRET_OFFSET 1
#define STRRET_CSTR 2
#define SHGDFIL_FINDDATA 1
#define SHGDFIL_NETRESOURCE 2
#define SHGDFIL_DESCRIPTIONID 3
#define SHDID_ROOT_REGITEM 1
#define SHDID_FS_FILE 2
#define SHDID_FS_DIRECTORY 3
#define SHDID_FS_OTHER 4
#define SHDID_COMPUTER_DRIVE35 5
#define SHDID_COMPUTER_DRIVE525 6
#define SHDID_COMPUTER_REMOVABLE 7
#define SHDID_COMPUTER_FIXED 8
#define SHDID_COMPUTER_NETDRIVE 9
#define SHDID_COMPUTER_CDROM 10
#define SHDID_COMPUTER_RAMDISK 11
#define SHDID_COMPUTER_OTHER 12
#define SHDID_NET_DOMAIN 13
#define SHDID_NET_SERVER 14
#define SHDID_NET_SHARE 15
#define SHDID_NET_RESTOFNET 16
#define SHDID_NET_OTHER 17
#ifndef REGSTR_PATH_EXPLORER
#    define REGSTR_PATH_EXPLORER TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer")
#endif
#define REGSTR_PATH_SPECIAL_FOLDERS REGSTR_PATH_EXPLORER TEXT("\\Shell Folders")
#define CSIDL_DESKTOP 0
#define CSIDL_INTERNET 1
#define CSIDL_PROGRAMS 2
#define CSIDL_CONTROLS 3
#define CSIDL_PRINTERS 4
#define CSIDL_PERSONAL 5
#define CSIDL_FAVORITES 6
#define CSIDL_STARTUP 7
#define CSIDL_RECENT 8
#define CSIDL_SENDTO 9
#define CSIDL_BITBUCKET 10
#define CSIDL_STARTMENU 11
#define CSIDL_MYMUSIC 13
#define CSIDL_MYVIDEO 14
#define CSIDL_DESKTOPDIRECTORY 16
#define CSIDL_DRIVES 17
#define CSIDL_NETWORK 18
#define CSIDL_NETHOOD 19
#define CSIDL_FONTS 20
#define CSIDL_TEMPLATES 21
#define CSIDL_COMMON_STARTMENU 22
#define CSIDL_COMMON_PROGRAMS 23
#define CSIDL_COMMON_STARTUP 24
#define CSIDL_COMMON_DESKTOPDIRECTORY 25
#define CSIDL_APPDATA 26
#define CSIDL_PRINTHOOD 27
#define CSIDL_LOCAL_APPDATA 28
#define CSIDL_ALTSTARTUP 29
#define CSIDL_COMMON_ALTSTARTUP 30
#define CSIDL_COMMON_FAVORITES 31
#define CSIDL_INTERNET_CACHE 32
#define CSIDL_COOKIES 33
#define CSIDL_HISTORY 34
#define CSIDL_COMMON_APPDATA 35
#define CSIDL_WINDOWS 36
#define CSIDL_SYSTEM 37
#define CSIDL_PROGRAM_FILES 38
#define CSIDL_MYPICTURES 39
#define CSIDL_PROFILE 40
#define CSIDL_SYSTEMX86 41
#define CSIDL_PROGRAM_FILESX86 42
#define CSIDL_PROGRAM_FILES_COMMON 43
#define CSIDL_PROGRAM_FILES_COMMONX86 44
#define CSIDL_COMMON_TEMPLATES 45
#define CSIDL_COMMON_DOCUMENTS 46
#define CSIDL_COMMON_ADMINTOOLS 47
#define CSIDL_ADMINTOOLS 48
#define CSIDL_CONNECTIONS 49
#define CSIDL_COMMON_MUSIC 53
#define CSIDL_COMMON_PICTURES 54
#define CSIDL_COMMON_VIDEO 55
#define CSIDL_RESOURCES 56
#define CSIDL_RESOURCES_LOCALIZED 57
#define CSIDL_COMMON_OEM_LINKS 58
#define CSIDL_CDBURN_AREA 59
#define CSIDL_COMPUTERSNEARME 61
#define CSIDL_FLAG_DONT_VERIFY 0x4000
#define CSIDL_FLAG_CREATE 0x8000
#define CSIDL_FLAG_MASK 0xFF00
#define CFSTR_SHELLIDLIST TEXT("Shell IDList Array")
#define CFSTR_SHELLIDLISTOFFSET TEXT("Shell Object Offsets")
#define CFSTR_NETRESOURCES TEXT("Net Resource")
#define CFSTR_FILECONTENTS TEXT("FileContents")
#define CFSTR_FILENAMEA TEXT("FileName")
#define CFSTR_FILENAMEMAPA TEXT("FileNameMap")
#define CFSTR_FILEDESCRIPTORA TEXT("FileGroupDescriptor")
#define CFSTR_INETURLA TEXT("UniformResourceLocator")
#define CFSTR_SHELLURL CFSTR_INETURLA
#define CFSTR_FILENAMEW TEXT("FileNameW")
#define CFSTR_FILENAMEMAPW TEXT("FileNameMapW")
#define CFSTR_FILEDESCRIPTORW TEXT("FileGroupDescriptorW")
#define CFSTR_INETURLW TEXT("UniformResourceLocatorW")
#ifdef UNICODE
#    define CFSTR_FILENAME CFSTR_FILENAMEW
#    define CFSTR_FILENAMEMAP CFSTR_FILENAMEMAPW
#    define CFSTR_FILEDESCRIPTOR CFSTR_FILEDESCRIPTORW
#    define CFSTR_INETURL CFSTR_INETURLW
#else
#    define CFSTR_FILENAME CFSTR_FILENAMEA
#    define CFSTR_FILENAMEMAP CFSTR_FILENAMEMAPA
#    define CFSTR_FILEDESCRIPTOR CFSTR_FILEDESCRIPTORA
#    define CFSTR_INETURL CFSTR_INETURLA
#endif
#define CFSTR_PRINTERGROUP TEXT("PrinterFriendlyName")
#define CFSTR_INDRAGLOOP TEXT("InShellDragLoop")
#define CFSTR_PASTESUCCEEDED TEXT("Paste Succeeded")
#define CFSTR_PERFORMEDDROPEFFECT TEXT("Performed DropEffect")
#define CFSTR_PREFERREDDROPEFFECT TEXT("Preferred DropEffect")
#define CMF_NORMAL 0
#define CMF_DEFAULTONLY 1
#define CMF_VERBSONLY 2
#define CMF_EXPLORE 4
#define CMF_NOVERBS 8
#define CMF_CANRENAME 16
#define CMF_NODEFAULT 32
#define CMF_INCLUDESTATIC 64
#define CMF_RESERVED 0xffff0000
#define GCS_VERBA 0
#define GCS_HELPTEXTA 1
#define GCS_VALIDATEA 2
#define GCS_VERBW 4
#define GCS_HELPTEXTW 5
#define GCS_VALIDATEW 6
#define GCS_UNICODE 4
#ifdef UNICODE
#    define GCS_VERB GCS_VERBW
#    define GCS_HELPTEXT GCS_HELPTEXTW
#    define GCS_VALIDATE GCS_VALIDATEW
#else
#    define GCS_VERB GCS_VERBA
#    define GCS_HELPTEXT GCS_HELPTEXTA
#    define GCS_VALIDATE GCS_VALIDATEA
#endif
#define CMDSTR_NEWFOLDERA "NewFolder"
#define CMDSTR_VIEWLISTA "ViewList"
#define CMDSTR_VIEWDETAILSA "ViewDetails"
#define CMDSTR_NEWFOLDERW L"NewFolder"
#define CMDSTR_VIEWLISTW L"ViewList"
#define CMDSTR_VIEWDETAILSW L"ViewDetails"
#ifdef UNICODE
#    define CMDSTR_NEWFOLDER CMDSTR_NEWFOLDERW
#    define CMDSTR_VIEWLIST CMDSTR_VIEWLISTW
#    define CMDSTR_VIEWDETAILS CMDSTR_VIEWDETAILSW
#else
#    define CMDSTR_NEWFOLDER CMDSTR_NEWFOLDER
#    define CMDSTR_VIEWLIST CMDSTR_VIEWLIST
#    define CMDSTR_VIEWDETAILS CMDSTR_VIEWDETAILS
#endif
#define CMIC_MASK_HOTKEY SEE_MASK_HOTKEY
#define CMIC_MASK_ICON SEE_MASK_ICON
#define CMIC_MASK_FLAG_NO_UI SEE_MASK_FLAG_NO_UI
#define CMIC_MASK_MODAL 0x80000000
#define CMIC_VALID_SEE_FLAGS SEE_VALID_CMIC_FLAGS
#define GIL_OPENICON 1
#define GIL_FORSHELL 2
#define GIL_SIMULATEDOC 1
#define GIL_PERINSTANCE 2
#define GIL_PERCLASS 4
#define GIL_NOTFILENAME 8
#define GIL_DONTCACHE 16
#define FVSIF_RECT 1
#define FVSIF_PINNED 2
#define FVSIF_NEWFAILED 0x8000000
#define FVSIF_NEWFILE 0x80000000
#define FVSIF_CANVIEWIT 0x40000000
#define CDBOSC_SETFOCUS 0
#define CDBOSC_KILLFOCUS 1
#define CDBOSC_SELCHANGE 2
#define CDBOSC_RENAME 3
#define FCIDM_SHVIEWFIRST 0
#define FCIDM_SHVIEWLAST 0x7fff
#define FCIDM_BROWSERFIRST 0xa000
#define FCIDM_BROWSERLAST 0xbf00
#define FCIDM_GLOBALFIRST 0x8000
#define FCIDM_GLOBALLAST 0x9fff
#define FCIDM_MENU_FILE FCIDM_GLOBALFIRST
#define FCIDM_MENU_EDIT (FCIDM_GLOBALFIRST + 0x0040)
#define FCIDM_MENU_VIEW (FCIDM_GLOBALFIRST + 0x0080)
#define FCIDM_MENU_VIEW_SEP_OPTIONS (FCIDM_GLOBALFIRST + 0x0081)
#define FCIDM_MENU_TOOLS (FCIDM_GLOBALFIRST + 0x00c0)
#define FCIDM_MENU_TOOLS_SEP_GOTO (FCIDM_GLOBALFIRST + 0x00c1)
#define FCIDM_MENU_HELP (FCIDM_GLOBALFIRST + 0x0100)
#define FCIDM_MENU_FIND (FCIDM_GLOBALFIRST + 0x0140)
#define FCIDM_MENU_EXPLORE (FCIDM_GLOBALFIRST + 0x0150)
#define FCIDM_MENU_FAVORITES (FCIDM_GLOBALFIRST + 0x0170)
#define FCIDM_TOOLBAR FCIDM_BROWSERFIRST
#define FCIDM_STATUS (FCIDM_BROWSERFIRST + 1)
#define SBSP_DEFBROWSER 0
#define SBSP_SAMEBROWSER 1
#define SBSP_NEWBROWSER 2
#define SBSP_DEFMODE 0
#define SBSP_OPENMODE 16
#define SBSP_EXPLOREMODE 32
#define SBSP_ABSOLUTE 0
#define SBSP_RELATIVE 0x1000
#define SBSP_PARENT 0x2000
#define SBSP_INITIATEDBYHLINKFRAME 0x80000000
#define SBSP_REDIRECT 0x40000000
#define FCW_STATUS 1
#define FCW_TOOLBAR 2
#define FCW_TREE 3
#define FCT_MERGE 1
#define FCT_CONFIGABLE 2
#define FCT_ADDTOEND 4
#define SVSI_DESELECT 0
#define SVSI_SELECT 1
#define SVSI_EDIT 3
#define SVSI_DESELECTOTHERS 4
#define SVSI_ENSUREVISIBLE 8
#define SVSI_FOCUSED 16
#define SVGIO_BACKGROUND 0
#define SVGIO_SELECTION 1
#define SVGIO_ALLVIEW 2
#define SV2GV_CURRENTVIEW ((UINT)-1)
#define SV2GV_DEFAULTVIEW ((UINT)-2)

    typedef ULONG SFGAOF;
    typedef DWORD SHGDNF;

    typedef struct _IDA
    {
        UINT cidl;
        UINT aoffset[1];
    } CIDA, *LPIDA;
    typedef struct _SHITEMID
    {
        USHORT cb;
        BYTE abID[1];
    } SHITEMID, *LPSHITEMID;
    typedef const SHITEMID* LPCSHITEMID;
    typedef struct _ITEMIDLIST
    {
        SHITEMID mkid;
    } ITEMIDLIST, *LPITEMIDLIST;
    typedef const ITEMIDLIST* LPCITEMIDLIST;
    typedef int(CALLBACK* BFFCALLBACK)(HWND, UINT, LPARAM, LPARAM);
    typedef struct _browseinfoA
    {
        HWND hwndOwner;
        LPCITEMIDLIST pidlRoot;
        LPSTR pszDisplayName;
        LPCSTR lpszTitle;
        UINT ulFlags;
        BFFCALLBACK lpfn;
        LPARAM lParam;
        int iImage;
    } BROWSEINFOA, *PBROWSEINFOA, *LPBROWSEINFOA;
    typedef struct _browseinfoW
    {
        HWND hwndOwner;
        LPCITEMIDLIST pidlRoot;
        LPWSTR pszDisplayName;
        LPCWSTR lpszTitle;
        UINT ulFlags;
        BFFCALLBACK lpfn;
        LPARAM lParam;
        int iImage;
    } BROWSEINFOW, *PBROWSEINFOW, *LPBROWSEINFOW;
    typedef struct _CMInvokeCommandInfo
    {
        DWORD cbSize;
        DWORD fMask;
        HWND hwnd;
        LPCSTR lpVerb;
        LPCSTR lpParameters;
        LPCSTR lpDirectory;
        int nShow;
        DWORD dwHotKey;
        HANDLE hIcon;
    } CMINVOKECOMMANDINFO, *LPCMINVOKECOMMANDINFO;
    typedef struct _DROPFILES
    {
        DWORD pFiles;
        POINT pt;
        BOOL fNC;
        BOOL fWide;
    } DROPFILES, *LPDROPFILES;
    typedef enum tagSHGDN
    {
        SHGDN_NORMAL = 0,
        SHGDN_INFOLDER,
        SHGDN_FOREDITING = 0x1000,
        SHGDN_INCLUDE_NONFILESYS = 0x2000,
        SHGDN_FORADDRESSBAR = 0x4000,
        SHGDN_FORPARSING = 0x8000
    } SHGNO;
    typedef enum tagSHCONTF
    {
        SHCONTF_FOLDERS = 32,
        SHCONTF_NONFOLDERS = 64,
        SHCONTF_INCLUDEHIDDEN = 128,
        SHCONTF_INIT_ON_FIRST_NEXT = 256,
        SHCONTF_NETPRINTERSRCH = 512,
        SHCONTF_SHAREABLE = 1024,
        SHCONTF_STORAGE = 2048
    } SHCONTF;
    typedef struct _STRRET
    {
        UINT uType;
        _ANONYMOUS_UNION union
        {
            LPWSTR pOleStr;
            UINT uOffset;
            char cStr[MAX_PATH];
        } DUMMYUNIONNAME;
    } STRRET, *LPSTRRET;
    typedef enum
    {
        FD_CLSID = 1,
        FD_SIZEPOINT = 2,
        FD_ATTRIBUTES = 4,
        FD_CREATETIME = 8,
        FD_ACCESSTIME = 16,
        FD_WRITESTIME = 32,
        FD_FILESIZE = 64,
        FD_LINKUI = 0x8000
    } FD_FLAGS;
    typedef struct _FILEDESCRIPTORA
    {
        DWORD dwFlags;
        CLSID clsid;
        SIZEL sizel;
        POINTL pointl;
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
        CHAR cFileName[MAX_PATH];
    } FILEDESCRIPTORA, *LPFILEDESCRIPTORA;
    typedef struct _FILEDESCRIPTORW
    {
        DWORD dwFlags;
        CLSID clsid;
        SIZEL sizel;
        POINTL pointl;
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
        WCHAR cFileName[MAX_PATH];
    } FILEDESCRIPTORW, *LPFILEDESCRIPTORW;
    typedef struct _FILEGROUPDESCRIPTORA
    {
        UINT cItems;
        FILEDESCRIPTORA fgd[1];
    } FILEGROUPDESCRIPTORA, *LPFILEGROUPDESCRIPTORA;
    typedef struct _FILEGROUPDESCRIPTORW
    {
        UINT cItems;
        FILEDESCRIPTORW fgd[1];
    } FILEGROUPDESCRIPTORW, *LPFILEGROUPDESCRIPTORW;
    typedef enum
    {
        SLR_NO_UI = 1,
        SLR_ANY_MATCH = 2,
        SLR_UPDATE = 4,
        SLR_NOUPDATE = 8,
        SLR_NOSEARCH = 16,
        SLR_NOTRACK = 32,
        SLR_NOLINKINFO = 64,
        SLR_INVOKE_MSI = 128
    } SLR_FLAGS;
    typedef enum
    {
        SLGP_SHORTPATH = 1,
        SLGP_UNCPRIORITY = 2,
        SLGP_RAWPATH = 4
    } SLGP_FLAGS;
    typedef PBYTE LPVIEWSETTINGS;
    typedef enum
    {
        FWF_AUTOARRANGE = 1,
        FWF_ABBREVIATEDNAMES = 2,
        FWF_SNAPTOGRID = 4,
        FWF_OWNERDATA = 8,
        FWF_BESTFITWINDOW = 16,
        FWF_DESKTOP = 32,
        FWF_SINGLESEL = 64,
        FWF_NOSUBFOLDERS = 128,
        FWF_TRANSPARENT = 256,
        FWF_NOCLIENTEDGE = 512,
        FWF_NOSCROLL = 0x400,
        FWF_ALIGNLEFT = 0x800,
        FWF_SINGLECLICKACTIVATE = 0x8000
    } FOLDERFLAGS;
    typedef enum
    {
        FVM_ICON = 1,
        FVM_SMALLICON,
        FVM_LIST,
        FVM_DETAILS
    } FOLDERVIEWMODE;
    typedef struct
    {
        UINT ViewMode;
        UINT fFlags;
    } FOLDERSETTINGS, *LPFOLDERSETTINGS;
    typedef const FOLDERSETTINGS* LPCFOLDERSETTINGS;
    typedef struct
    {
        DWORD cbSize;
        HWND hwndOwner;
        int iShow;
        DWORD dwFlags;
        RECT rect;
        LPUNKNOWN punkRel;
        OLECHAR strNewFile[MAX_PATH];
    } FVSHOWINFO, *LPFVSHOWINFO;
    typedef struct _NRESARRAY
    {
        UINT cItems;
        NETRESOURCE nr[1];
    } NRESARRAY, *LPNRESARRAY;
    enum
    {
        SBSC_HIDE,
        SBSC_SHOW,
        SBSC_TOGGLE,
        SBSC_QUERY
    };
    enum
    {
        SBCMDID_ENABLESHOWTREE,
        SBCMDID_SHOWCONTROL,
        SBCMDID_CANCELNAVIGATION,
        SBCMDID_MAYSAVECHANGES,
        SBCMDID_SETHLINKFRAME,
        SBCMDID_ENABLESTOP,
        SBCMDID_OPTIONS
    };
    typedef enum
    {
        SVUIA_DEACTIVATE,
        SVUIA_ACTIVATE_NOFOCUS,
        SVUIA_ACTIVATE_FOCUS,
        SVUIA_INPLACEACTIVATE
    } SVUIA_STATUS;
#if (_WIN32_IE >= 0x0500)
    typedef struct tagEXTRASEARCH
    {
        GUID guidSearch;
        WCHAR wszFriendlyName[80];
        WCHAR wszUrl[2084];
    } EXTRASEARCH, *LPEXTRASEARCH;
    typedef DWORD SHCOLSTATEF;
    typedef struct
    {
        GUID fmtid;
        DWORD pid;
    } SHCOLUMNID, *LPSHCOLUMNID;
    typedef const SHCOLUMNID* LPCSHCOLUMNID;
    typedef struct _SHELLDETAILS
    {
        int fmt;
        int cxChar;
        STRRET str;
    } SHELLDETAILS, *LPSHELLDETAILS;
    typedef struct
    {
        LPITEMIDLIST pidlTargetFolder;
        WCHAR szTargetParsingName[MAX_PATH];
        WCHAR szNetworkProvider[MAX_PATH];
        DWORD dwAttributes;
        int csidl;
    } PERSIST_FOLDER_TARGET_INFO;

    typedef enum
    {
        SHGFP_TYPE_CURRENT = 0,
        SHGFP_TYPE_DEFAULT = 1,
    } SHGFP_TYPE;
#endif

    DECLARE_ENUMERATOR_(IEnumIDList, LPITEMIDLIST);
    typedef IEnumIDList* LPENUMIDLIST;

#ifdef COBJMACROS
#    define IEnumIDList_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IEnumIDList_Release(T) (T)->lpVtbl->AddRef(T)
#    define IEnumIDList_AddRef(T) (T)->lpVtbl->Release(T)
#    define IEnumIDList_Next(T, a, b, c) (T)->lpVtbl->Next(T, a, b, c)
#    define IEnumIDList_Skip(T, a) (T)->lpVtbl->Skip(T, a)
#    define IEnumIDList_Reset(T) (T)->lpVtbl->Reset(T)
#    define IEnumIDList_Clone(T, a) (T)->lpVtbl->Clone(T, a)
#endif

#define INTERFACE IObjMgr
    DECLARE_INTERFACE_(IObjMgr, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(Append)(THIS_ IUnknown*) PURE;
        STDMETHOD(Remove)(THIS_ IUnknown*) PURE;
    };
#undef INTERFACE

#define INTERFACE IContextMenu
    DECLARE_INTERFACE_(IContextMenu, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(QueryContextMenu)(THIS_ HMENU, UINT, UINT, UINT, UINT) PURE;
        STDMETHOD(InvokeCommand)(THIS_ LPCMINVOKECOMMANDINFO) PURE;
        STDMETHOD(GetCommandString)(THIS_ UINT, UINT, PUINT, LPSTR, UINT) PURE;
    };
#undef INTERFACE
    typedef IContextMenu* LPCONTEXTMENU;

#ifdef COBJMACROS
#    define IContextMenu_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IContextMenu_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IContextMenu_Release(T) (T)->lpVtbl->Release(T)
#    define IContextMenu_QueryContextMenu(T, a, b, c, d, e) (T)->lpVtbl->QueryContextMenu(T, a, b, c, d, e)
#    define IContextMenu_InvokeCommand(T, a) (T)->lpVtbl->InvokeCommand(T, a)
#    define IContextMenu_GetCommandString(T, a, b, c, d, e) (T)->lpVtbl->GetCommandString(T, a, b, c, d, e)
#endif

#define INTERFACE IContextMenu2
    DECLARE_INTERFACE_(IContextMenu2, IContextMenu)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(QueryContextMenu)(THIS_ HMENU, UINT, UINT, UINT, UINT) PURE;
        STDMETHOD(InvokeCommand)(THIS_ LPCMINVOKECOMMANDINFO) PURE;
        STDMETHOD(GetCommandString)(THIS_ UINT, UINT, PUINT, LPSTR, UINT) PURE;
        STDMETHOD(HandleMenuMsg)(THIS_ UINT, WPARAM, LPARAM) PURE;
    };
#undef INTERFACE
    typedef IContextMenu2* LPCONTEXTMENU2;

#define INTERFACE IContextMenu3
    DECLARE_INTERFACE_(IContextMenu3, IContextMenu2)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(QueryContextMenu)(THIS_ HMENU, UINT, UINT, UINT, UINT) PURE;
        STDMETHOD(InvokeCommand)(THIS_ LPCMINVOKECOMMANDINFO) PURE;
        STDMETHOD(GetCommandString)(THIS_ UINT, UINT, PUINT, LPSTR, UINT) PURE;
        STDMETHOD(HandleMenuMsg)(THIS_ UINT, WPARAM, LPARAM) PURE;
        STDMETHOD(HandleMenuMsg2)(THIS_ UINT, WPARAM, LPARAM, LRESULT*) PURE;
    };
#undef INTERFACE
    typedef IContextMenu3* LPCONTEXTMENU3;

#if (_WIN32_IE >= 0x0500)
#    pragma pack(push, 8)
    typedef struct
    {
        ULONG dwFlags;
        ULONG dwReserved;
        WCHAR wszFolder[MAX_PATH];
    } SHCOLUMNINIT, *LPSHCOLUMNINIT;
    typedef const SHCOLUMNINIT* LPCSHCOLUMNINIT;
    typedef struct
    {
        ULONG dwFlags;
        DWORD dwFileAttributes;
        ULONG dwReserved;
        WCHAR* pwszExt;
        WCHAR wszFile[MAX_PATH];
    } SHCOLUMNDATA, *LPSHCOLUMNDATA;
    typedef const SHCOLUMNDATA* LPCSHCOLUMNDATA;
#    pragma pack(pop)

#    define MAX_COLUMN_NAME_LEN 80
#    define MAX_COLUMN_DESC_LEN 128

#    pragma pack(push, 1)
    typedef struct
    {
        SHCOLUMNID scid;
        VARTYPE vt;
        DWORD fmt;
        UINT cChars;
        DWORD csFlags;
        WCHAR wszTitle[MAX_COLUMN_NAME_LEN];
        WCHAR wszDescription[MAX_COLUMN_DESC_LEN];
    } SHCOLUMNINFO, *LPSHCOLUMNINFO;
    typedef const SHCOLUMNINFO* LPCSHCOLUMNINFO;
#    pragma pack(pop)

    typedef enum
    {
        SHCOLSTATE_TYPE_STR = 0x00000001,
        SHCOLSTATE_TYPE_INT = 0x00000002,
        SHCOLSTATE_TYPE_DATE = 0x00000003,
        SHCOLSTATE_TYPEMASK = 0x0000000f,
        SHCOLSTATE_ONBYDEFAULT = 0x00000010,
        SHCOLSTATE_SLOW = 0x00000020,
        SHCOLSTATE_EXTENDED = 0x00000040,
        SHCOLSTATE_SECONDARYUI = 0x00000080,
        SHCOLSTATE_HIDDEN = 0x00000100,
        SHCOLSTATE_PREFER_VARCMP = 0x00000200
    } SHCOLSTATE;

#    ifdef COBJMACROS
#        define IContextMenu2_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#        define IContextMenu2_AddRef(T) (T)->lpVtbl->AddRef(T)
#        define IContextMenu2_Release(T) (T)->lpVtbl->Release(T)
#        define IContextMenu2_QueryContextMenu(T, a, b, c, d, e) (T)->lpVtbl->QueryContextMenu(T, a, b, c, d, e)
#        define IContextMenu2_InvokeCommand(T, a) (T)->lpVtbl->InvokeCommand(T, a)
#        define IContextMenu2_GetCommandString(T, a, b, c, d, e) (T)->lpVtbl->GetCommandString(T, a, b, c, d, e)
#        define IContextMenu2_HandleMenuMsg(T, a, b, c) (T)->lpVtbl->HandleMenuMsg(T, a, b, c)
#    endif

#    ifdef COBJMACROS
#        define IContextMenu3_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#        define IContextMenu3_AddRef(T) (T)->lpVtbl->AddRef(T)
#        define IContextMenu3_Release(T) (T)->lpVtbl->Release(T)
#        define IContextMenu3_QueryContextMenu(T, a, b, c, d, e) (T)->lpVtbl->QueryContextMenu(T, a, b, c, d, e)
#        define IContextMenu3_InvokeCommand(T, a) (T)->lpVtbl->InvokeCommand(T, a)
#        define IContextMenu3_GetCommandString(T, a, b, c, d, e) (T)->lpVtbl->GetCommandString(T, a, b, c, d, e)
#        define IContextMenu3_HandleMenuMsg(T, a, b, c) (T)->lpVtbl->HandleMenuMsg(T, a, b, c)
#        define IContextMenu3_HandleMenuMsg2(T, a, b, c, d) (T)->lpVtbl->HandleMenuMsg(T, a, b, c, d)
#    endif

#    define INTERFACE IColumnProvider
    DECLARE_INTERFACE_(IColumnProvider, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(Initialize)(THIS_ LPCSHCOLUMNINIT) PURE;
        STDMETHOD(GetColumnInfo)(THIS_ DWORD, SHCOLUMNINFO*) PURE;
        STDMETHOD(GetItemData)(THIS_ LPCSHCOLUMNID, LPCSHCOLUMNDATA, VARIANT*) PURE;
    };
#    undef INTERFACE
#endif /* _WIN32_IE >= 0x0500 */

#define INTERFACE IQueryInfo
    DECLARE_INTERFACE_(IQueryInfo, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetInfoTip)(THIS_ DWORD, WCHAR**) PURE;
        STDMETHOD(GetInfoFlags)(THIS_ DWORD*) PURE;
    };
#undef INTERFACE

#define INTERFACE IShellExtInit
    DECLARE_INTERFACE_(IShellExtInit, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(Initialize)(THIS_ LPCITEMIDLIST, LPDATAOBJECT, HKEY) PURE;
    };
#undef INTERFACE
    typedef IShellExtInit* LPSHELLEXTINIT;

#define INTERFACE IShellPropSheetExt
    DECLARE_INTERFACE_(IShellPropSheetExt, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(AddPages)(THIS_ LPFNADDPROPSHEETPAGE, LPARAM) PURE;
        STDMETHOD(ReplacePage)(THIS_ UINT, LPFNADDPROPSHEETPAGE, LPARAM) PURE;
    };
#undef INTERFACE
    typedef IShellPropSheetExt* LPSHELLPROPSHEETEXT;

#define INTERFACE IExtractIconA
    DECLARE_INTERFACE_(IExtractIconA, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetIconLocation)(THIS_ UINT, LPSTR, UINT, int*, PUINT) PURE;
        STDMETHOD(Extract)(THIS_ LPCSTR, UINT, HICON*, HICON*, UINT) PURE;
    };
#undef INTERFACE
    typedef IExtractIconA* LPEXTRACTICONA;

#ifdef COBJMACROS
#    define IExtractIconA_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IExtractIconA_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IExtractIconA_Release(T) (T)->lpVtbl->Release(T)
#    define IExtractIconA_GetIconLocation(T, a, b, c, d, e) (T)->lpVtbl->GetIconLocation(T, a, b, c, d, e)
#    define IExtractIconA_Extract(T, a, b, c, d, e) (T)->lpVtbl->Extract(T, a, b, c, d, e)
#endif

#define INTERFACE IExtractIconW
    DECLARE_INTERFACE_(IExtractIconW, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetIconLocation)(THIS_ UINT, LPWSTR, UINT, int*, PUINT) PURE;
        STDMETHOD(Extract)(THIS_ LPCWSTR, UINT, HICON*, HICON*, UINT) PURE;
    };
#undef INTERFACE
    typedef IExtractIconW* LPEXTRACTICONW;

#ifdef COBJMACROS
#    define IExtractIconW_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IExtractIconW_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IExtractIconW_Release(T) (T)->lpVtbl->Release(T)
#    define IExtractIconW_GetIconLocation(T, a, b, c, d, e) (T)->lpVtbl->GetIconLocation(T, a, b, c, d, e)
#    define IExtractIconW_Extract(T, a, b, c, d, e) (T)->lpVtbl->Extract(T, a, b, c, d, e)
#endif

#ifdef UNICODE
#    define IExtractIcon IExtractIconW
#    define LPEXTRACTICON LPEXTRACTICONW
#else
#    define IExtractIcon IExtractIconA
#    define LPEXTRACTICON LPEXTRACTICONA
#endif

#define INTERFACE IShellLinkA
    DECLARE_INTERFACE_(IShellLinkA, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetPath)(THIS_ LPSTR, int, WIN32_FIND_DATAA*, DWORD) PURE;
        STDMETHOD(GetIDList)(THIS_ LPITEMIDLIST*) PURE;
        STDMETHOD(SetIDList)(THIS_ LPCITEMIDLIST) PURE;
        STDMETHOD(GetDescription)(THIS_ LPSTR, int) PURE;
        STDMETHOD(SetDescription)(THIS_ LPCSTR) PURE;
        STDMETHOD(GetWorkingDirectory)(THIS_ LPSTR, int) PURE;
        STDMETHOD(SetWorkingDirectory)(THIS_ LPCSTR) PURE;
        STDMETHOD(GetArguments)(THIS_ LPSTR, int) PURE;
        STDMETHOD(SetArguments)(THIS_ LPCSTR) PURE;
        STDMETHOD(GetHotkey)(THIS_ PWORD) PURE;
        STDMETHOD(SetHotkey)(THIS_ WORD) PURE;
        STDMETHOD(GetShowCmd)(THIS_ int*) PURE;
        STDMETHOD(SetShowCmd)(THIS_ int) PURE;
        STDMETHOD(GetIconLocation)(THIS_ LPSTR, int, int*) PURE;
        STDMETHOD(SetIconLocation)(THIS_ LPCSTR, int) PURE;
        STDMETHOD(SetRelativePath)(THIS_ LPCSTR, DWORD) PURE;
        STDMETHOD(Resolve)(THIS_ HWND, DWORD) PURE;
        STDMETHOD(SetPath)(THIS_ LPCSTR) PURE;
    };
#undef INTERFACE

#ifdef COBJMACROS
#    define IShellLinkA_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IShellLinkA_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IShellLinkA_Release(T) (T)->lpVtbl->Release(T)
#    define IShellLinkA_GetPath(T, a, b, c, d) (T)->lpVtbl->GetPath(T, a, b, c, d)
#    define IShellLinkA_GetIDList(T, a) (T)->lpVtbl->GetIDList(T, a)
#    define IShellLinkA_SetIDList(T, a) (T)->lpVtbl->SetIDList(T, a)
#    define IShellLinkA_GetDescription(T, a, b) (T)->lpVtbl->GetDescription(T, a, b)
#    define IShellLinkA_SetDescription(T, a) (T)->lpVtbl->SetDescription(T, a)
#    define IShellLinkA_GetWorkingDirectory(T, a, b) (T)->lpVtbl->GetWorkingDirectory(T, a, b)
#    define IShellLinkA_SetWorkingDirectory(T, a) (T)->lpVtbl->SetWorkingDirectory(T, a)
#    define IShellLinkA_GetArguments(T, a, b) (T)->lpVtbl->GetArguments(T, a, b)
#    define IShellLinkA_SetArguments(T, a) (T)->lpVtbl->SetArguments(T, a)
#    define IShellLinkA_GetHotkey(T, a) (T)->lpVtbl->GetHotkey(T, a)
#    define IShellLinkA_SetHotkey(T, a) (T)->lpVtbl->SetHotkey(T, a)
#    define IShellLinkA_GetShowCmd(T, a) (T)->lpVtbl->GetShowCmd(T, a)
#    define IShellLinkA_SetShowCmd(T, a) (T)->lpVtbl->SetShowCmd(T, a)
#    define IShellLinkA_GetIconLocation(T, a, b, c) (T)->lpVtbl->GetIconLocation(T, a, b, c)
#    define IShellLinkA_SetIconLocation(T, a, b) (T)->lpVtbl->SetIconLocation(T, a, b)
#    define IShellLinkA_SetRelativePath(T, a, b) (T)->lpVtbl->SetRelativePath(T, a, b)
#    define IShellLinkA_Resolve(T, a, b) (T)->lpVtbl->Resolve(T, a, b)
#    define IShellLinkA_SetPath(T, a) (T)->lpVtbl->SetPath(T, a)
#endif

#define INTERFACE IShellLinkW
    DECLARE_INTERFACE_(IShellLinkW, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetPath)(THIS_ LPWSTR, int, WIN32_FIND_DATAW*, DWORD) PURE;
        STDMETHOD(GetIDList)(THIS_ LPITEMIDLIST*) PURE;
        STDMETHOD(SetIDList)(THIS_ LPCITEMIDLIST) PURE;
        STDMETHOD(GetDescription)(THIS_ LPWSTR, int) PURE;
        STDMETHOD(SetDescription)(THIS_ LPCWSTR) PURE;
        STDMETHOD(GetWorkingDirectory)(THIS_ LPWSTR, int) PURE;
        STDMETHOD(SetWorkingDirectory)(THIS_ LPCWSTR) PURE;
        STDMETHOD(GetArguments)(THIS_ LPWSTR, int) PURE;
        STDMETHOD(SetArguments)(THIS_ LPCWSTR) PURE;
        STDMETHOD(GetHotkey)(THIS_ PWORD) PURE;
        STDMETHOD(SetHotkey)(THIS_ WORD) PURE;
        STDMETHOD(GetShowCmd)(THIS_ int*) PURE;
        STDMETHOD(SetShowCmd)(THIS_ int) PURE;
        STDMETHOD(GetIconLocation)(THIS_ LPWSTR, int, int*) PURE;
        STDMETHOD(SetIconLocation)(THIS_ LPCWSTR, int) PURE;
        STDMETHOD(SetRelativePath)(THIS_ LPCWSTR, DWORD) PURE;
        STDMETHOD(Resolve)(THIS_ HWND, DWORD) PURE;
        STDMETHOD(SetPath)(THIS_ LPCWSTR) PURE;
    };
#undef INTERFACE

#ifdef COBJMACROS
#    define IShellLinkW_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IShellLinkW_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IShellLinkW_Release(T) (T)->lpVtbl->Release(T)
#    define IShellLinkW_GetPath(T, a, b, c, d) (T)->lpVtbl->GetPath(T, a, b, c, d)
#    define IShellLinkW_GetIDList(T, a) (T)->lpVtbl->GetIDList(T, a)
#    define IShellLinkW_SetIDList(T, a) (T)->lpVtbl->SetIDList(T, a)
#    define IShellLinkW_GetDescription(T, a, b) (T)->lpVtbl->GetDescription(T, a, b)
#    define IShellLinkW_SetDescription(T, a) (T)->lpVtbl->SetDescription(T, a)
#    define IShellLinkW_GetWorkingDirectory(T, a, b) (T)->lpVtbl->GetWorkingDirectory(T, a, b)
#    define IShellLinkW_SetWorkingDirectory(T, a) (T)->lpVtbl->SetWorkingDirectory(T, a)
#    define IShellLinkW_GetArguments(T, a, b) (T)->lpVtbl->GetArguments(T, a, b)
#    define IShellLinkW_SetArguments(T, a) (T)->lpVtbl->SetArguments(T, a)
#    define IShellLinkW_GetHotkey(T, a) (T)->lpVtbl->GetHotkey(T, a)
#    define IShellLinkW_SetHotkey(T, a) (T)->lpVtbl->SetHotkey(T, a)
#    define IShellLinkW_GetShowCmd(T, a) (T)->lpVtbl->GetShowCmd(T, a)
#    define IShellLinkW_SetShowCmd(T, a) (T)->lpVtbl->SetShowCmd(T, a)
#    define IShellLinkW_GetIconLocation(T, a, b, c) (T)->lpVtbl->GetIconLocation(T, a, b, c)
#    define IShellLinkW_SetIconLocation(T, a, b) (T)->lpVtbl->SetIconLocation(T, a, b)
#    define IShellLinkW_SetRelativePath(T, a, b) (T)->lpVtbl->SetRelativePath(T, a, b)
#    define IShellLinkW_Resolve(T, a, b) (T)->lpVtbl->Resolve(T, a, b)
#    define IShellLinkW_SetPath(T, a) (T)->lpVtbl->SetPath(T, a)
#endif

#define INTERFACE IShellFolder
    DECLARE_INTERFACE_(IShellFolder, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(ParseDisplayName)(THIS_ HWND, LPBC, LPOLESTR, PULONG, LPITEMIDLIST*, PULONG) PURE;
        STDMETHOD(EnumObjects)(THIS_ HWND, DWORD, LPENUMIDLIST*) PURE;
        STDMETHOD(BindToObject)(THIS_ LPCITEMIDLIST, LPBC, REFIID, PVOID*) PURE;
        STDMETHOD(BindToStorage)(THIS_ LPCITEMIDLIST, LPBC, REFIID, PVOID*) PURE;
        STDMETHOD(CompareIDs)(THIS_ LPARAM, LPCITEMIDLIST, LPCITEMIDLIST) PURE;
        STDMETHOD(CreateViewObject)(THIS_ HWND, REFIID, PVOID*) PURE;
        STDMETHOD(GetAttributesOf)(THIS_ UINT, LPCITEMIDLIST*, PULONG) PURE;
        STDMETHOD(GetUIObjectOf)(THIS_ HWND, UINT, LPCITEMIDLIST*, REFIID, PUINT, PVOID*) PURE;
        STDMETHOD(GetDisplayNameOf)(THIS_ LPCITEMIDLIST, DWORD, LPSTRRET) PURE;
        STDMETHOD(SetNameOf)(THIS_ HWND, LPCITEMIDLIST, LPCOLESTR, DWORD, LPITEMIDLIST*) PURE;
    };
#undef INTERFACE
    typedef IShellFolder* LPSHELLFOLDER;

#ifdef COBJMACROS
#    define IShellFolder_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IShellFolder_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IShellFolder_Release(T) (T)->lpVtbl->Release(T)
#    define IShellFolder_ParseDisplayName(T, a, b, c, d, e, f) (T)->lpVtbl->ParseDisplayName(T, a, b, c, d, e, f)
#    define IShellFolder_EnumObjects(T, a, b, c) (T)->lpVtbl->EnumObjects(T, a, b, c)
#    define IShellFolder_BindToObject(T, a, b, c, d) (T)->lpVtbl->BindToObject(T, a, b, c, d)
#    define IShellFolder_BindToStorage(T, a, b, c, d) (T)->lpVtbl->BindToStorage(T, a, b, c, d)
#    define IShellFolder_CompareIDs(T, a, b, c) (T)->lpVtbl->CompareIDs(T, a, b, c)
#    define IShellFolder_CreateViewObject(T, a, b) (T)->lpVtbl->CreateViewObject(T, a, b)
#    define IShellFolder_GetAttributesOf(T, a, b, c) (T)->lpVtbl->GetAttributesOf(T, a, b, c)
#    define IShellFolder_GetUIObjectOf(T, a, b, c, d, e, f) (T)->lpVtbl->GetUIObjectOf(T, a, b, c, d, e, f)
#    define IShellFolder_GetDisplayNameOf(T, a, b, c) (T)->lpVtbl->GetDisplayNameOf(T, a, b, c)
#    define IShellFolder_SetNameOf(T, a, b, c, d, e) (T)->lpVtbl->SetNameOf(T, a, b, c, d, e)
#endif

#if (_WIN32_IE >= 0x0500)

    DECLARE_ENUMERATOR_(IEnumExtraSearch, LPEXTRASEARCH);
    typedef IEnumExtraSearch* LPENUMEXTRASEARCH;

#    define INTERFACE IShellFolder2
    DECLARE_INTERFACE_(IShellFolder2, IShellFolder)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(ParseDisplayName)(THIS_ HWND, LPBC, LPOLESTR, PULONG, LPITEMIDLIST*, PULONG) PURE;
        STDMETHOD(EnumObjects)(THIS_ HWND, DWORD, LPENUMIDLIST*) PURE;
        STDMETHOD(BindToObject)(THIS_ LPCITEMIDLIST, LPBC, REFIID, PVOID*) PURE;
        STDMETHOD(BindToStorage)(THIS_ LPCITEMIDLIST, LPBC, REFIID, PVOID*) PURE;
        STDMETHOD(CompareIDs)(THIS_ LPARAM, LPCITEMIDLIST, LPCITEMIDLIST) PURE;
        STDMETHOD(CreateViewObject)(THIS_ HWND, REFIID, PVOID*) PURE;
        STDMETHOD(GetAttributesOf)(THIS_ UINT, LPCITEMIDLIST*, PULONG) PURE;
        STDMETHOD(GetUIObjectOf)(THIS_ HWND, UINT, LPCITEMIDLIST*, REFIID, PUINT, PVOID*) PURE;
        STDMETHOD(GetDisplayNameOf)(THIS_ LPCITEMIDLIST, DWORD, LPSTRRET) PURE;
        STDMETHOD(SetNameOf)(THIS_ HWND, LPCITEMIDLIST, LPCOLESTR, DWORD, LPITEMIDLIST*) PURE;
        STDMETHOD(GetDefaultSearchGUID)(THIS_ GUID*) PURE;
        STDMETHOD(EnumSearches)(THIS_ IEnumExtraSearch**) PURE;
        STDMETHOD(GetDefaultColumn)(THIS_ DWORD, ULONG*, ULONG*) PURE;
        STDMETHOD(GetDefaultColumnState)(THIS_ UINT, SHCOLSTATEF*) PURE;
        STDMETHOD(GetDetailsEx)(THIS_ LPCITEMIDLIST, const SHCOLUMNID*, VARIANT*) PURE;
        STDMETHOD(GetDetailsOf)(THIS_ LPCITEMIDLIST, UINT, SHELLDETAILS*) PURE;
        STDMETHOD(MapColumnToSCID)(THIS_ UINT, SHCOLUMNID*) PURE;
    };
#    undef INTERFACE
    typedef IShellFolder2* LPSHELLFOLDER2;

#    ifdef COBJMACROS
#        define IShellFolder2_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#        define IShellFolder2_AddRef(T) (T)->lpVtbl->AddRef(T)
#        define IShellFolder2_Release(T) (T)->lpVtbl->Release(T)
#        define IShellFolder2_ParseDisplayName(T, a, b, c, d, e, f) (T)->lpVtbl->ParseDisplayName(T, a, b, c, d, e, f)
#        define IShellFolder2_EnumObjects(T, a, b, c) (T)->lpVtbl->EnumObjects(T, a, b, c)
#        define IShellFolder2_BindToObject(T, a, b, c, d) (T)->lpVtbl->BindToObject(T, a, b, c, d)
#        define IShellFolder2_BindToStorage(T, a, b, c, d) (T)->lpVtbl->BindToStorage(T, a, b, c, d)
#        define IShellFolder2_CompareIDs(T, a, b, c) (T)->lpVtbl->CompareIDs(T, a, b, c)
#        define IShellFolder2_CreateViewObject(T, a, b) (T)->lpVtbl->CreateViewObject(T, a, b)
#        define IShellFolder2_GetAttributesOf(T, a, b, c) (T)->lpVtbl->GetAttributesOf(T, a, b, c)
#        define IShellFolder2_GetUIObjectOf(T, a, b, c, d, e, f) (T)->lpVtbl->GetUIObjectOf(T, a, b, c, d, e, f)
#        define IShellFolder2_GetDisplayNameOf(T, a, b, c) (T)->lpVtbl->GetDisplayNameOf(T, a, b, c)
#        define IShellFolder2_SetNameOf(T, a, b, c, d, e) (T)->lpVtbl->SetNameOf(T, a, b, c, d, e)
#        define IShellFolder2_GetDefaultSearchGUID(T, a) (T)->lpVtbl->GetDefaultSearchGUID(T, a)
#        define IShellFolder2_EnumSearches(T, a) (T)->lpVtbl->EnumSearches(T, a)
#        define IShellFolder2_GetDefaultColumn(T, a, b, c) (T)->lpVtbl->GetDefaultColumn(T, a, b, c)
#        define IShellFolder2_GetDefaultColumnState(T, a, b) (T)->lpVtbl->GetDefaultColumnState(T, a, b)
#        define IShellFolder2_GetDetailsEx(T, a, b, c) (T)->lpVtbl->GetDetailsEx(T, a, b, c)
#        define IShellFolder2_GetDetailsOf(T, a, b, c) (T)->lpVtbl->GetDetailsOf(T, a, b, c)
#        define IShellFolder2_MapColumnToSCID(T, a, b) (T)->lpVtbl->MapColumnToSCID(T, a, b)
#    endif

#endif /* _WIN32_IE >= 0x0500 */
#define INTERFACE ICopyHook
    DECLARE_INTERFACE_(ICopyHook, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD_(UINT, CopyCallback)(THIS_ HWND, UINT, UINT, LPCSTR, DWORD, LPCSTR, DWORD) PURE;
    };
#undef INTERFACE
    typedef ICopyHook* LPCOPYHOOK;

#define INTERFACE IFileViewerSite
    DECLARE_INTERFACE(IFileViewerSite)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(SetPinnedWindow)(THIS_ HWND) PURE;
        STDMETHOD(GetPinnedWindow)(THIS_ HWND*) PURE;
    };
#undef INTERFACE
    typedef IFileViewerSite* LPFILEVIEWERSITE;

#define INTERFACE IFileViewer
    DECLARE_INTERFACE(IFileViewer)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(ShowInitialize)(THIS_ LPFILEVIEWERSITE) PURE;
        STDMETHOD(Show)(THIS_ LPFVSHOWINFO) PURE;
        STDMETHOD(PrintTo)(THIS_ LPSTR, BOOL) PURE;
    };
#undef INTERFACE
    typedef IFileViewer* LPFILEVIEWER;

#define INTERFACE IFileSystemBindData
    DECLARE_INTERFACE_(IFileSystemBindData, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(SetFindData)(THIS_ const WIN32_FIND_DATAW*) PURE;
        STDMETHOD(GetFindData)(THIS_ WIN32_FIND_DATAW*) PURE;
    };
#undef INTERFACE

#define INTERFACE IPersistFolder
    DECLARE_INTERFACE_(IPersistFolder, IPersist)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetClassID)(THIS_ CLSID*) PURE;
        STDMETHOD(Initialize)(THIS_ LPCITEMIDLIST) PURE;
    };
#undef INTERFACE
    typedef IPersistFolder* LPPERSISTFOLDER;

#ifdef COBJMACROS
#    define IPersistFolder_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IPersistFolder_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IPersistFolder_Release(T) (T)->lpVtbl->Release(T)
#    define IPersistFolder_GetClassID(T, a) (T)->lpVtbl->GetClassID(T, a)
#    define IPersistFolder_Initialize(T, a) (T)->lpVtbl->Initialize(T, a)
#endif

#if (_WIN32_IE >= 0x0400 || _WIN32_WINNT >= 0x0500)

#    define INTERFACE IPersistFolder2
    DECLARE_INTERFACE_(IPersistFolder2, IPersistFolder)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetClassID)(THIS_ CLSID*) PURE;
        STDMETHOD(Initialize)(THIS_ LPCITEMIDLIST) PURE;
        STDMETHOD(GetCurFolder)(THIS_ LPITEMIDLIST*) PURE;
    };
#    undef INTERFACE
    typedef IPersistFolder2* LPPERSISTFOLDER2;

#    ifdef COBJMACROS
#        define IPersistFolder2_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#        define IPersistFolder2_AddRef(T) (T)->lpVtbl->AddRef(T)
#        define IPersistFolder2_Release(T) (T)->lpVtbl->Release(T)
#        define IPersistFolder2_GetClassID(T, a) (T)->lpVtbl->GetClassID(T, a)
#        define IPersistFolder2_Initialize(T, a) (T)->lpVtbl->Initialize(T, a)
#        define IPersistFolder2_GetCurFolder(T, a) (T)->lpVtbl->GetCurFolder(T, a)
#    endif

#endif /* _WIN32_IE >= 0x0400 || _WIN32_WINNT >= 0x0500 */

#if (_WIN32_IE >= 0x0500)

#    define INTERFACE IPersistFolder3
    DECLARE_INTERFACE_(IPersistFolder3, IPersistFolder2)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetClassID)(THIS_ CLSID*) PURE;
        STDMETHOD(Initialize)(THIS_ LPCITEMIDLIST) PURE;
        STDMETHOD(GetCurFolder)(THIS_ LPITEMIDLIST*) PURE;
        STDMETHOD(InitializeEx)(THIS_ IBindCtx*, LPCITEMIDLIST, const PERSIST_FOLDER_TARGET_INFO*) PURE;
        STDMETHOD(GetFolderTargetInfo)(THIS_ PERSIST_FOLDER_TARGET_INFO*) PURE;
    };
#    undef INTERFACE
    typedef IPersistFolder3* LPPERSISTFOLDER3;

#    ifdef COBJMACROS
#        define IPersistFolder3_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#        define IPersistFolder3_AddRef(T) (T)->lpVtbl->AddRef(T)
#        define IPersistFolder3_Release(T) (T)->lpVtbl->Release(T)
#        define IPersistFolder3_GetClassID(T, a) (T)->lpVtbl->GetClassID(T, a)
#        define IPersistFolder3_Initialize(T, a) (T)->lpVtbl->Initialize(T, a)
#        define IPersistFolder3_GetCurFolder(T, a) (T)->lpVtbl->GetCurFolder(T, a)
#        define IPersistFolder3_InitializeEx(T, a, b, c) (T)->lpVtbl->InitializeEx(T, a, b, c)
#        define IPersistFolder3_GetFolderTargetInfo(T, a) (T)->lpVtbl->GetFolderTargetInfo(T, a)
#    endif

#endif /* _WIN32_IE >= 0x0500 */

    typedef _COM_interface IShellBrowser* LPSHELLBROWSER;
    typedef _COM_interface IShellView* LPSHELLVIEW;

#define INTERFACE IShellBrowser
    DECLARE_INTERFACE_(IShellBrowser, IOleWindow)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetWindow)(THIS_ HWND*) PURE;
        STDMETHOD(ContextSensitiveHelp)(THIS_ BOOL) PURE;
        STDMETHOD(InsertMenusSB)(THIS_ HMENU, LPOLEMENUGROUPWIDTHS) PURE;
        STDMETHOD(SetMenuSB)(THIS_ HMENU, HOLEMENU, HWND) PURE;
        STDMETHOD(RemoveMenusSB)(THIS_ HMENU) PURE;
        STDMETHOD(SetStatusTextSB)(THIS_ LPCOLESTR) PURE;
        STDMETHOD(EnableModelessSB)(THIS_ BOOL) PURE;
        STDMETHOD(TranslateAcceleratorSB)(THIS_ LPMSG, WORD) PURE;
        STDMETHOD(BrowseObject)(THIS_ LPCITEMIDLIST, UINT) PURE;
        STDMETHOD(GetViewStateStream)(THIS_ DWORD, LPSTREAM*) PURE;
        STDMETHOD(GetControlWindow)(THIS_ UINT, HWND*) PURE;
        STDMETHOD(SendControlMsg)(THIS_ UINT, UINT, WPARAM, LPARAM, LRESULT*) PURE;
        STDMETHOD(QueryActiveShellView)(THIS_ LPSHELLVIEW*) PURE;
        STDMETHOD(OnViewWindowActive)(THIS_ LPSHELLVIEW) PURE;
        STDMETHOD(SetToolbarItems)(THIS_ LPTBBUTTON, UINT, UINT) PURE;
    };
#undef INTERFACE

#ifdef COBJMACROS
#    define IShellBrowser_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IShellBrowser_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IShellBrowser_Release(T) (T)->lpVtbl->Release(T)
#    define IShellBrowser_GetWindow(T, a) (T)->lpVtbl->GetWindow(T, a)
#    define IShellBrowser_ContextSensitiveHelp(T, a) (T)->lpVtbl->ContextSensitiveHelp(T, a)
#    define IShellBrowser_InsertMenusSB(T, a, b) (T)->lpVtbl->InsertMenusSB(T, a, b)
#    define IShellBrowser_SetMenuSB(T, a, b, c) (T)->lpVtbl->SetMenuSB(T, a, b, c)
#    define IShellBrowser_RemoveMenusSB(T, a) (T)->lpVtbl->RemoveMenusSB(T, a)
#    define IShellBrowser_SetStatusTextSB(T, a) (T)->lpVtbl->SetStatusTextSB(T, a)
#    define IShellBrowser_EnableModelessSB(T, a) (T)->lpVtbl->EnableModelessSB(T, a)
#    define IShellBrowser_TranslateAcceleratorSB(T, a, b) (T)->lpVtbl->TranslateAcceleratorSB(T, a, b)
#    define IShellBrowser_BrowseObject(T, a, b) (T)->lpVtbl->BrowseObject(T, a, b)
#    define IShellBrowser_GetViewStateStream(T, a, b) (T)->lpVtbl->GetViewStateStream(T, a, b)
#    define IShellBrowser_GetControlWindow(T, a, b) (T)->lpVtbl->GetControlWindow(T, a, b)
#    define IShellBrowser_SendControlMsg(T, a, b, c, d, e) (T)->lpVtbl->SendControlMsg(T, a, b, c, d, e)
#    define IShellBrowser_QueryActiveShellView(T, a) (T)->lpVtbl->QueryActiveShellView(T, a)
#    define IShellBrowser_OnViewWindowActive(T, a) (T)->lpVtbl->OnViewWindowActive(T, a)
#    define IShellBrowser_SetToolbarItems(T, a, b, c) (T)->lpVtbl->SetToolbarItems(T, a, b, c)
#endif

#define INTERFACE IShellView
    DECLARE_INTERFACE_(IShellView, IOleWindow)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetWindow)(THIS_ HWND*) PURE;
        STDMETHOD(ContextSensitiveHelp)(THIS_ BOOL) PURE;
        STDMETHOD(TranslateAccelerator)(THIS_ LPMSG) PURE;
#ifdef _FIX_ENABLEMODELESS_CONFLICT
        STDMETHOD(EnableModelessSV)(THIS_ BOOL) PURE;
#else
    STDMETHOD(EnableModeless)(THIS_ BOOL) PURE;
#endif
        STDMETHOD(UIActivate)(THIS_ UINT) PURE;
        STDMETHOD(Refresh)(THIS) PURE;
        STDMETHOD(CreateViewWindow)(THIS_ IShellView*, LPCFOLDERSETTINGS, LPSHELLBROWSER, RECT*, HWND*) PURE;
        STDMETHOD(DestroyViewWindow)(THIS) PURE;
        STDMETHOD(GetCurrentInfo)(THIS_ LPFOLDERSETTINGS) PURE;
        STDMETHOD(AddPropertySheetPages)(THIS_ DWORD, LPFNADDPROPSHEETPAGE, LPARAM) PURE;
        STDMETHOD(SaveViewState)(THIS) PURE;
        STDMETHOD(SelectItem)(THIS_ LPCITEMIDLIST, UINT) PURE;
        STDMETHOD(GetItemObject)(THIS_ UINT, REFIID, PVOID*) PURE;
    };
#undef INTERFACE

#ifdef COBJMACROS
#    define IShellView_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define IShellView_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define IShellView_Release(T) (T)->lpVtbl->Release(T)
#    define IShellView_GetWindow(T, a) (T)->lpVtbl->GetWindow(T, a)
#    define IShellView_ContextSensitiveHelp(T, a) (T)->lpVtbl->ContextSensitiveHelp(T, a)
#    define IShellView_TranslateAccelerator(T, a) (T)->lpVtbl->TranslateAccelerator(T, a)
#    ifdef _FIX_ENABLEMODELESS_CONFLICT
#        define IShellView_EnableModeless(T, a) (T)->lpVtbl->EnableModelessSV(T, a)
#    else
#        define IShellView_EnableModeless(T, a) (T)->lpVtbl->EnableModeless(T, a)
#    endif
#    define IShellView_UIActivate(T, a) (T)->lpVtbl->UIActivate(T, a)
#    define IShellView_Refresh(T) (T)->lpVtbl->Refresh(T)
#    define IShellView_CreateViewWindow(T, a, b, c, d, e) (T)->lpVtbl->CreateViewWindow(T, a, b, c, d, e)
#    define IShellView_DestroyViewWindow(T) (T)->lpVtbl->DestroyViewWindow(T)
#    define IShellView_GetCurrentInfo(T, a) (T)->lpVtbl->GetCurrentInfo(T, a)
#    define IShellView_AddPropertySheetPages(T, a, b, c) (T)->lpVtbl->AddPropertySheetPages(T, a, b, c)
#    define IShellView_SaveViewState(T) (T)->lpVtbl->SaveViewState(T)
#    define IShellView_SelectItem(T, a, b) (T)->lpVtbl->SelectItem(T, a, b)
#    define IShellView_GetItemObject(T, a, b, c) (T)->lpVtbl->GetItemObject(T, a, b, c)
#endif

#define INTERFACE ICommDlgBrowser
    DECLARE_INTERFACE_(ICommDlgBrowser, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(OnDefaultCommand)(THIS_ IShellView*) PURE;
        STDMETHOD(OnStateChange)(THIS_ IShellView*, ULONG) PURE;
        STDMETHOD(IncludeObject)(THIS_ IShellView*, LPCITEMIDLIST) PURE;
    };
#undef INTERFACE
    typedef ICommDlgBrowser* LPCOMMDLGBROWSER;

#ifdef COBJMACROS
#    define ICommDlgBrowser_QueryInterface(T, a, b) (T)->lpVtbl->QueryInterface(T, a, b)
#    define ICommDlgBrowser_AddRef(T) (T)->lpVtbl->AddRef(T)
#    define ICommDlgBrowser_Release(T) (T)->lpVtbl->Release(T)
#    define ICommDlgBrowser_OnDefaultCommand(T, a) (T)->lpVtbl->OnDefaultCommand(T, a)
#    define ICommDlgBrowser_OnStateChange(T, a, b) (T)->lpVtbl->OnStateChange(T, a, b)
#    define ICommDlgBrowser_IncludeObject(T, a, b) (T)->lpVtbl->IncludeObject(T, a, b)
#endif

    typedef GUID SHELLVIEWID;
    typedef struct _SV2CVW2_PARAMS
    {
        DWORD cbSize;
        IShellView* psvPrev;
        FOLDERSETTINGS const* pfs;
        IShellBrowser* psbOwner;
        RECT* prcView;
        SHELLVIEWID const* pvid;
        HWND hwndView;
    } SV2CVW2_PARAMS, *LPSV2CVW2_PARAMS;
#define INTERFACE IShellView2
    DECLARE_INTERFACE_(IShellView2, IShellView)
    {

        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetWindow)(THIS_ HWND*) PURE;
        STDMETHOD(ContextSensitiveHelp)(THIS_ BOOL) PURE;
        STDMETHOD(TranslateAccelerator)(THIS_ LPMSG) PURE;
#ifdef _FIX_ENABLEMODELESS_CONFLICT
        STDMETHOD(EnableModelessSV)(THIS_ BOOL) PURE;
#else
    STDMETHOD(EnableModeless)(THIS_ BOOL) PURE;
#endif
        STDMETHOD(UIActivate)(THIS_ UINT) PURE;
        STDMETHOD(Refresh)(THIS) PURE;
        STDMETHOD(CreateViewWindow)(THIS_ IShellView*, LPCFOLDERSETTINGS, LPSHELLBROWSER, RECT*, HWND*) PURE;
        STDMETHOD(DestroyViewWindow)(THIS) PURE;
        STDMETHOD(GetCurrentInfo)(THIS_ LPFOLDERSETTINGS) PURE;
        STDMETHOD(AddPropertySheetPages)(THIS_ DWORD, LPFNADDPROPSHEETPAGE, LPARAM) PURE;
        STDMETHOD(SaveViewState)(THIS) PURE;
        STDMETHOD(SelectItem)(THIS_ LPCITEMIDLIST, UINT) PURE;
        STDMETHOD(GetItemObject)(THIS_ UINT, REFIID, PVOID*) PURE;
        STDMETHOD(GetView)(THIS_ SHELLVIEWID*, ULONG) PURE;
        STDMETHOD(CreateViewWindow2)(THIS_ LPSV2CVW2_PARAMS) PURE;
    };
#undef INTERFACE

#define INTERFACE IShellExecuteHookA
    DECLARE_INTERFACE_(IShellExecuteHookA, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(Execute)(THIS_ LPSHELLEXECUTEINFOA) PURE;
    };
#undef INTERFACE

#define INTERFACE IShellExecuteHookW
    DECLARE_INTERFACE_(IShellExecuteHookW, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(Execute)(THIS_ LPSHELLEXECUTEINFOW) PURE;
    };
#undef INTERFACE

#define INTERFACE IShellIcon
    DECLARE_INTERFACE_(IShellIcon, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetIconOf)(THIS_ LPCITEMIDLIST, UINT, PINT) PURE;
    };
#undef INTERFACE
    typedef IShellIcon* LPSHELLICON;

    typedef struct
    {
        BOOL fShowAllObjects : 1;
        BOOL fShowExtensions : 1;
        BOOL fNoConfirmRecycle : 1;
        BOOL fShowSysFiles : 1;
        BOOL fShowCompColor : 1;
        BOOL fDoubleClickInWebView : 1;
        BOOL fDesktopHTML : 1;
        BOOL fWin95Classic : 1;
        BOOL fDontPrettyPath : 1;
        BOOL fShowAttribCol : 1;
        BOOL fMapNetDrvBtn : 1;
        BOOL fShowInfoTip : 1;
        BOOL fHideIcons : 1;
        UINT fRestFlags : 3;
    } SHELLFLAGSTATE, *LPSHELLFLAGSTATE;

#define SSF_SHOWALLOBJECTS 0x1
#define SSF_SHOWEXTENSIONS 0x2
#define SSF_SHOWCOMPCOLOR 0x8
#define SSF_SHOWSYSFILES 0x20
#define SSF_DOUBLECLICKINWEBVIEW 0x80
#define SSF_SHOWATTRIBCOL 0x100
#define SSF_DESKTOPHTML 0x200
#define SSF_WIN95CLASSIC 0x400
#define SSF_DONTPRETTYPATH 0x800
#define SSF_MAPNETDRVBUTTON 0x1000
#define SSF_SHOWINFOTIP 0x2000
#define SSF_HIDEICONS 0x4000
#define SSF_NOCONFIRMRECYCLE 0x8000

#define INTERFACE IShellIconOverlayIdentifier
    DECLARE_INTERFACE_(IShellIconOverlayIdentifier, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(IsMemberOf)(THIS_ LPCWSTR, DWORD) PURE;
        STDMETHOD(GetOverlayInfo)(THIS_ LPWSTR, int, int*, DWORD*) PURE;
        STDMETHOD(GetPriority)(THIS_ int*) PURE;
    };
#undef INTERFACE

#if (_WIN32_WINNT >= 0x0501) /* WXP */
    typedef _COM_interface IFolderView* LPFOLDERVIEW;

#    define INTERFACE IFolderView
    DECLARE_INTERFACE_(IFolderView, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(GetAutoArrange)(THIS) PURE;
        STDMETHOD(GetCurrentViewMode)(THIS_ UINT) PURE;
        STDMETHOD(GetDefaultSpacing)(THIS_ POINT*) PURE;
        STDMETHOD(GetFocusedItem)(THIS_ int*) PURE;
        STDMETHOD(GetFolder)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD(GetItemPosition)(THIS_ LPCITEMIDLIST, POINT*) PURE;
        STDMETHOD(GetSelectionMarkedItem)(THIS_ int*) PURE;
        STDMETHOD(GetSpacing)(THIS_ POINT*) PURE;
        STDMETHOD(Item)(THIS_ int, LPITEMIDLIST*) PURE;
        STDMETHOD(ItemCount)(THIS_ UINT, int*) PURE;
        STDMETHOD(Items)(THIS_ UINT, REFIID, PVOID*) PURE;
        STDMETHOD(SelectAndPositionItems)(THIS_ UINT, LPCITEMIDLIST*, POINT*, DWORD) PURE;
        STDMETHOD(SelectItem)(THIS_ int, DWORD) PURE;
        STDMETHOD(SetCurrentViewMode)(THIS_ UINT) PURE;
    };
#    undef INTERFACE
#endif /* _WIN32_WINNT >= 0x0501 */

#define ISIOI_ICONFILE 0x00000001
#define ISIOI_ICONINDEX 0x00000002

#if (_WIN32_WINNT >= 0x0500) /* W2K */
    typedef struct
    {
        BOOL fShowAllObjects : 1;
        BOOL fShowExtensions : 1;
        BOOL fNoConfirmRecycle : 1;
        BOOL fShowSysFiles : 1;
        BOOL fShowCompColor : 1;
        BOOL fDoubleClickInWebView : 1;
        BOOL fDesktopHTML : 1;
        BOOL fWin95Classic : 1;
        BOOL fDontPrettyPath : 1;
        BOOL fShowAttribCol : 1;
        BOOL fMapNetDrvBtn : 1;
        BOOL fShowInfoTip : 1;
        BOOL fHideIcons : 1;
        BOOL fWebView : 1;
        BOOL fFilter : 1;
        BOOL fShowSuperHidden : 1;
        BOOL fNoNetCrawling : 1;
        DWORD dwWin95Unused;
        UINT uWin95Unused;
        LONG lParamSort;
        int iSortDirection;
        UINT version;
        UINT uNotUsed;
        BOOL fSepProcess : 1;
        BOOL fStartPanelOn : 1;
        BOOL fShowStartPage : 1;
        UINT fSpareFlags : 13;
    } SHELLSTATE, *LPSHELLSTATE;
#endif /* _WIN32_WINNT >= 0x0500 */

#if (_WIN32_IE >= 0x0500)
#    pragma pack(push, 8)
    typedef struct
    {
        SIZE sizeDragImage;
        POINT ptOffset;
        HBITMAP hbmpDragImage;
        COLORREF crColorKey;
    } SHDRAGIMAGE, *LPSHDRAGIMAGE;
#    pragma pack(pop)

#    define INTERFACE IDragSourceHelper
    DECLARE_INTERFACE_(IDragSourceHelper, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppv) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(InitializeFromBitmap)(THIS_ LPSHDRAGIMAGE pshdi, IDataObject * pDataObject) PURE;
        STDMETHOD(InitializeFromWindow)(THIS_ HWND hwnd, POINT * ppt, IDataObject * pDataObject) PURE;
    };
#    undef INTERFACE

#    define INTERFACE IDropTargetHelper
    DECLARE_INTERFACE_(IDropTargetHelper, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppv) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;
        STDMETHOD(DragEnter)(THIS_ HWND hwndTarget, IDataObject * pDataObject, POINT * ppt, DWORD dwEffect) PURE;
        STDMETHOD(DragLeave)(THIS) PURE;
        STDMETHOD(DragOver)(THIS_ POINT * ppt, DWORD dwEffect) PURE;
        STDMETHOD(Drop)(THIS_ IDataObject * pDataObject, POINT * ppt, DWORD dwEffect) PURE;
        STDMETHOD(Show)(THIS_ BOOL fShow) PURE;
    };
#    undef INTERFACE
#endif /* _WIN32_IE >= 0x0500 */

#if (_WIN32_WINNT >= 0x0500)
    BOOL WINAPI PathResolve(LPWSTR, LPCWSTR*, UINT);
#    define PRF_VERIFYEXISTS 0x0001
#    define PRF_TRYPROGRAMEXTENSIONS (0x0002 | PRF_VERIFYEXISTS)
#    define PRF_FIRSTDIRDEF 0x0004
#    define PRF_DONTFINDLNK 0x0008
#    define IDO_SHGIOI_SHARE 0x0FFFFFFF
#    define IDO_SHGIOI_LINK 0x0FFFFFFE
#    define IDO_SHGIOI_SLOWFILE 0x0FFFFFFD
#    define IDO_SHGIOI_DEFAULT 0x0FFFFFFC
#endif

    void WINAPI SHAddToRecentDocs(UINT, PCVOID);
    LPITEMIDLIST WINAPI SHBrowseForFolderA(PBROWSEINFOA);
    LPITEMIDLIST WINAPI SHBrowseForFolderW(PBROWSEINFOW);
    void WINAPI SHChangeNotify(LONG, UINT, PCVOID, PCVOID);
    HRESULT WINAPI SHGetDataFromIDListA(LPSHELLFOLDER, LPCITEMIDLIST, int, PVOID, int);
    HRESULT WINAPI SHGetDataFromIDListW(LPSHELLFOLDER, LPCITEMIDLIST, int, PVOID, int);
    HRESULT WINAPI SHGetDesktopFolder(LPSHELLFOLDER*);
    HRESULT WINAPI SHGetInstanceExplorer(IUnknown**);
    HRESULT WINAPI SHGetMalloc(LPMALLOC*);
    BOOL WINAPI SHGetPathFromIDListA(LPCITEMIDLIST, LPSTR);
    BOOL WINAPI SHGetPathFromIDListW(LPCITEMIDLIST, LPWSTR);
    HRESULT WINAPI SHGetSpecialFolderLocation(HWND, int, LPITEMIDLIST*);
    HRESULT WINAPI SHLoadInProc(REFCLSID);
#if (_WIN32_IE >= 0x0400)
    BOOL WINAPI SHGetSpecialFolderPathA(HWND, LPSTR, int, BOOL);
    BOOL WINAPI SHGetSpecialFolderPathW(HWND, LPWSTR, int, BOOL);
#endif
    /* SHGetFolderPath in shfolder.dll on W9x, NT4, also in shell32.dll on W2K */
    HRESULT WINAPI SHGetFolderPathA(HWND, int, HANDLE, DWORD, LPSTR);
    HRESULT WINAPI SHGetFolderPathW(HWND, int, HANDLE, DWORD, LPWSTR);
#if (_WIN32_WINDOWS >= 0x0490) || (_WIN32_WINNT >= 0x0500) /* ME or W2K */
    HRESULT WINAPI SHGetFolderLocation(HWND, int, HANDLE, DWORD, LPITEMIDLIST*);
#endif
#if (_WIN32_WINNT >= 0x0500)
    INT WINAPI SHGetIconOverlayIndexW(LPCWSTR pszIconPath, int iIconIndex);
    INT WINAPI SHGetIconOverlayIndexA(LPCSTR pszIconPath, int iIconIndex);
    INT WINAPI SHCreateDirectoryExA(HWND, LPCSTR, LPSECURITY_ATTRIBUTES);
    INT WINAPI SHCreateDirectoryExW(HWND, LPCWSTR, LPSECURITY_ATTRIBUTES);
    HRESULT WINAPI SHBindToParent(LPCITEMIDLIST, REFIID, VOID**, LPCITEMIDLIST*);
#endif
#if (_WIN32_WINNT >= 0x0501) /* XP */
    HRESULT WINAPI SHGetFolderPathAndSubDirA(HWND, int, HANDLE, DWORD, LPCSTR, LPSTR);
    HRESULT WINAPI SHGetFolderPathAndSubDirW(HWND, int, HANDLE, DWORD, LPCWSTR, LPWSTR);
    HRESULT WINAPI SHParseDisplayName(LPCWSTR, IBindCtx*, LPITEMIDLIST, SFGAOF, SFGAOF*);
#endif
    void WINAPI SHGetSettings(LPSHELLFLAGSTATE, DWORD);
#if (_WIN32_WINNT >= 0x0500) /* W2K */
    void WINAPI SHGetSetSettings(LPSHELLSTATE, DWORD, BOOL);
#endif

#if (_WIN32_WINNT >= 0x0500) /* W2K */
    BOOL WINAPI ILIsEqual(LPCITEMIDLIST, LPCITEMIDLIST);
    BOOL WINAPI ILIsParent(LPCITEMIDLIST, LPCITEMIDLIST, BOOL);
    BOOL WINAPI ILRemoveLastID(LPITEMIDLIST);
    HRESULT WINAPI ILLoadFromStream(IStream*, LPITEMIDLIST*);
    HRESULT WINAPI ILSaveToStream(IStream*, LPCITEMIDLIST);
    LPITEMIDLIST WINAPI ILAppendID(LPITEMIDLIST, LPCSHITEMID, BOOL);
    LPITEMIDLIST WINAPI ILClone(LPCITEMIDLIST);
    LPITEMIDLIST WINAPI ILCloneFirst(LPCITEMIDLIST);
    LPITEMIDLIST WINAPI ILCombine(LPCITEMIDLIST, LPCITEMIDLIST);
    LPITEMIDLIST WINAPI ILFindChild(LPCITEMIDLIST, LPCITEMIDLIST);
    LPITEMIDLIST WINAPI ILFindLastID(LPCITEMIDLIST);
    LPITEMIDLIST WINAPI ILGetNext(LPCITEMIDLIST);
    UINT WINAPI ILGetSize(LPCITEMIDLIST);
    void WINAPI ILFree(LPITEMIDLIST);

    HRESULT WINAPI SHCoCreateInstance(LPCWSTR, REFCLSID, IUnknown*, REFIID, void**);
#endif

#ifdef UNICODE
    typedef IShellExecuteHookW IShellExecuteHook;
    typedef IShellLinkW IShellLink;
    typedef BROWSEINFOW BROWSEINFO, *PBROWSEINFO, *LPBROWSEINFO;
#    define SHBrowseForFolder SHBrowseForFolderW
#    define SHGetDataFromIDList SHGetDataFromIDListW
#    define SHGetPathFromIDList SHGetPathFromIDListW
#    if (_WIN32_IE >= 0x0400)
#        define SHGetSpecialFolderPath SHGetSpecialFolderPathW
#    endif
#    define SHGetFolderPath SHGetFolderPathW
#    if (_WIN32_WINNT >= 0x0500)
#        define SHGetIconOverlayIndex SHGetIconOverlayIndexW
#        define SHCreateDirectoryEx SHCreateDirectoryExW
#    endif
#    if (_WIN32_WINNT >= 0x0501)
#        define SHGetFolderPathAndSubDir SHGetFolderPathAndSubDirW
#    endif
#    define FILEDESCRIPTOR FILEDESCRIPTORW
#    define LPFILEDESCRIPTOR LPFILEDESCRIPTORW
#    define FILEGROUPDESCRIPTOR FILEGROUPDESCRIPTORW
#    define LPFILEGROUPDESCRIPTOR LPFILEGROUPDESCRIPTORW

#else
typedef IShellExecuteHookA IShellExecuteHook;
typedef IShellLinkA IShellLink;
typedef BROWSEINFOA BROWSEINFO, *PBROWSEINFO, *LPBROWSEINFO;
#    define SHBrowseForFolder SHBrowseForFolderA
#    define SHGetDataFromIDList SHGetDataFromIDListA
#    define SHGetPathFromIDList SHGetPathFromIDListA
#    if (_WIN32_IE >= 0x0400)
#        define SHGetSpecialFolderPath SHGetSpecialFolderPathA
#    endif
#    define SHGetFolderPath SHGetFolderPathA
#    if (_WIN32_WINNT >= 0x0500)
#        define SHGetIconOverlayIndex SHGetIconOverlayIndexA
#        define SHCreateDirectoryEx SHCreateDirectoryExA
#    endif
#    if (_WIN32_WINNT >= 0x0501)
#        define SHGetFolderPathAndSubDir SHGetFolderPathAndSubDirA
#    endif
#    define FILEDESCRIPTOR FILEDESCRIPTORA
#    define LPFILEDESCRIPTOR LPFILEDESCRIPTORA
#    define FILEGROUPDESCRIPTOR FILEGROUPDESCRIPTORA
#    define LPFILEGROUPDESCRIPTOR LPFILEGROUPDESCRIPTORA
#endif /* UNICODE */

    DWORD WINAPI SHFormatDrive(HWND, UINT, UINT, UINT);

#define SHFMT_ID_DEFAULT 0xFFFF
#define SHFMT_OPT_FULL 1
#define SHFMT_OPT_SYSONLY 2
#define SHFMT_ERROR 0xFFFFFFFF
#define SHFMT_CANCEL 0xFFFFFFFE
#define SHFMT_NOFORMAT 0xFFFFFFFD

#pragma pack(pop)
#ifdef __cplusplus
}
#endif

#endif /* _SHLOBJ_H */
