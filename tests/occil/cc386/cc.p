
/* Protogen Version 3.0.1.6Friday September 09, 2016  21:44:29 */

                               /* An386.c */

void reserveregs(int *datareg, int *addreg, int *floatreg);
void loadds(void);
void allocate(int datareg, int addreg, int floatreg, SNODE *block);
void loadregs(void);
void asm_scannode(ENODE *node);
void asm_scan1(AMODE *ap);
void asm_scan(OCODE *cd);
void asm_repnode(ENODE **node);
int voidexpr(ENODE *node);
void voidfloat(SNODE *block);
void asm_repcse1(AMODE *ap);
void asm_repcse(OCODE *cd);

                              /* Analyze.c */

int equalnode(ENODE *node1, ENODE *node2);
CSE *searchnode(ENODE *node);
CSE *enternode(ENODE *node, int duse, int size);
CSE *voidauto(ENODE *node);
CSE *voidlref(ENODE *node);
void voidall(void);
void scan_for_inline(ENODE *node);
void scanexpr(ENODE *node, int duse, int size);
void scan(SNODE *block);
void exchange(CSE **c1);
int desire(CSE *csp);
int bsort(CSE **list);
void repexpr(ENODE *node, int size);
void repcse(SNODE *block);
void repvoidassign(ENODE *node, int size);
void voidAssign(SNODE *block, int state);
void allocstack(void);
void opt1(SNODE *block);

                               /* Args.c */

void remove_arg(int pos, int *count, char *list[]);               /* STATIC */
BOOL cmatch(char t1, char t2);                                    /* STATIC */
int scan_args(char *string, int index, char *arg);                /* STATIC */
BOOL parse_args(int *argc, char *argv[], BOOL case_sensitive);

                              /* Browse.c */

void abspath(char *name);
void browse_init(void);
void browse_startfunc(SYM *func, int lineno);
void browse_endfunc(SYM *func, int lineno);
void browse_startfile(char *name);
void browse_variable(SYM *var, int lineno);
void browse_define(char *name, int lineno);
void browse_blockstart(int lineno);
void browse_blockend(int lineno);

                               /* Cmain.c */

void bool_setup(char select, char *string); /* PROTOTYPE */
void err_setup(char select, char *string); /* PROTOTYPE */
void incl_setup(char select, char *string); /* PROTOTYPE */
void def_setup(char select, char *string); /* PROTOTYPE */
void codegen_setup(char select, char *string); /* PROTOTYPE */
void optimize_setup(char select, char *string); /* PROTOTYPE */
void warning_setup(char select, char *string); /* PROTOTYPE */
void parsefile(char select, char *string); /* PROTOTYPE */
void output_setup(char select, char *string); /* PROTOTYPE */
void winmode_setup(char select, char *string); /* PROTOTYPE */
void winmode_setup(char select, char *string);
void bool_setup(char select, char *string);
void codegen_setup(char select, char *string);
void optimize_setup(char select, char *string);
void err_setup(char select, char *string);
void incl_setup(char select, char *string);
void def_setup(char select, char *string);
void output_setup(char select, char *string);
void setglbdefs(void);
void InsertAnyFile(FILE *inf, FILE *outf, char *filename, char *path, int drive);
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
void summary(void);
void ctrlchandler(int aa);
void internalError(int a);
int main(int argc, char *argv[]);

                              /* Conf386.c */

int confcodegen(char s, int bool);
void confsetup(void);
int alignment(int type, TYP *tp);
int getalign(int sc, TYP *tp);
long getautoval(long val);
int funcvaluesize(int size);

                             /* Cppbltin.c */

void installCPPBuiltins(void);

                              /* Dbg386.c */

int debug_outputtype(TYP *type); /* PROTOTYPE */
void emitsym(char *buf); /* PROTOTYPE */                         /* STATIC */
void out_sym_header(void); /* PROTOTYPE */                       /* STATIC */
void dbginit(void);
int emittype(char *buf);                                          /* STATIC */
void emitsym(char *buf);                                          /* STATIC */
int isintrins(TYP *type, int allowptr);                           /* STATIC */
int numeric_leaf(char *buf, unsigned int size);
int debug_outputpointer(TYP *type);
int debug_outputref(TYP *type);
int putname(char *buf, char *name);                               /* STATIC */
int outputFields(SYM *field, int *count);                         /* STATIC */
int debug_outputnamesym(TYP *type, int idx);                      /* STATIC */
int debug_outputstruct(TYP *type);                                /* STATIC */
int enumfields(SYM *field, int *count);                           /* STATIC */
int debug_outputenum(TYP *type);
int debug_outputtypell(TYP *type);
void debug_outputtypedef(SYM *sp);
int debug_outputtype(TYP *type);
void debug_outfunc(SYM *sp);
void debug_beginblock(TABLE lsyms);
void getSymSize(DBGBLOCK *blk);                                   /* STATIC */
void dumpblocks(DBGBLOCK *blk, char *buf, int *ofs, int parent, int
    baseoffs);                                                    /* STATIC */
void dumptypes(DBGBLOCK *blk);
void debug_endblock(TABLE lsyms, TABLE oldsyms);
void debug_outdata(SYM *sp, int BSS);
//void dumphashtable(HASHREC **syms);                               /* STATIC */
void debug_dumpglobaltypedefs(void);
void out_sym_header(void);                                        /* STATIC */

                               /* Decl.c */

void templateWade(void); /* PROTOTYPE */
void declini(void);
int isfunc(TYP *tp);
char *litlate(char *s);
TYP *maketype(int bt, int siz);
TYP *cponetype(TYP *t);
TYP *copytype(TYP *itp, int flags);
SYM *parsetype(SYM *typesp, int pass);
int decl(TABLE *table, int cppflags);
SYM *makesym(enum e_sc sc);
void overloadoperator(enum e_sc sc, SYM *sp);
SYM *decl1(enum e_sc sc, SYM *sp);
int isclassarg(SYM *sp);
void CheckOverloadedArgs(SYM *sp);
ENODE *do_constructor(SYM *sp_in, TYP *tp, TYP *head1, ENODE *node, int
    size, int offset, int assn, int implicit);
ENODE *do_destructor(SYM *sp_in, TYP *tp, TYP *head1, ENODE *xnode, int
    size, int offset, int assn);
ENODE **actualexpr(ENODE **qnode);
int conspair(SYM *sp, enum e_sc al, int flag, TYP *tp1, ENODE *qnode, int
    assn, int offs, int implicit);
void decl2(enum e_sc sc, SYM *sp);
void decl3(enum e_sc sc);
int bitsize(int type);
int oksize(void);
void checkDefaults(SYM *oldf, SYM *newf);
int basedeclare(TABLE *table, int al, long ilc, int ztype, int flags, int
    xcppflags);

                              /* Declass.c */

void defclassini(void);
void SetExtFlag(SYM *sp, int state);
SYM *copysym(SYM *sp);
int copydata(SYM *sym, SYM *from, int mode);                      /* STATIC */
int loadclassdata(SYM *sp, SYM *spc, int mode, int virtual);
	int insertVirtualData(SYM *sp);
char *findname(char *buf);                                        /* STATIC */
int vtabmatch(SYM *candidate, SYM *current);                      /* STATIC */
void addvtabentry(SYM *cl, SYM *sp);
void classerrs(SYM *sp);
ENODE *conscall(ENODE *tcnode, SYM *sp, ENODE *ep);
void setthunkflags(SYM *sp_in, int state);
void curcons(SYM *sp_in, SYM *vtabsp, ENODE **exps, CLASSLIST *l, ENODE
    *thisn, int checkaccess, int ofs);
void prevcons(SYM *sp_in, ENODE **exps, CLASSLIST *l, ENODE *thisn, int
    checkaccess, int ofs);
void embeddedcons(SYM *sp_in, SNODE *snp3, ENODE **exps, ENODE *thisn);
SYM *lookupcopycons(SYM *sp_in, SYM **spr);
void prevcopycons(SYM *sp_in, ENODE *argn, ENODE **exps, CLASSLIST *l,
    ENODE *thisn, int checkaccess, int ofs);
void embeddedcopycons(SYM *sp_in, ENODE *argn, SNODE *snp3, ENODE **exps,
    ENODE *thisn);
