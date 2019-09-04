/* Protogen Version 1.00 Saturday November 18, 2006  21:08:23 */

                             /* Beinterf.c */

BE_IMODEDATA* beArgType(IMODE* in);
int sizeFromISZ(int isz);
int needsAtomicLockFromType(TYPE* tp);
int getSize(enum e_bt type);
int getBaseAlign(enum e_bt type);
long getautoval(long val);
int funcvaluesize(int val);
int alignment(int sc, TYPE* tp);
int getAlign(int sc, TYPE* tp);
const char* getUsageText(void);
const char* lookupRegName(int regnum);
KEYWORD* GetProcKeywords(void);
int init_backend(int* argc ,char* *argv);

                              /* Browse.c */

void abspath(char* name);
void browse_init(void);
void browse_startfunc(SYMBOL* func, int lineno);
void browse_endfunc(SYMBOL* func, int lineno);
void browse_startfile(const char* name, int filenum);
void browse_variable(SYMBOL* var);
void browse_define(char* name, int lineno, int charindex);
void browse_blockstart(int lineno);
void browse_blockend(int lineno);
void browse_usage(SYMBOL* var, int file);

                               /* Ccerr.c */
void DisableWarning(int num);
void EnableWarning(int num);
void WarningOnlyOnce(int num);
void WarningAsError(int num);
void AllWarningsAsError();
void AllWarningsDisable();
void PushWarnings();
void PopWarnings();
void DisableTrivialWarnings();
void errorinit(void);
void errskim(LEXEME* *lex, enum e_kw* skimlist);
void skip(LEXEME* *lex, enum e_kw kw);
bool needkw(LEXEME* *lex, enum e_kw kw);
void pperror(int err, int data);
void pperrorstr(int err, const char* str);
void preverror(int err, const char* name, const char* origfile, int origline);
void preverrorsym(int err, SYMBOL* sp, const char* origfile, int origline);
void getcls(char* buf, SYMBOL* clssym);
void errorat(int err, const char* name, char* file, int line);
void error(int err);
void errorabstract(int error, SYMBOL* sp);
void errorcurrent(int err);
void errorqualified(int err, SYMBOL* strSym, NAMESPACEVALUELIST* nsv, const char* name);
void errorNotMember(SYMBOL* strSym, NAMESPACEVALUELIST* nsv, const char* name);
void errorint(int err, int val);
void errorstr(int err, const char* val);
void errorstr2(int err, const char* val, const char* two);
void errorstringtype(int err, char* str, TYPE* tp1);
void errorsym(int err, SYMBOL* sp);
void errorsym2(int err, SYMBOL* sym1, SYMBOL* sym2);
void errorstrsym(int err, const char* name, SYMBOL* sym2);
void errortype (int err, TYPE* tp1, TYPE* tp2);
void membererror(char* name, TYPE* tp1);
void errorarg(int err, int argnum, SYMBOL* declsp, SYMBOL* funcsp);
void specerror(int err, const char* name, const char* file, int line);
void diag(const char* fmt, ...);
void printToListFile(const char* fmt, ...);
void ErrorsToListFile(void);
void AddErrorToList(const char* tag, const char* str);
void checkUnlabeledReferences(BLOCKDATA* parent);
void checkGotoPastVLA(STATEMENT* stmt, bool first);
void checkUnused(HASHTABLE* syms);
void findUnusedStatics(NAMESPACEVALUELIST* syms);
void assignmentUsages(EXPRESSION* node, bool first);
void checkDefaultArguments(SYMBOL* spi);
int TotalErrors();
                              /* Ccmain.c */

void bool_setup(char select, char* string);
void codegen_setup(char select, char* string);
void optimize_setup(char select, char* string);
void compile(bool global);
int main(int argc, char* argv[]);

                             /* Constopt.c */

void constoptinit(void);
ULLONG_TYPE CastToInt(int size, LLONG_TYPE value);
FPF CastToFloat(int size, FPF* value);
FPF* IntToFloat(FPF* temp, int size, LLONG_TYPE value);
FPF refloat(EXPRESSION* node);
LLONG_TYPE MaxOut(enum e_bt size, LLONG_TYPE value);
ULLONG_TYPE reint(EXPRESSION* node);
void dooper(EXPRESSION* *node, int mode);
int pwrof2(LLONG_TYPE i);
LLONG_TYPE mod_mask(int i);
void addaside(EXPRESSION* node);
int opt0(EXPRESSION* *node);
void enswap(EXPRESSION* *one, EXPRESSION* *two);
int fold_const(EXPRESSION* node);
int typedconsts(EXPRESSION* node1);
void optimize_for_constants(EXPRESSION* *expr);
LEXEME* optimized_expression(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, 
	EXPRESSION* *expr, bool commaallowed);

                               /* Debug.c */

void displayLexeme(LEXEME* lex);

                              /* Declare.c */
int CountPacks(TEMPLATEPARAMLIST* packs);
void calculateVirtualBaseOffsets(SYMBOL* sp);


SYMBOL* finishSearch(const char* name, SYMBOL* encloser, NAMESPACEVALUELIST* ns, bool tagsOnly, bool throughClass,
                     bool namespaceOnly);

bool intcmp(TYPE* t1, TYPE* t2);
EXPRESSION* GetSymRef(EXPRESSION* n);
EXPRESSION* copy_expression(EXPRESSION* exp);
bool equalTemplateIntNode(EXPRESSION* exp1, EXPRESSION* exp2);
bool matchTemplateSpecializationToParams(TEMPLATEPARAMLIST* param, TEMPLATEPARAMLIST* special, SYMBOL* sp);

bool templatecomparetypes(TYPE* tp1, TYPE* tp2, bool exact);
bool templatecompareexpressions(EXPRESSION* exp1, EXPRESSION* exp2);
bool templateselectorcompare(TEMPLATESELECTOR* ts1, TEMPLATESELECTOR* ts2);
TEMPLATEPARAMLIST* TemplateLookupSpecializationParam(const char* name);
bool hasVTab(SYMBOL* sp);
bool sameTemplatePointedTo(TYPE* tnew, TYPE* told);
bool sameTemplate(TYPE* P, TYPE* A);
void ParseBuiltins(void);
LEXEME* tagsearch(LEXEME* lex, char* name, SYMBOL** rsp, HASHTABLE** table, SYMBOL** strSym_out, NAMESPACEVALUELIST** nsv_out,
                         enum e_sc storage_class);
