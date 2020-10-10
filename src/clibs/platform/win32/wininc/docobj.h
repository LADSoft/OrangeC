#ifndef _DOCOBJ_H
#define _DOCOBJ_H
#if __GNUC__ >= 3
#    pragma GCC system_header
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define OLECMDERR_E_UNKNOWNGROUP (-2147221244)
#define OLECMDERR_E_DISABLED (-2147221247)
#define OLECMDERR_E_NOHELP (-2147221246)
#define OLECMDERR_E_CANCELED (-2147221245)
#define OLECMDERR_E_NOTSUPPORTED (-2147221248)

    typedef enum
    {
        OLECMDID_OPEN = 1,
        OLECMDID_NEW = 2,
        OLECMDID_SAVE = 3,
        OLECMDID_SAVEAS = 4,
        OLECMDID_SAVECOPYAS = 5,
        OLECMDID_PRINT = 6,
        OLECMDID_PRINTPREVIEW = 7,
        OLECMDID_PAGESETUP = 8,
        OLECMDID_SPELL = 9,
        OLECMDID_PROPERTIES = 10,
        OLECMDID_CUT = 11,
        OLECMDID_COPY = 12,
        OLECMDID_PASTE = 13,
        OLECMDID_PASTESPECIAL = 14,
        OLECMDID_UNDO = 15,
        OLECMDID_REDO = 16,
        OLECMDID_SELECTALL = 17,
        OLECMDID_CLEARSELECTION = 18,
        OLECMDID_ZOOM = 19,
        OLECMDID_GETZOOMRANGE = 20,
        OLECMDID_UPDATECOMMANDS = 21,
        OLECMDID_REFRESH = 22,
        OLECMDID_STOP = 23,
        OLECMDID_HIDETOOLBARS = 24,
        OLECMDID_SETPROGRESSMAX = 25,
        OLECMDID_SETPROGRESSPOS = 26,
        OLECMDID_SETPROGRESSTEXT = 27,
        OLECMDID_SETTITLE = 28,
        OLECMDID_SETDOWNLOADSTATE = 29,
        OLECMDID_STOPDOWNLOAD = 30
    } OLECMDID;
    typedef enum
    {
        OLECMDF_SUPPORTED = 1,
        OLECMDF_ENABLED = 2,
        OLECMDF_LATCHED = 4,
        OLECMDF_NINCHED = 8
    } OLECMDF;
    typedef enum
    {
        OLECMDEXECOPT_DODEFAULT = 0,
        OLECMDEXECOPT_PROMPTUSER = 1,
        OLECMDEXECOPT_DONTPROMPTUSER = 2,
        OLECMDEXECOPT_SHOWHELP = 3
    } OLECMDEXECOPT;

    typedef struct _tagOLECMDTEXT
    {
        DWORD cmdtextf;
        ULONG cwActual;
        ULONG cwBuf;
        wchar_t rgwz[1];
    } OLECMDTEXT;
    typedef struct _tagOLECMD
    {
        ULONG cmdID;
        DWORD cmdf;
    } OLECMD;

    typedef _COM_interface IOleInPlaceSite* LPOLEINPLACESITE;
    typedef _COM_interface IEnumOleDocumentViews* LPENUMOLEDOCUMENTVIEWS;

    EXTERN_C const IID IID_IContinueCallback;
    EXTERN_C const IID IID_IEnumOleDocumentViews;
    EXTERN_C const IID IID_IPrint;

    EXTERN_C const IID IID_IOleDocumentView;
#define INTERFACE IOleDocumentView
    DECLARE_INTERFACE_(IOleDocumentView, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;

        STDMETHOD(SetInPlaceSite)(THIS_ LPOLEINPLACESITE) PURE;
        STDMETHOD(GetInPlaceSite)(THIS_ LPOLEINPLACESITE*) PURE;
        STDMETHOD(GetDocument)(THIS_ IUnknown**) PURE;
        STDMETHOD(SetRect)(THIS_ LPRECT) PURE;
        STDMETHOD(GetRect)(THIS_ LPRECT) PURE;
        STDMETHOD(SetRectComplex)(THIS_ LPRECT, LPRECT, LPRECT, LPRECT) PURE;
        STDMETHOD(Show)(THIS_ BOOL) PURE;
        STDMETHOD(UIActivate)(THIS_ BOOL) PURE;
        STDMETHOD(Open)(THIS) PURE;
        STDMETHOD(Close)(THIS_ DWORD) PURE;
        STDMETHOD(SaveViewState)(THIS_ IStream*) PURE;
        STDMETHOD(ApplyViewState)(THIS_ IStream*) PURE;
        STDMETHOD(Clone)(THIS_ LPOLEINPLACESITE, IOleDocumentView**) PURE;
    };
#undef INTERFACE
    DECLARE_ENUMERATOR_(IEnumOleDocumentViews, IOleDocumentView);

    EXTERN_C const IID IID_IOleDocument;
#define INTERFACE IOleDocument
    DECLARE_INTERFACE_(IOleDocument, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;

        STDMETHOD(CreateView)(THIS_ LPOLEINPLACESITE, IStream*, DWORD, IOleDocumentView**) PURE;
        STDMETHOD(GetDocMiscStatus)(THIS_ DWORD*) PURE;
        STDMETHOD(EnumViews)(THIS_ LPENUMOLEDOCUMENTVIEWS*, IOleDocumentView**) PURE;
    };
#undef INTERFACE

    EXTERN_C const IID IID_IOleCommandTarget;
#define INTERFACE IOleCommandTarget
    DECLARE_INTERFACE_(IOleCommandTarget, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;

        STDMETHOD(QueryStatus)(THIS_ const GUID*, ULONG, OLECMD*, OLECMDTEXT*) PURE;
        STDMETHOD(Exec)(THIS_ const GUID*, DWORD, DWORD, VARIANTARG*, VARIANTARG*) PURE;
    };
#undef INTERFACE

    EXTERN_C const IID IID_IOleDocumentSite;
#define INTERFACE IOleDocumentSite
    DECLARE_INTERFACE_(IOleDocumentSite, IUnknown)
    {
        STDMETHOD(QueryInterface)(THIS_ REFIID, PVOID*) PURE;
        STDMETHOD_(ULONG, AddRef)(THIS) PURE;
        STDMETHOD_(ULONG, Release)(THIS) PURE;

        STDMETHOD(ActivateMe)(THIS_ IOleDocumentView*) PURE;
    };
#undef INTERFACE

#ifdef __cplusplus
}
#endif
#endif