void thunkConstructors(SNODE *snp3, SYM *sp_in, int ofs, int checkaccess);
void thunkCopyConstructors(SNODE *snp3, SYM *sp_in, SYM *arg, int ofs, int
    checkaccess);
void thunkDestructors(SNODE *snp3, SYM *sp_in, int ofs, int checkaccess,
    int skipself);
void CreateBaseConstructor(SYM *sp_in);
void classbaseasn(SYM *sp, SYM *func);
void setRefFlags(SYM *sp);
void RefErrs(SYM *sp);
void gen_vtab(SYM *sp);

                               /* Einit.c */

void initassign(ENODE *node, int offset, TYP *tp);                /* STATIC */
ENODE *doeinit(ENODE *node,TYP *tp);
int einittype(TYP *tp);
int einitarray(TYP *tp);
int einitunion(TYP *tp);
int einitstruct(TYP *tp);
ENODE *ecppfloat(void);
ENODE *ecppint(void);
int einitfcomplex(void);
int einitlrcomplex(void);
int einitrcomplex(void);
int einitfloat(void);
int einitlongdouble(void);
int einitdouble(void);
int einitchar(void);
int einitshort(void);
int einituchar(void);
int einitushort(void);
int einitenum(void);
int einitint(void);
int einitlong(void);
int einituint(void);
int einitulong(void);
int einitlonglong(void);
int einitulonglong(void);
int einitmemberptr(TYP *tp, SYM *btypequal);
int einitref(TYP *tp);
void edopointer(int makextern);
int einitpointer(void);
int einitfarpointer(void);
int einitpointerfunc(void);

                               /* Error.c */

void initerr(void);
void warning_setup(char select, char *string);
void diag(char *s);
char *getpunct (int punct);
int printerr(char *buf0, ERRORS *err);
void lferror(void);
void basicskim(int *skimlist);
BALANCE *newbalance(BALANCE *bal);
void setbalance(BALANCE **bal);
void expskim(int *skimlist, int tpl);
void tplskm(int *skmlist);
void basicerror(int n, void *data);
void Error(char *string);
void generror(int n, int data, int *skimlist);
void gensymerror(int n, char *data);
void genfuncerror(int n, char *func, char *data);
void genfunc2error(int n, char *func, char *func2);
void genclasserror(int n, char *name);
void genclass2error(int n, char *struc, char *elem);
void genmismatcherror(TYP *tp1, TYP *tp2);
void doubleerr(int n, char *s1, char *s2);
void expecttoken(int n, int *skimlist);
void generrorexp(int n, int data, int *skimlist);
void gensymerrorexp(int n, char *data);
void expecttokenexp(int n, int *skimlist);

                               /* Expr.c */

void exprini(void);
TYP *basictype(TYP *t);
int retvalsize(TYP *t);
ENODE *makenode(enum e_node nt, void *v1, void *v2);
TYP *deref(ENODE **node, TYP *tp);
int checkstructconst(TYP *tp);
ENODE *sbll(SYM *sp_in, ENODE *en, ENODE *ep1);
ENODE *cbll(ENODE *ep1, ENODE *ep2, TYP *tp);
ENODE *make_callblock(ENODE *ep1, ENODE *ep2, TYP *tp, int size);
ENODE *immed_callblock(ENODE *ep1, ENODE *ep2, TYP *tp, int size);
ENODE *repcallb(ENODE *ep2);
ENODE *dummyvar(int size, TYP *type, char *name);
int isintconst(int type);
int isfloatconst(int type);
int isimaginaryconst(int type);
int iscomplexconst(int type);
int matchreflvalue(ENODE **node, TYP *tp1, TYP *tp2);
ENODE *declRef(SYM *sp_in, TYP *tp1, TYP *tp2, ENODE *ep1, ENODE *ep2, int sc);
SYM *unnamed_search(char *na, SYM *cosym, int *offset);
TYP *nameref(ENODE **node, TYP *tin);
int cppcast(TYP *newtype, TYP *oldtype, ENODE **node, int alwayserr);
void promote_type(TYP *typ, ENODE **node);
TYP *gatherparms(ENODE **node, int isvoid);
int checkparmconst(TYP *tp , TYP *tpi /*left*/);
ENODE *copynode(ENODE *node);
ENODE *copy_enode(ENODE *node);
ENODE *copy_default(ENODE *node);
TYP *FindMemberPointer(TYP *tp1, TYP *tp2, ENODE **ep2);
void parameter_rundown(TABLE *tbl);
int checkexactpointer(TYP *tp1, TYP *tp2);
void parmlist(ENODE **node, TYP *tpi, TYP *tp);
int floatrecurse(ENODE *node);
void floatcheck(ENODE *node);
int castbegin(int st);
int tostring(int *elems);
ENODE *onefunc(ENODE *pnode, TYP **tptr, ENODE *qnode, TYP *tp1);
ENODE *complex_functions(ENODE *pnode, TYP **tptr, ENODE *qnode, TYP *tp1);
TYP *doalloca(ENODE **node);
void newoverload(enum overloadop op, TYP *tp, TYP *decl, ENODE **ep);/* STATIC */
void newcons(ENODE **node, TYP *tp, int array, ENODE *size, TYP *initype,
    ENODE *initnode);                                             /* STATIC */
void newdest(ENODE **node, TYP *tp, int array, ENODE *deadptr);   /* STATIC */
TYP *donew(ENODE **node);
TYP *dodelete(ENODE **node);
TYP *docppcasts(enum e_st st, ENODE **node);
TYP *dotypeid(ENODE **node);
TYP *dooperator(ENODE **node);
void dothrow(ENODE **node);
TYP *primary(ENODE **node);
int isstructured(TYP *tp);
int castvalue(ENODE *node);
int lvalue(ENODE *node);
int pointermode(TYP *tp1, TYP *tp2);
TYP *unary(ENODE **node);
ENODE *makeparam(ENODE *in, ENODE *link, TYP *tp);
TYP *mathoverload(enum e_bt lst, TYP *tp1, TYP *tp2, ENODE **ep1, ENODE
    **ep2);
int absrank(TYP *tp);
TYP *ambigcontext(TYP *tp, int *rv);
TYP *inttype(enum e_bt t1);
TYP *forcefit(ENODE **node1, TYP *tp1, ENODE **node2, TYP *tp2, int allowpointers, int multop);
TYP *asnfit(ENODE **node1, TYP *tp1, ENODE **node2, TYP *tp2, int matchref);
int isscalar(TYP *tp);
int scalarnoncomplex(TYP *tp);
int scalarnonfloat(TYP *tp);
void checknp(TYP *tp1, TYP *tp2, ENODE *ep1, ENODE *ep2);
int isunsignedresult(TYP *tp1, TYP *tp2, ENODE *ep1, ENODE *ep2);
TYP *multops(ENODE **node);
TYP *addops(ENODE **node);
TYP *shiftop(ENODE **node);
TYP *relation(ENODE **node);
TYP *equalops(ENODE **node);
TYP *bitand(ENODE **node);
TYP *bitxor(ENODE **node);
TYP *bitor(ENODE **node);
TYP *andop(ENODE **node);
TYP *orop(ENODE **node);
TYP *conditional(ENODE **node);
TYP *autoasnop(ENODE **node, SYM *sp, int defalt);
void asncombine(ENODE **node);
void checkConstClass(ENODE *node);
TYP *asnop(ENODE **node, TYP *tptr);
TYP *exprnc(ENODE **node);
TYP *commaop(ENODE **node);
TYP *expression(ENODE **node, int tocheck);

                                /* Ext.c */

void AddExt(char *buffer, char *ext);
void StripExt(char *buffer);
void EXEPath(char *buffer, char *filename);
int HasExt(char *buffer, char *ext);

                               /* Fatal.c */

void fatal(char *fmt, ...);

                               /* Float.c */

void diag(char *s); /* PROTOTYPE */
void SetFPFZero(FPF *dest,
                        uchar sign);
void SetFPFInfinity(FPF *dest,
                        uchar sign);
void SetFPFNaN(FPF *dest);
int ValueIsOne(FPF *value);
int FPFEQ(FPF *left, FPF *right);
int FPFGT(FPF *left, FPF *right);
int FPFGTE(FPF *left, FPF *right);
int IsMantissaZero(u16 *mant);
int IsMantissaOne(u16 *mant);
void Add16Bits(u16 *carry,
                u16 *a,
                u16 b,
                u16 c);
