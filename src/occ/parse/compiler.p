/* Protogen Version 1.00 Saturday November 18, 2006  21:08:23 */

                             /* Beinterf.c */

BE_IMODEDATA *beArgType(IMODE *in);
int sizeFromISZ(int isz);
int needsAtomicLockFromType(TYPE *tp);
int getSize(enum e_bt type);
int getBaseAlign(enum e_bt type);
long getautoval(long val);
int funcvaluesize(int val);
int alignment(int sc, TYPE *tp);
int getAlign(int sc, TYPE *tp);
char *getUsageText(void);
char *lookupRegName(int regnum);
KEYWORD *GetProcKeywords(void);
int init_backend(int *argc ,char **argv);

                              /* Browse.c */

void abspath(char *name);
void browse_init(void);
void browse_startfunc(SYMBOL *func, int lineno);
void browse_endfunc(SYMBOL *func, int lineno);
void browse_startfile(char *name, int filenum);
void browse_variable(SYMBOL *var);
void browse_define(char *name, int lineno, int charindex);
void browse_blockstart(int lineno);
void browse_blockend(int lineno);

                               /* Ccerr.c */
void errorinit(void);
void errskim(LEXEME **lex, enum e_kw *skimlist);
void skip(LEXEME **lex, enum e_kw kw);
BOOLEAN needkw(LEXEME **lex, enum e_kw kw);
void pperror(int err, int data);
void pperrorstr(int err, char *str);
void preverror(int err, char *name, char *origfile, int origline);
void preverrorsym(int err, SYMBOL *sp, char *origfile, int origline);
void errorat(int err, char *name, char *file, int line);
void error(int err);
void errorcurrent(int err);
void errorqualified(int err, SYMBOL *strSym, NAMESPACEVALUES *nsv, char *name);
void errorNotMember(SYMBOL *strSym, NAMESPACEVALUES *nsv, char *name);
void errorint(int err, int val);
void errorstr(int err, char *val);
void errorsym(int err, SYMBOL *sym);
void errorsym2(int err, SYMBOL *sym1, SYMBOL *sym2);
void errorstrsym(int err, char *name, SYMBOL *sym2);
void errortype (int err, TYPE *tp1, TYPE *tp2);
void membererror(char *name, TYPE *tp1);
void errorarg(int err, int argnum, SYMBOL *declsp, SYMBOL *funcsp);
void specerror(int err, char *name, char *file, int line);
void diag(char *fmt, ...);
void printToListFile(char *fmt, ...);
void ErrorsToListFile(void);
void AddErrorToList(char *tag, char *str);
void checkUnlabeledReferences(BLOCKDATA *parent);
void checkGotoPastVLA(STATEMENT *stmt, BOOLEAN first);
void checkUnused(HASHTABLE *syms);
void findUnusedStatics(NAMESPACEVALUES *syms);
void assignmentUsages(EXPRESSION *node, BOOLEAN first);
void checkDefaultArguments(SYMBOL *spi);

                              /* Ccmain.c */

void BOOLEAN_setup(char select, char *string);
void codegen_setup(char select, char *string);
void optimize_setup(char select, char *string);
void compile(void);
int main(int argc, char *argv[]);

                             /* Constopt.c */

void constoptinit(void);
ULLONG_TYPE CastToInt(int size, LLONG_TYPE value);
FPF CastToFloat(int size, FPF *value);
FPF *IntToFloat(FPF* temp, int size, LLONG_TYPE value);
FPF refloat(EXPRESSION *node);
LLONG_TYPE MaxOut(enum e_bt size, LLONG_TYPE value);
ULLONG_TYPE reint(EXPRESSION *node);
void dooper(EXPRESSION **node, int mode);
int pwrof2(LLONG_TYPE i);
LLONG_TYPE mod_mask(int i);
void addaside(EXPRESSION *node);
int opt0(EXPRESSION **node);
void enswap(EXPRESSION **one, EXPRESSION **two);
int fold_const(EXPRESSION *node);
int typedconsts(EXPRESSION *node1);
void optimize_for_constants(EXPRESSION **expr);
LEXEME *optimized_expression(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, 
	EXPRESSION **expr, BOOLEAN commaallowed);

                               /* Debug.c */

void displayLexeme(LEXEME *lex);

                              /* Declare.c */

