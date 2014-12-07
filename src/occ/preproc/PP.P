
/* Protogen Version 1.00 Sunday June 01, 2008  03:22:53 */

                               /* Ccerr.c */

void errorinit(void);
char kwtosym(enum e_kw kw);                                       /* STATIC */
void printerr(int err, char *file, int line, ...);                /* STATIC */
void pperror(int err, int data);
void pperrorstr(int err, char *str);
void preverror(int err, char *name, char *origfile, int origline);
void preverrorsym(int err, SYMBOL *sp, char *origfile, int origline);
void error(int err);
void errorint(int err, int val);
void errorstr(int err, char *val);
void errorsym(int err, SYMBOL *sym);

                              /* Memory.c */

void mem_summary(void);
MEMBLK *galloc(MEMBLK **arena, int size);                         /* STATIC */
void *memAlloc(MEMBLK **arena, int size);
void memFree(MEMBLK **arena, int *peak);
void *globalAlloc(int size);
void globalFree(void);
void *localAlloc(int size);
void localFree(void);
void *Alloc( int size);
void *oAlloc(int size);
void oFree(void);
void *aAlloc(int size);
void aFree(void);
void *tAlloc(int size);
void tFree(void);
void *sAlloc(int size);
void sFree(void);
void IncGlobalFlag(void);
void DecGlobalFlag(void);
void SetGlobalFlag(int flag);
int GetGlobalFlag(void);
char *litlate(char *name);
LCHAR *wlitlate(LCHAR *name);

                              /* Osutil.c */

void fatal(char *fmt, ...);
void banner(char *fmt, ...);
void usage(char *prog_name);
int strcasecmp(char *left, char *right);                          /* STATIC */
void AddExt(char *buffer, char *ext);
void StripExt(char *buffer);
void EXEPath(char *buffer, char *filename);
int HasExt(char *buffer, char *ext);
char *parsepath(char *path, char *buffer);                        /* STATIC */
FILE *SrchPth3(char *string, char *searchpath, char *mode);
FILE *SrchPth2(char *name, char *path, char *attrib);
FILE *SrchPth(char *name, char *path, char *attrib, BOOL sys);
void remove_arg(int pos, int *count, char *list[]);               /* STATIC */
int cmatch(char t1, char t2);                                     /* STATIC */
int scan_args(char *string, int index, char *arg);                /* STATIC */
BOOL parse_args(int *argc, char *argv[], BOOL case_sensitive);
void err_setup(char select, char *string);
void incl_setup(char select, char *string);
void def_setup(char select, char *string);
void undef_setup(char select, char *string);
void output_setup(char select, char *string);
void setglbdefs(void);
void InsertAnyFile(char *filename, char *path, int drive);
void dumperrs(FILE *file); /* PROTOTYPE */
void setfile(char *buf, char *orgbuf, char *ext);
void outputfile(char *buf, char *orgbuf, char *ext);
void scan_env(char *output, char *string);
int parse_arbitrary(char *string);
void parsefile(char select, char *string);
void addinclude(void);
int parseenv(char *name);
int parseconfigfile(char *name);
void dumperrs(FILE *file);
void ctrlchandler(int aa);
void internalError(int a);
void ccinit(int argc, char *argv[]);

                              /* Ppexpr.c */

int getsch(int bytes, unsigned char **source) ;
PPINT ieprimary(void)   ;                                         /* STATIC */
PPINT ieunary(void);                                              /* STATIC */
PPINT iemultops(void);                                            /* STATIC */
PPINT ieaddops(void);                                             /* STATIC */
PPINT ieshiftops(void);                                           /* STATIC */
PPINT ierelation(void);                                           /* STATIC */
PPINT ieequalops(void);                                           /* STATIC */
PPINT ieandop(void);                                              /* STATIC */
PPINT iexorop(void);                                              /* STATIC */
PPINT ieorop(void);                                               /* STATIC */
PPINT ielandop(void);                                             /* STATIC */
PPINT ielorop(void);                                              /* STATIC */
PPINT iecondop(void);                                             /* STATIC */
PPINT iecommaop(void);                                            /* STATIC */
PPINT ppexpr(void);

                              /* Ppmain.c */

