
/* Protogen Version 3.0.1.6Friday October 11, 2019  16:44:47 */
void asm_expressiontag(QUAD* q);
void generate_instructions(QUAD* intermed_head);
bool InsertOption(const char* name);
void asm_seh(QUAD* q);
void asm_tag(QUAD* q);
void SelectBackendData();

                            /* Beintrins.cpp */

void promoteToBoolean(AMODE* addr);
bool handleBSR(void);
bool handleBSF(void);
bool handleINB(void);
bool handleINW(void);
bool handleIND(void);
bool handleOUTB(void);
bool handleOUTW(void);
bool handleOUTD(void);
bool handleROTL8(void);
bool handleROTL16(void);
bool handleROTR8(void);
bool handleROTR16(void);
bool handleROTL(void);
bool handleROTR(void);
bool handleCTZ(void);
bool handleCLZ(void);
bool handleBSWAP16(void);
bool handleBSWAP32(void);
bool handleBSWAP64(void);
bool BackendIntrinsic(QUAD* q);

                             /* Config.cpp */

enum e_lk get_dll_linkage(void); /* PROTOTYPE */
void WinmodeSetup(const char select, const char* string);
void SetUsesESP(bool yes);
void adjustUsesESP(void);

                            /* Dbgtypes.cpp */

                               /* Gen.cpp */

bool BackendIntrinsic(QUAD* q); /* PROTOTYPE */
SimpleExpression* copy_expression(SimpleExpression* node);
AMODE* make_muldivval(AMODE* ap);
AMODE* make_label(int lab);
AMODE* makesegreg(int seg);
AMODE* makeSSE(int reg);
void make_floatconst(AMODE* ap);
AMODE* moveFP(AMODE* apa, int sza, AMODE* apl, int szl);
AMODE* aimmed(unsigned long long i);
AMODE* aimmedt(long i, int size);
bool isauto(SimpleExpression* ep);
AMODE* make_offset(SimpleExpression* node);
AMODE* make_stack(int number);
AMODE* setSymbol(const char* name);
void oa_gen_vtt(int dataOffset, SimpleSymbol* func);
void oa_gen_vc1(SimpleSymbol* func);
void oa_gen_importThunk(SimpleSymbol* func);
void make_complexconst(AMODE* ap, AMODE* api);
void floatchs(AMODE* ap, int sz);
void zerocleanup(void);
AMODE* floatzero(AMODE* ap);
bool sameTemp(QUAD* head); /* PROTOTYPE */
int beRegFromTempInd(QUAD* q, IMODE* im, int which);
int beRegFromTemp(QUAD* q, IMODE* im); /* PROTOTYPE */
bool sameTemp(QUAD* head);
int imaginary_offset(int sz);
int samereg(AMODE* ap1, AMODE* ap2);
void getAmodes(QUAD* q, enum e_opcode* op, IMODE* im, AMODE** apl, AMODE** aph);
void bit_store(AMODE* dest, AMODE* src, int size, int bits, int startbit);
void liveQualify(AMODE* reg, AMODE* left, AMODE* right);
bool overlaps(AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh);
void func_axdx(enum e_opcode func, AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh);
void func_axdx_mov(enum e_opcode op, AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh, AMODE* aprl, AMODE* aprh);
void gen_lshift(enum e_opcode op, AMODE* aph, AMODE* apl, AMODE* n);
void gen_xset(QUAD* q, enum e_opcode pos, enum e_opcode neg, enum e_opcode flt);
void gen_goto(QUAD* q, enum e_opcode pos, enum e_opcode neg, enum e_opcode llpos, enum e_opcode llneg, enum e_opcode llintermpos,
              enum e_opcode llintermneg, enum e_opcode flt);
