enum e_opt { OPT_DEAD=1, OPT_DELETECOMMON=2, OPT_LCSE=4, OPT_FOLDCONST=8,
             OPT_GCSE = 16, OPT_COPY = 32, OPT_LIVE = 64 } ;
typedef struct {
    enum e_opt opts;
    int  prm_maxerr;
    char prm_quieterrors;
    char prm_warning;
    char prm_extwarning;
    char prm_diag;
    char prm_ansi;
    char prm_cmangle;
    char prm_c99;
    char prm_cplusplus;
    char prm_xcept;
    char prm_icdfile;
    char prm_asmfile;
    char prm_compileonly;
    char prm_debug;
    char prm_listfile;
    char prm_cppfile;
    char prm_errfile;
    char prm_browse;
    char prm_trigraph;
    char prm_oldfor;
    char prm_stackcheck;
    char prm_profiler;
    char prm_mergestrings;
    char prm_revbits;
    char prm_lines;
    char prm_bss;
    char prm_intrinsic;
    char prm_smartframes;
    char prm_farkeyword;
    char prm_linkreg;
} COMPILER_PARAMS;
typedef struct {
    char a_bool;
    char a_char;
    char a_short;
    char a_wchar_t;
    char a_enum;
    char a_int;
    char a_long;
    char a_longlong;
    char a_addr;
    char a_farptr;
    char a_farseg;
    char a_memberptr;
    char a_float;
    char a_double;
    char a_longdouble;
    char a_fcomplexpad;
    char a_rcomplexpad;
    char a_lrcomplexpad;
    char a_struct;
} ARCH_SIZING ;

typedef struct {
    char *name ;
    char min_width ;
    char max_width ;
    enum { ar_data = 1, ar_addr = 2, ar_volatile = 4, ar_float = 8, ar_saveduringfunction = 16 } flags;
} ARCH_REG ;

enum e_pm {
    ap_datareg=1, ap_addrreg=2, ap_volatile=4, ap_const0=8, ap_const1=16
} ;
typedef struct {
    char unused ;
} ARCH_PEEP;
typedef struct {
    ARCH_SIZING *type_align;
    int (*align)(int size);
    ARCH_SIZING *type_sizes;
    int (*param_offs)(int size);
    ARCH_REG *register_defs;
    ARCH_PEEP *peephole_defs;
    int bits_per_mau;
    int preferopts;
    int denyopts;
#define ABM_USESIZE    0
#define ABM_GLOBALBITS 1
#define ABM_LOCALBITS  2
    char boolmode;
#define ARM_NONE     0
#define ARM_FUNCTIONSCOPE 1
    char regallocmode ;
    char reversedefaultbitallocation;
    char globallocals ;
    char spgrowsup ;
    char assignmath ;
    char dynamiclocals ;
    char endianness[8] ;
    char retblocksize ;
    char parmwidth;
    char stackwidth;
    char stackalign;
} ARCH_CHARACTERISTICS;

typedef struct _arch_dbg {
    char *name;
    void *userdata;
    int  *blocknum;
    void (*init)(void);
    void (*rundown)(void);
    void (*startenum)(SYM *sp);
    void (*endenum)(void);
    void (*enumvalue)(SYM *sp);
    void (*outputtypedef)(SYM *sp);
    void (*outputtype)(TYP *head);
    void (*outfunc)(SYM *sp);
    void (*beginblock)(TABLE lsyms);
    void (*endblock)(TABLE syms);
    void (*outdata)(SYM *sp, int BSS);
    void (*dumpglobaltypedefs)(void) ;
    void (*init_browsedata)(char *name);
    void (*dump_browsedata)(char *s, int len);
    void (*rundown_browsedata)(void);
} ARCH_DEBUG ;
typedef struct _arch_defines {
    char *define;
    char *value;
    char respect;
    char permanent;
} ARCH_DEFINES;

typedef int (*ASMFUNC)(QUAD *q);

typedef struct {
    char c ;
    int asstring;
} ASM_ARGS ;