void Sub16Bits(u16 *borrow,
                u16 *a,
                u16 b,
                u16 c);
void ShiftMantLeft1(u16 *carry,
                        u16 *mantissa);
void ShiftMantRight1(u16 *carry,
                        u16 *mantissa);
void StickyShiftRightMant(FPF *ptr,
                        int amount);
void normalize(FPF *ptr);
void denormalize(FPF *ptr,
                int minimum_exponent);
void RoundFPF(FPF *ptr);
void choose_nan(FPF *x,
                FPF *y,
                FPF *z,
                int intel_flag);
void AddSubFPF(uchar operation,
                FPF *x,
                FPF *y,
                FPF *z);
void MultiplyFPF(FPF *x,
                        FPF *y,
                        FPF *z);
void DivideFPF(FPF *x, 
                        FPF *y, 
                        FPF *z);
void LongLongToFPF(FPF *dest, LLONG_TYPE myllong);
void UnsignedLongLongToFPF(FPF *dest, LLONG_TYPE myllong);
int FPFTensExponent(FPF *value);
void FPFMultiplyPowTen(FPF *value, int power);
char * FPFToString(char *dest,
                FPF *src);
LLONG_TYPE FPFToLongLong(FPF *src);
int FPFToFloat(unsigned char *dest, FPF *src);
int FPFToDouble(unsigned char *dest, FPF *src);
int FPFToLongDouble(unsigned char *dest, FPF *src);
int LongDoubleToFPF(FPF *dest, unsigned char *src);
void FPFTruncate(FPF *value, int bits, int maxexp, int minexp);

                             /* Floatexp.c */

FPF floatexpr(void); /* PROTOTYPE */
FPF feprimary(void);                                              /* STATIC */
FPF feunary(void);                                                /* STATIC */
FPF femultops(void);                                              /* STATIC */
FPF feaddops(void);                                               /* STATIC */
FPF floatexpr(void);

                              /* Friend.c */

void insertfriend(SYM *decl, SYM *tfriend);
int friendsearch(SYM *spx, char *name, SYM *sp1);
int isfriend(SYM *spx, TABLE *tbl);

                               /* Func.c */

void funcini(void);
void declfuncarg(int isint, int isfar, SYM *sp);
void check_funcused(TABLE *oldlsym, TABLE *lsyms);
void dump_instantiates(void);
void funcbody(SYM *sp);
SYM *makeint(char *name, TABLE *table);
void addrundown(SNODE *snp);
void addblocklist(SYM *sp, SYM *old);
//int scanforgoto(SNODE *block, SNODE *root, DLIST **data, int id);
void block(SNODE *vlainit);
void gather_labels(TABLE *oldlsym, TABLE *lsyms);
SNODE *compoundblock(void);

                              /* Getsym.c */

void initsym(void);
void lineToCpp(void);
int stripcomment(char *line, int begin);                          /* STATIC */
void stripdigraph(short *buf);
void striptrigraph(char *buf);
int getstring(char *s, int len, int file);
int getline(int listflag);
int getch(void);
void getid(void);
int getsch(int bytes) ;
int radix36(int c);
LLONG_TYPE getbase(int b, char **ptr);
void getfrac(int radix, char **ptr, FPF *rval);                   /* STATIC */
int getexp(int radix, char **ptr);                                /* STATIC */
void getnum(void);
int getsym2(void);
void getsym(void);
int needpunc(enum e_sym p, int *skimlist);
int needpuncexp(enum e_sym p, int *skimlist);
void backup(int st);

                             /* Gexpr386.c */

AMODE *gen_expr(ENODE *node, int areg, int sreg, int novalue, int flags, int size); /* PROTOTYPE *//* STATIC */
AMODE *reuseop(AMODE *ap, int areg, int *n); /* PROTOTYPE */
void reuseaxdx(AMODE *ap); /* PROTOTYPE */
AMODE *floatconvpos(void);
int defseg(ENODE *node);
AMODE *truncateFloat(AMODE *ap1, int size);
int chksize(int lsize, int rsize);
AMODE *fstack(void);
AMODE *make_muldivval(AMODE *ap);
void make_floatconst(AMODE *ap);
AMODE *make_label(int lab);
AMODE *makesegreg(int seg);
AMODE *make_immed(long i);
AMODE *make_immedt(long i, int size);
AMODE *make_offset(ENODE *node);
AMODE *make_stack(int number);
void do_ftol(void);
AMODE *complexstore(ENODE *node, AMODE *apr, int novalue, int size);
AMODE *floatstore(ENODE *node, AMODE *apr, int novalue, int size);
void complexload(AMODE *ap, int flag);
void floatload(AMODE *ap, int flag);
int is_memory(AMODE *ap);
AMODE *bit_load(AMODE *ap, int areg, int sreg, ENODE *node);
void bit_store(AMODE *ap2, AMODE *ap1, int areg, int sreg, ENODE *node, int novalue);
AMODE * FloatToInt(int areg, int isize, int osize);
AMODE * FloatToLower(AMODE *ap, int areg, int isize, int osize);
AMODE *IntToLower(AMODE *ap, int areg, int sreg, int isize, int osize, int flags);
AMODE *FloatToHigher(AMODE *ap, int areg, int isize, int osize);
AMODE *QuadToHigher(AMODE *ap, int areg, int isize, int osize);
AMODE *FarPtrToHigher(AMODE *ap, int areg, int isize, int osize);
AMODE *xIntToFloat(AMODE *ap, int areg, int isize, int osize);
AMODE *IntToHigher(AMODE *ap, int areg, int isize, int osize);
AMODE *Normalize(AMODE *ap, int areg, int sreg, int isize, int osize, int flags);
AMODE * do_extend(AMODE *ap, int areg, int sreg, int osize, int flags);
int isshort(ENODE *node);
int isbyte(ENODE *node);
AMODE *indx_data(AMODE *ap, int areg);
AMODE *doindex(ENODE *node, int areg, int sreg, enum e_node type);
AMODE *gen_index(ENODE *node, int areg, int sreg);
AMODE *gen_deref(ENODE *node, int areg, int sreg);
int isbit(ENODE *node);                                           /* STATIC */
AMODE *gen_unary(ENODE *node, int areg, int sreg, int op, int fop);
AMODE *gen_complexadd(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr);
AMODE *gen_fbinary(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr);
int xlvalue(ENODE *node);
AMODE *gen_64bin(ENODE *node, int areg, int sreg, int op, int size);
AMODE *gen_segadd(ENODE *node, int areg, int sreg);
AMODE *gen_xbin(ENODE *node, int areg, int sreg, int op, int op2);
AMODE *gen_binary(ENODE *node, int areg, int sreg, int op, int fopp, int fop, int foppr, int fopr);
void doshift(AMODE *ap1, AMODE *ap2, int op, int div, int size);
AMODE *do64shift(ENODE *node, int areg, int sreg, int novalue, int op, int size, AMODE **apr);
AMODE *gen_shift(ENODE *node, int areg, int sreg, int op, int div);
void push_complex_param(ENODE *node, int areg, int sreg, int ofs, int size);
AMODE *gen_complexdiv(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr);
void divprepare(int op, int size);
void divintoeax(AMODE *ap1, int size, int op, int modflag);
void dodiv(AMODE *ap1, AMODE *ap2, int op, int modflag);
AMODE *do64div(ENODE *node, int areg, int sreg, int op, int modflag, int size, AMODE **apr);
AMODE *gen_modiv(ENODE *node, int areg, int sreg, int op, int fopp, int fop, int foppr, int fopr,
    int modflag);
AMODE *gen_pdiv(ENODE *node, int areg, int sreg);
AMODE *gen_complexmul(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr);
AMODE *do64mul(ENODE *node, int areg, int sreg, int size, AMODE **apr);
AMODE *gen_pmul(ENODE *node, int areg, int sreg);
AMODE *gen_mul(ENODE *node, int areg, int sreg, int op, int fopp, int fop, int foppr, int fopr);
AMODE *gen_hook(ENODE *node, int areg, int sreg);
AMODE *gen_asunary(ENODE *node, int areg, int sreg, int novalue, int op, int fop);
AMODE *gen_asadd(ENODE *node, int areg, int sreg, int novalue, int op, int fopp, int fop, int foppr,
    int fopr);