void bingen(int lower, int avg, int higher);
int getPushMask(int i);
void asm_line(QUAD* q) ;
void asm_blockstart(QUAD* q) ;
void asm_blockend(QUAD* q) ;
void asm_varstart(QUAD* q) ;
void asm_func(QUAD* q) ;
void asm_passthrough(QUAD* q) ;
void asm_datapassthrough(QUAD* q); /* PROTOTYPE */
void asm_label(QUAD* q) ;
void asm_goto(QUAD* q) ;
void asm_parm(QUAD* q) ;
void asm_parmblock(QUAD* q) ;
void asm_parmadj(QUAD* q) ;
void asm_gosub(QUAD* q) ;
void asm_fargosub(QUAD* q) ;
void asm_trap(QUAD* q) ;
void asm_int(QUAD* q) ;
void asm_ret(QUAD* q) ;
void asm_fret(QUAD* q) ;
void asm_rett(QUAD* q) ;
void asm_add(QUAD* q) ;
void asm_sub(QUAD* q) ;
void asm_umod(QUAD* q) ;
void asm_sdiv(QUAD* q) ;
void asm_smod(QUAD* q) ;
void asm_muluh(QUAD* q);
void asm_mulsh(QUAD* q);
void asm_udiv(QUAD* q); 
void asm_mul(QUAD* q) ;
void asm_lsr(QUAD* q) ;
void asm_lsl(QUAD* q) ;
void asm_asr(QUAD* q) ;
void asm_neg(QUAD* q) ;
void asm_not(QUAD* q) ;
void asm_and(QUAD* q) ;
void asm_or(QUAD* q) ;
void asm_eor(QUAD* q) ;
void asm_setne(QUAD* q); 
void asm_sete(QUAD* q)  ;
void asm_setc(QUAD* q)  ;
void asm_seta(QUAD* q)  ;
void asm_setnc(QUAD* q) ;
void asm_setbe(QUAD* q) ;
void asm_setl(QUAD* q)  ;
void asm_setg(QUAD* q)  ;
void asm_setle(QUAD* q) ;
void asm_setge(QUAD* q) ;
void asm_assn(QUAD* q) ;
void asm_genword(QUAD* q) ;
void asm_coswitch(QUAD* q) ;
void asm_swbranch(QUAD* q) ;
void asm_dc(QUAD* q);  /* PROTOTYPE */
void asm_assnblock(QUAD* q) ;
void asm_clrblock(QUAD* q) ;
void asm_cmpblock(QUAD* q);
void asm_jc(QUAD* q);  
void asm_ja(QUAD* q) ; 
void asm_je(QUAD* q)  ;
void asm_jnc(QUAD* q) ;
void asm_jbe(QUAD* q) ;
void asm_jne(QUAD* q) ;
void asm_jl(QUAD* q)  ;
void asm_jg(QUAD* q)  ;
void asm_jle(QUAD* q) ;
void asm_jge(QUAD* q) ;
void asm_cppini(QUAD* q); /* PROTOTYPE */
void asm_prologue(QUAD* q) ;
void asm_epilogue(QUAD* q) ;
void asm_pushcontext(QUAD* q) ;
void asm_popcontext(QUAD* q) ;
void asm_loadcontext(QUAD* q) ;
void asm_unloadcontext(QUAD* q) ;
void asm_tryblock(QUAD* q) ;
void asm_stackalloc(QUAD* q) ;
void asm_loadstack(QUAD* q) ;
void asm_savestack(QUAD* q) ;
void asm_functail(QUAD* q, int begin, int size) ;
void asm_atomic(QUAD* q);

                             /* Invoke.cpp */

int InsertExternalFile(const char* name, bool primary);
void WinmodeSetup(const char select, const char* str); /* PROTOTYPE */
void InsertOutputFileName(const char* name);
int RunExternalFiles();

                              /* Iout.cpp */

void putamode(QUAD* q, IMODE* ap); /* PROTOTYPE */
void nl(void); /* PROTOTYPE */
void xstringseg(void); /* PROTOTYPE */
void outcodeini(void);
void putconst(SimpleExpression* offset, int color);
void putlen(int l);
void putamode(QUAD* q, IMODE* ap);
void put_code(QUAD* q);
int beVariableLive(IMODE* m);
void rewrite_icode(void);
void gen_vtt(unsigned dataOffset, SimpleSymbol* func, SimpleSymbol* name);
void gen_importThunk(SimpleSymbol* func);
void gen_vc1(SimpleSymbol* func);
void gen_strlab(SimpleSymbol* sym);
void put_label(int lab);
void put_string_label(int lab, int type);
void put_staticlabel(long label);
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
void genaddress(unsigned long long address);
void gensrref(SimpleSymbol* sym, int val, int type);
void genref(SimpleSymbol* sym, int offset);
void genpcref(SimpleSymbol* sym, int offset);
void genstorage(int nbytes);
void gen_labref(int n);
void gen_labdifref(int n1, int n2);
int wchart_cmp(LCHAR* left, LCHAR* right, int len);
//EXPRESSION* stringlit(STRING* s);
void dumpLits(void);
void nl(void);
void exitseg(void);
void enterseg(enum e_sg seg);
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

                             /* Outasm.cpp */