typedef struct _arch_gen {
    ASMFUNC asm_line;
    ASMFUNC asm_passthrough;
    ASMFUNC asm_label; 
    ASMFUNC asm_goto; 
    ASMFUNC asm_gosub; 
    ASMFUNC asm_fargosub;
    ASMFUNC asm_trap; 
    ASMFUNC asm_int;
    ASMFUNC asm_ret; 
    ASMFUNC asm_rett; 
    ASMFUNC asm_add; 
    ASMFUNC asm_sub; 
    ASMFUNC asm_udiv; 
    ASMFUNC asm_umod; 
    ASMFUNC asm_sdiv; 
    ASMFUNC asm_smod;
    ASMFUNC asm_umul; 
    ASMFUNC asm_smul; 
    ASMFUNC asm_lsl; 
    ASMFUNC asm_lsr; 
    ASMFUNC asm_asl; 
    ASMFUNC asm_asr; 
    ASMFUNC asm_neg; 
    ASMFUNC asm_not;
    ASMFUNC asm_and; 
    ASMFUNC asm_or; 
    ASMFUNC asm_eor;
    ASMFUNC asm_asadd;
    ASMFUNC asm_assub;
    ASMFUNC asm_asudiv; 
    ASMFUNC asm_asumod;
    ASMFUNC asm_assdiv;
    ASMFUNC asm_assmod;
    ASMFUNC asm_asumul;
    ASMFUNC asm_assmul;
    ASMFUNC asm_aslsl;
    ASMFUNC asm_aslsr;
    ASMFUNC asm_asasl;
    ASMFUNC asm_asasr;
    ASMFUNC asm_asand;
    ASMFUNC asm_asor;
    ASMFUNC asm_aseor;
    ASMFUNC asm_asuminus;
    ASMFUNC asm_ascompl;
    ASMFUNC asm_setne;
    ASMFUNC asm_sete;
    ASMFUNC asm_setc;
    ASMFUNC asm_seta;
    ASMFUNC asm_setnc;
    ASMFUNC asm_setbe;
    ASMFUNC asm_setl;
    ASMFUNC asm_setg;
    ASMFUNC asm_setle;
    ASMFUNC asm_setge;
    ASMFUNC asm_assn;
    ASMFUNC asm_genword;
    ASMFUNC asm_coswitch;
    ASMFUNC asm_swbranch;
    ASMFUNC asm_dc; 
    ASMFUNC asm_assnblock;
    ASMFUNC asm_clrblock;
    ASMFUNC asm_jc; 
    ASMFUNC asm_ja;
    ASMFUNC asm_je;
    ASMFUNC asm_jnc;
    ASMFUNC asm_jbe;
    ASMFUNC asm_jne;
    ASMFUNC asm_jl;
    ASMFUNC asm_jg;
    ASMFUNC asm_jle;
    ASMFUNC asm_jge;
    ASMFUNC asm_parm;
    ASMFUNC asm_parmadj;
    ASMFUNC asm_parmblock;
    ASMFUNC asm_array; 
    ASMFUNC asm_arrayindex;
    ASMFUNC asm_cppini; 
    ASMFUNC asm_block;
    ASMFUNC asm_livein;
    ASMFUNC asm_prologue; 
    ASMFUNC asm_epilogue;
    ASMFUNC asm_pushcontext;
    ASMFUNC asm_popcontext;
    ASMFUNC asm_loadcontext;
    ASMFUNC asm_substack;
    ASMFUNC asm_blockstart;
    ASMFUNC asm_blockend;
    void (*gen_strlab)(SYM *sp);
    void (*gen_label)(int labnum);
    void (*gen_bit) (LLONG_TYPE val);
    void (*gen_bool) (LLONG_TYPE val);
    void (*gen_char) (LLONG_TYPE val);
    void (*gen_short) (LLONG_TYPE val);
    void (*gen_enum) (LLONG_TYPE val);
    void (*gen_int) (LLONG_TYPE val);
    void (*gen_long) (LLONG_TYPE val);
    void (*gen_longlong) (LLONG_TYPE val);
    void (*gen_float)(float val);
    void (*gen_double)(double val);
    void (*gen_longdouble)(long double val);
    void (*gen_address) (char *string);
    void (*gen_ref) (char *name, int offset);
    void (*gen_pcref)(SYM *sp, int offset);
    void (*gen_srref) (char *name, LLONG_TYPE val);
    void (*gen_dataref) (SYM *sp, long offset);
    void (*gen_coderef) (SYM *sp, long offset);
    void (*gen_labref)(int labnum);
    void (*gen_labdifref)(int left, int right);
    void (*gen_virtual)(SYM *sp);
    void (*gen_endvirtual)(SYM *sp);
    void (*gen_storage)(int size);
    void (*align)(int size);
    void (*cseg)(void);
    void (*dseg)(void);
    void (*bssseg)(void);
    void (*constseg)(void);
    void (*stringseg)(void);
    void (*startupseg)(void);
    void (*rundownseg)(void);
    void (*cpp_startupseg)(void);
    void (*cpp_rundownseg)(void);
    void (*exitseg)(void);
    void (*gen_header)(char *filename, char *compiler_version);
    void (*gen_trailer)(void);
    void (*global_define)(SYM *sp);
    void (*extern_define)(SYM *sp, int code);
    void (*import_define)(SYM *sp, char *file);
    void (*export_define)(SYM *sp);
    void (*output_alias)(SYM *sp, char *alias);
    void (*output_includelib)(char *name);
    void (*output_obj_file)(void);
    void (*outcode_init)(void);
    void (*pre_gcse_rewrite)(QUAD *list);
    void (*post_gcse_rewrite)(QUAD *list);
} ARCH_GEN;

typedef struct _arch_asm {
    char *name;
    void *userdata;
    char *asmext;
    char *objext;   /* 0 = has no object mode */
    char *envname;
    char *progname;
    char *cfgname;
    char *usage_text;
    char **registers;
    ASMNAME *oplst;
    ASMREG *reglst;
    KEYWORDS *keywords;
    ARCH_DEFINES *defines ;
    ARCH_DEBUG *debug ;
    ARCH_CHARACTERISTICS *arch ;
    ARCH_GEN *gen;
    int (*init)(COMPILER_PARAMS *params, struct _arch_asm *data, ARCH_DEBUG *debug); /* return 1 to proceed */
    int (*parse_param)(int mode, char *string); /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    int (*parse_codegen)(int mode, char *string); /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    int (*parse_optimizer)(int mode, char *string); /* return 1 to eat a single char.  2 = eat rest of string.  0 = unknown */
    void (*enter_filename)(char *name);
    int (*insert_noncompile_file)(char *name);
    int (*compiler_postprocess)(void);
    int (*rundown)(void);
    void (*inlineAsmInit)(void);
    SNODE *(*inlineAsm)(int);
    void (*intrinsicInit)(void);
    void (*SearchIntrins)(SYM *sp);
    IMODE *(*handleIntrins)(ENODE *node, int novalue);
    
} ARCH_ASM ;