TYPE* AttributeFinish(SYMBOL* sp, TYPE* tp);
bool ParseAttributeSpecifiers(LEXEME* *lex, SYMBOL* funcsp, bool always);
void declare_init(void);
void addStructureDeclaration(STRUCTSYM* decl);
void addTemplateDeclaration(STRUCTSYM* decl);
void dropStructureDeclaration(void);
SYMBOL* getStructureDeclaration(void);
void FinishStruct(SYMBOL* sp, SYMBOL* funcsp);
LEXEME* innerDeclStruct(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, bool isTemplate, enum e_ac defaultAccess, bool isfinal, bool* defd);
void checkPackedType(SYMBOL* sp);
void checkPackedExpression(EXPRESSION* sym);
void checkUnpackedExpression(EXPRESSION* sym);
void GatherPackedVars(int* count, SYMBOL* *arg, EXPRESSION* packedExp);
INITLIST* *expandPackedInitList(INITLIST* *lptr, SYMBOL* funcsp, LEXEME* start, EXPRESSION* packedExp);
MEMBERINITIALIZERS* expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, MEMBERINITIALIZERS* *init, BASECLASS* bc, VBASEENTRY* vbase);
void expandPackedMemberInitializers(SYMBOL* cls, SYMBOL* funcsp, TEMPLATEPARAMLIST* templatePack, MEMBERINITIALIZERS* *p, LEXEME* start, INITLIST* list);
LEXEME* parse_declspec(LEXEME* lex, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3);
void checkOperatorArgs(SYMBOL* sp, bool asFriend);
SYMBOL* getCopyCons(SYMBOL* base, bool move);
void ConsDestDeclarationErrors(SYMBOL* sp, bool notype);
MEMBERINITIALIZERS* GetMemberInitializers(LEXEME* *lex, SYMBOL* funcsp, SYMBOL* sp);
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons);
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl);
void lambda_init(void);
bool matchesCopy(SYMBOL* sp, bool move);
void createConstructorsForLambda(SYMBOL* sp);
void createConstructor(SYMBOL* sp, SYMBOL* consfunc);
void createDefaultConstructors(SYMBOL* sp);
EXPRESSION* destructLocal(EXPRESSION* exp);
void destructBlock(EXPRESSION* *exp, SYMLIST* hr, bool mainDestruct);
EXPRESSION* thunkConstructorHead(BLOCKDATA* b, SYMBOL* sp, SYMBOL* cons, HASHTABLE* syms, bool parseInitializers, bool doCopy);
void thunkDestructorTail(BLOCKDATA* b, SYMBOL* sp, SYMBOL* dest, HASHTABLE* syms);
void createAssignment(SYMBOL* sp, SYMBOL* asnfunc);
void makeArrayConsDest(TYPE* *tp, EXPRESSION* *exp, SYMBOL* cons, SYMBOL* dest, EXPRESSION* count);
void callDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION* *exp, EXPRESSION* arrayElms, bool top, 
                    bool pointer, bool skipAccess);
bool callConstructor(TYPE* *tp, EXPRESSION* *exp, FUNCTIONCALL* params, 
                    bool checkcopy, EXPRESSION* arrayElms, bool top, 
                    bool maybeConversion, bool implicit, bool pointer,
                    bool usesInitList, bool useAssign);
bool callConstructorParam(TYPE** tp, EXPRESSION** exp, TYPE* paramTP, EXPRESSION* paramExp, bool top,
    bool maybeConversion, bool implicit, bool pointer);
void PromoteConstructorArgs(SYMBOL* cons1, FUNCTIONCALL* params);

LEXEME* getDeclType(LEXEME* lex, SYMBOL* funcsp, TYPE* *tn);
LEXEME* insertNamespace(LEXEME* lex, enum e_lk linkage, enum e_sc storage_class, bool* linked);
LEXEME* insertUsing(LEXEME* lex, SYMBOL* *sym, enum e_ac access, enum e_sc storage_class, bool inTemplate, bool hasAttribs);
LEXEME* handleStaticAssert(LEXEME* lex);
void InsertExtern(SYMBOL* sp);
void InsertGlobal(SYMBOL* sp);
const char* AnonymousName(void);
SYMBOL* makeID(enum e_sc storage_class, TYPE* tp, SYMBOL* spi, const char* name);
SYMBOL* makeUniqueID(enum e_sc storage_class, TYPE* tp, SYMBOL* spi, const char* name);
void InsertSymbol(SYMBOL* sp, enum e_sc storage_class, enum e_lk linkage, bool ignoreDups);
LEXEME* getDeferredData(LEXEME* lex, LEXEME **savePos, bool braces);
LEXEME* get_type_id(LEXEME* lex, TYPE* *tp, SYMBOL* funcsp, enum e_sc storage_class, bool beforeOnly, bool toErr);
int classRefCount(SYMBOL* base, SYMBOL* derived);
void CheckCalledException(SYMBOL* cst, EXPRESSION* exp);
void calculateVTabEntries(SYMBOL* sp, SYMBOL* base, VTABENTRY* *pos, int offset);
void HandleDeferredCompiles(void);
void deferredCompileOne(SYMBOL* cur);
void deferredCompile(void);
void deferredInitializeStructFunctions(SYMBOL* cur);
void deferredInitializeStructMembers(SYMBOL* cur);
TYPE*  PerformDeferredInitialization (TYPE* tp, SYMBOL* funcsp);
void warnCPPWarnings(SYMBOL* sp, bool localClassWarnings);
bool usesVTab(SYMBOL* sp);
LEXEME* baseClasses(LEXEME* lex, SYMBOL* funcsp, SYMBOL* declsym, enum e_ac defaultAccess);
void resolveAnonymousUnions(SYMBOL* sp);
SYMBOL*  calculateStructAbstractness(SYMBOL* top, SYMBOL* sp);
LEXEME* getFunctionParams(LEXEME* lex, SYMBOL* funcsp, SYMBOL* *spin, TYPE* *tp, bool inTemplate, enum e_sc storage_class);
LEXEME* getQualifiers(LEXEME* lex, TYPE* *tp, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3, bool* asFriend);
LEXEME* getBasicType(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, SYMBOL* *strSym, bool isTemplate, enum e_sc storage_class, 
					enum e_lk* linkage_in, enum e_lk* linkage2_in, enum e_lk* linkage3, 
                    enum e_ac access, bool* notype, bool* defd, int* consdest, 
                    bool* templateArg, bool isTypedef, bool templateErr);