AMODE *gen_aslogic(ENODE *node, int areg, int sreg, int novalue, int op, int op2);
AMODE *gen_asshift(ENODE *node, int areg, int sreg, int novalue, int op, int div);
AMODE *gen_asmul(ENODE *node, int areg, int sreg, int novalue, int op, int foppr, int fopr, int
    fopp, int fop);
AMODE *gen_asmodiv(ENODE *node, int areg, int sreg, int novalue, int op, int fopp, int fop, int
    foppr, int fopr, int modflag);
void mov1(int reg, AMODE *dst, AMODE *src, int size);
void mov2(int reg, AMODE *dst, AMODE *src, int size1, int size2);
void mov3(int reg, AMODE *dst, AMODE *src, int size1, int size2, int size3);
AMODE *amode_moveblock(AMODE *ap1, AMODE *ap2, int areg, int sreg, int size);
AMODE *gen_moveblock(ENODE *node, int areg, int sreg);
AMODE *amode_clearblock(AMODE *ap1, int size);
AMODE *gen_clearblock(ENODE *node, int areg, int sreg);
AMODE *gen_assign(ENODE *node, int areg, int sreg, int novalue, int stdc);
AMODE *gen_refassign(ENODE *node, int areg, int sreg, int novalue);
AMODE *gen_aincdec(ENODE *node, int areg, int sreg, int novalue, int op);
int pushfloatconst(int size, long double aa);
int push_param(ENODE *ep, int areg, int sreg, int size);
int push_stackblock(ENODE *ep, int areg, int sreg);
int gen_parms(ENODE *plist, int areg, int sreg);
int parmsize(ENODE *plist);
AMODE *gen_tcall(ENODE *node, int areg, int sreg, int novalue);
AMODE *inlinecall(ENODE *node, int areg, int sreg, int novalue);  /* STATIC */
AMODE *gen_fcall(ENODE *node, int areg, int sreg, int novalue);   /* STATIC */
AMODE *gen_repcons(ENODE *node, int areg, int sreg);              /* STATIC */
AMODE *gen_pfcall(ENODE *node, int areg, int sreg, int novalue);  /* STATIC */
AMODE *gen_void(ENODE *node, int areg, int sreg);                 /* STATIC */
int natural_size(ENODE *node);
int defcond(ENODE *node, int areg, int sreg);                     /* STATIC */
AMODE *truerelat(ENODE *node, int areg, int sreg);                /* STATIC */
int complex_relat(ENODE *node);                                   /* STATIC */
AMODE *gen_relat(ENODE *node, int areg, int sreg);
AMODE *gen_compare(ENODE *node, int areg, int sreg, int btype1, int btype2, int btype3, int btype4,
    int fbtype1, int fbtype2, int label, int truecc);
void truejp(ENODE *node, int areg, int sreg, int label);
void falsejp(ENODE *node, int areg, int sreg, int label);
AMODE *gen_expr(ENODE *node, int areg, int sreg, int novalue, int flags, int size);/* STATIC */
AMODE *gen_expr_external(ENODE *node, int novalue, int flags, int sz);
void gen_void_external(ENODE *node);

                             /* Gstmt386.c */

AMODE *makedreg(int r);
AMODE *makefreg(int r);
AMODE *make_direct(int i);
AMODE *make_strlab(char *s);
void genwhile(SNODE *stmt);
void gen_for(SNODE *stmt);
void genif(SNODE *stmt);
void gendo(SNODE *stmt);
AMODE *set_symbol(char *name, int flag);
void call_library(char *lib_name);
void count_cases(SNODE *stmt, struct cases *cs);
void gather_cases(SNODE *stmt, struct cases *cs, int compact);
int analyzeswitch(SNODE *stmt, struct cases *cs);
void bingen(int lower, int avg, int higher, AMODE *ap1, struct cases *cs, int size);
int sortcmpsigned(const void *one, const void *two);
int sortcmpunsigned(const void *one, const void *two);
void genbinaryswitch(SNODE *stmt, struct cases *cs);
void gencompactswitch(SNODE *stmt, struct cases *cs);
void gensimpleswitch(SNODE *stmt, struct cases *cs);
void genxswitch(SNODE *stmt);
void AddProfilerData(void);
void SubProfilerData(void);
OCODE *ThunkAddPeep(OCODE *after, int op, AMODE *ap1, AMODE *ap2);
void ThunkUncommittedStack(void);
AMODE *genreturn(SNODE *stmt, int flag);
void gen_tryblock(void *val);
void genstmt(SNODE *stmt);
void scppinit(void);
SYM *gen_mp_virtual_thunk(SYM *vsp);
SYM *gen_vsn_virtual_thunk(SYM *func, int ofs);
void genfunc(SNODE *stmt);

                             /* Inasm386.c */

void inasmini(void);
void asm_err(int errnum);                                         /* STATIC */
void allocReg(ASMREG *regimage);                                  /* STATIC */
ENODE *asm_ident(void);                                           /* STATIC */
ENODE *asm_label(void);                                           /* STATIC */
int asm_getsize(void);                                            /* STATIC */
int getscale(int *scale);                                         /* STATIC */
int asm_enterauto(ENODE *node, int *reg1, int *reg2);
int asm_structsize(void);                                         /* STATIC */
AMODE *asm_mem(void);                                             /* STATIC */
AMODE *asm_amode(int nosegreg);                                   /* STATIC */
AMODE *asm_immed(void);                                           /* STATIC */
int isrm(AMODE *ap, int dreg_allowed);
AMODE *getimmed(void);
enum e_op asm_op(void);
OCODE *make_ocode(AMODE *ap1, AMODE *ap2, AMODE *ap3);            /* STATIC */
OCODE *ope_math(void);                                            /* STATIC */
OCODE *ope_arpl(void);                                            /* STATIC */
OCODE *ope_bound(void);                                           /* STATIC */
OCODE *ope_bitscan(void);                                         /* STATIC */
OCODE *ope_bit(void);                                             /* STATIC */
OCODE *ope_call(void);                                            /* STATIC */
OCODE *ope_incdec(void);                                          /* STATIC */
OCODE *ope_rm(void);                                              /* STATIC */
OCODE *ope_enter(void);                                           /* STATIC */
OCODE *ope_imul(void);                                            /* STATIC */
OCODE *ope_in(void);                                              /* STATIC */
OCODE *ope_imm8(void);                                            /* STATIC */
OCODE *ope_relbra(void);                                          /* STATIC */
OCODE *ope_relbr8(void);                                          /* STATIC */
OCODE *ope_jmp(void);                                             /* STATIC */
OCODE *ope_regrm(void);                                           /* STATIC */
OCODE *ope_loadseg(void);                                         /* STATIC */
OCODE *ope_lgdt(void);                                            /* STATIC */
OCODE *ope_lidt(void);                                            /* STATIC */
OCODE *ope_rm16(void);                                            /* STATIC */
OCODE *ope_mov(void);                                             /* STATIC */
OCODE *ope_movsx(void);                                           /* STATIC */
OCODE *ope_out(void);                                             /* STATIC */
OCODE *ope_pushpop(void);                                         /* STATIC */
OCODE *ope_shift(void);                                           /* STATIC */
OCODE *ope_ret(void);                                             /* STATIC */
OCODE *ope_set(void);                                             /* STATIC */
OCODE *ope_shld(void);                                            /* STATIC */
OCODE *ope_test(void);                                            /* STATIC */
OCODE *ope_xchg(void);                                            /* STATIC */
OCODE *ope_fmath(void);                                           /* STATIC */
OCODE *ope_fmathp(void);                                          /* STATIC */
OCODE *ope_fmathi(void);                                          /* STATIC */
OCODE *ope_fcom(void);                                            /* STATIC */
OCODE *ope_freg(void);                                            /* STATIC */
OCODE *ope_ficom(void);                                           /* STATIC */
OCODE *ope_fild(void);                                            /* STATIC */
OCODE *ope_fist(void);                                            /* STATIC */
OCODE *ope_fld(void);                                             /* STATIC */
OCODE *ope_fst(void);                                             /* STATIC */
OCODE *ope_fstp(void);                                            /* STATIC */
OCODE *ope_fucom(void);                                           /* STATIC */
OCODE *ope_fxch(void);                                            /* STATIC */
OCODE *ope_mn(void);                                              /* STATIC */
OCODE *ope_m16(void);                                             /* STATIC */
OCODE *ope_cmps(void);                                            /* STATIC */
OCODE *ope_ins(void);                                             /* STATIC */
OCODE *ope_lods(void);                                            /* STATIC */
OCODE *ope_movs(void);                                            /* STATIC */
OCODE *ope_outs(void);                                            /* STATIC */
OCODE *ope_scas(void);                                            /* STATIC */
OCODE *ope_xlat(void);                                            /* STATIC */
OCODE *ope_reg32(void);                                           /* STATIC */
OCODE *ope_stos(void);                                            /* STATIC */
SNODE *asm_statement(int shortfin);

                               /* Init.c */