char *getUsageText(void);
void bool_setup(char select, char *string); /* PROTOTYPE */
void err_setup(char select, char *string); /* PROTOTYPE */
void incl_setup(char select, char *string); /* PROTOTYPE */
void def_setup(char select, char *string); /* PROTOTYPE */
void codegen_setup(char select, char *string); /* PROTOTYPE */
void optimize_setup(char select, char *string); /* PROTOTYPE */
void warning_setup(char select, char *string); /* PROTOTYPE */
void parsefile(char select, char *string); /* PROTOTYPE */
void output_setup(char select, char *string); /* PROTOTYPE */
void bool_setup(char select, char *string);
int main(int argc, char *argv[]);

                              /* Preproc.c */

void ppdefcheck(char *line); /* PROTOTYPE */
void filemac(char *string); /* PROTOTYPE */
void datemac(char *string); /* PROTOTYPE */
void timemac(char *string); /* PROTOTYPE */
void linemac(char *string); /* PROTOTYPE */
void pushif(void), popif(void); /* PROTOTYPE */
void preprocini(char *name, FILE *fil);
int defid(char *name, unsigned char **p);
void skipspace(void);
BOOL expectid(char *buf);
BOOL expectstring(char *buf, unsigned char **in, BOOL angle);
PPINT expectnum(void);
void lineToCpp(void);                                             /* STATIC */
void stripcomment(unsigned char *line);                                    /* STATIC */
void striptrigraph(unsigned char *buf);                                    /* STATIC */
int getstring(unsigned char *s, int len, FILE *file);                      /* STATIC */
BOOL getline(void);
void preprocess(void);
void doerror(void);
void dowarning(void);
unsigned char *getauxname(unsigned char *ptr, char **bufp);
void pragerror(int error);                                        /* STATIC */
void dopragma(void);
void Compile_Pragma(void);
void doline(void);
INCLUDES *GetIncludeData(void);
void FreeInclData(INCLUDES *data);
void doinclude(void);
void glbdefine(char *name, char *value, BOOL permanent);
void glbUndefine(char *name);
int undef2(char *name);
void dodefine(void);
void doundef(void);
void nodefines(void);
BOOL indefine(DEFSTRUCT *sp);
void enterdefine(DEFSTRUCT *sp);
void exitdefine(void);
int definsert(char *macro, char *end, char *begin, char *text, int len, int replen);
int defstringize(char *macro, char *end, char *begin, char *text, int len);
BOOL defreplaceargs(char *id, char *macro, int count, char **oldargs, char **newargs, char *varargs);
void deftokenizing(char *macro);
char *fullqualify(char *string);
void filemac(char *string);
void datemac(char *string);
void timemac(char *string);
void linemac(char *string);
void defmacroreplace(char *macro, char *name);
int replacesegment(char *start, char *end, int *inbuffer, int totallen, unsigned char **pptr);
void ppdefcheck(unsigned char *line);
int defcheck(unsigned char *line);
void repdefines(unsigned char *lptr);                                      /* STATIC */
void pushif(void);
void popif(void);
void ansieol(void);
void doifdef(BOOL flag);
void doif(void);
void doelif(void);
void doelse(void);
void doendif(void);

                               /* Reerr.c */

void process(char *name);
int main(int argc, char **argv);

                              /* Symtab.c */

void syminit(void);
HASHTABLE *CreateHashTable(int size);
HASHREC **GetHashLink(HASHTABLE *t, char *string);                /* STATIC */
HASHREC *AddName(SYMBOL *item, HASHTABLE *table);                 /* STATIC */
HASHREC **LookupName(char *name, HASHTABLE *table);
SYMBOL *search(char *name, HASHTABLE *table);
SYMBOL *gsearch(char *name);
SYMBOL *tagsearch(char *name);
void insert(SYMBOL *in, HASHTABLE *table);
