
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
void stripcomment(short *line);                                   /* STATIC */
int getstring(char *s, int len, FILE *file);
LIST *GetCachedLines(void);
void CacheLine(WCHAR *lptr, char *xbuf);                          /* STATIC */
int getch(void);
void getid(void);
int getsch(void) ;
int radix36(char c);
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

void filemac(short *string); /* PROTOTYPE */
void datemac(short *string); /* PROTOTYPE */
void timemac(short *string); /* PROTOTYPE */
void linemac(short *string); /* PROTOTYPE */
void repdefines(short *lptr); /* PROTOTYPE */                    /* STATIC */
void pushif(void); /* PROTOTYPE */
void preprocini(void);
int preprocess(void);
int doerror(void);
int pragerror(int errno);                                         /* STATIC */
int dopragma(void);
int doline(void);
void ReadFileName(void);
int doinclude(int unquoted);
short *prcStrdup(short *string);
void glbdefine(char *name, char *value);
void getdefsym(void);
int dodefine(void);
int doundef(void);
void undef2(void);
int defid(short *name, short **p, char *q);
int definsert(short *end, short *begin, short *text, int len, int replen);
int defreplace(short *macro, int count, short **oldargs, short **newargs);
void cnvt(short *out, char *in);
void filemac(short *string);
void datemac(short *string);
void timemac(short *string);
void linemac(short *string);
void defmacroreplace(short *macro, short *name);
void nodefines(void);
int indefine(SYM *sp);
void enterdefine(SYM *sp);
void exitdefine(void);
int replacesegment(short *start, short *end, int *inbuffer, int totallen, int
    *changed);
int defcheck(short *line);
void repdefines(short *lptr);                                     /* STATIC */
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
char *rcStrdup(char *s);
void rcFree(void *p);
int GetWord(FILE *fil);                                           /* STATIC */
unsigned int GetLong(FILE *fil);                                  /* STATIC */
int GetData(FILE *fil, BYTE *p, int c);                           /* STATIC */
int CompareType(IDENT *id1, IDENT *id2);                          /* STATIC */
CURSOR *RCLoadCursor(char *fileName);
int StringAsciiToWChar(short **text, char *string, int len);
void IdentFromString(IDENT *val, char *string);                   /* STATIC */
CONTROL *CreateControl(char *text, int id, int x, int y, int
    width, int height, int class , int style, int exstyle);       /* STATIC */
FONT *RCLoadFont(char *fileName);
ICON *RCLoadIcon(char *fileName);
BYTE *RCLoadMessageTable(char *fileName, int *size);
RCDATA *CreateRCDataResourceString(char *string, int len);        /* STATIC */
RCDATA *CreateRCDataResourceNumber(int val, int dword);           /* STATIC */
void ReadResID(IDENT *id);                                        /* STATIC */
void ReadQuotedResID(IDENT *id);                                  /* STATIC */
void ReadMemflags(CHARACTERISTICS *info);                         /* STATIC */
int ReadString(WCHAR **string);                                   /* STATIC */
void ParseExtendedControl(CONTROL *c, int extended);              /* STATIC */
int GetIdentString(void);                                         /* STATIC */
int StrToClass(char *str);
void SetClass(CONTROL *c, int st);                                /* STATIC */
void ParseGenericControl(CONTROL *c, int extended);               /* STATIC */
void ParseStandardControl(CONTROL *c, int class , int style,
    int extended, int text);                                      /* STATIC */
int ParseControl(CONTROL * * * ctl, int extended);                /* STATIC */
void ReadMenuFlags(MENUITEM *m);                                  /* STATIC */
void ReadMenuList(MENUITEM * * * i, int extended);                /* STATIC */
void FreeResources(RESOURCE_DATA *r);
void SetRCMallocScope(RESOURCE_DATA *r);
void SelectControlIdBase(RESOURCE_DATA *select);                  /* STATIC */
void RecurseMenuIdBase(RESOURCE_DATA *select, MENUITEM *items);   /* STATIC */
void SelectMenuIdBase(RESOURCE_DATA *select);                     /* STATIC */
void SelectResourceIdBase(RESOURCE_DATA *select);                 /* STATIC */
void SelectStringIdBase(RESOURCE_DATA *select);                   /* STATIC */
int GetIdVal(SYM *sym);                                           /* STATIC */
SYM * GetIds(RESOURCE_DATA *select, SYM *syms);                   /* STATIC */
RESOURCE_DATA *ReadResources(char *fileName);

                               /* Rcsym.c */

void symini(void);
unsigned int ComputeHash(char *string, int size);                 /* STATIC */
HASHREC *AddHash(HASHREC *item, HASHREC **table, int size);
HASHREC **LookupHash(char *key, HASHREC **table, int size);
SYM *basesearch(char *na, TABLE *table, int checkaccess);
SYM *search(char *na, TABLE *table);
void insert(SYM *sp, TABLE *table);

                              /* Rcutil.c */