void insertSpace(SYM *sp, int *bytes); /* PROTOTYPE */
long gen(SYM *sp); /* PROTOTYPE */                               /* STATIC */
long gen2(SYM *sp); /* PROTOTYPE */                              /* STATIC */
void initini(void);
void initrundown(void);
long gen2(SYM *sp);                                               /* STATIC */
long gen(SYM *sp);                                                /* STATIC */
void cppinitinsert(ENODE *node);
void cpprundowninsert(ENODE *node);
void cppinitassign(ENODE *node);
void insert_decl_sp(SYM *sp);
void ReshuffleDeclsp(SYM *sp);
void insertSpace(SYM *sp, int *bytes);
int doinit(SYM *sp);
int inittype(TYP *tp);
int initarray(TYP *tp);
int initunion(TYP *tp);
int initstruct(TYP *tp);
int makelabel(int id);
int makeref(int ispc, SYM *sp, int offset);
int makestorage(long val);
int agflush(int size, LLONG_TYPE val);
int agbits(int size, LLONG_TYPE value);
FPF cppfloat(int size, int imaginary);
int arr(ENODE *ep);
int intconstinit(int type, LLONG_TYPE *val);
long cppint(int size);
int initfcomplex(void);
int initlrcomplex(void);
int initrcomplex(void);
int initfloat(int imaginary);
int initlongdouble(int imaginary);
int initdouble(int imaginary);
int initchar(void);
int initbool(void);
int initshort(void);
int inituchar(void);
int initushort(void);
int initint(void);
int inituint(void);
int initlong(void);
int initulong(void);
int initlonglong(void);
int initulonglong(void);
int initenum(void);
int initmemberptr(TYP *tp, SYM *btypequal);
int initref(TYP *tp);
void dopointer(int makextern);
int initpointer(void);
int initfarpointer(void);
int initpointerfunc(void);
void endinit(void);

                              /* Inline.c */

void inlineinit(void);
SYM *inlineSearch(SYM *name);                                     /* STATIC */
void inlineinsert(SYM *sp);                                       /* STATIC */
LIST *symsToBlock(SYM *sp);                                       /* STATIC */
void insertInlineSyms(SYM *sp);                                   /* STATIC */
SYM *copyInlineFunc(SYM *sp);                                     /* STATIC */
ENODE *inlineexpr(ENODE *node);                                   /* STATIC */
ENODE *asm_inlinestmt(ENODE *e);                                  /* STATIC */
SNODE *inlinestmt(SNODE *block);                                  /* STATIC */
ENODE *inlinefuncargs(ENODE *node, SYM *sp);                      /* STATIC */
void inlinereblock(SYM *sp);                                      /* STATIC */
void redoParameters(SYM *sp);                                     /* STATIC */
ENODE *doinline(ENODE *node);

                              /* Intexpr.c */

LLONG_TYPE intexpr(TYP **tp);

                              /* Intr386.c */

void IntrinsIni(void);
void SearchIntrins(SYM *sp);
int oneparm(ENODE *parms);                                        /* STATIC */
int twoparms(ENODE *parms);                                       /* STATIC */
int threeparms(ENODE *parms);                                     /* STATIC */
AMODE *roll(ENODE *parms, int size, int op);                      /* STATIC */
void LoadCX(ENODE *parms);                                        /* STATIC */
void LoadSIDI(ENODE *parms, int backwards, int savecx);           /* STATIC */
void LoadDI(ENODE *parms, int savecx);                            /* STATIC */
AMODE *popDI(AMODE *ap, int tocx);                                /* STATIC */
AMODE *popSIDI(AMODE *ap, int tocx);                              /* STATIC */
void skipdi(void);                                                /* STATIC */
AMODE *xstrlen(int todec);                                        /* STATIC */
void xmove(void);                                                 /* STATIC */
void xset(void);                                                  /* STATIC */
AMODE *xstrcmp(void);                                             /* STATIC */
AMODE *inport(ENODE *parms, int size);
AMODE *outport(ENODE *parms, int size);
AMODE *HandleIntrins(ENODE *node, int novalue);

                              /* Invoke.c */

void InsertFile(LIST **r, char *name, char *ext);                 /* STATIC */
int InsertExternalFile(char *name);
void InsertOutputFile(char *name);
int RunExternalFiles(void);

                               /* List.c */

char *unmangledname(char *str);                                   /* STATIC */
void put_sc(int scl);
void put_ty(TYP *tp);
void list_var(SYM *sp, int i);
void list_table(TABLE *t, int j);

                              /* Mangle.c */

extern char *cpponearg(char *buf, TYP *tp); /* PROTOTYPE */
char *unmang1(char *buf, char *name, int firsttime); /* PROTOTYPE */
#define IT_SIZE (sizeof(cpp_funcname_tab)/sizeof(char *));
char *cppargs(char *buf, SYM *sp); /* PROTOTYPE */
char *unmangcppfunc(char *buf, char *name, int firsttime); /* PROTOTYPE *//* STATIC */
char *unmangcpptype(char *buf, char *name, int firsttime); /* PROTOTYPE *//* STATIC */
char *cpponearg(char *buf, TYP *tp); /* PROTOTYPE */
char *unmang1(char *buf, char *name, int firsttime); /* PROTOTYPE */
void cpp_unmang_intrins(char **buf, char **name, char *last);
void funcrefinsert(char *name, SYM *refsym, TABLE *tbl, SYM *insp);
int templateNameSearch(TYP *typ1, TYP *typ2);                     /* STATIC */
int matchone(SYM *sp1, SYM *sp2, ENODE *node, int nodefault, int any);
int funciterate(SYM *sp1, TYP *tp, ENODE *node, int nodefault, int any, SYM
    **sp2, SYM **sp3, SYM **sp4);
SYM *funcovermatch2(SYM *tbsym, TYP *tp, ENODE *node, int exact, int nodefault,
    int any);
SYM *funcovermatch(SYM *tbsym, TYP *tp, ENODE *ep, int exact, int nodefault);
SYM *castmatch(SYM *sp, TYP *tp, TABLE *tb);
void mname(char *buf, SYM *sp);                                   /* STATIC */
int lookupname(char *name);
char *putvc(char *buf, TYP *tp);                                  /* STATIC */
char *cppval(char *buf, TYP *tp);
void tplPlaceholder(char *buf, char *nm, LIST *tn);
char *cpponearg(char *buf, TYP *tp);
char *cppargs(char *buf, SYM *sp);
char *cppmangle(char *name, TYP *tp);
char *manglens(char *buf, NAMESPACE *ns);                         /* STATIC */
void mangleclass(char *buf, SYM *sl);
char *fullcppmangle(SYM *class , char *name, TYP *tp);
void unmangdollars(char *buf, char **nm);                         /* STATIC */
char *unmang1(char *buf, char *name, int firsttime);
char *unmangcppfunc(char *buf, char *name, int firsttime);        /* STATIC */
char *unmangcpptype(char *buf, char *name, int firsttime);        /* STATIC */
void xlate_cppname(char **buf, char **name, char *lastname);      /* STATIC */
void unmangle(char *buf, char *name);
char *funcwithns(SYM *sp);

                              /* Memmgt.c */

void release_local(void); /* PROTOTYPE */
void release_global(void); /* PROTOTYPE */
void release_opt(void); /* PROTOTYPE */
void release_oc(void); /* PROTOTYPE */
void mem_summary(void); /* PROTOTYPE */
void memini(void);
char *palloc(int *sizepos, int size, int *indxpos, BLK **blk);    /* STATIC */
char *xalloc(int siz);
char *oalloc(int siz);
char *ocalloc(int siz);
long release(int *sizepos, BLK **blk);                            /* STATIC */
void release_local(void);
void release_global(void);
void release_opt(void);
void release_oc(void);
void mem_summary(void);

                              /* Obj386.c */

