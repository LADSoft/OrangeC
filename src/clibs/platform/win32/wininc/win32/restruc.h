#pragma pack(4)

typedef enum tagTextMode
{
    TM_PLAINTEXT			= 1,
    TM_RICHTEXT				= 2,
    TM_SINGLELEVELUNDO		= 4,
    TM_MULTILEVELUNDO		= 8,
    TM_SINGLECODEPAGE		= 16,
    TM_MULTICODEPAGE		= 32
} TEXTMODE;

typedef LONG (*EDITWORDBREAKPROCEX)(char *pchText, LONG cchText, BYTE bCharSet, INT action);

typedef struct _charformat
{
    UINT		cbSize;
    DWORD		dwMask;
    DWORD		dwEffects;
    LONG		yHeight;
    LONG		yOffset;
    COLORREF	crTextColor;
    BYTE		bCharSet;
    BYTE		bPitchAndFamily;
    BCHAR		szFaceName[LF_FACESIZE];
} CHARFORMAT;

typedef struct _charrange
{
    LONG	cpMin;
    LONG	cpMax;
} CHARRANGE;

typedef struct _textrange
{
    CHARRANGE chrg;
    LPTSTR lpstrText;
} TEXTRANGE;

typedef DWORD (CALLBACK *EDITSTREAMCALLBACK)(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

typedef struct _editstream
{
    DWORD_PTR dwCookie;
    DWORD	  dwError;
    EDITSTREAMCALLBACK pfnCallback;
} EDITSTREAM;


typedef struct _findtext
{
    CHARRANGE chrg;
    LPCTSTR lpstrText;
} FINDTEXT;


typedef struct _findtextex
{
    CHARRANGE chrg;
    LPCTSTR	  lpstrText;
    CHARRANGE chrgText;
} FINDTEXTEX;


typedef struct _formatrange
{
    HDC hdc;
    HDC hdcTarget;
    RECT rc;
    RECT rcPage;
    CHARRANGE chrg;
} FORMATRANGE;

typedef struct _paraformat
{
    UINT	cbSize;
    DWORD	dwMask;
    WORD	wNumbering;
    WORD	wEffects;
    LONG	dxStartIndent;
    LONG	dxRightIndent;
    LONG	dxOffset;
    WORD	wAlignment;
    SHORT	cTabCount;
    LONG	rgxTabs[MAX_TAB_STOPS];
} PARAFORMAT;

typedef struct _charformat2
{
    UINT		cbSize;
    DWORD		dwMask;
    DWORD		dwEffects;
    LONG		yHeight;
    LONG		yOffset;			
    COLORREF	crTextColor;
    BYTE		bCharSet;
    BYTE		bPitchAndFamily;
    BCHAR		szFaceName[LF_FACESIZE];
    WORD		wWeight;			
    SHORT		sSpacing;			
    COLORREF	crBackColor;		
    LCID		lcid;				
    DWORD		dwReserved;			
    SHORT		sStyle;				
    WORD		wKerning;			
    BYTE		bUnderlineType;		
    BYTE		bAnimation;			
    BYTE		bRevAuthor;			
    BYTE		bReserved1;
} CHARFORMAT2;

#define CHARFORMATDELTA		(sizeof(CHARFORMAT2) - sizeof(CHARFORMAT))


typedef struct _paraformat2
{
    UINT	cbSize;
    DWORD	dwMask;
    WORD	wNumbering;
    WORD	wReserved;
    LONG	dxStartIndent;
    LONG	dxRightIndent;
    LONG	dxOffset;
    WORD	wAlignment;
    SHORT	cTabCount;
    LONG	rgxTabs[MAX_TAB_STOPS];
     LONG	dySpaceBefore;			
    LONG	dySpaceAfter;			
    LONG	dyLineSpacing;			
    SHORT	sStyle;					
    BYTE	bLineSpacingRule;		
    BYTE	bOutlineLevel;			
    WORD	wShadingWeight;			
    WORD	wShadingStyle;			
    WORD	wNumberingStart;		
    WORD	wNumberingStyle;		
    WORD	wNumberingTab;			
    WORD	wBorderSpace;			
    WORD	wBorderWidth;			
    WORD	wBorders;				
} PARAFORMAT2;

typedef struct _msgfilter
{
    NMHDR	nmhdr;
    UINT	msg;
    WPARAM	wParam;
    LPARAM	lParam;
} MSGFILTER;

typedef struct _reqresize
{
    NMHDR nmhdr;
    RECT rc;
} REQRESIZE;

typedef struct _selchange
{
    NMHDR nmhdr;
    CHARRANGE chrg;
    WORD seltyp;
} SELCHANGE;

typedef struct _endropfiles
{
    NMHDR nmhdr;
    HANDLE hDrop;
    LONG cp;
    BOOL fProtected;
} ENDROPFILES;

typedef struct _enprotected
{
    NMHDR nmhdr;
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
    CHARRANGE chrg;
} ENPROTECTED;

typedef struct _ensaveclipboard
{
    NMHDR nmhdr;
    LONG cObjectCount;
    LONG cch;
} ENSAVECLIPBOARD;

typedef struct _enoleopfailed
{
    NMHDR nmhdr;
    LONG iob;
    LONG lOper;
    HRESULT hr;
} ENOLEOPFAILED;

typedef struct _objectpositions
{
    NMHDR nmhdr;
    LONG cObjectCount;
    LONG *pcpPositions;
} OBJECTPOSITIONS;

typedef struct _enlink
{
    NMHDR nmhdr;
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
    CHARRANGE chrg;
} ENLINK;

typedef struct _encorrecttext
{
    NMHDR nmhdr;
    CHARRANGE chrg;
    WORD seltyp;
} ENCORRECTTEXT;

typedef struct _punctuation
{
    UINT	iSize;
    LPSTR	szPunctuation;
} PUNCTUATION;

typedef struct _compcolor
{
    COLORREF crText;
    COLORREF crBackground;
    DWORD dwEffects;
}COMPCOLOR;

typedef struct _repastespecial
{
    DWORD		dwAspect;
    DWORD_PTR	dwParam;
} REPASTESPECIAL;

typedef enum _undonameid
{
    UID_UNKNOWN     = 0,
    UID_TYPING		= 1,
    UID_DELETE 		= 2,
    UID_DRAGDROP	= 3,
    UID_CUT			= 4,
    UID_PASTE		= 5
} UNDONAMEID;

typedef struct _settextex
{
    DWORD	flags;
    UINT	codepage;
} SETTEXTEX;

typedef struct _gettextex
{
    DWORD	cb;
    DWORD	flags;
    UINT	codepage;
    LPCSTR	lpDefaultChar;
    LPBOOL	lpUsedDefChar;
} GETTEXTEX;

typedef struct _gettextlengthex
{
    DWORD	flags;			
    UINT	codepage;		
} GETTEXTLENGTHEX;
    

typedef struct _bidioptions
{
    UINT	cbSize;
    WORD	wMask;
    WORD	wEffects; 
} BIDIOPTIONS;

#pragma pack()