LEXEME* getBeforeType(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, SYMBOL* *spi, SYMBOL* *strSym,
                      NAMESPACEVALUELIST* *nsv, bool isTemplate, enum e_sc storage_class,
							 enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3, bool asFriend,
                        int consdest, bool beforeOnly, bool funcptr);
void sizeQualifiers(TYPE* tp);
void SetParams(SYMBOL* cons);
void unvisitUsingDirectives(NAMESPACEVALUELIST* v);
void injectThisPtr(SYMBOL* sp, HASHTABLE* syms);
void TemplateValidateSpecialization(TEMPLATEPARAMLIST* arg);
void ScrubTemplateValues(SYMBOL*func);
TYPE* LookupTypeFromExpression(EXPRESSION* exp, TEMPLATEPARAMLIST* enclosing, bool alt);
TYPE* TemplateLookupTypeFromDeclType(TYPE* tp);
void propagateTemplateDefinition(SYMBOL* sp);
int PushTemplateNamespace(SYMBOL* sp);
void PopTemplateNamespace(int count);
bool typeHasTemplateArg(TYPE* t);
void templateInit(void);
void TemplateGetDeferred(SYMBOL* sp);
TEMPLATEPARAMLIST* TemplateGetParams(SYMBOL* sp);
void TemplateRegisterDeferred(LEXEME* lex);
bool exactMatchOnTemplateArgs(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym);
bool exactMatchOnTemplateParams(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym);
bool TemplateInstantiationMatch(SYMBOL* orig, SYMBOL* sp);
SYMBOL* FindSpecialization(SYMBOL* sp, TEMPLATEPARAMLIST* templateParams);
SYMBOL* LookupSpecialization(SYMBOL* sp, TEMPLATEPARAMLIST* templateParams);
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp);
TEMPLATEPARAMLIST*  TemplateMatching(LEXEME* lex, TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym, SYMBOL* sp, bool definition);
LEXEME* GetTemplateArguments(LEXEME* lex, SYMBOL* funcsp, SYMBOL* templ, TEMPLATEPARAMLIST* *lst);
bool TemplateIntroduceArgs(TEMPLATEPARAMLIST* sym, TEMPLATEPARAMLIST* args);
TYPE* SynthesizeType(TYPE* tp,TEMPLATEPARAMLIST* enclosing,  bool alt);
SYMBOL* SynthesizeResult(SYMBOL* sp, TEMPLATEPARAMLIST* params);
SYMBOL* TemplateSynthesizeFunction(SYMBOL* sp);
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sp, TYPE* tp);
void NormalizePacked(TYPE* tpo);
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sp, FUNCTIONCALL* args);
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sp);
void TemplatePartialOrdering(SYMBOL* *table, int count, FUNCTIONCALL* funcparams, TYPE* atype, bool byClass, bool save);
SYMBOL* TemplateClassInstantiateInternal(SYMBOL* sp, TEMPLATEPARAMLIST* args, bool isExtern);
SYMBOL* TemplateClassInstantiate(SYMBOL* sp, TEMPLATEPARAMLIST* args, bool isExtern, enum e_sc storage_class);
void TemplateDataInstantiate(SYMBOL* sp, bool warning, bool isExtern);
void SetTemplateNamespace(SYMBOL* sp);
void HandleDeferredFunctionInstantiate(void);
SYMBOL* TemplateFunctionInstantiate(SYMBOL* sp, bool warning, bool isExtern);
bool allTemplateArgsSpecified(SYMBOL* sp, TEMPLATEPARAMLIST* args);
SYMBOL* GetClassTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* args, bool noErr);
SYMBOL* GetVariableTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* args);
SYMBOL* GetTypedefSpecialization(SYMBOL* sp, TEMPLATEPARAMLIST* args);
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL* *sym, NAMESPACEVALUELIST* nsv, SYMBOL* strSym, TEMPLATEPARAMLIST* templateParams, bool isExtern);
void DoDefaultSpecialization(SYMBOL* sp2);
TEMPLATEPARAMLIST* getCurrentSpecialization(SYMBOL* sp);
bool TemplateFullySpecialized(SYMBOL* sp);
LEXEME* TemplateDeclaration(LEXEME* lex, SYMBOL* funcsp, enum e_ac access, enum e_sc storage_class, bool isextern);
bool isConstexprConstructor(SYMBOL* sp);
bool MatchesConstFunction(SYMBOL* sp);
void CollapseReferences(TYPE *tp_in);
LEXEME* declare(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, enum e_sc storage_class, enum e_lk defaultLinkage,
					   BLOCKDATA* parent, bool needsemi, int asExpression, bool isTemplate, enum e_ac access );

                               /* Expr.c */