void obj_init(void);
int put_ident(char *buf, int ident);
void emit_record(enum omf_type type, char *data, int len);
void omfFileName(void);
void omfTranslatorName(void);
void omfFileTime(char *file);
void omfDebugMarker(void);
void omfLNames(void);
void omfLibMod(void);
void omfSegs(void);
void omfComDefs(void);
void omfGroups(void);
void FlushExtBuf(void);
void omfputext(SYM *sp);
void omfextdumphash(HASHREC **syms);                              /* STATIC */
void omfExtDefs(void);
void omfputimport(SYM *sp);
void omfImports(void);
int omfgetseg(SYM *sp);
void omfputpub(SYM *sp);
void omfpubdumphash(HASHREC **syms);
void omfPublics(void);
void omfputexport(SYM *sp);
void omfExports(void);
void omfPassSeperator(void);
int omfFixups(int seg, EMIT_LIST *rec, char *data, char *buf, int *len);
void omfComDefData(void);
void omfData(void);
void omfSourceFile(char *file, int num);
void omfLineNumbers(int file);
void omfEmitLineInfo(void);
void omfModEnd(void);
void output_obj_file(void);

                             /* Optimize.c */

ULLONG_TYPE reint(ENODE *node); /* PROTOTYPE */
LLONG_TYPE mod_mask(int i); /* PROTOTYPE */
void constoptinit(void);
int optimizerfloatconst(ENODE *en);                               /* STATIC */
int isoptconst(ENODE *en);                                        /* STATIC */
int maxinttype(ENODE *ep1, ENODE *ep2);                           /* STATIC */
int isunsignedexpr(ENODE *ep1);                                   /* STATIC */
int maxfloattype(ENODE *ep1, ENODE *ep2);                         /* STATIC */
int maximaginarytype(ENODE *ep1, ENODE *ep2);                     /* STATIC */
int maxcomplextype(ENODE *ep1, ENODE *ep2);                       /* STATIC */
int hasFloats(ENODE *node);                                       /* STATIC */
int getmode(ENODE *ep1, ENODE *ep2);                              /* STATIC */
ULLONG_TYPE CastToInt(int size, LLONG_TYPE value);
FPF CastToFloat(int size, FPF *value);
FPF *IntToFloat(FPF* temp, int size, LLONG_TYPE value);
FPF refloat(ENODE *node);
ULLONG_TYPE reint(ENODE *node);
void dooper(ENODE **node, int mode);
int pwrof2(LLONG_TYPE i);
LLONG_TYPE mod_mask(int i);
void addaside(ENODE *node);
int opt0(ENODE **node);
void enswap(ENODE **one, ENODE **two);
int fold_const(ENODE *node);
int typedconsts(ENODE *node1);
int depth(ENODE *ep);                                             /* STATIC */
void rebalance(ENODE *ep);                                        /* STATIC */
void opt4(ENODE **expr, int globvar);

                             /* Outas386.c */

void outcodeini(void);
void nl(void);
void outop(char *name);
void putop(enum e_op op, AMODE *aps, AMODE *apd, int nooptx);
char *dumpns(NAMESPACE *ns, char *buf, int *done);
void putsym(char *buf, SYM *sp, char *p);
void putconst(ENODE *offset, int doSign);
void putlen(int l);
void putsizedreg(char *string, int reg, int size);
void pointersize(int size);
void putseg(int seg, int usecolon);
int islabeled(ENODE *n);
void putamode(AMODE *ap);
void put_code(OCODE *cd);
void gen_strlab(SYM *sp);
void put_label(int lab);
void put_staticlabel(long label);
void genfloat(FPF *val);
void gendouble(FPF *val);
void genlongdouble(FPF *val);
int genstring(char *str, int uselong, int len);
void genbyte(long val);
void genbool(int val);
void genword(long val);
void genlong(long val);
void genint(int val);
void genenum(int val);
void genlonglong(LLONG_TYPE val);
void gensrref(SYM *sp, int val);
void genref(SYM *sp, int offset);
void genpcref(SYM *sp, int offset);
void genstorage(int nbytes);
void gen_labref(int n);
void gen_labdifref(int n1, int n2);
int stringlit(char *s, int uselong, int len);
void dumplits(void);
void exitseg(void);
void cseg(void);
void xconstseg(void);
void xstringseg(void);
void dseg(void);
void bssseg(void);
void startupseg(void);
void rundownseg(void);
void gen_virtual(SYM *sp, int data);
void gen_endvirtual(SYM *sp);
void align(int size);
long queue_muldivval(long number);
long queue_floatval(FPF *number, int size);
void dump_muldivval(void);
void dump_browsedata(unsigned char *data, int len);
void asm_header(void);
void globaldef(SYM *sp);
void output_alias(char *name, char *alias);
int put_exdata(SYM *sp, int notyet);
int put_exfunc(SYM *sp, int notyet);
int put_expfunc(SYM *sp, int notyet);
//int dumphashtable(int notyet, HASHREC **syms);                    /* STATIC */
void putexterns(void);

                             /* Outco386.c */