BOOLEAN sameTemplate(TYPE *P, TYPE *A);
void ParseBuiltins(void);
BOOLEAN ParseAttributeSpecifiers(LEXEME **lex, SYMBOL *funcsp, BOOLEAN always);
void declare_init(void);
void addStructureDeclaration(STRUCTSYM *decl);
void addTemplateDeclaration(STRUCTSYM *decl);
void dropStructureDeclaration(void);
SYMBOL *getStructureDeclaration(void);
void FinishStruct(SYMBOL *sp, SYMBOL *funcsp);
LEXEME *innerDeclStruct(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, BOOLEAN isTemplate, enum e_ac defaultAccess, BOOLEAN isfinal, BOOLEAN *defd);
void checkPackedType(SYMBOL *sp);
void checkPackedExpression(EXPRESSION *sp);
void checkUnpackedExpression(EXPRESSION *sp);
INITLIST **expandPackedInitList(INITLIST **lptr, SYMBOL *funcsp, LEXEME *start, EXPRESSION *packedExp);
void expandPackedMemberInitializers(SYMBOL *cls, SYMBOL *funcsp, TEMPLATEPARAMLIST *templatePack, MEMBERINITIALIZERS **p, LEXEME *start, INITLIST *list);
void checkOperatorArgs(SYMBOL *sp, BOOLEAN asFriend);
SYMBOL *getCopyCons(SYMBOL *base, BOOLEAN move);
void ConsDestDeclarationErrors(SYMBOL *sp, BOOLEAN notype);
MEMBERINITIALIZERS *GetMemberInitializers(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sp);
void ParseMemberInitializers(SYMBOL *cls, SYMBOL *cons);
SYMBOL *insertFunc(SYMBOL *sp, SYMBOL *ovl);
void createConstructorsForLambda(SYMBOL *sp);
void createDefaultConstructors(SYMBOL *sp);
void destructBlock(EXPRESSION **exp, HASHREC *hr);
EXPRESSION *thunkConstructorHead(BLOCKDATA *b, SYMBOL *sym, SYMBOL *cons, HASHTABLE *syms, BOOLEAN parseInitializers, BOOLEAN doCopy);
void thunkDestructorTail(BLOCKDATA *b, SYMBOL *sp, SYMBOL *dest, HASHTABLE *syms);
void createAssignment(SYMBOL *sym, SYMBOL *asnfunc);
void makeArrayConsDest(TYPE **tp, EXPRESSION **exp, SYMBOL *cons, SYMBOL *dest, EXPRESSION *count);
void callDestructor(SYMBOL *sp, EXPRESSION **exp, EXPRESSION *arrayElms, BOOLEAN top, BOOLEAN noinline, BOOLEAN pointer);
BOOLEAN callConstructor(TYPE **tp, EXPRESSION **exp, FUNCTIONCALL *params, 
                    BOOLEAN checkcopy, EXPRESSION *arrayElms, BOOLEAN top, 
                    BOOLEAN maybeConversion, BOOLEAN noinline, BOOLEAN implicit, BOOLEAN pointer);
LEXEME *insertNamespace(LEXEME *lex, enum e_lk linkage, enum e_sc storage_class, BOOLEAN *linked);
LEXEME *insertUsing(LEXEME *lex, enum e_ac access, enum e_sc storage_class, BOOLEAN hasAttribs);
LEXEME *handleStaticAssert(LEXEME *lex);
void InsertExtern(SYMBOL *sp);
void InsertGlobal(SYMBOL *sp);
char *AnonymousName(void);
SYMBOL *makeID(enum e_sc storage_class, TYPE *tp, SYMBOL *spi, char *name);
void InsertSymbol(SYMBOL *sp, enum e_sc storage_class, enum e_lk linkage, BOOLEAN ignoreDups);
LEXEME *getDeferredData(LEXEME *lex, SYMBOL *sym, BOOLEAN braces);
LEXEME *get_type_id(LEXEME *lex, TYPE **tp, SYMBOL *funcsp, BOOLEAN beforeOnly);
int classRefCount(SYMBOL *base, SYMBOL *derived);
int allocVTabSpace(VTABENTRY *vtab, int offset);
void calculateVTabEntries(SYMBOL *sp, SYMBOL *base, VTABENTRY **pos, int offset);
void calculateVirtualBaseOffsets(SYMBOL *sp, SYMBOL *base, BOOLEAN isvirtual, int offset);
void deferredCompileOne(SYMBOL *cur);
void deferredCompile(void);
void backFillDeferredInitializersForFunction(SYMBOL *cur, SYMBOL *funcsp);
void backFillDeferredInitializers(SYMBOL *declsym, SYMBOL *funcsp);
TYPE * PerformDeferredInitialization (TYPE *tp, SYMBOL *funcsp);
void warnCPPWarnings(SYMBOL *sym, BOOLEAN localClassWarnings);
BOOLEAN usesVTab(SYMBOL *sym);
LEXEME *baseClasses(LEXEME *lex, SYMBOL *funcsp, SYMBOL *declsym, enum e_ac defaultAccess);
SYMBOL * calculateStructAbstractness(SYMBOL *top, SYMBOL *sp);
LEXEME *getFunctionParams(LEXEME *lex, SYMBOL *funcsp, SYMBOL **spin, TYPE **tp, BOOLEAN inTemplate, enum e_sc storage_class);
LEXEME *getQualifiers(LEXEME *lex, TYPE **tp, enum e_lk *linkage, enum e_lk *linkage2, enum e_lk *linkage3);
LEXEME *getBasicType(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, SYMBOL **strSym, BOOLEAN isTemplate, enum e_sc storage_class, 
					enum e_lk *linkage_in, enum e_lk *linkage2_in, enum e_lk *linkage3, 
                    enum e_ac access, BOOLEAN *notype, BOOLEAN *defd, int *consdest, BOOLEAN isTypedef);
LEXEME *getBeforeType(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, SYMBOL **spi, SYMBOL **strSym,
                      NAMESPACEVALUES **nsv, BOOLEAN isTemplate, enum e_sc storage_class,
							 enum e_lk *linkage, enum e_lk *linkage2, enum e_lk *linkage3, BOOLEAN asFriend,
                        int consdest, BOOLEAN beforeOnly);