void oa_ini(void);
void oa_nl(void);
void outop(const char* name);
void putop(enum e_opcode op, AMODE* aps, AMODE* apd, int nooptx);
void oa_putconst(int op, int sz, SimpleExpression* offset, bool doSign);
void oa_putlen(int l);
void putsizedreg(const char* string, int reg, int size);
void pointersize(int size);
void putseg(int seg, int usecolon);
int islabeled(SimpleExpression* n);
void oa_putamode(int op, int szalt, AMODE* ap);
void oa_put_code(OCODE* cd);
void oa_gen_strlab(SimpleSymbol* sym);
void oa_put_label(int lab);
void oa_put_string_label(int lab, int type);
void oa_genfloat(enum e_gt type, FPF* val);
void oa_genstring(char* str, int len);
void oa_genint(enum e_gt type, long long val);
void oa_genaddress(unsigned long long val);
void oa_gensrref(SimpleSymbol* sym, int val, int type);
void oa_genref(SimpleSymbol* sym, int offset);
void oa_genpcref(SimpleSymbol* sym, int offset);
void oa_genstorage(int nbytes);
void oa_gen_labref(int n);
void oa_gen_labdifref(int n1, int n2);
void oa_exitseg(enum e_sg seg);
void oa_enterseg(enum e_sg seg);
void oa_gen_virtual(SimpleSymbol* sym, int data);
void oa_gen_endvirtual(SimpleSymbol* sym);
void oa_align(int size);
void oa_setalign(int code, int data, int bss, int constant);
long queue_muldivval(long number);
long queue_large_const(unsigned constant[], int count);
long queue_floatval(FPF* number, int size);
void dump_muldivval(void);
void dump_browsedata(BROWSEINFO* bri);
void dump_browsefile(BROWSEFILE* brf);
void oa_header(const char* filename, const char* compiler_version);
void oa_trailer(void);
void oa_localdef(SimpleSymbol* sym);
void oa_localstaticdef(SimpleSymbol* sym);
void oa_globaldef(SimpleSymbol* sym);
void oa_output_alias(char* name, char* alias);
void oa_put_extern(SimpleSymbol* sym, int code);
void oa_put_impfunc(SimpleSymbol* sym, const char* file);
void oa_put_expfunc(SimpleSymbol* sym);
void oa_output_includelib(const char* name);
void oa_end_generation(void);/* PROTOTYPE */

                             /* Outcode.cpp */

extern void adjustUsesESP(void); /* PROTOTYPE */
void omfInit(void);
void dbginit(void);
void debug_outputtypedef(SimpleSymbol* sym)					;
void outcode_file_init(void);
void outcode_func_init(void);
void omf_globaldef(SimpleSymbol* sym);						
void omf_put_extern(SimpleSymbol* sym, int code);
void omf_put_impfunc(SimpleSymbol* sym, const char* file)	;
void omf_put_expfunc(SimpleSymbol* sym)						;
void omf_put_includelib(const char* name)					;
//Label* LookupLabel(const std::string& string);
void Release(void);
//ObjSection* LookupSection(std::string& string);
//void HandleDebugInfo(ObjFactory& factory, Section* sect, Instruction* ins);
//ObjFile* MakeFile(ObjFactory& factory, std::string& name);
//ObjFile* MakeBrowseFile(ObjFactory& factory, std::string name);
void output_obj_file(void);
void compile_start(char* name);
void include_start(char* name, int num);
void outcode_enterseg(int seg);
void InsertInstruction(Instruction* ins);
void outcode_gen_strlab(SimpleSymbol* sym);
void InsertLabel(int lbl);
void emit(void* data, int len);
//void emit(void* data, int len, Fixup* fixup, int fixofs);
//void emit(Label* label);
//void emit(int size);
//Fixup* gen_symbol_fixup(SimpleSymbol* pub, int offset, bool PC);
//Fixup* gen_label_fixup(int lab, int offset, bool PC);
//Fixup* gen_threadlocal_fixup(SimpleSymbol* tls, SimpleSymbol* base, int offset);
//Fixup* gen_diff_fixup(int lab1, int lab2);
void outcode_dump_muldivval(void);
void outcode_genref(SimpleSymbol* sym, int offset);
void outcode_gen_labref(int n);
void outcode_gen_labdifref(int n1, int n2);
void outcode_gensrref(SimpleSymbol* sym, int val);
void outcode_genstorage(int len); /* PROTOTYPE */
void outcode_genfloat(FPF* val);
void outcode_gendouble(FPF* val);
void outcode_genlongdouble(FPF* val);
void outcode_genstring(char* string, int len);
void outcode_genbyte(int val);
void outcode_genword(int val);
void outcode_genlong(int val); /* PROTOTYPE */
void outcode_genlonglong(long long val); /* PROTOTYPE */
void outcode_align(int size); /* PROTOTYPE */
void outcode_put_label(int lab);
void outcode_start_virtual_seg(SimpleSymbol* sym, int data);
void outcode_end_virtual_seg(SimpleSymbol* sym);
int resolveoffset(SimpleExpression* n, int* resolved);
AsmExprNode* MakeFixup(SimpleExpression* offset);
void InsertAttrib(ATTRIBDATA* ad);
void InsertLine(LINEDATA* linedata);
void InsertVarStart(SimpleSymbol* sym);
void InsertFunc(SimpleSymbol* sym, int start);
void InsertBlock(int start);
void AddFixup(Instruction* newIns, OCODE* ins, const std::list<Numeric*>& operands);
void outcode_diag(OCODE* ins, const char* str);
void outcode_AssembleIns(OCODE* ins);
void outcode_gen(OCODE* peeplist);

                              /* Peep.cpp */