void outcode_file_init(void);
void outcode_func_init(void);
void InsertLabel(int lbl, int address, int seg);
int LabelAddress(int lbl);
int LabelSeg(int lbl);
void InsertLine(int address, int line, int file);
EMIT_TAB *gettab(int seg);
void emit(int seg, BYTE *data, int len);
void write_to_seg(int seg, int offset, char *value, int len);
void gen_symbol_fixup(enum mode xmode, int seg, int address, SYM *pub);
void outcode_dump_browsedata(unsigned char *buf, int len);
void outcode_dumplits(void);
void outcode_genref(SYM *sp, int offset);
void outcode_gen_labref(int n);
void outcode_gen_labdifref(int n1, int n2);
void outcode_gensrref(SYM *sp, int val);
void outcode_genstorage(int len);
void outcode_genfloat(FPF *val);
void outcode_gendouble(FPF *val);
void outcode_genlongdouble(FPF *val);
void outcode_genstring(char *string, int len);
void outcode_genbyte(int val);
void outcode_genword(int val);
void outcode_genlong(int val);
void outcode_genlonglong(LLONG_TYPE val);
void outcode_align(int size);
void outcode_put_label(int lab);
void outcode_start_virtual_seg(SYM *sp, int data);
void outcode_end_virtual_seg(SYM *sp);
ENODE *GetSymRef(ENODE *n);
int resolveoffset(OCODE *ins, ENODE *n, int *resolved);
int asmrm(int reg, OCODE *ins, AMODE *data, BYTE **p);
int asmfrm(int reg, OCODE *ins, AMODE *data, BYTE **p);
int AOP0(OPCODE *descript, OCODE *data, BYTE **p);
int AOP1(OPCODE *descript, OCODE *data, BYTE **p);
int AOP2(OPCODE *descript, OCODE *data, BYTE **p);
int AOP3(OPCODE *descript, OCODE *data, BYTE **p);
int AOP4(OPCODE *descript, OCODE *data, BYTE **p);
int AOP5(OPCODE *descript, OCODE *data, BYTE **p);
int AOP6(OPCODE *descript, OCODE *data, BYTE **p);
int AOP7(OPCODE *descript, OCODE *data, BYTE **p);
int AOP8(OPCODE *descript, OCODE *data, BYTE **p);
int AOP9(OPCODE *descript, OCODE *data, BYTE **p);
int AOP10(OPCODE *descript, OCODE *data, BYTE **p);
int AOP11(OPCODE *descript, OCODE *data, BYTE **p);
int AOP12(OPCODE *descript, OCODE *data, BYTE **p);
int AOP13(OPCODE *descript, OCODE *data, BYTE **p);
int AOP14(OPCODE *descript, OCODE *data, BYTE **p);
int AOP15(OPCODE *descript, OCODE *data, BYTE **p);
int AOP16(OPCODE *descript, OCODE *data, BYTE **p);
int AOP17(OPCODE *descript, OCODE *data, BYTE **p);
int AOP18(OPCODE *descript, OCODE *data, BYTE **p);
int AOP19(OPCODE *descript, OCODE *data, BYTE **p);
int AOP20(OPCODE *descript, OCODE *data, BYTE **p);
int AOP21(OPCODE *descript, OCODE *data, BYTE **p);
int AOP22(OPCODE *descript, OCODE *data, BYTE **p);
int AOP23(OPCODE *descript, OCODE *data, BYTE **p);
int AOP24(OPCODE *descript, OCODE *data, BYTE **p);
int AOP25(OPCODE *descript, OCODE *data, BYTE **p);
int AOP26(OPCODE *descript, OCODE *data, BYTE **p);
int AOP27(OPCODE *descript, OCODE *data, BYTE **p);
int AOP28(OPCODE *descript, OCODE *data, BYTE **p);
int AOP29(OPCODE *descript, OCODE *data, BYTE **p);
int AOP30(OPCODE *descript, OCODE *data, BYTE **p);
int AOP31(OPCODE *descript, OCODE *data, BYTE **p);
int AOP32(OPCODE *descript, OCODE *data, BYTE **p);
int AOP33(OPCODE *descript, OCODE *data, BYTE **p);
int AOP34(OPCODE *descript, OCODE *data, BYTE **p);
int AOP35(OPCODE *descript, OCODE *data, BYTE **p);
int AOP36(OPCODE *descript, OCODE *data, BYTE **p);
int AOP37(OPCODE *descript, OCODE *data, BYTE **p);
int AOP38(OPCODE *descript, OCODE *data, BYTE **p);
int AOP40(OPCODE *descript, OCODE *data, BYTE **p);
int AOP39(OPCODE *descript, OCODE *data, BYTE **p);
int AOP41(OPCODE *descript, OCODE *data, BYTE **p);
int AOP42(OPCODE *descript, OCODE *data, BYTE **p);
int AOP43(OPCODE *descript, OCODE *data, BYTE **p);
int AOP44(OPCODE *descript, OCODE *data, BYTE **p);
int AOP45(OPCODE *descript, OCODE *data, BYTE **p);
int AOP46(OPCODE *descript, OCODE *data, BYTE **p);
int AOP47(OPCODE *descript, OCODE *data, BYTE **p);
int AOP48(OPCODE *descript, OCODE *data, BYTE **p);
int AOP50(OPCODE *descript, OCODE *data, BYTE **p);
int AOP49(OPCODE *descript, OCODE *data, BYTE **p);
int AOP51(OPCODE *descript, OCODE *data, BYTE **p);
int AOP52(OPCODE *descript, OCODE *data, BYTE **p);
int AOP53(OPCODE *descript, OCODE *data, BYTE **p);
int AOP54(OPCODE *descript, OCODE *data, BYTE **p);
int AOP55(OPCODE *descript, OCODE *data, BYTE **p);
int AOP56(OPCODE *descript, OCODE *data, BYTE **p);
int AOP57(OPCODE *descript, OCODE *data, BYTE **p);
int AOP58(OPCODE *descript, OCODE *data, BYTE **p);
int AOP59(OPCODE *descript, OCODE *data, BYTE **p);
int AOP60(OPCODE *descript, OCODE *data, BYTE **p);
int AOP61(OPCODE *descript, OCODE *data, BYTE **p);
int AOP62(OPCODE *descript, OCODE *data, BYTE **p);
int AOP63(OPCODE *descript, OCODE *data, BYTE **p);
int AOP64(OPCODE *descript, OCODE *data, BYTE **p);
int AOP65(OPCODE *descript, OCODE *data, BYTE **p);
int outcode_AssembleIns(OCODE *ins, int address);
void outcode_optimize(OCODE *peeplist);
void outcode_dumpIns(OCODE *peeplist);
void outcode_gen(OCODE *peeplist);

                              /* Output.c */

void oinit(void);
void oflush(FILE *file);
void oputc(int ch, FILE *file);
void owrite(char *buf, size_t size, int n, FILE *fil);
void oprintf(FILE *file, char *format, ...);

                              /* Peep386.c */

