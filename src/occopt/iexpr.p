
/* Protogen Version 3.0.1.6Wednesday October 09, 2019  19:56:32 */

const char *lookupRegName(int);
void InternalConflict(QUAD*);
void x86InternalConflict(QUAD* head);
void PreColor(QUAD* head);
void x86PreColor(QUAD* head);
int PreRegAlloc(QUAD* tail, BRIGGS_SET* globalVars, BRIGGS_SET* eobGlobals, int pass);
int x86PreRegAlloc(QUAD* ins, BRIGGS_SET* globalVars, BRIGGS_SET* eobGlobals, int pass);
const char *LookupAlias(const char* name);
std::list<std::string>& GetIncludeLibs();
void putstring(char *string, int len);
void putstring(SimpleExpression* offset);
void InitIntermediate();
bool InputIntermediate();
void OutputIntermediate();
void OutputIcdFile();
int msil_examine_icode(QUAD* head);
int x86_examine_icode(QUAD* head);
void SelectBackendData();
void SetUsesESP(bool yes);
void constoptinit();
int GetOutputSize();
void InsertExternal(struct sym*);

                             /* Ialias.cpp */

void AliasInit(void);
void AliasRundown(void); /* PROTOTYPE */
void AliasStruct(BITINT* bits, IMODE* ans, IMODE* left, IMODE* right);
void AliasGosub(QUAD* tail, BITINT* parms, BITINT* bits, int n);
void AliasUses(BITINT* bits, IMODE* im, bool rhs);
void AliasPass1(void);
void AliasPass2(void);

                             /* Iblock.cpp */

