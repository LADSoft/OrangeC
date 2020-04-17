
/* Protogen Version 3.0.1.6Saturday October 12, 2019  02:39:02 */
void putstring(char* value, SimpleExpression* exp);
#define l_Ustr 16
#define l_ustr 15
#define l_wstr 14
#define l_astr 13
void generate_instructions(QUAD* head);
Type* FindType(const char* name, bool toErr);
int msil_main_preprocess(char* fileName);
void msil_main_postprocess(bool errors);
void msil_end_generation(char* fileName);
void Import();


                             /* Config.cpp */

                               /* Gen.cpp */

void include_start(char* name, int num);
Instruction* gen_code(Instruction::iop op, Operand* operand);
SimpleExpression *simpleExpressionNode(enum se_type type, SimpleExpression*left, SimpleExpression* right);
SimpleExpression *simpleIntNode(enum se_type type, unsigned long long i);
void msil_oa_gen_label(int labno);
Operand* make_constant(int sz, SimpleExpression* exp);
bool isauto(SimpleExpression* ep);
void msil_oa_gen_vtt(int dataOffset, SimpleSymbol* func);
void msil_oa_gen_vc1(SimpleSymbol* func);
void msil_oa_gen_importThunk(SimpleSymbol* func);
Operand* getCallOperand(QUAD* q, bool& virt);
Operand* getOperand(IMODE* oper);
void load_ind(IMODE* im);
void store_ind(IMODE* im);
void load_arithmetic_constant(int sz, Operand* operand);
void load_constant(int sz, SimpleExpression* exp);
void gen_load(IMODE* im, Operand* dest, bool retval);
void gen_store(IMODE* im, Operand* dest);
void gen_convert(Operand* dest, IMODE* im, int sz);
void gen_branch(Instruction::iop op, int label, bool decrement);
void asm_expressiontag(QUAD* q);
void asm_tag(QUAD* q);
void asm_line(QUAD* q) ;
void asm_blockstart(QUAD* q);
void asm_blockend(QUAD* q);
void asm_varstart(QUAD* q);
void asm_func(QUAD* q);
void asm_passthrough(QUAD* q);
void asm_datapassthrough(QUAD* q);
void asm_label(QUAD* q); /* PROTOTYPE */
void asm_goto(QUAD* q) ;
BoxedType* boxedType(int isz);
void unbox(int val);
void asm_parm(QUAD* q) ;
void asm_parmblock(QUAD* q) ;
void asm_parmadj(QUAD* q) ;
void asm_gosub(QUAD* q) ;
void asm_fargosub(QUAD* q);
void asm_trap(QUAD* q);
void asm_int(QUAD* q);
void asm_ret(QUAD* q); /* PROTOTYPE */
void asm_fret(QUAD* q);
void asm_rett(QUAD* q);
void asm_add(QUAD* q) ;
void asm_sub(QUAD* q) ;
void asm_udiv(QUAD* q) ;
void asm_umod(QUAD* q) ;
void asm_sdiv(QUAD* q) ;
void asm_smod(QUAD* q) ;
void asm_muluh(QUAD* q);
void asm_mulsh(QUAD* q);
void asm_mul(QUAD* q) ;
void asm_lsr(QUAD* q) ;
void asm_lsl(QUAD* q) ;
void asm_asr(QUAD* q) ;
void asm_neg(QUAD* q); /* PROTOTYPE */
void asm_not(QUAD* q); /* PROTOTYPE */
void asm_and(QUAD* q) ;
void asm_or(QUAD* q) ;
void asm_eor(QUAD* q) ;
void asm_setne(QUAD* q) ;
void asm_sete(QUAD* q) ;
void asm_setc(QUAD* q) ;
void asm_seta(QUAD* q) ;
void asm_setnc(QUAD* q) ;
void asm_setbe(QUAD* q) ;
void asm_setl(QUAD* q) ;
void asm_setg(QUAD* q) ;
void asm_setle(QUAD* q) ;
void asm_setge(QUAD* q) ;
void asm_assn(QUAD* q) ;
void asm_genword(QUAD* q);
void compactgen(Instruction* i, int lab);
void bingen(int lower, int avg, int higher);
void asm_coswitch(QUAD* q) ;
void asm_swbranch(QUAD* q) ;
void asm_dc(QUAD* q);
void asm_assnblock(QUAD* q) ;
void asm_clrblock(QUAD* q) ;
void asm_cmpblock(QUAD* q);
void asm_jc(QUAD* q)  ;
void asm_ja(QUAD* q)  ;
void asm_je(QUAD* q) ;
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
void asm_pushcontext(QUAD* q)  ;
void asm_popcontext(QUAD* q)  ;
void asm_loadcontext(QUAD* q)  ;
void asm_unloadcontext(QUAD* q)  ;
void asm_tryblock(QUAD* q)  ;
void asm_seh(QUAD* q) ;
void asm_stackalloc(QUAD* q)  ;
void asm_loadstack(QUAD* q)  ;
void asm_savestack(QUAD* q)  ;
void asm_functail(QUAD* q, int begin, int size)  ;
void asm_atomic(QUAD* q) ;
QUAD* leftInsertionPos(QUAD* head, IMODE* im);
int examine_icode(QUAD* head);

                             /* Invoke.cpp */