void sizeQualifiers(TYPE *tp);
void unvisitUsingDirectives(NAMESPACEVALUES *v);
void injectThisPtr(SYMBOL *sp, HASHTABLE *syms);
BOOLEAN typeHasTemplateArg(TYPE *t);
void templateInit(void);
void TemplateGetDeferred(SYMBOL *sym);
TEMPLATEPARAMLIST *TemplateGetParams(SYMBOL *sym);
void TemplateRegisterDeferred(LEXEME *lex);
BOOLEAN exactMatchOnTemplateArgs(TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym);
BOOLEAN exactMatchOnTemplateParams(TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym);
SYMBOL *LookupSpecialization(SYMBOL *sym, TEMPLATEPARAMLIST *templateParams);
SYMBOL *LookupFunctionSpecialization(SYMBOL *overloads, SYMBOL *sp, TEMPLATEPARAMLIST *templateParams);
TEMPLATEPARAMLIST * TemplateMatching(LEXEME *lex, TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym, SYMBOL *sp, BOOLEAN definition);
LEXEME *GetTemplateArguments(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST **lst);
BOOLEAN TemplateIntroduceArgs(TEMPLATEPARAMLIST *sym, TEMPLATEPARAMLIST *args);
TYPE *SynthesizeType(TYPE *tp,TEMPLATEPARAMLIST *enclosing,  BOOLEAN alt);
SYMBOL *SynthesizeResult(SYMBOL *sym, TEMPLATEPARAMLIST *params);
SYMBOL *TemplateSynthesizeFunction(SYMBOL *sym);
SYMBOL *TemplateDeduceArgsFromType(SYMBOL *sym, TYPE *tp);
void NormalizePacked(TYPE *tpo);
SYMBOL *TemplateDeduceArgsFromArgs(SYMBOL *sym, FUNCTIONCALL *args);
SYMBOL *TemplateDeduceWithoutArgs(SYMBOL *sym);
void TemplatePartialOrdering(SYMBOL **table, int count, FUNCTIONCALL *funcparams, TYPE *atype, BOOLEAN byClass, BOOLEAN save);
SYMBOL *TemplateClassInstantiateInternal(SYMBOL *sym, TEMPLATEPARAMLIST *args, BOOLEAN isExtern);
SYMBOL *TemplateClassInstantiate(SYMBOL *sym, TEMPLATEPARAMLIST *args, BOOLEAN isExtern, enum e_sc storage_class);
void TemplateDataInstantiate(SYMBOL *sym, BOOLEAN warning, BOOLEAN isExtern);
SYMBOL *TemplateFunctionInstantiate(SYMBOL *sym, BOOLEAN warning, BOOLEAN isExtern);
SYMBOL *GetClassTemplate(SYMBOL *sp, TEMPLATEPARAMLIST *args, BOOLEAN isExtern, enum e_sc storage_class);
void DoInstantiateTemplateFunction(TYPE *tp, SYMBOL **sp, NAMESPACEVALUES *nsv, SYMBOL *strSym, TEMPLATEPARAMLIST *templateParams, BOOLEAN isExtern);
void DoDefaultSpecialization(SYMBOL *sp2);
TEMPLATEPARAMLIST *getCurrentSpecialization(SYMBOL *sp);
LEXEME *TemplateDeclaration(LEXEME *lex, SYMBOL *funcsp, enum e_ac access, enum e_sc storage_class, BOOLEAN isextern);
LEXEME *declare(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, enum e_sc storage_class, enum e_lk defaultLinkage,
					   BLOCKDATA *parent, BOOLEAN needsemi, BOOLEAN asExpression, BOOLEAN asfriend, BOOLEAN isTemplate, enum e_ac access );

                               /* Expr.c */

void checkscope(TYPE *tp1, TYPE *tp2);
void checkauto(TYPE *tp);
void qualifyForFunc(SYMBOL *sym, TYPE **tp);
void getThisType(SYMBOL *sym, TYPE **tp);
SYMBOL *lambda_capture(SYMBOL *sym, enum e_cm mode, BOOLEAN isExplicit);
LEXEME *expression_lambda(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, int flags);
EXPRESSION *getMemberBase(SYMBOL *memberSym, SYMBOL *strSym, SYMBOL *funcsp, BOOLEAN toError);
EXPRESSION *getMemberNode(SYMBOL *memberSym, SYMBOL *strSym, TYPE **tp, SYMBOL *funcsp);
EXPRESSION *getMemberPtr(SYMBOL *memberSym, SYMBOL *strSym, TYPE **tp, SYMBOL *funcsp);
EXPRESSION *substitute_params_for_function(FUNCTIONCALL *funcparams, HASHTABLE *syms);
EXPRESSION *substitute_params_for_constexpr(EXPRESSION *exp, FUNCTIONCALL *funcparams, HASHTABLE *syms);
LEXEME *expression_func_type_cast(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags);
BOOLEAN doDynamicCast(TYPE **newType, TYPE *oldTYPE, EXPRESSION **exp, SYMBOL *funcsp, BOOLEAN noinline);
BOOLEAN doStaticCast(TYPE **newType, TYPE *oldTYPE, EXPRESSION **exp, SYMBOL *funcsp, BOOLEAN checkconst, BOOLEAN noinline);
BOOLEAN doConstCast(TYPE **newType, TYPE *oldTYPE, EXPRESSION **exp, SYMBOL *funcsp);
BOOLEAN doReinterpretCast(TYPE **newType, TYPE *oldTYPE, EXPRESSION **exp, SYMBOL *funcsp, BOOLEAN checkconst);
BOOLEAN castToPointer(TYPE **tp, EXPRESSION **exp, enum e_kw kw, TYPE *other, BOOLEAN noinline);
void castToArithmetic(BOOLEAN integer, TYPE **tp, EXPRESSION **exp, enum e_kw kw, TYPE *other, BOOLEAN noinline, BOOLEAN implicit);
BOOLEAN cppCast(TYPE *src, TYPE **dest, EXPRESSION **exp, BOOLEAN noinline);
LEXEME *GetCastInfo(LEXEME *lex, SYMBOL *funcsp, TYPE **newType, TYPE **oldType, EXPRESSION **oldExp, BOOLEAN packed);
LEXEME *expression_typeid(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags);
BOOLEAN insertOperatorParams(SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, FUNCTIONCALL *funcParams, BOOLEAN noinline);
BOOLEAN insertOperatorFunc(enum ovcl cls, enum e_kw kw, SYMBOL *funcsp, 
                        TYPE **tp, EXPRESSION **exp, TYPE *tp1, EXPRESSION *exp1, INITLIST *args, BOOLEAN noinline);
