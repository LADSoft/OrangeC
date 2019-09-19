
/* Protogen Version 3.0.1.6Saturday March 15, 2014  19:13:39 */

                               /* Rcerr.c */

void initerr(void);
void fatal(char *fmt, ...);
int printerr(char *buf, int errnum, void *data);
void basicerror(int n, void *data);
void Error(char *string);
void generror(int n, int data);
void gensymerror(int n, char *data);
void expecttoken(int n);
void generrorexp(int n, int data);
void gensymerrorexp(int n, char *data);
void expecttokenexp(int n);

                               /* Rcgs.c */

BOOL iswhitespacechar(short x) ;
BOOL issymchar(short x) ;
BOOL isstartchar(short x) ;
void initsym(void);
void backup(int st);
int getstring(char *s, int len);
void getnum(void);
LIST *GetCachedLines(void);
int getch(void);
void getid(void);
int getsch(void) ;
int getsym2(void);
void getsym(void);
void skip_comma(void);
void need_eol(void);
void need_begin(void);
void need_end(void);
int needpunc(enum e_sym p);
int needpuncexp(enum e_sym p);
int is_number(void);

                               /* Rcie.c */

EXPRESSION *ReadExp(void); /* PROTOTYPE */
int Eval(EXPRESSION *); /* PROTOTYPE */
void freeExpr(EXPRESSION *r);
EXPRESSION *ReadExpFromString(WCHAR *id);
EXPRESSION *InternalLookup(char *id, int translate);
EXPRESSION *LookupWithTranslation(char *id);
EXPRESSION *ReadExp(void);
EXPRESSION *ReadExpOr(EXPRESSION *p);
int Eval(EXPRESSION *p);
int intexpr(void);

                             /* Rcinterf.c */

void FormatExp(char *buf, EXPRESSION *exp);
void FormatResId(char *buf, IDENT *id);
BOOL ResCheckChanged(RESOURCE *resources);
void ResGetMenuItemName(EXPRESSION *id, char *text);
void ResGetStringItemName(EXPRESSION *id, char *text);
EXPRESSION *ResAllocateMenuId(void);
EXPRESSION *ResAllocateStringId(void);
int ResNextCtlId(CONTROL *ctls);
EXPRESSION *ResAllocateControlId(struct resRes *dlgData, char *sel);
EXPRESSION *ResAllocateResourceId(int type);
EXPRESSION *ResReadExp(struct resRes *data, char *buf);

                               /* Rckw.c */

char *namefromkw(int st);
void kwini(void);
int searchkw(void);

                               /* Rcpp.c */

void filemac(WCHAR *string); /* PROTOTYPE */
void datemac(WCHAR *string); /* PROTOTYPE */
void timemac(WCHAR *string); /* PROTOTYPE */
void linemac(WCHAR *string); /* PROTOTYPE */
void pushif(void); /* PROTOTYPE */
void preprocini(void);
int preprocess(void);
int doerror(void);
int dopragma(void);
int doline(void);
void ReadFileName(void);
int doinclude(int unquoted);
WCHAR *prcStrdup(const WCHAR *string);
void glbdefine(char *name, char *value);
void getdefsym(void);
int dodefine(void);
int doundef(void);
void undef2(void);
int defid(WCHAR *name, WCHAR **p, char *q);
int definsert(WCHAR *end, WCHAR *begin, WCHAR *text, int len, int replen);
int defreplace(WCHAR *macro, int count, WCHAR **oldargs, WCHAR **newargs);
void cnvt(WCHAR *out, char *in);
void filemac(WCHAR *string);
void datemac(WCHAR *string);
void timemac(WCHAR *string);
void linemac(WCHAR *string);
void defmacroreplace(WCHAR *macro, WCHAR *name);
void nodefines(void);
int indefine(SYM *sp);
void enterdefine(SYM *sp);
void exitdefine(void);
int replacesegment(WCHAR *start, WCHAR *end, int *inbuffer, int totallen, int
    *changed);
int defcheck(WCHAR *line);
void pushif(void);
void popif(void);
int doifdef(int flag);
int doif(int flag);
int doelif(void);
int doelse(void);
int doendif(void);

                              /* Rcread.c */

EXPRESSION *ReadExp(void); /* PROTOTYPE */
EXPRESSION *ReadExpOr(EXPRESSION *); /* PROTOTYPE */
int Eval(EXPRESSION *); /* PROTOTYPE */
void *rcAlloc(int v);
char *rcStrdup(const char *s);
void rcFree(void *p);
CURSOR *RCLoadCursor(char *fileName);
int StringAsciiToWChar(WCHAR **text, char *string, int len);
BYTE *RCLoadMessageTable(char *fileName, int *size);
int StrToClass(char *str);
void FreeResources(RESOURCE_DATA *r);
void SetRCMallocScope(RESOURCE_DATA *r);
RESOURCE_DATA *ReadResources(char *fileName);

                               /* Rcsym.c */

void symini(void);
HASHREC *AddHash(HASHREC *item, HASHREC **table, int size);
HASHREC **LookupHash(char *key, HASHREC **table, int size);
SYM *basesearch(char *na, TABLE *table, int checkaccess);
SYM *search(char *na, TABLE *table);
void insert(SYM *sp, TABLE *table);

                              /* Rcutil.c */

FILE *MySearchPath(char *string, char *searchpath, char *mode);
int pstrncmp(const WCHAR *str1, const WCHAR *str2, int n);
int pstrcmp(const WCHAR *str1, const WCHAR *str2);
void pstrcpy(WCHAR *str1, const WCHAR *str2);
void pstrncpy(WCHAR *str1, const WCHAR *str2, int len);
void pstrcat(WCHAR *str1, const WCHAR *str2);
WCHAR *pstrchr(WCHAR *str, WCHAR ch);
WCHAR *pstrrchr(WCHAR *str, WCHAR ch);
int pstrlen(const WCHAR *s);
WCHAR *pstrstr(WCHAR *str1,const WCHAR *str2);
void PopupResFullScreen(HWND hwnd);
void ReleaseResFullScreen(HWND hwnd);

                              /* Rcwrite.c */

void StringWToA(char *a, WCHAR *w, int l);
void WriteResources(char *fileName, RESOURCE_DATA *select);