void expr_init(void);
void libcxx_init(void);
bool parseBuiltInTypelistFunc(LEXEME* *lex, SYMBOL* funcsp, SYMBOL* sp, TYPE* *tp, EXPRESSION* *exp);
void thunkForImportTable(EXPRESSION* *exp);
void checkscope(TYPE* tp1, TYPE* tp2);
void checkauto(TYPE* tp, int err);
void qualifyForFunc(SYMBOL* sp, TYPE* *tp, bool isMutable);
void getThisType(SYMBOL* sp, TYPE* *tp);
SYMBOL* lambda_capture(SYMBOL* sp, enum e_cm mode, bool isExplicit);
LEXEME* expression_lambda(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, int flags);
EXPRESSION* getMemberBase(SYMBOL* memberSym, SYMBOL* strSym, SYMBOL* funcsp, bool toError);
EXPRESSION* getMemberNode(SYMBOL* memberSym, SYMBOL* strSym, TYPE* *tp, SYMBOL* funcsp);
EXPRESSION* getMemberPtr(SYMBOL* memberSym, SYMBOL* strSym, TYPE* *tp, SYMBOL* funcsp);
EXPRESSION* substitute_params_for_function(FUNCTIONCALL* funcparams, HASHTABLE* syms);
EXPRESSION* substitute_params_for_constexpr(EXPRESSION* exp, FUNCTIONCALL* funcparams, HASHTABLE* syms);
LEXEME* expression_func_type_cast(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp, int flags);
bool doDynamicCast(TYPE* *newType, TYPE* oldTYPE, EXPRESSION* *exp, SYMBOL* funcsp);
bool doStaticCast(TYPE* *newType, TYPE* oldTYPE, EXPRESSION* *exp, SYMBOL* funcsp, bool checkconst);
bool doConstCast(TYPE* *newType, TYPE* oldTYPE, EXPRESSION* *exp, SYMBOL* funcsp);
bool doReinterpretCast(TYPE* *newType, TYPE* oldTYPE, EXPRESSION* *exp, SYMBOL* funcsp, bool checkconst);
bool castToPointer(TYPE* *tp, EXPRESSION* *exp, enum e_kw kw, TYPE* other);
bool castToArithmeticInternal(bool integer, TYPE* *tp, EXPRESSION* *exp, enum e_kw kw, TYPE* other, bool implicit);
void castToArithmetic(bool integer, TYPE* *tp, EXPRESSION* *exp, enum e_kw kw, TYPE* other, bool implicit);
bool cppCast(TYPE* src, TYPE* *dest, EXPRESSION* *exp);
LEXEME* GetCastInfo(LEXEME* lex, SYMBOL* funcsp, TYPE* *newType, TYPE* *oldType, EXPRESSION* *oldExp, bool packed);
LEXEME* expression_typeid(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp, int flags);
bool insertOperatorParams(SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp, FUNCTIONCALL* funcParams, int flags);
bool insertOperatorFunc(enum ovcl cls, enum e_kw kw, SYMBOL* funcsp, 
                        TYPE* *tp, EXPRESSION* *exp, TYPE* tp1, EXPRESSION* exp1, INITLIST* args, int flags);
LEXEME* expression_new(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp, bool global, int flags);
LEXEME* expression_delete(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp, bool global, int flags);
LEXEME* expression_noexcept(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp);
void ResolveTemplateVariable(TYPE* *ttype, EXPRESSION* *texpr, TYPE* atype, TYPE* rtype);
LEXEME* expression_cast(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, bool* ismutable, int flags);
EXPRESSION* exprNode(enum e_node type, EXPRESSION* left, EXPRESSION* right);
EXPRESSION* varNode(enum e_node type, SYMBOL* sp);
EXPRESSION* intNode(enum e_node type, LLONG_TYPE val);
EXPRESSION* typeNode(TYPE* tp);
bool doBox(TYPE* *tp, EXPRESSION* *exp, TYPE* boxed, TYPE* unboxed);
bool doUnbox(TYPE* *tp, EXPRESSION* *exp, TYPE* boxed, TYPE* unboxed);
EXPRESSION* baseClassOffset(SYMBOL* base, SYMBOL* derived, EXPRESSION* en);
LEXEME* getInitList(LEXEME* lex, SYMBOL* funcsp, INITLIST* *owner);
LEXEME* getArgs(LEXEME* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, enum e_kw finish, bool allowPack, int flags);
LEXEME* getMemberInitializers(LEXEME* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, enum e_kw finish, bool allowPack);
EXPRESSION* DerivedToBase(TYPE* tpn, TYPE* tpo, EXPRESSION* exp, int flags);
void CreateInitializerList(TYPE* initializerListTemplate, TYPE* initializerListType, 
                           INITLIST* *lptr, bool operands, bool asref);
void AdjustParams(SYMBOL* func, SYMLIST* hr, INITLIST* *lptr, bool operands, bool implicit);
LEXEME* expression_arguments(LEXEME* lex, SYMBOL* funcsp, TYPE* *tp, EXPRESSION* *exp, int flags);
LEXEME* expression_unary(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, bool* ismutable, int flags);
LEXEME* expression_assign(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, bool* ismutable, int flags);
LEXEME* expression_comma(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, bool* ismutable, int flags);
LEXEME* expression_no_comma(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, bool* ismutable, int flags);
LEXEME* expression_no_check(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, int flags);
LEXEME* expression(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE* *tp, EXPRESSION* *exp, int flags);

                               /* Help.c */
void helpinit();
void deprecateMessage(SYMBOL* sp);
bool ismember(SYMBOL* sp);
bool istype(SYMBOL* sp);
bool ismemberdata(SYMBOL* sp);
bool startOfType(LEXEME* lex, bool assumeType);
void UpdateRootTypes(TYPE* tp);
bool isDerivedFromTemplate(TYPE* tp);
bool isautotype(TYPE* tp);
bool isunsigned(TYPE* tp);
bool isint(TYPE* tp);
bool isfloat(TYPE* tp);
bool iscomplex(TYPE* tp);
bool isimaginary(TYPE* tp);
bool isarithmetic(TYPE* tp);
bool ismsil(TYPE* tp);
bool isconstraw(const TYPE* tp, bool useTemplate);
bool isconst(const TYPE* tp);
bool isvolatile(const TYPE* tp);
bool islrqual(TYPE* tp);
bool isrrqual(TYPE* tp);
bool isrestrict(TYPE* tp);
bool isatomic(TYPE* tp);
bool isvoid(TYPE* tp);
bool isvoidptr(TYPE* tp);
bool isarray(TYPE* tp);
bool isunion(TYPE* tp);
void DeduceAuto(TYPE* *pat, TYPE* nt);
SYMBOL* getFunctionSP(TYPE* *tp);
LEXEME* concatStringsInternal(LEXEME* lex, STRING* *str, int* elems);
LEXEME* concatStrings(LEXEME* lex, EXPRESSION* *exp, enum e_lexType* tp, int* elems);
bool isintconst(EXPRESSION* exp);
bool isfloatconst(EXPRESSION* exp);
bool isimaginaryconst(EXPRESSION* exp);
bool iscomplexconst(EXPRESSION* exp);
EXPRESSION* anonymousVar(enum e_sc storage_class, TYPE* tp);
void deref(TYPE* tp, EXPRESSION* *exp);
int sizeFromType(TYPE* tp);
void cast(TYPE* tp, EXPRESSION* *exp);
bool castvalue(EXPRESSION* exp);
bool lvalue(EXPRESSION* exp);
bool xvalue(EXPRESSION* exp);
EXPRESSION* convertInitToExpression(TYPE* tp, SYMBOL* sp, SYMBOL* funcsp, INITIALIZER* init, EXPRESSION* thisptr, bool isdest);
bool assignDiscardsConst(TYPE* dest, TYPE* source);
bool isconstzero(TYPE* tp, EXPRESSION* exp);
bool fittedConst(TYPE* tp, EXPRESSION* exp);
bool isarithmeticconst(EXPRESSION* exp);
bool isconstaddress(EXPRESSION* exp);
SYMBOL* clonesym(SYMBOL* sp);
TYPE* destSize(TYPE* tp1, TYPE* tp2, EXPRESSION* *exp1, EXPRESSION* *exp2, bool minimizeInt, TYPE* atp);
EXPRESSION* RemoveAutoIncDec(EXPRESSION* exp);
LLONG_TYPE imax(LLONG_TYPE x, LLONG_TYPE y);
LLONG_TYPE imin(LLONG_TYPE x, LLONG_TYPE y);
void my_sprintf(char* dest, const char* fmt, ...);


							  /* IAlias.c */