void peepini(void);
AMODE *copy_addr(AMODE *ap);
void fixlen(int op, AMODE *ap);                                   /* STATIC */
OCODE *gen_code(int op, AMODE *ap1, AMODE *ap2);
OCODE *gen_code3(int op, AMODE *ap1, AMODE *ap2, AMODE *ap3);
OCODE *gen_codes(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_coden(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_codefs(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_codef(int op, AMODE *ap1, AMODE *ap2);
void gen_codes2(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_code2(int op, int len1, int len2, AMODE *ap1, AMODE *ap2);
void gen_codelab(SYM *lab);
void gen_branch(int op, AMODE *ap1);
void gen_line(SNODE *stmt);
void gen_comment(char *txt);
void add_peep(OCODE *new);
void gen_label(int labno);
void flush_peep(void);
void peep_add(OCODE *ip);
void peep_sub(OCODE *ip);
int makeshl(ENODE *node, int scale, int *rv);
void peep_lea2(OCODE *ip, AMODE *ap);
void peep_lea(OCODE *ip);
void peep_lea3(OCODE *ip);
void peep_signextend(OCODE *ip);
void replacereg(AMODE *ap, int reg, int reg2);
OCODE *peep_test(OCODE *ip);
void peep_cmp(OCODE *ip);
void peep_push(OCODE *ip);
void peep_mov(OCODE *ip);
void peep_and(OCODE *ip);
void peep_or(OCODE *ip);
void peep_neg(OCODE *ip);
void peep_btr(OCODE *ip);
OCODE *peep_jmp(OCODE *ip);
void peep_uctran(OCODE *ip);
void peep_label(OCODE *ip);
int novalue(OCODE *ip);
int equal_address(AMODE *ap1, AMODE *ap2);
int single_bit(int val);
void peep_prefixes(OCODE *ip);
void peep_call(OCODE *ip);
void peep_pop(OCODE *ip);
void remove_peep_entry(OCODE *ip);
void opt3(void);

                              /* Preproc.c */

void filemac(short *string); /* PROTOTYPE */
void datemac(short *string); /* PROTOTYPE */
void dateisomac(short *string); /* PROTOTYPE */
void timemac(short *string); /* PROTOTYPE */
void linemac(short *string); /* PROTOTYPE */
void repdefines(short *lptr); /* PROTOTYPE */                    /* STATIC */
void pushif(void); /* PROTOTYPE */
void preprocini(void);
short *allocateMacroReplaceBuffer(void);
void freeMacroReplaceBuffer(short *buf);
int preprocess(void);
int doerror(int n);
char *getauxname(short *ptr, char **bufp);
int dopragma(void);
void Compile_Pragma(void);
void setalias(SYM *sp);
void dumpstartups(void);
int doline(void);
int srchpth2(char *name, char *path, int attrib);
int srchpth(char *name, char *path, int attrib, int sys);
int doinclude(void);
short *plitlate(short *string);
void glbdefine(char *name, char *value);
int dodefine(void);
int doundef(void);
int undef2(void);
void getdefsym(void);
int defid(short *name, short **p, char *q);
int definsert(short *macro, short *end, short *begin, short *text, short *etext, int len, int replen);
int oddslash(short *start, short *p);                             /* STATIC */
void defstringizing(short *macro);
int defreplaceargs(short *macro, int count, short **oldargs, short **newargs,
                                   short **expandedargs, short *varargs);
void deftokenizing(short *macro);
void cnvt(short *out, char *in);
char *fullqualify(char *string);
void filemac(short *string);
void datemac(short *string);
void dateisomac(short *string);
void timemac(short *string);
void linemac(short *string);
void defmacroreplace(short *macro, char *name);
void SetupAlreadyReplaced(short *macro);
int ppNumber(short *start, short *pos);
int replacesegment(short *start, short *end, int *inbuffer, int totallen, short **pptr);
int defcheck(short *line);
void repdefines(short *lptr);                                     /* STATIC */
void pushif(void);
void popif(void);
void ansieol(void);
int doifdef(int flag);
int doif(int flag);
int doelif(void);
int doelse(void);
int doendif(void);

                               /* Pstr.c */

int pstrncmp(short *str1, short *str2, int n);
int pstrcmp(short *str1, short *str2);
void pstrcpy(short *str1, short *str2);
void pstrncpy(short *str1, short *str2, int len);
void pstrcat(short *str1, short *str2);
short *pstrchr(short *str, short ch);
short *pstrrchr(short *str, short ch);
int pstrlen(short *s);
short *pstrstr(short *str1, short *str2);

                              /* Reg386.c */

void regini(void);
void SetFuncMode(int mode);
void GenPush(int reg, int rmode, int flag);
void GenPop(int reg, int rmode, int flag);
void PushRegs(unsigned mask);
void PopRegs(unsigned mask);
void InitRegs(void);
void ChooseRegs(int *a, int *s);
void MarkRegs(void);
void unpush(void);
int ReleaseWouldPop(void);
void ReleaseRegs(void);
AMODE *temp_axdx(void);
void RealizeSreg(int reg);
AMODE * RealizeDreg(int reg);
void loadaxdx(AMODE *ap);
void reuseaxdx(AMODE *ap);
AMODE *reuseop(AMODE *ap, int areg, int *n);
void FreeUnused(int n);
AMODE *axdx_tolower(int areg);
void FreeReg(int n);
void FreeInd(AMODE *left, AMODE *right);

                               /* Rtti.c */

void xceptinit(void);
void getxtname(char *buf, TYP *tp);                               /* STATIC */
SYM *getxtsym(TYP *tp);
void xceptData(SYM *func, SYM *sp);                               /* STATIC */
void trydata(SYM *func, TRYBLOCK *t);                             /* STATIC */
int genthrowdata(SYM *sp);
void dumpXceptBlock(SYM *sp);
void dumpname(SYM *sp);
void dumpdest(SYM *sp);
void dumpClassData(CLASSLIST *l, int flags);
void dumpSymData(SYM *sp, int flags);
void insertRTTI(SYM *label, TYP *tp);
int registeredBaseRTTI(char *name);
void registerBaseRTTI(SYM *sp);
void dumpClassRTTI(SYM *sp_in, SYM **spo);
void dumpBaseRTTI(TYP *tp, SYM **spo);
int dumpPointerRTTI(TYP *tp, SYM *spbase, SYM **spo);
void dumpRTTI(TYP *tp, SYM **spo);
void dumpRTTIDescript(int label, SYM *destructor, SYM *xceptlst);
void dumpXceptTabs(void);

                             /* Searchkw.c */

void kwini(void);
int searchkw(void);

                             /* Srchpath.c */

char *parsepath(char *path, char *buffer);                        /* STATIC */
int SearchPath(char *string, char *searchpath, int mode);

                               /* Stmt.c */

int selectscalar(TYP *tp, ENODE *ep);
void stmtini(void);
TYP *stmtsize(ENODE **node, TYP *tp);                             /* STATIC */
TYP *doassign(ENODE **exp, int canerror, int *skm);
SNODE *whilestmt(void);
SNODE *dostmt(void);
SNODE *forstmt(void);
SNODE *ifstmt(void);
SNODE *casestmt(void);
SNODE *switchstmt(void);
ENODE *do_ret_constructor(TYP *tp, TYP *head1, ENODE *node, int size, int
    offset, int implicit);
SNODE *breakcontrundowns(SNODE *snp);
ENODE *retrundowns(void);
SNODE *retstmt(void);
SNODE *breakstmt(void);
SNODE *contstmt(void);
SNODE *_genwordstmt(void);
SNODE *exprstmt(void);
SNODE *snp_line(int tocode);
void dodefaultinit(SYM *sp);
void evaluateVLAtypedef(SYM *sp);
ENODE *createVarArray(SYM *sp, TYP *tp, int global, int alloc);
void doautoinit(SYM *sym);
SNODE *compound(void);
SNODE *labelstmt(int fetchnext);
SNODE *gotostmt(void);
SNODE *catchstmt(TRYBLOCK *a);
SNODE *trystmt(void);
SNODE *asm_statement(int shortfin); /* PROTOTYPE */
SNODE *statement(void);
SNODE *cppblockedstatement(int breakdest);

                              /* Symbol.c */

void symini(void);
unsigned int ComputeHash(char *string, int size);                 /* STATIC */
HASHREC *AddHash(HASHREC *item, HASHREC **table, int size);
HASHREC **LookupHash(char *key, HASHREC **table, int size);
void pushusing(SYM *sp, int tag);
void aliasnamespace(char *name);
void switchtonamespace(char *name);
void switchfromnamespace(void);
void inject_namespace(int ns);
SYM *namespace_search(char *name, NAMESPACE *data, int tags);
SYM *namespace_search_full(char *name, LIST **l, TABLE *table);   /* STATIC */
int isaccessible(SYM *sp);
SYM *basesearch(char *na, TABLE *table, int checkaccess);
SYM *search2(char *na, TABLE *table, int checkaccess, int definition);
SYM *search(char *na, TABLE *table);
SYM *losearch(char *na);
SYM *tcsearch(char *na, int checkaccess);
SYM *gsearch(char *na);
SYM *typesearch(char *na);
void insert(SYM *sp, TABLE *table);
int unlinkTag(SYM *sp);

                             /* Template.c */

void template_init(void);
void template_rundown(void);
void savecontext(void);                                           /* STATIC */
void restorecontext(void);                                        /* STATIC */
int tcmp(short *p, short *str, int *len);                         /* STATIC */
short *matchname(short *s, short *str, int *len, int nolt);       /* STATIC */
short *doreplace(short *buf, int *max, int *len, short *find, short
    *replace, int slen, int flen, int nolt);                      /* STATIC */
short *llreplace2(short *buf, int *max, int *len, char *find, char
    *replace);                                                    /* STATIC */
short *llreplace(short *buf, int *max, int *len, TYP *to, TYP *tn);/* STATIC */
short *replace2(char *buf, int *max, int *len, LIST *to, LIST *tn);/* STATIC */
short *replace(struct _templatelist *tl, struct template  *tm);   /* STATIC */
int tlookup(TYP *t, struct template  *tpl);
int tlookuptp(TYP *f, TYP *t, struct template  *tpl, TYP **rv);
struct _templatelist *conjoint(SYM *fi, SYM *ti, SYM *sp, char *name);/* STATIC */
struct _templatelist *search_template_func(SYM *tbsym, TYP *tp, char
    *name);                                                       /* STATIC */
TYP *tplconst(void);                                              /* STATIC */
void matchandmangle(char *mangname, char *nm, LIST **tail, SYM *tplargs);/* STATIC */
TYP *InstantiateTemplateFuncDirect(ENODE **pnode, TYP *tptr);
SYM *lookupTemplateFunc(SYM *tbsym, TYP *tp, char *name);
void instantiateTemplateFuncs(SYM *tempsym, SYM *actual, LIST *tn, int recons);
LIST *getTemplateName(SYM *sp, char *name);
void tsname(char *unmang, char *name);
int spcomp(TYP **lst, int *count, int *partials, struct template  *tm,
    LIST *fi, LIST *ti);                                          /* STATIC */
SYM *specialize(SYM *sp, TYP **tn);                               /* STATIC */
SYM *lookupTemplateType(SYM *tempsym, int cppflags);
void replaceTemplateFunctionClasses(SYM *sp, SYM *typequal);
void templateMatchCheck(SYM *tq, struct template  *p);
void replaceSize(TYP *candidate);
void parseClassFuncs(ILIST *l);

                               /* Types.c */

int exactype(TYP *typ1, TYP *typ2, int asn);
int checktype(TYP *typ1, TYP *typ2, int scalarok);
int checktypeassign(TYP *typ1, TYP *typ2);
void unm2name(char *buf, SYM *sp);                                /* STATIC */
TYP *typenum2(char *buf, TYP *tp);
char *typenum(char *buf, TYP *typ);

                               /* Usage.c */

void banner(char *fmt, ...);
void usage(char *prog_name);
ENODE *makeintnode(enum e_node nt, LLONG_TYPE val);
void compile(void);
void dodecl(int defclass);
void declstruct(TABLE *table, int ztype, int flags, int cppflags);
void declenum(TABLE *table, int cppflags);
void structbody(SYM *sp, int ztype);
SYM *enumbody(SYM *sp_in, TABLE *table);
void doargdecl(int defclass, char *names[], int *nparms, TABLE *table, int
    isinline);
int blockdecl(void);
int imax(int i, int j);
void checkTemplateSpecialization(SYM *sp, char *name);
void insertTemplateChar(short t);
void createTemplate(TABLE *tbl, enum e_sc sc);
int declbegin(int st);
void using_keyword(void);
void outcode_dump_muldivval(void);