void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right); /* PROTOTYPE */
int equalimode(IMODE* ap1, IMODE* ap2);
short dhash(UBYTE* str, int len);
QUAD* LookupNVHash(UBYTE* key, int size, DAGLIST** table);
DAGLIST* ReplaceHash(QUAD* rv, UBYTE* key, int size, DAGLIST** table);
void add_intermed(QUAD* newQuad);
IMODE* liveout2(QUAD* q);
QUAD* liveout(QUAD* node);
int ToQuadConst(IMODE** im);
bool usesAddress(IMODE* im);
void flush_dag(void);
void dag_rundown(void);
BLOCKLIST* newBlock(void);
void addblock(int val);
void gen_label(int labno);
QUAD* gen_icode_with_conflict(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, bool conflicting);
QUAD* gen_icode(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
void gen_iiconst(IMODE* res, long long val);
void gen_ifconst(IMODE* res, FPF val);
void gen_igoto(enum i_ops op, long label);
void gen_data(int val);
void gen_icgoto(enum i_ops op, long label, IMODE* left, IMODE* right);
QUAD* gen_igosub(enum i_ops op, IMODE* left);
void gen_icode2(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, int label);
void gen_line(LINEDATA* data);
void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
void RemoveFromUses(QUAD* ins, int tnum);
void InsertUses(QUAD* ins, int tnum);
void RemoveInstruction(QUAD* ins);
void InsertInstruction(QUAD* before, QUAD* ins);

                             /* Iconfl.cpp */

void conflictini(void);
void resetConflict(void);
int findPartition(int T0);
void insertConflict(int i, int j);
void JoinConflictLists(int T0, int T1);
bool isConflicting(int T0, int T1);
void CalculateConflictGraph(BRIGGS_SET* nodes, bool optimize);

                             /* Iconst.cpp */

IMODE* intconst(long long val); /* PROTOTYPE */
void badconst(void) ;/* PROTOTYPE */
long long calmask(int i);
QUAD* ReCast(int size, QUAD* in, QUAD* newMode);
void ConstantFold(QUAD* d, bool reflow);
void ConstantFlow(void);

                           /* Ifloatconv.cpp */

unsigned long long CastToInt(int size, long long value);
FPF* IntToFloat(FPF* temp, int size, long long value);
FPF CastToFloat(int size, FPF* value);
FPF dorefloat(SimpleExpression* node);

                              /* Iflow.cpp */

void flow_init(void);
void dump_flowgraph(void);
void WalkFlowgraph(BLOCK* b, int (*func)(enum e_fgtype type, BLOCK* parent, BLOCK* b), int fwd);
void reflowConditional(BLOCK* src, BLOCK* dst);
bool dominatedby(BLOCK* src, BLOCK* ancestor);
enum e_fgtype getEdgeType(int first, int second);
void UnlinkCritical(BLOCK* s);
void RemoveCriticalThunks(void);
void unlinkBlock(BLOCK* succ, BLOCK* pred);
void doms_only(bool always);
void flows_and_doms(void);

                             /* Iinvar.cpp */

void ScanForInvariants(BLOCK* b);
void MoveLoopInvariants(void);

                              /* Ilazy.cpp */

void SetunMoveableTerms(void);
void RearrangePrecolors(void);
void SetGlobalTerms(void);
void GlobalOptimization(void);

                              /* Ilive.cpp */

QUAD* beforeJmp(QUAD* I, bool before);
void removeDead(BLOCK* b);
void liveVariables(void);

                             /* Ilocal.cpp */

SimpleExpression* anonymousVar(enum e_scc_type storage_class, SimpleType* tp);
IMODE* InitTempOpt(int size1, int size2);
void insertDefines(QUAD* head, BLOCK* b, int tnum);
void insertUses(QUAD* head, int dest);
void definesInfo(void);
void usesInfo(void);
void gatherLocalInfo(std::vector<SimpleSymbol*>& vars);

                              /* Iloop.cpp */

void CancelInfinite(int orgBlockCount);
void RemoveInfiniteThunks(void);
void BuildLoopTree(void);
bool isAncestor(LOOP* l1, LOOP* l2);
LIST* strongRegiondfs(int tnum);
void CalculateInduction(void);

                            /* Ioptutil.cpp */

void BitInit(void);
BRIGGS_SET* briggsAlloc(int size);
BRIGGS_SET* briggsAlloct(int size);
BRIGGS_SET* briggsAllocc(int size);
BRIGGS_SET* briggsAllocs(int size);
BRIGGS_SET* briggsReAlloc(BRIGGS_SET* set, int size);
int briggsSet(BRIGGS_SET* p, int index);
int briggsReset(BRIGGS_SET* p, int index);
int briggsTest(BRIGGS_SET* p, int index);
int briggsUnion(BRIGGS_SET* s1, BRIGGS_SET* s2);
int briggsIntersect(BRIGGS_SET* s1, BRIGGS_SET* s2);
#if 0
BITARRAY* allocbit(int size);
BITARRAY* tallocbit(int size);
BITARRAY* sallocbit(int size);
BITARRAY* aallocbit(int size);
BITARRAY* callocbit(int size);
bool isset(BITARRAY* arr, int bit);
void setbit(BITARRAY* arr, int bit);
void clearbit(BITARRAY* arr, int bit);
void bitarrayClear(BITARRAY* arr, int count);
#endif

                              /* Iout.cpp */

void putamode(QUAD* q, IMODE* ap); /* PROTOTYPE */
void nl(void); /* PROTOTYPE */
void xstringseg(void); /* PROTOTYPE */
void outcodeini(void);
void beDecorateSymName(char* buf, SimpleSymbol* sym);
void putconst(SimpleExpression* offset, int color);
void putlen(int l);
void putamode(QUAD* q, IMODE* ap);
void put_code(QUAD* q);
int beVariableLive(IMODE* m);
void rewrite_icode(void);
void gen_vtt(int offset, SimpleSymbol* func, SimpleSymbol* name);
void gen_importThunk(SimpleSymbol* func);
void gen_vc1(SimpleSymbol* func);
void gen_strlab(SimpleSymbol* sym);
void gen_funcref(SimpleSymbol* sym);
void put_label(int lab);
void put_string_label(int lab, int type);
void put_staticlabel(long label);/* PROTOTYPE */
void genfloat(FPF* val);
void gendouble(FPF* val);
void genlongdouble(FPF* val);
void genbyte(long val);
void genbool(int val);
void genbit(SimpleSymbol* sym, int val);
void genshort(long val);
void genwchar_t(long val);
void genlong(long val);
void genlonglong(long long val);
void genint(int val);
void genuint16(int val);
void genuint32(int val);
void genenum(int val);
//int genstring(STRING* str);
void genaddress(unsigned long long address);
void gensrref(SimpleSymbol* sym, int val, int type);
void genref(SimpleSymbol* sym, int offset);
void genpcref(SimpleSymbol* sym, int offset);
void genstorage(int nbytes);
void gen_labref(int n);
void gen_labdifref(int n1, int n2);
int wchart_cmp(LCHAR* left, LCHAR* right, int len);
#ifdef ISPARSER
//EXPRESSION* stringlit(STRING* s);
#endif
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
void gen_virtual(SimpleSymbol* sym, int data);
void gen_endvirtual(SimpleSymbol* sym);
void align(int size);
void asm_header(const char* name, const char* version);
void asm_trailer(void);
void localdef(SimpleSymbol* sym);
void localstaticdef(SimpleSymbol* sym);
void globaldef(SimpleSymbol* sym);
int put_exfunc(SimpleSymbol* sym, int notyet);
void put_expfunc(SimpleSymbol* sym);
void putexterns(void);

                              /* Ipeep.cpp */

void peepini(void);
void weed_goto(void);
void kill_labeledgoto(BLOCK* b, QUAD* head);
void kill_jumpover(BLOCK* b, QUAD* head);
void peep_icode(bool branches);

                               /* Irc.cpp */

void LoadSaturationBounds(void);
void regInit(void);
void alloc_init(void); /* PROTOTYPE */
void cacheTempSymbol(SimpleSymbol* sym);
void AllocateStackSpace();
void FillInPrologue(QUAD* head, SimpleSymbol* funcsp);
int SqueezeChange(int temp, int t, int delta);
void SqueezeInit(void);
void Precolor(void);
void retemp(void);
void AllocateRegisters(QUAD* head);

                            /* Ireshape.cpp */

void DumpInvariants(void);
bool variantThisLoop(BLOCK* b, int tnum);
bool matchesop(enum i_ops one, enum i_ops two);
void ReplaceHashReshape(QUAD* rv, UBYTE* key, int size, DAGLIST** table);
void unmarkPreSSA(QUAD* ins);
void RewriteInnerExpressions(void);
void Reshape(void);

                            /* Irewrite.cpp */

void Prealloc(int pass);
void CalculateBackendLives(void);

                              /* Issa.cpp */

void SSAInit(void); /* PROTOTYPE */
DAGLIST** getSavedDAG(void);
void releaseSavedDAG(DAGLIST** dag);
void TranslateToSSA(void);
void TranslateFromSSA(bool all);

                             /* Istren.cpp */

void ReplaceOneUses(QUAD* head, IMODE** im);
void ReplaceStrengthUses(QUAD* head);
void ReduceStrengthAssign(QUAD* head);
void ReduceLoopStrength(void);

                             /* Memory.cpp */

void mem_summary(void);
void* memAlloc(MEMBLK** arena, int size);
void memFree(MEMBLK** arena, int* peak);
void* globalAlloc(int size); /* PROTOTYPE */
void globalFree(void);
void* localAlloc(int size); /* PROTOTYPE */
void localFree(void); /* PROTOTYPE */
void* Alloc(int size);
void* oAlloc(int size); /* PROTOTYPE */
void oFree(void); /* PROTOTYPE */
void* aAlloc(int size); /* PROTOTYPE */
void aFree(void); /* PROTOTYPE */
void* tAlloc(int size); /* PROTOTYPE */
void tFree(void); /* PROTOTYPE */
void* cAlloc(int size); /* PROTOTYPE */
void cFree(void); /* PROTOTYPE */
void* sAlloc(int size); /* PROTOTYPE */
void sFree(void); /* PROTOTYPE */
char* litlate(const char* name);
LCHAR* wlitlate(const LCHAR* name);

                             /* Output.cpp */

void oinit(void);
void oflush(FILE* file);
void oputc(int ch, FILE* file);
void beputc(int ch); /* PROTOTYPE */
void owrite(const char* buf, size_t size, int n, FILE* fil);
void beWrite(const char* buf, size_t size); /* PROTOTYPE */
void oprintf(FILE* file, const char* format, ...);
void bePrintf(const char* format, ...);
void beRewind(void);  /* PROTOTYPE */

                            /* Symfuncs.cpp */

bool comparetypes(SimpleType* typ1, SimpleType* typ2, int exact);
bool equalnode(SimpleExpression* left, SimpleExpression *right);
SimpleExpression* GetSymRef(SimpleExpression* n);

                              /* Utils.cpp */

void diag(const char *fmt, ...);
int sizeFromISZ(int isz);
int alignFromISZ(int isz);
int needsAtomicLockFromISZ(int isz);
SimpleSymbol* varsp(SimpleExpression* node);
IMODE* make_immed(int size, long long i);
IMODE* make_fimmed(int size, FPF f);
IMODE* make_parmadj(long i);
SimpleExpression* tempenode(void);
IMODE* tempreg(int size, int mode);
IMODE* GetStoreTemp(IMODE* dest);
IMODE* GetLoadTemp(IMODE* dest);
IMODE* LookupStoreTemp(IMODE* dest, IMODE* src);
IMODE* LookupLoadTemp(IMODE* dest, IMODE* source);
IMODE* LookupImmedTemp(IMODE* dest, IMODE* source);/* PROTOTYPE */
IMODE* LookupCastTemp(IMODE* im, int size);
int pwrof2(long long i);
long long mod_mask(int i);

inline long long imax(long long x, long long y) { return x > y ? x : y; }
inline long long imin(long long x, long long y) { return x < y ? x : y; }
void my_sprintf(char* dest, const char* fmt, ...);
IMODE* indnode(IMODE* ap1, int size);
SimpleExpression *simpleExpressionNode(enum se_type type, SimpleExpression*left, SimpleExpression* right);
SimpleExpression *simpleIntNode(enum se_type type, unsigned long long i);