LEXEME *expression_new(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOLEAN global, int flags);
LEXEME *expression_delete(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOLEAN global, int flags);
LEXEME *expression_noexcept(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp);
LEXEME *expression_cast(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
EXPRESSION *exprNode(enum e_node type, EXPRESSION *left, EXPRESSION *right);
EXPRESSION *varNode(enum e_node type, SYMBOL *sp);
EXPRESSION *intNode(enum e_node type, LLONG_TYPE val);
EXPRESSION *baseClassOffset(SYMBOL *base, SYMBOL *derived, EXPRESSION *en);
LEXEME *getInitList(LEXEME *lex, SYMBOL *funcsp, INITLIST **owner);
LEXEME *getArgs(LEXEME *lex, SYMBOL *funcsp, FUNCTIONCALL *funcparams, enum e_kw finish, BOOLEAN allowPack);
LEXEME *getMemberInitializers(LEXEME *lex, SYMBOL *funcsp, FUNCTIONCALL *funcparams, enum e_kw finish, BOOLEAN allowPack);
void DerivedToBase(TYPE *tpn, TYPE *tpo, EXPRESSION **exp);
void AdjustParams(HASHREC *hr, INITLIST **lptr, BOOLEAN operands, BOOLEAN noinline);
LEXEME *expression_arguments(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags);
LEXEME *expression_unary(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
LEXEME *expression_assign(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
LEXEME *expression_no_comma(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
LEXEME *expression_no_check(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, int flags);
LEXEME *expression(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, int flags);

                               /* Float.c */

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
void FPFTruncate(FPF *value, int bits, int maxexp, int minexp);

                               /* Help.c */
BOOLEAN ismember(SYMBOL *sym);
BOOLEAN istype(SYMBOL *sym);
BOOLEAN ismemberdata(SYMBOL *sp);
BOOLEAN startOfType(LEXEME *lex, BOOLEAN assumeType);
TYPE *basetype(TYPE *tp);
BOOLEAN isunsigned(TYPE *tp);
BOOLEAN isint(TYPE *tp);
BOOLEAN isfloat(TYPE *tp);
BOOLEAN iscomplex(TYPE *tp);
BOOLEAN isimaginary(TYPE *tp);
BOOLEAN isarithmetic(TYPE *tp);
BOOLEAN isconst(TYPE *tp);
BOOLEAN isvolatile(TYPE *tp);
BOOLEAN isrestrict(TYPE *tp);
BOOLEAN isatomic(TYPE *tp);
BOOLEAN isvoid(TYPE *tp);
BOOLEAN ispointer(TYPE *tp);
BOOLEAN isref(TYPE *tp);
BOOLEAN isvoidptr(TYPE *tp);
BOOLEAN isfunction(TYPE *tp);
BOOLEAN isfuncptr(TYPE *tp);
BOOLEAN isarray(TYPE *tp);
BOOLEAN isunion(TYPE *tp);
BOOLEAN isstructured(TYPE *tp);
SYMBOL *getFunctionSP(TYPE **tp);
LEXEME *concatStringsInternal(LEXEME *lex, STRING **str, int *elems);
LEXEME *concatStrings(LEXEME *lex, EXPRESSION **exp, enum e_lexType *tp, int *elems);
BOOLEAN isintconst(EXPRESSION *exp);
BOOLEAN isfloatconst(EXPRESSION *exp);
BOOLEAN isimaginaryconst(EXPRESSION *exp);
BOOLEAN iscomplexconst(EXPRESSION *exp);
EXPRESSION *anonymousVar(enum e_sc storage_class, TYPE *tp);
void deref(TYPE *tp, EXPRESSION **exp);
int sizeFromType(TYPE *tp);
void cast(TYPE *tp, EXPRESSION **exp);
BOOLEAN castvalue(EXPRESSION *exp);
BOOLEAN lvalue(EXPRESSION *exp);
BOOLEAN xvalue(EXPRESSION *exp);
EXPRESSION *convertInitToExpression(TYPE *tp, SYMBOL *sp, SYMBOL *funcsp, INITIALIZER *init, EXPRESSION *thisptr, BOOLEAN noinline);
BOOLEAN assignDiscardsConst(TYPE *dest, TYPE *source);
BOOLEAN isconstzero(TYPE *tp, EXPRESSION *exp);
BOOLEAN isarithmeticconst(EXPRESSION *exp);
BOOLEAN isconstaddress(EXPRESSION *exp);
SYMBOL *clonesym(SYMBOL *sym);
TYPE *destSize(TYPE *tp1, TYPE *tp2, EXPRESSION **exp1, EXPRESSION **exp2, BOOLEAN minimizeInt, TYPE *atp);
EXPRESSION *RemoveAutoIncDec(EXPRESSION *exp);
							  /* IAlias.c */
void AliasInit(void);
void AliasPass1(void);
void AliasPass2(void);
void AliasStruct(BITINT *bits, IMODE *ans, IMODE *left, IMODE *right);
void AliasGosub(QUAD *ins, BITINT *parms, BITINT *bits, int n);
void AliasUses(BITINT *bit, IMODE *im, BOOLEAN rhs);

                              /* Iblock.c */

int equalnode(EXPRESSION *node1, EXPRESSION *node2);
int equalimode(IMODE *ap1, IMODE *ap2);
short dhash(UBYTE *str, int len);
QUAD *LookupNVHash(UBYTE *key, int size, DAGLIST **table);
DAGLIST *ReplaceHash(QUAD *rv, UBYTE *key, int size, DAGLIST **table);
QUAD *liveout(QUAD *node);
int ToQuadConst(IMODE **im);
void dag_rundown(void);
BLOCKLIST *newBlock(void);
void addblock(int val);
BOOLEAN usesAddress(IMODE *im);
void gen_label(int labno);
void flush_dag(void);
QUAD * gen_icode(enum e_op op, IMODE *res, IMODE *left, IMODE *right);
void gen_iiconst(IMODE *res, LLONG_TYPE val);
void gen_ifconst(IMODE *res, FPF val);
void gen_igoto(enum i_ops op, long label);
void gen_data(int val);
void gen_icgoto(int op, long label, IMODE *left, IMODE *right);
QUAD *gen_igosub(int op, IMODE *left);
void gen_icode2(int op, IMODE *res, IMODE *left, IMODE *right, int label);
void gen_line(LINEDATA *data);
void gen_asm(STATEMENT *stmt);
void gen_asmdata(STATEMENT *stmt);
void gen_nodag(int op, IMODE *res, IMODE *left, IMODE *right);
void RemoveFromUses(QUAD *ins, int tnum);
void InsertUses(QUAD *ins, int tnum);
void RemoveInstruction(QUAD *ins);
void InsertInstruction(QUAD *before, QUAD *ins);

                               /* Icom.c */

void commondelete(QUAD *quad);

							   /* iconfl.c */

void conflictini(void);
void resetConflict(void);
int findPartition(int T0);
void insertConflict(int i, int j);
void JoinConflictLists(int T0, int T1);
BOOLEAN isConflicting(int T0, int T1);
void CalculateConflictGraph(BRIGGS_SET *globals, BOOLEAN optimize);

                              /* Iconst.c */

void ConstantFold(QUAD *d, BOOLEAN reflow);
void ConstantFlow(void);

                               /* Iexpr.c */

void iexpr_init(void);
void iexpr_func_init(void);
IMODE *GetLoadTemp(IMODE *dest);
IMODE *LookupLoadTemp(IMODE *dest, IMODE *source);
int chksize(int lsize, int rsize);
SYMBOL *varsp(EXPRESSION *node);
void DumpIncDec(SYMBOL *funcsp);
IMODE *make_imaddress(EXPRESSION *node, int size);
IMODE *make_bf(EXPRESSION *node, IMODE *ap, int size);
IMODE *make_immed(int size, LLONG_TYPE i);
IMODE *make_fimmed(int size,FPF f);
IMODE *make_parmadj(long i);
IMODE *make_ioffset(EXPRESSION *node);
IMODE *genasn(EXPRESSION *node, SYMBOL *funcsp, int size);
IMODE *indnode(IMODE *ap1, int size);
IMODE *LookupExpression(enum e_op op, int size, IMODE *left, IMODE *right);
IMODE *gen_deref(EXPRESSION *node, SYMBOL *funcsp, int flags);
IMODE *gen_unary(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, int op);
IMODE *gen_asrhd(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, int op);
IMODE *gen_binary(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, int op);
IMODE *gen_pdiv(SYMBOL *funcsp, EXPRESSION *node, int flags, int size);
IMODE *gen_pmul(SYMBOL *funcsp, EXPRESSION *node, int flags, int size);
IMODE *gen_hook(SYMBOL *funcsp, EXPRESSION *node, int flags, int size);
IMODE *gen_moveblock(EXPRESSION *node, SYMBOL *funcsp);
IMODE *gen_clearblock(EXPRESSION *node, SYMBOL *funcsp);
IMODE *gen_assign(SYMBOL *funcsp, EXPRESSION *node, int flags, int size);
IMODE *gen_aincdec(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, int op);
IMODE *gen_tcall(EXPRESSION *node, SYMBOL *funcsp, int flags);
IMODE *gen_trapcall(SYMBOL *funcsp, EXPRESSION *node, int flags);
IMODE *gen_stmt_from_expr(SYMBOL *funcsp, EXPRESSION *node, int flags);
IMODE *gen_funccall(SYMBOL *funcsp, EXPRESSION *node, int flags);
IMODE *gen_expr(SYMBOL *funcsp, EXPRESSION *node, int flags, int size);
IMODE *gen_void(EXPRESSION *node, SYMBOL *funcsp);
int natural_size(EXPRESSION *node);
void gen_compare(EXPRESSION *node, SYMBOL *funcsp, int btype, int label);
IMODE *gen_set(EXPRESSION *node, SYMBOL *funcsp, int btype);
IMODE *defcond(EXPRESSION *node, SYMBOL *funcsp);
IMODE *gen_relat(EXPRESSION *node, SYMBOL *funcsp);
void truejp(EXPRESSION *node, SYMBOL *funcsp, int label);
void falsejp(EXPRESSION *node, SYMBOL *funcsp, int label);

                               /* Iflow.c */

void flow_init(void);
void dump_dfst(void);
void create_dfst(int n);
enum e_fgtype getEdgeType(int first, int second);
void unlinkBlock(BLOCK *succ, BLOCK *pred);
void UnlinkCritical(BLOCK *s);
void reflowConditional(BLOCK *src, BLOCK *dst);
void doms_only(BOOLEAN always);
void flows_and_doms(void);
BOOLEAN dominatedby(BLOCK *src, BLOCK *ancestor);
void WalkFlowgraph(BLOCK *b, 
				   int (*func)(enum e_fgtype type, BLOCK *parent, BLOCK *b), int fwd);


							  /* iglobal.c */

void LocalOptimization(void);

							  /* iinvar.c */

void MoveLoopInvariants(void);

                              /* Ilazy.c */
void RearrangePrecolors(void);
void GlobalOptimization(void);

								/* ilimit.c */

void limitInit(void);
QUAD *beforeJmp(QUAD *I, BOOLEAN before);
void LimitRegisterPressure(void);

                               /* Ilive.c */

void liveVariables(void);

                              /* Ilocal.c */

void usesInfo(void);
void definesInfo(void);
IMODE *InitTempOpt(int size1, int size2);
void gatherLocalInfo(SYMBOL *funcsp);
void removeDead(BLOCK *b);
								/* iloop.c */

void CancelInfinite(int orgBlockCount);
void BuildLoopTree(void);
BOOLEAN isAncestor(LOOP *l1, LOOP *l2);
void RemoveInfiniteThunks(void);
void CalculateInduction(void);

                               /* Init.c */

void init_init(void);
void dumpStartups(void);
void dumpCPPStaticsAndTLS();
void insertStartup(BOOLEAN startupFlag, char *name, int prio);
char *lookupAlias(char *name);
void insertAlias(char *name, char *alias);
int dumpMemberPtr(SYMBOL *sp, TYPE *membertp, BOOLEAN make_label);
void dumpInitializers(void);
void dumpVTab(SYMBOL *sym);
void insertInitSym(SYMBOL *sp);
BOOLEAN IsConstWithArr(TYPE *tp);
enum e_node referenceTypeError(TYPE *tp, EXPRESSION *exp);
EXPRESSION *createTemporary(TYPE *tp, EXPRESSION *val);
LEXEME *initType(LEXEME *lex, SYMBOL *funcsp, int offset, enum e_sc sc, 
				 INITIALIZER **init, INITIALIZER **dest, TYPE *itype, SYMBOL *sp, BOOLEAN arrayMember);
BOOLEAN IsConstantExpression(EXPRESSION *node, BOOLEAN allowParams);
LEXEME *initialize(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, enum e_sc storage_class_in, BOOLEAN uninitconst);

                              /* Inline.c */

void inlineinit(void);
void dumpInlines(void);
void ReferenceVTab(SYMBOL *sym);
void InsertInline(SYMBOL *sp);
void InsertInlineData(SYMBOL *sp);
SYMBOL *getvc1Thunk(int offset);
SYMBOL *inlineSearch(SYMBOL *name);
void inlineinsert(SYMBOL *sp);
EXPRESSION *inlineexpr(EXPRESSION *node, BOOLEAN *fromlval);
STATEMENT *inlinestmt(STATEMENT *block);
STATEMENT *inlinefuncargs(FUNCTIONCALL *params);
void inlinereblock(SYMBOL *sp);
EXPRESSION *doinline(FUNCTIONCALL *params, SYMBOL *funcsp);
BOOLEAN IsEmptyFunction(FUNCTIONCALL *params, SYMBOL *funcsp);
EXPRESSION *EvaluateConstFunction(FUNCTIONCALL *funcparams, SYMBOL *funcsp);

                             /* Ioptutil.c */

BRIGGS_SET *briggsAlloc(int size);
BRIGGS_SET *briggsAlloct(int size);
BRIGGS_SET *briggsAllocs(int size);
BRIGGS_SET *briggsReAlloc(BRIGGS_SET *set, int size);
int briggsSet(BRIGGS_SET *p, int index);
int briggsReset(BRIGGS_SET *p, int index);
int briggsTest(BRIGGS_SET *p, int index);
int briggsUnion(BRIGGS_SET *s1, BRIGGS_SET *s2);
int briggsIntersect(BRIGGS_SET *s1, BRIGGS_SET *s2);

                               /* Iout.c */

void outcodeini(void);
void beDecorateSymName(char *buf, SYMBOL *sp);
void putconst(EXPRESSION *offset, int color);
void putlen(int l);
void putamode(QUAD *q, IMODE *ap);
void put_code(QUAD *q);
int beVariableLive(IMODE *m);
void rewrite_icode(void);
void gen_vtt(VTABENTRY *entry, SYMBOL *func, SYMBOL *name);
void gen_strlab(SYMBOL *sp);
void put_label(int lab);
void put_staticlabel(long label);
void genfloat(FPF *val);
void gendouble(FPF *val);
void genlongdouble(FPF *val);
void genUBYTE(long val);
void genBOOLEAN(int val);
void genbit(SYMBOL *sp, int val);
void genshort(long val);
void genwchar_t(long val);
void genlong(long val);
void genlonglong(LLONG_TYPE val);
void genint(int val);
void genuint16(int val);
void genuint32(int val);
void genenum(int val);
int genstring(STRING *str);
void genaddress(ULLONG_TYPE address);
void gensrref(SYMBOL *sp, int val);
void genlabref(int label);
void genref(SYMBOL *sp, int offset);
void genpcref(SYMBOL *sp, int offset);
void localdef(SYMBOL *sp);
void genstorage(int nUBYTEs);
void gen_labref(int n);
void gen_labdifref(int n1, int n2);
EXPRESSION *stringlit(STRING *s);
void dumpLits(void);
void nl(void);
void cseg(void);
void dseg(void);
void tseg(void);
void bssseg(void);
void xconstseg(void);
void xstringseg(void);
void startupseg(void);
void rundownseg(void);
void tlsstartupseg(void);
void tlsrundownseg(void);
void gen_virtual(SYMBOL *sp, int data);
void gen_endvirtual(SYMBOL *sp);
void align(int size);
void asm_header(char *name, char *version);
void asm_trailer(void);
void globaldef(SYMBOL *sp);
int put_exfunc(SYMBOL *sp, int notyet);
void put_expfunc(SYMBOL *sp);
void putexterns(void);

                               /* Ipeep.c */

void peepini(void);
void peep_icode(BOOLEAN branches);

								/*irc.c */

void regInit(void);
void alloc_init(void);
EXPRESSION *spillVar( enum e_sc storage_class, TYPE *tp);
void AllocateStackSpace(SYMBOL *funcsp);
void FillInPrologue(QUAD *head, SYMBOL *funcsp);
void Precolor(void);
void AllocateRegisters(QUAD *head);

							   /* Ireshape */

BOOLEAN variantThisLoop(BLOCK *b, int tnum);
void ReplaceHashReshape(QUAD *rv, UBYTE *key, int size, DAGLIST **table);
void InsertReshapeInstruction(QUAD *before, QUAD *ins);
BOOLEAN matchesop(enum i_ops one, enum i_ops two);
void Reshape(void);

							   /* Irewrite */
void Prealloc(int pass);
void RewriteLoopEntries(void);
void CalculateBackendLives(void);


                                /* Issa.c */

void AddToWipeList(DAGLIST *head);
void TranslateToSSA(void);
void RemoveCriticalThunks(void);
void TranslateFromSSA(BOOLEAN all);

                               /* Istmt.c */

void genstmtini(void);
EXPRESSION *tempenode(void);
IMODE *tempreg(int size, int mode);
IMODE *imake_label(int label);
IMODE *make_direct(int i);
void genwhile(STATEMENT *stmt, SYMBOL *funcsp);
void gen_for(STATEMENT *stmt, SYMBOL *funcsp);
void genif(STATEMENT *stmt, SYMBOL *funcsp);
void gendo(STATEMENT *stmt, SYMBOL *funcsp);
void gen_genword(STATEMENT *stmt, SYMBOL *funcsp);
IMODE *set_symbol(char *name, int isproc);
IMODE *call_library(char *lib_name, int size);
void count_cases(CASEDATA *cd, struct cases *cs);
void gather_cases(CASEDATA *cd, struct cases *cs);
int gcs_compare(void const *left, void const *right);
void genxswitch(STATEMENT *stmt, SYMBOL *funcsp);
void genreturn(STATEMENT *stmt, SYMBOL *funcsp, int flag, int noepilogue, IMODE *allocaAP);
void gen_tryblock(void *val);
IMODE *genstmt(STATEMENT *stmt, SYMBOL *funcsp);
SYMBOL *gen_mp_virtual_thunk(SYMBOL *vsp);
SYMBOL *gen_vsn_virtual_thunk(SYMBOL *func, int ofs);
void optimize(SYMBOL *funcsp);
void genfunc(SYMBOL *funcsp);
void genASM(STATEMENT *st);

							  /* istren.c */

void unmarkPreSSA(QUAD *ins);
void ReduceLoopStrength(void);

                                /* Lex.c */

void lexini(void);
KEYWORD *searchkw(char **p);
int getChar(char **source, enum e_lexType *tp);
SLCHAR *getString(char **source, enum e_lexType *tp);
int getId(char **ptr , char *dest);
LEXEME *SkipToNextLine(void);
LEXEME *getGTSym(LEXEME *in);
LEXEME *getsym(void);
LEXEME *prevsym(LEXEME *lex);
LEXEME *backupsym(void);
LEXEME *SetAlternateLex(LEXEME *lexList);

                               /* List.c */

void put_sc(int scl);
void put_ty(TYPE *tp);
void list_var(SYMBOL *sp, int i);
void list_table(HASHTABLE *t, int j);

                              /* Mangle.c */

char *mangleType (char *in, TYPE *tp, BOOLEAN first);
void SetLinkerNames(SYMBOL *sym, enum e_lk linkage);
char *unmangle(char *name, char *source);

                              /* Memory.c */

void mem_summary(void);
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
void AddExt(char *buffer, char *ext);
void StripExt(char *buffer);
void EXEPath(char *buffer, char *filename);
int HasExt(char *buffer, char *ext);
FILE *SrchPth3(char *string, char *searchpath, char *mode);
FILE *SrchPth2(char *name, char *path, char *attrib);
FILE *SrchPth(char *name, char *path, char *attrib, BOOLEAN sys);
BOOLEAN parse_args(int *argc, char *argv[], BOOLEAN case_sensitive);
void err_setup(char select, char *string);
void incl_setup(char select, char *string);
void def_setup(char select, char *string);
void undef_setup(char select, char *string);
void output_setup(char select, char *string);
void setglbdefs(void);
void InsertAnyFile(char *filename, char *path, int drive);
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

                              /* Output.c */

void oinit(void);
void oflush(FILE *file);
void oputc(int ch, FILE *file);
void beputc(int ch);
void owrite(char *buf, size_t size, int n, FILE *fil);
void beWrite(char *buf, size_t size);
void oprintf(FILE *file, char *format, ...);
void bePrintf(char *format, ...);

                              /* Ppexpr.c */

int getsch(int UBYTEs, char **source) ;
PPINT ppexpr(void);

                              /* Ppmain.c */

char *getUsageText(void);
void BOOLEAN_setup(char select, char *string);
int main(int argc, char *argv[]);

                              /* Preproc.c */

void preprocini(char *name, FILE *fil);
int defid(char *name, char **p);
void skipspace(void);
BOOLEAN expectid(char *buf);
BOOLEAN expectstring(char *buf, char **in, BOOLEAN angle);
LLONG_TYPE expectnum(BOOLEAN *uns);
BOOLEAN getline(void);
void preprocess(void);
void doerror(void);
void dowarning(void);
char *getauxname(char *ptr, char **bufp);
void dopragma(void);
void Compile_Pragma(void);
void doline(void);
INCLUDES *GetIncludeData(void);
void FreeInclData(INCLUDES *data);
void doinclude(void);
void glbdefine(char *name, char *value, BOOLEAN permanent);
void glbUndefine(char *name);
int undef2(char *name);
void dodefine(void);
void doundef(void);
void nodefines(void);
BOOLEAN indefine(DEFSTRUCT *sp);
void enterdefine(DEFSTRUCT *sp);
void exitdefine(void);
int definsert(unsigned char *macro, unsigned char *end, unsigned char *begin, unsigned char *text, unsigned char *expandedtext, int len, int replen);
void defstringizing(unsigned char *macro);
int defreplaceargs(unsigned char *macro, int count, unsigned char **oldargs, unsigned char **newargs, unsigned char **expandedargs, unsigned char *varargs);
void deftokenizing(unsigned char *macro);
char *fullqualify(char *string);
void filemac(char *string);
void datemac(char *string);
void timemac(char *string);
void linemac(char *string);
void defmacroreplace(char *macro, char *name);
int replacesegment(unsigned char *start, unsigned char *end, int *inbuffer, int totallen, char **pptr);
int defcheck(unsigned char *line);
void pushif(void);
void popif(void);
void ansieol(void);
void doifdef(BOOLEAN flag);
void doif(void);
void doelif(void);
void doelse(void);
void doendif(void);

                               /* Rtti.c */

SYMBOL * RTTIDumpType(TYPE *tp);
void XTDumpTab(SYMBOL *funcsp);

                               /* Stmt.c */

void statement_ini(void);
void InsertLineData(int lineno, int fileindex, char *fname, char *line);
void FlushLineData(char *file, int lineno);
STATEMENT *currentLineData(BLOCKDATA *parent, LEXEME *lex, int offset);
STATEMENT *stmtNode(LEXEME *lex, BLOCKDATA *parent, enum e_stmt stype);
int insertLabel(BLOCKDATA *parent);
LEXEME *statement_asm(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent);
void assignParam(SYMBOL *funcsp, int *base, SYMBOL *param);
BOOLEAN resolveToDeclaration(LEXEME *lex);
LEXEME *body (LEXEME *lex, SYMBOL *funcsp);

                              /* Symtab.c */

void syminit(void);
BOOLEAN singlebase(SYMBOL *base, SYMBOL *of);
HASHTABLE *CreateHashTable(int size);
void AllocateLocalContext(BLOCKDATA *parent, SYMBOL *sp);
void FreeLocalContext(BLOCKDATA *parent, SYMBOL *sp);
HASHREC **LookupName(char *name, HASHTABLE *table);
SYMBOL *search(char *name, HASHTABLE *table);
SYMBOL *matchOverload(SYMBOL *snew, SYMBOL *sold);
SYMBOL *searchOverloads(SYMBOL *sym, HASHTABLE *table);
SYMBOL *gsearch(char *name);
SYMBOL *tsearch(char *name);
TEMPLATEPARAMLIST *getTemplateStruct(char *name);
LIST *tablesearchone(char *name, NAMESPACEVALUES *ns, BOOLEAN tagsOnly);
SYMBOL *namespacesearch(char *name, NAMESPACEVALUES *ns, BOOLEAN qualified, BOOLEAN tagsOnly);
SYMBOL *templatesearch(char *name, TEMPLATEPARAMLIST *arg);
SYMBOL *classsearch(char *name, BOOLEAN tagsOnly);
LEXEME *nestedPath(LEXEME *lex, SYMBOL **sym, NAMESPACEVALUES **ns, BOOLEAN *throughPath, BOOLEAN tagsOnly, enum e_sc storage_class);
LEXEME *nestedSearch(LEXEME *lex, SYMBOL **sym, SYMBOL **strSym, NAMESPACEVALUES **nsv, BOOLEAN *destructor, 
                     BOOLEAN *isTemplate, BOOLEAN tagsOnly, enum e_sc storage_class);
LEXEME *getIdName(LEXEME *lex, SYMBOL *funcsp, char *buf, int *ov, TYPE **castType);
LEXEME *id_expression(LEXEME *lex, SYMBOL *funcsp, SYMBOL **sym, SYMBOL **strSym, NAMESPACEVALUES **nsv, BOOLEAN *isTemplate, BOOLEAN tagsOnly, BOOLEAN membersOnly, char *name);
BOOLEAN isAccessible(SYMBOL *derived, SYMBOL *current, SYMBOL *member, SYMBOL *funcsp, enum e_ac minAccess, BOOLEAN asAddress);
BOOLEAN isExpressionAccessible(SYMBOL *sym, SYMBOL *funcsp, EXPRESSION *exp, BOOLEAN asAddress);
BOOLEAN checkDeclarationAccessible(TYPE *tp, SYMBOL *funcsp);
SYMBOL *LookupSym(char *name);
SYMBOL *lookupSpecificCast(SYMBOL *sp, TYPE *tp);
SYMBOL *lookupNonspecificCast(SYMBOL *sp, TYPE *tp);
SYMBOL *lookupIntCast(SYMBOL *sp, TYPE *tp);
SYMBOL *lookupArithmeticCast(SYMBOL *sp, TYPE *tp);
SYMBOL *lookupPointerCast(SYMBOL *sp, TYPE *tp);
SYMBOL *GetOverloadedTemplate(SYMBOL *sp, FUNCTIONCALL *args);
SYMBOL *GetOverloadedFunction(TYPE **tp, EXPRESSION **exp, SYMBOL *sp, 
                              FUNCTIONCALL *args, TYPE *atp, BOOLEAN toErr, 
                              BOOLEAN maybeConversion);
void insert(SYMBOL *in, HASHTABLE *table);
void insertOverload(SYMBOL *in, HASHTABLE *table);

                               /* Types.c */

BOOLEAN checktypeassign(TYPE *typ1, TYPE *typ2);
BOOLEAN comparetypes(TYPE *typ1, TYPE *typ2, int exact);
BOOLEAN matchingCharTypes(TYPE *typ1, TYPE *typ2);
void typenumptr(char *buf, TYPE *tp);
TYPE *typenum(char *buf, TYPE *tp);
void typeToString(char *buf, TYPE *typ);