void insert_peep_entry(OCODE* after, enum e_opcode opcode, int size, AMODE* ap1, AMODE* ap2); /* PROTOTYPE */
void o_peepini(void) ; /* PROTOTYPE */
void oa_adjust_codelab(void* select, int offset);
AMODE* makedregSZ(int r, char size);
AMODE* makedreg(int r);
AMODE* makefreg(int r);
AMODE* copy_addr(AMODE* ap);
OCODE* gen_code(int op, AMODE* ap1, AMODE* ap2);
OCODE* gen_code_sse(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2);
OCODE* gen_code_sse_imm(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2, AMODE *ap3);
OCODE* gen_code3(int op, AMODE* ap1, AMODE* ap2, AMODE* ap3);
OCODE* gen_codes(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_coden(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_codefs(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_codef(int op, AMODE* ap1, AMODE* ap2);
void gen_codes2(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_code2(int op, int len1, int len2, AMODE* ap1, AMODE* ap2);
void gen_codelab(SimpleSymbol* lab);
void gen_branch(int op, int label);
void gen_comment(char* txt);
void add_peep(OCODE* newitem);
void oa_gen_label(int labno);
void flush_peep(SimpleSymbol* funcsp, QUAD* list);
void peep_add(OCODE* ip);
void peep_sub(OCODE* ip);
OCODE* peep_test(OCODE* ip);
OCODE* peep_neg(OCODE* ip);
void peep_cmpzx(OCODE* ip);
void peep_cmp(OCODE* ip);
void peep_mov(OCODE* ip);
void peep_movzx(OCODE* ip);
void peep_movzx2(OCODE* ip);
void peep_tworeg(OCODE* ip);
void peep_mathdirect(OCODE* ip);
void peep_xor(OCODE* ip);
void peep_or(OCODE* ip);
void peep_and(OCODE* ip);
void peep_lea(OCODE* ip);
void peep_btr(OCODE* ip);
void peep_sar(OCODE* ip);
void peep_mul(OCODE* ip);
int novalue(OCODE* ip); /* PROTOTYPE */
int equal_address(AMODE* ap1, AMODE* ap2);
void peep_prefixes(OCODE* ip);
OCODE* peep_div(OCODE* ip);
void peep_push(OCODE* ip);
void peep_call(OCODE* ip);
void peep_fmovs(OCODE* ip);
void peep_fld(OCODE* ip);
void insert_peep_entry(OCODE* after, enum e_opcode opcode, int size, AMODE* ap1, AMODE* ap2);
void remove_peep_entry(OCODE* ip);
void oa_peep(void);

                             /* Rewrite.cpp */

SimpleExpression *simpleExpressionNode(enum se_type type, SimpleExpression*left, SimpleExpression* right);
SimpleExpression *simpleIntNode(enum se_type type, unsigned long long i);
void insert_parm(QUAD* head, QUAD* q);
void insert_nullparmadj(QUAD* head, int v);
void precolor(QUAD* head) ;
void ProcessOneInd(SimpleExpression* match, SimpleExpression** ofs1, SimpleExpression** ofs2, SimpleExpression** ofs3, int* scale);
void ProcessInd(SimpleExpression** ofs1, SimpleExpression** ofs2, SimpleExpression** ofs3, int* scale);
int preRegAlloc(QUAD* ins, BRIGGS_SET* globalVars, BRIGGS_SET* eobGlobals, int pass);
int examine_icode(QUAD* head);
void cg_internal_conflict(QUAD* head);