void AliasInit(void);
void AliasRundown(void);
void AliasPass1(void);
void AliasPass2(void);
void AliasStruct(BITINT* bits, IMODE* ans, IMODE* left, IMODE* right);
void AliasGosub(QUAD* ins, BITINT* parms, BITINT* bits, int n);
void AliasUses(BITINT* bit, IMODE* im, bool rhs);

                              /* Iblock.c */

int equalnode(EXPRESSION* node1, EXPRESSION* node2);
int equalimode(IMODE* ap1, IMODE* ap2);
short dhash(UBYTE* str, int len);
QUAD* LookupNVHash(UBYTE* key, int size, DAGLIST* *table);
DAGLIST* ReplaceHash(QUAD* rv, UBYTE* key, int size, DAGLIST* *table);
QUAD* liveout(QUAD* node);
int ToQuadConst(IMODE* *im);
void dag_rundown(void);
BLOCKLIST* newBlock(void);
void addblock(int val);
bool usesAddress(IMODE* im);
void gen_label(int labno);
void flush_dag(void);
QUAD*  gen_icode_with_conflict(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, bool conflicting);
QUAD*  gen_icode(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
void gen_iiconst(IMODE* res, LLONG_TYPE val);
void gen_ifconst(IMODE* res, FPF val);
void gen_igoto(enum i_ops op, long label);
void gen_data(int val);
void gen_icgoto(enum i_ops op, long label, IMODE* left, IMODE* right);
QUAD* gen_igosub(enum i_ops op, IMODE* left);
void gen_icode2(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, int label);
void gen_line(LINEDATA* data);
void gen_asm(STATEMENT* stmt);
void gen_asmdata(STATEMENT* stmt);
void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
void RemoveFromUses(QUAD* ins, int tnum);
void InsertUses(QUAD* ins, int tnum);
void RemoveInstruction(QUAD* ins);
void InsertInstruction(QUAD* before, QUAD* ins);

                               /* Icom.c */

void commondelete(QUAD* quad);

							   /* iconfl.c */

void conflictini(void);
void resetConflict(void);
int findPartition(int T0);
void insertConflict(int i, int j);
void JoinConflictLists(int T0, int T1);
bool isConflicting(int T0, int T1);
void CalculateConflictGraph(BRIGGS_SET* globals, bool optimize);

                              /* Iconst.c */

void ConstantFold(QUAD* d, bool reflow);
void ConstantFlow(void);

                               /* Iexpr.c */

void iexpr_init(void);
void iexpr_func_init(void);
IMODE* GetLoadTemp(IMODE* dest);
IMODE* LookupLoadTemp(IMODE* dest, IMODE* source);
int chksize(int lsize, int rsize);
SYMBOL* varsp(EXPRESSION* node);
void DumpIncDec(SYMBOL* funcsp);
IMODE* make_imaddress(EXPRESSION* node, int size);
IMODE* make_bf(EXPRESSION* node, IMODE* ap, int size);
IMODE* make_immed(int size, LLONG_TYPE i);
IMODE* make_fimmed(int size,FPF f);
IMODE* make_parmadj(long i);
IMODE* make_ioffset(EXPRESSION* node);
IMODE* genasn(EXPRESSION* node, SYMBOL* funcsp, int size);
IMODE* indnode(IMODE* ap1, int size);
IMODE* LookupExpression(enum i_ops op, int size, IMODE* left, IMODE* right);
IMODE* gen_deref(EXPRESSION* node, SYMBOL* funcsp, int flags);
IMODE* gen_unary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_asrhd(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_binary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_pdiv(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_pmul(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_hook(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_moveblock(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_clearblock(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_assign(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_aincdec(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_tcall(EXPRESSION* node, SYMBOL* funcsp, int flags);
IMODE* gen_trapcall(SYMBOL* funcsp, EXPRESSION* node, int flags);
IMODE* gen_stmt_from_expr(SYMBOL* funcsp, EXPRESSION* node, int flags);
IMODE* gen_funccall(SYMBOL* funcsp, EXPRESSION* node, int flags);
IMODE* doatomicFence(SYMBOL*  funcsp, EXPRESSION* parent, EXPRESSION* node, IMODE* barrier);
IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_void(EXPRESSION* node, SYMBOL* funcsp);
int natural_size(EXPRESSION* node);
void gen_compare(EXPRESSION* node, SYMBOL* funcsp, int btype, int label);
IMODE* gen_set(EXPRESSION* node, SYMBOL* funcsp, int btype);
IMODE* defcond(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp);
void truejp(EXPRESSION* node, SYMBOL* funcsp, int label);
void falsejp(EXPRESSION* node, SYMBOL* funcsp, int label);

                               /* Iflow.c */

void flow_init(void);
void dump_dfst(void);
void create_dfst(int n);
enum e_fgtype getEdgeType(int first, int second);
void unlinkBlock(BLOCK* succ, BLOCK* pred);
void UnlinkCritical(BLOCK* s);
void reflowConditional(BLOCK* src, BLOCK* dst);
void doms_only(bool always);
void flows_and_doms(void);
bool dominatedby(BLOCK* src, BLOCK* ancestor);
void WalkFlowgraph(BLOCK* b, 
				   int (*func)(enum e_fgtype type, BLOCK* parent, BLOCK* b), int fwd);


							  /* iglobal.c */

void LocalOptimization(void);

							  /* iinline.c */

void iinlineInit(void);
void dumpImportThunks(void);
void dumpvc1Thunks(void);
IMODE* gen_inline(SYMBOL* funcsp, EXPRESSION* node, int flags);

							  /* iinvar.c */

void MoveLoopInvariants(void);

                              /* Ilazy.c */
void RearrangePrecolors(void);
void SetGlobalTerms(void);
void GlobalOptimization(void);

								/* ilimit.c */

void limitInit(void);
QUAD* beforeJmp(QUAD* I, bool before);
void LimitRegisterPressure(void);

                               /* Ilive.c */

void liveVariables(void);

                              /* Ilocal.c */

void usesInfo(void);
void definesInfo(void);
IMODE* InitTempOpt(int size1, int size2);
void TransferInlineTemps(void);
void gatherLocalInfo(SYMBOL* funcsp);
void removeDead(BLOCK* b);
								/* iloop.c */

void CancelInfinite(int orgBlockCount);
void BuildLoopTree(void);
bool isAncestor(LOOP* l1, LOOP* l2);
void RemoveInfiniteThunks(void);
void CalculateInduction(void);

                               /* Init.c */

void init_init(void);
void dumpStartups(void);
void dumpCPPStaticsAndTLS();
void insertStartup(bool startupFlag, char* name, int prio);
const char* lookupAlias(const char* name);
void insertAlias(char* name, char* alias);
void insertDynamicInitializer(SYMBOL* sp, INITIALIZER* init);
void insertDynamicDestructor(SYMBOL* sp, INITIALIZER* init);
int dumpMemberPtr(SYMBOL* sp, TYPE* membertp, bool make_label);
void dumpTemplateInitializers(void);
int dumpInit(SYMBOL* sp, INITIALIZER* init);
void dumpInitGroup(SYMBOL* sp, TYPE* tp);
void dumpInitializers(void);
void dumpVTab(SYMBOL* sp);
void insertInitSym(SYMBOL* sp);
bool IsConstWithArr(TYPE* tp);
enum e_node referenceTypeError(TYPE* tp, EXPRESSION* exp);
EXPRESSION* createTemporary(TYPE* tp, EXPRESSION* val);
EXPRESSION* msilCreateTemporary(TYPE* tp, EXPRESSION* val);
INITIALIZER* initInsert(INITIALIZER* *pos, TYPE* tp, EXPRESSION* exp, 
                               int offset, bool noassign);
LEXEME* initType(LEXEME* lex, SYMBOL* funcsp, int offset, enum e_sc sc, 
				 INITIALIZER* *init, INITIALIZER* *dest, TYPE* itype, SYMBOL* sp, bool arrayMember, int flags);
bool IsConstantExpression(EXPRESSION* node, bool allowParams, bool allowFunc);
EXPRESSION* getThisNode(SYMBOL* sp);
void RecalculateVariableTemplateInitializers(INITIALIZER* *in, INITIALIZER* **out, TYPE* tp, int ioffset);
LEXEME* initialize(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, enum e_sc storage_class_in, bool uninitconst, int flags);
LEXEME* initialize_property(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, enum e_sc storage_class_in, bool asExpression, int flags);

                              /* Inline.c */

void inlineinit(void);
void dumpInlines(void);
void ReferenceVTab(SYMBOL* sp);
void InsertInline(SYMBOL* sp);
void InsertInlineData(SYMBOL* sp);
SYMBOL* getvc1Thunk(int offset);
SYMBOL* inlineSearch(SYMBOL* name);
void inlineinsert(SYMBOL* sp);
EXPRESSION* inlineexpr(EXPRESSION* node, bool* fromlval);
STATEMENT* inlinestmt(STATEMENT* block);
STATEMENT* inlinefuncargs(FUNCTIONCALL* params);
void inlinereblock(SYMBOL* sp);
EXPRESSION* doinline(FUNCTIONCALL* params, SYMBOL* funcsp);
bool IsEmptyFunction(FUNCTIONCALL* params, SYMBOL* funcsp);

                             /* Ioptutil.c */

BRIGGS_SET* briggsAlloc(int size);
BRIGGS_SET* briggsAlloct(int size);
BRIGGS_SET* briggsAllocs(int size);
BRIGGS_SET* briggsAllocc(int size);
BRIGGS_SET* briggsReAlloc(BRIGGS_SET* set, int size);
int briggsSet(BRIGGS_SET* p, int index);
int briggsReset(BRIGGS_SET* p, int index);
int briggsTest(BRIGGS_SET* p, int index);
int briggsUnion(BRIGGS_SET* s1, BRIGGS_SET* s2);
int briggsIntersect(BRIGGS_SET* s1, BRIGGS_SET* s2);

                               /* Iout.c */

void outcodeini(void);
void BitInit(void);
void beDecorateSymName(char* buf, SYMBOL* sp);
void putconst(EXPRESSION* offset, int color);
void putlen(int l);
void putamode(QUAD* q, IMODE* ap);
void put_code(QUAD* q);
int beVariableLive(IMODE* m);
void rewrite_icode(void);
void genbyte(long val);
void genbool(int val);
void gen_vtt(VTABENTRY* entry, SYMBOL* func, SYMBOL* name);
void gen_strlab(SYMBOL* sp);
void gen_importThunk(SYMBOL* func);
void gen_vc1(SYMBOL* func);
void put_label(int lab);
void put_staticlabel(long label);
void genfloat(FPF* val);
void gendouble(FPF* val);
void genlongdouble(FPF* val);
void genUBYTE(long val);
void genbool(int val);
void genbit(SYMBOL* sp, int val);
void genshort(long val);
void genwchar_t(long val);
void genlong(long val);
void genlonglong(LLONG_TYPE val);
void genint(int val);
void genuint16(int val);
void genuint32(int val);
void genenum(int val);
int genstring(STRING* str);
void genaddress(ULLONG_TYPE address);
void gensrref(SYMBOL* sp, int val, int prio);
void genlabref(int label);
void genref(SYMBOL* sp, int offset);
void genpcref(SYMBOL* sp, int offset);
void localdef(SYMBOL* sp);
void localstaticdef(SYMBOL* sp);
void genstorage(int nUBYTEs);
void gen_labref(int n);
void gen_labdifref(int n1, int n2);
EXPRESSION* stringlit(STRING* s);
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
void gen_virtual(SYMBOL* sp, int data);
void gen_endvirtual(SYMBOL* sp);
void align(int size);
void asm_header(const char* name, const char* version);
void asm_trailer(void);
void globaldef(SYMBOL* sp);
int put_exfunc(SYMBOL* sp, int notyet);
void put_expfunc(SYMBOL* sp);
void putexterns(void);

                               /* Ipeep.c */

void peepini(void);
void weed_goto(void);
void peep_icode(bool branches);

								/*irc.c */

void regInit(void);
void alloc_init(void);
void cacheTempSymbol(SYMBOL* sp);
void AllocateStackSpace(SYMBOL* funcsp);
void FillInPrologue(QUAD* head, SYMBOL* funcsp);
void Precolor(void);
void AllocateRegisters(QUAD* head);

							   /* Ireshape */

bool variantThisLoop(BLOCK* b, int tnum);
void ReplaceHashReshape(QUAD* rv, UBYTE* key, int size, DAGLIST* *table);
void InsertReshapeInstruction(QUAD* before, QUAD* ins);
bool matchesop(enum i_ops one, enum i_ops two);
void Reshape(void);

							   /* Irewrite */
void Prealloc(int pass);
void RewriteLoopEntries(void);
void CalculateBackendLives(void);


                                /* Issa.c */

void SSAInit(void);
void AddToWipeList(DAGLIST* head);
void TranslateToSSA(void);
void RemoveCriticalThunks(void);
void TranslateFromSSA(bool all);

                               /* Istmt.c */

void genstmtini(void);
void makeXCTab(SYMBOL* funcsp);
EXPRESSION* tempenode(void);
IMODE* tempregpure(int size, int mode);
IMODE* tempreg(int size, int mode);
IMODE* imake_label(int label);
IMODE* make_direct(int i);
void genwhile(STATEMENT* stmt, SYMBOL* funcsp);
void gen_for(STATEMENT* stmt, SYMBOL* funcsp);
void genif(STATEMENT* stmt, SYMBOL* funcsp);
void gendo(STATEMENT* stmt, SYMBOL* funcsp);
void gen_genword(STATEMENT* stmt, SYMBOL* funcsp);
IMODE* set_symbol(const char* name, int isproc);
IMODE* call_library(const char* lib_name, int size);
void count_cases(CASEDATA* cd, struct cases* cs);
void gather_cases(CASEDATA* cd, struct cases* cs);
int gcs_compare(void const* left, void const* right);
void genxswitch(STATEMENT* stmt, SYMBOL* funcsp);
void genreturn(STATEMENT* stmt, SYMBOL* funcsp, int flag, int noepilogue, IMODE* allocaAP);
void gen_tryblock(void* val);
IMODE* genstmt(STATEMENT* stmt, SYMBOL* funcsp);
SYMBOL* gen_mp_virtual_thunk(SYMBOL* vsp);
SYMBOL* gen_vsn_virtual_thunk(SYMBOL* func, int ofs);
void optimize(SYMBOL* funcsp);
void genfunc(SYMBOL* funcsp, bool  optimize);
void genASM(STATEMENT* st);

							  /* istren.c */

void unmarkPreSSA(QUAD* ins);
void ReduceLoopStrength(void);

                                /* Lex.c */

void lexini(void);
KEYWORD* searchkw(const char* *p);
int getChar(unsigned char* *source, enum e_lexType* tp);
SLCHAR* getString(unsigned char* *source, enum e_lexType* tp);
int getId(unsigned char* *ptr , unsigned char* dest);
LEXEME* SkipToNextLine(void);
LEXEME* getGTSym(LEXEME* in);
LEXEME* getsym(void);
LEXEME* prevsym(LEXEME* lex);
LEXEME* backupsym(void);
LEXEME* SetAlternateLex(LEXEME* lexList);
bool CompareLex(LEXEME *left, LEXEME *right);
void SetAlternateParse(bool set, const std::string& alt);
void SkipToEol();
bool AtEol();
                               /* List.c */

void put_sc(int scl);
void put_ty(TYPE* tp);
void list_var(SYMBOL* sp, int i);
void list_table(HASHTABLE* t, int j);

                              /* Mangle.c */

void mangleInit();
char* mangleNameSpaces(char* in, SYMBOL* sp);
char* mangleType (char* in, TYPE* tp, bool first);
void SetLinkerNames(SYMBOL* sp, enum e_lk linkage);
const char* unmang1(char* buf, const char* name, const char* last, bool tof);
const char* unmang_intrins(char* buf, const char* name, const char* last);
char* unmangle(char* name, const char* source);

                              /* Memory.c */

void mem_summary(void);
void* memAlloc(MEMBLK* *arena, int size);
void memFree(MEMBLK* *arena, int* peak);
void* globalAlloc(int size);
void globalFree(void);
void* localAlloc(int size);
void localFree(void);
void* Alloc( int size);
void* oAlloc(int size);
void oFree(void);
void* aAlloc(int size);
void aFree(void);
void* tAlloc(int size);
void tFree(void);
void* cAlloc(int size);
void cFree(void);
void* sAlloc(int size);
void sFree(void);
void IncGlobalFlag(void);
void DecGlobalFlag(void);
void SetGlobalFlag(int flag);
int GetGlobalFlag(void);
char* litlate(const char* name);
LCHAR* wlitlate(const LCHAR* name);

                              /* Osutil.c */

void AddExt(char* buffer, const char* ext);
void StripExt(char* buffer);
void EXEPath(char* buffer, char* filename);
int HasExt(char* buffer, const char* ext);
void setglbdefs(void);
void InsertAnyFile(const char* filename, char* path, int drive, bool primary);
void setfile(char* buf, char* orgbuf, char* ext);
void outputfile(char* buf, const char* orgbuf, const char* ext);
int parse_arbitrary(char* string);
void parsefile(char select, char* string);
void addinclude(void);
int parseenv(char* name);
int parseconfigfile(char* name);
void dumperrs(FILE* file);
void ctrlchandler(int aa);
void internalError(int a);
void ccinit(int argc, char* argv[]);

                              /* Output.c */

void oinit(void);
void oflush(FILE* file);
void oputc(int ch, FILE* file);
void beputc(int ch);
void owrite(const char* buf, size_t size, int n, FILE* fil);
void beWrite(const char* buf, size_t size);
void oprintf(FILE* file, const char* format, ...);
void bePrintf(const char* format, ...);
void beRewind(void);
                              /* Ppexpr.c */

int getsch(int UBYTEs, unsigned char* *source) ;

                              /* Preproc.c */

void glbdefine(const char* name, const char* value);
void glbUndefine(const char* name);

                               /* Rtti.c */

void rtti_init(void);
SYMBOL*  RTTIDumpType(TYPE* tp);
void XTDumpTab(SYMBOL* funcsp);

				/* seh.c */

LEXEME* statement_SEH(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent);

                               /* Stmt.c */

void statement_ini(bool global);
void InsertLineData(int lineno, int fileindex, const char* fname, char* line);
void FlushLineData(const char* file, int lineno);
STATEMENT* currentLineData(BLOCKDATA* parent, LEXEME* lex, int offset);
STATEMENT* stmtNode(LEXEME* lex, BLOCKDATA* parent, enum e_stmt stype);
int insertLabel(BLOCKDATA* parent);
LEXEME* statement_asm(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent);
void assignParam(SYMBOL* funcsp, int* base, SYMBOL* param);
bool resolveToDeclaration(LEXEME* lex);
LEXEME* statement(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent, 
                           bool viacontrol);
LEXEME* compound(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent,   
                        bool first);
LEXEME* body (LEXEME* lex, SYMBOL* funcsp);

                              /* Symtab.c */

void syminit(void);
bool singlebase(SYMBOL* base, SYMBOL* of);
HASHTABLE* CreateHashTable(int size);
SYMLIST* AddName(SYMBOL* item, HASHTABLE* table);
void AllocateLocalContext(BLOCKDATA* parent, SYMBOL* sp, int label);
void FreeLocalContext(BLOCKDATA* parent, SYMBOL* sp, int label);
SYMLIST* *GetHashLink(HASHTABLE* t, const char* string);
SYMLIST* *LookupName(const char* name, HASHTABLE* table);
SYMBOL* classdata(const char* name, SYMBOL* cls, SYMBOL* last, bool isvirtual, bool tagsOnly);
SYMBOL* search(const char* name, HASHTABLE* table);
bool matchOverload(TYPE* tnew, TYPE* told, bool argsOnly);
SYMBOL* searchOverloads(SYMBOL* sp, HASHTABLE* table);
SYMBOL* gsearch(const char* name);
SYMBOL* tsearch(const char* name);
TEMPLATEPARAMLIST* getTemplateStruct(char* name);
LIST* tablesearchone(const char* name, NAMESPACEVALUELIST* ns, bool tagsOnly);
LIST* tablesearchinline(const char* name, NAMESPACEVALUELIST* ns, bool tagsOnly);
SYMBOL* namespacesearch(const char* name, NAMESPACEVALUELIST* ns, bool qualified, bool tagsOnly);
SYMBOL* templatesearch(const char* name, TEMPLATEPARAMLIST* arg);
SYMBOL* classsearch(const char* name, bool tagsOnly, bool toErr);
LEXEME* nestedPath(LEXEME* lex, SYMBOL* *sym, NAMESPACEVALUELIST* *ns, bool* throughPath, bool tagsOnly, enum e_sc storage_class, bool isType);
LEXEME* nestedSearch(LEXEME* lex, SYMBOL* *sym, SYMBOL* *strSym, NAMESPACEVALUELIST* *nsv, bool* destructor, 
                     bool* isTemplate, bool tagsOnly, enum e_sc storage_class, bool errIfNotFound, bool isType);
LEXEME* getIdName(LEXEME* lex, SYMBOL* funcsp, char* buf, int* ov, TYPE* *castType);
LEXEME* id_expression(LEXEME* lex, SYMBOL* funcsp, SYMBOL* *sym, SYMBOL* *strSym, NAMESPACEVALUELIST* *nsv, bool* isTemplate, bool tagsOnly, bool membersOnly, char* name);
bool isAccessible(SYMBOL* derived, SYMBOL* current, SYMBOL* member, SYMBOL* funcsp, enum e_ac minAccess, bool asAddress);
bool isExpressionAccessible(SYMBOL* derived, SYMBOL* sp, SYMBOL* funcsp, EXPRESSION* exp, bool asAddress);
bool checkDeclarationAccessible(TYPE* tp, SYMBOL* funcsp);
SYMBOL* LookupSym(char* name);
SYMBOL*lookupGenericConversion(SYMBOL*sym, TYPE *tp);
SYMBOL* lookupSpecificCast(SYMBOL* sp, TYPE* tp);
SYMBOL* lookupNonspecificCast(SYMBOL* sp, TYPE* tp);
SYMBOL* lookupIntCast(SYMBOL* sp, TYPE* tp, bool implicit);
SYMBOL* lookupArithmeticCast(SYMBOL* sp, TYPE* tp, bool implicit);
SYMBOL* lookupPointerCast(SYMBOL* sp, TYPE* tp);
void getSingleConversion(TYPE* tpp, TYPE* tpa, EXPRESSION* expa, int* n, enum e_cvsrn* seq, 
                                SYMBOL* candidate, SYMBOL* *userFunc, bool allowUser);
SYMBOL* GetOverloadedTemplate(SYMBOL* sp, FUNCTIONCALL* args);
SYMBOL* detemplate(SYMBOL* sp, FUNCTIONCALL* args, TYPE* atp);
SYMBOL* GetOverloadedFunction(TYPE* *tp, EXPRESSION* *exp, SYMBOL* sp, 
                              FUNCTIONCALL* args, TYPE* atp, bool toErr, 
                              bool maybeConversion, bool toInstantiate, int flags);
SYMBOL* MatchOverloadedFunction(TYPE* tp, TYPE* *mtp, SYMBOL* sp, EXPRESSION* *exp, int flags);
void baseinsert(SYMBOL* in, HASHTABLE* table);
void insert(SYMBOL* in, HASHTABLE* table);
SYMLIST* AddOverloadName(SYMBOL* item, HASHTABLE* table);
void insertOverload(SYMBOL* in, HASHTABLE* table);

                               /* Types.c */

bool checktypeassign(TYPE* typ1, TYPE* typ2);
bool comparetypes(TYPE* typ1, TYPE* typ2, int exact);
bool matchingCharTypes(TYPE* typ1, TYPE* typ2);
void typenumptr(char* buf, TYPE* tp);
TYPE* typenum(char* buf, TYPE* tp);
void typeToString(char* buf, TYPE* typ);