int InsertExternalFile(const char* name, bool);
void InsertOutputFileName(const char* name); /* PROTOTYPE */
void GetOutputFileName(char* name, char* path, bool obj);
void NextOutputFileName(void);
int RunExternalFiles();

                             /* Process.cpp */

void parse_pragma(const char* kw, const char* tag);
MethodSignature* LookupArrayMethod(Type* tp, std::string name);
bool qualifiedStruct(SimpleSymbol* sp);
bool IsPointedStruct(SimpleType* tp);
Field* GetField(SimpleSymbol* sp);
MethodSignature* GetMethodSignature(SimpleType* tp, bool pinvoke);
MethodSignature* FindPInvokeWithVarargs(std::string name, std::list<Param*>::iterator vbegin, std::list<Param*>::iterator vend,
                                        size_t size);
MethodSignature* GetMethodSignature(SimpleSymbol* sp);
std::string GetArrayName(SimpleType* tp);
Value* GetStructField(SimpleSymbol* sp);
void msil_create_property(SimpleSymbol* property, SimpleSymbol* getter, SimpleSymbol* setter);
Type* GetType(SimpleType* tp, bool commit, bool funcarg=false, bool pinvoke=false);
void msil_oa_enter_type(SimpleSymbol* sp);
void CacheExtern(SimpleSymbol* sp);
void CacheGlobal(SimpleSymbol* sp);
void CacheStatic(SimpleSymbol* sp);
Value* GetParamData(std::string name);
Value* GetLocalData(SimpleSymbol* sp);
Value* GetFieldData(SimpleSymbol* sp);
void LoadLocals(std::vector<SimpleSymbol*>& functionVariable);
void LoadParams(SimpleSymbol* funcsp, std::vector<SimpleSymbol*>& functionVariables, std::map<SimpleSymbol*, Param*, byName>& paramList);
void compile_start(char* name);
void LoadFuncs(void);
void msil_flush_peep(SimpleSymbol* funcsp, QUAD* list);
void CreateFunction(MethodSignature* sig, SimpleSymbol* sp);
void ReplaceName(std::map<std::string, Value*>& list, Value* v, char* name);
void msil_oa_gensrref(SimpleSymbol* sp, int val, int type);
int msil_oa_main_preprocess(void);
void msil_oa_main_postprocess(bool errors);
void msil_oa_end_generation(void);
void msil_oa_put_extern(SimpleSymbol* sp, int code);
void msil_oa_gen_strlab(SimpleSymbol* sp);
Type* GetStringType(int type);
Value* GetStringFieldData(int lab, int type);
void msil_oa_put_string_label(int lab, int type);
void msil_oa_genbyte(int bt);
void msil_oa_genstring(char* str, int len);
void msil_oa_enterseg(e_sg segnum);

                              /* Using.cpp */

void _using_init(void) ;
bool _using_(const char* file);
void _add_global_using(const char* str);
void _apply_global_using(void);
bool msil_managed(SimpleSymbol* sp);