char *parsepath(char *path, char *buffer);                        /* STATIC */
FILE *MySearchPath(char *string, char *searchpath, char *mode);
int pstrncmp(short *str1, short *str2, int n);
int pstrcmp(short *str1, short *str2);
void pstrcpy(short *str1, short *str2);
void pstrncpy(short *str1, short *str2, int len);
void pstrcat(short *str1, short *str2);
short *pstrchr(short *str, short ch);
short *pstrrchr(short *str, short ch);
int pstrlen(short *s);
short *pstrstr(short *str1, short *str2);

                              /* Rcwrite.c */

void StringWToA(char *a, WCHAR *w, int l);
FILE * DoOpen(char *name, char *mode);                            /* STATIC */
void WriteCursorFile(RESOURCE *res);                              /* STATIC */
void WriteIconFile(RESOURCE *res);                                /* STATIC */
void WriteBitmapFile(RESOURCE *res);                              /* STATIC */
void WriteMessageTableFile(RESOURCE *res);                        /* STATIC */
void WriteUserDataFile(RESOURCE *res);                            /* STATIC */
void WriteFontFile(RESOURCE *res);                                /* STATIC */
void WriteExpInternal(FILE *outputFile, EXPRESSION *p, int n);    /* STATIC */
void WriteExp(FILE *outputFile, EXPRESSION *exp);                 /* STATIC */
void WriteCommentLines(FILE *outputFile, LIST *lines);            /* STATIC */
void WriteAString(FILE *outputFile, char *string, int len);       /* STATIC */
void WriteWString(FILE *outputFile, WCHAR *str, int len);         /* STATIC */
void WriteQuotedResId(FILE *outputFile, IDENT *id);               /* STATIC */
void WriteResId(FILE *outputFile, IDENT *id);                     /* STATIC */
void WriteFileName(FILE *outputFile, char *rel, char *name);      /* STATIC */
void WriteMemFlags(FILE *outputFile, int flags);                  /* STATIC */
void WriteSecondaryCharacteristics(FILE *outputFile, CHARACTERISTICS *info);/* STATIC */
void WriteMenuFlags(FILE *outputFile, MENUITEM *item);            /* STATIC */
void WriteLanguage(FILE *outputFile, RESOURCE *res);              /* STATIC */
void WriteUserData(FILE *outputFile, char *rel, RESOURCE *res);   /* STATIC */
void WriteCursor(FILE *outputFile, char *rel, RESOURCE *res);     /* STATIC */
void WriteBitmap(FILE *outputFile, char *rel, RESOURCE *res);     /* STATIC */
void WriteIcon(FILE *outputFile, char *rel, RESOURCE *res);       /* STATIC */
void WriteFont(FILE *outputFile, char *rel, RESOURCE *res);       /* STATIC */
void WriteMessageTable(FILE *outputFile, char *rel, RESOURCE *res);/* STATIC */
void WriteDLGInclude(FILE *outputFile, char *rel, RESOURCE *res); /* STATIC */
void WriteString(FILE *outputFile, RESOURCE *res);                /* STATIC */
void WriteAccelerator(FILE *outputFile, RESOURCE *res);           /* STATIC */
void WriteRCData(FILE *outputFile, char *rel, RESOURCE *res);     /* STATIC */
void WriteVersion(FILE *outputFile, RESOURCE *res);               /* STATIC */
void WriteIndent(FILE *outputFile, int indent);                   /* STATIC */
void WritePopup(FILE *outputFile, MENUITEM *items, int extended, int indent);/* STATIC */
void WriteMenu(FILE *outputFile, RESOURCE *res);                  /* STATIC */
void WriteControl(FILE *outputFile, CONTROL *control, int extended);/* STATIC */
void WriteDialogSettings(FILE *outputFile, DIALOG *dlg, CHARACTERISTICS *info);/* STATIC */
void WriteDialog(FILE *outputFile, RESOURCE *res);                /* STATIC */
void WriteRes(FILE *outputFile, char *rel, RESOURCE *res);        /* STATIC */
void MarkDef(RESOURCE_DATA *select, EXPRESSION *id);              /* STATIC */
void MarkAcceleratorDefinitions(RESOURCE_DATA *select, ACCELERATOR *accel);/* STATIC */
void MarkStringDefinitions(RESOURCE_DATA *select, STRINGS *strings);/* STATIC */
void MarkControlDefinitions(RESOURCE_DATA *select, DIALOG *dlg);  /* STATIC */
void MarkMenuDefinitions(RESOURCE_DATA *select, MENUITEM *items); /* STATIC */
void MarkNewDefinitions(RESOURCE_DATA *select);                   /* STATIC */
void WriteDefinitionSet(FILE *outputFile, SYM *definitions, BOOL weed);/* STATIC */
void WriteDefines(FILE *outputFile, RESOURCE_DATA *select);       /* STATIC */
void WriteResources(char *fileName, RESOURCE_DATA *select);
