#pragma pack(8)

typedef struct tagCCSTYLEA {
    DWORD   flStyle;                    
    DWORD   flExtStyle;                 
    CHAR    szText[CCHCCTEXT];          
    LANGID  lgid;                       
    WORD    wReserved1;                 
} CCSTYLEA, *LPCCSTYLEA;

typedef struct tagCCSTYLEW {
    DWORD   flStyle;                    
    DWORD   flExtStyle;                 
    WCHAR   szText[CCHCCTEXT];          
    LANGID  lgid;                       
    WORD    wReserved1;                 
} CCSTYLEW, *LPCCSTYLEW;

typedef BOOL (CALLBACK* LPFNCCSTYLEA)(HWND hwndParent,  LPCCSTYLEA pccs);
typedef BOOL (CALLBACK* LPFNCCSTYLEW)(HWND hwndParent,  LPCCSTYLEW pccs);


typedef INT (CALLBACK* LPFNCCSIZETOTEXTA)(DWORD flStyle, DWORD flExtStyle,
    HFONT hfont, LPSTR pszText);
typedef INT (CALLBACK* LPFNCCSIZETOTEXTW)(DWORD flStyle, DWORD flExtStyle,
    HFONT hfont, LPWSTR pszText);

typedef struct tagCCSTYLEFLAGA {
    DWORD flStyle;                      
    DWORD flStyleMask;                  
    LPSTR pszStyle;                     
} CCSTYLEFLAGA, *LPCCSTYLEFLAGA;

typedef struct tagCCSTYLEFLAGW {
    DWORD flStyle;                      
    DWORD flStyleMask;                  
    LPWSTR pszStyle;                     
} CCSTYLEFLAGW, *LPCCSTYLEFLAGW;

typedef struct tagCCINFOA {
    CHAR    szClass[CCHCCCLASS];        
    DWORD   flOptions;                  
    CHAR    szDesc[CCHCCDESC];          
    UINT    cxDefault;                  
    UINT    cyDefault;                  
    DWORD   flStyleDefault;             
    DWORD   flExtStyleDefault;          
    DWORD   flCtrlTypeMask;             
    CHAR    szTextDefault[CCHCCTEXT];   
    INT     cStyleFlags;                
    LPCCSTYLEFLAGA aStyleFlags;         
    LPFNCCSTYLEA lpfnStyle;             
    LPFNCCSIZETOTEXTA lpfnSizeToText;   
    DWORD   dwReserved1;                
    DWORD   dwReserved2;                
} CCINFOA, *LPCCINFOA;

typedef struct tagCCINFOW {
    WCHAR   szClass[CCHCCCLASS];
    DWORD   flOptions;
    WCHAR   szDesc[CCHCCDESC];
    UINT    cxDefault;
    UINT    cyDefault;
    DWORD   flStyleDefault;
    DWORD   flExtStyleDefault;
    DWORD   flCtrlTypeMask;
    INT     cStyleFlags;
    LPCCSTYLEFLAGW aStyleFlags;
    WCHAR   szTextDefault[CCHCCTEXT];
    LPFNCCSTYLEW lpfnStyle;
    LPFNCCSIZETOTEXTW lpfnSizeToText;
    DWORD   dwReserved1;
    DWORD   dwReserved2;
} CCINFOW, *LPCCINFOW;

typedef UINT (CALLBACK* LPFNCCINFOA)(LPCCINFOA acci);
typedef UINT (CALLBACK* LPFNCCINFOW)(LPCCINFOW acci);

#ifdef UNICODE
#define LPFNCCSTYLE LPFNCCSTYLEW
#define LPFNCCSIZETOTEXT LPFNCCSIZETOTEXTW
#define CCSTYLE CCSTYLEW
#define LPCCSTYLE LPCCSTYLEW
#define CCSTYLEFLAG CCSTYLEFLAGW
#define LPCCSTYLEFLAG LPCCSTYLEFLAGW
#define CCINFO CCINFOW
#define LPCCINFO LPCCINFOW
#else
#define LPFNCCSTYLE LPFNCCSTYLEA
#define LPFNCCSIZETOTEXT LPFNCCSIZETOTEXTA
#define CCSTYLE CCSTYLEA
#define LPCCSTYLE LPCCSTYLEA
#define CCSTYLEFLAG CCSTYLEFLAGA
#define LPCCSTYLEFLAG LPCCSTYLEFLAGA
#define CCINFO CCINFOA
#define LPCCINFO LPCCINFOA
#endif

#pragma pack()